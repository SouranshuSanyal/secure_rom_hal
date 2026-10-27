
#!/usr/bin/env bash
set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

mkdir -p reports build/verification

LOG="reports/final_verification.log"
: > "$LOG"

run_logged() {
    local name="$1"
    shift

    echo
    echo "===== $name ====="
    echo "===== $name =====" >> "$LOG"

    "$@" 2>&1 | tee -a "$LOG"
}

echo "Secure ROM HAL - Final Verification"
echo "Project: $ROOT_DIR"
echo "Started: $(date)"
echo "Started: $(date)" >> "$LOG"

# Check required tools.
for tool in cmake ctest iverilog vvp; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "ERROR: Required tool '$tool' is not installed." | tee -a "$LOG"
        exit 1
    fi
done

# Build all C targets.
run_logged "CMake configure" \
    cmake -S . -B build

run_logged "C build" \
    cmake --build build

# Run the complete registered C test suite.
run_logged "C regression tests" \
    ctest --test-dir build --output-on-failure

# Compile and execute the AXI functional testbench.
run_logged "Compile AXI functional testbench" \
    iverilog -g2012 -o build/verification/axi_functional \
    rtl/axi_secure_peripheral.v \
    rtl/tb/tb_axi_secure_peripheral.v

run_logged "Run AXI functional testbench" \
    vvp build/verification/axi_functional

# Compile and execute the backpressure testbench.
run_logged "Compile AXI backpressure testbench" \
    iverilog -g2012 -o build/verification/axi_backpressure \
    rtl/axi_secure_peripheral.v \
    rtl/tb/tb_axi_backpressure.v

run_logged "Run AXI backpressure testbench" \
    vvp build/verification/axi_backpressure

# Compile and execute the reset testbench.
run_logged "Compile AXI reset testbench" \
    iverilog -g2012 -o build/verification/axi_reset \
    rtl/axi_secure_peripheral.v \
    rtl/tb/tb_axi_reset.v

run_logged "Run AXI reset testbench" \
    vvp build/verification/axi_reset

echo
echo "===== VERIFICATION COMMANDS COMPLETED ====="
echo "Log saved to: $LOG"
echo "Completed: $(date)" | tee -a "$LOG"
