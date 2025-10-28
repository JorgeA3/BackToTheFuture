/*
 * Adaptado para la Tarea HW6 - Conexión a Adafruit IO (Nube)
 * ¡VERSIÓN FINAL CON FORMATO JSON PARA EL MAPA!
 * Actualizado con los datos de JorgeA3.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h> // Para rand() y srand()
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include <assert.h>

// --- 1. CONFIGURACIÓN WI-FI (Tus datos) ---
#define WIFI_SSID       "INFINITUM49F4"
#define WIFI_PASS       "KKeYq3yHXK"

// --- 2. CONFIGURACIÓN ADAFRUIT IO (Tus datos) ---
#define MQTT_BROKER_URI "mqtt://io.adafruit.com:1883"
#define MQTT_USERNAME   "JorgeA3" 
#define MQTT_PASSWORD   "aio_guvF02CDDJ0NwJrJjqbuP1klI3Fa" // Tu AIO Key

// --- 3. TEMAS DE ADAFRUIT (Topics) ---
#define MQTT_TOPIC_TEMP "JorgeA3/feeds/u01.temperature"
#define MQTT_TOPIC_GPS  "JorgeA3/feeds/u01.gps"

static const char *TAG = "HW6_ADAFRUIT_JSON"; // Tag para los logs

// --- 4. DATOS GPS DE GUADALAJARA (como floats) ---
static const float guadalajara_lats[] = {
    20.6736, 20.6748, 20.6761, 20.6774, 20.6802, 20.6840
};
static const float guadalajara_lons[] = {
    -103.3440, -103.3525, -103.3612, -103.3689, -103.3755, -103.3801
};
static int gps_index = 0;
const int num_coords = 6;


// Tarea que publica los datos
static void mqtt_publish_task(void *pvParameters)
{
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;
    char temp_message[10];
    char gps_payload_buffer[100]; // Buffer más grande para el JSON
    
    srand(time(NULL));

    while (1)
    {
        // 1. Generar y enviar Temperatura
        int random_temp = (rand() % 31) + 10; // Temp. aleatoria entre 10 y 40
        snprintf(temp_message, sizeof(temp_message), "%d", random_temp);

        esp_mqtt_client_publish(client, MQTT_TOPIC_TEMP, temp_message, 0, 1, 0);
        ESP_LOGI(TAG, "Publicado en %s: %s", MQTT_TOPIC_TEMP, temp_message);
        
        vTaskDelay(pdMS_TO_TICKS(100));

        // 2. Obtener Coordenadas
        float lat = guadalajara_lats[gps_index];
        float lon = guadalajara_lons[gps_index];

        // 3. --- ¡CREAR EL PAYLOAD EN FORMATO JSON! ---
        // Formato: {"value": X, "lat": Y, "lon": Z}
        snprintf(gps_payload_buffer, sizeof(gps_payload_buffer), 
                 "{\"value\": %d, \"lat\": %f, \"lon\": %f}", 
                 random_temp, lat, lon);
        
        // 4. Publicar JSON al Mapa
        esp_mqtt_client_publish(client, MQTT_TOPIC_GPS, gps_payload_buffer, 0, 1, 0);
        ESP_LOGI(TAG, "Publicado en %s: %s", MQTT_TOPIC_GPS, gps_payload_buffer);

        // Avanzar al siguiente índice de GPS
        gps_index = (gps_index + 1) % num_coords;

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xTaskCreate(mqtt_publish_task, "mqtt_publish_task", 4096, client, 5, NULL);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("Received data on topic: %.*s\r\n", event->topic_len, event->topic);
            printf("Message: %.*s\r\n", event->data_len, event->data);
            break;
        default:
            ESP_LOGI(TAG, "Other MQTT event id:%d", event->event_id);
            break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = MQTT_BROKER_URI,
            },
        },
        .credentials = {
            .username = MQTT_USERNAME,
            .authentication = {
                .password = MQTT_PASSWORD,
            },
        },
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying Wi-Fi connection...");
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Wi-Fi connected!");
        mqtt_app_start();
    }
}

void wifi_init_sta(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    assert(netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

void app_main(void)
{
    nvs_flash_init();
    wifi_init_sta();
}

