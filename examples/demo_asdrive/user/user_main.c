/* http_get_mbedtls - HTTPS version of the http_get example, using mbed TLS.
 *
 * Retrieves a JSON response from the howsmyssl.com API via HTTPS over TLS v1.2.
 *
 * Validates the server's certificate using the root CA loaded (in PEM format) in cert.c.
 *
 * Adapted from the ssl_client1 example in mbedtls.
 *
 * Original Copyright (C) 2006-2015, ARM Limited, All Rights Reserved, Apache 2.0 License.
 * Additions Copyright (C) 2015 Angus Gratton, Apache 2.0 License.
 */
#include "espressif/esp_common.h"

#include <string.h>

#ifdef ESP_OPEN_RTOS
#include "FreeRTOS.h"
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#include "asdrive.h"
#include "gpio.h"

xTaskHandle xHandle;

void user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);

    printf("SDK version:%s\n", system_get_sdk_version());
    SyslogDial("54.169.15.38",514);

    struct station_config config = {
        .ssid = "NETGEAR46",
        .password = "0973171371"
    };

    printf( "(%s:%d): \n", __FILE__ , __LINE__);
    printf("SSID:%s,PW:%s\n", config.ssid ,config.password);

    wifi_set_opmode(STATION_MODE);
    wifi_station_set_config(&config);

    PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);
    PIN_FUNC_SELECT(GPIO_PIN_REG_13, FUNC_GPIO13);
    PIN_FUNC_SELECT(GPIO_PIN_REG_5, FUNC_GPIO5);

    xTaskCreate(&http_get_task, (signed char *)"get_task", 1500 ,  NULL , 2, &xHandle );
}




#include "common_macros.h"
#define _GPIO_TO_IOMUX { 12, 5, 13, 4, 14, 15, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3 }
const IROM uint32_t GPIO_TO_IOMUX_MAP[] = _GPIO_TO_IOMUX;

#include <esp/hwrand.h>
#include <esp/wdev_regs.h>
#include <string.h>

/* Return a random 32-bit number */
uint32_t hwrand(void)
{
    return WDEV.HWRNG;
}

/* Fill a variable size buffer with data from the Hardware RNG */
void hwrand_fill(uint8_t *buf, size_t len)
{
    size_t i;
    for(i = 0; i < len; i+=4) {
        uint32_t random = WDEV.HWRNG;
        /* using memcpy here in case 'buf' is unaligned */
        memcpy(buf + i, &random, (i+4 <= len) ? 4 : (len % 4));
    }
}
