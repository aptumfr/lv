#pragma once

/**
 * @file textarea.hpp
 * @brief Zero-cost wrapper for LVGL textarea widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Textarea widget wrapper
 *
 * A multi-line text input area.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Textarea : public ObjectView,
            public ObjectMixin<Textarea>,
                 public EventMixin<Textarea>,
                 public StyleMixin<Textarea> {
public:
    constexpr Textarea() noexcept : ObjectView(nullptr) {}
    constexpr Textarea(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_textarea_class; }

    [[nodiscard]] static Textarea create(lv_obj_t* parent) {
        return Textarea(wrap, lv_textarea_create(parent));
    }
    [[nodiscard]] static Textarea create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Text ====================

    /// Set text content
    Textarea& text(const char* txt) noexcept {
        lv_textarea_set_text(m_obj, txt);
        return *this;
    }

    /// Get current text
    [[nodiscard]] const char* text() const noexcept {
        return lv_textarea_get_text(m_obj);
    }

    /// Add text at cursor position
    Textarea& add_text(const char* txt) noexcept {
        lv_textarea_add_text(m_obj, txt);
        return *this;
    }

    /// Add single character at cursor
    Textarea& add_char(uint32_t c) noexcept {
        lv_textarea_add_char(m_obj, c);
        return *this;
    }

    /// Delete character before cursor
    Textarea& del_char() noexcept {
        lv_textarea_delete_char(m_obj);
        return *this;
    }

    /// Delete character after cursor
    Textarea& del_char_forward() noexcept {
        lv_textarea_delete_char_forward(m_obj);
        return *this;
    }

    // ==================== Placeholder ====================

    /// Set placeholder text
    Textarea& placeholder(const char* txt) noexcept {
        lv_textarea_set_placeholder_text(m_obj, txt);
        return *this;
    }

    /// Get placeholder text
    [[nodiscard]] const char* placeholder() const noexcept {
        return lv_textarea_get_placeholder_text(m_obj);
    }

    // ==================== Cursor ====================

    /// Set cursor position
    Textarea& cursor_pos(int32_t pos) noexcept {
        lv_textarea_set_cursor_pos(m_obj, pos);
        return *this;
    }

    /// Get cursor position
    [[nodiscard]] int32_t cursor_pos() const noexcept {
        return lv_textarea_get_cursor_pos(m_obj);
    }

    /// Move cursor to start
    Textarea& cursor_start() noexcept {
        lv_textarea_set_cursor_pos(m_obj, 0);
        return *this;
    }

    /// Move cursor to end
    Textarea& cursor_end() noexcept {
        lv_textarea_set_cursor_pos(m_obj, LV_TEXTAREA_CURSOR_LAST);
        return *this;
    }

    /// Enable/disable cursor click positioning
    Textarea& cursor_click_pos(bool enable) noexcept {
        lv_textarea_set_cursor_click_pos(m_obj, enable);
        return *this;
    }

    // ==================== Input Constraints ====================

    /// Set maximum length
    Textarea& max_length(uint32_t len) noexcept {
        lv_textarea_set_max_length(m_obj, len);
        return *this;
    }

    /// Set accepted characters
    Textarea& accepted_chars(const char* chars) noexcept {
        lv_textarea_set_accepted_chars(m_obj, chars);
        return *this;
    }

    /// Set password mode
    Textarea& password_mode(bool enable) noexcept {
        lv_textarea_set_password_mode(m_obj, enable);
        return *this;
    }

    /// Check if password mode is enabled
    [[nodiscard]] bool password_mode() const noexcept {
        return lv_textarea_get_password_mode(m_obj);
    }

    /// Set password bullet character
    Textarea& password_bullet(const char* bullet) noexcept {
        lv_textarea_set_password_bullet(m_obj, bullet);
        return *this;
    }

    /// Set password show time (0 = don't show)
    Textarea& password_show_time(uint32_t time_ms) noexcept {
        lv_textarea_set_password_show_time(m_obj, time_ms);
        return *this;
    }

    // ==================== Behavior ====================

    /// Set one-line mode
    Textarea& one_line(bool enable) noexcept {
        lv_textarea_set_one_line(m_obj, enable);
        return *this;
    }

    /// Check if one-line mode is enabled
    [[nodiscard]] bool one_line() const noexcept {
        return lv_textarea_get_one_line(m_obj);
    }

    /// Set text selection enabled
    Textarea& text_selection(bool enable) noexcept {
        lv_textarea_set_text_selection(m_obj, enable);
        return *this;
    }

    /// Check if text selection is enabled
    [[nodiscard]] bool text_selection() const noexcept {
        return lv_textarea_get_text_selection(m_obj);
    }

    /// Clear selection
    Textarea& clear_selection() noexcept {
        lv_textarea_clear_selection(m_obj);
        return *this;
    }

    // size(), width(), height(), fill_width() are inherited from ObjectMixin<Textarea>.

    // ==================== Style Shortcuts ====================

    /// Set text font
    Textarea& font(const lv_font_t* f) noexcept {
        lv_obj_set_style_text_font(m_obj, f, LV_PART_MAIN);
        return *this;
    }

    /// Set text color
    Textarea& text_color(lv_color_t color) noexcept {
        lv_obj_set_style_text_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

};

} // namespace lv
