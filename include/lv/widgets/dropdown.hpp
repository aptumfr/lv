#pragma once

/**
 * @file dropdown.hpp
 * @brief Zero-cost wrapper for LVGL dropdown widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Dropdown widget wrapper
 *
 * A dropdown list for selecting from multiple options.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Dropdown : public ObjectView,
            public ObjectMixin<Dropdown>,
                 public EventMixin<Dropdown>,
                 public StyleMixin<Dropdown> {
public:
    constexpr Dropdown() noexcept : ObjectView(nullptr) {}
    constexpr Dropdown(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_dropdown_class; }

    [[nodiscard]] static Dropdown create(lv_obj_t* parent) {
        return Dropdown(wrap, lv_dropdown_create(parent));
    }
    [[nodiscard]] static Dropdown create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Options ====================

    /// Set options (newline separated)
    Dropdown& options(const char* opts) noexcept {
        lv_dropdown_set_options(m_obj, opts);
        return *this;
    }

    /// Set static options (string must remain valid)
    Dropdown& options_static(const char* opts) noexcept {
        lv_dropdown_set_options_static(m_obj, opts);
        return *this;
    }

    /// Add an option at index
    Dropdown& add_option(const char* opt, uint32_t pos = LV_DROPDOWN_POS_LAST) noexcept {
        lv_dropdown_add_option(m_obj, opt, pos);
        return *this;
    }

    /// Clear all options
    Dropdown& clear_options() noexcept {
        lv_dropdown_clear_options(m_obj);
        return *this;
    }

    /// Get number of options
    [[nodiscard]] uint32_t option_count() const noexcept {
        return lv_dropdown_get_option_count(m_obj);
    }

    // ==================== Selection ====================

    /// Get selected index
    [[nodiscard]] uint32_t selected() const noexcept {
        return lv_dropdown_get_selected(m_obj);
    }

    /// Set selected index
    Dropdown& selected(uint32_t idx) noexcept {
        lv_dropdown_set_selected(m_obj, idx);
        return *this;
    }

    /// Get selected option text
    void selected_str(char* buf, uint32_t buf_size) const noexcept {
        lv_dropdown_get_selected_str(m_obj, buf, buf_size);
    }

    // ==================== Appearance ====================

    /// Set the text shown when closed (nullptr = show selected)
    Dropdown& text(const char* txt) noexcept {
        lv_dropdown_set_text(m_obj, txt);
        return *this;
    }

    /// Set dropdown direction
    Dropdown& dir(lv_dir_t d) noexcept {
        lv_dropdown_set_dir(m_obj, d);
        return *this;
    }

    /// Open downward (default)
    Dropdown& dir_down() noexcept {
        return dir(LV_DIR_BOTTOM);
    }

    /// Open upward
    Dropdown& dir_up() noexcept {
        return dir(LV_DIR_TOP);
    }

    /// Open to the left
    Dropdown& dir_left() noexcept {
        return dir(LV_DIR_LEFT);
    }

    /// Open to the right
    Dropdown& dir_right() noexcept {
        return dir(LV_DIR_RIGHT);
    }

    /// Set symbol (arrow icon)
    Dropdown& symbol(const void* sym) noexcept {
        lv_dropdown_set_symbol(m_obj, sym);
        return *this;
    }

    /// Remove symbol
    Dropdown& no_symbol() noexcept {
        return symbol(nullptr);
    }

    // ==================== Highlight ====================

    /// Enable/disable highlighting of the selected item
    Dropdown& selected_highlight(bool enable = true) noexcept {
        lv_dropdown_set_selected_highlight(m_obj, enable);
        return *this;
    }

    // ==================== State ====================

    /// Check if dropdown list is open
    [[nodiscard]] bool is_open() const noexcept {
        return lv_dropdown_is_open(m_obj);
    }

    /// Open the dropdown list
    Dropdown& open() noexcept {
        lv_dropdown_open(m_obj);
        return *this;
    }

    /// Close the dropdown list
    Dropdown& close() noexcept {
        lv_dropdown_close(m_obj);
        return *this;
    }

#if LV_USE_OBSERVER
    /// Bind dropdown value to a subject
    Dropdown& bind_value(lv_subject_t* subject) noexcept {
        lv_dropdown_bind_value(m_obj, subject);
        return *this;
    }
#endif

    /// Disable the dropdown
    Dropdown& disabled(bool value = true) noexcept {
        if (value) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    // ==================== Size ====================

    /// Set width
    Dropdown& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    /// Fill parent width
    Dropdown& fill_width() noexcept {
        lv_obj_set_width(m_obj, LV_PCT(100));
        return *this;
    }

    // ==================== List Access ====================

    /// Get the dropdown list object (for styling)
    [[nodiscard]] ObjectRef list() const noexcept {
        return ObjectRef(lv_dropdown_get_list(m_obj));
    }
};

} // namespace lv
