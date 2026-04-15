#pragma once

/**
 * @file bar.hpp
 * @brief Zero-cost wrapper for LVGL bar widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Bar widget wrapper
 *
 * A horizontal or vertical bar for showing progress or levels.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Bar : public ObjectView,
            public ObjectMixin<Bar>,
            public EventMixin<Bar>,
            public StyleMixin<Bar> {
public:
    constexpr Bar() noexcept : ObjectView(nullptr) {}
    constexpr Bar(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_bar_class; }

    [[nodiscard]] static Bar create(lv_obj_t* parent) {
        return Bar(wrap, lv_bar_create(parent));
    }
    [[nodiscard]] static Bar create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Value ====================

    /// Set current value
    Bar& value(int32_t val) noexcept {
        lv_bar_set_value(m_obj, val, LV_ANIM_OFF);
        return *this;
    }

    /// Set current value with animation
    Bar& value_anim(int32_t val) noexcept {
        lv_bar_set_value(m_obj, val, LV_ANIM_ON);
        return *this;
    }

    /// Get current value
    [[nodiscard]] int32_t value() const noexcept {
        return lv_bar_get_value(m_obj);
    }

    /// Set start value (for range mode)
    Bar& start_value(int32_t val) noexcept {
        lv_bar_set_start_value(m_obj, val, LV_ANIM_OFF);
        return *this;
    }

    /// Set start value with animation
    Bar& start_value_anim(int32_t val) noexcept {
        lv_bar_set_start_value(m_obj, val, LV_ANIM_ON);
        return *this;
    }

    /// Get start value
    [[nodiscard]] int32_t start_value() const noexcept {
        return lv_bar_get_start_value(m_obj);
    }

    /// Set value range
    Bar& range(int32_t min, int32_t max) noexcept {
        lv_bar_set_range(m_obj, min, max);
        return *this;
    }

    /// Get minimum value
    [[nodiscard]] int32_t min_value() const noexcept {
        return lv_bar_get_min_value(m_obj);
    }

    /// Get maximum value
    [[nodiscard]] int32_t max_value() const noexcept {
        return lv_bar_get_max_value(m_obj);
    }

    // ==================== Mode ====================

    /// Set bar mode
    Bar& mode(lv_bar_mode_t m) noexcept {
        lv_bar_set_mode(m_obj, m);
        return *this;
    }

    /// Normal mode (from min to value)
    Bar& mode_normal() noexcept {
        return mode(LV_BAR_MODE_NORMAL);
    }

    /// Symmetrical mode (from center)
    Bar& mode_symmetrical() noexcept {
        return mode(LV_BAR_MODE_SYMMETRICAL);
    }

    /// Range mode (from start_value to value)
    Bar& mode_range() noexcept {
        return mode(LV_BAR_MODE_RANGE);
    }

    /// Get current mode
    [[nodiscard]] lv_bar_mode_t get_mode() const noexcept {
        return lv_bar_get_mode(m_obj);
    }

    // ==================== Orientation ====================

    /// Set bar orientation
    Bar& orientation(lv_bar_orientation_t o) noexcept {
        lv_bar_set_orientation(m_obj, o);
        return *this;
    }

    /// Horizontal orientation (default)
    Bar& horizontal() noexcept {
        return orientation(LV_BAR_ORIENTATION_HORIZONTAL);
    }

    /// Vertical orientation
    Bar& vertical() noexcept {
        return orientation(LV_BAR_ORIENTATION_VERTICAL);
    }

    // ==================== Size ====================

    /// Set width
    Bar& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    /// Set height
    Bar& height(int32_t h) noexcept {
        lv_obj_set_height(m_obj, h);
        return *this;
    }

    /// Set size
    Bar& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set indicator color
    Bar& indicator_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set background color
    Bar& bar_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

    /// Set indicator background color (alias)
    Bar& indicator_bg_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator background opacity
    Bar& indicator_bg_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_bg_opa(m_obj, opa, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator background gradient color
    Bar& indicator_bg_grad_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_grad_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator background gradient direction
    Bar& indicator_bg_grad_dir(lv_grad_dir_t dir) noexcept {
        lv_obj_set_style_bg_grad_dir(m_obj, dir, LV_PART_INDICATOR);
        return *this;
    }

#if LV_USE_OBSERVER
    /// Bind bar value to an integer subject
    Bar& bind_value(lv_subject_t* subject) noexcept {
        lv_bar_bind_value(m_obj, subject);
        return *this;
    }
#endif
};

} // namespace lv
