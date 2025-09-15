#include "esp_timer.h"
#include "lookuptable.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <stdio.h>
#include <stdlib.h>

int regression_func(int adc_value) {
    float a = -0.1164;
    float b = 0.00001;
    float c = 140.2359;

    float adc_float = (float)adc_value;
    float humidity_float = c + (a * adc_float) + (b * adc_float * adc_float);

    if (humidity_float > 100.0) return 100;
    if (humidity_float < 0.0) return 0;
    return (int)humidity_float;
}

void app_main(void) {
    int result;

    // Configuramos ADC1, canal 4 (GPIO32)
    adc1_config_width(ADC_WIDTH_BIT_12); // 12 bits
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11); // GPIO32

    while (1) {
        // Leemos el ADC del pin 32
        int adc_val = adc1_get_raw(ADC1_CHANNEL_4);

        // Medimos tiempo de la función de regresión
        int64_t start = esp_timer_get_time();
        result = regression_func(adc_val);
        int64_t end = esp_timer_get_time();
        printf("ADC = %d | Resultado de Regresion = %d (Humedad), tiempo = %lld us\n", adc_val, result, (end - start));

        // Medimos tiempo de la tabla
        start = esp_timer_get_time();
        result = percentage_values[adc_val]; // usando la tabla
        end = esp_timer_get_time();
        printf("ADC = %d | Resultado de la Tabla = %d (Humedad), tiempo = %lld us\n", adc_val, result, (end - start));

        // Esperamos 1 segundo antes de la siguiente iteración
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
