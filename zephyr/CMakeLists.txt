if(CONFIG_MODBUS_ZEPHYR_ESP32)
  zephyr_library_named(modbus_zephyr_esp32)

  zephyr_library_sources(
    ../src/modbus.c
  )

  zephyr_library_include_directories(../include)
  zephyr_include_directories(../include)
endif()

