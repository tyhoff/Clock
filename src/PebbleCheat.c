#include <pebble.h>
#include "common.h"

static Window * window;
static TextLayer * text_layer;
static uint8_t answer_request_pending = 0;

static void answer_request_if_needed( uint8_t accepted );

// non statics
int32_t question_number = 0;
int32_t answer = 0;


void trigger_answer_request_notifier(){
  // TODO tyler, draw something
  // then set 5 second timeout, if it expires, make the alert dissappear
  // and unset answer_request_pending
  answer_request_pending = 1;
}

void trigger_answer_pending_notifier(){
  // TODO tyler, draw something
  // this is triggered when we recieve confirmation that a teammate has
  // accepted your request and is answering it.
}

static void accel_tap_handler( AccelAxisType axis, int32_t direction ) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  //
  send_request_init();
}


static void answer_request_if_needed( uint8_t accepted ){

  APP_LOG(APP_LOG_LEVEL_DEBUG, "accepted state(%d) and pending is %d\n",
                               accepted,
                               answer_request_pending );
  if ( answer_request_pending == 1 ){
    answer_request_pending = 0;
    if ( !accepted ){
      send_msg( question_number, -1 );
    }

    fill_request_init();
  }
}

static void select_click_handler( ClickRecognizerRef recognizer,
                                  void * context ) {
  //text_layer_set_text(text_layer, "Select");
  answer_request_if_needed( 1 );
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "This is a clock:)");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));  
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void window_appear(Window *window) {
  accel_tap_service_subscribe(&accel_tap_handler);
}

static void window_disappear(Window *window) {
  accel_tap_service_unsubscribe();
}

static void init(void) {
  app_msg_init();

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
