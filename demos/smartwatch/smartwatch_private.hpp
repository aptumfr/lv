#pragma once
/**
 * @file smartwatch_private.hpp
 * @brief Shared definitions for smartwatch demo
 *
 * Architecture:
 * - DemoController owns all shared UI elements and screen instances
 * - Screens receive DemoController reference via constructor
 * - No global variables - all cross-screen communication via controller
 */

#include <lv/lv.hpp>
#include <cstdint>

namespace smartwatch {

// Screen size
constexpr int32_t SCREEN_SIZE = 384;
constexpr int32_t TRANSITION_GAP = 50;

// Arc animation types
enum class ArcAnimation {
    SHRINK_DOWN,
    EXPAND_UP,
    SHRINK_LEFT,
    EXPAND_RIGHT
};

// Arc position helper
constexpr int32_t arc_pos(uint32_t i) {
    return static_cast<int32_t>(i) * 30 - 15;
}

// Arc colors used throughout the demo
constexpr uint32_t arc_colors[] = {
    0xB70074, // Purple
    0x792672, // Violet
    0x193E8D, // Deep Blue
    0x179DD6, // Light Blue
    0x009059, // Dark Green
    0x5FB136, // Light Green
    0xF9EE19, // Yellow
    0xF6D10E, // Ocher
    0xEF7916, // Cream
    0xEA4427, // Orange
    0xEB0F13, // Red
    0xD50059, // Garnet
};

// Forward declarations
class DemoController;
class ControlScreen;
class HealthScreen;
class MusicScreen;
class SportsScreen;
class WeatherScreen;

// Animation helpers using typed exec
inline void animate_x(lv_obj_t* obj, int32_t x, int32_t duration, int32_t delay) {
    lv::Anim()
        .exec_translate_x(lv::ObjectView(obj))
        .values(lv_obj_get_style_translate_x(obj, lv::kPart::main), x)
        .duration(duration)
        .delay(delay)
        .start();
}

inline void animate_x_from(lv_obj_t* obj, int32_t start, int32_t x, int32_t duration, int32_t delay) {
    lv::Anim()
        .exec_translate_x(lv::ObjectView(obj))
        .values(start, x)
        .duration(duration)
        .delay(delay)
        .start();
}

inline void animate_y(lv_obj_t* obj, int32_t y, int32_t duration, int32_t delay) {
    lv::Anim()
        .exec_translate_y(lv::ObjectView(obj))
        .values(lv_obj_get_style_translate_y(obj, lv::kPart::main), y)
        .duration(duration)
        .delay(delay)
        .start();
}

inline void animate_opa(lv_obj_t* obj, lv_opa_t opa, int32_t duration, int32_t delay) {
    lv::Anim()
        .exec_opa(lv::ObjectView(obj))
        .values(lv_obj_get_style_opa(obj, lv::kPart::main), opa)
        .duration(duration)
        .delay(delay)
        .start();
}

inline void animate_arc(lv_obj_t* obj, ArcAnimation animation, int32_t duration, int32_t delay) {
    auto shrink_to_down = [](void* var, int32_t v) {
        auto parent = lv::ref(static_cast<lv_obj_t*>(var));
        for (uint32_t i = 0; i < parent.child_count(); i++) {
            auto child = lv::Arc(lv::wrap, parent.child(i).get());
            int32_t orig = arc_pos(i);
            int32_t target = 75;
            int32_t final_pos = orig + ((target - orig) * v) / 100;
            int32_t thick = 27 + ((5 - 27) * v) / 100;
            auto hsv = lv::rgb_to_hsv(lv::rgb(arc_colors[i]));
            hsv.s = 100 - v;
            hsv.v = hsv.v + ((100 - hsv.v) * v) / 100;
            child.rotation(final_pos).arc_width(thick).arc_color(lv::hsv_to_rgb(hsv));
        }
    };

    auto expand_from_down = [](void* var, int32_t v) {
        auto parent = lv::ref(static_cast<lv_obj_t*>(var));
        for (uint32_t i = 0; i < parent.child_count(); i++) {
            auto child = lv::Arc(lv::wrap, parent.child(i).get());
            int32_t orig = 75;
            int32_t target = arc_pos(i);
            int32_t final_pos = orig + ((target - orig) * v) / 100;
            int32_t thick = 5 + ((27 - 5) * v) / 100;
            auto hsv = lv::rgb_to_hsv(lv::rgb(arc_colors[i]));
            hsv.s = v;
            hsv.v = 100 + ((hsv.v - 100) * v) / 100;
            child.rotation(final_pos).arc_width(thick).arc_color(lv::hsv_to_rgb(hsv));
        }
    };

    auto shrink_to_left = [](void* var, int32_t v) {
        auto parent = lv::ref(static_cast<lv_obj_t*>(var));
        for (uint32_t i = 0; i < parent.child_count(); i++) {
            auto child = lv::Arc(lv::wrap, parent.child(i).get());
            int32_t orig = arc_pos(i);
            int32_t target = 165;
            int32_t final_pos = orig + ((target - orig) * v) / 100;
            int32_t thick = 27 + ((5 - 27) * v) / 100;
            auto hsv = lv::rgb_to_hsv(lv::rgb(arc_colors[i]));
            hsv.s = 100 - v;
            hsv.v = hsv.v + ((100 - hsv.v) * v) / 100;
            child.rotation(final_pos).arc_width(thick).arc_color(lv::hsv_to_rgb(hsv));
        }
    };

    auto expand_from_left = [](void* var, int32_t v) {
        auto parent = lv::ref(static_cast<lv_obj_t*>(var));
        for (uint32_t i = 0; i < parent.child_count(); i++) {
            auto child = lv::Arc(lv::wrap, parent.child(i).get());
            int32_t orig = 165;
            int32_t target = arc_pos(i);
            int32_t final_pos = orig + ((target - orig) * v) / 100;
            int32_t thick = 5 + ((27 - 5) * v) / 100;
            auto hsv = lv::rgb_to_hsv(lv::rgb(arc_colors[i]));
            hsv.s = v;
            hsv.v = 100 + ((hsv.v - 100) * v) / 100;
            child.rotation(final_pos).arc_width(thick).arc_color(lv::hsv_to_rgb(hsv));
        }
    };

    lv_anim_exec_xcb_t exec_cb = nullptr;
    switch (animation) {
        case ArcAnimation::SHRINK_DOWN:  exec_cb = shrink_to_down; break;
        case ArcAnimation::EXPAND_UP:    exec_cb = expand_from_down; break;
        case ArcAnimation::SHRINK_LEFT:  exec_cb = shrink_to_left; break;
        case ArcAnimation::EXPAND_RIGHT: exec_cb = expand_from_left; break;
    }

    lv::Anim()
        .var(obj)
        .values(0, 100)
        .duration(duration)
        .delay(delay)
        .exec(exec_cb)
        .start();
}

} // namespace smartwatch
