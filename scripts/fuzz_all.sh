#!/usr/bin/env bash
# =============================================================================
# fuzz_all.sh — Full-lifecycle fuzzing script for ML-DSA
#
# Configures, builds, generates seed corpus, runs all 13 fuzzers in parallel,
# and produces a summary report.
#
# Usage:
#   ./scripts/fuzz_all.sh                      # 1-hour run (default)
#   FUZZ_TIME=120 ./scripts/fuzz_all.sh        # 2-minute quick run
#   FUZZ_JOBS=4 ./scripts/fuzz_all.sh          # Limit to 4 parallel fuzzers
#   FUZZ_FORK=2 ./scripts/fuzz_all.sh          # 2 fork workers per fuzzer
# =============================================================================
set -euo pipefail

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"
CORPUS_DIR="${CORPUS_DIR:-$PROJECT_ROOT/fuzz_corpus}"
REPORT_DIR="${REPORT_DIR:-$PROJECT_ROOT/fuzz_report}"
NPROC=$(nproc)
FUZZ_TIME="${FUZZ_TIME:-3600}"
FUZZ_JOBS="${FUZZ_JOBS:-4}"
DEFAULT_FORK=$(( (NPROC / FUZZ_JOBS) - 1 ))
FUZZ_FORK="${FUZZ_FORK:-$(( DEFAULT_FORK > 0 ? DEFAULT_FORK : 1 ))}"
CXX="${CXX:-clang++}"

# Color output helpers
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

info()  { echo -e "${CYAN}[INFO]${NC}  $*"; }
ok()    { echo -e "${GREEN}[  OK]${NC}  $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
fail()  { echo -e "${RED}[FAIL]${NC}  $*"; }
header(){ echo -e "\n${BOLD}━━━ $* ━━━${NC}"; }

# ---------------------------------------------------------------------------
# ML-DSA Sizes (from FIPS 204, Table 1 & 2)
# ---------------------------------------------------------------------------
declare -A FUZZER_MAX_LEN

# --- Component fuzzers ---
FUZZER_MAX_LEN[field_arithmetic]=8
FUZZER_MAX_LEN[poly_ntt]=1024             # 256 * 4
FUZZER_MAX_LEN[poly_sampling]=65          # 1B selector + max(32, 64)
FUZZER_MAX_LEN[poly_bit_packing]=641      # 1B selector + max(blens)

# --- ML-DSA-44 ---
FUZZER_MAX_LEN[ml_dsa_44_keygen]=32
FUZZER_MAX_LEN[ml_dsa_44_sign]=2657       # Mode A: 1(disc) + 32(rnd) + 2560(sk) + 64(mu)
FUZZER_MAX_LEN[ml_dsa_44_verify]=3797     # Mode A: 1(disc) + 1312(pk) + 64(mu) + 2420(sig)

# --- ML-DSA-65 ---
FUZZER_MAX_LEN[ml_dsa_65_keygen]=32
FUZZER_MAX_LEN[ml_dsa_65_sign]=4129       # Mode A: 1(disc) + 32(rnd) + 4032(sk) + 64(mu)
FUZZER_MAX_LEN[ml_dsa_65_verify]=5326     # Mode A: 1(disc) + 1952(pk) + 64(mu) + 3309(sig)

# --- ML-DSA-87 ---
FUZZER_MAX_LEN[ml_dsa_87_keygen]=32
FUZZER_MAX_LEN[ml_dsa_87_sign]=4993       # Mode A: 1(disc) + 32(rnd) + 4896(sk) + 64(mu)
FUZZER_MAX_LEN[ml_dsa_87_verify]=7348     # Mode A: 1(disc) + 2592(pk) + 64(mu) + 4627(sig)

ALL_FUZZERS=(
  field_arithmetic poly_ntt poly_sampling poly_bit_packing
  ml_dsa_44_keygen ml_dsa_44_sign ml_dsa_44_verify
  ml_dsa_65_keygen ml_dsa_65_sign ml_dsa_65_verify
  ml_dsa_87_keygen ml_dsa_87_sign ml_dsa_87_verify
)

phase_configure() {
  header "Phase 1/5: Configure"
  info "Configuring CMake with clang++ and fuzzer sanitizers..."
  cmake -B "$BUILD_DIR" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DCMAKE_BUILD_TYPE=Release \
    -DML_DSA_BUILD_FUZZERS=ON \
    "$PROJECT_ROOT"
  ok "CMake configured."
}

phase_build() {
  header "Phase 2/5: Build"
  info "Building all 13 fuzzer binaries..."
  cmake --build "$BUILD_DIR" -j"$(nproc)"
  ok "All fuzzers built."
}

generate_seeds_for_fuzzer() {
  local name="$1"
  local size="${FUZZER_MAX_LEN[$name]}"
  local corpus_path="$CORPUS_DIR/$name"

  mkdir -p "$corpus_path"

  for i in 1 2; do
    local seed_file="$corpus_path/seed_modeA_${i}"
    head -c "$size" /dev/urandom > "$seed_file"
    printf '\x01' | dd of="$seed_file" bs=1 count=1 conv=notrunc 2>/dev/null
  done

  for i in 1 2; do
    local seed_file="$corpus_path/seed_modeB_${i}"
    head -c "$size" /dev/urandom > "$seed_file"
    printf '\x00' | dd of="$seed_file" bs=1 count=1 conv=notrunc 2>/dev/null
  done

  dd if=/dev/zero of="$corpus_path/seed_zeros" bs=1 count="$size" 2>/dev/null
  dd if=/dev/zero bs=1 count="$size" 2>/dev/null | tr '\0' '\377' > "$corpus_path/seed_ones"
}

phase_seed_corpus() {
  header "Phase 3/5: Generate Seed Corpus"
  info "Creating per-fuzzer seed corpus in $CORPUS_DIR ..."

  for name in "${ALL_FUZZERS[@]}"; do
    generate_seeds_for_fuzzer "$name"
  done

  local total_seeds=0
  for name in "${ALL_FUZZERS[@]}"; do
    local count
    count=$(find "$CORPUS_DIR/$name" -type f | wc -l)
    total_seeds=$((total_seeds + count))
  done
  ok "Generated $total_seeds seed files across ${#ALL_FUZZERS[@]} corpus directories."
}

phase_fuzz() {
  header "Phase 4/5: Fuzz (${FUZZ_TIME}s per fuzzer, ${FUZZ_JOBS} parallel, ${FUZZ_FORK} fork workers)"

  mkdir -p "$REPORT_DIR"
  local pids=()
  local i=0
  for name in "${ALL_FUZZERS[@]}"; do
    local bin="$BUILD_DIR/${name}_fuzzer"
    local corpus_path="$CORPUS_DIR/$name"
    local max_len="${FUZZER_MAX_LEN[$name]}"
    local log_file="$REPORT_DIR/${name}.log"

    info "Launching: $name (max_len=$max_len)"

    "$bin" \
      "$corpus_path" \
      -max_total_time="$FUZZ_TIME" \
      -max_len="$max_len" \
      -fork="$FUZZ_FORK" \
      -print_final_stats=1 \
      -print_corpus_stats=1 \
      > "$log_file" 2>&1 &

    pids+=($!)
    i=$((i + 1))

    if [[ $(( i % FUZZ_JOBS )) -eq 0 ]]; then
      info "Waiting for current batch ($FUZZ_JOBS fuzzers) to finish..."
      for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
          fail "A fuzzer in the current batch failed (potential bug found!)."
          exit 1
        fi
      done
      pids=()
    fi
  done

  if [[ ${#pids[@]} -gt 0 ]]; then
    info "Waiting for remaining ${#pids[@]} fuzzer(s) to finish..."
    for pid in "${pids[@]}"; do
      wait "$pid" || true
    done
  fi
  ok "All fuzzers completed."
}

phase_report() {
  header "Phase 5/5: Fuzzing Report"

  local report_file="$REPORT_DIR/summary.txt"
  local timestamp
  timestamp=$(date -u '+%Y-%m-%d %H:%M:%S UTC')

  {
    echo "============================================================"
    echo "  ML-DSA Fuzzing Report"
    echo "  Generated: $timestamp"
    echo "  Duration:  ${FUZZ_TIME}s per fuzzer"
    echo "  Workers:   ${FUZZ_FORK} fork workers per fuzzer"
    echo "  Parallel:  ${FUZZ_JOBS} concurrent fuzzers"
    echo "============================================================"
    echo ""
    printf "%-30s %8s %8s %10s %8s %s\n" "Fuzzer" "MaxLen" "Corpus" "Execs" "Cov" "Status"
    printf "%-30s %8s %8s %10s %8s %s\n" "------------------------------" "--------" "--------" "----------" "--------" "------"

    for name in "${ALL_FUZZERS[@]}"; do
      local corpus_path="$CORPUS_DIR/$name"
      local log_file="$REPORT_DIR/${name}.log"
      local max_len="${FUZZER_MAX_LEN[$name]}"

      local corpus_count=0
      [[ -d "$corpus_path" ]] && corpus_count=$(find "$corpus_path" -type f | wc -l)

      local execs="N/A"
      local cov="N/A"
      if [[ -f "$log_file" ]]; then
        execs=$(grep -a 'BINGO TOTAL' "$log_file" | grep -oP 'RUNS:\s*\K[0-9]+' | tail -1) || true
        [[ -z "$execs" ]] && execs=$(grep -a '^#[0-9]' "$log_file" | tail -1 | grep -oP '^#\K[0-9]+') || true
        
        cov=$(grep -a 'BINGO TOTAL' "$log_file" | grep -oP 'COV:\s*\K[0-9]+' | tail -1) || true
        [[ -z "$cov" ]] && cov=$(grep -a 'cov:' "$log_file" | grep -oP 'cov:\s*\K[0-9]+' | tail -1) || true
      fi

      local status="OK"
      if [[ -f "$log_file" ]]; then
        if grep -qi "SUMMARY:.*ERROR\|SUMMARY:.*bug\|==.*==ERROR" "$log_file" 2>/dev/null; then
          status="FINDING"
        elif grep -qi "BINGO\|crash-\|leak-" "$log_file" 2>/dev/null; then
          status="CRASH"
        fi
      else
        status="NO LOG"
      fi

      printf "%-30s %8d %8d %10s %8s %s\n" "$name" "$max_len" "$corpus_count" "${execs:-N/A}" "${cov:-N/A}" "$status"
    done
    echo ""
    echo "Corpus directory : $CORPUS_DIR"
    echo "Log files        : $REPORT_DIR/*.log"
    echo "============================================================"
  } | tee "$report_file"

  ok "Report saved to: $report_file"
}

main() {
  header "ML-DSA Fuzzing Suite"
  info "Project root : $PROJECT_ROOT"
  info "Build dir    : $BUILD_DIR"
  info "Corpus dir   : $CORPUS_DIR"
  info "Report dir   : $REPORT_DIR"
  echo ""

  local start_time=$(date +%s)

  phase_configure
  phase_build
  phase_seed_corpus
  phase_fuzz
  phase_report

  local end_time=$(date +%s)
  local elapsed_min=$(( (end_time - start_time) / 60 ))
  ok "Total wall-clock time: ${elapsed_min} minutes."
}

main "$@"
