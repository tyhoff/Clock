#ifndef accel_bar_h
#define accel_bar_h

#include <pebble.h>

typedef Layer AccelBarLayer;

typedef enum {
  AccelBarDirectionUp = 0x00,
  AccelBarDirectionRight = 0x01,
  AccelBarDirectionDown = 0x02,
  AccelBarDirectionLeft = 0x03,
} AccelBarDirection;

typedef struct {
  int32_t min;
  int32_t max;
  int32_t value;
  GColor frame_color;
  GColor bar_color;
  AccelBarDirection direction;
} AccelBarData;

AccelBarLayer* accel_bar_layer_create(GRect frame);
void accel_bar_layer_destroy(AccelBarLayer* bar);
void accel_bar_layer_set_range(AccelBarLayer* bar, int32_t min, int32_t max);
void accel_bar_layer_set_value(AccelBarLayer* bar, int32_t value);
void accel_bar_layer_set_frame_color(AccelBarLayer* bar, GColor color);
void accel_bar_layer_set_bar_color(AccelBarLayer* bar, GColor color);
void accel_bar_layer_set_orientation(AccelBarLayer* bar, AccelBarDirection direction);

#endif
