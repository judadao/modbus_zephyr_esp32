# modbus_zephyr_esp32

Reusable Modbus adapter module for Linux development and Zephyr/ESP32 builds.

`modbus_zephyr_esp32` owns the Modbus frame boundary. It builds RTU/TCP
requests, validates responses, handles exception frames, and delegates actual
transport to a caller-provided callback. That keeps UART, TCP, Zephyr driver,
and Linux fake transport code outside the portable Modbus core.

## Overview

Use this repo when code needs Modbus framing/parsing but should choose its own
transport. The module provides the protocol-safe core; the caller provides UART,
TCP, Zephyr driver, or Linux fake transport behavior.

## Key Value

- Products need Modbus behavior without embedding transport-specific logic.
- Linux tests can validate framing and parsing before ESP32 hardware is ready.
- RTU and TCP modes share bounded buffer handling and response validation.
- Higher-level modules such as `dephy_iot` and `dephy_industrial_io` can reuse
  the same Modbus boundary.

## How To Use

1. Configure Modbus mode, unit ID, and timing in the caller.
2. Provide a transport callback that sends bytes and returns received bytes.
3. Call `modbus_zephyr_esp32_transfer()` with a PDU payload.
4. The module builds the wire frame, calls the transport, validates the reply,
   and returns the response PDU.

Example:

```c
struct modbus_zephyr_esp32_transport transport = {
    .transfer = my_transfer,
    .user = my_context,
};

modbus_zephyr_esp32_set_transport(&transport);
modbus_zephyr_esp32_transfer(3, payload, payload_len, &response);
```

## How It Works

The portable core never opens UART or sockets directly. It owns deterministic
frame construction and parsing, then calls the registered transport. For RTU it
adds and checks CRC16. For TCP it handles MBAP-style framing. Transport can be a
Linux fake, TCP socket, Zephyr UART driver, or future ESP32-specific adapter.

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

## Zephyr

Add this repository to `ZEPHYR_EXTRA_MODULES` and enable:

```conf
CONFIG_MODBUS_ZEPHYR_ESP32=y
```

## TODO

TODO state is tracked in `docs/todo.yaml` and summarized in `docs/todo.md`.
