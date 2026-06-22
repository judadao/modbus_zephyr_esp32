#include "modbus_zephyr_esp32/modbus.h"

#include <string.h>

#define MODBUS_FRAME_BUFFER_SIZE 260u
#define MODBUS_TCP_HEADER_SIZE 7u
#define MODBUS_RTU_MIN_RESPONSE_SIZE 5u

static struct modbus_zephyr_esp32_config active_config;
static struct modbus_zephyr_esp32_transport active_transport;
static unsigned char frame_buffer[MODBUS_FRAME_BUFFER_SIZE];
static bool configured;
static unsigned short transaction_id;

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

int modbus_zephyr_esp32_set_transport(const struct modbus_zephyr_esp32_transport *transport)
{
    if (transport == 0 || transport->transfer == 0) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    active_transport = *transport;
    return MODBUS_ZEPHYR_ESP32_OK;
}

unsigned short modbus_zephyr_esp32_crc16(const unsigned char *data,
                                         unsigned int len)
{
    unsigned short crc = 0xffffu;
    unsigned int i;
    unsigned int bit;

    if (data == 0 && len != 0u) {
        return 0u;
    }

    for (i = 0; i < len; ++i) {
        crc ^= data[i];
        for (bit = 0; bit < 8u; ++bit) {
            if ((crc & 1u) != 0u) {
                crc = (unsigned short)((crc >> 1) ^ 0xa001u);
            } else {
                crc = (unsigned short)(crc >> 1);
            }
        }
    }

    return crc;
}

static int parse_rtu_response(unsigned char *frame,
                              unsigned int len,
                              struct modbus_zephyr_esp32_response *response)
{
    unsigned short expected_crc;
    unsigned short actual_crc;

    if (len < MODBUS_RTU_MIN_RESPONSE_SIZE) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }

    expected_crc = modbus_zephyr_esp32_crc16(frame, len - 2u);
    actual_crc = (unsigned short)(frame[len - 2u] | ((unsigned short)frame[len - 1u] << 8));
    if (expected_crc != actual_crc) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }
    if (frame[0] != active_config.unit_id) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }
    if ((frame[1] & 0x80u) != 0u) {
        return MODBUS_ZEPHYR_ESP32_ERR_UNSUPPORTED;
    }

    response->unit_id = frame[0];
    response->function = frame[1];
    response->payload = &frame[2];
    response->payload_len = len - 4u;
    return MODBUS_ZEPHYR_ESP32_OK;
}

static int parse_tcp_response(unsigned char *frame,
                              unsigned int len,
                              struct modbus_zephyr_esp32_response *response)
{
    unsigned int pdu_len;

    if (len < MODBUS_TCP_HEADER_SIZE + 2u) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }
    if (frame[2] != 0u || frame[3] != 0u || frame[6] != active_config.unit_id) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }

    pdu_len = ((unsigned int)frame[4] << 8) | frame[5];
    if (pdu_len < 2u || pdu_len + 6u != len) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }
    if ((frame[7] & 0x80u) != 0u) {
        return MODBUS_ZEPHYR_ESP32_ERR_UNSUPPORTED;
    }

    response->unit_id = frame[6];
    response->function = frame[7];
    response->payload = &frame[8];
    response->payload_len = len - 8u;
    return MODBUS_ZEPHYR_ESP32_OK;
}

int modbus_zephyr_esp32_transfer(unsigned char function,
                                 const unsigned char *payload,
                                 unsigned int payload_len,
                                 struct modbus_zephyr_esp32_response *response)
{
    unsigned int request_len;
    unsigned int response_len = 0u;
    int rc;

    if (!configured || active_transport.transfer == 0 || response == 0 ||
        function == 0u || (payload == 0 && payload_len != 0u)) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    if (payload_len > MODBUS_FRAME_BUFFER_SIZE - 9u) {
        return MODBUS_ZEPHYR_ESP32_ERR_INVALID_ARG;
    }

    memset(response, 0, sizeof(*response));
    if (active_config.mode == MODBUS_ZEPHYR_ESP32_MODE_RTU) {
        frame_buffer[0] = (unsigned char)active_config.unit_id;
        frame_buffer[1] = function;
        if (payload_len != 0u) {
            memcpy(&frame_buffer[2], payload, payload_len);
        }
        request_len = payload_len + 2u;
        unsigned short crc = modbus_zephyr_esp32_crc16(frame_buffer, request_len);
        frame_buffer[request_len++] = (unsigned char)(crc & 0xffu);
        frame_buffer[request_len++] = (unsigned char)(crc >> 8);
    } else {
        transaction_id++;
        frame_buffer[0] = (unsigned char)(transaction_id >> 8);
        frame_buffer[1] = (unsigned char)(transaction_id & 0xffu);
        frame_buffer[2] = 0u;
        frame_buffer[3] = 0u;
        frame_buffer[4] = (unsigned char)((payload_len + 2u) >> 8);
        frame_buffer[5] = (unsigned char)((payload_len + 2u) & 0xffu);
        frame_buffer[6] = (unsigned char)active_config.unit_id;
        frame_buffer[7] = function;
        if (payload_len != 0u) {
            memcpy(&frame_buffer[8], payload, payload_len);
        }
        request_len = payload_len + 8u;
    }

    rc = active_transport.transfer(frame_buffer,
                                   request_len,
                                   frame_buffer,
                                   sizeof(frame_buffer),
                                   &response_len,
                                   active_transport.user);
    if (rc == MODBUS_ZEPHYR_ESP32_ERR_TIMEOUT) {
        return rc;
    }
    if (rc != MODBUS_ZEPHYR_ESP32_OK) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }

    if (active_config.mode == MODBUS_ZEPHYR_ESP32_MODE_RTU) {
        return parse_rtu_response(frame_buffer, response_len, response);
    }
    return parse_tcp_response(frame_buffer, response_len, response);
}
