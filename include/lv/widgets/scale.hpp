#pragma once

/**
 * @file scale.hpp
 * @brief Zero-cost wrapper for LVGL scale widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Scale widget wrapper
 *
 * A scale/gauge with tick marks and labels.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Scale : public ObjectView,
            public ObjectMixin<Scale>,
              public EventMixin<Scale>,
              public StyleMixin<Scale> {
public:
    constexpr Scale() noexcept : ObjectView(nullptr) {}
    constexpr Scale(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_scale_class; }

    [[nodiscard]] static Scale create(lv_obj_t* parent) {
        return Scale(wrap, lv_scale_create(parent));
    }
    [[nodiscard]] static Scale create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Mode ====================

    /// Set scale mode
    Scale& mode(lv_scale_mode_t m) noexcept {
        lv_scale_set_mode(m_obj, m);
        return *this;
    }

    /// Horizontal bottom mode
    Scale& mode_horizontal_bottom() noexcept {
        return mode(LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    }

    /// Horizontal top mode
    Scale& mode_horizontal_top() noexcept {
        return mode(LV_SCALE_MODE_HORIZONTAL_TOP);
    }

    /// Vertical left mode
    Scale& mode_vertical_left() noexcept {
        return mode(LV_SCALE_MODE_VERTICAL_LEFT);
    }

    /// Vertical right mode
    Scale& mode_vertical_right() noexcept {
        return mode(LV_SCALE_MODE_VERTICAL_RIGHT);
    }

    /// Round inner mode (for circular scales)
    Scale& mode_round_inner() noexcept {
        return mode(LV_SCALE_MODE_ROUND_INNER);
    }

    /// Round outer mode
    Scale& mode_round_outer() noexcept {
        return mode(LV_SCALE_MODE_ROUND_OUTER);
    }

    // ==================== Range ====================

    /// Set value range
    Scale& range(int32_t min, int32_t max) noexcept {
        lv_scale_set_range(m_obj, min, max);
        return *this;
    }

    // ==================== Ticks ====================

    /// Set total tick count and major tick interval
    Scale& ticks(uint32_t total_count, uint32_t major_every) noexcept {
        lv_scale_set_total_tick_count(m_obj, total_count);
        lv_scale_set_major_tick_every(m_obj, major_every);
        return *this;
    }

    /// Set total tick count
    Scale& total_tick_count(uint32_t count) noexcept {
        lv_scale_set_total_tick_count(m_obj, count);
        return *this;
    }

    /// Set major tick every N ticks
    Scale& major_tick_every(uint32_t n) noexcept {
        lv_scale_set_major_tick_every(m_obj, n);
        return *this;
    }

    // ==================== Labels ====================

    /// Enable/disable labels on major ticks
    Scale& label_show(bool show) noexcept {
        lv_scale_set_label_show(m_obj, show);
        return *this;
    }

    /// Set custom text source (array of strings for each major tick)
    Scale& text_src(const char* txt_src[]) noexcept {
        lv_scale_set_text_src(m_obj, txt_src);
        return *this;
    }

    /// Set post-draw callback for custom drawing
    Scale& post_draw(bool enable) noexcept {
        lv_scale_set_post_draw(m_obj, enable);
        return *this;
    }

    // ==================== Sections ====================

    /// Add a section (colored range)
    [[nodiscard]] lv_scale_section_t* add_section() noexcept {
        return lv_scale_add_section(m_obj);
    }

    /// Set section range
    Scale& section_range(lv_scale_section_t* section, int32_t min, int32_t max) noexcept {
        lv_scale_section_set_range(section, min, max);
        return *this;
    }

    /// Set section style
    Scale& section_style(lv_scale_section_t* section, lv_part_t part, lv_style_t* style) noexcept {
        lv_scale_section_set_style(section, part, style);
        return *this;
    }

    // ==================== Round Scale ====================

    /// Set angle range for round scale
    Scale& angle_range(uint32_t angle) noexcept {
        lv_scale_set_angle_range(m_obj, angle);
        return *this;
    }

    /// Set rotation for round scale
    Scale& rotation(int32_t angle) noexcept {
        lv_scale_set_rotation(m_obj, angle);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Scale& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
