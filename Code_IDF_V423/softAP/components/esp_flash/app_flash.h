#ifndef __ESP_FLASH_H
#define __ESP_FLASH_H
void app_flash_set_type_config(int type);
int app_flash_get_type_config(void);
void app_flash_set_clientid(char* clientid);
char* app_flash_get_clientid(void);
#endif