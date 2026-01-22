#pragma once

/**
 * @file draw_mask.hpp
 * @brief Wrapper for LVGL mask rectangle drawing
 */

#include <lvgl.h>
#include <src/draw/lv_draw_mask.h>
#include "layer.hpp"
#include "primitives.hpp"

namespace lv {

/**
 * @brief Fluent wrapper for lv_draw_mask_rect_dsc_t
 *
 * Masks a rectangular area on a layer. Can be used to create rounded corners,
 * clip regions, or clear content outside a specified area.
 *
 * Example:
 * ```cpp
 * lv::Layer layer;
 * canvas.init_layer(layer);
 *
 * // Draw some content first...
 *
 * // Apply a mask to clip to a rounded rectangle
 * lv::MaskRectDsc mask;
 * mask.area(10, 10, 100, 100).radius(15);
 * lv::draw::mask_rect(layer, mask);
 *
 * canvas.finish_layer(layer);
 * ```
 */
class MaskRectDsc {
    lv_draw_mask_rect_dsc_t m_dsc{};

public:
    MaskRectDsc() noexcept {
        lv_draw_mask_rect_dsc_init(&m_dsc);
    }

    /// Get the underlying descriptor
    [[nodiscard]] lv_draw_mask_rect_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_mask_rect_dsc_t* get() const noexcept { return &m_dsc; }

    /// Implicit conversion for LVGL API
    operator const lv_draw_mask_rect_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Properties ====================

    /// Set the mask area from lv_area_t
    MaskRectDsc& area(const lv_area_t& a) noexcept {
        m_dsc.area = a;
        return *this;
    }

    /// Set the mask area by coordinates (x1, y1, x2, y2)
    MaskRectDsc& area(int32_t x1, int32_t y1, int32_t x2, int32_t y2) noexcept {
        m_dsc.area.x1 = x1;
        m_dsc.area.y1 = y1;
        m_dsc.area.x2 = x2;
        m_dsc.area.y2 = y2;
        return *this;
    }

    /// Set the corner radius (LV_RADIUS_CIRCLE for fully rounded)
    MaskRectDsc& radius(int32_t r) noexcept {
        m_dsc.radius = r;
        return *this;
    }

    /// Set whether to keep content outside the mask area
    /// When true: don't touch outside the area (only mask inside)
    /// When false (default): clear content outside the area
    MaskRectDsc& keep_outside(bool keep = true) noexcept {
        m_dsc.keep_outside = keep ? 1 : 0;
        return *this;
    }

    // ==================== Getters ====================

    /// Get the mask area
    [[nodiscard]] const lv_area_t& get_area() const noexcept {
        return m_dsc.area;
    }

    /// Get the radius
    [[nodiscard]] int32_t get_radius() const noexcept {
        return m_dsc.radius;
    }

    /// Get keep_outside flag
    [[nodiscard]] bool get_keep_outside() const noexcept {
        return m_dsc.keep_outside != 0;
    }
};

// ==================== Draw Functions ====================

namespace draw {

/// Apply a rectangle mask to mask out content
inline void mask_rect(lv_layer_t* layer, const MaskRectDsc& dsc) noexcept {
    lv_draw_mask_rect(layer, dsc.get());
}

} // namespace draw

} // namespace lv
