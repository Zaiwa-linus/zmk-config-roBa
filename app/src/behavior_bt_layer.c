/*
 * BT Profile with Layer Behavior
 *
 * A behavior that combines BT profile selection with automatic layer switching.
 * - Profile 1: Layer 1 ON, Layer 2 OFF
 * - Profile 2: Layer 2 ON, Layer 1 OFF
 * - Others: Layer 1 OFF, Layer 2 OFF
 */

#define DT_DRV_COMPAT zmk_behavior_bt_layer

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/ble.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(behavior_bt_layer, CONFIG_ZMK_LOG_LEVEL);

#define LAYER_1 1
#define LAYER_2 2

static void update_layers_for_profile(uint8_t profile_index) {
    LOG_INF("Updating layers for profile %d", profile_index);

    switch (profile_index) {
        case 1:
            zmk_keymap_layer_deactivate(LAYER_2);
            zmk_keymap_layer_activate(LAYER_1);
            break;
        case 2:
            zmk_keymap_layer_deactivate(LAYER_1);
            zmk_keymap_layer_activate(LAYER_2);
            break;
        default:
            zmk_keymap_layer_deactivate(LAYER_1);
            zmk_keymap_layer_deactivate(LAYER_2);
            break;
    }
}

static int behavior_bt_layer_init(const struct device *dev) {
    return 0;
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    uint8_t profile_index = binding->param1;

    LOG_INF("BT Layer behavior pressed: selecting profile %d", profile_index);

    /* Select the BT profile */
    int ret = zmk_ble_prof_select(profile_index);
    if (ret < 0) {
        LOG_ERR("Failed to select BT profile %d: %d", profile_index, ret);
        return ret;
    }

    /* Update layers based on the profile */
    update_layers_for_profile(profile_index);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_bt_layer_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_bt_layer_init, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_bt_layer_driver_api);
