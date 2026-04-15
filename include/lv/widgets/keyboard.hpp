#pragma once

/**
 * @file keyboard.hpp
 * @brief Zero-cost wrapper for LVGL keyboard widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Keyboard widget wrapper
 *
 * An on-screen keyboard for text input.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Keyboard : public ObjectView,
            public ObjectMixin<Keyboard>,
                 public EventMixin<Keyboard>,
                 public StyleMixin<Keyboard> {
public:
    constexpr Keyboard() noexcept : ObjectView(nullptr) {}
    constexpr Keyboard(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_keyboard_class; }

    [[nodiscard]] static Keyboard create(lv_obj_t* parent) {
        return Keyboard(wrap, lv_keyboard_create(parent));
    }
    [[nodiscard]] static Keyboard create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Target ====================

    /// Set target textarea
    Keyboard& textarea(ObjectView ta) noexcept {
        lv_keyboard_set_textarea(m_obj, ta.get());
        return *this;
    }

    /// Get target textarea
    [[nodiscard]] ObjectRef get_textarea() const noexcept {
        return ObjectRef(lv_keyboard_get_textarea(m_obj));
    }

    // ==================== Mode ====================

    /// Set keyboard mode
    Keyboard& mode(lv_keyboard_mode_t m) noexcept {
        lv_keyboard_set_mode(m_obj, m);
        return *this;
    }

    /// Get keyboard mode
    [[nodiscard]] lv_keyboard_mode_t mode() const noexcept {
        return lv_keyboard_get_mode(m_obj);
    }

    /// Text lowercase mode
    Keyboard& mode_text_lower() noexcept {
        return mode(LV_KEYBOARD_MODE_TEXT_LOWER);
    }

    /// Text uppercase mode
    Keyboard& mode_text_upper() noexcept {
        return mode(LV_KEYBOARD_MODE_TEXT_UPPER);
    }

    /// Special characters mode
    Keyboard& mode_special() noexcept {
        return mode(LV_KEYBOARD_MODE_SPECIAL);
    }

    /// Number mode
    Keyboard& mode_number() noexcept {
        return mode(LV_KEYBOARD_MODE_NUMBER);
    }

    /// User-defined mode 1
    Keyboard& mode_user1() noexcept {
        return mode(LV_KEYBOARD_MODE_USER_1);
    }

    /// User-defined mode 2
    Keyboard& mode_user2() noexcept {
        return mode(LV_KEYBOARD_MODE_USER_2);
    }

    /// User-defined mode 3
    Keyboard& mode_user3() noexcept {
        return mode(LV_KEYBOARD_MODE_USER_3);
    }

    /// User-defined mode 4
    Keyboard& mode_user4() noexcept {
        return mode(LV_KEYBOARD_MODE_USER_4);
    }

    // ==================== Custom Map ====================

    /// Set custom button map for a mode
    Keyboard& map(lv_keyboard_mode_t m, const char* const map_array[],
                  const lv_buttonmatrix_ctrl_t ctrl_array[]) noexcept {
        lv_keyboard_set_map(m_obj, m, map_array, ctrl_array);
        return *this;
    }

    // ==================== Popovers ====================

    /// Enable button popovers
    Keyboard& popovers(bool enable) noexcept {
        lv_keyboard_set_popovers(m_obj, enable);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Keyboard& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width
    Keyboard& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    /// Set height
    Keyboard& height(int32_t h) noexcept {
        lv_obj_set_height(m_obj, h);
        return *this;
    }

    /// Fill parent width
    Keyboard& fill_width() noexcept {
        lv_obj_set_width(m_obj, LV_PCT(100));
        return *this;
    }

    // ==================== Position ====================

    /// Align relative to parent
    Keyboard& align(lv_align_t align, int32_t x_ofs = 0, int32_t y_ofs = 0) noexcept {
        lv_obj_align(m_obj, align, x_ofs, y_ofs);
        return *this;
    }

    /// Align to bottom of parent
    Keyboard& align_bottom() noexcept {
        return align(LV_ALIGN_BOTTOM_MID);
    }

    // ==================== Visibility ====================

    /// Hide the keyboard
    Keyboard& hide() noexcept {
        lv_obj_add_flag(m_obj, LV_OBJ_FLAG_HIDDEN);
        return *this;
    }

    /// Show the keyboard
    Keyboard& show() noexcept {
        lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_HIDDEN);
        return *this;
    }

    /// Check if keyboard is hidden
    [[nodiscard]] bool is_hidden() const noexcept {
        return lv_obj_has_flag(m_obj, LV_OBJ_FLAG_HIDDEN);
    }
};

} // namespace lv
