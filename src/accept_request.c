#include "common.h"

#define NUM_BARS 2
#define INCREASE_RATE 2
#define ACCEL_STEP_MS 50
#define BAR_PX_LENGTH 50
#define PADDING 20
#define BAR_HALF_WIDTH 10
#define BAR_WIDTH 20

typedef enum {
  TOP = 0,
  RIGHT = 1,
  BOTTOM = 2,
  LEFT = 3,
} Location;

typedef struct bar {
  GRect rect;
} Bar;

typedef struct barfill {
  int x;
  int y;
} BarFill;

static Window *window;
static Layer *main_layer;
static TextLayer *text_layer;
static AppTimer *timer;
static Bar bars[NUM_BARS];
static BarFill bfill;

static char question_num_text[10];

static BitmapLayer* check_icon_layer;
static GBitmap *check_icon;

static BitmapLayer* x_icon_layer;
static GBitmap *x_icon;

// globals
extern int32_t question_number;
extern int32_t answer;

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
    layer_mark_dirty(main_layer);
    timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
  }

  /* APP_LOG(APP_LOG_LEVEL_DEBUG, "x: %d y: %d", accel.x, accel.y); */

  //compute where the next progress and direction should be

 
}

static void main_layer_update_callback(Layer *me, GContext *ctx) {
  for (int i=0; i<4; i++) {
    graphics_draw_rect(ctx, bars[i].rect);
  }
  // right
  if (bfill.x > 0 && bfill.y == 0) {
    graphics_fill_rect(ctx,
        GRect(bars[0].rect.origin.x,
          bars[0].rect.origin.y,
          bfill.x,
          bars[0].rect.size.h),
        0,
        GCornerNone);
    // left
  } else if (bfill.x < 0 && bfill.y == 0) {
    graphics_fill_rect(ctx,
        GRect(bars[1].rect.origin.x + BAR_PX_LENGTH,
          bars[1].rect.origin.y,
          bfill.x,
          bars[1].rect.size.h),
        0,
        GCornerNone);
  } else {
    ;
  }
}

static void draw_letters(Layer *window_layer, GRect bounds) {
  text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/2 - 65, 10}, .size = { 130, 50 } });
  text_layer_set_text(text_layer, "Answer\nRequested");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/2 - 65, 60}, .size = { 130, 50 } });
  strncpy(question_num_text, "Q: ", 10);
  strncat(question_num_text, itoa(question_number), 10);
  text_layer_set_text(text_layer, question_num_text);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/2 - 10 - 50, bounds.size.h/2 - 30 + 55}, .size = { 50, 20 } });
  text_layer_set_text(text_layer, "Deny");
  text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/2 + 10 , bounds.size.h/2 - 30 + 55}, .size = { 50, 20 } });
  text_layer_set_text(text_layer, "Accept");
  text_layer_set_text_alignment(text_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}


static void window_load( Window * window ) {
  vibes_short_pulse();
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "INSIDE FILL REQUEST\n" );

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  main_layer = layer_create(bounds);
  layer_set_update_proc(main_layer, main_layer_update_callback);
  layer_add_child(window_layer, main_layer);

  int halfWidth = bounds.size.w/2;
  int halfHeight = bounds.size.h/2;

  draw_letters(window_layer, bounds);

  //right
  bars[0].rect = GRect(halfWidth + BAR_HALF_WIDTH -1, halfHeight - BAR_HALF_WIDTH +55, BAR_PX_LENGTH , BAR_WIDTH);
  
  //left
  bars[1].rect = GRect(halfWidth - BAR_HALF_WIDTH - BAR_PX_LENGTH + 1, halfHeight-BAR_HALF_WIDTH +55, BAR_PX_LENGTH , BAR_WIDTH);

  check_icon_layer = bitmap_layer_create(GRect(halfWidth + 43,halfHeight-6 + 55,12,12));
  check_icon = gbitmap_create_with_resource(RESOURCE_ID_CHECK_ICON);
  bitmap_layer_set_bitmap(check_icon_layer, check_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(check_icon_layer));

  x_icon_layer = bitmap_layer_create(GRect(halfWidth - 55,halfHeight-6 + 55,12,12));
  x_icon = gbitmap_create_with_resource(RESOURCE_ID_X_ICON);
  bitmap_layer_set_bitmap(x_icon_layer, x_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(x_icon_layer));

  
  
}

static void window_appear(Window *window) {
  accel_data_service_subscribe(0, NULL);
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void window_disappear(Window *window) {
  accel_data_service_unsubscribe();
  app_timer_cancel(timer);
}

static void window_unload( Window * window ) {
  // text_layer_destroy( text_layer );
  gbitmap_destroy(check_icon);
  gbitmap_destroy(x_icon);
  layer_destroy(main_layer);
}

void accept_request_init(void) {

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear
  });
  const bool animated = true;
  window_stack_push(window, animated);

  bfill.x = 0;
}