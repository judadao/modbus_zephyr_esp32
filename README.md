# modbus_zephyr_esp32

Reusable Modbus adapter module for Linux development and Zephyr / ESP32 builds.

This repository is intentionally small at first. It provides the module shape,
public API, Linux smoke coverage, and Zephyr metadata needed by higher-level
repositories such as `dephy_iot`. Protocol implementation can be expanded behind
the same API without changing dependency layout.

## Layout

```text
modbus_zephyr_esp32/
├── include/modbus_zephyr_esp32/  # public headers
├── src/                          # portable implementation
├── zephyr/                       # Zephyr module metadata
├── tests/                        # Linux unit tests
└── Makefile.linux                # Linux build
```

## Linux

```bash
make -f Makefile.linux test
```

## Zephyr

Add this repository to `ZEPHYR_EXTRA_MODULES` and enable:

```text
CONFIG_MODBUS_ZEPHYR_ESP32=y
```

Current initial release tag:

```text
modbus-v0.1.0
```

