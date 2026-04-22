zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)
zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)

zephyr_library_sources(led_control.c)
zephyr_library_sources(ws2812/ws2812_control.c)