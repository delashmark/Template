#include <pebble.h>

Window *window;

static AppTimer *timer = NULL;

static TextLayer *text_layer;

static char* outString = "";//"Template";
static bool SUNSHINE = true;

// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1,
};

// Write message to buffer & send
void send_message(int STATUS, char* MSG) {
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_int32(iter, STATUS_KEY, STATUS);
	dict_write_cstring(iter, MESSAGE_KEY, MSG);
	dict_write_end(iter);
  	app_message_outbox_send();
}

static void timer_callback(void *data) {
	send_message(3, "timer");
	timer = app_timer_register(10000, timer_callback, NULL);
}

// Called when a message is received from PebbleKitJS
void in_received_handler(DictionaryIterator *received, void *context) {
	int STATUS = (int)dict_find(received, STATUS_KEY)->value->uint16;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", STATUS);
	
	char* MSG = dict_find(received, MESSAGE_KEY)->value->cstring;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", MSG);
	if (STATUS < 2) {
		SUNSHINE = (bool)STATUS;
		window_set_background_color(window, SUNSHINE ? GColorWhite : GColorBlack);
		text_layer_set_text_color(text_layer, SUNSHINE ? GColorBlack : GColorWhite);
		text_layer_set_text(text_layer, outString = MSG);
	}
	else text_layer_set_text(text_layer, outString = MSG);
}

// Called when an incoming message from PebbleKitJS is dropped
void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void up_pressed_handler(ClickRecognizerRef recognizer, void *context) {
	send_message(1, "U");
	//vibes_short_pulse();
}

void up_released_handler(ClickRecognizerRef recognizer, void *context) {
	send_message(0, "U");
}

void down_pressed_handler(ClickRecognizerRef recognizer, void *context) {
	send_message(1, "D");
	//vibes_double_pulse();
}

void down_released_handler(ClickRecognizerRef recognizer, void *context) {
	send_message(0, "D");
}

void select_pressed_handler(ClickRecognizerRef recognizer, void *context) {
	send_message(1, "S");
	//vibes_long_pulse();
}

void select_released_handler(ClickRecognizerRef recognizer, void *context) {
	send_message(0, "S");
}

void click_config_provider(void *context) {
  	//const uint16_t repeat_interval_ms = 50;
  	//window_long_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_pressed_handler);
  	window_long_click_subscribe(BUTTON_ID_UP, 0, (ClickHandler) up_pressed_handler, (ClickHandler) up_released_handler);
  	window_long_click_subscribe(BUTTON_ID_DOWN, 0, (ClickHandler) down_pressed_handler, (ClickHandler) down_released_handler);
  	window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_pressed_handler, (ClickHandler) select_released_handler);
  	//window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) down_click_handler);
}

void window_load(Window *me) {
	window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);
	
  	Layer *layer = window_get_root_layer(me);
  	const int16_t width = layer_get_frame(layer).size.w;// - ACTION_BAR_WIDTH - 3;
	const int16_t height = layer_get_frame(layer).size.h;// - ACTION_BAR_WIDTH - 3;
	
  	text_layer = text_layer_create(GRect(4, -4, width - 4, height));
  	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  	//text_layer_set_text_color(text_layer, SUNSHINE ? GColorBlack : GColorWhite);
  	text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
  	text_layer_set_background_color(text_layer, GColorClear);
  	layer_add_child(layer, text_layer_get_layer(text_layer));
	text_layer_set_text(text_layer, outString);
}

void window_unload(Window *window) {
  	text_layer_destroy(text_layer);
}

void init(void) {
  	window = window_create();
	window_set_fullscreen(window, true);
	
	// Get the count from persistent storage for use if it exists, otherwise use the default
	//SUNSHINE = persist_exists(0) ? persist_read_bool(0) : true;

	//window_set_background_color(window, SUNSHINE ? GColorWhite : GColorBlack);
  	window_set_window_handlers(window, (WindowHandlers) {
    	.load = window_load,
    	.unload = window_unload,
    });
	
	// Register AppMessage events
	app_message_register_inbox_received(in_received_handler);           
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());    //Large input and output buffer sizes
	
	timer = app_timer_register(10000, timer_callback, NULL); // First timer is set to 10 seconds!
	
	window_stack_push(window, false /* Animated */);
}

void deinit(void) {
	// Save the count into persistent storage on app exit
  	//persist_write_bool(0, !SUNSHINE);
	app_message_deregister_callbacks();
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
