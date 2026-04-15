#pragma once

/**
 * @file switch.hpp
 * @brief Zero-cost wrapper for LVGL switch widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

#if LV_USE_OBSERVER
template<typename T> class State;  // forward declaration for bind()
#endif

/**
 * @brief Switch (toggle) widget wrapper
 *
 * A toggle switch for on/off states.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Switch : public ObjectView,
               public ObjectMixin<Switch>,
               public EventMixin<Switch>,
               public StyleMixin<Switch> {
public:
    constexpr Switch() noexcept : ObjectView(nullptr) {}
    constexpr Switch(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_switch_class; }

    [[nodiscard]] static Switch create(lv_obj_t* parent) {
        return Switch(wrap, lv_switch_create(parent));
    }
    [[nodiscard]] static Switch create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== State ====================

    /// Check if switch is on
    [[nodiscard]] bool is_on() const noexcept {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    /// Set switch state
    Switch& on(bool value = true) noexcept {
        if (value) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    /// Set switch off
    Switch& off() noexcept {
        return on(false);
    }

    /// Toggle switch state
    Switch& toggle() noexcept {
        return on(!is_on());
    }

    // ==================== Orientation ====================

    /// Set switch orientation
    Switch& orientation(lv_switch_orientation_t orient) noexcept {
        lv_switch_set_orientation(m_obj, orient);
        return *this;
    }

    /// Set horizontal orientation
    Switch& horizontal() noexcept {
        return orientation(LV_SWITCH_ORIENTATION_HORIZONTAL);
    }

    /// Set vertical orientation
    Switch& vertical() noexcept {
        return orientation(LV_SWITCH_ORIENTATION_VERTICAL);
    }

    /// Set auto orientation (based on size)
    Switch& orientation_auto() noexcept {
        return orientation(LV_SWITCH_ORIENTATION_AUTO);
    }

    /// Get switch orientation
    [[nodiscard]] lv_switch_orientation_t get_orientation() const noexcept {
        return lv_switch_get_orientation(m_obj);
    }

    /// Disable the switch
    Switch& disabled(bool value = true) noexcept {
        if (value) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set the knob color
    Switch& knob_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_KNOB);
        return *this;
    }

    /// Set the background color when on
    Switch& bg_on_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | LV_STATE_CHECKED);
        return *this;
    }

    /// Set the background color when off
    Switch& bg_off_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

#if LV_USE_OBSERVER
    /// Bind switch state to a boolean subject (two-way binding)
    Switch& bind(lv_subject_t* subject) noexcept {
        lv_obj_bind_checked(m_obj, subject);
        return *this;
    }

    /// Bind to State<bool>
    template<typename T>
        requires std::is_same_v<T, bool>
    Switch& bind(State<T>& state) noexcept {
        lv_obj_bind_checked(m_obj, state.subject());
        return *this;
    }
#endif
};

} // namespace lv
