#include "modbus_zephyr_esp32/modbus.h"

#include <string.h>
#include <stdio.h>

#define ASSERT(cond, msg)                                                     \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("FAIL: %s\n", msg);                                       \
            return 1;                                                         \
        }                                                                     \
        printf("PASS: %s\n", msg);                                           \
    } while (0)

struct fake_transport {
    unsigned char request[260];
    unsigned int request_len;
    unsigned char response[260];
    unsigned int response_len;
    int rc;
};

static int fake_transfer(const unsigned char *request,
                         unsigned int request_len,
                         unsigned char *response,
                         unsigned int response_capacity,
                         unsigned int *response_len,
                         void *user)
{
    struct fake_transport *fake = user;

    memcpy(fake->request, request, request_len);
    fake->request_len = request_len;
    if (fake->rc != MODBUS_ZEPHYR_ESP32_OK) {
        return fake->rc;
    }
    if (fake->response_len > response_capacity) {
        return MODBUS_ZEPHYR_ESP32_ERR_IO;
    }
    memcpy(response, fake->response, fake->response_len);
    *response_len = fake->response_len;
    return MODBUS_ZEPHYR_ESP32_OK;
}

static void set_rtu_response(struct fake_transport *fake,
                             unsigned char unit_id,
                             unsigned char function,
                             const unsigned char *payload,
                             unsigned int payload_len)
{
    unsigned short crc;

    fake->response[0] = unit_id;
    fake->response[1] = function;
    memcpy(&fake->response[2], payload, payload_len);
    fake->response_len = payload_len + 2u;
    crc = modbus_zephyr_esp32_crc16(fake->response, fake->response_len);
    fake->response[fake->response_len++] = (unsigned char)(crc & 0xffu);
    fake->response[fake->response_len++] = (unsigned char)(crc >> 8);
}

static void set_tcp_response(struct fake_transport *fake,
                             unsigned char unit_id,
                             unsigned char function,
                             const unsigned char *payload,
                             unsigned int payload_len)
{
    fake->response[0] = 0;
    fake->response[1] = 1;
    fake->response[2] = 0;
    fake->response[3] = 0;
    fake->response[4] = 0;
    fake->response[5] = (unsigned char)(payload_len + 2u);
    fake->response[6] = unit_id;
    fake->response[7] = function;
    memcpy(&fake->response[8], payload, payload_len);
    fake->response_len = payload_len + 8u;
}

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
    struct fake_transport fake = { 0 };
    struct modbus_zephyr_esp32_transport transport = {
        .transfer = fake_transfer,
        .user = &fake,
    };
    struct modbus_zephyr_esp32_response response;
    const unsigned char crc_req[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
    const unsigned char read_req[] = { 0x00, 0x00, 0x00, 0x02 };
    const unsigned char read_payload[] = { 0x02, 0x00, 0x2a };

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
    ASSERT(modbus_zephyr_esp32_crc16(crc_req, sizeof(crc_req)) == 0x0bc4,
           "crc16 matches known payload");
    ASSERT(modbus_zephyr_esp32_set_transport(&transport) == MODBUS_ZEPHYR_ESP32_OK,
           "transport is accepted");

    fake.rc = MODBUS_ZEPHYR_ESP32_OK;
    ASSERT(modbus_zephyr_esp32_init(&rtu) == MODBUS_ZEPHYR_ESP32_OK,
           "rtu config restored");
    set_rtu_response(&fake, 1, 3, read_payload, sizeof(read_payload));
    ASSERT(modbus_zephyr_esp32_transfer(3, read_req, sizeof(read_req), &response) ==
               MODBUS_ZEPHYR_ESP32_OK,
           "rtu transfer parses valid response");
    ASSERT(fake.request_len == 8 && fake.request[0] == 1 && fake.request[1] == 3,
           "rtu request includes unit and function");
    ASSERT(response.unit_id == 1 && response.function == 3 && response.payload_len == 3,
           "rtu response exposes header and payload");

    fake.response[fake.response_len - 1] ^= 0xffu;
    ASSERT(modbus_zephyr_esp32_transfer(3, read_req, sizeof(read_req), &response) ==
               MODBUS_ZEPHYR_ESP32_ERR_IO,
           "rtu transfer rejects bad crc");

    set_rtu_response(&fake, 1, 0x83, read_payload, 1);
    ASSERT(modbus_zephyr_esp32_transfer(3, read_req, sizeof(read_req), &response) ==
               MODBUS_ZEPHYR_ESP32_ERR_UNSUPPORTED,
           "rtu transfer reports exception responses");

    fake.rc = MODBUS_ZEPHYR_ESP32_ERR_TIMEOUT;
    ASSERT(modbus_zephyr_esp32_transfer(3, read_req, sizeof(read_req), &response) ==
               MODBUS_ZEPHYR_ESP32_ERR_TIMEOUT,
           "transport timeout is returned");

    /* cppcheck-suppress redundantAssignment ; fake_transfer() consumes fake.rc above. */
    fake.rc = MODBUS_ZEPHYR_ESP32_OK;
    ASSERT(modbus_zephyr_esp32_init(&tcp) == MODBUS_ZEPHYR_ESP32_OK,
           "tcp config restored");
    set_tcp_response(&fake, 2, 3, read_payload, sizeof(read_payload));
    ASSERT(modbus_zephyr_esp32_transfer(3, read_req, sizeof(read_req), &response) ==
               MODBUS_ZEPHYR_ESP32_OK,
           "tcp transfer parses valid response");
    ASSERT(fake.request_len == 12 && fake.request[2] == 0 && fake.request[3] == 0 &&
               fake.request[6] == 2 && fake.request[7] == 3,
           "tcp request includes mbap header");

    return 0;
}
