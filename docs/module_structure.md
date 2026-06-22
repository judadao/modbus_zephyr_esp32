# modbus_zephyr_esp32 Module Structure

`modbus_zephyr_esp32` is a reusable Modbus adapter module for Linux validation
and Zephyr/ESP32 builds. The current implementation owns module shape,
configuration validation, and Zephyr metadata; RTU/TCP transport internals can
expand behind the same public API.

## Public API

- `include/modbus_zephyr_esp32/modbus.h`: configuration model, mode selection,
  lifecycle entry points, and error codes.

## Source Layout

- `src/modbus.c`: portable configuration validation and lifecycle state.
- `src/main.c`: standalone Zephyr sample entry point.
- `tests/unit_modbus.c`: Linux unit coverage for invalid arguments, RTU config,
  TCP config, stored config access, and start behavior.

## Build And Metadata

- `Makefile.linux`: Linux static library and unit test target.
- `CMakeLists.txt` and `Kconfig`: root Zephyr/module integration.
- `zephyr/CMakeLists.txt`, `zephyr/Kconfig`, and `zephyr/module.yml`: Zephyr
  module metadata consumed by product builds.
- `scripts/test_zephyr_module.sh`: metadata smoke test for required Zephyr
  files.

## Expansion Boundary

Keep protocol transport implementation inside this module. Higher-level modules
such as `dephy_industrial_io` should depend on public callbacks or public
transport APIs, not on product-specific Modbus code.
