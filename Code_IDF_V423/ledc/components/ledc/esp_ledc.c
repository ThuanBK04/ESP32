#include "esp_ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void esp_ledc_create(ledc_timer_t timer_num, int gpio_num, ledc_channel_t channel)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,           // timer mode
        .timer_num = timer_num,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
            .channel    = gpio_num,
            .duty       = 0,
            .gpio_num   = channel,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = timer_num
    };
    // Set LED Controller with previously prepared configuration
    ledc_channel_config(&ledc_channel);
}

void esp_ledc_dim_led(int gpio_num, int resol_duty, int delay)
{
    for (int duty_cycle = 0; duty_cycle < 8192; duty_cycle+=resol_duty)
    {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, gpio_num, duty_cycle);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, gpio_num);
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
    for (int duty_cycle = 0; duty_cycle < 8192; duty_cycle+=resol_duty)
    {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, gpio_num, 8192 - duty_cycle);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, gpio_num);
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}

