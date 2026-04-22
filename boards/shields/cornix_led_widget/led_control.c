#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

#include "ws2812/ws2812_control.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static bool is_usb_connected = false;
static uint8_t current_battery_capacity = 0;

static void update_led_state(void) {
  if (!is_usb_connected) {
    ws2812_clear_led(CONFIG_CHARGING_LED_INDEX);
  } else if (current_battery_capacity >= 100) {
    ws2812_clear_led(CONFIG_CHARGING_LED_INDEX);
  } else if (current_battery_capacity >= 80) {
    ws2812_set_led(CONFIG_CHARGING_LED_INDEX, COLOR_INDEX_WHITE);
  } else if (current_battery_capacity >= 60) {
    ws2812_set_led(CONFIG_CHARGING_LED_INDEX, COLOR_INDEX_GREEN);
  } else if (current_battery_capacity >= 40) {
    ws2812_set_led(CONFIG_CHARGING_LED_INDEX, COLOR_INDEX_BLUE);
  } else if (current_battery_capacity >= 20) {
    ws2812_set_led(CONFIG_CHARGING_LED_INDEX, COLOR_INDEX_MAGENTA);
  } else {
    ws2812_set_led(CONFIG_CHARGING_LED_INDEX, COLOR_INDEX_RED);
  }
}

static int usb_conn_listener(const struct zmk_event_t *eh) {
  struct zmk_usb_conn_state_changed *ev = as_zmk_usb_conn_state_changed(eh);
  if (ev == NULL) {
    return ZMK_EV_EVENT_BUBBLE;
  }

  is_usb_connected = ev->conn_state;

  update_led_state();

  return ZMK_EV_EVENT_BUBBLE;
}

static int battery_listener(const struct zmk_event_t *eh) {
  struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
  if (ev == NULL) {
    return ZMK_EV_EVENT_BUBBLE;
  }

  current_battery_capacity = ev->state_of_charge;

  update_led_state();

  return ZMK_EV_EVENT_BUBBLE;
}

#ifdef CONFIG_LED_WIDGET

ZMK_LISTENER(rgb_widget_usb_conn_listener, usb_conn_listener);
ZMK_LISTENER(rgb_widget_battery_listener, battery_listener);

ZMK_SUBSCRIPTION(rgb_widget_usb_conn_listener, zmk_usb_conn_state_changed);
ZMK_SUBSCRIPTION(rgb_widget_battery_listener, zmk_battery_state_changed);

static int init_led_control(const struct device *dev) {
  ARG_UNUSED(dev);
  LOG_INF("Charge Indicator Module initialized");

  is_usb_connected = zmk_usb_is_powered();
  current_battery_capacity = zmk_battery_state_of_charge();

  ws2812_strip_init();
  update_led_state();

  return 0;
}

SYS_INIT(init_led_control, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
