#ifndef __ESP_LEDC_H
#define __ESP_LEDC_H
#include "driver/ledc.h"

void esp_ledc_create(ledc_timer_t timer_num, int gpio_num, ledc_channel_t channel);
void esp_ledc_dim_led(int gpio_num, int resol_duty, int delay);
#endif