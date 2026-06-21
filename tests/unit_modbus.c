#include "modbus_zephyr_esp32/modbus.h"

#include <stdio.h>

#define ASSERT(cond, msg)                                                     \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("FAIL: %s\n", msg);                                       \
            return 1;                                                         \
        }                                                                     \
        printf("PASS: %s\n", msg);                                           \
    } while (0)

int main(void)
{
    struct modbus_zephyr_esp32_config rtu = {
        .mode = MODBUS_ZEPHYR_ESP32_MODE_RTU,
        .unit_id = 1,
        .baudrate = 115200,
        .tcp_port = 0,
    };
    struct modbus_zephyr_esp32_config tcp = {
        .mode = MODBUS_ZEPHYR_ESP32_MODE_TCP,
        .unit_id = 2,
        .baudrate = 0,
        .tcp_port = 502,
    };

    ASSERT(modbus_zephyr_esp32_init(0) == MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG,
           "init rejects null config");
    ASSERT(modbus_zephyr_esp32_init(&rtu) == MODBUS_ZEPHYR_ESP32_OK,
           "init accepts rtu config");
    ASSERT(modbus_zephyr_esp32_is_configured(), "module is configured");
    ASSERT(modbus_zephyr_esp32_config_get()->baudrate == 115200,
           "stored rtu config is readable");
    ASSERT(modbus_zephyr_esp32_init(&tcp) == MODBUS_ZEPHYR_ESP32_OK,
           "init accepts tcp config");
    ASSERT(modbus_zephyr_esp32_start() == MODBUS_ZEPHYR_ESP32_OK,
           "enabled module starts");

    return 0;
}

