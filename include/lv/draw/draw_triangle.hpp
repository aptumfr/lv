#pragma once

/**
 * @file draw_triangle.hpp
 * @brief Wrapper for LVGL triangle drawing
 */

#include <lvgl.h>
#include "layer.hpp"
#include "primitives.hpp"

namespace lv {

/**
 * @brief Fluent wrapper for lv_draw_triangle_dsc_t
 *
 * Example:
 * ```cpp
 * lv::TriangleDsc dsc;
 * dsc.points(50, 10, 10, 90, 90, 90)
 *    .color(lv::color::red())
 *    .opa(LV_OPA_COVER);
 * lv::draw::triangle(layer, dsc);
 * ```
 */
class TriangleDsc {
    lv_draw_triangle_dsc_t m_dsc{};

public:
    TriangleDsc() noexcept {
        lv_draw_triangle_dsc_init(&m_dsc);
    }

    [[nodiscard]] lv_draw_triangle_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_triangle_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_triangle_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Points ====================

    /// Set all three points
    TriangleDsc& points(lv_value_precise_t x1, lv_value_precise_t y1,
                        lv_value_precise_t x2, lv_value_precise_t y2,
                        lv_value_precise_t x3, lv_value_precise_t y3) noexcept {
        m_dsc.p[0].x = x1; m_dsc.p[0].y = y1;
        m_dsc.p[1].x = x2; m_dsc.p[1].y = y2;
        m_dsc.p[2].x = x3; m_dsc.p[2].y = y3;
        return *this;
    }

    /// Set first point
    TriangleDsc& p1(lv_value_precise_t x, lv_value_precise_t y) noexcept {
        m_dsc.p[0].x = x;
        m_dsc.p[0].y = y;
        return *this;
    }

    /// Set second point
    TriangleDsc& p2(lv_value_precise_t x, lv_value_precise_t y) noexcept {
        m_dsc.p[1].x = x;
        m_dsc.p[1].y = y;
        return *this;
    }

    /// Set third point
    TriangleDsc& p3(lv_value_precise_t x, lv_value_precise_t y) noexcept {
        m_dsc.p[2].x = x;
        m_dsc.p[2].y = y;
        return *this;
    }

    // ==================== Appearance ====================

    /// Set fill color
    TriangleDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set opacity
    TriangleDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    /// Set gradient (overrides color)
    TriangleDsc& grad(const lv_grad_dsc_t& g) noexcept {
        m_dsc.grad = g;
        return *this;
    }
};

namespace draw {

/// Draw a triangle
inline void triangle(lv_layer_t* layer, const TriangleDsc& dsc) noexcept {
    lv_draw_triangle(layer, dsc.get());
}

/// Convenience: draw a simple triangle
inline void triangle(lv_layer_t* layer,
                     lv_value_precise_t x1, lv_value_precise_t y1,
                     lv_value_precise_t x2, lv_value_precise_t y2,
                     lv_value_precise_t x3, lv_value_precise_t y3,
                     lv_color_t color) noexcept {
    TriangleDsc dsc;
    dsc.points(x1, y1, x2, y2, x3, y3).color(color);
    lv_draw_triangle(layer, dsc.get());
}

} // namespace draw

} // namespace lv
