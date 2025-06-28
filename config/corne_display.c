/*
 * Custom display configuration for Corne keyboard with WPM
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>

#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/wpm.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Custom widget for displaying layer names
static const char *layer_names[] = {
    "QWERTY",
    "LOWER",
    "RAISE",
    "ADJUST"
};

// LVGL objects
static lv_obj_t *layer_label;
static lv_obj_t *wpm_label;

// Create display widgets
static void create_display_widgets(void) {
    lv_obj_t *screen = lv_scr_act();
    
    // Create layer label
    layer_label = lv_label_create(screen);
    lv_obj_align(layer_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(layer_label, "Layer: QWERTY");
    
    // Create WPM label
    wpm_label = lv_label_create(screen);
    lv_obj_align(wpm_label, LV_ALIGN_TOP_LEFT, 0, 20);
    lv_label_set_text(wpm_label, "WPM: 0");
}

// Custom display update function
static void update_display_content(void) {
    if (!layer_label || !wpm_label) {
        return;
    }
    
    // Get current layer
    uint8_t active_layer = zmk_keymap_highest_layer_active();
    
    // Get current WPM
    uint8_t current_wpm = zmk_wpm_get_state();
    
    // Update labels
    char layer_text[32];
    snprintf(layer_text, sizeof(layer_text), "Layer: %s", 
             active_layer < 4 ? layer_names[active_layer] : "UNKNOWN");
    lv_label_set_text(layer_label, layer_text);
    
    char wpm_text[32];
    snprintf(wpm_text, sizeof(wpm_text), "WPM: %d", current_wpm);
    lv_label_set_text(wpm_label, wpm_text);
}

// Listen for layer changes
static int layer_status_listener(const zmk_event_t *eh) {
    update_display_content();
    return 0;
}

// Listen for WPM changes
static int wpm_status_listener(const zmk_event_t *eh) {
    update_display_content();
    return 0;
}

ZMK_LISTENER(custom_display, layer_status_listener)
ZMK_SUBSCRIPTION(custom_display, zmk_layer_state_changed)

ZMK_LISTENER(wpm_display, wpm_status_listener)
ZMK_SUBSCRIPTION(wpm_display, zmk_wpm_state_changed)

// Initialize custom display
static int custom_display_init(const struct device *dev) {
    create_display_widgets();
    update_display_content();
    return 0;
}

SYS_INIT(custom_display_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);