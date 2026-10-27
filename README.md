# Secure ROM and Peripheral Driver Framework for ARM-Based SoC

## 1. Project Overview

This project demonstrates a modular embedded software framework for memory-mapped peripheral access, hardware abstraction, driver validation, and RTL peripheral verification.

It combines C-based peripheral drivers and platform utilities with a custom Verilog peripheral exposing an AXI4-Lite register interface.

The project is intended as a practical portfolio demonstrating embedded C, peripheral-driver design, register-level programming, RTL design, and verification.

## 2. Architecture

The project has four principal layers:

1. **Application and tests:** Exercises the peripheral APIs and validates expected behavior.
2. **Hardware abstraction layer (HAL):** Provides reusable GPIO, UART, SPI, I2C, and custom peripheral interfaces.
3. **Platform utilities:** Provides register access, status types, timeout handling, ring buffers, and critical-section helpers.
4. **RTL and simulation:** Implements a custom register peripheral and verifies its AXI4-Lite interface using Verilog testbenches.

### High-Level Architecture

```text
Application / Test Programs
            |
            v
     Peripheral HAL
   GPIO / UART / SPI / I2C
   Custom AXI Secure Driver
            |
            v
      Platform Utilities
   Register / Timeout / Ring Buffer
            |
            v
      Memory-Mapped Interface
            |
            v
   AXI4-Lite Secure Peripheral
            |
            v
        GPIO Output
```

The host-side C tests use simulated memory-mapped registers. The diagram represents the intended software-to-hardware architecture; it does not imply that a physical processor-to-RTL connection has already been demonstrated.

## 3. Custom AXI4-Lite Peripheral

RTL source: `rtl/axi_secure_peripheral.v`

| Register | Offset | Access | Description |
|---|---:|---|---|
| CONTROL | `0x00` | Read/write | Control register; bit 0 is the enable bit |
| STATUS | `0x04` | Read-only | Reflects the CONTROL enable bit |
| OUTPUT | `0x08` | Read/write | GPIO output register |

The interface implements independent write-address and write-data handshakes, write responses, read-address handshakes, and read responses.

Byte-write strobes allow individual byte lanes to be updated. Unsupported addresses and writes to the read-only STATUS register return a decode error.

## 4. Software Driver

The custom driver is implemented in:

- `hal/include/axi_secure_regs.h`
- `hal/include/axi_secure_driver.h`
- `hal/src/axi_secure_driver.c`

The driver provides APIs for initialization, enabling and disabling the peripheral, writing GPIO output values, and reading OUTPUT and STATUS registers.

The low-level `periph` driver provides memory-mapped register access. The custom wrapper adds argument validation and status-code reporting.

## 5. Verification Strategy

### C tests

The CTest suite exercises:

- Concurrent UART behavior
- Ring-buffer operations
- Driver regression behavior
- Negative and recovery cases
- Platform utilities
- Peripheral HAL behavior
- Custom AXI secure driver behavior
- Custom driver negative tests

The latest reported CTest run passed all nine registered tests.

### RTL tests

Separate Verilog testbenches cover:

- Register reads and writes
- Partial writes using byte strobes
- Invalid-address responses and read-only register protection
- AXI response backpressure
- Reset behavior and transaction recovery

The Day 43, Day 44, and Day 45 testbenches were reported as passing.

## 6. Build and Test

Configure and build the C project:

```bash
cmake -S . -B build
cmake --build build
```

Run the C regression suite:

```bash
ctest --test-dir build --output-on-failure
```

Run the AXI functional testbench:

```bash
iverilog -g2012 -Wall \
  -o build_axi_secure_peripheral \
  rtl/axi_secure_peripheral.v \
  rtl/tb/tb_axi_secure_peripheral.v

vvp build_axi_secure_peripheral
```

Run the backpressure testbench:

```bash
iverilog -g2012 -Wall \
  -o build_axi_backpressure \
  rtl/axi_secure_peripheral.v \
  rtl/tb/tb_axi_backpressure.v

vvp build_axi_backpressure
```

Run the reset testbench:

```bash
iverilog -g2012 -Wall \
  -o build_axi_reset \
  rtl/axi_secure_peripheral.v \
  rtl/tb/tb_axi_reset.v

vvp build_axi_reset
```

## 7. Tools

- C and Embedded C
- Verilog
- CMake
- GCC
- Icarus Verilog
- VVP
- CTest

## 8. Current Limitations

- The custom C driver is currently validated using host-side mock registers.
- The host tests do not demonstrate physical PS-to-PL communication.
- AXI RTL verification is based on the implemented simulation testbenches; it is not a formal AXI compliance certification.
- Hardware integration, FPGA synthesis, timing analysis, and on-board validation should be documented separately when completed.

## 9. Future Improvements

- Integrate the peripheral into a Zynq processing system through an AXI interconnect.
- Validate register access from a bare-metal application.
- Add automated RTL simulation to the project build.
- Generate and review RTL coverage reports.
- Add waveform examples and a system-level integration diagram.

## 10. Project Objective

The objective is to demonstrate a reproducible, testable embedded peripheral framework that connects software-driver concepts with custom RTL design and verification.
