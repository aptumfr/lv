#pragma once

/**
 * @file draw_arc.hpp
 * @brief Wrapper for LVGL arc drawing
 */

#include <lvgl.h>
#include "layer.hpp"
#include "primitives.hpp"

namespace lv {

/**
 * @brief Fluent wrapper for lv_draw_arc_dsc_t
 *
 * Angles: 0° is 3 o'clock, 90° is 6 o'clock (clockwise).
 *
 * Example:
 * ```cpp
 * lv::ArcDsc dsc;
 * dsc.center(100, 100)
 *    .radius(50)
 *    .start_angle(0).end_angle(270)
 *    .color(lv::color::blue())
 *    .width(10)
 *    .rounded();
 * lv::draw::arc(layer, dsc);
 * ```
 */
class ArcDsc {
    lv_draw_arc_dsc_t m_dsc{};

public:
    ArcDsc() noexcept {
        lv_draw_arc_dsc_init(&m_dsc);
    }

    /// Initialize from an object's styles
    ArcDsc(lv_obj_t* obj, lv_part_t part) noexcept {
        lv_draw_arc_dsc_init(&m_dsc);
        lv_obj_init_draw_arc_dsc(obj, part, &m_dsc);
    }

    /// Create from object's styles
    [[nodiscard]] static ArcDsc from_obj(lv_obj_t* obj, lv_part_t part = LV_PART_MAIN) noexcept {
        return ArcDsc(obj, part);
    }

    [[nodiscard]] lv_draw_arc_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_arc_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_arc_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Geometry ====================

    /// Set center point
    ArcDsc& center(int32_t x, int32_t y) noexcept {
        m_dsc.center.x = x;
        m_dsc.center.y = y;
        return *this;
    }

    /// Set center point
    ArcDsc& center(lv_point_t pt) noexcept {
        m_dsc.center = pt;
        return *this;
    }

    /// Set outer radius
    ArcDsc& radius(uint16_t r) noexcept {
        m_dsc.radius = r;
        return *this;
    }

    /// Set start angle (0° = 3 o'clock, clockwise)
    ArcDsc& start_angle(lv_value_precise_t angle) noexcept {
        m_dsc.start_angle = angle;
        return *this;
    }

    /// Set end angle
    ArcDsc& end_angle(lv_value_precise_t angle) noexcept {
        m_dsc.end_angle = angle;
        return *this;
    }

    /// Set both angles at once
    ArcDsc& angles(lv_value_precise_t start, lv_value_precise_t end) noexcept {
        m_dsc.start_angle = start;
        m_dsc.end_angle = end;
        return *this;
    }

    // ==================== Appearance ====================

    /// Set arc color
    ArcDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set arc width (thickness)
    ArcDsc& width(int32_t w) noexcept {
        m_dsc.width = w;
        return *this;
    }

    /// Set opacity
    ArcDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    /// Make arc ends rounded
    ArcDsc& rounded(bool r = true) noexcept {
        m_dsc.rounded = r ? 1 : 0;
        return *this;
    }

    /// Use an image instead of solid color
    ArcDsc& img_src(const void* src) noexcept {
        m_dsc.img_src = src;
        return *this;
    }
};

namespace draw {

/// Draw an arc
inline void arc(lv_layer_t* layer, const ArcDsc& dsc) noexcept {
    lv_draw_arc(layer, dsc.get());
}

/// Convenience: draw a simple arc
inline void arc(lv_layer_t* layer,
                int32_t center_x, int32_t center_y,
                uint16_t radius,
                lv_value_precise_t start_angle, lv_value_precise_t end_angle,
                lv_color_t color, int32_t width = 1) noexcept {
    ArcDsc dsc;
    dsc.center(center_x, center_y)
       .radius(radius)
       .angles(start_angle, end_angle)
       .color(color)
       .width(width);
    lv_draw_arc(layer, dsc.get());
}

/// Get the area that would be invalidated when an arc's angle changes
inline void arc_get_area(int32_t x, int32_t y, uint16_t radius,
                         lv_value_precise_t start_angle, lv_value_precise_t end_angle,
                         int32_t w, bool rounded, lv_area_t* area) noexcept {
    lv_draw_arc_get_area(x, y, radius, start_angle, end_angle, w, rounded, area);
}

} // namespace draw

} // namespace lv
