# Repository Guidelines

## Structure

Public headers live in `include/modbus_zephyr_esp32/`, implementation lives in
`src/`, Zephyr metadata lives in `zephyr/`, and Linux tests live in `tests/`.

## Build

Use `make -f Makefile.linux test` for local validation. Zephyr consumers should
include this repository through `ZEPHYR_EXTRA_MODULES` and enable
`CONFIG_MODBUS_ZEPHYR_ESP32`.

## Style

Use C11, four-space indentation, snake_case functions and variables, and
uppercase config symbols.

