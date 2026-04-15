#pragma once

/**
 * @file span.hpp
 * @brief Zero-cost wrapper for LVGL span widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Spangroup widget wrapper
 *
 * A text container with multiple styled spans (rich text).
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Spangroup : public ObjectView,
            public ObjectMixin<Spangroup>,
                  public EventMixin<Spangroup>,
                  public StyleMixin<Spangroup> {
public:
    // Spangroup declares its own align() for text alignment, which
    // would otherwise hide ObjectMixin<Spangroup>::align() for
    // object positioning (C++ name hiding applies to all base
    // overloads). Re-expose the object-positioning overloads so
    // users can still call e.g. span.align(LV_ALIGN_CENTER).
    // Overload resolution distinguishes the two by argument type
    // (lv_align_t vs lv_text_align_t).
    using ObjectMixin<Spangroup>::align;

    constexpr Spangroup() noexcept : ObjectView(nullptr) {}
    constexpr Spangroup(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_spangroup_class; }

    [[nodiscard]] static Spangroup create(lv_obj_t* parent) {
        return Spangroup(wrap, lv_spangroup_create(parent));
    }
    [[nodiscard]] static Spangroup create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Spans ====================

    /// Create a new span
    [[nodiscard]] lv_span_t* new_span() noexcept {
        return lv_spangroup_new_span(m_obj);
    }


    /// Get span by index
    [[nodiscard]] lv_span_t* span(uint32_t idx) const noexcept {
        return lv_spangroup_get_child(m_obj, idx);
    }

    /// Get span count
    [[nodiscard]] uint32_t span_count() const noexcept {
        return lv_spangroup_get_span_count(m_obj);
    }

    // ==================== Span Text ====================

    /// Set span text
    static void span_text(lv_span_t* span, const char* txt) noexcept {
        lv_span_set_text(span, txt);
    }

    /// Set span text static (string must remain valid)
    static void span_text_static(lv_span_t* span, const char* txt) noexcept {
        lv_span_set_text_static(span, txt);
    }

    // ==================== Span Style ====================

    /// Get span style for modification
    [[nodiscard]] static lv_style_t* span_style(lv_span_t* span) noexcept {
        return lv_span_get_style(span);
    }

    // ==================== Layout ====================

    /// Set text align
    Spangroup& align(lv_text_align_t a) noexcept {
        lv_spangroup_set_align(m_obj, a);
        return *this;
    }

    /// Center align
    Spangroup& align_center() noexcept {
        return align(LV_TEXT_ALIGN_CENTER);
    }

    /// Left align
    Spangroup& align_left() noexcept {
        return align(LV_TEXT_ALIGN_LEFT);
    }

    /// Right align
    Spangroup& align_right() noexcept {
        return align(LV_TEXT_ALIGN_RIGHT);
    }

    /// Set overflow mode
    Spangroup& overflow(lv_span_overflow_t mode) noexcept {
        lv_spangroup_set_overflow(m_obj, mode);
        return *this;
    }

    /// Clip overflow
    Spangroup& overflow_clip() noexcept {
        return overflow(LV_SPAN_OVERFLOW_CLIP);
    }

    /// Ellipsis overflow
    Spangroup& overflow_ellipsis() noexcept {
        return overflow(LV_SPAN_OVERFLOW_ELLIPSIS);
    }

    /// Set indent for first line
    Spangroup& indent(int32_t px) noexcept {
        lv_spangroup_set_indent(m_obj, px);
        return *this;
    }

    /// Set wrap mode
    Spangroup& mode(lv_span_mode_t m) noexcept {
        lv_spangroup_set_mode(m_obj, m);
        return *this;
    }

    /// Fixed mode (fixed size)
    Spangroup& mode_fixed() noexcept {
        return mode(LV_SPAN_MODE_FIXED);
    }

    /// Expand mode (grow to fit)
    Spangroup& mode_expand() noexcept {
        return mode(LV_SPAN_MODE_EXPAND);
    }

    /// Break mode (wrap text)
    Spangroup& mode_break() noexcept {
        return mode(LV_SPAN_MODE_BREAK);
    }

    /// Set max lines
    Spangroup& max_lines(int32_t lines) noexcept {
        lv_spangroup_set_max_lines(m_obj, lines);
        return *this;
    }

    // ==================== Refresh ====================

    /// Refresh layout after span changes
    Spangroup& refr_mode() noexcept {
        lv_spangroup_refr_mode(m_obj);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Spangroup& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width
    Spangroup& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }
};

} // namespace lv
