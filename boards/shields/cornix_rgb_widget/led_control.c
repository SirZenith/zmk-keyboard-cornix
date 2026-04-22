#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

#include "ws2812/ws2812_control.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static bool is_usb_power = false;
static uint8_t current_battery_capacity = 0;

static void update_led_state(void) {
  LOG_INF("Charging condition met. USB: %d, Battery: %d", is_usb_power,
          current_battery_capacity);

  if (is_usb_power && current_battery_capacity < 100) {
    ws2812_set_led(0, 1);
  } else {
    ws2812_clear_led(0);
  }
}

static void usb_conn_listener(const struct zmk_event_header *eh) {
  struct zmk_usb_conn_state_changed *ev =
      (struct zmk_usb_conn_state_changed *)eh;
  is_usb_power = ev->conn_state == ZMK_USB_CONN_POWERED;
  update_led_state();
}

static void battery_listener(const struct zmk_event_header *eh) {
  struct zmk_battery_state_changed *ev = (struct zmk_battery_state_changed *)eh;
  current_battery_capacity = ev->state_of_charge;
  update_led_state();
}

ZMK_LISTENER(charge_indicator_usb_listener, usb_conn_listener);
ZMK_LISTENER(charge_indicator_battery_listener, battery_listener);

ZMK_SUBSCRIPTION(charge_indicator_usb_listener, zmk_usb_conn_state_changed);
ZMK_SUBSCRIPTION(charge_indicator_battery_listener, zmk_battery_state_changed);

static int init_led_control(const struct device *dev) {
  ARG_UNUSED(dev);
  LOG_INF("Charge Indicator Module initialized");

  is_usb_power = zmk_usb_is_powered();
  current_battery_capacity = zmk_battery_state_of_charge();

  ws2812_strip_init();
  update_led_state();

  return 0;
}

SYS_INIT(init_led_control, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
