/* MQTT Mutual Authentication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include <json_generator.h>
#include <json_parser.h>

static const char *TAG = "MQTTS_EXAMPLE";

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");

typedef struct {
    char buf[256];
    size_t offset;
} json_gen_test_result_t;

static json_gen_test_result_t result;

typedef struct{
    int id;
    char name[30];
    char sex[30];
    bool pass;
} data_user_t;

static data_user_t data_user;
int json_parse_data(char *json, data_user_t *out_data);

static void flush_str(char *buf, void *priv)
{
    json_gen_test_result_t *result = (json_gen_test_result_t *)priv;
    if (result) {
        if (strlen(buf) > sizeof(result->buf) - result->offset) {
            printf("Result Buffer too small\r\n");
            return;
        }
        memcpy(result->buf + result->offset, buf, strlen(buf));
        result->offset += strlen(buf);
    }
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/topic/deviot", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/deviot", "thuandeptrai", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            event->data[event->data_len] = 0;
            json_parse_data(event->data, &data_user);
            printf("%d - %s - %s - %d\n", data_user.id, data_user.name, data_user.sex, data_user.pass);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://test.mosquitto.org:8884",
        .event_handle = mqtt_event_handler,
        .client_cert_pem = (const char *)client_cert_pem_start,
        .client_key_pem = (const char *)client_key_pem_start,
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

static char* json_gen(json_gen_test_result_t *result, char *key1, int value1, \
                    char *key2, char *value2, char *key3, char *value3, \
                    char *key4, bool value4)
{
	char buf[20];
    memset(result, 0, sizeof(json_gen_test_result_t));
	json_gen_str_t jstr;
	json_gen_str_start(&jstr, buf, sizeof(buf), flush_str, result);
	json_gen_start_object(&jstr);
	json_gen_obj_set_int(&jstr, key1, value1);
	json_gen_obj_set_string(&jstr, key2, value2);
	json_gen_obj_set_string(&jstr, key3, value3);
	json_gen_obj_set_bool(&jstr, key4, value4);
	json_gen_end_object(&jstr);
	json_gen_str_end(&jstr);
    return result->buf;
}

int json_parse_data(char *json, data_user_t *out_data)
{
	jparse_ctx_t jctx;
	int ret = json_parse_start(&jctx, json, strlen(json));
	if (ret != OS_SUCCESS) {
		printf("Parser failed\n");
		return -1;
	}

	if (json_obj_get_int(&jctx, "id", &out_data->id) != OS_SUCCESS)
    {
		printf("Parser failed\n");
    }

	if (json_obj_get_string(&jctx, "name", out_data->name, 30) != OS_SUCCESS)
    {
		printf("Parser failed\n");
    }

	if (json_obj_get_string(&jctx, "sex", out_data->sex, 30) != OS_SUCCESS)
    {
		printf("Parser failed\n");
    }

	if (json_obj_get_bool(&jctx, "pass", &out_data->pass) != OS_SUCCESS)
    {
		printf("Parser failed\n");
    }

	json_parse_end(&jctx);
    return 0;
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    json_gen(&result, "id", 12, "name", "tom", "sex", "man", "pass", true);
    printf("%s\n", result.buf);
    json_parse_data(result.buf, &data_user);
    printf("%d - %s - %s - %d\n", data_user.id, data_user.name, data_user.sex, data_user.pass);

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
}
