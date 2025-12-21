#pragma once

/**
 * @file draw_buf.hpp
 * @brief RAII wrapper for LVGL draw buffers
 */

#include <lvgl.h>
#include <utility>

namespace lv {

// Note: color_format constants are defined in core/image.hpp

/**
 * @brief RAII wrapper for lv_draw_buf_t
 *
 * Automatically creates and destroys draw buffers.
 *
 * Example:
 * ```cpp
 * lv::DrawBuf buf(200, 200, LV_COLOR_FORMAT_ARGB8888);
 * canvas.draw_buf(buf.get());
 * ```
 */
class DrawBuf {
    lv_draw_buf_t* m_buf;

public:
    /// Create a draw buffer with given dimensions and format
    DrawBuf(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride = LV_STRIDE_AUTO) noexcept
        : m_buf(lv_draw_buf_create(w, h, cf, stride)) {}

    /// Wrap an existing draw buffer (non-owning)
    struct wrap_t {};
    static constexpr wrap_t wrap{};

    DrawBuf(wrap_t, lv_draw_buf_t* buf) noexcept : m_buf(buf) {}

    /// Default constructor (null buffer)
    DrawBuf() noexcept : m_buf(nullptr) {}

    /// Destructor - automatically destroys the buffer
    ~DrawBuf() {
        if (m_buf) {
            lv_draw_buf_destroy(m_buf);
        }
    }

    // Non-copyable
    DrawBuf(const DrawBuf&) = delete;
    DrawBuf& operator=(const DrawBuf&) = delete;

    // Moveable
    DrawBuf(DrawBuf&& other) noexcept : m_buf(other.m_buf) {
        other.m_buf = nullptr;
    }

    DrawBuf& operator=(DrawBuf&& other) noexcept {
        if (this != &other) {
            if (m_buf) {
                lv_draw_buf_destroy(m_buf);
            }
            m_buf = other.m_buf;
            other.m_buf = nullptr;
        }
        return *this;
    }

    /// Get the underlying buffer pointer
    [[nodiscard]] lv_draw_buf_t* get() const noexcept { return m_buf; }

    /// Check if buffer is valid
    [[nodiscard]] explicit operator bool() const noexcept { return m_buf != nullptr; }

    // ==================== Properties ====================

    /// Get buffer width
    [[nodiscard]] uint32_t width() const noexcept {
        return m_buf ? m_buf->header.w : 0;
    }

    /// Get buffer height
    [[nodiscard]] uint32_t height() const noexcept {
        return m_buf ? m_buf->header.h : 0;
    }

    /// Get buffer stride
    [[nodiscard]] uint32_t stride() const noexcept {
        return m_buf ? m_buf->header.stride : 0;
    }

    /// Get color format
    [[nodiscard]] lv_color_format_t color_format() const noexcept {
        return m_buf ? static_cast<lv_color_format_t>(m_buf->header.cf) : LV_COLOR_FORMAT_UNKNOWN;
    }

    /// Get raw data pointer
    [[nodiscard]] uint8_t* data() const noexcept {
        return m_buf ? m_buf->data : nullptr;
    }

    /// Get data size
    [[nodiscard]] uint32_t data_size() const noexcept {
        return m_buf ? m_buf->data_size : 0;
    }

    // ==================== Operations ====================

    /// Clear the buffer (or a specific area)
    DrawBuf& clear(const lv_area_t* area = nullptr) noexcept {
        if (m_buf) {
            lv_draw_buf_clear(m_buf, area);
        }
        return *this;
    }

    /// Go to a specific pixel position
    [[nodiscard]] void* goto_xy(uint32_t x, uint32_t y) const noexcept {
        return m_buf ? lv_draw_buf_goto_xy(m_buf, x, y) : nullptr;
    }

    /// Set palette color (for indexed formats)
    DrawBuf& set_palette(uint8_t index, lv_color32_t color) noexcept {
        if (m_buf) {
            lv_draw_buf_set_palette(m_buf, index, color);
        }
        return *this;
    }

    /// Invalidate cache
    DrawBuf& invalidate_cache(const lv_area_t* area = nullptr) noexcept {
        if (m_buf) {
            lv_draw_buf_invalidate_cache(m_buf, area);
        }
        return *this;
    }

    /// Flush cache
    DrawBuf& flush_cache(const lv_area_t* area = nullptr) noexcept {
        if (m_buf) {
            lv_draw_buf_flush_cache(m_buf, area);
        }
        return *this;
    }

    /// Copy from another buffer
    DrawBuf& copy(const lv_area_t* dest_area, const DrawBuf& src, const lv_area_t* src_area) noexcept {
        if (m_buf && src.m_buf) {
            lv_draw_buf_copy(m_buf, dest_area, src.m_buf, src_area);
        }
        return *this;
    }

    /// Premultiply alpha
    DrawBuf& premultiply() noexcept {
        if (m_buf) {
            lv_draw_buf_premultiply(m_buf);
        }
        return *this;
    }

    /// Check flag
    [[nodiscard]] bool has_flag(lv_image_flags_t flag) const noexcept {
        return m_buf && lv_draw_buf_has_flag(m_buf, flag);
    }

    /// Set flag
    DrawBuf& set_flag(lv_image_flags_t flag) noexcept {
        if (m_buf) {
            lv_draw_buf_set_flag(m_buf, flag);
        }
        return *this;
    }

    /// Clear flag
    DrawBuf& clear_flag(lv_image_flags_t flag) noexcept {
        if (m_buf) {
            lv_draw_buf_clear_flag(m_buf, flag);
        }
        return *this;
    }

    // ==================== Static Utilities ====================

    /// Calculate stride from width and format
    [[nodiscard]] static uint32_t width_to_stride(uint32_t w, lv_color_format_t cf) noexcept {
        return lv_draw_buf_width_to_stride(w, cf);
    }

    /// Duplicate a buffer
    [[nodiscard]] static DrawBuf duplicate(const DrawBuf& src) noexcept {
        if (!src.m_buf) return DrawBuf{};
        return DrawBuf(wrap, lv_draw_buf_dup(src.m_buf));
    }

    /// Duplicate with custom handlers
    [[nodiscard]] static DrawBuf duplicate_ex(const lv_draw_buf_handlers_t* handlers, const DrawBuf& src) noexcept {
        if (!src.m_buf) return DrawBuf{};
        return DrawBuf(wrap, lv_draw_buf_dup_ex(handlers, src.m_buf));
    }

    /// Create with custom handlers
    [[nodiscard]] static DrawBuf create_ex(const lv_draw_buf_handlers_t* handlers,
                                           uint32_t w, uint32_t h,
                                           lv_color_format_t cf,
                                           uint32_t stride = LV_STRIDE_AUTO) noexcept {
        return DrawBuf(wrap, lv_draw_buf_create_ex(handlers, w, h, cf, stride));
    }

    /// Reshape buffer (reuse existing memory with new dimensions)
    [[nodiscard]] lv_draw_buf_t* reshape(lv_color_format_t cf, uint32_t w, uint32_t h,
                                          uint32_t stride = LV_STRIDE_AUTO) noexcept {
        if (m_buf) {
            return lv_draw_buf_reshape(m_buf, cf, w, h, stride);
        }
        return nullptr;
    }

    /// Adjust stride in place
    [[nodiscard]] lv_result_t adjust_stride(uint32_t stride) noexcept {
        if (m_buf) {
            return lv_draw_buf_adjust_stride(m_buf, stride);
        }
        return LV_RESULT_INVALID;
    }

    /// Convert to image descriptor
    void to_image(lv_image_dsc_t* img) const noexcept {
        if (m_buf && img) {
            lv_draw_buf_to_image(m_buf, img);
        }
    }

    /// Initialize from image descriptor
    [[nodiscard]] lv_result_t from_image(const lv_image_dsc_t* img) noexcept {
        if (m_buf && img) {
            return lv_draw_buf_from_image(m_buf, img);
        }
        return LV_RESULT_INVALID;
    }
};

/**
 * @brief Non-owning view of an existing draw buffer
 *
 * Use this when you need to initialize a draw buffer with existing memory
 * that you manage yourself.
 */
class DrawBufView {
    lv_draw_buf_t m_buf{};

public:
    DrawBufView() noexcept = default;

    /// Initialize with existing data
    [[nodiscard]] lv_result_t init(uint32_t w, uint32_t h, lv_color_format_t cf,
                                   uint32_t stride, void* data, uint32_t data_size) noexcept {
        return lv_draw_buf_init(&m_buf, w, h, cf, stride, data, data_size);
    }

    [[nodiscard]] lv_draw_buf_t* get() noexcept { return &m_buf; }
    [[nodiscard]] const lv_draw_buf_t* get() const noexcept { return &m_buf; }
    operator lv_draw_buf_t*() noexcept { return &m_buf; }
};

// ==================== Free Functions ====================

namespace draw {

/// Align a buffer pointer for the given color format
[[nodiscard]] inline void* buf_align(void* buf, lv_color_format_t cf) noexcept {
    return lv_draw_buf_align(buf, cf);
}

/// Align with custom handlers
[[nodiscard]] inline void* buf_align_ex(const lv_draw_buf_handlers_t* handlers,
                                         void* buf, lv_color_format_t cf) noexcept {
    return lv_draw_buf_align_ex(handlers, buf, cf);
}

/// Calculate stride from width and format
[[nodiscard]] inline uint32_t buf_width_to_stride(uint32_t w, lv_color_format_t cf) noexcept {
    return lv_draw_buf_width_to_stride(w, cf);
}

/// Calculate stride with custom handlers
[[nodiscard]] inline uint32_t buf_width_to_stride_ex(const lv_draw_buf_handlers_t* handlers,
                                                      uint32_t w, lv_color_format_t cf) noexcept {
    return lv_draw_buf_width_to_stride_ex(handlers, w, cf);
}

/// Get the default draw buffer handlers
[[nodiscard]] inline lv_draw_buf_handlers_t* buf_get_handlers() noexcept {
    return lv_draw_buf_get_handlers();
}

/// Get font-specific draw buffer handlers
[[nodiscard]] inline lv_draw_buf_handlers_t* buf_get_font_handlers() noexcept {
    return lv_draw_buf_get_font_handlers();
}

/// Get image-specific draw buffer handlers
[[nodiscard]] inline lv_draw_buf_handlers_t* buf_get_image_handlers() noexcept {
    return lv_draw_buf_get_image_handlers();
}

/// Initialize handlers with default implementations
inline void buf_init_default_handlers(lv_draw_buf_handlers_t* handlers) noexcept {
    lv_draw_buf_init_with_default_handlers(handlers);
}

/// Initialize handlers with custom callbacks
inline void buf_handlers_init(lv_draw_buf_handlers_t* handlers,
                              lv_draw_buf_malloc_cb_t malloc_cb,
                              lv_draw_buf_free_cb_t free_cb,
                              lv_draw_buf_copy_cb_t copy_cb,
                              lv_draw_buf_align_cb_t align_cb,
                              lv_draw_buf_cache_operation_cb_t invalidate_cache_cb,
                              lv_draw_buf_cache_operation_cb_t flush_cache_cb,
                              lv_draw_buf_width_to_stride_cb_t stride_cb) noexcept {
    lv_draw_buf_handlers_init(handlers, malloc_cb, free_cb, copy_cb, align_cb,
                              invalidate_cache_cb, flush_cache_cb, stride_cb);
}

} // namespace draw

} // namespace lv
