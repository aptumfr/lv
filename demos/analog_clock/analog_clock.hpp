#pragma once

/**
 * @file analog_clock.hpp
 * @brief Analog clock demo - extracted from NXP Smartwatch
 *
 * A complete analog clock face with all UI elements from the original
 * NXP smartwatch S2_Watch_Analog screen.
 */

#include <lv/lv.hpp>
#include <array>
#include <ctime>

// Forward declarations for image assets
extern "C" {
extern const lv_image_dsc_t ui_img_img_bg_analog_png;
extern const lv_image_dsc_t ui_img_img_clockwise_hour_png;
extern const lv_image_dsc_t ui_img_img_clockwise_min_png;
extern const lv_image_dsc_t ui_img_img_clockwise_sec_png;
extern const lv_image_dsc_t ui_img_icn_flash_png;
extern const lv_image_dsc_t ui_img_icn_sport_png;
extern const lv_image_dsc_t ui_img_text_sport_png;
extern const lv_image_dsc_t ui_img_text_message_png;
extern const lv_image_dsc_t ui_img_icn_message_png;
extern const lv_image_dsc_t ui_img_icn_step_png;
extern const lv_image_dsc_t ui_img_icn_weather_1_png;
extern const lv_image_dsc_t ui_img_img_nxp_png;
extern const lv_image_dsc_t ui_img_img_menu_png;
extern const lv_font_t ui_font_medium_font;
}

namespace analog_clock {

/**
 * @brief Analog clock demo with all original NXP smartwatch elements
 *
 * Features:
 * - Dial background image
 * - Rotating hour, minute, second hands
 * - Battery indicator (arc + icon + label)
 * - Step counter (arc + icon + label)
 * - Date, weekday, temperature labels
 * - Weather icon
 * - Sport and Messages indicators
 * - NXP logo
 * - Side menu panel
 */
class AnalogClockDemo {
    // Background
    lv::Image m_bg;

    // Clock hands
    lv::Image m_second_hand;
    lv::Image m_hour_hand;
    lv::Image m_minute_hand;

    // Container for overlay elements
    lv::Box m_anim_container;

    // Battery section (top)
    lv::Label m_power_label;
    lv::Image m_flash_icon;
    lv::Arc m_battery_arc;

    // Sport section (top right)
    lv::Image m_sport_icon;
    lv::Image m_sport_label_img;

    // Messages section (bottom right)
    lv::Image m_message_label_img;
    lv::Image m_message_icon;

    // Step counter (bottom)
    lv::Image m_step_icon;
    lv::Label m_step_count_label;
    lv::Arc m_step_arc;

    // Weather section (left)
    lv::Image m_weather_icon;
    lv::Label m_degree_label;

    // Date section (upper left)
    lv::Label m_date_label;
    lv::Label m_week_label;

    // NXP logo (right side)
    lv::Image m_nxp_logo;

    // Side menu panel
    lv::Box m_menu_panel;
    lv::Box m_menu_knob;

    // RAII timer - automatically deleted in destructor
    lv::Timer m_timer{};

    // Hand pivot points (from image dimensions)
    static constexpr int kHourPivotX = 9;
    static constexpr int kHourPivotY = 98;
    static constexpr int kMinPivotX = 9;
    static constexpr int kMinPivotY = 157;
    static constexpr int kSecPivotX = 16;
    static constexpr int kSecPivotY = 156;

public:
    void create() {
        auto screen = lv::screen_active();

        // Black background, no scrolling
        screen.bg_color(lv::colors::black())
              .bg_opa(lv::opa::cover)
              .remove_flag(lv::kFlag::scrollable);

        // Dial background
        m_bg = lv::Image::create(screen)
            .src(&ui_img_img_bg_analog_png)
            .align(lv::kAlign::center);

        // Second hand (created early for z-order, behind hour/min)
        m_second_hand = lv::Image::create(screen)
            .src(&ui_img_img_clockwise_sec_png)
            .size(31, 180)
            .align(lv::kAlign::center, 0, -66)
            .pivot(kSecPivotX, kSecPivotY)
            .rotation(0);

        // Transparent container for overlay elements (392x395)
        m_anim_container = lv::Box::create(screen)
            .size(392, 395)
            .align(lv::kAlign::center)
            .remove_flag(lv::kFlag::scrollable)
            .bg_opa(lv::opa::transp)
            .border_width(0);

        create_battery_section();
        create_sport_section();
        create_messages_section();
        create_step_section();
        create_weather_section();
        create_date_section();
        create_nxp_logo();

        // Hour hand
        m_hour_hand = lv::Image::create(screen)
            .src(&ui_img_img_clockwise_hour_png)
            .size(18, 98)
            .align(lv::kAlign::center, 0, -50)
            .pivot(kHourPivotX, kHourPivotY)
            .rotation(0);

        // Minute hand
        m_minute_hand = lv::Image::create(screen)
            .src(&ui_img_img_clockwise_min_png)
            .size(18, 157)
            .align(lv::kAlign::center, 1, -80)
            .pivot(kMinPivotX, kMinPivotY)
            .rotation(0);

        create_menu_panel(screen);

        // Bring battery label and icon to front
        m_power_label.move_foreground();
        m_flash_icon.move_foreground();

        // Timer for clock updates (every 100ms for smooth second hand)
        // Uses type-safe member function callback with zero-cost trampoline
        m_timer = lv::Timer::create<&AnalogClockDemo::update_time>(100, this);

        // Initial update
        update_time();
    }

    void destroy() {
        // Release the current timer (move-assign an empty Timer)
        m_timer = lv::Timer{};
    }

private:
    void create_battery_section() {
        // Battery percentage label
        m_power_label = lv::Label::create(m_anim_container)
            .text("86%")
            .align(lv::kAlign::center, 9, -152)
            .text_color(lv::colors::white())
            .text_font(&ui_font_medium_font);

        // Flash/battery icon
        m_flash_icon = lv::Image::create(m_anim_container)
            .src(&ui_img_icn_flash_png)
            .align(lv::kAlign::center, -26, -152)
            .image_recolor(lv::rgb(0x1099E6))
            .image_recolor_opa(lv::opa::cover);

        // Battery arc (top area)
        m_battery_arc = lv::Arc::create(m_anim_container)
            .size(370, 370)
            .align(lv::kAlign::center)
            .remove_flag(lv::kFlag::clickable)
            .range(0, 100)
            .value(80)
            .bg_angles(0, 50)
            .rotation(245)
            .arc_color(lv::rgb(0x4A4C4A))
            .indicator_color(lv::rgb(0x1099E6))
            .arc_rounded(true)
            .indicator_rounded(true)
            .knob_bg_opa(lv::opa::transp);
    }

    void create_sport_section() {
        // Sport icon
        m_sport_icon = lv::Image::create(m_anim_container)
            .src(&ui_img_icn_sport_png)
            .align(lv::kAlign::center, 130, -77);

        // "SPORT" text image
        m_sport_label_img = lv::Image::create(m_anim_container)
            .src(&ui_img_text_sport_png)
            .align(lv::kAlign::center, 155, -94);
    }

    void create_messages_section() {
        // "MESSAGES" text image
        m_message_label_img = lv::Image::create(m_anim_container)
            .src(&ui_img_text_message_png)
            .align(lv::kAlign::center, 145, 101);

        // Message/notification icon
        m_message_icon = lv::Image::create(m_anim_container)
            .src(&ui_img_icn_message_png)
            .align(lv::kAlign::center, 129, 81);
    }

    void create_step_section() {
        // Step icon
        m_step_icon = lv::Image::create(m_anim_container)
            .src(&ui_img_icn_step_png)
            .align(lv::kAlign::center, -24, 150)
            .image_recolor(lv::rgb(0xE35515))
            .image_recolor_opa(lv::opa::cover);

        // Step count label
        m_step_count_label = lv::Label::create(m_anim_container)
            .text("1526")
            .align(lv::kAlign::center, 13, 152)
            .text_color(lv::colors::white())
            .text_font(&ui_font_medium_font);

        // Step arc (bottom area)
        m_step_arc = lv::Arc::create(m_anim_container)
            .size(370, 370)
            .align(lv::kAlign::center)
            .range(0, 100)
            .value(80)
            .bg_angles(0, 50)
            .rotation(65)
            .arc_color(lv::rgb(0x4A4C4A))
            .indicator_color(lv::rgb(0xFF5D18))
            .arc_rounded(true)
            .indicator_rounded(true)
            .knob_bg_opa(lv::opa::transp);
    }

    void create_weather_section() {
        // Weather icon
        m_weather_icon = lv::Image::create(m_anim_container)
            .src(&ui_img_icn_weather_1_png)
            .align(lv::kAlign::center, -117, 30)
            .image_recolor(lv::rgb(0x7B797B))
            .image_recolor_opa(lv::opa::cover);

        // Temperature label
        m_degree_label = lv::Label::create(m_anim_container)
            .text("26\xC2\xB0")  // UTF-8 degree symbol
            .align(lv::kAlign::center, -115, 65)
            .text_color(lv::colors::white())
            .text_font(&ui_font_medium_font);
    }

    void create_date_section() {
        // Date label (MM/DD)
        m_date_label = lv::Label::create(m_anim_container)
            .text("05/15")
            .align(lv::kAlign::center, -116, -76)
            .text_color(lv::rgb(0x7B797B))
            .text_font(&ui_font_medium_font);

        // Weekday label
        m_week_label = lv::Label::create(m_anim_container)
            .text("WED")
            .align(lv::kAlign::center, -116, -51)
            .text_color(lv::colors::white())
            .text_font(&ui_font_medium_font);
    }

    void create_nxp_logo() {
        m_nxp_logo = lv::Image::create(m_anim_container)
            .src(&ui_img_img_nxp_png)
            .align(lv::kAlign::center, 130, 0);
    }

    void create_menu_panel(lv::Screen& screen) {
        // Side menu panel
        m_menu_panel = lv::Box::create(screen)
            .size(33, 124)
            .align(lv::kAlign::right_mid)
            .bg_opa(lv::opa::transp)
            .border_width(0)
            .bg_image_src(&ui_img_img_menu_png);

        // Menu knob indicator
        m_menu_knob = lv::Box::create(m_menu_panel)
            .size(8, 8)
            .align(lv::kAlign::center, -7, -48)
            .radius(8);
    }

    /// Timer callback - called every 100ms to update clock hands
    void update_time() {
        // Get current local time
        const std::time_t now = std::time(nullptr);
        const std::tm* local = std::localtime(&now);

        const int hours = local->tm_hour;
        const int minutes = local->tm_min;
        const int seconds = local->tm_sec;

        // Convert to 12-hour format for hour hand
        const int hour12 = hours % 12;

        // Calculate angles in 0.1 degree units (LVGL uses tenths of degrees)
        // Hour: 360 degrees / 12 hours = 30 degrees per hour
        // Plus minute contribution: 30 degrees / 60 minutes = 0.5 degrees per minute
        const int32_t hour_angle = (hour12 * 300) + (minutes * 5);

        // Minute: 360 degrees / 60 minutes = 6 degrees per minute
        // Plus second contribution for smooth movement
        const int32_t min_angle = (minutes * 60) + seconds;

        // Second: 360 degrees / 60 seconds = 6 degrees per second
        const int32_t sec_angle = seconds * 60;

        // Apply rotations
        m_hour_hand.rotation(hour_angle);
        m_minute_hand.rotation(min_angle);
        m_second_hand.rotation(sec_angle);

        // Update date label (MM/DD format)
        std::array<char, 16> date_buf{};
        lv::snprintf(date_buf.data(), date_buf.size(), "%02d/%02d",
                     local->tm_mon + 1, local->tm_mday);
        m_date_label.text(date_buf.data());

        // Update weekday label
        static constexpr std::array<const char*, 7> kDays = {
            "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
        };
        m_week_label.text(kDays[static_cast<size_t>(local->tm_wday)]);
    }
};

} // namespace analog_clock
