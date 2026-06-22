#!/bin/sh
set -eu

root="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"

required="
CMakeLists.txt
Kconfig
zephyr/CMakeLists.txt
zephyr/Kconfig
zephyr/module.yml
"

for path in $required; do
    if [ ! -f "$root/$path" ]; then
        echo "missing $path" >&2
        exit 1
    fi
done

grep -q "name: modbus_zephyr_esp32" "$root/zephyr/module.yml"
grep -q "MODBUS_ZEPHYR_ESP32" "$root/zephyr/Kconfig"

echo "modbus_zephyr_esp32 Zephyr metadata OK"
