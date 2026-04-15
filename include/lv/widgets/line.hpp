#pragma once

/**
 * @file line.hpp
 * @brief Zero-cost wrapper for LVGL line widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Line widget wrapper
 *
 * Draws a line or polyline from an array of points.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Line : public ObjectView,
            public ObjectMixin<Line>,
             public EventMixin<Line>,
             public StyleMixin<Line> {
public:
    constexpr Line() noexcept : ObjectView(nullptr) {}
    constexpr Line(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_line_class; }

    [[nodiscard]] static Line create(lv_obj_t* parent) {
        return Line(wrap, lv_line_create(parent));
    }
    [[nodiscard]] static Line create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Points ====================

    /// Set points array (must remain valid while line exists)
    Line& points(const lv_point_precise_t* point_array, uint32_t point_count) noexcept {
        lv_line_set_points(m_obj, point_array, point_count);
        return *this;
    }

    /// Set points from lv_point_t array
    Line& points(const lv_point_t* point_array, uint32_t point_count) noexcept {
        // Note: lv_point_t and lv_point_precise_t may differ based on LV_USE_FLOAT
        lv_line_set_points(m_obj, reinterpret_cast<const lv_point_precise_t*>(point_array), point_count);
        return *this;
    }

    // ==================== Appearance ====================

    /// Invert Y axis (0 at bottom instead of top)
    Line& y_invert(bool enable) noexcept {
        lv_line_set_y_invert(m_obj, enable);
        return *this;
    }

    /// Check if Y is inverted
    [[nodiscard]] bool y_invert() const noexcept {
        return lv_line_get_y_invert(m_obj);
    }

    // ==================== Style Shortcuts ====================

    /// Set line color
    Line& color(lv_color_t c) noexcept {
        lv_obj_set_style_line_color(m_obj, c, 0);
        return *this;
    }

    /// Set line stroke width (the drawn line thickness, NOT the widget's
    /// bounding box width — use the inherited width()/size() from
    /// ObjectMixin for the latter).
    Line& line_width(int32_t w) noexcept {
        lv_obj_set_style_line_width(m_obj, w, 0);
        return *this;
    }

    /// Set rounded line ends
    Line& rounded(bool enable = true) noexcept {
        lv_obj_set_style_line_rounded(m_obj, enable, 0);
        return *this;
    }

    /// Set line opacity
    Line& line_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_line_opa(m_obj, opa, 0);
        return *this;
    }
};

} // namespace lv
