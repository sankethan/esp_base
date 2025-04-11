idf_component_register(SRCS "main.cpp""i2c_base.cpp"
                       PRIV_REQUIRES spi_flash esp_driver_i2c
                       INCLUDE_DIRS "")
