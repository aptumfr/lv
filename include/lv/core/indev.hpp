#pragma once

/**
 * @file indev.hpp
 * @brief Zero-cost input device wrapper for LVGL
 *
 * Provides C++ API for input devices (touch, mouse, keyboard, encoder).
 */

#include <lvgl.h>
#include "objectref.hpp"
#include "version.hpp"

namespace lv {

// ==================== Input Device Types ====================

namespace indev_type {
    constexpr auto none = LV_INDEV_TYPE_NONE;
    constexpr auto pointer = LV_INDEV_TYPE_POINTER;
    constexpr auto keypad = LV_INDEV_TYPE_KEYPAD;
    constexpr auto button = LV_INDEV_TYPE_BUTTON;
    constexpr auto encoder = LV_INDEV_TYPE_ENCODER;
} // namespace indev_type

// ==================== Input Device States ====================

namespace indev_state {
    constexpr auto released = LV_INDEV_STATE_RELEASED;
    constexpr auto pressed = LV_INDEV_STATE_PRESSED;
} // namespace indev_state

// ==================== Keys ====================

namespace key {
    constexpr uint32_t up = LV_KEY_UP;
    constexpr uint32_t down = LV_KEY_DOWN;
    constexpr uint32_t right = LV_KEY_RIGHT;
    constexpr uint32_t left = LV_KEY_LEFT;
    constexpr uint32_t esc = LV_KEY_ESC;
    constexpr uint32_t del = LV_KEY_DEL;
    constexpr uint32_t backspace = LV_KEY_BACKSPACE;
    constexpr uint32_t enter = LV_KEY_ENTER;
    constexpr uint32_t next = LV_KEY_NEXT;
    constexpr uint32_t prev = LV_KEY_PREV;
    constexpr uint32_t home = LV_KEY_HOME;
    constexpr uint32_t end = LV_KEY_END;
} // namespace key

/**
 * @brief Input device wrapper
 *
 * Wraps LVGL's input device system.
 */
class Indev {
    lv_indev_t* m_indev;
    bool m_owned;

public:
    /// Create a new input device
    Indev() noexcept : m_indev(lv_indev_create()), m_owned(true) {}

    /// Wrap existing input device (non-owning)
    explicit Indev(lv_indev_t* indev) noexcept : m_indev(indev), m_owned(false) {}

    ~Indev() {
        if (m_owned && m_indev) {
            lv_indev_delete(m_indev);
        }
    }

    // Non-copyable
    Indev(const Indev&) = delete;
    Indev& operator=(const Indev&) = delete;

    // Moveable
    Indev(Indev&& other) noexcept : m_indev(other.m_indev), m_owned(other.m_owned) {
        other.m_indev = nullptr;
        other.m_owned = false;
    }

    Indev& operator=(Indev&& other) noexcept {
        if (this != &other) {
            if (m_owned && m_indev) {
                lv_indev_delete(m_indev);
            }
            m_indev = other.m_indev;
            m_owned = other.m_owned;
            other.m_indev = nullptr;
            other.m_owned = false;
        }
        return *this;
    }

    /// Get underlying input device pointer
    [[nodiscard]] lv_indev_t* get() noexcept { return m_indev; }
    [[nodiscard]] const lv_indev_t* get() const noexcept { return m_indev; }

    // ==================== Configuration ====================

    /// Set input device type
    Indev& type(lv_indev_type_t t) noexcept {
        lv_indev_set_type(m_indev, t);
        return *this;
    }

    /// Set as pointer (touch/mouse)
    Indev& as_pointer() noexcept {
        return type(LV_INDEV_TYPE_POINTER);
    }

    /// Set as keypad
    Indev& as_keypad() noexcept {
        return type(LV_INDEV_TYPE_KEYPAD);
    }

    /// Set as encoder
    Indev& as_encoder() noexcept {
        return type(LV_INDEV_TYPE_ENCODER);
    }

    /// Set as button
    Indev& as_button() noexcept {
        return type(LV_INDEV_TYPE_BUTTON);
    }

    /// Set read callback
    Indev& read_cb(lv_indev_read_cb_t cb) noexcept {
        lv_indev_set_read_cb(m_indev, cb);
        return *this;
    }

    /// Set user data
    Indev& user_data(void* data) noexcept {
        lv_indev_set_user_data(m_indev, data);
        return *this;
    }

    /// Get user data
    [[nodiscard]] void* user_data() const noexcept {
        return lv_indev_get_user_data(m_indev);
    }

    /// Set display
    Indev& display(lv_display_t* disp) noexcept {
        lv_indev_set_display(m_indev, disp);
        return *this;
    }

    // ==================== Group ====================

    /// Set group (for keypad/encoder navigation)
    Indev& group(lv_group_t* grp) noexcept {
        lv_indev_set_group(m_indev, grp);
        return *this;
    }

    /// Get group
    [[nodiscard]] lv_group_t* group() const noexcept {
        return lv_indev_get_group(m_indev);
    }

    // ==================== State ====================

    /// Enable or disable
    Indev& enable(bool en = true) noexcept {
        lv_indev_enable(m_indev, en);
        return *this;
    }

    /// Disable
    Indev& disable() noexcept {
        return enable(false);
    }

    /// Get type
    [[nodiscard]] lv_indev_type_t get_type() const noexcept {
        return lv_indev_get_type(m_indev);
    }

    /// Get current point (for pointer devices)
    [[nodiscard]] lv_point_t point() const noexcept {
        lv_point_t p;
        lv_indev_get_point(m_indev, &p);
        return p;
    }

    /// Get movement delta since the last read — useful for gesture tracking.
    [[nodiscard]] lv_point_t vect() const noexcept {
        lv_point_t p;
        lv_indev_get_vect(m_indev, &p);
        return p;
    }

    /// Get current key (for keypad devices)
    [[nodiscard]] uint32_t get_key() const noexcept {
        return lv_indev_get_key(m_indev);
    }

    /// Get scroll direction
    [[nodiscard]] lv_dir_t scroll_dir() const noexcept {
        return lv_indev_get_scroll_dir(m_indev);
    }

    /// Get scroll object
    [[nodiscard]] ObjectRef scroll_obj() const noexcept {
        return ObjectRef(lv_indev_get_scroll_obj(m_indev));
    }

    /// Get gesture direction
    [[nodiscard]] lv_dir_t gesture_dir() const noexcept {
        return lv_indev_get_gesture_dir(m_indev);
    }

    // ==================== Cursor ====================

    /// Set cursor object (for pointer devices)
    Indev& cursor(ObjectView obj) noexcept {
        lv_indev_set_cursor(m_indev, obj.get());
        return *this;
    }

    // ==================== Long Press ====================

    /// Set long press time
    Indev& long_press_time(uint32_t ms) noexcept {
        lv_indev_set_long_press_time(m_indev, ms);
        return *this;
    }

    /// Set long press repeat time
    Indev& long_press_repeat_time(uint32_t ms) noexcept {
        lv_indev_set_long_press_repeat_time(m_indev, ms);
        return *this;
    }

    /// Set scroll limit (pixels to move before scroll starts)
    Indev& scroll_limit(uint32_t px) noexcept {
        lv_indev_set_scroll_limit(m_indev, px);
        return *this;
    }

    /// Set scroll throw slowdown (higher = slower deceleration)
    Indev& scroll_throw(uint32_t slowdown) noexcept {
        lv_indev_set_scroll_throw(m_indev, slowdown);
        return *this;
    }

    // ==================== Gesture (LVGL 9.5+) ====================

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Set minimum gesture velocity threshold (LVGL 9.5+)
    Indev& gesture_min_velocity(uint8_t velocity) noexcept {
        lv_indev_set_gesture_min_velocity(m_indev, velocity);
        return *this;
    }

    /// Set minimum gesture distance threshold (LVGL 9.5+)
    Indev& gesture_min_distance(uint8_t distance) noexcept {
        lv_indev_set_gesture_min_distance(m_indev, distance);
        return *this;
    }

    /// Set key remap callback (LVGL 9.5+)
    /// Callback signature: lv_key_t(lv_indev_t*, lv_key_t)
    Indev& key_remap_cb(lv_indev_key_remap_cb_t cb) noexcept {
        lv_indev_set_key_remap_cb(m_indev, cb);
        return *this;
    }
#endif
};

// ==================== Input Device Helpers ====================

/// Get active (currently processing) input device
inline lv_indev_t* indev_active() noexcept {
    return lv_indev_active();
}

/// Get next input device (for iteration)
inline lv_indev_t* indev_next(lv_indev_t* prev = nullptr) noexcept {
    return lv_indev_get_next(prev);
}

/// Enable or disable all input devices
inline void indev_enable_all(bool enable) noexcept {
    lv_indev_enable(nullptr, enable);
}

// Group class is defined in focus.hpp

/// Get default group
inline lv_group_t* group_default() noexcept {
    return lv_group_get_default();
}

/// Set default group
inline void group_set_default(lv_group_t* grp) noexcept {
    lv_group_set_default(grp);
}

} // namespace lv
