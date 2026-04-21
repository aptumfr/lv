#pragma once

/**
 * @file display.hpp
 * @brief Display and input device wrappers
 *
 * Provides clean C++ abstractions for LVGL display backends.
 */

#include <lvgl.h>
#include "objectref.hpp"
#include "version.hpp"
#include <cstdint>

namespace lv {

/**
 * @brief Non-owning wrapper for LVGL display
 */
class Display {
    lv_display_t* m_display;

public:
    constexpr Display(lv_display_t* d) noexcept : m_display(d) {}

    [[nodiscard]] constexpr lv_display_t* get() const noexcept { return m_display; }
    [[nodiscard]] constexpr operator lv_display_t*() const noexcept { return m_display; }

    [[nodiscard]] int32_t width() const noexcept {
        return lv_display_get_horizontal_resolution(m_display);
    }

    [[nodiscard]] int32_t height() const noexcept {
        return lv_display_get_vertical_resolution(m_display);
    }

    [[nodiscard]] ObjectRef screen_active() const noexcept {
        return ObjectRef(lv_display_get_screen_active(m_display));
    }

    /// Set theme for this display
    Display& set_theme(lv_theme_t* theme) noexcept {
        lv_display_set_theme(m_display, theme);
        return *this;
    }

    /// Get current theme
    [[nodiscard]] lv_theme_t* get_theme() const noexcept {
        return lv_display_get_theme(m_display);
    }

    // ==================== Rotation ====================

    /// Set display rotation
    Display& rotation(lv_display_rotation_t rot) noexcept {
        lv_display_set_rotation(m_display, rot);
        return *this;
    }

    /// Get current rotation
    [[nodiscard]] lv_display_rotation_t rotation() const noexcept {
        return lv_display_get_rotation(m_display);
    }

    // ==================== DPI ====================

    /// Set display DPI
    Display& dpi(int32_t val) noexcept {
        lv_display_set_dpi(m_display, val);
        return *this;
    }

    /// Get display DPI
    [[nodiscard]] int32_t dpi() const noexcept {
        return lv_display_get_dpi(m_display);
    }

    // ==================== Color Format ====================

    /// Set display color format
    Display& color_format(lv_color_format_t cf) noexcept {
        lv_display_set_color_format(m_display, cf);
        return *this;
    }

    /// Get display color format
    [[nodiscard]] lv_color_format_t color_format() const noexcept {
        return lv_display_get_color_format(m_display);
    }

    // ==================== Layers ====================

    /// Get top layer (above normal screens, for popups/overlays)
    [[nodiscard]] ObjectRef layer_top() const noexcept {
        return ObjectRef(lv_display_get_layer_top(m_display));
    }

    /// Get system layer (topmost, for system-level overlays like cursor)
    [[nodiscard]] ObjectRef layer_sys() const noexcept {
        return ObjectRef(lv_display_get_layer_sys(m_display));
    }

    /// Get bottom layer (below normal screens)
    [[nodiscard]] ObjectRef layer_bottom() const noexcept {
        return ObjectRef(lv_display_get_layer_bottom(m_display));
    }

    // ==================== Coordinate Transform ====================

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Rotate a point according to display rotation (LVGL 9.5+)
    void rotate_point(lv_point_t* point) const noexcept {
        lv_display_rotate_point(m_display, point);
    }
#endif

    // ==================== Configuration ====================

    /// Set flush callback
    Display& flush_cb(lv_display_flush_cb_t cb) noexcept {
        lv_display_set_flush_cb(m_display, cb);
        return *this;
    }

    /// Set display buffers
    Display& buffers(void* buf1, void* buf2, uint32_t size_bytes,
                     lv_display_render_mode_t mode) noexcept {
        lv_display_set_buffers(m_display, buf1, buf2, size_bytes, mode);
        return *this;
    }

    // ==================== Lifecycle ====================

    /// Signal that flushing is complete (call from flush callback)
    void flush_ready() noexcept {
        lv_display_flush_ready(m_display);
    }

    /// Delete the display
    void del() noexcept {
        if (m_display) {
            lv_display_delete(m_display);
            m_display = nullptr;
        }
    }

    // ==================== Static ====================

    /// Get default display
    [[nodiscard]] static Display get_default() noexcept {
        return Display(lv_display_get_default());
    }

    /// Create a new display with given resolution
    [[nodiscard]] static Display create(int32_t width, int32_t height) noexcept {
        return Display(lv_display_create(width, height));
    }
};

inline Display ObjectView::display() const noexcept {
    return Display(lv_obj_get_display(m_obj));
}


#if LV_USE_X11
/**
 * @brief X11 display backend
 *
 * Creates a native X11 window for Linux desktop.
 *
 * Usage:
 * @code
 * lv::init();
 * auto display = lv::X11Display("My App", 800, 480);
 * // ... create UI ...
 * lv::run();
 * @endcode
 *
 * With custom mouse cursor:
 * @code
 * LV_IMAGE_DECLARE(my_cursor);
 * lv::X11Display display("My App", 800, 480, &my_cursor);
 * @endcode
 */
class X11Display : public Display {
public:
    /// Create X11 window with optional mouse cursor image
    /// @param title Window title
    /// @param width Window width in pixels
    /// @param height Window height in pixels
    /// @param cursor Optional cursor image (nullptr = no visible cursor)
    X11Display(const char* title, int32_t width, int32_t height,
               const lv_image_dsc_t* cursor = nullptr)
        : Display(lv_x11_window_create(title, width, height)) {
        lv_x11_inputs_create(get(), cursor);
    }
};
#endif


#if LV_USE_SDL
/**
 * @brief SDL display backend
 *
 * Creates an SDL window for cross-platform development.
 *
 * Usage:
 * @code
 * lv::init();
 * auto display = lv::SDLDisplay(800, 480);
 * // ... create UI ...
 * lv::run();
 * @endcode
 */
class SDLDisplay : public Display {
public:
    SDLDisplay(int32_t width, int32_t height)
        : Display(lv_sdl_window_create(width, height)) {
        lv_sdl_mouse_create();
    }
};
#endif


#if LV_USE_LINUX_FBDEV
/**
 * @brief Linux framebuffer display backend
 */
class FBDisplay : public Display {
public:
    FBDisplay()
        : Display(lv_linux_fbdev_create()) {
    }
};
#endif


#if LV_USE_LINUX_DRM
/**
 * @brief Linux DRM/KMS display backend
 */
class DRMDisplay : public Display {
public:
    DRMDisplay()
        : Display(lv_linux_drm_create()) {
    }
};
#endif

} // namespace lv
