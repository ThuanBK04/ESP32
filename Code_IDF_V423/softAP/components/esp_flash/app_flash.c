#include "esp_flash.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define NAME   "__nvs_user"
#define KEY1   "TYPE_CONFIG"
#define KEY2   "CLIENTID"

static nvs_handle my_handle;

void app_flash_set_type_config(int type)
{
    nvs_open(NAME, NVS_READWRITE, &my_handle);
    nvs_set_u8(my_handle, KEY1, type);
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

int app_flash_get_type_config(void)
{
    uint8_t value;
    nvs_open(NAME, NVS_READWRITE, &my_handle);
    nvs_get_u8(my_handle, KEY1, &value);
    nvs_close(my_handle);
    return value;
}

void app_flash_set_clientid(char* clientid)
{
    nvs_open(NAME, NVS_READWRITE, &my_handle);
    nvs_set_str(my_handle, KEY2, clientid);
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

char* app_flash_get_clientid(void)
{
    uint8_t required_size;
    nvs_open(NAME, NVS_READWRITE, &my_handle);
    nvs_get_str(my_handle, KEY2, NULL, &required_size);
    char* clientid = malloc(required_size);
    nvs_get_str(my_handle, KEY2, clientid, &required_size);
    nvs_close(my_handle);
    return clientid;
}
