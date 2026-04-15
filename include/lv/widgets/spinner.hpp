#pragma once

/**
 * @file spinner.hpp
 * @brief Zero-cost wrapper for LVGL spinner widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Spinner widget wrapper
 *
 * A spinning/loading indicator animation.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Spinner : public ObjectView,
            public ObjectMixin<Spinner>,
                public EventMixin<Spinner>,
                public StyleMixin<Spinner> {
public:
    constexpr Spinner() noexcept : ObjectView(nullptr) {}
    constexpr Spinner(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_spinner_class; }

    [[nodiscard]] static Spinner create(lv_obj_t* parent, uint32_t anim_time = 1000, uint32_t arc_length = 60) {
        auto spinner = Spinner(wrap, lv_spinner_create(parent));
        lv_spinner_set_anim_params(spinner.get(), anim_time, arc_length);
        return spinner;
    }
    [[nodiscard]] static Spinner create(ObjectView parent, uint32_t anim_time = 1000, uint32_t arc_length = 60) {
        return create(parent.get(), anim_time, arc_length);
    }

    // ==================== Animation ====================

    /// Set animation parameters
    Spinner& anim_params(uint32_t time_ms, uint32_t arc_length) noexcept {
        lv_spinner_set_anim_params(m_obj, time_ms, arc_length);
        return *this;
    }

    // ==================== Size ====================

    /// Set size (spinners are typically square)
    Spinner& size(int32_t s) noexcept {
        lv_obj_set_size(m_obj, s, s);
        return *this;
    }

    /// Set width and height separately
    Spinner& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set indicator (spinning part) color
    Spinner& indicator_color(lv_color_t color) noexcept {
        lv_obj_set_style_arc_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set background arc color
    Spinner& arc_color(lv_color_t color) noexcept {
        lv_obj_set_style_arc_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

    /// Set indicator width
    Spinner& indicator_width(int32_t w) noexcept {
        lv_obj_set_style_arc_width(m_obj, w, LV_PART_INDICATOR);
        return *this;
    }

    /// Set background arc width
    Spinner& arc_width(int32_t w) noexcept {
        lv_obj_set_style_arc_width(m_obj, w, LV_PART_MAIN);
        return *this;
    }
};

} // namespace lv
