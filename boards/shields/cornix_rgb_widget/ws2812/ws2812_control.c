#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ws2812_control.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct led_state led_states[CONFIG_RGBLED_WIDGET_LED_COUNT] = {0};
static struct led_rgb led_colors[CONFIG_RGBLED_WIDGET_LED_COUNT] = {0};

static const struct device *ws2812_dev = DEVICE_DT_GET(DT_ALIAS(status_ws2812));

static void color_index_to_rgb(uint8_t color_idx, struct led_rgb *rgb) {
  static const struct led_rgb lut[] = {
      {0, 0, 0}, // 0: off
      {1, 0, 0}, // 1: red
      {0, 1, 0}, // 2: green
      {1, 1, 0}, // 3: yellow
      {0, 0, 1}, // 4: blue
      {1, 0, 1}, // 5: magenta
      {0, 1, 1}, // 6: cyan
      {1, 1, 1}, // 7: white
  };

  uint8_t brightness = CONFIG_RGBLED_WIDGET_BRIGHTNESS;
  struct led_rgb tmp = {0};

  if (color_idx < ARRAY_SIZE(lut)) {
    tmp.r = lut[color_idx].r * brightness;
    tmp.g = lut[color_idx].g * brightness;
    tmp.b = lut[color_idx].b * brightness;
  }
  *rgb = tmp;
}

static int ws2812_update_strip(void) {
  return led_strip_update_rgb(ws2812_dev, led_colors,
                              CONFIG_RGBLED_WIDGET_LED_COUNT);
}

static void ws2812_clear_strip(void) {
  for (int i = 0; i < CONFIG_RGBLED_WIDGET_LED_COUNT; i++) {
    led_colors[i] = (struct led_rgb){0, 0, 0};
    led_states[i].current_color = 0;
  }
  ws2812_update_strip();
}

void ws2812_strip_init(void) {
  if (!device_is_ready(ws2812_dev)) {
    LOG_ERR("WS2812 device not ready");
    return;
  }

  // Initialize all LEDs to off
  for (int i = 0; i < CONFIG_RGBLED_WIDGET_LED_COUNT; i++) {
    led_colors[i] = (struct led_rgb){0, 0, 0};
    led_states[i].current_color = 0;
    led_states[i].base_color = 0;
    led_states[i].status_type = 0;
    led_states[i].priority = PRIORITY_AMBIENT;
    led_states[i].is_shared = false;
    led_states[i].is_persistent = false;
    led_states[i].share_end_time = 0;
  }

  led_strip_update_rgb(ws2812_dev, led_colors, CONFIG_RGBLED_WIDGET_LED_COUNT);
  LOG_INF("WS2812 strip initialized with %d LEDs",
          CONFIG_RGBLED_WIDGET_LED_COUNT);
}

int ws2812_set_led(uint8_t led_index, uint8_t color_idx) {
  if (led_index >= CONFIG_RGBLED_WIDGET_LED_COUNT) {
    LOG_ERR("LED index %d out of range (max %d)", led_index,
            CONFIG_RGBLED_WIDGET_LED_COUNT - 1);
    return -EINVAL;
  }

  color_index_to_rgb(color_idx, &led_colors[led_index]);
  led_states[led_index].current_color = color_idx;

  return led_strip_update_rgb(ws2812_dev, led_colors,
                              CONFIG_RGBLED_WIDGET_LED_COUNT);
}

int ws2812_clear_led(uint8_t led_index) {
  if (led_index >= CONFIG_RGBLED_WIDGET_LED_COUNT) {
    return -EINVAL;
  }

  led_colors[led_index] = (struct led_rgb){0, 0, 0};
  led_states[led_index].current_color = 0;
  led_states[led_index].status_type = STATUS_CUSTOM;
  led_states[led_index].priority = PRIORITY_AMBIENT;
  led_states[led_index].is_shared = false;
  led_states[led_index].is_persistent = false;
  led_states[led_index].share_end_time = 0;

  return led_strip_update_rgb(ws2812_dev, led_colors,
                              CONFIG_RGBLED_WIDGET_LED_COUNT);
}

int ws2812_clear_all(void) {
  ws2812_clear_strip();
  // Reset all LED states
  for (int i = 0; i < CONFIG_RGBLED_WIDGET_LED_COUNT; i++) {
    led_states[i].status_type = STATUS_CUSTOM;
    led_states[i].priority = PRIORITY_AMBIENT;
    led_states[i].is_shared = false;
    led_states[i].is_persistent = false;
    led_states[i].share_end_time = 0;
  }
  return 0;
}
