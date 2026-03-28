#pragma once
/**
 * @file smartwatch_control.hpp
 * @brief Control screen for smartwatch demo
 */

#include "smartwatch_private.hpp"

// Image declarations
LV_IMAGE_DECLARE(image_mute_icon);
LV_IMAGE_DECLARE(image_dnd_icon);
LV_IMAGE_DECLARE(image_flashlight_icon);
LV_IMAGE_DECLARE(image_flight_icon);
LV_IMAGE_DECLARE(image_cellular_icon);
LV_IMAGE_DECLARE(image_repeat_icon);
LV_IMAGE_DECLARE(image_night_icon);
LV_IMAGE_DECLARE(image_brightness_icon);
LV_IMAGE_DECLARE(image_video_icon);
LV_IMAGE_DECLARE(image_battery_icon);
LV_IMAGE_DECLARE(image_wifi_icon);
LV_IMAGE_DECLARE(image_silence_icon);
LV_IMAGE_DECLARE(image_bluetooth_icon);
LV_IMAGE_DECLARE(image_qr_icon);
LV_IMAGE_DECLARE(image_settings_icon);

namespace smartwatch {

class ControlScreen {
public:
    void create(DemoController& controller);
    [[nodiscard]] lv_obj_t* get() const { return m_screen; }

private:
    void on_gesture(lv::Event e);
    void on_long_press(lv::Event e);
    static void on_scroll(lv::Event e);  // Stateless, no user_data needed

    lv_obj_t* m_screen = nullptr;
    DemoController* m_controller = nullptr;
    lv::Style m_main_style;
    bool m_inited = false;

    // Control icons
    static constexpr const lv_image_dsc_t* control_icons[] = {
        &image_mute_icon,
        &image_dnd_icon,
        &image_flashlight_icon,
        &image_flight_icon,
        &image_cellular_icon,
        &image_repeat_icon,
        &image_night_icon,
        &image_brightness_icon,
        &image_video_icon,
        &image_battery_icon,
        &image_wifi_icon,
        &image_silence_icon,
        &image_bluetooth_icon,
        &image_qr_icon,
        &image_settings_icon
    };
};

inline void ControlScreen::create(DemoController& controller) {
    m_controller = &controller;

    if (!m_inited) {
        m_main_style.text_color(lv::colors::white())
            .bg_color(lv::rgb(0x4a4a4a))
            .bg_opa(lv::opa::_100)
            .radius(lv::kRadius::circle)
            .translate_y(-SCREEN_SIZE)
            .layout(lv::kLayout::flex)
            .flex_flow(lv::kFlexFlow::column)
            .flex_main_place(lv::kFlexAlign::start)
            .flex_cross_place(lv::kFlexAlign::center)
            .flex_track_place(lv::kFlexAlign::center)
            .pad_top(80)
            .pad_bottom(0)
            .pad_row(20);
        m_inited = true;
    }

    auto screen = lv::Box::create(lv::screen_active());
    screen.remove_all_styles()
        .add_style(m_main_style.get())
        .fill()
        .scroll_dir(lv::kDir::ver)
        .scroll_snap_y(lv::kScrollSnap::start)
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble);
    screen.on<&ControlScreen::on_gesture>(lv::kEvent::gesture, this)
          .on<&ControlScreen::on_long_press>(lv::kEvent::long_pressed, this)
          .on(lv::kEvent::scroll, [](lv::Event e) { on_scroll(e); });
    m_screen = screen.get();

    // Create 5 row containers
    for (uint32_t i = 0; i < 5; i++) {
        auto inner_cont = lv::hbox(lv::ObjectView(m_screen))
            .size(345, 102)
            .space_between()
            .add_flag(lv::kFlag::snappable)
            .add_flag(lv::kFlag::scroll_on_focus);

        // Add 3 buttons to each row
        for (uint32_t j = 0; j < 3; j++) {
            auto item = lv::Box::create(inner_cont);
            item.remove_all_styles();
            constexpr lv_style_selector_t checked_sel = lv::kPart::main | static_cast<lv_style_selector_t>(lv::kState::checked);
            item.size(102, 102)
                .bg_color(lv::rgb(0xffffff))
                .bg_opa(46)
                .radius(50)
                .bg_color(lv::rgb(0x438bff), checked_sel)
                .bg_opa(lv::opa::cover, checked_sel)
                .add_flag(lv::kFlag::checkable);

            lv::Image::create(item)
                .src(control_icons[(i * 3) + j])
                .align(lv::kAlign::center)
                .remove_flag(lv::kFlag::clickable);
        }
    }

    lv::ref(m_screen).update_snap(lv::kAnim::on).scroll_by(0, -1, lv::kAnim::on);
}

// on_gesture and on_long_press implementations deferred to smartwatch_gestures.hpp
// to avoid incomplete type issues with DemoController

inline void ControlScreen::on_scroll(lv::Event e) {
    auto cont = e.target();

    lv_area_t cont_a;
    cont.get_coords(&cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    int32_t r = cont.get_height() * 7 / 10;
    uint32_t child_cnt = cont.child_count();

    for (uint32_t i = 0; i < child_cnt; i++) {
        auto child = cont.child(i);
        lv_area_t child_a;
        child.get_coords(&child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = LV_ABS(child_y_center - cont_y_center);

        // Smallest size default parameters
        int32_t icon_size = 82;
        int32_t cont_width = 262;
        int32_t img_scale = 208;

        if (diff_y < r) {
            int32_t cos_val = lv_trigo_cos((90 * diff_y) / r);
            int32_t factor = lv_pow(cos_val, 2) / LV_TRIGO_SIN_MAX / LV_TRIGO_SIN_MAX;
            icon_size = 82 + (20 * factor);
            cont_width = 262 + (83 * factor);
            img_scale = 208 + (48 * factor);
        }

        child.width(cont_width);

        for (uint32_t c = 0; c < child.child_count(); c++) {
            auto grand = child.child(c);
            grand.size(icon_size, icon_size);
            lv::Image(lv::wrap, grand.child(0).get()).scale(img_scale);
        }
    }
}

} // namespace smartwatch
