#include <pebble_fonts.h>
#include "common.h"

static Window * window;
static TextLayer * text_layer;
static AppTimer *timer;

static bool canTriggerTap = false;

static char clock_time[20];

// non statics
int32_t question_number = 0;
int32_t answer = 0;
uint8_t all_answers[120];
uint8_t all_answers_length;


void trigger_answer_request_notifier(){
  accept_request_init();
}

void trigger_answer_pending_notifier(){
  // TODO
  // this is triggered when we recieve confirmation that a teammate has
  // accepted your request and is answering it.
}

static void listenForTaps() {
  canTriggerTap = true;
}

static void accel_tap_handler( AccelAxisType axis, int32_t direction ) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  // TRANSITION
  // tap will enter the send request screen
  if (canTriggerTap) {
   send_request_init(); 
  }
}

static void select_click_handler( ClickRecognizerRef recognizer,
                                  void * context ) {
  // maybe use this for answer summary later
  all_answers_init();
  // receive_answer_init();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void handle_minute_tick( struct tm *tick_time,
                                TimeUnits units_changed) {
  clock_copy_time_string( clock_time, 20 );  
  text_layer_set_text(text_layer, clock_time );
}

static void window_load( Window * window ) {
  Layer *window_layer = window_get_root_layer( window );
  GRect bounds = layer_get_bounds( window_layer );
  window_set_background_color( window, GColorBlack );

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 },
                                           .size = { bounds.size.w, 40 } });
  clock_copy_time_string( clock_time, 20 );  
  text_layer_set_text(text_layer, clock_time );
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_background_color( text_layer, GColorBlack );
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_color( text_layer, GColorWhite );

  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  timer = app_timer_register(1000, listenForTaps, NULL);
}

static void window_unload( Window * window ) {
  text_layer_destroy( text_layer );
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
  window_set_fullscreen(window, true);
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear
  });
  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
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
