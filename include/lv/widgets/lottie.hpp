#pragma once

/**
 * @file lottie.hpp
 * @brief Zero-cost wrapper for LVGL Lottie animation widget
 *
 * Requires ThorVG to be enabled in lv_conf.h:
 *   #define LV_USE_THORVG 1
 *   #define LV_USE_LOTTIE 1
 */

#include <lvgl.h>

#if LV_USE_LOTTIE

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Lottie animation widget wrapper
 *
 * Provides a fluent API for displaying Lottie animations using ThorVG.
 * Animations can be loaded from embedded data or files.
 *
 * Example:
 * @code
 *   // With static buffer
 *   static uint8_t buf[128 * 128 * 4];
 *   lv::Lottie::create(parent)
 *       .buffer(128, 128, buf)
 *       .src_data(animation_json, animation_size)
 *       .center();
 *
 *   // With file
 *   lv::Lottie::create(parent)
 *       .buffer(128, 128, buf)
 *       .src_file("path/to/animation.json")
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Lottie : public ObjectView,
               public ObjectMixin<Lottie>,
               public EventMixin<Lottie>,
               public StyleMixin<Lottie> {
public:
    constexpr Lottie() noexcept : ObjectView(nullptr) {}
    constexpr Lottie(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    [[nodiscard]] static Lottie create(lv_obj_t* parent) {
        return Lottie(wrap, lv_lottie_create(parent));
    }
    [[nodiscard]] static Lottie create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Buffer Setup ====================

    /**
     * @brief Set a static buffer for the animation
     *
     * The buffer must be at least w * h * 4 bytes (ARGB8888 format).
     * The buffer must remain valid for the lifetime of the widget.
     *
     * @param w Width of the animation
     * @param h Height of the animation
     * @param buf Static buffer (w * h * 4 bytes)
     */
    Lottie& buffer(int32_t w, int32_t h, void* buf) noexcept {
        lv_lottie_set_buffer(m_obj, w, h, buf);
        return *this;
    }

    /**
     * @brief Set a draw buffer for the animation
     *
     * Use this when special alignment/stride is required (e.g., for GPUs).
     * The draw buffer must be initialized with ARGB8888 format.
     *
     * @param draw_buf An initialized draw buffer
     */
    Lottie& draw_buf(lv_draw_buf_t* draw_buf) noexcept {
        lv_lottie_set_draw_buf(m_obj, draw_buf);
        return *this;
    }

    // ==================== Source ====================

    /**
     * @brief Load animation from embedded JSON data
     *
     * @param data Pointer to Lottie JSON data (null-terminated array)
     * @param size Size of the data in bytes
     */
    Lottie& src_data(const void* data, size_t size) noexcept {
        lv_lottie_set_src_data(m_obj, data, size);
        return *this;
    }

    /**
     * @brief Load animation from a JSON file
     *
     * Note: Lottie uses direct file access, not LVGL's File System API.
     *
     * @param path Path to the JSON file
     */
    Lottie& src_file(const char* path) noexcept {
        lv_lottie_set_src_file(m_obj, path);
        return *this;
    }

    // ==================== Animation Control ====================

    /**
     * @brief Get the underlying LVGL animation
     *
     * Use this to control playback (pause, resume, set speed, etc.)
     */
    [[nodiscard]] lv_anim_t* get_anim() const noexcept {
        return lv_lottie_get_anim(m_obj);
    }

    /**
     * @brief Pause the animation
     */
    Lottie& pause() noexcept {
        if (auto* anim = get_anim()) {
            lv_anim_pause(anim);
        }
        return *this;
    }

    /**
     * @brief Resume the animation
     */
    Lottie& resume() noexcept {
        if (auto* anim = get_anim()) {
            lv_anim_resume(anim);
        }
        return *this;
    }

    /**
     * @brief Check if animation is paused
     */
    [[nodiscard]] bool is_paused() const noexcept {
        if (auto* anim = get_anim()) {
            return lv_anim_is_paused(anim);
        }
        return true;
    }

    /**
     * @brief Set animation playback time (duration)
     * @param duration_ms Total duration in milliseconds
     */
    Lottie& duration(uint32_t duration_ms) noexcept {
        if (auto* anim = get_anim()) {
            lv_anim_set_duration(anim, duration_ms);
        }
        return *this;
    }

    /**
     * @brief Set repeat count
     * @param count Number of repeats (LV_ANIM_REPEAT_INFINITE for infinite)
     */
    Lottie& repeat(uint32_t count) noexcept {
        if (auto* anim = get_anim()) {
            lv_anim_set_repeat_count(anim, count);
        }
        return *this;
    }

    /**
     * @brief Set infinite looping
     */
    Lottie& loop() noexcept {
        return repeat(LV_ANIM_REPEAT_INFINITE);
    }
};

// LVGL defines lv_lottie_class but omits the extern declaration in the public header.
extern "C" { extern const lv_obj_class_t lv_lottie_class; }

namespace detail {
    template<> inline const lv_obj_class_t*
    widget_lv_class<Lottie>() noexcept { return &lv_lottie_class; }
}

} // namespace lv

#endif // LV_USE_LOTTIE
