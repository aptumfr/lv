#pragma once

/**
 * @file draw_line.hpp
 * @brief Wrapper for LVGL line drawing
 */

#include <lvgl.h>
#include "layer.hpp"
#include "primitives.hpp"

namespace lv {

/**
 * @brief Fluent wrapper for lv_draw_line_dsc_t
 *
 * Example:
 * ```cpp
 * lv::LineDsc dsc;
 * dsc.p1(0, 0).p2(100, 100)
 *    .color(lv::color::red())
 *    .width(3)
 *    .round_start().round_end();
 * lv::draw::line(layer, dsc);
 * ```
 */
class LineDsc {
    lv_draw_line_dsc_t m_dsc{};

public:
    LineDsc() noexcept {
        lv_draw_line_dsc_init(&m_dsc);
    }

    /// Initialize from an object's styles
    LineDsc(lv_obj_t* obj, lv_part_t part) noexcept {
        lv_draw_line_dsc_init(&m_dsc);
        lv_obj_init_draw_line_dsc(obj, part, &m_dsc);
    }

    /// Create from object's styles
    [[nodiscard]] static LineDsc from_obj(lv_obj_t* obj, lv_part_t part = LV_PART_MAIN) noexcept {
        return LineDsc(obj, part);
    }

    [[nodiscard]] lv_draw_line_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_line_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_line_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Endpoints ====================

    /// Set first point
    LineDsc& p1(lv_value_precise_t x, lv_value_precise_t y) noexcept {
        m_dsc.p1.x = x;
        m_dsc.p1.y = y;
        return *this;
    }

    /// Set first point
    LineDsc& p1(lv_point_precise_t pt) noexcept {
        m_dsc.p1 = pt;
        return *this;
    }

    /// Set second point
    LineDsc& p2(lv_value_precise_t x, lv_value_precise_t y) noexcept {
        m_dsc.p2.x = x;
        m_dsc.p2.y = y;
        return *this;
    }

    /// Set second point
    LineDsc& p2(lv_point_precise_t pt) noexcept {
        m_dsc.p2 = pt;
        return *this;
    }

    /// Set both points at once
    LineDsc& points(lv_value_precise_t x1, lv_value_precise_t y1,
                    lv_value_precise_t x2, lv_value_precise_t y2) noexcept {
        m_dsc.p1.x = x1;
        m_dsc.p1.y = y1;
        m_dsc.p2.x = x2;
        m_dsc.p2.y = y2;
        return *this;
    }

    // ==================== Appearance ====================

    /// Set line color
    LineDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set line width (thickness)
    LineDsc& width(int32_t w) noexcept {
        m_dsc.width = w;
        return *this;
    }

    /// Set opacity
    LineDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    // ==================== Dashing ====================

    /// Set dash pattern
    LineDsc& dash(int32_t dash_width, int32_t gap) noexcept {
        m_dsc.dash_width = dash_width;
        m_dsc.dash_gap = gap;
        return *this;
    }

    /// Set dash width only
    LineDsc& dash_width(int32_t w) noexcept {
        m_dsc.dash_width = w;
        return *this;
    }

    /// Set dash gap only
    LineDsc& dash_gap(int32_t g) noexcept {
        m_dsc.dash_gap = g;
        return *this;
    }

    // ==================== Line Endings ====================

    /// Make line start rounded
    LineDsc& round_start(bool rounded = true) noexcept {
        m_dsc.round_start = rounded ? 1 : 0;
        return *this;
    }

    /// Make line end rounded
    LineDsc& round_end(bool rounded = true) noexcept {
        m_dsc.round_end = rounded ? 1 : 0;
        return *this;
    }

    /// Make both ends rounded
    LineDsc& rounded(bool rounded = true) noexcept {
        m_dsc.round_start = rounded ? 1 : 0;
        m_dsc.round_end = rounded ? 1 : 0;
        return *this;
    }

    /// Don't process line ending (optimization hint)
    LineDsc& raw_end(bool raw = true) noexcept {
        m_dsc.raw_end = raw ? 1 : 0;
        return *this;
    }
};

namespace draw {

/// Draw a line
inline void line(lv_layer_t* layer, const LineDsc& dsc) noexcept {
    lv_draw_line(layer, dsc.get());
}

/// Convenience: draw a line with just endpoints and color
inline void line(lv_layer_t* layer,
                 lv_value_precise_t x1, lv_value_precise_t y1,
                 lv_value_precise_t x2, lv_value_precise_t y2,
                 lv_color_t color, int32_t width = 1) noexcept {
    LineDsc dsc;
    dsc.points(x1, y1, x2, y2).color(color).width(width);
    lv_draw_line(layer, dsc.get());
}

} // namespace draw

} // namespace lv
