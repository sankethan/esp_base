idf_component_register(SRCS "main.cpp" "i2c_base.cpp" "LDC1612.cpp" "storage.cpp" "usb_lib.c"
                       PRIV_REQUIRES spi_flash esp_driver_i2c usb esp_driver_gpio nvs_flash
                       INCLUDE_DIRS "")
