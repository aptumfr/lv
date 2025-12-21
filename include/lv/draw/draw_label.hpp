#pragma once

/**
 * @file draw_label.hpp
 * @brief Wrapper for LVGL text/label drawing
 */

#include <lvgl.h>
#include "layer.hpp"
#include "primitives.hpp"

namespace lv {

/**
 * @brief Fluent wrapper for lv_draw_label_dsc_t
 *
 * Example:
 * ```cpp
 * lv::LabelDsc dsc;
 * dsc.text("Hello World")
 *    .font(&lv_font_montserrat_24)
 *    .color(lv::color::black())
 *    .align(LV_TEXT_ALIGN_CENTER);
 * lv::draw::label(layer, dsc, lv::area(10, 10, 200, 50));
 * ```
 */
class LabelDsc {
    lv_draw_label_dsc_t m_dsc{};

public:
    LabelDsc() noexcept {
        lv_draw_label_dsc_init(&m_dsc);
    }

    /// Initialize from an object's styles
    LabelDsc(lv_obj_t* obj, lv_part_t part) noexcept {
        lv_draw_label_dsc_init(&m_dsc);
        lv_obj_init_draw_label_dsc(obj, part, &m_dsc);
    }

    /// Create from object's styles
    [[nodiscard]] static LabelDsc from_obj(lv_obj_t* obj, lv_part_t part = LV_PART_MAIN) noexcept {
        return LabelDsc(obj, part);
    }

    [[nodiscard]] lv_draw_label_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_label_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_label_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Text Content ====================

    /// Set text to draw
    LabelDsc& text(const char* t) noexcept {
        m_dsc.text = t;
        return *this;
    }

    /// Set text length (0 = until null terminator)
    LabelDsc& text_length(uint32_t len) noexcept {
        m_dsc.text_length = len;
        return *this;
    }

    /// Mark text as static (can be cached)
    LabelDsc& text_static(bool s = true) noexcept {
        m_dsc.text_static = s ? 1 : 0;
        return *this;
    }

    // ==================== Font ====================

    /// Set font
    LabelDsc& font(const lv_font_t* f) noexcept {
        m_dsc.font = f;
        return *this;
    }

    // ==================== Color & Opacity ====================

    /// Set text color
    LabelDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set opacity
    LabelDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    // ==================== Spacing ====================

    /// Set line spacing
    LabelDsc& line_space(int32_t s) noexcept {
        m_dsc.line_space = s;
        return *this;
    }

    /// Set letter spacing
    LabelDsc& letter_space(int32_t s) noexcept {
        m_dsc.letter_space = s;
        return *this;
    }

    // ==================== Offset & Transform ====================

    /// Set text offset
    LabelDsc& ofs(int32_t x, int32_t y) noexcept {
        m_dsc.ofs_x = x;
        m_dsc.ofs_y = y;
        return *this;
    }

    /// Set rotation in 0.1 degree units (e.g., 450 = 45°)
    LabelDsc& rotation(int32_t r) noexcept {
        m_dsc.rotation = r;
        return *this;
    }

    // ==================== Alignment ====================

    /// Set text alignment
    LabelDsc& align(lv_text_align_t a) noexcept {
        m_dsc.align = a;
        return *this;
    }

    /// Set base direction (for RTL text)
    LabelDsc& bidi_dir(lv_base_dir_t d) noexcept {
        m_dsc.bidi_dir = d;
        return *this;
    }

    // ==================== Decoration ====================

    /// Set text decoration (underline, strikethrough)
    LabelDsc& decor(lv_text_decor_t d) noexcept {
        m_dsc.decor = d;
        return *this;
    }

    // ==================== Selection ====================

    /// Set text selection range
    LabelDsc& selection(uint32_t start, uint32_t end) noexcept {
        m_dsc.sel_start = start;
        m_dsc.sel_end = end;
        return *this;
    }

    /// Set selection colors
    LabelDsc& sel_color(lv_color_t text_color, lv_color_t bg_color) noexcept {
        m_dsc.sel_color = text_color;
        m_dsc.sel_bg_color = bg_color;
        return *this;
    }

    /// Clear selection
    LabelDsc& no_selection() noexcept {
        m_dsc.sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
        m_dsc.sel_end = LV_DRAW_LABEL_NO_TXT_SEL;
        return *this;
    }

    // ==================== Outline ====================

    /// Set text outline stroke
    LabelDsc& outline(lv_color_t color, int32_t width, lv_opa_t opacity = LV_OPA_COVER) noexcept {
        m_dsc.outline_stroke_color = color;
        m_dsc.outline_stroke_width = width;
        m_dsc.outline_stroke_opa = opacity;
        return *this;
    }

    // ==================== Flags ====================

    /// Set text flags
    LabelDsc& flag(lv_text_flag_t f) noexcept {
        m_dsc.flag = f;
        return *this;
    }
};

// Note: lv_draw_glyph_dsc_t is defined in private header (lv_draw_label_private.h)
// Use lv_draw_glyph_dsc_t* directly with draw::label_iterate_characters()

/**
 * @brief Fluent wrapper for lv_draw_letter_dsc_t (single character)
 *
 * Example:
 * ```cpp
 * lv::LetterDsc dsc;
 * dsc.unicode('A')
 *    .font(&lv_font_montserrat_48)
 *    .color(lv::color::red());
 * lv::draw::letter(layer, dsc, lv::point(100, 100));
 * ```
 */
class LetterDsc {
    lv_draw_letter_dsc_t m_dsc{};

public:
    LetterDsc() noexcept {
        lv_draw_letter_dsc_init(&m_dsc);
    }

    [[nodiscard]] lv_draw_letter_dsc_t* get() noexcept { return &m_dsc; }
    [[nodiscard]] const lv_draw_letter_dsc_t* get() const noexcept { return &m_dsc; }
    operator const lv_draw_letter_dsc_t*() const noexcept { return &m_dsc; }

    // ==================== Character ====================

    /// Set unicode character
    LetterDsc& unicode(uint32_t u) noexcept {
        m_dsc.unicode = u;
        return *this;
    }

    /// Set font
    LetterDsc& font(const lv_font_t* f) noexcept {
        m_dsc.font = f;
        return *this;
    }

    // ==================== Color & Opacity ====================

    /// Set color
    LetterDsc& color(lv_color_t c) noexcept {
        m_dsc.color = c;
        return *this;
    }

    /// Set opacity
    LetterDsc& opa(lv_opa_t o) noexcept {
        m_dsc.opa = o;
        return *this;
    }

    // ==================== Transform ====================

    /// Set rotation in 0.1 degree units
    LetterDsc& rotation(int32_t r) noexcept {
        m_dsc.rotation = r;
        return *this;
    }

    /// Set scale (256 = 100%)
    LetterDsc& scale(int32_t x, int32_t y) noexcept {
        m_dsc.scale_x = x;
        m_dsc.scale_y = y;
        return *this;
    }

    /// Set uniform scale
    LetterDsc& scale(int32_t s) noexcept {
        m_dsc.scale_x = s;
        m_dsc.scale_y = s;
        return *this;
    }

    /// Set skew in 0.1 degree units
    LetterDsc& skew(int32_t x, int32_t y) noexcept {
        m_dsc.skew_x = x;
        m_dsc.skew_y = y;
        return *this;
    }

    /// Set transform pivot point
    LetterDsc& pivot(int32_t x, int32_t y) noexcept {
        m_dsc.pivot.x = x;
        m_dsc.pivot.y = y;
        return *this;
    }

    // ==================== Decoration ====================

    /// Set decoration
    LetterDsc& decor(lv_text_decor_t d) noexcept {
        m_dsc.decor = d;
        return *this;
    }

    /// Set blend mode
    LetterDsc& blend_mode(lv_blend_mode_t m) noexcept {
        m_dsc.blend_mode = m;
        return *this;
    }

    // ==================== Outline ====================

    /// Set outline stroke
    LetterDsc& outline(lv_color_t color, int32_t width, lv_opa_t opacity = LV_OPA_COVER) noexcept {
        m_dsc.outline_stroke_color = color;
        m_dsc.outline_stroke_width = width;
        m_dsc.outline_stroke_opa = opacity;
        return *this;
    }
};

namespace draw {

/// Draw text
inline void label(lv_layer_t* layer, const LabelDsc& dsc, const lv_area_t& coords) noexcept {
    lv_draw_label(layer, dsc.get(), &coords);
}

/// Convenience: draw simple text
inline void label(lv_layer_t* layer, const char* text, const lv_area_t& coords,
                  const lv_font_t* font = nullptr, lv_color_t color = lv_color_black()) noexcept {
    LabelDsc dsc;
    dsc.text(text).color(color);
    if (font) dsc.font(font);
    lv_draw_label(layer, dsc.get(), &coords);
}

/// Draw a single character
inline void character(lv_layer_t* layer, LabelDsc& dsc, const lv_point_t& point, uint32_t unicode) noexcept {
    lv_draw_character(layer, dsc.get(), &point, unicode);
}

/// Draw a single letter with full control
inline void letter(lv_layer_t* layer, LetterDsc& dsc, const lv_point_t& point) noexcept {
    lv_draw_letter(layer, dsc.get(), &point);
}

// ==================== Advanced Text Rendering ====================

/**
 * @brief Iterate through characters for custom rendering
 *
 * This is primarily used by custom draw units to implement text rendering.
 * The callback is called for each glyph that needs to be drawn.
 *
 * @param task The draw task
 * @param dsc The label descriptor
 * @param coords The label coordinates
 * @param cb Callback function for each glyph
 */
inline void label_iterate_characters(
    lv_draw_task_t* task,
    const lv_draw_label_dsc_t* dsc,
    const lv_area_t* coords,
    lv_draw_glyph_cb_t cb) noexcept {
    lv_draw_label_iterate_characters(task, dsc, coords, cb);
}

/**
 * @brief Draw a letter using draw unit primitives
 *
 * Used by custom draw units to render individual letters with full control.
 */
inline void unit_draw_letter(
    lv_draw_task_t* task,
    lv_draw_glyph_dsc_t* dsc,
    const lv_point_t* pos,
    const lv_font_t* font,
    uint32_t letter,
    lv_draw_glyph_cb_t cb) noexcept {
    lv_draw_unit_draw_letter(task, dsc, pos, font, letter, cb);
}

} // namespace draw

} // namespace lv
