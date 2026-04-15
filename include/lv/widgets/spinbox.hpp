#pragma once

/**
 * @file spinbox.hpp
 * @brief Zero-cost wrapper for LVGL spinbox widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Spinbox widget wrapper
 *
 * A numeric input with increment/decrement buttons.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Spinbox : public ObjectView,
            public ObjectMixin<Spinbox>,
                public EventMixin<Spinbox>,
                public StyleMixin<Spinbox> {
public:
    constexpr Spinbox() noexcept : ObjectView(nullptr) {}
    constexpr Spinbox(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_spinbox_class; }

    [[nodiscard]] static Spinbox create(lv_obj_t* parent) {
        return Spinbox(wrap, lv_spinbox_create(parent));
    }
    [[nodiscard]] static Spinbox create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Value ====================

    /// Set value
    Spinbox& value(int32_t val) noexcept {
        lv_spinbox_set_value(m_obj, val);
        return *this;
    }

    /// Get value
    [[nodiscard]] int32_t value() const noexcept {
        return lv_spinbox_get_value(m_obj);
    }

    /// Increment value
    Spinbox& increment() noexcept {
        lv_spinbox_increment(m_obj);
        return *this;
    }

    /// Decrement value
    Spinbox& decrement() noexcept {
        lv_spinbox_decrement(m_obj);
        return *this;
    }

    // ==================== Range ====================

    /// Set value range
    Spinbox& range(int32_t min, int32_t max) noexcept {
        lv_spinbox_set_range(m_obj, min, max);
        return *this;
    }

    // ==================== Format ====================

    /// Set digit format (total digits, decimal places)
    Spinbox& digit_format(uint32_t digit_count, uint32_t separator_pos) noexcept {
        lv_spinbox_set_digit_format(m_obj, digit_count, separator_pos);
        return *this;
    }

    /// Set step (amount to change per increment/decrement)
    Spinbox& step(uint32_t step_val) noexcept {
        lv_spinbox_set_step(m_obj, step_val);
        return *this;
    }

    /// Get step
    [[nodiscard]] uint32_t step() const noexcept {
        return lv_spinbox_get_step(m_obj);
    }

    // ==================== Cursor ====================

    /// Set cursor position (digit index from right)
    Spinbox& cursor_pos(uint32_t pos) noexcept {
        lv_spinbox_set_cursor_pos(m_obj, pos);
        return *this;
    }

    /// Move cursor left
    Spinbox& cursor_left() noexcept {
        lv_spinbox_step_next(m_obj);
        return *this;
    }

    /// Move cursor right
    Spinbox& cursor_right() noexcept {
        lv_spinbox_step_prev(m_obj);
        return *this;
    }

    // ==================== Rollover ====================

    /// Enable/disable rollover (wrap around at min/max)
    Spinbox& rollover(bool enable) noexcept {
        lv_spinbox_set_rollover(m_obj, enable);
        return *this;
    }

    /// Check if rollover is enabled
    [[nodiscard]] bool rollover() const noexcept {
        return lv_spinbox_get_rollover(m_obj);
    }

    // ==================== Size ====================

    /// Set size
    Spinbox& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width
    Spinbox& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }
};

} // namespace lv
