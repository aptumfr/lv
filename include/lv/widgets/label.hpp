#pragma once

/**
 * @file label.hpp
 * @brief Zero-cost wrapper for LVGL label widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"
#include "../core/string_utils.hpp"
#include <string_view>
#include <type_traits>

namespace lv {

// Forward declaration – full definition in core/state.hpp.
// bind_text() below requires State<T> to be complete at the point of instantiation.
template<typename T> class State;

/**
 * @brief Label widget wrapper
 *
 * Provides a fluent API for creating and configuring labels.
 * Zero overhead - just wraps the lv_obj_t pointer.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Label : public ObjectView,
              public ObjectMixin<Label>,
              public EventMixin<Label>,
              public StyleMixin<Label> {
public:
    /// Default constructor (null label)
    constexpr Label() noexcept : ObjectView(nullptr) {}

    /// Wrap existing label object
    constexpr Label(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_label_class; }

    /**
     * @brief Create a new Label widget
     * @param parent Parent object
     * @return New Label instance
     */
    [[nodiscard]] static Label create(lv_obj_t* parent) {
        return Label(wrap, lv_label_create(parent));
    }

    [[nodiscard]] static Label create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Text ====================

    /// Set label text (LVGL copies the string)
    Label& text(const char* txt) noexcept {
        lv_label_set_text(m_obj, txt);
        return *this;
    }

    /// Set label text with format string
    template<typename... Args>
    Label& text_fmt(const char* fmt, Args... args) noexcept {
        lv_label_set_text_fmt(m_obj, fmt, args...);
        return *this;
    }

    /// Set label text (static - string must remain valid)
    Label& text_static(const char* txt) noexcept {
        lv_label_set_text_static(m_obj, txt);
        return *this;
    }

    /// Set text from string_view (safely copies to ensure null-termination)
    Label& text(std::string_view txt) noexcept {
        with_cstr(txt, [this](const char* cstr) {
            lv_label_set_text(m_obj, cstr);
        });
        return *this;
    }

    /// Get current text
    [[nodiscard]] const char* get_text() const noexcept {
        return lv_label_get_text(m_obj);
    }

    // ==================== Long Mode ====================

    /// Set long text mode (wrap, scroll, dot, etc.)
    Label& long_mode(lv_label_long_mode_t mode) noexcept {
        lv_label_set_long_mode(m_obj, mode);
        return *this;
    }

    /// Wrap long text to multiple lines
    Label& text_wrap() noexcept {
        lv_label_set_long_mode(m_obj, LV_LABEL_LONG_WRAP);
        return *this;
    }

    /// Scroll long text horizontally
    Label& scroll() noexcept {
        lv_label_set_long_mode(m_obj, LV_LABEL_LONG_SCROLL);
        return *this;
    }

    /// Show dots (...) for truncated text
    Label& dot() noexcept {
        lv_label_set_long_mode(m_obj, LV_LABEL_LONG_DOT);
        return *this;
    }

    /// Scroll text in circular motion
    Label& scroll_circular() noexcept {
        lv_label_set_long_mode(m_obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
        return *this;
    }

    /// Clip long text
    Label& clip() noexcept {
        lv_label_set_long_mode(m_obj, LV_LABEL_LONG_CLIP);
        return *this;
    }

    // ==================== Text Selection ====================

    /// Enable text selection
    Label& selectable(bool enable = true) noexcept {
        if (enable) {
            lv_label_set_text_selection_start(m_obj, 0);
            lv_label_set_text_selection_end(m_obj, LV_LABEL_TEXT_SELECTION_OFF);
        }
        return *this;
    }

    /// Set selection range
    Label& selection(uint32_t start, uint32_t end) noexcept {
        lv_label_set_text_selection_start(m_obj, start);
        lv_label_set_text_selection_end(m_obj, end);
        return *this;
    }

    // ==================== Recolor ====================

    /// Enable recoloring with #RRGGBB# syntax in text
    Label& recolor(bool enable = true) noexcept {
        lv_label_set_recolor(m_obj, enable);
        return *this;
    }

#if LV_USE_TRANSLATION
    // ==================== Translation ====================

    /// Bind a translation tag to the label
    Label& translation_tag(const char* tag) noexcept {
        lv_label_set_translation_tag(m_obj, tag);
        return *this;
    }
#endif

    // ==================== Style Shortcuts ====================

    /// Set text font
    Label& font(const lv_font_t* f) noexcept {
        lv_obj_set_style_text_font(m_obj, f, 0);
        return *this;
    }

    /// Set text color
    Label& color(lv_color_t c) noexcept {
        lv_obj_set_style_text_color(m_obj, c, 0);
        return *this;
    }

    /// Set text alignment
    Label& align_text(lv_text_align_t a) noexcept {
        lv_obj_set_style_text_align(m_obj, a, 0);
        return *this;
    }

    /// Center text
    Label& center_text() noexcept {
        lv_obj_set_style_text_align(m_obj, LV_TEXT_ALIGN_CENTER, 0);
        return *this;
    }

    /// Left-align text
    Label& left_text() noexcept {
        lv_obj_set_style_text_align(m_obj, LV_TEXT_ALIGN_LEFT, 0);
        return *this;
    }

    /// Right-align text
    Label& right_text() noexcept {
        lv_obj_set_style_text_align(m_obj, LV_TEXT_ALIGN_RIGHT, 0);
        return *this;
    }

    /// Set letter spacing
    Label& letter_space(int32_t space) noexcept {
        lv_obj_set_style_text_letter_space(m_obj, space, 0);
        return *this;
    }

    /// @deprecated Use letter_space() instead
    [[deprecated("Use letter_space() instead")]]
    Label& text_letter_space(int32_t space) noexcept {
        return letter_space(space);
    }

    /// Set line spacing
    Label& line_space(int32_t space) noexcept {
        lv_obj_set_style_text_line_space(m_obj, space, 0);
        return *this;
    }

    // ==================== Geometry ====================

    /// Get text size info
    [[nodiscard]] lv_point_t text_size() const noexcept {
        lv_point_t size;
        lv_text_get_size(&size, lv_label_get_text(m_obj),
                         lv_obj_get_style_text_font(m_obj, LV_PART_MAIN),
                         lv_obj_get_style_text_letter_space(m_obj, LV_PART_MAIN),
                         lv_obj_get_style_text_line_space(m_obj, LV_PART_MAIN),
                         lv_obj_get_width(m_obj),
                         LV_TEXT_FLAG_NONE);
        return size;
    }

    // ==================== Binding (for reactive state) ====================

#if LV_USE_OBSERVER
    /// Bind label text to an integer subject with format
    Label& bind_text(lv_subject_t* subject, const char* fmt = "%d") noexcept {
        lv_label_bind_text(m_obj, subject, fmt);
        return *this;
    }

    /// Bind to State<int>
    template<typename T>
        requires (std::is_integral_v<T> && sizeof(T) <= sizeof(int32_t))
    Label& bind_text(State<T>& state, const char* fmt = "%d") noexcept {
        lv_label_bind_text(m_obj, state.subject(), fmt);
        return *this;
    }
#endif
};

} // namespace lv
