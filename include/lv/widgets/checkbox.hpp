#pragma once

/**
 * @file checkbox.hpp
 * @brief Zero-cost wrapper for LVGL checkbox widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Checkbox widget wrapper
 *
 * A checkbox with optional label text.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Checkbox : public ObjectView,
            public ObjectMixin<Checkbox>,
                 public EventMixin<Checkbox>,
                 public StyleMixin<Checkbox> {
public:
    constexpr Checkbox() noexcept : ObjectView(nullptr) {}
    constexpr Checkbox(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_checkbox_class; }

    [[nodiscard]] static Checkbox create(lv_obj_t* parent) {
        return Checkbox(wrap, lv_checkbox_create(parent));
    }
    [[nodiscard]] static Checkbox create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Text ====================

    /// Set checkbox text
    Checkbox& text(const char* txt) noexcept {
        lv_checkbox_set_text(m_obj, txt);
        return *this;
    }

    /// Set static text (string must remain valid)
    Checkbox& text_static(const char* txt) noexcept {
        lv_checkbox_set_text_static(m_obj, txt);
        return *this;
    }

    /// Get checkbox text
    [[nodiscard]] const char* get_text() const noexcept {
        return lv_checkbox_get_text(m_obj);
    }

    // ==================== State ====================

    /// Check if checkbox is checked
    [[nodiscard]] bool is_checked() const noexcept {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    /// Set checked state
    Checkbox& checked(bool value = true) noexcept {
        if (value) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    /// Set unchecked
    Checkbox& unchecked() noexcept {
        return checked(false);
    }

    /// Toggle checked state
    Checkbox& toggle() noexcept {
        return checked(!is_checked());
    }

    /// Disable the checkbox
    Checkbox& disabled(bool value = true) noexcept {
        if (value) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set the indicator (box) color when checked
    Checkbox& indicator_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | LV_STATE_CHECKED);
        return *this;
    }

    /// Set the border color of the indicator
    Checkbox& indicator_border_color(lv_color_t color) noexcept {
        lv_obj_set_style_border_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set text color
    Checkbox& text_color(lv_color_t color) noexcept {
        lv_obj_set_style_text_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

    /// Set text font
    Checkbox& font(const lv_font_t* f) noexcept {
        lv_obj_set_style_text_font(m_obj, f, LV_PART_MAIN);
        return *this;
    }
};

} // namespace lv
