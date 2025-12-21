#pragma once

/**
 * @file color.hpp
 * @brief Zero-cost wrappers for LVGL color API
 */

#include <lvgl.h>
#include <cstdint>

namespace lv {

// ==================== Color Types ====================

/// Color type (alias for lv_color_t)
using Color = lv_color_t;

/// Opacity type (alias for lv_opa_t)
using Opacity = lv_opa_t;

/// HSV color type (alias for lv_color_hsv_t)
using ColorHSV = lv_color_hsv_t;

// ==================== Color Creation ====================

/// Create color from RGB components (0-255)
inline lv_color_t rgb(uint8_t r, uint8_t g, uint8_t b) noexcept {
    return lv_color_make(r, g, b);
}

/// Create color from hex value (0xRRGGBB)
inline lv_color_t rgb(uint32_t hex) noexcept {
    return lv_color_hex(hex);
}

/// Create color from 3-digit hex (0xRGB -> 0xRRGGBB)
inline lv_color_t rgb3(uint32_t hex) noexcept {
    return lv_color_hex3(hex);
}

// ==================== Color Constants ====================

namespace colors {
    inline lv_color_t white() noexcept { return lv_color_white(); }
    inline lv_color_t black() noexcept { return lv_color_black(); }
} // namespace colors

// ==================== Color Operations ====================

/// Mix two colors with a ratio (0 = color1, 255 = color2)
inline lv_color_t color_mix(lv_color_t c1, lv_color_t c2, uint8_t ratio) noexcept {
    return lv_color_mix(c2, c1, ratio);
}

/// Lighten a color
inline lv_color_t color_lighten(lv_color_t color, lv_opa_t level) noexcept {
    return lv_color_lighten(color, level);
}

/// Darken a color
inline lv_color_t color_darken(lv_color_t color, lv_opa_t level) noexcept {
    return lv_color_darken(color, level);
}

/// Get brightness of a color (0-255)
inline uint8_t color_brightness(lv_color_t color) noexcept {
    return lv_color_brightness(color);
}

/// Convert HSV to RGB color (separate components)
inline lv_color_t hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v) noexcept {
    return lv_color_hsv_to_rgb(h, s, v);
}

/// Convert HSV to RGB color (struct)
inline lv_color_t hsv_to_rgb(lv_color_hsv_t hsv) noexcept {
    return lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v);
}

/// Convert RGB color to HSV
inline lv_color_hsv_t rgb_to_hsv(lv_color_t color) noexcept {
    return lv_color_to_hsv(color);
}

// ==================== Opacity Constants ====================

namespace opa {
    constexpr lv_opa_t transp = LV_OPA_TRANSP;
    constexpr lv_opa_t _0 = LV_OPA_0;
    constexpr lv_opa_t _10 = LV_OPA_10;
    constexpr lv_opa_t _20 = LV_OPA_20;
    constexpr lv_opa_t _30 = LV_OPA_30;
    constexpr lv_opa_t _40 = LV_OPA_40;
    constexpr lv_opa_t _50 = LV_OPA_50;
    constexpr lv_opa_t _60 = LV_OPA_60;
    constexpr lv_opa_t _70 = LV_OPA_70;
    constexpr lv_opa_t _80 = LV_OPA_80;
    constexpr lv_opa_t _90 = LV_OPA_90;
    constexpr lv_opa_t _100 = LV_OPA_100;
    constexpr lv_opa_t cover = LV_OPA_COVER;
} // namespace opa

} // namespace lv
