#pragma once

/**
 * @file list.hpp
 * @brief Zero-cost wrapper for LVGL list widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief List widget wrapper
 *
 * A scrollable list with text items and optional icons.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class List : public ObjectView,
            public ObjectMixin<List>,
             public EventMixin<List>,
             public StyleMixin<List> {
public:
    constexpr List() noexcept : ObjectView(nullptr) {}

    constexpr List(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_list_class; }

    [[nodiscard]] static List create(lv_obj_t* parent) {
        return List(wrap, lv_list_create(parent));
    }

    [[nodiscard]] static List create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Items ====================

    /// Add text item (returns the button object)
    [[nodiscard]] ObjectRef add_text(const char* txt) noexcept {
        return ObjectRef(lv_list_add_text(m_obj, txt));
    }

    /// Add button item with icon and text
    [[nodiscard]] ObjectRef add_button(const void* icon, const char* txt) noexcept {
        return ObjectRef(lv_list_add_button(m_obj, icon, txt));
    }

    /// Add button item with text only
    [[nodiscard]] ObjectRef add_button(const char* txt) noexcept {
        return ObjectRef(lv_list_add_button(m_obj, nullptr, txt));
    }

    /// Get button text
    [[nodiscard]] static const char* button_text(lv_obj_t* btn) noexcept {
        return lv_list_get_button_text(nullptr, btn);
    }

    // ==================== Size ====================

    /// Set size
    List& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width
    List& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    /// Set height
    List& height(int32_t h) noexcept {
        lv_obj_set_height(m_obj, h);
        return *this;
    }
};

} // namespace lv
