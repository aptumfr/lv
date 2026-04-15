#pragma once

/**
 * @file slider.hpp
 * @brief Zero-cost wrapper for LVGL slider widget
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
 * @brief Slider widget wrapper
 *
 * A slider for selecting values in a range.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Slider : public ObjectView,
               public ObjectMixin<Slider>,
               public EventMixin<Slider>,
               public StyleMixin<Slider> {
public:
    constexpr Slider() noexcept : ObjectView(nullptr) {}
    constexpr Slider(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_slider_class; }

    [[nodiscard]] static Slider create(lv_obj_t* parent) {
        return Slider(wrap, lv_slider_create(parent));
    }
    [[nodiscard]] static Slider create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Value ====================

    /// Get current value
    [[nodiscard]] int32_t value() const noexcept {
        return lv_slider_get_value(m_obj);
    }

    /// Set current value
    Slider& value(int32_t val) noexcept {
        lv_slider_set_value(m_obj, val, LV_ANIM_OFF);
        return *this;
    }

    /// Set value with animation
    Slider& value_anim(int32_t val) noexcept {
        lv_slider_set_value(m_obj, val, LV_ANIM_ON);
        return *this;
    }

    // ==================== Range ====================

    /// Get minimum value
    [[nodiscard]] int32_t min() const noexcept {
        return lv_slider_get_min_value(m_obj);
    }

    /// Get maximum value
    [[nodiscard]] int32_t max() const noexcept {
        return lv_slider_get_max_value(m_obj);
    }

    /// Set range
    Slider& range(int32_t min_val, int32_t max_val) noexcept {
        lv_slider_set_range(m_obj, min_val, max_val);
        return *this;
    }

    // ==================== Mode ====================

    /// Set slider mode
    Slider& mode(lv_slider_mode_t m) noexcept {
        lv_slider_set_mode(m_obj, m);
        return *this;
    }

    /// Normal mode (default)
    Slider& mode_normal() noexcept {
        return mode(LV_SLIDER_MODE_NORMAL);
    }

    /// Symmetrical mode (indicator from center)
    Slider& mode_symmetrical() noexcept {
        return mode(LV_SLIDER_MODE_SYMMETRICAL);
    }

    /// Range mode (two knobs)
    Slider& mode_range() noexcept {
        return mode(LV_SLIDER_MODE_RANGE);
    }

    // ==================== Range Mode ====================

    /// Get left value (range mode)
    [[nodiscard]] int32_t left_value() const noexcept {
        return lv_slider_get_left_value(m_obj);
    }

    /// Set left value (range mode)
    Slider& left_value(int32_t val) noexcept {
        lv_slider_set_left_value(m_obj, val, LV_ANIM_OFF);
        return *this;
    }

    /// Set start value (range mode) - same as left_value
    Slider& start_value(int32_t val, bool animate = false) noexcept {
        lv_slider_set_start_value(m_obj, val, animate ? LV_ANIM_ON : LV_ANIM_OFF);
        return *this;
    }

    // ==================== State ====================

    /// Check if slider is being dragged
    [[nodiscard]] bool is_dragged() const noexcept {
        return lv_slider_is_dragged(m_obj);
    }

    /// Disable the slider
    Slider& disabled(bool value = true) noexcept {
        if (value) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    // width(), fill_width() are inherited from ObjectMixin<Slider>.

    // ==================== Style Shortcuts ====================

    /// Set the indicator (filled part) color
    Slider& indicator_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_INDICATOR);
        return *this;
    }

    /// Set the knob color
    Slider& knob_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_KNOB);
        return *this;
    }

    /// Set the track (background) color
    Slider& track_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_MAIN);
        return *this;
    }

    /// Set background image source
    Slider& bg_image_src(const void* src) noexcept {
        lv_obj_set_style_bg_image_src(m_obj, src, LV_PART_MAIN);
        return *this;
    }

    /// Set indicator background opacity
    Slider& indicator_bg_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_bg_opa(m_obj, opa, LV_PART_INDICATOR);
        return *this;
    }

    /// Set knob border opacity
    Slider& knob_border_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_border_opa(m_obj, opa, LV_PART_KNOB);
        return *this;
    }

    /// Set knob border width
    Slider& knob_border_width(int32_t w) noexcept {
        lv_obj_set_style_border_width(m_obj, w, LV_PART_KNOB);
        return *this;
    }

    /// Set knob border color
    Slider& knob_border_color(lv_color_t color) noexcept {
        lv_obj_set_style_border_color(m_obj, color, LV_PART_KNOB);
        return *this;
    }

    /// Set knob background color
    Slider& knob_bg_color(lv_color_t color) noexcept {
        lv_obj_set_style_bg_color(m_obj, color, LV_PART_KNOB);
        return *this;
    }

    /// Set knob background opacity
    Slider& knob_bg_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_bg_opa(m_obj, opa, LV_PART_KNOB);
        return *this;
    }

#if LV_USE_OBSERVER
    /// Bind slider value to an integer subject
    Slider& bind(lv_subject_t* subject) noexcept {
        lv_slider_bind_value(m_obj, subject);
        return *this;
    }

    /// Bind to State<int>
    template<typename T>
        requires (std::is_integral_v<T> && sizeof(T) <= sizeof(int32_t))
    Slider& bind(State<T>& state) noexcept {
        lv_slider_bind_value(m_obj, state.subject());
        return *this;
    }
#endif
};

} // namespace lv
