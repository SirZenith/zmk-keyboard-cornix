/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/sys_io.h>

static int pinmux_cornix_init(void) {
#if (CONFIG_BOARD_CORNIX)
  const struct device *p0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
#if CONFIG_ENABLE_CHARGER
  gpio_pin_configure(p0, 5, GPIO_OUTPUT);
  gpio_pin_set(p0, 5, 0);
#else
  gpio_pin_configure(p0, 5, GPIO_INPUT);
#endif
#endif
  return 0;
}

SYS_INIT(pinmux_cornix_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
