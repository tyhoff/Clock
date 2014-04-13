#include "pebble.h"

#define NUM_MENU_SECTIONS 1

static Window *window;
static MenuLayer *menu_layer;
extern uint8_t all_answers[120];
extern uint8_t all_answers_length;

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return all_answers_length/2;
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  menu_cell_basic_draw(ctx, cell_layer, "Basic Item", "With a subtitle", NULL);
}

// This initializes the menu upon window load
void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
  window_destroy(window);
}

void all_answers_init(void) {
  window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });


  char * strings[256];

  int problem_number = 0;
  for (int i=0; i<all_answers_length; i++) {
    if (i % 2 == 0) {
      problem_number = all_answers[i];
      if (strings[problem_number] == NULL) {
        strings[problem_number] = (char *) malloc(20); 
      }
    } else {
      char character[4];
      snprintf(character, 4, "%c,", all_answers[i]);
      strncat(strings[problem_number], character, 4);  
    }
  }

  for (int i=0; i<256; i++) {
    if (strings[i] != NULL)
      APP_LOG(APP_LOG_LEVEL_DEBUG, "%s",strings[i] );
  }


  window_stack_push(window, true);
  
}
