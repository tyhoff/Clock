#include "common.h"


static Window *window;
static TextLayer *text_layer;
static TextLayer *text_answer;
static TextLayer *text_question;
static AppTimer *timer;
static char answer_text[30];
static char question_text[30];

// globals
extern int32_t question_number;
extern int32_t answer;

static void popView() {
  window_stack_pop(true);
}

static void window_load( Window * window ) {
  vibes_short_pulse();

  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color( window, GColorBlack );
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 5 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text( text_layer, "Answer" );
  text_layer_set_background_color( text_layer, GColorBlack );
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment( text_layer, GTextAlignmentCenter );
  text_layer_set_text_color( text_layer, GColorWhite );
  layer_add_child(window_layer, text_layer_get_layer( text_layer ));

  text_answer = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w, 50 } });
  snprintf( answer_text, 30, "%c", (char) answer );
  // snprintf( answer_text, 30, "B" );
  text_layer_set_text( text_answer, answer_text );
  text_layer_set_background_color( text_answer, GColorBlack );
  text_layer_set_font(text_answer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment( text_answer, GTextAlignmentCenter );
  text_layer_set_text_color( text_answer, GColorWhite );
  layer_add_child(window_layer, text_layer_get_layer( text_answer ));

  text_question = text_layer_create((GRect) { .origin = { 0, 110 }, .size = { bounds.size.w, 40 } });
  snprintf( question_text, 30, "Q: %ld", question_number );
  // snprintf( question_text, 30, "Q: 37" );
  text_layer_set_text( text_question, question_text );
  text_layer_set_background_color( text_question, GColorBlack );
  text_layer_set_font(text_question, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment( text_question, GTextAlignmentCenter );
  text_layer_set_text_color( text_question, GColorWhite );
  layer_add_child(window_layer, text_layer_get_layer( text_question ));


  timer = app_timer_register(3000, popView, NULL);
  
}

static void window_unload( Window * window ) {

}

static void init(void) {
  window = window_create();
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

void receive_answer_init(){
  init();
}
