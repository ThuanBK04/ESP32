#include <stdio.h>
#include "esp_ledc.h"

void app_main(void)
{
    esp_ledc_create(LEDC_TIMER_0, LEDC_CHANNEL_0, 2);

    while (1) {
        esp_ledc_dim_led(LEDC_CHANNEL_0, 82, 10);
    }
}
