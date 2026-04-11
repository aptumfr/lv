#pragma once

/**
 * @file anim.hpp
 * @brief Zero-cost animation wrapper for LVGL
 *
 * Provides a fluent C++ API for LVGL animations.
 */

#include <lvgl.h>
#include "object.hpp"

namespace lv {

// ==================== Animation Types ====================

/// Animation struct type (alias for lv_anim_t)
using AnimData = lv_anim_t;

/// Animation path callback type (easing function)
using anim_path_cb = lv_anim_path_cb_t;

/// Animation executor callback type
using anim_exec_cb = lv_anim_exec_xcb_t;

// ==================== Animation Path Functions ====================

namespace anim_path {
    /// Linear path function
    constexpr anim_path_cb linear = lv_anim_path_linear;
    /// Ease in (slow start)
    constexpr anim_path_cb ease_in = lv_anim_path_ease_in;
    /// Ease out (slow end)
    constexpr anim_path_cb ease_out = lv_anim_path_ease_out;
    /// Ease in-out (slow start and end)
    constexpr anim_path_cb ease_in_out = lv_anim_path_ease_in_out;
    /// Overshoot
    constexpr anim_path_cb overshoot = lv_anim_path_overshoot;
    /// Bounce
    constexpr anim_path_cb bounce = lv_anim_path_bounce;
    /// Step (instant change at end)
    constexpr anim_path_cb step = lv_anim_path_step;
} // namespace anim_path

/**
 * @brief Animation builder with fluent API
 *
 * Usage:
 *   lv::Anim()
 *       .var(button)
 *       .exec([](void* obj, int32_t v) { lv_obj_set_x((lv_obj_t*)obj, v); })
 *       .values(0, 100)
 *       .duration(500)
 *       .path(lv::anim_path::ease_out)
 *       .start();
 *
 * Or use the object animation helpers:
 *   lv::anim_x(button, 0, 100).duration(500).start();
 *
 * @note No interruption / velocity preservation.
 *
 * lv::Anim is a thin fluent wrapper over LVGL's native lv_anim_t, which
 * is a fire-and-forget duration + easing animation. It does NOT preserve
 * velocity across retarget: starting a new animation on the same property
 * while another is in flight snaps the value back to the new start and
 * replays the easing curve from rest.
 *
 * If you need interruptible spring motion (drag-release, gesture handoff,
 * rapid retarget, or any case where a value is already in motion when
 * you want to redirect it), this class is not the right tool — you will
 * need a spring/physics integrator layered on top, or a hand-rolled state
 * machine that tracks the current animated value and feeds it back in
 * via lv_anim_set_values() before restarting.
 */
class Anim {
    lv_anim_t m_anim;

public:
    /// Initialize animation
    Anim() noexcept {
        lv_anim_init(&m_anim);
    }

    /// Get underlying animation struct
    [[nodiscard]] lv_anim_t* get() noexcept { return &m_anim; }
    [[nodiscard]] const lv_anim_t* get() const noexcept { return &m_anim; }

    // ==================== Target ====================

    /// Set variable to animate
    Anim& var(void* v) noexcept {
        lv_anim_set_var(&m_anim, v);
        return *this;
    }

    /// Set object to animate
    Anim& var(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        return *this;
    }

    /// Set executor callback
    Anim& exec(lv_anim_exec_xcb_t cb) noexcept {
        lv_anim_set_exec_cb(&m_anim, cb);
        return *this;
    }

    // ==================== Typed Exec Helpers ====================
    // These avoid repeated lv::ref(...) rewrapping in callbacks

    /// Animate translate_x style property
    Anim& exec_translate_x(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_translate_x(static_cast<lv_obj_t*>(v), val, 0);
        });
        return *this;
    }

    /// Animate translate_y style property
    Anim& exec_translate_y(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_translate_y(static_cast<lv_obj_t*>(v), val, 0);
        });
        return *this;
    }

    /// Animate opacity style property
    Anim& exec_opa(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_opa(static_cast<lv_obj_t*>(v), static_cast<lv_opa_t>(val), 0);
        });
        return *this;
    }

    /// Animate background opacity style property
    Anim& exec_bg_opa(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_bg_opa(static_cast<lv_obj_t*>(v), static_cast<lv_opa_t>(val), 0);
        });
        return *this;
    }

    /// Animate image opacity style property
    Anim& exec_image_opa(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_image_opa(static_cast<lv_obj_t*>(v), static_cast<lv_opa_t>(val), 0);
        });
        return *this;
    }

    /// Animate transform rotation style property (for any object)
    Anim& exec_rotation(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_transform_rotation(static_cast<lv_obj_t*>(v), val, 0);
        });
        return *this;
    }

#if LV_USE_ARC
    /// Animate arc rotation (for Arc widgets - uses lv_arc_set_rotation)
    Anim& exec_arc_rotation(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_arc_set_rotation(static_cast<lv_obj_t*>(v), static_cast<lv_value_precise_t>(val));
        });
        return *this;
    }
#endif

    /// Animate transform scale style property (uniform x/y scale)
    Anim& exec_scale(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_style_transform_scale(static_cast<lv_obj_t*>(v), val, 0);
        });
        return *this;
    }

#if LV_USE_IMAGE
    /// Animate image scale property (for Image widgets)
    Anim& exec_image_scale(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_image_set_scale(static_cast<lv_obj_t*>(v), static_cast<uint32_t>(val));
        });
        return *this;
    }
#endif

    /// Animate object X position
    Anim& exec_x(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_x(static_cast<lv_obj_t*>(v), val);
        });
        return *this;
    }

    /// Animate object Y position
    Anim& exec_y(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_y(static_cast<lv_obj_t*>(v), val);
        });
        return *this;
    }

    /// Animate object width
    Anim& exec_width(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_width(static_cast<lv_obj_t*>(v), val);
        });
        return *this;
    }

    /// Animate object height
    Anim& exec_height(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_obj_set_height(static_cast<lv_obj_t*>(v), val);
        });
        return *this;
    }

#if LV_USE_OBSERVER
    /// Animate integer subject value (for State<int>/IntState)
    Anim& exec_int_subject(lv_subject_t* subject) noexcept {
        lv_anim_set_var(&m_anim, subject);
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_subject_set_int(static_cast<lv_subject_t*>(v), val);
        });
        return *this;
    }
#endif // LV_USE_OBSERVER

#if LV_USE_ARC
    /// Animate arc value
    Anim& exec_arc_value(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_arc_set_value(static_cast<lv_obj_t*>(v), static_cast<int16_t>(val));
        });
        return *this;
    }

    /// Animate arc start angle
    Anim& exec_arc_start_angle(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_arc_set_start_angle(static_cast<lv_obj_t*>(v), static_cast<lv_value_precise_t>(val));
        });
        return *this;
    }

    /// Animate arc end angle
    Anim& exec_arc_end_angle(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_arc_set_end_angle(static_cast<lv_obj_t*>(v), static_cast<lv_value_precise_t>(val));
        });
        return *this;
    }
#endif

#if LV_USE_BAR
    /// Animate bar value
    Anim& exec_bar_value(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_bar_set_value(static_cast<lv_obj_t*>(v), val, LV_ANIM_OFF);
        });
        return *this;
    }
#endif

#if LV_USE_SLIDER
    /// Animate slider value
    Anim& exec_slider_value(ObjectView obj) noexcept {
        lv_anim_set_var(&m_anim, obj.get());
        lv_anim_set_exec_cb(&m_anim, [](void* v, int32_t val) {
            lv_slider_set_value(static_cast<lv_obj_t*>(v), val, LV_ANIM_OFF);
        });
        return *this;
    }
#endif

    // ==================== Values ====================

    /// Set start and end values
    Anim& values(int32_t start, int32_t end) noexcept {
        lv_anim_set_values(&m_anim, start, end);
        return *this;
    }

    /// Set start value only
    Anim& start_value(int32_t v) noexcept {
        m_anim.start_value = v;
        return *this;
    }

    /// Set end value only
    Anim& end_value(int32_t v) noexcept {
        m_anim.end_value = v;
        return *this;
    }

    // ==================== Timing ====================

    /// Set duration in milliseconds
    Anim& duration(uint32_t ms) noexcept {
        lv_anim_set_duration(&m_anim, ms);
        return *this;
    }

    /// Set delay before starting
    Anim& delay(uint32_t ms) noexcept {
        lv_anim_set_delay(&m_anim, ms);
        return *this;
    }

    // ==================== Path (Easing) ====================

    /// Set path callback (easing function)
    Anim& path(anim_path_cb cb) noexcept {
        lv_anim_set_path_cb(&m_anim, cb);
        return *this;
    }

    /// Linear path
    Anim& linear() noexcept {
        return path(lv_anim_path_linear);
    }

    /// Ease in (slow start)
    Anim& ease_in() noexcept {
        return path(lv_anim_path_ease_in);
    }

    /// Ease out (slow end)
    Anim& ease_out() noexcept {
        return path(lv_anim_path_ease_out);
    }

    /// Ease in-out (slow start and end)
    Anim& ease_in_out() noexcept {
        return path(lv_anim_path_ease_in_out);
    }

    /// Overshoot
    Anim& overshoot() noexcept {
        return path(lv_anim_path_overshoot);
    }

    /// Bounce
    Anim& bounce() noexcept {
        return path(lv_anim_path_bounce);
    }

    /// Step (instant change at end)
    Anim& step() noexcept {
        return path(lv_anim_path_step);
    }

    // ==================== Repeat ====================

    /// Set repeat count (LV_ANIM_REPEAT_INFINITE for forever)
    Anim& repeat(uint32_t count) noexcept {
        lv_anim_set_repeat_count(&m_anim, count);
        return *this;
    }

    /// Repeat forever
    Anim& repeat_infinite() noexcept {
        return repeat(LV_ANIM_REPEAT_INFINITE);
    }

    /// Set delay between repeats
    Anim& repeat_delay(uint32_t ms) noexcept {
        lv_anim_set_repeat_delay(&m_anim, ms);
        return *this;
    }

    // ==================== Playback ====================

    /// Enable reverse playback (yo-yo)
    Anim& playback(uint32_t delay_ms = 0) noexcept {
        lv_anim_set_playback_duration(&m_anim, m_anim.duration);
        lv_anim_set_playback_delay(&m_anim, delay_ms);
        return *this;
    }

    /// Set playback duration (different from forward)
    Anim& playback_duration(uint32_t ms) noexcept {
        lv_anim_set_playback_duration(&m_anim, ms);
        return *this;
    }

    /// Set playback delay
    Anim& playback_delay(uint32_t ms) noexcept {
        lv_anim_set_playback_delay(&m_anim, ms);
        return *this;
    }

    // ==================== Callbacks ====================

    /// Set callback when animation starts (after delay)
    Anim& on_start(lv_anim_start_cb_t cb) noexcept {
        lv_anim_set_start_cb(&m_anim, cb);
        return *this;
    }

    /// Set callback when animation completes
    Anim& on_complete(lv_anim_completed_cb_t cb) noexcept {
        lv_anim_set_completed_cb(&m_anim, cb);
        return *this;
    }

    /// Set callback when animation is deleted
    Anim& on_deleted(lv_anim_deleted_cb_t cb) noexcept {
        lv_anim_set_deleted_cb(&m_anim, cb);
        return *this;
    }

    /// Set user data
    Anim& user_data(void* data) noexcept {
        lv_anim_set_user_data(&m_anim, data);
        return *this;
    }

    // ==================== Early Apply ====================

    /// Apply start value immediately (before delay)
    Anim& early_apply(bool enable = true) noexcept {
        lv_anim_set_early_apply(&m_anim, enable);
        return *this;
    }

    // ==================== Control ====================

    /// Start the animation (returns handle for optional pause/resume/delete)
    lv_anim_t* start() noexcept {
        return lv_anim_start(&m_anim);
    }
};

// ==================== Object Animation Helpers ====================

/// Animate object X position
[[nodiscard]] inline Anim anim_x(ObjectView obj, int32_t start, int32_t end) {
    return Anim()
        .var(obj)
        .exec([](void* o, int32_t v) { lv_obj_set_x(static_cast<lv_obj_t*>(o), v); })
        .values(start, end);
}

/// Animate object Y position
[[nodiscard]] inline Anim anim_y(ObjectView obj, int32_t start, int32_t end) {
    return Anim()
        .var(obj)
        .exec([](void* o, int32_t v) { lv_obj_set_y(static_cast<lv_obj_t*>(o), v); })
        .values(start, end);
}

/// Animate object width
[[nodiscard]] inline Anim anim_width(ObjectView obj, int32_t start, int32_t end) {
    return Anim()
        .var(obj)
        .exec([](void* o, int32_t v) { lv_obj_set_width(static_cast<lv_obj_t*>(o), v); })
        .values(start, end);
}

/// Animate object height
[[nodiscard]] inline Anim anim_height(ObjectView obj, int32_t start, int32_t end) {
    return Anim()
        .var(obj)
        .exec([](void* o, int32_t v) { lv_obj_set_height(static_cast<lv_obj_t*>(o), v); })
        .values(start, end);
}

/// Animate object opacity
[[nodiscard]] inline Anim anim_opa(ObjectView obj, lv_opa_t start, lv_opa_t end) {
    return Anim()
        .var(obj)
        .exec([](void* o, int32_t v) { lv_obj_set_style_opa(static_cast<lv_obj_t*>(o), static_cast<lv_opa_t>(v), 0); })
        .values(start, end);
}

// ==================== Animation Control ====================

/// Delete all animations on an object
inline void anim_delete(ObjectView obj) {
    lv_anim_delete(obj.get(), nullptr);
}

/// Delete animations with specific exec callback
inline void anim_delete(ObjectView obj, lv_anim_exec_xcb_t exec_cb) {
    lv_anim_delete(obj.get(), exec_cb);
}

/// Get running animation on object
[[nodiscard]] inline lv_anim_t* anim_get(ObjectView obj, lv_anim_exec_xcb_t exec_cb) {
    return lv_anim_get(obj.get(), exec_cb);
}

/// Pause animation
inline void anim_pause(lv_anim_t* a) {
    lv_anim_pause(a);
}

/// Resume animation
inline void anim_resume(lv_anim_t* a) {
    lv_anim_resume(a);
}

} // namespace lv
