#pragma once

/**
 * @file focus.hpp
 * @brief Zero-cost wrapper for LVGL focus groups (keyboard/encoder navigation)
 */

#include <lvgl.h>
#include "objectref.hpp"
#include "version.hpp"

namespace lv {

/**
 * @brief Owning wrapper for lv_group_t (focus group)
 *
 * Use focus groups to navigate widgets with keyboard/encoder input.
 * The group is created/destroyed automatically (RAII) and is pointer-sized.
 *
 * @note lv_group_delete() already detaches the group from all input devices
 * internally, so no extra cleanup is needed in the wrapper.
 */
class Group {
    lv_group_t* m_group = nullptr;

public:
    /// Create a new group
    Group() : m_group(lv_group_create()) {}

    /// Take ownership of an existing group
    struct adopt_t {};
    static constexpr adopt_t adopt{};

    Group(adopt_t, lv_group_t* g) noexcept : m_group(g) {}

    ~Group() {
        if (m_group) lv_group_delete(m_group);
    }

    // Non-copyable
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    // Moveable
    Group(Group&& other) noexcept : m_group(other.m_group) {
        other.m_group = nullptr;
    }

    Group& operator=(Group&& other) noexcept {
        if (this != &other) {
            if (m_group) lv_group_delete(m_group);
            m_group = other.m_group;
            other.m_group = nullptr;
        }
        return *this;
    }

    /// Get raw group pointer
    [[nodiscard]] lv_group_t* get() const noexcept { return m_group; }
    [[nodiscard]] operator lv_group_t*() const noexcept { return m_group; }

    // ==================== Objects ====================

    /// Add object to the group
    Group& add(ObjectView obj) noexcept {
        lv_group_add_obj(m_group, obj);
        return *this;
    }

    /// Remove object from the group
    Group& remove(ObjectView obj) noexcept {
        lv_group_remove_obj(obj);
        return *this;
    }

    /// Remove all objects
    Group& remove_all() noexcept {
        lv_group_remove_all_objs(m_group);
        return *this;
    }

    /// Get focused object
    [[nodiscard]] ObjectRef focused() const noexcept {
        return ObjectRef(lv_group_get_focused(m_group));
    }

    /// Get object count
    [[nodiscard]] uint32_t count() const noexcept {
        return lv_group_get_obj_count(m_group);
    }

    // ==================== Focus ====================

    /// Focus next object
    Group& focus_next() noexcept {
        lv_group_focus_next(m_group);
        return *this;
    }

    /// Focus previous object
    Group& focus_prev() noexcept {
        lv_group_focus_prev(m_group);
        return *this;
    }

    /// Focus a specific object
    Group& focus(ObjectView obj) noexcept {
        lv_group_focus_obj(obj);
        return *this;
    }

    /// Send key to focused object
    Group& send_key(uint32_t k) noexcept {
        lv_group_send_data(m_group, k);
        return *this;
    }

    // ==================== Wrap ====================

    /// Enable/disable wrap around
    Group& wrap(bool enable = true) noexcept {
        lv_group_set_wrap(m_group, enable);
        return *this;
    }

    /// Get wrap state
    [[nodiscard]] bool get_wrap() const noexcept {
        return lv_group_get_wrap(m_group);
    }

    // ==================== Editing ====================

    /// Enable/disable editing mode
    Group& editing(bool enable = true) noexcept {
        lv_group_set_editing(m_group, enable);
        return *this;
    }

    /// Get editing state
    [[nodiscard]] bool get_editing() const noexcept {
        return lv_group_get_editing(m_group);
    }

    // ==================== User Data (LVGL 9.5+) ====================

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Set user data (LVGL 9.5+)
    Group& user_data(void* data) noexcept {
        lv_group_set_user_data(m_group, data);
        return *this;
    }

    /// Get user data (LVGL 9.5+)
    [[nodiscard]] void* user_data() const noexcept {
        return lv_group_get_user_data(m_group);
    }

    /// Get user data as typed pointer (LVGL 9.5+)
    template<typename T>
    [[nodiscard]] T* user_data_as() const noexcept {
        return static_cast<T*>(lv_group_get_user_data(m_group));
    }
#endif
};

// Zero-cost check: wrapper must be pointer-sized
static_assert(sizeof(Group) == sizeof(void*),
    "Group must be pointer-sized");

} // namespace lv
