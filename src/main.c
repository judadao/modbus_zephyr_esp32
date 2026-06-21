#include "modbus_zephyr_esp32/modbus.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    struct modbus_zephyr_esp32_config config = {
        .mode = MODBUS_ZEPHYR_ESP32_MODE_RTU,
        .unit_id = CONFIG_MODBUS_ZEPHYR_ESP32_UNIT_ID,
        .baudrate = CONFIG_MODBUS_ZEPHYR_ESP32_BAUDRATE,
        .tcp_port = CONFIG_MODBUS_ZEPHYR_ESP32_TCP_PORT,
    };

    printk("modbus_zephyr_esp32 starting\n");
    if (modbus_zephyr_esp32_init(&config) != MODBUS_ZEPHYR_ESP32_OK) {
        printk("modbus_zephyr_esp32 config invalid\n");
        return 0;
    }

    (void)modbus_zephyr_esp32_start();
    return 0;
}

