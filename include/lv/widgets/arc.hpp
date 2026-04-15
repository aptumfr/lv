#pragma once

/**
 * @file arc.hpp
 * @brief Zero-cost wrapper for LVGL arc widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Arc widget wrapper
 *
 * A circular arc that can be used as a meter, gauge, or input.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Arc : public ObjectView,
            public ObjectMixin<Arc>,
            public EventMixin<Arc>,
            public StyleMixin<Arc> {
public:
    constexpr Arc() noexcept : ObjectView(nullptr) {}
    constexpr Arc(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_arc_class; }

    [[nodiscard]] static Arc create(lv_obj_t* parent) {
        return Arc(wrap, lv_arc_create(parent));
    }
    [[nodiscard]] static Arc create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Angles ====================

    /// Set start angle (0 = right, 90 = bottom)
    Arc& start_angle(int32_t angle) noexcept {
        lv_arc_set_start_angle(m_obj, angle);
        return *this;
    }

    /// Set end angle
    Arc& end_angle(int32_t angle) noexcept {
        lv_arc_set_end_angle(m_obj, angle);
        return *this;
    }

    /// Set both start and end angles
    Arc& angles(int32_t start, int32_t end) noexcept {
        lv_arc_set_angles(m_obj, start, end);
        return *this;
    }

    /// Set background start angle
    Arc& bg_start_angle(int32_t angle) noexcept {
        lv_arc_set_bg_start_angle(m_obj, angle);
        return *this;
    }

    /// Set background end angle
    Arc& bg_end_angle(int32_t angle) noexcept {
        lv_arc_set_bg_end_angle(m_obj, angle);
        return *this;
    }

    /// Set both background angles
    Arc& bg_angles(int32_t start, int32_t end) noexcept {
        lv_arc_set_bg_angles(m_obj, start, end);
        return *this;
    }

    // ==================== Value ====================

    /// Set current value
    Arc& value(int32_t val) noexcept {
        lv_arc_set_value(m_obj, val);
        return *this;
    }

    /// Get current value
    [[nodiscard]] int32_t value() const noexcept {
        return lv_arc_get_value(m_obj);
    }

    /// Set value range
    Arc& range(int32_t min, int32_t max) noexcept {
        lv_arc_set_range(m_obj, min, max);
        return *this;
    }

    /// Get minimum value
    [[nodiscard]] int32_t min_value() const noexcept {
        return lv_arc_get_min_value(m_obj);
    }

    /// Get maximum value
    [[nodiscard]] int32_t max_value() const noexcept {
        return lv_arc_get_max_value(m_obj);
    }

    // ==================== Mode ====================

    /// Set arc mode
    Arc& mode(lv_arc_mode_t m) noexcept {
        lv_arc_set_mode(m_obj, m);
        return *this;
    }

    /// Normal mode (clockwise)
    Arc& mode_normal() noexcept {
        return mode(LV_ARC_MODE_NORMAL);
    }

    /// Reverse mode (counter-clockwise)
    Arc& mode_reverse() noexcept {
        return mode(LV_ARC_MODE_REVERSE);
    }

    /// Symmetrical mode (grow from center)
    Arc& mode_symmetrical() noexcept {
        return mode(LV_ARC_MODE_SYMMETRICAL);
    }

    /// Get current mode
    [[nodiscard]] lv_arc_mode_t get_mode() const noexcept {
        return lv_arc_get_mode(m_obj);
    }

    // ==================== Appearance ====================

    /// Set rotation offset for the whole arc
    Arc& rotation(int32_t angle) noexcept {
        lv_arc_set_rotation(m_obj, angle);
        return *this;
    }

    /// Get rotation
    [[nodiscard]] int32_t rotation() const noexcept {
        return lv_arc_get_rotation(m_obj);
    }

    /// Set knob offset
    Arc& knob_offset(int32_t offset) noexcept {
        lv_arc_set_knob_offset(m_obj, offset);
        return *this;
    }

    /// Rotate an object to align with the arc's current angle
    /// @param obj_to_rotate Object to rotate
    /// @param r_offset Radius offset from the arc
    void rotate_obj_to_angle(lv_obj_t* obj_to_rotate, int32_t r_offset = 0) const noexcept {
        lv_arc_rotate_obj_to_angle(m_obj, obj_to_rotate, r_offset);
    }

    /// Rotate an object to align with the arc's current angle
    void rotate_obj_to_angle(ObjectView obj_to_rotate, int32_t r_offset = 0) const noexcept {
        lv_arc_rotate_obj_to_angle(m_obj, obj_to_rotate.get(), r_offset);
    }

    // ==================== Size ====================

    /// Set size (width and height, arcs are typically square)
    Arc& size(int32_t s) noexcept {
        lv_obj_set_size(m_obj, s, s);
        return *this;
    }

    /// Set width and height separately
    Arc& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set indicator (foreground) color
    Arc& indicator_color(lv_color_t color) noexcept {
        lv_obj_set_style_arc_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set background arc color
    Arc& arc_color(lv_color_t color) noexcept {
        lv_obj_set_style_arc_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

    /// Set indicator width
    Arc& indicator_width(int32_t w) noexcept {
        lv_obj_set_style_arc_width(m_obj, w, LV_PART_INDICATOR);
        return *this;
    }

    /// Set background arc width
    Arc& arc_width(int32_t w) noexcept {
        lv_obj_set_style_arc_width(m_obj, w, LV_PART_MAIN);
        return *this;
    }

    /// Set knob color
    Arc& knob_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_KNOB);
        return *this;
    }

    /// Set knob background color (alias for knob_color)
    Arc& knob_bg_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_KNOB);
        return *this;
    }

    /// Set knob background opacity
    Arc& knob_bg_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_bg_opa(m_obj, opa, LV_PART_KNOB);
        return *this;
    }

    /// Set indicator opacity
    Arc& indicator_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_arc_opa(m_obj, opa, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator arc rounded
    Arc& indicator_rounded(bool rounded) noexcept {
        lv_obj_set_style_arc_rounded(m_obj, rounded, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator arc rounded (alias)
    Arc& indicator_arc_rounded(bool rounded) noexcept {
        lv_obj_set_style_arc_rounded(m_obj, rounded, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator color (alias with arc prefix for consistency)
    Arc& indicator_arc_color(lv_color_t color) noexcept {
        lv_obj_set_style_arc_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator opacity (alias with arc prefix)
    Arc& indicator_arc_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_arc_opa(m_obj, opa, LV_PART_INDICATOR);
        return *this;
    }

    /// Set indicator width (alias with arc prefix)
    Arc& indicator_arc_width(int32_t w) noexcept {
        lv_obj_set_style_arc_width(m_obj, w, LV_PART_INDICATOR);
        return *this;
    }

    /// Set blend mode
    Arc& blend_mode(lv_blend_mode_t mode) noexcept {
        lv_obj_set_style_blend_mode(m_obj, mode, LV_PART_MAIN);
        return *this;
    }

#if LV_USE_OBSERVER
    /// Bind arc value to an integer subject
    Arc& bind_value(lv_subject_t* subject) noexcept {
        lv_arc_bind_value(m_obj, subject);
        return *this;
    }
#endif
};

} // namespace lv
