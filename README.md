# modbus_zephyr_esp32

Reusable Modbus adapter module for Linux development and Zephyr/ESP32 builds.

The module owns Modbus configuration validation, RTU/TCP request framing,
caller-provided transport callbacks, CRC validation, exception response
handling, and bounded fixed-buffer behavior.

## Layout

```text
include/modbus_zephyr_esp32/modbus.h  public API
src/modbus.c                          portable implementation
tests/unit_modbus.c                   Linux unit coverage
scripts/test_zephyr_module.sh         Zephyr metadata smoke
zephyr/                               Zephyr module metadata
Makefile.linux                        Linux build/test
```

## Commands

```sh
make -f Makefile.linux test
scripts/test_zephyr_module.sh
```

## Public Boundary

`modbus_zephyr_esp32_transfer()` builds RTU or TCP frames into a fixed internal
buffer and calls a user-provided transport callback:

```c
struct modbus_zephyr_esp32_transport transport = {
    .transfer = my_transfer,
    .user = my_context,
};

modbus_zephyr_esp32_set_transport(&transport);
modbus_zephyr_esp32_transfer(3, payload, payload_len, &response);
```

The transport can be UART, TCP socket, Zephyr driver code, or a Linux test fake.

## Zephyr

Add this repository to `ZEPHYR_EXTRA_MODULES` and enable:

```conf
CONFIG_MODBUS_ZEPHYR_ESP32=y
```

## TODO

TODO state is tracked in `docs/todo.yaml` and summarized in `docs/todo.md`.
