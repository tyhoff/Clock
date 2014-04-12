#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static uint8_t answer_request_accepted = 0;
static Tuple last_answer_request;

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
  uint8_t capacity = 100;
  uint8_t buffer[capacity];
  Tuple * tuple = dict_read_begin_from_buffer( received,
                                               buffer,
                                               sizeof(buffer) );
  while ( tuple ){
    switch (tuple->key) {
      case REQUEST_ID:
        memcpy( &last_answer_request, tuple, sizeof(Tuple) );
        trigger_answer_request_notifier();
        break;
    }
    tuple = dict_read_next(&iter);
  }
}

static void in_dropped_handler( AppMessageResult reason, void * context ) {
  // incoming message dropped
}

static void trigger_answer_request_notifier(){
  ; // TODO tyler, draw something
  ; // then set 5 second timeout, if it expires, make the alert dissappear
}

static void accel_tap_handler( AccelAxisType axis, int32_t direction ) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  answer_request_accepted = 1;


  /*
  Tuplet message[] = {
    TupletInteger( UUID, (uint8_t) gen_uuid() ),
    TupletInteger( ROOM_ID, (uint8_t) room_id ),
    TupletInteger( SENDER_ID, (uint8_t) my_id ),
    TupletInteger( RECIPIENT_ID, (uint8_t) last_answer_request->data ),
    TupletInteger( REQUEST_ID, (uint8_t) recipient_id ),
  }
  */
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if ( iter == NULL ){
    return;
  }
  /*
  dict_serialize_tuplets_to_buffer( message,
                                    ARRAY_LENGTH( message ),
                                    buffer,
                                    &size );
  */
  Tuplet accepted = TupletInteger( request_id, 1 );
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();

  ; // TODO render the fill_request window
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
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
