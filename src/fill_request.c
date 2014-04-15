#include <pebble.h>
#include "common.h"

#define NUM_BARS 4
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
static TextLayer *ticker_layer;
static TextLayer *notification_layer;
static AppTimer *timer;
static Bar bars[NUM_BARS];
static BarFill bfill;

static char question_num_text[10];

static BitmapLayer* b_icon_layer;
static GBitmap *b_icon;

static BitmapLayer* d_icon_layer;
static GBitmap *d_icon;

static BitmapLayer* a_icon_layer;
static GBitmap *a_icon;

static BitmapLayer* c_icon_layer;
static GBitmap *c_icon;

// globals
extern int32_t question_number;
extern int32_t answer;

static void popView() {
  window_stack_pop(false);
}

static void send_response(char answer) {
  text_layer_set_text(notification_layer, "Answer\nSent");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(notification_layer)); 
  app_timer_register(2000, popView, NULL);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Request filled for Q: %u with answer %c", (int)question_number, (int)answer);
  send_msg( question_number, (int32_t) answer );
}

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
    } else {

      // up 
      if (accel.y >= 0) {
        bfill.y += INCREASE_RATE;
        bfill.x = 0;


        //down
      } else {
        bfill.y -= INCREASE_RATE;
        bfill.x = 0;

      }
    }
  }

  if (bfill.x >= 50) {
    send_response('B');
  } else if (bfill.x <= -50) {
    send_response('D');
  } else if (bfill.y >= 50) {
    send_response('A');
  } else if (bfill.y <= -50) {
    send_response('C');
  } else {
    layer_mark_dirty(main_layer);
    timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
  }  
}

static void main_layer_update_callback(Layer *me, GContext *ctx) {
  for (int i=0; i<4; i++) {
    graphics_draw_rect(ctx, bars[i].rect);
  }
  // top
  if (bfill.x == 0 && bfill.y > 0) {
    graphics_fill_rect(ctx,
        GRect(bars[0].rect.origin.x,
          bars[0].rect.origin.y + BAR_PX_LENGTH,
          bars[0].rect.size.w,
          0 - bfill.y),
        0,
        GCornerNone);
    // right
  } else if (bfill.x > 0 && bfill.y == 0) {
    graphics_fill_rect(ctx,
        GRect(bars[1].rect.origin.x,
          bars[1].rect.origin.y,
          bfill.x,
          bars[1].rect.size.h),
        0,
        GCornerNone);
    // bottom
  } else if (bfill.x == 0 && bfill.y < 0) {
    graphics_fill_rect(ctx,
        GRect(bars[2].rect.origin.x,
          bars[2].rect.origin.y,
          bars[2].rect.size.w,
          0 - bfill.y),
        0,
        GCornerNone);
    // left
  } else if (bfill.x < 0 && bfill.y == 0) {
    graphics_fill_rect(ctx,
        GRect(bars[3].rect.origin.x + BAR_PX_LENGTH,
          bars[3].rect.origin.y,
          bfill.x,
          bars[3].rect.size.h),
        0,
        GCornerNone);
  } else {
    ;
  }
}

static void draw_letters(Layer *window_layer, GRect bounds) {
  text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/2 +17, 0}, .size = { 50, 50 } });
  text_layer_set_text(text_layer, "Fill\nRequest");
  text_layer_set_text_alignment(text_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_load( Window * window ) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  main_layer = layer_create(bounds);
  layer_set_update_proc(main_layer, main_layer_update_callback);
  layer_add_child(window_layer, main_layer);

  ticker_layer = text_layer_create((GRect) { .origin = { 5, 0 }, .size = { 55, 20 } });
  strncpy(question_num_text, "Q: ", 10);
  strncat(question_num_text, itoa(question_number), 10);
  text_layer_set_text(ticker_layer, question_num_text);
  text_layer_set_text_alignment(ticker_layer, GTextAlignmentLeft);
  text_layer_set_font(ticker_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(ticker_layer));

  draw_letters(window_layer, bounds);

  int halfWidth = bounds.size.w/2;
  int halfHeight = bounds.size.h/2;

  bars[0].rect = GRect(halfWidth-BAR_HALF_WIDTH , halfHeight - BAR_HALF_WIDTH - BAR_PX_LENGTH + 1, BAR_WIDTH , BAR_PX_LENGTH);
  bars[1].rect = GRect(halfWidth + BAR_HALF_WIDTH -1 , halfHeight - BAR_HALF_WIDTH , BAR_PX_LENGTH , BAR_WIDTH);
  bars[2].rect = GRect(halfWidth-BAR_HALF_WIDTH , halfHeight + BAR_HALF_WIDTH - 1, BAR_WIDTH , BAR_PX_LENGTH);
  bars[3].rect = GRect(halfWidth - BAR_HALF_WIDTH - BAR_PX_LENGTH + 1, halfHeight-BAR_HALF_WIDTH , BAR_PX_LENGTH , BAR_WIDTH);

  a_icon_layer = bitmap_layer_create(GRect(halfWidth - 6,halfHeight - 55,12,12));
  a_icon = gbitmap_create_with_resource(RESOURCE_ID_A_ICON);
  bitmap_layer_set_bitmap(a_icon_layer, a_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(a_icon_layer));

  b_icon_layer = bitmap_layer_create(GRect(halfWidth + 43,halfHeight-6,12,12));
  b_icon = gbitmap_create_with_resource(RESOURCE_ID_B_ICON);
  bitmap_layer_set_bitmap(b_icon_layer, b_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(b_icon_layer));

  c_icon_layer = bitmap_layer_create(GRect(halfWidth - 6,halfHeight + 43,12,12));
  c_icon = gbitmap_create_with_resource(RESOURCE_ID_C_ICON);
  bitmap_layer_set_bitmap(c_icon_layer, c_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(c_icon_layer));

  d_icon_layer = bitmap_layer_create(GRect(halfWidth - 55,halfHeight-6,12,12));
  d_icon = gbitmap_create_with_resource(RESOURCE_ID_D_ICON);
  bitmap_layer_set_bitmap(d_icon_layer, d_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(d_icon_layer));

  notification_layer = text_layer_create((GRect) { .origin = { 10, 20 }, .size = { 124, 60 } });
  text_layer_set_background_color( notification_layer, GColorBlack );
  text_layer_set_text_alignment(notification_layer, GTextAlignmentCenter);
  text_layer_set_font(notification_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color( notification_layer, GColorWhite );

  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void window_unload( Window * window ) {
  gbitmap_destroy(a_icon);
  gbitmap_destroy(b_icon);
  gbitmap_destroy(c_icon);
  gbitmap_destroy(d_icon);
  window_destroy(window);
  accel_data_service_unsubscribe();
  text_layer_destroy( text_layer );
  text_layer_destroy(notification_layer);
  text_layer_destroy(ticker_layer);
  layer_destroy(main_layer);
}

void fill_request_init(){
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  bfill.x = 0;
  bfill.y = 0;

  accel_data_service_subscribe(0, NULL);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Window - fill_request pushed" );
}
