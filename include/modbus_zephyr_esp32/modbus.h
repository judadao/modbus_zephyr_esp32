#ifndef MODBUS_ZEPHYR_ESP32_MODBUS_H
#define MODBUS_ZEPHYR_ESP32_MODBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

enum modbus_zephyr_esp32_mode {
    MODBUS_ZEPHYR_ESP32_MODE_RTU = 0,
    MODBUS_ZEPHYR_ESP32_MODE_TCP = 1,
};

enum modbus_zephyr_esp32_status {
    MODBUS_ZEPHYR_ESP32_OK = 0,
    MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG = -1,
    MODBUS_ZEPHYR_ESP32_ERR_NOT_ENABLED = -2,
    MODBUS_ZEPHYR_ESP32_ERR_UNSUPPORTED = -3,
    MODBUS_ZEPHYR_ESP32_ERR_TIMEOUT = -4,
    MODBUS_ZEPHYR_ESP32_ERR_IO = -5,
};

struct modbus_zephyr_esp32_config {
    enum modbus_zephyr_esp32_mode mode;
    unsigned int unit_id;
    unsigned int baudrate;
    unsigned short tcp_port;
};

typedef int (*modbus_zephyr_esp32_transfer_cb_t)(const unsigned char *request,
                                                 unsigned int request_len,
                                                 unsigned char *response,
                                                 unsigned int response_capacity,
                                                 unsigned int *response_len,
                                                 void *user);

struct modbus_zephyr_esp32_transport {
    modbus_zephyr_esp32_transfer_cb_t transfer;
    void *user;
};

struct modbus_zephyr_esp32_response {
    unsigned char unit_id;
    unsigned char function;
    const unsigned char *payload;
    unsigned int payload_len;
};

int modbus_zephyr_esp32_init(const struct modbus_zephyr_esp32_config *config);
int modbus_zephyr_esp32_start(void);
bool modbus_zephyr_esp32_is_configured(void);
const struct modbus_zephyr_esp32_config *modbus_zephyr_esp32_config_get(void);
int modbus_zephyr_esp32_set_transport(const struct modbus_zephyr_esp32_transport *transport);
unsigned short modbus_zephyr_esp32_crc16(const unsigned char *data,
                                         unsigned int len);
int modbus_zephyr_esp32_transfer(unsigned char function,
                                 const unsigned char *payload,
                                 unsigned int payload_len,
                                 struct modbus_zephyr_esp32_response *response);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_ZEPHYR_ESP32_MODBUS_H */
