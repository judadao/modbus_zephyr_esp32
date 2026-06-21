#include "modbus_zephyr_esp32/modbus.h"

static struct modbus_zephyr_esp32_config active_config;
static bool configured;

static bool valid_unit_id(unsigned int unit_id)
{
    return unit_id >= 1u && unit_id <= 247u;
}

int modbus_zephyr_esp32_init(const struct modbus_zephyr_esp32_config *config)
{
    if (config == 0 || !valid_unit_id(config->unit_id)) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    if (config->mode == MODBUS_ZEPHYR_ESP32_MODE_RTU && config->baudrate == 0u) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    if (config->mode == MODBUS_ZEPHYR_ESP32_MODE_TCP && config->tcp_port == 0u) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    active_config = *config;
    configured = true;
    return MODBUS_ZEPHYR_ESP32_OK;
}

int modbus_zephyr_esp32_start(void)
{
#if defined(CONFIG_MODBUS_ZEPHYR_ESP32)
    if (!configured) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    return MODBUS_ZEPHYR_ESP32_OK;
#else
    return MODBUS_ZEPHYR_ESP32_ERR_NOT_ENABLED;
#endif
}

bool modbus_zephyr_esp32_is_configured(void)
{
    return configured;
}

const struct modbus_zephyr_esp32_config *modbus_zephyr_esp32_config_get(void)
{
    return configured ? &active_config : 0;
}

