#pragma once

/**
 * @file draw_rect.hpp
 * @brief Wrappers for LVGL rectangle drawing (fill, border, shadow, rect)
 */

#include <lvgl.h>
#include "layer.hpp"
#include "primitives.hpp"

namespace lv {

/**
 * @brief Fluent wrapper for lv_draw_fill_dsc_t
 *
 * Example:
 * ```cpp
 * lv::FillDsc dsc;
 * dsc.color(lv::color::red()).radius(10).opa(LV_OPA_80);
 * lv::draw::fill(layer, dsc, lv::area(10, 10, 100, 100));
 * ```
 */
class FillDsc {
    lv_draw_fill_dsc_t m_dsc{};

public:
    FillDsc() noexcept {
        lv_draw_fill_dsc_init(&m_dsc);
    }

    /// Get the underlying descriptor
    [[nodiscard]] lv_draw_fill_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_fill_dsc_t* get() const noexcept { return &m_dsc; }

    /// Implicit conversion for LVGL API
    operator const lv_draw_fill_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Properties ====================

    /// Set fill color
    FillDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set corner radius (LV_RADIUS_CIRCLE for fully rounded)
    FillDsc& radius(int32_t r) noexcept {
        m_dsc.radius = r;
        return *this;
    }

    /// Set opacity
    FillDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    /// Set gradient
    FillDsc& grad(const lv_grad_dsc_t& g) noexcept {
        m_dsc.grad = g;
        return *this;
    }
};

/**
 * @brief Fluent wrapper for lv_draw_border_dsc_t
 *
 * Example:
 * ```cpp
 * lv::BorderDsc dsc;
 * dsc.color(lv::color::blue()).width(2).radius(5);
 * lv::draw::border(layer, dsc, lv::area(10, 10, 100, 100));
 * ```
 */
class BorderDsc {
    lv_draw_border_dsc_t m_dsc{};

public:
    BorderDsc() noexcept {
        lv_draw_border_dsc_init(&m_dsc);
    }

    [[nodiscard]] lv_draw_border_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_border_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_border_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Properties ====================

    /// Set border color
    BorderDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set border width
    BorderDsc& width(int32_t w) noexcept {
        m_dsc.width = w;
        return *this;
    }

    /// Set corner radius
    BorderDsc& radius(int32_t r) noexcept {
        m_dsc.radius = r;
        return *this;
    }

    /// Set opacity
    BorderDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    /// Set which sides to draw
    BorderDsc& side(lv_border_side_t s) noexcept {
        m_dsc.side = s;
        return *this;
    }
};

/**
 * @brief Fluent wrapper for lv_draw_box_shadow_dsc_t
 *
 * Example:
 * ```cpp
 * lv::BoxShadowDsc dsc;
 * dsc.color(lv::color::black()).width(10).spread(5).ofs(2, 2);
 * lv::draw::box_shadow(layer, dsc, lv::area(10, 10, 100, 100));
 * ```
 */
class BoxShadowDsc {
    lv_draw_box_shadow_dsc_t m_dsc{};

public:
    BoxShadowDsc() noexcept {
        lv_draw_box_shadow_dsc_init(&m_dsc);
    }

    [[nodiscard]] lv_draw_box_shadow_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_box_shadow_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_box_shadow_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Properties ====================

    /// Set shadow color
    BoxShadowDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set shadow blur radius
    BoxShadowDsc& width(int32_t w) noexcept {
        m_dsc.width = w;
        return *this;
    }

    /// Set shadow spread (positive = larger, negative = smaller)
    BoxShadowDsc& spread(int32_t s) noexcept {
        m_dsc.spread = s;
        return *this;
    }

    /// Set shadow offset
    BoxShadowDsc& ofs(int32_t x, int32_t y) noexcept {
        m_dsc.ofs_x = x;
        m_dsc.ofs_y = y;
        return *this;
    }

    /// Set corner radius
    BoxShadowDsc& radius(int32_t r) noexcept {
        m_dsc.radius = r;
        return *this;
    }

    /// Set opacity
    BoxShadowDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    /// Hint that background will cover the shadow
    BoxShadowDsc& bg_cover(bool cover = true) noexcept {
        m_dsc.bg_cover = cover ? 1 : 0;
        return *this;
    }
};

/**
 * @brief Fluent wrapper for lv_draw_rect_dsc_t (composite rectangle)
 *
 * A rectangle combines fill, border, background image, and shadow.
 *
 * Example:
 * ```cpp
 * lv::RectDsc dsc;
 * dsc.bg_color(lv::color::white())
 *    .border_color(lv::color::gray()).border_width(1)
 *    .radius(10);
 * lv::draw::rect(layer, dsc, lv::area(10, 10, 100, 100));
 * ```
 */
class RectDsc {
    lv_draw_rect_dsc_t m_dsc{};

public:
    RectDsc() noexcept {
        lv_draw_rect_dsc_init(&m_dsc);
    }

    /// Initialize from an object's styles
    RectDsc(lv_obj_t* obj, lv_part_t part) noexcept {
        lv_draw_rect_dsc_init(&m_dsc);
        lv_obj_init_draw_rect_dsc(obj, part, &m_dsc);
    }

    /// Create from object's styles
    [[nodiscard]] static RectDsc from_obj(lv_obj_t* obj, lv_part_t part = LV_PART_MAIN) noexcept {
        return RectDsc(obj, part);
    }

    [[nodiscard]] lv_draw_rect_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_rect_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_rect_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Corner Radius ====================

    RectDsc& radius(int32_t r) noexcept {
        m_dsc.radius = r;
        return *this;
    }

    // ==================== Background ====================

    RectDsc& bg_color(lv_color_t c) noexcept {
        m_dsc.bg_color = c;
        return *this;
    }

    RectDsc& bg_opa(lv_opa_t o) noexcept {
        m_dsc.bg_opa = o;
        return *this;
    }

    RectDsc& bg_grad(const lv_grad_dsc_t& g) noexcept {
        m_dsc.bg_grad = g;
        return *this;
    }

    // ==================== Background Image ====================

    RectDsc& bg_image_src(const void* src) noexcept {
        m_dsc.bg_image_src = src;
        return *this;
    }

    RectDsc& bg_image_opa(lv_opa_t o) noexcept {
        m_dsc.bg_image_opa = o;
        return *this;
    }

    RectDsc& bg_image_recolor(lv_color_t c, lv_opa_t o = LV_OPA_COVER) noexcept {
        m_dsc.bg_image_recolor = c;
        m_dsc.bg_image_recolor_opa = o;
        return *this;
    }

    RectDsc& bg_image_tiled(bool tiled = true) noexcept {
        m_dsc.bg_image_tiled = tiled ? 1 : 0;
        return *this;
    }

    // ==================== Border ====================

    RectDsc& border_color(lv_color_t c) noexcept {
        m_dsc.border_color = c;
        return *this;
    }

    RectDsc& border_width(int32_t w) noexcept {
        m_dsc.border_width = w;
        return *this;
    }

    RectDsc& border_opa(lv_opa_t o) noexcept {
        m_dsc.border_opa = o;
        return *this;
    }

    RectDsc& border_side(lv_border_side_t s) noexcept {
        m_dsc.border_side = s;
        return *this;
    }

    RectDsc& border_post(bool post = true) noexcept {
        m_dsc.border_post = post ? 1 : 0;
        return *this;
    }

    // ==================== Outline ====================

    RectDsc& outline_color(lv_color_t c) noexcept {
        m_dsc.outline_color = c;
        return *this;
    }

    RectDsc& outline_width(int32_t w) noexcept {
        m_dsc.outline_width = w;
        return *this;
    }

    RectDsc& outline_pad(int32_t p) noexcept {
        m_dsc.outline_pad = p;
        return *this;
    }

    RectDsc& outline_opa(lv_opa_t o) noexcept {
        m_dsc.outline_opa = o;
        return *this;
    }

    // ==================== Shadow ====================

    RectDsc& shadow_color(lv_color_t c) noexcept {
        m_dsc.shadow_color = c;
        return *this;
    }

    RectDsc& shadow_width(int32_t w) noexcept {
        m_dsc.shadow_width = w;
        return *this;
    }

    RectDsc& shadow_ofs(int32_t x, int32_t y) noexcept {
        m_dsc.shadow_offset_x = x;
        m_dsc.shadow_offset_y = y;
        return *this;
    }

    RectDsc& shadow_spread(int32_t s) noexcept {
        m_dsc.shadow_spread = s;
        return *this;
    }

    RectDsc& shadow_opa(lv_opa_t o) noexcept {
        m_dsc.shadow_opa = o;
        return *this;
    }
};

// ==================== Draw Functions ====================

namespace draw {

/// Draw a filled rectangle
inline void fill(lv_layer_t* layer, const FillDsc& dsc, const lv_area_t& coords) noexcept {
    lv_draw_fill(layer, dsc.get(), &coords);
}

/// Draw a border
inline void border(lv_layer_t* layer, const BorderDsc& dsc, const lv_area_t& coords) noexcept {
    lv_draw_border(layer, dsc.get(), &coords);
}

/// Draw a box shadow
inline void box_shadow(lv_layer_t* layer, const BoxShadowDsc& dsc, const lv_area_t& coords) noexcept {
    lv_draw_box_shadow(layer, dsc.get(), &coords);
}

/// Draw a complete rectangle (fill + border + image + shadow)
inline void rect(lv_layer_t* layer, const RectDsc& dsc, const lv_area_t& coords) noexcept {
    lv_draw_rect(layer, dsc.get(), &coords);
}

} // namespace draw

} // namespace lv
