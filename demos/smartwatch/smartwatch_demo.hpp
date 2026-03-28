#pragma once
/**
 * @file smartwatch_demo.hpp
 * @brief Main smartwatch demo class - C++ port
 *
 * Architecture:
 * - DemoController owns all shared UI elements and screen instances
 * - No global variables - all cross-screen communication via controller
 * - Screens receive DemoController reference in create()
 */

#include <lv/lv.hpp>
#include "smartwatch_private.hpp"
#include "smartwatch_control.hpp"
#include "smartwatch_weather.hpp"
#include "smartwatch_health.hpp"
#include "smartwatch_sports.hpp"
#include "smartwatch_music.hpp"

// Font declarations
LV_FONT_DECLARE(font_roboto_serif_bold_164);
LV_FONT_DECLARE(font_inter_regular_24);
LV_FONT_DECLARE(font_inter_bold_24);

// Image declarations
LV_IMAGE_DECLARE(image_sun_icon);

namespace smartwatch {

/**
 * @brief Controller owning all shared UI elements and screen instances
 *
 * This replaces the previous global variables (g_arc_cont, g_main_arc, etc.)
 * with proper C++ encapsulation. Screens access shared elements via controller.
 */
class DemoController {
public:
    // Shared UI element accessors
    [[nodiscard]] lv_obj_t* arc_cont() const { return m_arc_cont; }
    [[nodiscard]] lv_obj_t* main_arc() const { return m_main_arc; }
    [[nodiscard]] lv_obj_t* overlay() const { return m_overlay; }

    // Screen accessors
    [[nodiscard]] ControlScreen& control_screen() { return m_control_screen; }
    [[nodiscard]] WeatherScreen& weather_screen() { return m_weather_screen; }
    [[nodiscard]] HealthScreen& health_screen() { return m_health_screen; }
    [[nodiscard]] SportsScreen& sports_screen() { return m_sports_screen; }
    [[nodiscard]] MusicScreen& music_screen() { return m_music_screen; }

    // Set shared UI elements (called during demo creation)
    void set_arc_cont(lv_obj_t* obj) { m_arc_cont = obj; }
    void set_main_arc(lv_obj_t* obj) { m_main_arc = obj; }
    void set_overlay(lv_obj_t* obj) { m_overlay = obj; }

private:
    // Shared UI elements
    lv_obj_t* m_arc_cont = nullptr;
    lv_obj_t* m_main_arc = nullptr;
    lv_obj_t* m_overlay = nullptr;

    // Screen instances (owned by controller)
    ControlScreen m_control_screen;
    WeatherScreen m_weather_screen;
    HealthScreen m_health_screen;
    SportsScreen m_sports_screen;
    MusicScreen m_music_screen;
};

/**
 * @brief Main smartwatch demo class
 *
 * Recommended screen size: 384x384
 */
class SmartwatchDemo {
public:
    void create();

    // Access controller for gesture handlers
    [[nodiscard]] DemoController& controller() { return m_controller; }

private:
    static constexpr int MASK_WIDTH = 200;
    static constexpr int MASK_HEIGHT = 130;

    void generate_mask(lv_draw_buf_t* mask, int32_t w, int32_t h, const char* txt);

    void on_home_gesture(lv::Event e);

    DemoController m_controller;
    lv_obj_t* m_home_screen = nullptr;
    lv_obj_t* m_label_hour = nullptr;
    lv_obj_t* m_label_minute = nullptr;

    lv_theme_t* m_theme_original = nullptr;
    lv::Style m_main_style;
    bool m_inited = false;
};

// Main demo implementation
inline void SmartwatchDemo::create() {
#if LV_USE_LOTTIE == 0
    LV_LOG_WARN("Enable LV_USE_LOTTIE to play lottie animations in the smartwatch demo");
#endif

    // Use the simple theme
    auto display = lv::Display::get_default();
    if (!(display.width() == display.height() && display.height() == SCREEN_SIZE)) {
        LV_LOG_WARN("a display size of exactly 384x384 is recommended for the smartwatch demo");
    }

    m_theme_original = display.get_theme();
    lv_theme_t* theme = lv::theme_default_init(display.get(),
        lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
        true, LV_FONT_DEFAULT);
    display.set_theme(theme);

    if (!m_inited) {
        m_main_style.text_color(lv::colors::white())
            .bg_color(lv::colors::black())
            .bg_opa(lv::opa::_100)
            .radius(lv::kRadius::circle);
        m_inited = true;
    }

    auto screen = lv::screen_active();
    screen.remove_flag(lv::kFlag::scrollable)
        .bg_color(lv::rgb(0x000000));

    // Create home screen
    auto home_screen = lv::Box::create(screen);
    home_screen.remove_all_styles()
        .fill()
        .add_style(m_main_style.get())
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble)
        .remove_flag(lv::kFlag::scrollable);
    home_screen.on<&SmartwatchDemo::on_home_gesture>(lv::kEvent::gesture, this);
    m_home_screen = home_screen.get();

    // Create background arcs
    for (uint32_t i = 0; i < 12; i++) {
        auto arc = lv::Arc::create(m_home_screen);
        arc.remove_all_styles()
            .size(lv::pct(100), lv::pct(100))
            .align(lv::kAlign::center)
            .bg_start_angle(0)
            .bg_end_angle(30)
            .rotation(arc_pos(i))
            .remove_flag(lv::kFlag::clickable)
            .remove_flag(lv::kFlag::click_focusable)
            .arc_width(lv::pct(50))
            .arc_rounded(false)
            .arc_color(lv::rgb(arc_colors[i]))
            .arc_opa(30);
    }

    // Create masked hour display
    LV_DRAW_BUF_DEFINE_STATIC(mask_hour, MASK_WIDTH, MASK_HEIGHT, LV_COLOR_FORMAT_L8);
    LV_DRAW_BUF_INIT_STATIC(mask_hour);
    generate_mask(&mask_hour, MASK_WIDTH, MASK_HEIGHT, "23");

    auto label_hour = lv::Box::create(m_home_screen);
    label_hour.y(-60)
        .align(lv::kAlign::center)
        .size(MASK_WIDTH, MASK_HEIGHT)
        .bg_color(lv::rgb(0x222222))
        .bg_opa(lv::opa::cover)
        .bg_grad_color(lv::rgb(0xFFFFFF))
        .bg_grad_stop(155)
        .bg_grad_dir(lv::kGradDir::ver)
        .bitmap_mask_src(&mask_hour);
    m_label_hour = label_hour.get();

    // Create masked minute display
    LV_DRAW_BUF_DEFINE_STATIC(mask_minute, MASK_WIDTH, MASK_HEIGHT, LV_COLOR_FORMAT_L8);
    LV_DRAW_BUF_INIT_STATIC(mask_minute);
    generate_mask(&mask_minute, MASK_WIDTH, MASK_HEIGHT, "17");

    auto label_minute = lv::Box::create(m_home_screen);
    label_minute.align(lv::kAlign::center)
        .y(70)
        .size(MASK_WIDTH, MASK_HEIGHT)
        .bg_color(lv::rgb(0x333333))
        .bg_opa(lv::opa::cover)
        .bg_grad_stop(155)
        .bg_grad_color(lv::rgb(0xFFFFFF))
        .bg_grad_dir(lv::kGradDir::ver)
        .bitmap_mask_src(&mask_minute);
    m_label_minute = label_minute.get();

    // Date container
    auto date_cont = lv::Box::create(m_home_screen);
    date_cont.remove_all_styles()
        .size(lv::kSize::content, lv::kSize::content)
        .x(-40)
        .align(lv::kAlign::right_mid)
        .flex_flow(lv::kFlexFlow::column);

    lv::Label::create(date_cont)
        .text("07/12")
        .letter_space(1)
        .text_font(&font_inter_regular_24);

    lv::Label::create(date_cont)
        .text("WED")
        .letter_space(3)
        .text_font(&font_inter_bold_24);

    // Weather icon
    lv::Image::create(m_home_screen)
        .src(&image_sun_icon)
        .x(56)
        .align(lv::kAlign::left_mid);

    // Black overlay for transitions
    auto overlay = lv::Box::create(m_home_screen);
    overlay.remove_all_styles()
        .fill()
        .remove_flag(lv::kFlag::scrollable)
        .bg_color(lv::rgb(0x000000))
        .bg_opa(lv::opa::cover)
        .opa(lv::opa::transp);
    m_controller.set_overlay(overlay.get());

    // Create other screens (pass controller for cross-screen access)
    m_controller.control_screen().create(m_controller);
    m_controller.weather_screen().create(m_controller);
    m_controller.health_screen().create(m_controller);
    m_controller.sports_screen().create(m_controller);
    m_controller.music_screen().create(m_controller);

    // Arc container
    auto arc_cont = lv::Box::create(screen);
    arc_cont.remove_all_styles()
        .fill()
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::click_focusable);
    m_controller.set_arc_cont(arc_cont.get());

    // Create outer arcs
    for (uint32_t i = 0; i < 12; i++) {
        auto arc = lv::Arc::create(m_controller.arc_cont());
        arc.remove_all_styles()
            .size(lv::pct(100), lv::pct(100))
            .align(lv::kAlign::center)
            .bg_start_angle(0)
            .bg_end_angle(30)
            .rotation(arc_pos(i))
            .remove_flag(lv::kFlag::clickable)
            .remove_flag(lv::kFlag::click_focusable)
            .arc_width(27)
            .arc_rounded(false)
            .arc_color(lv::rgb(arc_colors[i]))
            .arc_opa(lv::opa::cover);
    }

    // Main rotating arc - created on top of everything for the blend effect
    auto main_arc = lv::Arc::create(screen);
    main_arc.remove_all_styles()
        .size(lv::pct(100), lv::pct(100))
        .align(lv::kAlign::center)
        .bg_start_angle(0)
        .bg_end_angle(45)
        .rotation(0)
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::click_focusable)
        .arc_width(lv::pct(50))
        .arc_rounded(false)
        .arc_color(lv::colors::white())
        .arc_opa(lv::opa::cover)
        .blend_mode(lv::kBlendMode::difference)
        .opa(lv::opa::cover);
    m_controller.set_main_arc(main_arc.get());

    // Animate the rotating arc
    lv::Anim()
        .exec_arc_rotation(lv::ObjectView(m_controller.main_arc()))
        .values(0, 360)
        .duration(30000)
        .repeat_infinite()
        .start();
}

inline void SmartwatchDemo::generate_mask(lv_draw_buf_t* mask, int32_t w, int32_t h, const char* txt) {
    auto canvas = lv::Canvas::create(lv::screen_active());
    canvas.draw_buf(mask);
    canvas.fill_bg(lv::colors::black(), lv::opa::transp);

    lv_layer_t layer;
    canvas.init_layer(&layer);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv::colors::white();
    label_dsc.align = lv::kTextAlign::center;
    label_dsc.text = txt;
    label_dsc.ofs_y = -25;
    label_dsc.font = &font_roboto_serif_bold_164;
    lv_area_t a = {0, 0, w - 1, h - 1};
    lv_draw_label(&layer, &label_dsc, &a);

    canvas.finish_layer(&layer);
    canvas.del();
}

inline void SmartwatchDemo::on_home_gesture(lv::Event /*e*/) {
    lv_dir_t dir = lv::Indev(lv::indev_active()).gesture_dir();
    if (dir == lv::kDir::bottom) {
        animate_y(m_controller.control_screen().get(), 0, 800, 200);
        animate_arc(m_controller.arc_cont(), ArcAnimation::SHRINK_DOWN, 1000, 0);
        animate_opa(m_controller.main_arc(), 0, 700, 0);
    }
    if (dir == lv::kDir::left) {
        animate_x_from(m_controller.weather_screen().get(), SCREEN_SIZE, 0, 800, 200);
        animate_arc(m_controller.arc_cont(), ArcAnimation::SHRINK_LEFT, 1000, 0);
        animate_opa(m_controller.main_arc(), 0, 700, 0);
    }
}

} // namespace smartwatch
