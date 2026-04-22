#pragma once

// Status type definitions for enhanced API
enum status_type {
  STATUS_BATTERY = 0,
  STATUS_CONNECTIVITY = 1,
  STATUS_LAYER = 2,
  STATUS_CUSTOM = 3
};

// Animation type definitions
enum animation_type {
  ANIM_STATIC = 0,
  ANIM_BLINK = 1,
  ANIM_PULSE = 2,
  ANIM_FADE = 3,
  ANIM_WAVE = 4,
  ANIM_RAINBOW = 5
};

// Animation state structure
struct animation_state {
  enum animation_type type;
  uint32_t duration_ms;
  uint32_t period_ms;
  uint8_t start_color;
  uint8_t end_color;
  bool loop;
};

enum status_priority {
  PRIORITY_CRITICAL_BATTERY = 0,  // Highest - never shareable
  PRIORITY_CONNECTION_CHANGE = 1, // High - can interrupt sharing
  PRIORITY_LAYER_CHANGE = 2,      // Medium - can use shared LEDs
  PRIORITY_MANUAL_TRIGGER = 3,    // Normal
  PRIORITY_AMBIENT = 4            // Lowest - always shareable
};
enum color_index {
  COLOR_INDEX_OFF = 0,
  COLOR_INDEX_RED = 1,
  COLOR_INDEX_GREEN = 2,
  COLOR_INDEX_YELLOW = 3,
  COLOR_INDEX_BLUE = 4,
  COLOR_INDEX_MAGENTA = 5,
  COLOR_INDEX_CYAN = 6,
  COLOR_INDEX_WHITE = 7
};

// LED state tracking
struct led_state {
  uint8_t current_color;
  uint8_t base_color;          // Original color before sharing
  uint8_t status_type;         // Current status type assigned
  uint8_t priority;            // Current priority level
  bool is_shared;              // Whether LED is currently shared
  bool is_persistent;          // Whether status should persist
  uint32_t share_end_time;     // When to return to base status
  struct animation_state anim; // Current animation state
};

void ws2812_strip_init(void);
int ws2812_set_led(uint8_t led_index, uint8_t color_idx);
int ws2812_clear_led(uint8_t led_index);
int ws2812_clear_all(void);
