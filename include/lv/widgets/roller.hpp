#pragma once

/**
 * @file roller.hpp
 * @brief Zero-cost wrapper for LVGL roller widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Roller widget wrapper
 *
 * A scrollable list selector (like iOS date picker).
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Roller : public ObjectView,
            public ObjectMixin<Roller>,
               public EventMixin<Roller>,
               public StyleMixin<Roller> {
public:
    constexpr Roller() noexcept : ObjectView(nullptr) {}
    constexpr Roller(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_roller_class; }

    [[nodiscard]] static Roller create(lv_obj_t* parent) {
        return Roller(wrap, lv_roller_create(parent));
    }
    [[nodiscard]] static Roller create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Options ====================

    /// Set options (newline separated)
    Roller& options(const char* opts, lv_roller_mode_t mode = LV_ROLLER_MODE_NORMAL) noexcept {
        lv_roller_set_options(m_obj, opts, mode);
        return *this;
    }

    /// Set infinite scrolling mode
    Roller& options_infinite(const char* opts) noexcept {
        lv_roller_set_options(m_obj, opts, LV_ROLLER_MODE_INFINITE);
        return *this;
    }

    /// Get number of options
    [[nodiscard]] uint32_t option_count() const noexcept {
        return lv_roller_get_option_count(m_obj);
    }

    // ==================== Selection ====================

    /// Get selected index
    [[nodiscard]] uint32_t selected() const noexcept {
        return lv_roller_get_selected(m_obj);
    }

    /// Set selected index
    Roller& selected(uint32_t idx) noexcept {
        lv_roller_set_selected(m_obj, idx, LV_ANIM_OFF);
        return *this;
    }

    /// Set selected index with animation
    Roller& selected_anim(uint32_t idx) noexcept {
        lv_roller_set_selected(m_obj, idx, LV_ANIM_ON);
        return *this;
    }

    /// Get selected option text
    void selected_str(char* buf, uint32_t buf_size) const noexcept {
        lv_roller_get_selected_str(m_obj, buf, buf_size);
    }

    // ==================== Appearance ====================

    /// Set number of visible rows
    Roller& visible_rows(uint32_t rows) noexcept {
        lv_roller_set_visible_row_count(m_obj, rows);
        return *this;
    }

    // ==================== Size ====================

    /// Set width
    Roller& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set text font
    Roller& font(const lv_font_t* f) noexcept {
        lv_obj_set_style_text_font(m_obj, f, LV_PART_MAIN);
        return *this;
    }

    /// Set text color
    Roller& text_color(lv_color_t color) noexcept {
        lv_obj_set_style_text_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

    /// Set selected item text color
    Roller& selected_text_color(lv_color_t color) noexcept {
        lv_obj_set_style_text_color(m_obj, color, LV_PART_SELECTED);
        return *this;
    }

    /// Set selected item background color
    Roller& selected_bg_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_SELECTED);
        return *this;
    }

#if LV_USE_OBSERVER
    /// Bind roller selection to an integer subject
    Roller& bind_value(lv_subject_t* subject) noexcept {
        lv_roller_bind_value(m_obj, subject);
        return *this;
    }
#endif
};

} // namespace lv
