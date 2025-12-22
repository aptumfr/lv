#pragma once

/**
 * @file style.hpp
 * @brief Zero-cost style builder for LVGL
 *
 * Provides RAII-managed style objects and a fluent builder API.
 * Styles are embedded (stack-allocated when possible) - no heap allocation.
 */

#include <lvgl.h>
#include <cstdint>
#include <type_traits>

namespace lv {

/**
 * @brief RAII wrapper for lv_style_t
 *
 * Manages style lifecycle with automatic init/reset.
 * The style can be shared across multiple objects (LVGL copies pointer, not data).
 *
 * Size: sizeof(lv_style_t) - typically ~32 bytes
 * Overhead: Zero - just wraps the embedded style
 *
 * @warning Move operations assume lv_style_t is trivially movable (shallow copy).
 * As of LVGL 9.x, lv_style_t is a plain struct without internal pointers.
 * If future LVGL versions add internal references, move semantics may need revision.
 *
 * @note If the static_assert below fails in a future LVGL version:
 * Fix strategy for move operations:
 *   1. lv_style_init(&m_style) on the destination
 *   2. Use lv_style_copy() if LVGL provides it, OR
 *   3. Manually transfer properties via lv_style_get_prop() / lv_style_set_prop()
 *   4. lv_style_reset() on the source, then lv_style_init() to leave it valid
 * The shallow memcpy approach (m_style = other.m_style) will NOT work if
 * lv_style_t contains internal pointers (e.g., dynamically allocated prop arrays).
 */
class Style {
    // Verify lv_style_t is trivially copyable - move semantics rely on this.
    // If this fails, see the @note above for the fix strategy.
    static_assert(std::is_trivially_copyable_v<lv_style_t>,
        "lv_style_t is no longer trivially copyable. "
        "See Style class @note for fix strategy: use lv_style_copy() or "
        "iterate properties with lv_style_get_prop()/lv_style_set_prop().");

private:
    lv_style_t m_style;

public:
    /// Initialize an empty style
    Style() noexcept {
        lv_style_init(&m_style);
    }

    /// Destructor resets the style
    ~Style() {
        lv_style_reset(&m_style);
    }

    // Non-copyable (styles should be shared via pointer, not copied)
    Style(const Style&) = delete;
    Style& operator=(const Style&) = delete;

    // Moveable
    Style(Style&& other) noexcept : m_style(other.m_style) {
        // Reinit the moved-from style so it's valid for destruction
        lv_style_init(&other.m_style);
    }

    Style& operator=(Style&& other) noexcept {
        if (this != &other) {
            lv_style_reset(&m_style);
            m_style = other.m_style;
            lv_style_init(&other.m_style);
        }
        return *this;
    }

    /// Get pointer to underlying style (for LVGL API)
    [[nodiscard]] lv_style_t* get() noexcept { return &m_style; }
    [[nodiscard]] const lv_style_t* get() const noexcept { return &m_style; }

    /// Implicit conversion for C API interop
    [[nodiscard]] operator lv_style_t*() noexcept { return &m_style; }

    /// Reset style to empty state
    Style& reset() noexcept {
        lv_style_reset(&m_style);
        lv_style_init(&m_style);
        return *this;
    }

    // ==================== Background ====================

    Style& bg_color(lv_color_t color) noexcept {
        lv_style_set_bg_color(&m_style, color);
        return *this;
    }

    Style& bg_opa(lv_opa_t opa) noexcept {
        lv_style_set_bg_opa(&m_style, opa);
        return *this;
    }

    Style& bg_grad_color(lv_color_t color) noexcept {
        lv_style_set_bg_grad_color(&m_style, color);
        return *this;
    }

    Style& bg_grad_dir(lv_grad_dir_t dir) noexcept {
        lv_style_set_bg_grad_dir(&m_style, dir);
        return *this;
    }

    Style& bg_main_stop(int32_t stop) noexcept {
        lv_style_set_bg_main_stop(&m_style, stop);
        return *this;
    }

    Style& bg_grad_stop(int32_t stop) noexcept {
        lv_style_set_bg_grad_stop(&m_style, stop);
        return *this;
    }

    // ==================== Border ====================

    Style& border_color(lv_color_t color) noexcept {
        lv_style_set_border_color(&m_style, color);
        return *this;
    }

    Style& border_width(int32_t width) noexcept {
        lv_style_set_border_width(&m_style, width);
        return *this;
    }

    Style& border_opa(lv_opa_t opa) noexcept {
        lv_style_set_border_opa(&m_style, opa);
        return *this;
    }

    Style& border_side(lv_border_side_t side) noexcept {
        lv_style_set_border_side(&m_style, side);
        return *this;
    }

    // ==================== Outline ====================

    Style& outline_color(lv_color_t color) noexcept {
        lv_style_set_outline_color(&m_style, color);
        return *this;
    }

    Style& outline_width(int32_t width) noexcept {
        lv_style_set_outline_width(&m_style, width);
        return *this;
    }

    Style& outline_opa(lv_opa_t opa) noexcept {
        lv_style_set_outline_opa(&m_style, opa);
        return *this;
    }

    Style& outline_pad(int32_t pad) noexcept {
        lv_style_set_outline_pad(&m_style, pad);
        return *this;
    }

    // ==================== Shadow ====================

    Style& shadow_color(lv_color_t color) noexcept {
        lv_style_set_shadow_color(&m_style, color);
        return *this;
    }

    Style& shadow_width(int32_t width) noexcept {
        lv_style_set_shadow_width(&m_style, width);
        return *this;
    }

    Style& shadow_opa(lv_opa_t opa) noexcept {
        lv_style_set_shadow_opa(&m_style, opa);
        return *this;
    }

    Style& shadow_offset(int32_t x, int32_t y) noexcept {
        lv_style_set_shadow_offset_x(&m_style, x);
        lv_style_set_shadow_offset_y(&m_style, y);
        return *this;
    }

    Style& shadow_spread(int32_t spread) noexcept {
        lv_style_set_shadow_spread(&m_style, spread);
        return *this;
    }

    // ==================== Padding ====================

    Style& pad_all(int32_t pad) noexcept {
        lv_style_set_pad_all(&m_style, pad);
        return *this;
    }

    Style& pad_top(int32_t pad) noexcept {
        lv_style_set_pad_top(&m_style, pad);
        return *this;
    }

    Style& pad_bottom(int32_t pad) noexcept {
        lv_style_set_pad_bottom(&m_style, pad);
        return *this;
    }

    Style& pad_left(int32_t pad) noexcept {
        lv_style_set_pad_left(&m_style, pad);
        return *this;
    }

    Style& pad_right(int32_t pad) noexcept {
        lv_style_set_pad_right(&m_style, pad);
        return *this;
    }

    Style& pad_hor(int32_t pad) noexcept {
        lv_style_set_pad_hor(&m_style, pad);
        return *this;
    }

    Style& pad_ver(int32_t pad) noexcept {
        lv_style_set_pad_ver(&m_style, pad);
        return *this;
    }

    Style& pad_row(int32_t pad) noexcept {
        lv_style_set_pad_row(&m_style, pad);
        return *this;
    }

    Style& pad_column(int32_t pad) noexcept {
        lv_style_set_pad_column(&m_style, pad);
        return *this;
    }

    Style& pad_gap(int32_t pad) noexcept {
        lv_style_set_pad_row(&m_style, pad);
        lv_style_set_pad_column(&m_style, pad);
        return *this;
    }

    // ==================== Margin ====================

    Style& margin_all(int32_t margin) noexcept {
        lv_style_set_margin_top(&m_style, margin);
        lv_style_set_margin_bottom(&m_style, margin);
        lv_style_set_margin_left(&m_style, margin);
        lv_style_set_margin_right(&m_style, margin);
        return *this;
    }

    Style& margin_top(int32_t margin) noexcept {
        lv_style_set_margin_top(&m_style, margin);
        return *this;
    }

    Style& margin_bottom(int32_t margin) noexcept {
        lv_style_set_margin_bottom(&m_style, margin);
        return *this;
    }

    Style& margin_left(int32_t margin) noexcept {
        lv_style_set_margin_left(&m_style, margin);
        return *this;
    }

    Style& margin_right(int32_t margin) noexcept {
        lv_style_set_margin_right(&m_style, margin);
        return *this;
    }

    // ==================== Size ====================

    Style& width(int32_t w) noexcept {
        lv_style_set_width(&m_style, w);
        return *this;
    }

    Style& min_width(int32_t w) noexcept {
        lv_style_set_min_width(&m_style, w);
        return *this;
    }

    Style& max_width(int32_t w) noexcept {
        lv_style_set_max_width(&m_style, w);
        return *this;
    }

    Style& height(int32_t h) noexcept {
        lv_style_set_height(&m_style, h);
        return *this;
    }

    Style& min_height(int32_t h) noexcept {
        lv_style_set_min_height(&m_style, h);
        return *this;
    }

    Style& max_height(int32_t h) noexcept {
        lv_style_set_max_height(&m_style, h);
        return *this;
    }

    // ==================== Appearance ====================

    Style& radius(int32_t r) noexcept {
        lv_style_set_radius(&m_style, r);
        return *this;
    }

    Style& opa(lv_opa_t opa) noexcept {
        lv_style_set_opa(&m_style, opa);
        return *this;
    }

    Style& clip_corner(bool clip) noexcept {
        lv_style_set_clip_corner(&m_style, clip);
        return *this;
    }

    Style& blend_mode(lv_blend_mode_t mode) noexcept {
        lv_style_set_blend_mode(&m_style, mode);
        return *this;
    }

    // ==================== Text ====================

    Style& text_color(lv_color_t color) noexcept {
        lv_style_set_text_color(&m_style, color);
        return *this;
    }

    Style& text_opa(lv_opa_t opa) noexcept {
        lv_style_set_text_opa(&m_style, opa);
        return *this;
    }

    Style& text_font(const lv_font_t* font) noexcept {
        lv_style_set_text_font(&m_style, font);
        return *this;
    }

    Style& text_letter_space(int32_t space) noexcept {
        lv_style_set_text_letter_space(&m_style, space);
        return *this;
    }

    Style& text_line_space(int32_t space) noexcept {
        lv_style_set_text_line_space(&m_style, space);
        return *this;
    }

    Style& text_decor(lv_text_decor_t decor) noexcept {
        lv_style_set_text_decor(&m_style, decor);
        return *this;
    }

    Style& text_align(lv_text_align_t align) noexcept {
        lv_style_set_text_align(&m_style, align);
        return *this;
    }

    // ==================== Image ====================

    Style& image_opa(lv_opa_t opa) noexcept {
        lv_style_set_image_opa(&m_style, opa);
        return *this;
    }

    Style& image_recolor(lv_color_t color) noexcept {
        lv_style_set_image_recolor(&m_style, color);
        return *this;
    }

    Style& image_recolor_opa(lv_opa_t opa) noexcept {
        lv_style_set_image_recolor_opa(&m_style, opa);
        return *this;
    }

    // ==================== Line ====================

    Style& line_color(lv_color_t color) noexcept {
        lv_style_set_line_color(&m_style, color);
        return *this;
    }

    Style& line_width(int32_t width) noexcept {
        lv_style_set_line_width(&m_style, width);
        return *this;
    }

    Style& line_opa(lv_opa_t opa) noexcept {
        lv_style_set_line_opa(&m_style, opa);
        return *this;
    }

    Style& line_rounded(bool rounded) noexcept {
        lv_style_set_line_rounded(&m_style, rounded);
        return *this;
    }

    // ==================== Arc ====================

    Style& arc_color(lv_color_t color) noexcept {
        lv_style_set_arc_color(&m_style, color);
        return *this;
    }

    Style& arc_width(int32_t width) noexcept {
        lv_style_set_arc_width(&m_style, width);
        return *this;
    }

    Style& arc_opa(lv_opa_t opa) noexcept {
        lv_style_set_arc_opa(&m_style, opa);
        return *this;
    }

    Style& arc_rounded(bool rounded) noexcept {
        lv_style_set_arc_rounded(&m_style, rounded);
        return *this;
    }

    // ==================== Transform ====================

    Style& transform_width(int32_t width) noexcept {
        lv_style_set_transform_width(&m_style, width);
        return *this;
    }

    Style& transform_height(int32_t height) noexcept {
        lv_style_set_transform_height(&m_style, height);
        return *this;
    }

    Style& transform_scale(int32_t scale) noexcept {
        lv_style_set_transform_scale(&m_style, scale);
        return *this;
    }

    Style& transform_scale_x(int32_t scale) noexcept {
        lv_style_set_transform_scale_x(&m_style, scale);
        return *this;
    }

    Style& transform_scale_y(int32_t scale) noexcept {
        lv_style_set_transform_scale_y(&m_style, scale);
        return *this;
    }

    Style& transform_rotation(int32_t angle) noexcept {
        lv_style_set_transform_rotation(&m_style, angle);
        return *this;
    }

    Style& transform_pivot(int32_t x, int32_t y) noexcept {
        lv_style_set_transform_pivot_x(&m_style, x);
        lv_style_set_transform_pivot_y(&m_style, y);
        return *this;
    }

    // ==================== Layout ====================

    Style& layout(uint16_t layout) noexcept {
        lv_style_set_layout(&m_style, layout);
        return *this;
    }

    Style& flex_flow(lv_flex_flow_t flow) noexcept {
        lv_style_set_flex_flow(&m_style, flow);
        return *this;
    }

    Style& flex_main_place(lv_flex_align_t place) noexcept {
        lv_style_set_flex_main_place(&m_style, place);
        return *this;
    }

    Style& flex_cross_place(lv_flex_align_t place) noexcept {
        lv_style_set_flex_cross_place(&m_style, place);
        return *this;
    }

    Style& flex_track_place(lv_flex_align_t place) noexcept {
        lv_style_set_flex_track_place(&m_style, place);
        return *this;
    }

    Style& flex_grow(uint8_t grow) noexcept {
        lv_style_set_flex_grow(&m_style, grow);
        return *this;
    }

    // ==================== Alignment ====================

    Style& align(lv_align_t align) noexcept {
        lv_style_set_align(&m_style, align);
        return *this;
    }

    // ==================== Transform ====================

    Style& translate_x(int32_t x) noexcept {
        lv_style_set_translate_x(&m_style, x);
        return *this;
    }

    Style& translate_y(int32_t y) noexcept {
        lv_style_set_translate_y(&m_style, y);
        return *this;
    }
};


// ==================== Zero-Cost Verification ====================

// Static assertion to verify Style wraps lv_style_t without overhead
static_assert(sizeof(Style) == sizeof(lv_style_t),
    "Style must be exactly lv_style_t-sized for zero overhead");

// ==================== Style Property Setters on ObjectView ====================

/**
 * @brief Mixin for inline style setters
 *
 * These methods set local styles directly on the object.
 * Use Style class for shared styles.
 */
template<typename Derived>
class StyleMixin {
private:
    [[nodiscard]] lv_obj_t* obj() noexcept {
        return static_cast<Derived*>(this)->get();
    }

    [[nodiscard]] lv_obj_t* obj() const noexcept {
        return static_cast<const Derived*>(this)->get();
    }

public:
    // ==================== Background ====================

    Derived& bg_color(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_color(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& bg_opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& bg_grad_color(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_grad_color(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& bg_grad_dir(lv_grad_dir_t dir, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_grad_dir(obj(), dir, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& bg_grad_stop(int32_t stop, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_grad_stop(obj(), stop, sel);
        return *static_cast<Derived*>(this);
    }

    /// Set full gradient descriptor
    Derived& bg_grad(const lv_grad_dsc_t* grad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_grad(obj(), grad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& bitmap_mask_src(const void* src, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bitmap_mask_src(obj(), src, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& bg_image_src(const void* src, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_bg_image_src(obj(), src, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Border ====================

    Derived& border_color(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_border_color(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& border_width(int32_t width, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_border_width(obj(), width, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Padding ====================

    Derived& padding(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_all(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& pad_top(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_top(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& pad_bottom(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_bottom(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& pad_left(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_left(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& pad_right(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_right(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& padding_hor(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_left(obj(), pad, sel);
        lv_obj_set_style_pad_right(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& padding_ver(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_top(obj(), pad, sel);
        lv_obj_set_style_pad_bottom(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& pad_row(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_row(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& pad_column(int32_t pad, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_column(obj(), pad, sel);
        return *static_cast<Derived*>(this);
    }

    /// Set both row and column gap
    Derived& gap(int32_t g, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_pad_row(obj(), g, sel);
        lv_obj_set_style_pad_column(obj(), g, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Appearance ====================

    Derived& radius(int32_t r, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_radius(obj(), r, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& blend_mode(lv_blend_mode_t mode, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_blend_mode(obj(), mode, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Text ====================

    Derived& text_color(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_text_color(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& text_font(const lv_font_t* font, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_text_font(obj(), font, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& text_align(lv_text_align_t align, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_text_align(obj(), align, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& text_line_space(int32_t space, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_text_line_space(obj(), space, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Image ====================

    Derived& image_opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_image_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& image_recolor(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_image_recolor(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& image_recolor_opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_image_recolor_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Arc ====================

    Derived& arc_width(int32_t width, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_arc_width(obj(), width, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& arc_color(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_arc_color(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& arc_opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_arc_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& arc_rounded(bool rounded, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_arc_rounded(obj(), rounded, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Line ====================

    Derived& line_width(int32_t width, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_line_width(obj(), width, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& line_color(lv_color_t color, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_line_color(obj(), color, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& line_opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_line_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& line_rounded(bool rounded, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_line_rounded(obj(), rounded, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Transform ====================

    Derived& transform_pivot_x(int32_t x, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_pivot_x(obj(), x, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_pivot_y(int32_t y, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_pivot_y(obj(), y, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_pivot(int32_t x, int32_t y, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_pivot_x(obj(), x, sel);
        lv_obj_set_style_transform_pivot_y(obj(), y, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_rotation(int32_t angle, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_rotation(obj(), angle, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_scale(int32_t scale, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_scale(obj(), scale, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_scale_x(int32_t scale, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_scale_x(obj(), scale, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_scale_y(int32_t scale, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_scale_y(obj(), scale, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_width(int32_t width, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_width(obj(), width, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& transform_height(int32_t height, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_transform_height(obj(), height, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Border Extended ====================

    Derived& border_side(lv_border_side_t side, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_border_side(obj(), side, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& border_opa(lv_opa_t opa, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_border_opa(obj(), opa, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Size Constraints ====================

    Derived& min_width(int32_t w, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_min_width(obj(), w, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& max_width(int32_t w, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_max_width(obj(), w, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& min_height(int32_t h, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_min_height(obj(), h, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& max_height(int32_t h, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_max_height(obj(), h, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Margin ====================

    Derived& margin(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_top(obj(), m, sel);
        lv_obj_set_style_margin_bottom(obj(), m, sel);
        lv_obj_set_style_margin_left(obj(), m, sel);
        lv_obj_set_style_margin_right(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& margin_top(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_top(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& margin_bottom(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_bottom(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& margin_left(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_left(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& margin_right(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_right(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& margin_hor(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_left(obj(), m, sel);
        lv_obj_set_style_margin_right(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& margin_ver(int32_t m, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_margin_top(obj(), m, sel);
        lv_obj_set_style_margin_bottom(obj(), m, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Animation ====================

    Derived& anim_duration(uint32_t duration, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_anim_duration(obj(), duration, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Flex Layout Style ====================

    Derived& flex_main_place(lv_flex_align_t place, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_flex_main_place(obj(), place, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& flex_cross_place(lv_flex_align_t place, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_flex_cross_place(obj(), place, sel);
        return *static_cast<Derived*>(this);
    }

    Derived& flex_track_place(lv_flex_align_t place, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_flex_track_place(obj(), place, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Size (selector-based, for styling widget parts) ====================

    /// Set width for a specific part/state (e.g., LV_PART_INDICATOR)
    Derived& part_width(int32_t w, lv_style_selector_t sel) noexcept {
        lv_obj_set_style_width(obj(), w, sel);
        return *static_cast<Derived*>(this);
    }

    /// Set height for a specific part/state (e.g., LV_PART_KNOB)
    Derived& part_height(int32_t h, lv_style_selector_t sel) noexcept {
        lv_obj_set_style_height(obj(), h, sel);
        return *static_cast<Derived*>(this);
    }

    /// Set size for a specific part/state (e.g., LV_PART_INDICATOR)
    Derived& part_size(int32_t w, int32_t h, lv_style_selector_t sel) noexcept {
        lv_obj_set_style_width(obj(), w, sel);
        lv_obj_set_style_height(obj(), h, sel);
        return *static_cast<Derived*>(this);
    }

    // ==================== Transform / Translate ====================

    /// Set translate X
    Derived& translate_x(int32_t x, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_translate_x(obj(), x, sel);
        return *static_cast<Derived*>(this);
    }

    /// Set translate Y
    Derived& translate_y(int32_t y, lv_style_selector_t sel = 0) noexcept {
        lv_obj_set_style_translate_y(obj(), y, sel);
        return *static_cast<Derived*>(this);
    }

    /// Get translate X
    [[nodiscard]] int32_t get_style_translate_x(lv_part_t part = LV_PART_MAIN) const noexcept {
        return lv_obj_get_style_translate_x(obj(), part);
    }

    /// Get translate Y
    [[nodiscard]] int32_t get_style_translate_y(lv_part_t part = LV_PART_MAIN) const noexcept {
        return lv_obj_get_style_translate_y(obj(), part);
    }

    /// Get opacity
    [[nodiscard]] lv_opa_t get_style_opa(lv_part_t part = LV_PART_MAIN) const noexcept {
        return lv_obj_get_style_opa(obj(), part);
    }
};

// Color helpers moved to core/color.hpp

} // namespace lv
