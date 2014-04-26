#include "accel_bar.h"

static void accel_bar_layer_update(AccelBarLayer *bar, GContext *ctx);

AccelBarLayer* accel_bar_layer_create(GRect frame) {
  AccelBarLayer *bar = layer_create_with_data(frame, sizeof(AccelBarData));
  layer_set_update_proc(bar, accel_bar_layer_update);
  layer_mark_dirty(bar);

  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  data->min = 0;
  data->max = 100;
  data->value = 0;
  data->frame_color = GColorBlack;
  data->bar_color = GColorBlack;
  data->direction = AccelBarDirectionUp;

  return bar;
}

void accel_bar_layer_destroy(AccelBarLayer* bar) {
  layer_destroy(bar);
}

void accel_bar_layer_set_range(AccelBarLayer* bar, int32_t min, int32_t max) {
  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  data->min = min;
  data->max = max;
  layer_mark_dirty(bar);
}

void accel_bar_layer_set_value(AccelBarLayer* bar, int32_t value) {
  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  data->value = value;
  layer_mark_dirty(bar);
}

void accel_bar_layer_set_frame_color(AccelBarLayer* bar, GColor color) {
  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  data->frame_color = color;
  layer_mark_dirty(bar);
}

void accel_bar_layer_set_bar_color(AccelBarLayer* bar, GColor color) {
  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  data->bar_color = color;
  layer_mark_dirty(bar);
}

void accel_bar_layer_set_direction(AccelBarLayer* bar, AccelBarDirection direction) {
  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  data->direction = direction;
  layer_mark_dirty(bar);
}

static void accel_bar_layer_update(AccelBarLayer *bar, GContext *ctx) {
  AccelBarData *data = (AccelBarData *)layer_get_data(bar);
  GRect bounds = layer_get_bounds(bar);

  graphics_context_set_stroke_color(ctx, data->frame_color);
  graphics_draw_rect(ctx, bounds);

  if (data->direction == AccelBarDirectionUp) {
    bounds.origin.y = bounds.size.h - (((data->value - data->min) * bounds.size.h) / (data->max - data->min));
    bounds.size.h = ((data->value - data->min) * bounds.size.h) / (data->max - data->min);
  } else if (data->direction == AccelBarDirectionRight) {
    bounds.size.w = ((data->value - data->min) * bounds.size.w) / (data->max - data->min);
  } else if (data->direction == AccelBarDirectionDown) {
    bounds.origin.y = bounds.size.h - (((data->value - data->min) * bounds.size.h) / (data->max - data->min));
    bounds.size.h = ((data->value - data->min) * bounds.size.h) / (data->max - data->min);
  } else if (data->direction == AccelBarDirectionLeft) {
    bounds.size.w = ((data->value - data->min) * bounds.size.w) / (data->max - data->min);
  }

  graphics_context_set_fill_color(ctx, data->bar_color);
  graphics_fill_rect(ctx, bounds, 0, GCornersAll);
}
