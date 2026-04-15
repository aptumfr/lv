#pragma once

/**
 * @file gif.hpp
 * @brief Zero-cost wrapper for LVGL GIF animation widget
 *
 * Requires LV_USE_GIF enabled in lv_conf.h
 */

#include <lvgl.h>

#if LV_USE_GIF

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief GIF animation widget wrapper
 *
 * Displays animated GIF images from file or embedded data.
 *
 * Example:
 * @code
 *   // From file
 *   lv::GIF(parent)
 *       .src("A:/animations/loading.gif")
 *       .center();
 *
 *   // From embedded data
 *   extern const lv_image_dsc_t my_gif;
 *   lv::GIF(parent)
 *       .src(&my_gif)
 *       .loop_count(3)
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class GIF : public ObjectView,
            public EventMixin<GIF>,
            public StyleMixin<GIF> {
public:
    /// Default constructor (null widget)
    GIF() noexcept : ObjectView(nullptr) {}

    /// Create a GIF widget as child of parent
    explicit GIF(ObjectView parent)
        : ObjectView(lv_gif_create(parent)) {}

    /// Wrap existing GIF object
    GIF(lv::wrap_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_gif_class; }

    // ==================== Source ====================

    /**
     * @brief Set GIF source from file path or image descriptor
     * @param source File path (e.g. "A:/anim.gif") or lv_image_dsc_t*
     */
    GIF& src(const void* source) noexcept {
        lv_gif_set_src(m_obj, source);
        return *this;
    }

    /**
     * @brief Set GIF source from file path
     * @param path File path (e.g. "A:/anim.gif")
     */
    GIF& src(const char* path) noexcept {
        lv_gif_set_src(m_obj, path);
        return *this;
    }

    // ==================== Configuration ====================

    /**
     * @brief Set color format of internal framebuffer
     *
     * Call before setting source to avoid reallocation.
     * Default is LV_COLOR_FORMAT_ARGB8888.
     *
     * @param format Color format
     */
    GIF& color_format(lv_color_format_t format) noexcept {
        lv_gif_set_color_format(m_obj, format);
        return *this;
    }

    /**
     * @brief Set loop count
     * @param count Number of loops (-1 or 0 for infinite)
     */
    GIF& loop_count(int32_t count) noexcept {
        lv_gif_set_loop_count(m_obj, count);
        return *this;
    }

    /// Set infinite looping
    GIF& loop() noexcept {
        return loop_count(0);
    }

    // ==================== Playback Control ====================

    /// Restart animation from beginning
    GIF& restart() noexcept {
        lv_gif_restart(m_obj);
        return *this;
    }

    /// Pause animation
    GIF& pause() noexcept {
        lv_gif_pause(m_obj);
        return *this;
    }

    /// Resume animation
    GIF& resume() noexcept {
        lv_gif_resume(m_obj);
        return *this;
    }

    // ==================== Getters ====================

    /// Check if GIF loaded successfully
    [[nodiscard]] bool is_loaded() const noexcept {
        return lv_gif_is_loaded(m_obj);
    }

    /// Get loop count
    [[nodiscard]] int32_t get_loop_count() const noexcept {
        return lv_gif_get_loop_count(m_obj);
    }
};

} // namespace lv

#endif // LV_USE_GIF
