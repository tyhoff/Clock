#include "common.h"

#define NUM_BARS 2
#define INCREASE_RATE 2
#define ACCEL_STEP_MS 50
#define BAR_PX_LENGTH 50
#define PADDING 20
#define BAR_HALF_WIDTH 10
#define BAR_WIDTH 20


typedef struct barfill {
  int x;
  int y;
} BarFill;

static Window *window;
static AppTimer *timer;
static AccelBarLayer *accel_bar;
static BarFill bfill;


static void timer_callback(void *data) { 

  AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

  accel_service_peek(&accel);
  // Insert UI code here

  //ignore
  if (abs(accel.x) < 200 && abs(accel.y) < 200) {
    bfill.x = 0;
    bfill.y = 0;

    //accept
  } else {
    if (abs(accel.x) >= abs(accel.y)) {

      //make smoother

      //right
      if (accel.x >= 0) {
        bfill.x += INCREASE_RATE;
        bfill.y = 0;

        //left 
      } else {
        bfill.x -= INCREASE_RATE;
        bfill.y = 0;
      }
    }
  }

  // accept threshold hit
  if (bfill.x >= 50) {
    window_stack_pop(false);
    fill_request_init();

  //deny threshold hit
  } else  if (bfill.x <= -50) {
    window_stack_pop(true);

  // keep drawing, threshold not hit
  } else {
    timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
  }

  accel_bar_layer_set_value(accel_bar, bfill.x);
}


static void window_load( Window * window ) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  int halfWidth = bounds.size.w/2;
  int halfHeight = bounds.size.h/2;

  accel_bar = accel_bar_layer_create(GRect(halfWidth + BAR_HALF_WIDTH -1, halfHeight - BAR_HALF_WIDTH +55, BAR_PX_LENGTH , BAR_WIDTH));
  accel_bar_layer_set_direction(accel_bar, AccelBarDirectionLeft);
  accel_bar_layer_set_range(accel_bar, 0, 100);
  accel_bar_layer_set_frame_color(accel_bar, GColorBlack);
  accel_bar_layer_set_bar_color(accel_bar, GColorBlack);
  layer_add_child(window_layer, accel_bar);

  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void window_unload( Window * window ) {

}

static void window_appear(Window *window) {
  accel_data_service_subscribe(0, NULL);
}

static void window_disappear(Window *window) {
  accel_data_service_unsubscribe();
}

void test_bar_init(void) {
  window = window_create();
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear
  });
  const bool animated = true;
  window_stack_push(window, animated);
}
