#include "common.h"
static Window * window;
static TextLayer * text_layer;

static uint8_t question_requested;

static void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}

static void out_failed_handler( DictionaryIterator * failed,
                                AppMessageResult reason,
                                void * context ) {
  // outgoing message failed
}

static void in_received_handler( DictionaryIterator * received, 
                                 void * context ) {
  // incoming message received
}

static void in_dropped_handler( AppMessageResult reason,
                                void * context ) {
  // incoming message dropped
}

static void window_load( Window * window ) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 },
                                           .size = { bounds.size.w, 20 } });

  APP_LOG(APP_LOG_LEVEL_DEBUG, "accepted answer request in fill_request\n" );
  char num_as_str[] = "Accepted answer request";
  text_layer_set_text( text_layer, num_as_str );

  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

}

static void window_unload( Window * window ) {
  text_layer_destroy( text_layer );
}

static void init(void) {

  /* register message handlers  */
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  /* message handling init */
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

}

static void deinit(void) {
  window_destroy(window);
}

void fill_request_init( uint8_t question_requested ){
  init();
}
