#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ws2812b.h"

static const char *TAG = "main";

#define EXAMPLE_CHASE_SPEED_MS (10)

void app_main(void)
{
    ws2812_init(23, 8);
    ws2812_turn_off_all();
    ws2812_set_rgb_to_all(0, 0, 255/10);
}
