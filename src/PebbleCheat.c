#include "common.h"

static Window * window;
static TextLayer * text_layer;
static uint8_t answer_request_accepted = 0;
static uint8_t answer_request_pending = 0;
static uint16_t question_requested = 0;
static uint32_t question_request_id = 0;

static void trigger_answer_request_notifier(uint8_t question_request);

static void out_sent_handler( DictionaryIterator *sent, 
                              void *context ) {
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
  Tuple * request_id_tuple = dict_find( received, REQUEST_ID );
  Tuple * question_number_tuple = dict_find( received, QUESTION_NUMBER );



  if ( request_id_tuple ){
    question_request_id = ( request_id_tuple->value->uint32 );
    APP_LOG(APP_LOG_LEVEL_DEBUG, "got request_id %lu\n", question_request_id );
    trigger_answer_request_notifier(question_request_id);
  }
  if ( question_number_tuple ){
    if ( answer_request_accepted == 1 ){
      // this is part of an ANSWER_REQUEST
      question_requested = ( question_number_tuple->value->uint32 );
    } else {
      // this is part of an ANSWER_BROADCAST
    }
  }
}

static void in_dropped_handler( AppMessageResult reason, void * context ) {
  // incoming message dropped
}

static void trigger_answer_request_notifier(uint8_t question_request){
  accept_request_init(question_request);

  answer_request_pending = 1;
}

static void accel_tap_handler( AccelAxisType axis, int32_t direction ) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  //
  // if ( answer_request_accepted == 0 ){
  //   answer_request_accepted = 1;

  //   DictionaryIterator *iter;
  //   app_message_outbox_begin(&iter);
  //   if ( iter == NULL ){
  //     return;
  //   }
  //   //Tuplet accepted = TupletInteger( REQUEST_ID,  question_request_id );
  //   dict_write_uint32( iter,
  //                    REQUEST_ID,
  //                    question_request_id );
  //   app_message_outbox_send();

  //   fill_request_init( question_requested );
  // }
}

static void handle_init(void) {
  accel_tap_service_subscribe(&accel_tap_handler);
}

static void handle_deinit(void) {
  accel_tap_service_unsubscribe();
}

static void select_click_handler( ClickRecognizerRef recognizer,
                                  void * context ) {
  text_layer_set_text(text_layer, "Select");
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
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
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

  handle_init();

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  handle_deinit();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
