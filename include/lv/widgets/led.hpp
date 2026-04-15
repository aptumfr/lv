#pragma once

/**
 * @file led.hpp
 * @brief Zero-cost wrapper for LVGL LED widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief LED widget wrapper
 *
 * A simple LED indicator that can be on, off, or have variable brightness.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Led : public ObjectView,
            public ObjectMixin<Led>,
            public EventMixin<Led>,
            public StyleMixin<Led> {
public:
    constexpr Led() noexcept : ObjectView(nullptr) {}
    constexpr Led(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_led_class; }

    [[nodiscard]] static Led create(lv_obj_t* parent) {
        return Led(wrap, lv_led_create(parent));
    }
    [[nodiscard]] static Led create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== State ====================

    /// Turn LED on (full brightness)
    Led& on() noexcept {
        lv_led_on(m_obj);
        return *this;
    }

    /// Turn LED off
    Led& off() noexcept {
        lv_led_off(m_obj);
        return *this;
    }

    /// Toggle LED state
    Led& toggle() noexcept {
        lv_led_toggle(m_obj);
        return *this;
    }

    // ==================== Brightness ====================

    /// Set brightness (0-255)
    Led& brightness(uint8_t bright) noexcept {
        lv_led_set_brightness(m_obj, bright);
        return *this;
    }

    /// Get brightness
    [[nodiscard]] uint8_t brightness() const noexcept {
        return lv_led_get_brightness(m_obj);
    }

    // ==================== Color ====================

    /// Set LED color
    Led& color(lv_color_t c) noexcept {
        lv_led_set_color(m_obj, c);
        return *this;
    }

    // ==================== Size ====================

    /// Set size (LEDs are typically square)
    Led& size(int32_t s) noexcept {
        lv_obj_set_size(m_obj, s, s);
        return *this;
    }

    /// Set width and height separately
    Led& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
