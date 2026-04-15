#pragma once

/**
 * @file buttonmatrix.hpp
 * @brief Zero-cost wrapper for LVGL button matrix widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Button Matrix widget wrapper
 *
 * A grid of buttons defined by a string map.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class ButtonMatrix : public ObjectView,
            public ObjectMixin<ButtonMatrix>,
                     public EventMixin<ButtonMatrix>,
                     public StyleMixin<ButtonMatrix> {
public:
    constexpr ButtonMatrix() noexcept : ObjectView(nullptr) {}
    constexpr ButtonMatrix(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_buttonmatrix_class; }

    [[nodiscard]] static ButtonMatrix create(lv_obj_t* parent) {
        return ButtonMatrix(wrap, lv_buttonmatrix_create(parent));
    }
    [[nodiscard]] static ButtonMatrix create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Map ====================

    /// Set button map (array must remain valid, last element must be "")
    ButtonMatrix& map(const char* const map_array[]) noexcept {
        lv_buttonmatrix_set_map(m_obj, map_array);
        return *this;
    }

    /// Set control map for all buttons
    ButtonMatrix& ctrl_map(const lv_buttonmatrix_ctrl_t ctrl_array[]) noexcept {
        lv_buttonmatrix_set_ctrl_map(m_obj, ctrl_array);
        return *this;
    }

    // ==================== Selection ====================

    /// Get selected button index
    [[nodiscard]] uint32_t selected() const noexcept {
        return lv_buttonmatrix_get_selected_button(m_obj);
    }

    /// Set selected button
    ButtonMatrix& selected(uint32_t btn_id) noexcept {
        lv_buttonmatrix_set_selected_button(m_obj, btn_id);
        return *this;
    }

    /// Get button text
    [[nodiscard]] const char* button_text(uint32_t btn_id) const noexcept {
        return lv_buttonmatrix_get_button_text(m_obj, btn_id);
    }

    // ==================== Button Control ====================

    /// Set control flags for a button
    ButtonMatrix& button_ctrl(uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl) noexcept {
        lv_buttonmatrix_set_button_ctrl(m_obj, btn_id, ctrl);
        return *this;
    }

    /// Clear control flags for a button
    ButtonMatrix& clear_button_ctrl(uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl) noexcept {
        lv_buttonmatrix_clear_button_ctrl(m_obj, btn_id, ctrl);
        return *this;
    }

    /// Set control flags for all buttons
    ButtonMatrix& button_ctrl_all(lv_buttonmatrix_ctrl_t ctrl) noexcept {
        lv_buttonmatrix_set_button_ctrl_all(m_obj, ctrl);
        return *this;
    }

    /// Clear control flags for all buttons
    ButtonMatrix& clear_button_ctrl_all(lv_buttonmatrix_ctrl_t ctrl) noexcept {
        lv_buttonmatrix_clear_button_ctrl_all(m_obj, ctrl);
        return *this;
    }

    /// Check if button has control flag
    [[nodiscard]] bool has_button_ctrl(uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl) const noexcept {
        return lv_buttonmatrix_has_button_ctrl(m_obj, btn_id, ctrl);
    }

    // ==================== One Check ====================

    /// Enable one-check mode (only one button can be checked)
    ButtonMatrix& one_checked(bool enable) noexcept {
        lv_buttonmatrix_set_one_checked(m_obj, enable);
        return *this;
    }

    /// Check if one-check mode is enabled
    [[nodiscard]] bool one_checked() const noexcept {
        return lv_buttonmatrix_get_one_checked(m_obj);
    }

    // ==================== Size ====================

    /// Set size
    ButtonMatrix& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width
    ButtonMatrix& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    /// Set height
    ButtonMatrix& height(int32_t h) noexcept {
        lv_obj_set_height(m_obj, h);
        return *this;
    }
};

} // namespace lv
