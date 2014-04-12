#include <pebble.h>
#include "common.h"
static Window * window;
static TextLayer * text_layer;

extern int32_t question_number;
extern int32_t answer;

static void window_load( Window * window ) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 },
                                           .size = { bounds.size.w, 20 } });

  send_msg( question_number, (int32_t) 'A' );
  APP_LOG(APP_LOG_LEVEL_DEBUG, "accepted answer request in fill_request\n" );
  char num_as_str[] = "Accepted answer request";
  text_layer_set_text( text_layer, num_as_str );

  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void deinit(void) {
  window_destroy(window);
}

static void window_unload( Window * window ) {
  deinit();
  text_layer_destroy( text_layer );
}

static void init(void) {

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

}

void fill_request_init(){
  init();
}
