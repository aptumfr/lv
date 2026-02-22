#pragma once

/**
 * @file msgbox.hpp
 * @brief Zero-cost wrapper for LVGL message box widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Message Box widget wrapper
 *
 * A modal dialog with title, text, and buttons.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Msgbox : public ObjectView,
            public ObjectMixin<Msgbox>,
               public EventMixin<Msgbox>,
               public StyleMixin<Msgbox> {
public:
    constexpr Msgbox() noexcept : ObjectView(nullptr) {}

    constexpr Msgbox(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// Create message box (parent = nullptr for modal on active screen)
    [[nodiscard]] static Msgbox create(lv_obj_t* parent = nullptr) {
        return Msgbox(wrap, lv_msgbox_create(parent));
    }

    [[nodiscard]] static Msgbox create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Content ====================

    /// Add title
    [[nodiscard]] ObjectView add_title(const char* txt) noexcept {
        return ObjectView(lv_msgbox_add_title(m_obj, txt));
    }

    /// Add text content
    [[nodiscard]] ObjectView add_text(const char* txt) noexcept {
        return ObjectView(lv_msgbox_add_text(m_obj, txt));
    }

    /// Add header button (e.g., close button)
    [[nodiscard]] ObjectView add_header_button(const void* icon) noexcept {
        return ObjectView(lv_msgbox_add_header_button(m_obj, icon));
    }

    /// Add footer button
    [[nodiscard]] ObjectView add_footer_button(const char* txt) noexcept {
        return ObjectView(lv_msgbox_add_footer_button(m_obj, txt));
    }

    /// Add close button to header
    [[nodiscard]] ObjectView add_close_button() noexcept {
        return ObjectView(lv_msgbox_add_close_button(m_obj));
    }

    // ==================== Parts ====================

    /// Get header object
    [[nodiscard]] ObjectView header() const noexcept {
        return ObjectView(lv_msgbox_get_header(m_obj));
    }

    /// Get footer object
    [[nodiscard]] ObjectView footer() const noexcept {
        return ObjectView(lv_msgbox_get_footer(m_obj));
    }

    /// Get content object
    [[nodiscard]] ObjectView content() const noexcept {
        return ObjectView(lv_msgbox_get_content(m_obj));
    }

    /// Get title label
    [[nodiscard]] ObjectView title() const noexcept {
        return ObjectView(lv_msgbox_get_title(m_obj));
    }

    // ==================== Actions ====================

    /// Close the message box
    void close() noexcept {
        lv_msgbox_close(m_obj);
    }

    /// Close the message box asynchronously
    void close_async() noexcept {
        lv_msgbox_close_async(m_obj);
    }

    // ==================== Size ====================

    /// Set size
    Msgbox& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width
    Msgbox& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }
};

namespace detail {
    template<> inline const lv_obj_class_t*
    widget_lv_class<Msgbox>() noexcept { return &lv_msgbox_class; }
}

} // namespace lv
