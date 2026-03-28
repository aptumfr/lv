#pragma once
/**
 * @file smartwatch_health.hpp
 * @brief Health screen for smartwatch demo
 */

#include "smartwatch_private.hpp"

// Font/Image declarations (must be outside namespace - C symbols)
LV_FONT_DECLARE(font_space_grotesk_regular_28);
LV_FONT_DECLARE(font_space_grotesk_regular_23);
LV_FONT_DECLARE(font_space_grotesk_regular_80);
LV_IMAGE_DECLARE(image_heart_bg_large);
LV_IMAGE_DECLARE(image_heart_bg_small);
LV_IMAGE_DECLARE(image_health_off);
LV_IMAGE_DECLARE(image_health_on);

// Lottie data declarations (C symbols)
extern "C" {
    extern uint8_t lottie_ecg_wave[];
    extern size_t lottie_ecg_wave_size;
    extern uint8_t lottie_heart_small[];
    extern size_t lottie_heart_small_size;
}

namespace smartwatch {

class HealthScreen {
public:
    void create(DemoController& controller);
    [[nodiscard]] lv_obj_t* get() const { return m_screen; }

private:
    void on_gesture(lv::Event e);
    void on_button_click(lv::Event e);

    lv_obj_t* m_screen = nullptr;
    lv_obj_t* m_lottie_ecg = nullptr;
    lv_obj_t* m_heart_bg_2 = nullptr;
    lv_obj_t* m_heart_icon = nullptr;
    lv_obj_t* m_image_button = nullptr;

    DemoController* m_controller = nullptr;
    lv::Style m_main_style;
    bool m_inited = false;
};

inline void HealthScreen::create(DemoController& controller) {
    m_controller = &controller;

    if (!m_inited) {
        m_main_style.text_color(lv::colors::white())
            .bg_color(lv::rgb(0x000000))
            .bg_opa(lv::opa::_100)
            .radius(lv::kRadius::circle)
            .translate_x(SCREEN_SIZE);
        m_inited = true;
    }

    auto screen = lv::Box::create(lv::screen_active());
    screen.remove_all_styles()
        .add_style(m_main_style.get())
        .fill()
        .scrollbar_mode(lv::kScrollbarMode::off)
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble)
        .remove_flag(lv::kFlag::scrollable);
    screen.on<&HealthScreen::on_gesture>(lv::kEvent::gesture, this);
    m_screen = screen.get();

    // Heart background large
    lv::Image::create(m_screen)
        .src(&image_heart_bg_large)
        .pos(0, 2)
        .align(lv::kAlign::center)
        .add_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable);

    // Heart background small
    m_heart_bg_2 = lv::Image::create(m_screen)
        .src(&image_heart_bg_small)
        .pos(0, -23)
        .align(lv::kAlign::center)
        .add_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable)
        .get();

#if LV_USE_LOTTIE == 1
    auto lottie_ecg = lv::Lottie::create(m_screen)
        .src_data(lottie_ecg_wave, lottie_ecg_wave_size);
    lottie_ecg.remove_all_styles()
        .size(lv::kSize::content, lv::kSize::content)
        .pos(0, -60)
        .align(lv::kAlign::center)
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable)
        .add_flag(lv::kFlag::hidden);
    m_lottie_ecg = lottie_ecg.get();
#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    static uint8_t ecg_buf[410 * 176 * 4];
    lottie_ecg.buffer(410, 176, ecg_buf);
#else
    LV_DRAW_BUF_DEFINE(ecg_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888);
    lottie_ecg.draw_buf(&ecg_buf);
#endif
#endif

    // Time label
    lv::Label::create(m_screen)
        .pos(0, 25)
        .align(lv::kAlign::top_mid)
        .text("13:37")
        .text_font(&font_space_grotesk_regular_28);

    // Info container
    auto info_cont = lv::Box::create(m_screen);
    info_cont.remove_all_styles()
        .size(229, 116)
        .align(lv::kAlign::bottom_left)
        .pos(54, -100)
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable);

    lv::Label::create(info_cont)
        .text("Current")
        .text_font(&font_space_grotesk_regular_23);

    auto text_cont = lv::Box::create(info_cont);
    text_cont.remove_all_styles()
        .size(185, 71)
        .pos(0, -30)
        .align(lv::kAlign::bottom_left)
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable)
        .pad_row(0)
        .pad_column(5);

    lv::Label::create(text_cont)
        .pos(0, 18)
        .align(lv::kAlign::bottom_left)
        .text("64")
        .text_font(&font_space_grotesk_regular_80);

    lv::Label::create(text_cont)
        .pos(-3, 5)
        .align(lv::kAlign::bottom_right)
        .text("BPM")
        .text_color(lv::rgb(0xE3573D))
        .text_font(&font_space_grotesk_regular_28);

    lv::Label::create(info_cont)
        .align(lv::kAlign::bottom_left)
        .text("49bpm, 3 min ago")
        .text_font(&font_space_grotesk_regular_23);

    // Health button
    m_image_button = lv::Image::create(m_screen)
        .src(&image_health_off)
        .size(lv::kSize::content, lv::kSize::content)
        .align(lv::kAlign::bottom_right)
        .pos(-40, -40)
        .add_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable)
        .get();

#if LV_USE_LOTTIE == 1
    auto heart_icon = lv::Lottie::create(m_screen)
        .src_data(lottie_heart_small, lottie_heart_small_size);
    heart_icon.size(lv::kSize::content, lv::kSize::content)
        .pos(0, -30)
        .align(lv::kAlign::center)
        .add_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable);
    m_heart_icon = heart_icon.get();
#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    static uint8_t heart_buf[90 * 77 * 4];
    heart_icon.buffer(90, 77, heart_buf);
#else
    LV_DRAW_BUF_DEFINE(heart_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888);
    heart_icon.draw_buf(&heart_buf);
#endif
#endif

    // Click container
    auto click_cont = lv::Box::create(m_screen);
    click_cont.remove_all_styles()
        .size(63, 65)
        .remove_flag(lv::kFlag::scrollable);
    click_cont.align_to(lv::ObjectView(m_image_button), lv::kAlign::top_left, 5, 5);
    click_cont.on_click<&HealthScreen::on_button_click>(this);
}

// on_gesture implementation deferred to smartwatch_gestures.hpp
// to avoid incomplete type issues with forward-declared screen classes

inline void HealthScreen::on_button_click(lv::Event /*e*/) {
#if LV_USE_LOTTIE == 1
    auto lottie_ecg = lv::ref(m_lottie_ecg);
    auto heart_bg_2 = lv::ref(m_heart_bg_2);
    auto heart_icon = lv::ref(m_heart_icon);

    if (lottie_ecg.has_flag(lv::kFlag::hidden)) {
        lottie_ecg.remove_flag(lv::kFlag::hidden);
        heart_bg_2.add_flag(lv::kFlag::hidden);
        heart_icon.add_flag(lv::kFlag::hidden);
        lv::Image(lv::wrap, m_image_button).src(&image_health_on);
    } else {
        lottie_ecg.add_flag(lv::kFlag::hidden);
        heart_bg_2.remove_flag(lv::kFlag::hidden);
        heart_icon.remove_flag(lv::kFlag::hidden);
        lv::Image(lv::wrap, m_image_button).src(&image_health_off);
    }
#endif
}

} // namespace smartwatch
