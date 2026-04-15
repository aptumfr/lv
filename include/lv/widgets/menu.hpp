#pragma once

/**
 * @file menu.hpp
 * @brief Zero-cost wrapper for LVGL menu widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Menu widget wrapper
 *
 * A hierarchical menu with pages and navigation.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Menu : public ObjectView,
            public ObjectMixin<Menu>,
             public EventMixin<Menu>,
             public StyleMixin<Menu> {
public:
    constexpr Menu() noexcept : ObjectView(nullptr) {}

    constexpr Menu(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_menu_class; }

    [[nodiscard]] static Menu create(lv_obj_t* parent) {
        return Menu(wrap, lv_menu_create(parent));
    }

    [[nodiscard]] static Menu create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Pages ====================

    /// Create a new page
    [[nodiscard]] ObjectRef page_create(const char* title) noexcept {
        return ObjectRef(lv_menu_page_create(m_obj, const_cast<char*>(title)));
    }

    /// Set current page
    Menu& page(lv_obj_t* page_obj) noexcept {
        lv_menu_set_page(m_obj, page_obj);
        return *this;
    }

    /// Get current page
    [[nodiscard]] ObjectRef page() const noexcept {
        return ObjectRef(lv_menu_get_cur_main_page(m_obj));
    }

    // ==================== Sections ====================

    /// Create a section container
    [[nodiscard]] ObjectRef section_create(lv_obj_t* page_obj) noexcept {
        return ObjectRef(lv_menu_section_create(page_obj));
    }

    /// Create a menu item container
    [[nodiscard]] ObjectRef cont_create(lv_obj_t* parent_obj) noexcept {
        return ObjectRef(lv_menu_cont_create(parent_obj));
    }

    // ==================== Sidebar ====================

    /// Set sidebar page
    Menu& sidebar_page(lv_obj_t* page_obj) noexcept {
        lv_menu_set_sidebar_page(m_obj, page_obj);
        return *this;
    }

    /// Get sidebar page
    [[nodiscard]] ObjectRef sidebar_page() const noexcept {
        return ObjectRef(lv_menu_get_cur_sidebar_page(m_obj));
    }

    /// Set sidebar header mode
    Menu& sidebar_header_mode(lv_menu_mode_header_t mode) noexcept {
        lv_menu_set_mode_header(m_obj, mode);
        return *this;
    }

    // ==================== Header ====================

    /// Set header mode
    Menu& header_mode(lv_menu_mode_header_t mode) noexcept {
        lv_menu_set_mode_header(m_obj, mode);
        return *this;
    }

    /// Get main header
    [[nodiscard]] ObjectRef main_header() const noexcept {
        return ObjectRef(lv_menu_get_main_header(m_obj));
    }

    /// Get main header back button
    [[nodiscard]] ObjectRef main_header_back_button() const noexcept {
        return ObjectRef(lv_menu_get_main_header_back_button(m_obj));
    }

    /// Get sidebar header
    [[nodiscard]] ObjectRef sidebar_header() const noexcept {
        return ObjectRef(lv_menu_get_sidebar_header(m_obj));
    }

    /// Get sidebar header back button
    [[nodiscard]] ObjectRef sidebar_header_back_button() const noexcept {
        return ObjectRef(lv_menu_get_sidebar_header_back_button(m_obj));
    }

    // ==================== Navigation ====================

    /// Set load page event for a container
    Menu& set_load_page_event(lv_obj_t* cont, lv_obj_t* page_obj) noexcept {
        lv_menu_set_load_page_event(m_obj, cont, page_obj);
        return *this;
    }

    /// Clear history
    Menu& clear_history() noexcept {
        lv_menu_clear_history(m_obj);
        return *this;
    }

    // ==================== Root Back Button ====================

    /// Set root back button mode
    Menu& root_back_button_mode(lv_menu_mode_root_back_button_t mode) noexcept {
        lv_menu_set_mode_root_back_button(m_obj, mode);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Menu& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
