# modbus_zephyr_esp32

Reusable Modbus adapter module for Linux development and Zephyr/ESP32 builds.

## Overview

`modbus_zephyr_esp32` builds and validates Modbus RTU/TCP frames while leaving
transport to the caller. The same core can run with Linux fakes, sockets, Zephyr
UART, or future ESP32 adapters.

## Key Value

- Shared Modbus framing and response validation.
- Caller-provided transport callback.
- Linux tests before target hardware is ready.
- Reusable boundary for higher-level IoT and IO modules.

## How To Use

```c
modbus_zephyr_esp32_set_transport(&transport);
modbus_zephyr_esp32_transfer(3, payload, payload_len, &response);
```

```sh
make -f Makefile.linux test
scripts/test_zephyr_module.sh
```

## Simple Principle

The module owns protocol correctness. The caller owns byte transport.

## Docs

- `docs/todo.md`: current TODO summary.
