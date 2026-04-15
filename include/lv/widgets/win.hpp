#pragma once

/**
 * @file win.hpp
 * @brief Zero-cost wrapper for LVGL window widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Window widget wrapper
 *
 * A window with header, title, and content area.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Window : public ObjectView,
            public ObjectMixin<Window>,
               public EventMixin<Window>,
               public StyleMixin<Window> {
public:
    constexpr Window() noexcept : ObjectView(nullptr) {}

    constexpr Window(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_win_class; }

    [[nodiscard]] static Window create(lv_obj_t* parent) {
        return Window(wrap, lv_win_create(parent));
    }

    [[nodiscard]] static Window create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Header ====================

    /// Add title to header
    [[nodiscard]] ObjectRef add_title(const char* txt) noexcept {
        return ObjectRef(lv_win_add_title(m_obj, txt));
    }

    /// Add button to header (returns button object)
    [[nodiscard]] ObjectRef add_button(const void* icon, int32_t btn_w) noexcept {
        return ObjectRef(lv_win_add_button(m_obj, icon, btn_w));
    }

    /// Get header object
    [[nodiscard]] ObjectRef header() const noexcept {
        return ObjectRef(lv_win_get_header(m_obj));
    }

    // ==================== Content ====================

    /// Get content object (where to add child widgets)
    [[nodiscard]] ObjectRef content() const noexcept {
        return ObjectRef(lv_win_get_content(m_obj));
    }

    // ==================== Size ====================

    /// Set size
    Window& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Fill parent
    Window& fill() noexcept {
        lv_obj_set_size(m_obj, LV_PCT(100), LV_PCT(100));
        return *this;
    }
};

} // namespace lv
