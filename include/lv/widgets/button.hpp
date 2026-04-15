#pragma once

/**
 * @file button.hpp
 * @brief Zero-cost wrapper for LVGL button widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"
#include "../core/color.hpp"
#if LV_USE_LABEL
#include "label.hpp"
#endif

namespace lv {

/**
 * @brief Button widget wrapper
 *
 * Provides a fluent API for creating and configuring buttons.
 * Zero overhead - just wraps the lv_obj_t pointer.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Button : public ObjectView,
               public ObjectMixin<Button>,
               public EventMixin<Button>,
               public StyleMixin<Button> {
public:
    /// Default constructor (null button)
    constexpr Button() noexcept : ObjectView(nullptr) {}

    /// Wrap existing button object
    constexpr Button(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_button_class; }

    /**
     * @brief Create a new Button widget
     * @param parent Parent object
     * @return New Button instance
     */
    [[nodiscard]] static Button create(lv_obj_t* parent) {
        return Button(wrap, lv_button_create(parent));
    }

    [[nodiscard]] static Button create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Text (requires LV_USE_LABEL) ====================

#if LV_USE_LABEL
    /// Set centered text label on the button (creates label on first call)
    Button& text(const char* txt) noexcept {
        lv_obj_t* lbl = first_label_child(m_obj);
        if (!lbl) {
            lbl = lv_label_create(m_obj);
            lv_obj_center(lbl);
        }
        lv_label_set_text(lbl, txt);
        return *this;
    }

    /// Set centered label text with format (creates label on first call)
    template<typename... Args>
    Button& text_fmt(const char* fmt, Args... args) noexcept {
        lv_obj_t* lbl = first_label_child(m_obj);
        if (!lbl) {
            lbl = lv_label_create(m_obj);
            lv_obj_center(lbl);
        }
        lv_label_set_text_fmt(lbl, fmt, args...);
        return *this;
    }

    /// Get the button's label (first child label)
    [[nodiscard]] Label get_label() const noexcept {
        return Label(wrap, first_label_child(m_obj));
    }

    /// Set label text (finds first label child)
    Button& set_text(const char* txt) noexcept {
        lv_obj_t* lbl = first_label_child(m_obj);
        if (lbl) {
            lv_label_set_text(lbl, txt);
        }
        return *this;
    }

private:
    [[nodiscard]] static lv_obj_t* first_label_child(lv_obj_t* parent) noexcept {
        const uint32_t child_count = lv_obj_get_child_count(parent);
        for (uint32_t i = 0; i < child_count; ++i) {
            lv_obj_t* child = lv_obj_get_child(parent, static_cast<int32_t>(i));
            if (child && lv_obj_check_type(child, &lv_label_class)) {
                return child;
            }
        }
        return nullptr;
    }
public:
#endif // LV_USE_LABEL

    // ==================== State ====================

    /// Set checkable state (toggle button)
    Button& checkable(bool enable = true) noexcept {
        if (enable) {
            lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CHECKABLE);
        } else {
            lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_CHECKABLE);
        }
        return *this;
    }

    /// Check if button is checked
    [[nodiscard]] bool checked() const noexcept {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    /// Set checked state
    Button& checked(bool v) noexcept {
        if (v) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    /// Toggle checked state
    Button& toggle() noexcept {
        if (lv_obj_has_state(m_obj, LV_STATE_CHECKED)) {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    /// Disable the button
    Button& disabled(bool v = true) noexcept {
        if (v) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    /// Enable the button
    Button& enabled(bool v = true) noexcept {
        return disabled(!v);
    }

    // ==================== Style Shortcuts ====================

    /// Set background color
    Button& bg(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, 0);
        return *this;
    }

    /// Set background color for pressed state
    Button& bg_pressed(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_STATE_PRESSED);
        return *this;
    }

    /// Set background color for checked state
    Button& bg_checked(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_STATE_CHECKED);
        return *this;
    }

    /// Set corner radius
    Button& radius(int32_t r) noexcept {
        lv_obj_set_style_radius(m_obj, r, 0);
        return *this;
    }

    /// Make fully rounded (pill shape)
    Button& pill() noexcept {
        lv_obj_set_style_radius(m_obj, LV_RADIUS_CIRCLE, 0);
        return *this;
    }

    /// Set shadow
    Button& shadow(int32_t width) noexcept {
        lv_obj_set_style_shadow_width(m_obj, width, 0);
        lv_obj_set_style_shadow_color(m_obj, lv::rgb(0x404040), 0);  // dark gray
        return *this;
    }

    /// Set shadow with custom color
    Button& shadow(int32_t width, lv_color_t color) noexcept {
        lv_obj_set_style_shadow_width(m_obj, width, 0);
        lv_obj_set_style_shadow_color(m_obj, color, 0);
        return *this;
    }

    /// Set border
    Button& border(int32_t width, lv_color_t color) noexcept {
        lv_obj_set_style_border_width(m_obj, width, 0);
        lv_obj_set_style_border_color(m_obj, color, 0);
        return *this;
    }

};


#if LV_USE_LABEL
/**
 * @brief Create a simple text button
 */
inline Button text_button(ObjectView parent, const char* text) {
    return Button::create(parent).text(text);
}

/**
 * @brief Create a toggle button
 */
inline Button toggle_button(ObjectView parent, const char* text) {
    return Button::create(parent).text(text).checkable();
}
#endif // LV_USE_LABEL

} // namespace lv
