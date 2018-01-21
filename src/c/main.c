#include <pebble.h>

Window *s_main_window;

//static int s_btc_query_counter = 0;
//static int s_btc_query_counter_chk = 0;

static char *s_info_str;

static bool s_btc_up_to_date = false;
static bool s_btc_up_to_date_chk = false;

static GFont s_time_font;
static GFont s_text_font;
static GFont s_small_font;
static GFont s_week_font;

TextLayer *s_time_hour_layer;
TextLayer *s_day_of_week_layer;
TextLayer *s_week_layer;
TextLayer *s_btc_value;
TextLayer *s_month_day_layer;
TextLayer *s_battery_layer;
TextLayer *s_bluetooth_layer;

static void upcase(char *str) {
  
  char *s = str;
  
  while (*s) {
     if(*s>0x60) *s -= 0x20;
     s++;
  }
}

static void bluetooth_handler(bool connected) {
  
  if(connected)
      text_layer_set_text(s_bluetooth_layer, "BT");
  else {
    text_layer_set_text(s_bluetooth_layer, "");
  }
}

static void update_time(struct tm *tick_time) {

  static char s_buffer_hours[10];

  // Format hours:minutes
  strftime(s_buffer_hours, sizeof(s_buffer_hours), "%H:%M", tick_time);  
  text_layer_set_text(s_time_hour_layer, s_buffer_hours);
}

static void update_date(struct tm *tick_time) {
  
  static char s_buffer_day_of_week[10];
  static char s_buffer_week_of_year[15];
  static char s_buffer_name_of_month[15];
  
  // Format day of week
  strftime(s_buffer_day_of_week, sizeof(s_buffer_day_of_week), "%A", tick_time);  
  upcase(s_buffer_day_of_week);
  text_layer_set_text(s_day_of_week_layer, s_buffer_day_of_week);
  
  // Format week of year
  strftime(s_buffer_week_of_year, sizeof(s_buffer_week_of_year), "WEEK %OW - %Y", tick_time);  
  text_layer_set_text(s_week_layer, s_buffer_week_of_year);

  // Format name of month
  strftime(s_buffer_name_of_month, sizeof(s_buffer_name_of_month), "%B %d", tick_time);  
  upcase(s_buffer_name_of_month);
  text_layer_set_text(s_month_day_layer, s_buffer_name_of_month);
}

//static void redraw_btc_value() {
//  
 // s_btc_query_counter_chk = s_btc_query_counter;
//
//  if(s_info_str)
//    text_layer_set_text(s_btc_value, s_info_str);
//}

static void tick_time_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  if((units_changed & DAY_UNIT) == DAY_UNIT)
    update_date(tick_time);
  
  if((units_changed & MINUTE_UNIT) == MINUTE_UNIT) 
  {
    update_time(tick_time);

   // if(tick_time->tm_min == 0)
   // {
   //   DictionaryIterator *iter;
   //   app_message_outbox_begin(&iter);
//
   //   //APP_LOG(APP_LOG_LEVEL_INFO, "Request update!");
   //   app_message_outbox_send();      
   // }
  }

  if(s_btc_up_to_date != s_btc_up_to_date_chk) {
    s_btc_up_to_date_chk = s_btc_up_to_date;
    
    GColor color =  GColorVeryLightBlue;
    
    if(!s_btc_up_to_date)
      color = GColorRed;
    
     text_layer_set_text_color(s_btc_value, color);
  }
  
 // if (s_btc_query_counter != s_btc_query_counter_chk) {
 //   redraw_btc_value();
 // }

}

static void battery_state_handler(BatteryChargeState state) {
  
  static char s_buffer_battery[5];
   
  snprintf(s_buffer_battery, sizeof(s_buffer_battery), "%d%%", state.charge_percent);
  text_layer_set_text(s_battery_layer, s_buffer_battery);
  
  GColor color =  GColorWhite;

  if(!state.is_charging) {

    if(state.charge_percent > 30)
      color = GColorGreen;
    else if(state.charge_percent > 20)
      color = GColorYellow;
    else if(state.charge_percent > 10)
      color = GColorOrange;
     else
      color = GColorRed;
  }
  
  text_layer_set_text_color(s_battery_layer, color);
}

//static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
// 
//  Tuple *temp_tuple3 = dict_find(iterator, MESSAGE_KEY_I);
//  s_info_str = (char*)temp_tuple3->value->cstring;
//  
//  s_btc_query_counter++;
//  
//  if(!s_btc_up_to_date)
//        redraw_btc_value();
//
//  s_btc_up_to_date = true;
//}

//static void inbox_dropped_callback(AppMessageResult reason, void *context) {
// APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
//  s_btc_up_to_date = false;
//}

//static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
//  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
//  s_btc_up_to_date = false;
//}

//static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
//  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
//}



static void main_window_load(Window *window) {
 
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  
  window_set_background_color(window, GColorBlack);
  
  GRect bounds = layer_get_bounds(window_layer);

  // load fonts  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TIMEFONT56));
  s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TEXTFONT20));
  s_small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SMALLTEXT14));
  s_week_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_WEEKFONT16));
  
  // day of week
  s_day_of_week_layer = text_layer_create(GRect(0, 20, bounds.size.w, 80));
  text_layer_set_font(s_day_of_week_layer, s_text_font);
  text_layer_set_background_color(s_day_of_week_layer, GColorClear);
  text_layer_set_text_color(s_day_of_week_layer, GColorCyan);
  text_layer_set_text_alignment(s_day_of_week_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_day_of_week_layer));
  
  // hours:minutes
  s_time_hour_layer = text_layer_create(GRect(0, 40, bounds.size.w, 80));
  text_layer_set_font(s_time_hour_layer, s_time_font);
  text_layer_set_background_color(s_time_hour_layer, GColorClear);
  text_layer_set_text_color(s_time_hour_layer, GColorWhite);
  text_layer_set_text_alignment(s_time_hour_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_hour_layer));
  
  // month day
  s_month_day_layer = text_layer_create(GRect(0, 102, bounds.size.w, 80));
  text_layer_set_font(s_month_day_layer, s_text_font);
  text_layer_set_background_color(s_month_day_layer, GColorClear);
  text_layer_set_text_color(s_month_day_layer,  GColorOrange);
  text_layer_set_text_alignment(s_month_day_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_month_day_layer));

  // week - year
  s_week_layer = text_layer_create(GRect(0, 126, bounds.size.w, 80));
  text_layer_set_font(s_week_layer, s_week_font);
  text_layer_set_background_color(s_week_layer, GColorClear);
  text_layer_set_text_color(s_week_layer, GColorGreen);
  text_layer_set_text_alignment(s_week_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_week_layer));

  // btc value
  s_btc_value = text_layer_create(GRect(0, 146, bounds.size.w, 80));
  text_layer_set_font(s_btc_value, s_week_font);
  text_layer_set_background_color(s_btc_value, GColorClear);
  text_layer_set_text_color(s_btc_value, GColorVeryLightBlue);
  text_layer_set_text_alignment(s_btc_value, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_btc_value));
  
  // battery lvl
  s_battery_layer = text_layer_create(GRect(4, 0, bounds.size.w-8, 80));
  text_layer_set_font(s_battery_layer, s_small_font);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorDarkGray);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  
  // bluetooth connected
  s_bluetooth_layer = text_layer_create(GRect(4, 0, bounds.size.w-8, 80));
  text_layer_set_font(s_bluetooth_layer, s_small_font);
  text_layer_set_background_color(s_bluetooth_layer, GColorClear);
  text_layer_set_text_color(s_bluetooth_layer, GColorCyan);
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_bluetooth_layer));
  
  battery_state_handler(battery_state_service_peek());
  battery_state_service_subscribe(battery_state_handler);
  
  bluetooth_handler(connection_service_peek_pebble_app_connection());
  bluetooth_connection_service_subscribe(bluetooth_handler);
  
  // Register callbacks
  //app_message_register_inbox_received(inbox_received_callback);
  //app_message_register_inbox_dropped(inbox_dropped_callback);
  //app_message_register_outbox_failed(outbox_failed_callback);
  //app_message_register_outbox_sent(outbox_sent_callback);
  
  //const int inbox_size = 128;
  //const int outbox_size = 128;
  //app_message_open(inbox_size, outbox_size);
  
  // set the current time
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  tick_time_handler(tick_time, DAY_UNIT | MINUTE_UNIT);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_time_handler);
}

static void main_window_unload(Window *window) {
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  
  text_layer_destroy(s_btc_value);
  text_layer_destroy(s_bluetooth_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_time_hour_layer);
  text_layer_destroy(s_day_of_week_layer);
  text_layer_destroy(s_week_layer);
  text_layer_destroy(s_month_day_layer);
  
  fonts_unload_custom_font(s_small_font);
  fonts_unload_custom_font(s_week_font);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_text_font);
}

void handle_init(void) {
  s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
}

void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
