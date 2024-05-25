echo Erase Flash
idf.py -p COM6 erase_flash

echo Gen key
espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

echo burn key to flah
espefuse.py -p COM6 burn_key flash_encryption my_flash_encryption_key.bin

espefuse.py -p COM6 burn_efuse FLASH_CRYPT_CONFIG 0xf

echo enable flash encyption
espefuse.py -p COM6 burn_efuse FLASH_CNT

echo flash firmware
idf.py encypted-flash monitor
