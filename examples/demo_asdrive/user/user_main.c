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
#include "smartconfig.h"


#include <string.h>

#ifdef ESP_OPEN_RTOS
#include "FreeRTOS.h"
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#include "asdrive.h"
#include "gpio.h"
#include "config/config.h"

#define logprintf( fmt, args... ) \
    do {\
    static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = "(%s:%d): " fmt;  \
    printf( flash_str , __FILE__ , __LINE__,##args);\
    } while(0)


void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
    switch(status) {
        case SC_STATUS_WAIT:
            logprintf("SC_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:
            logprintf("SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            logprintf("SC_STATUS_GETTING_SSID_PSWD\n");
            sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                logprintf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                logprintf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
            logprintf("SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;

            wifi_station_set_config(sta_conf);
            wifi_station_disconnect();
            wifi_station_connect();

            Config_setSSID( sta_conf->ssid );
            Config_setPassword( sta_conf->password );

            Config_setDid( "" );
            Config_setDevicePrivateKey( "" );

            break;
        case SC_STATUS_LINK_OVER:
            logprintf("SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
                uint8 phone_ip[4] = {0};

                memcpy(phone_ip, (uint8*)pdata, 4);
                logprintf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
            }
            smartconfig_stop();

            break;
    }

}

void ICACHE_FLASH_ATTR
smartconfig_task(void *pvParameters)
{
    smartconfig_start(smartconfig_done);
    vTaskDelete(NULL);
}

void ICACHE_FLASH_ATTR
user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);

    logprintf("SDK:%s\n", system_get_sdk_version());

    PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);
    PIN_FUNC_SELECT(GPIO_PIN_REG_13, FUNC_GPIO13);
    PIN_FUNC_SELECT(GPIO_PIN_REG_5, FUNC_GPIO5);

    char *ssid = Config_getSSID();

    if ( strlen(ssid) && GPIO_INPUT_GET(5) == 1){

        char *pass = Config_getWifiPassword();

        struct station_config config;
        strcpy( config.ssid , ssid );
        strcpy( config.password , pass);

        printf("SSID:%s,PW:%s\n", config.ssid ,config.password);

        wifi_set_opmode(STATION_MODE);
        wifi_station_set_config(&config);

    }else
        xTaskCreate( smartconfig_task, "smartconfig_task", 256 ,0,2,0 );

    xTaskCreate( http_get_task, "get_task", 1500 ,0,0,0 );
    xTaskCreate( start_http_server , "serv", 1024 ,0,0,0 );
}



#if defined (__ESP8266__ESPRESSIF)
#else
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
#endif

