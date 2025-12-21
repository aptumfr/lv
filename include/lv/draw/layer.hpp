#pragma once

/**
 * @file layer.hpp
 * @brief Wrapper for LVGL draw layers
 */

#include <lvgl.h>

namespace lv {

/**
 * @brief Wrapper for lv_layer_t
 *
 * Layers are used as the drawing target for all draw operations.
 * For Canvas, use lv_canvas_init_layer()/finish_layer() to get a layer.
 *
 * Example with Canvas:
 * ```cpp
 * lv::Layer layer;
 * canvas.init_layer(layer);
 * lv::draw::rect(layer, area, rect_dsc);
 * lv::draw::line(layer, line_dsc);
 * canvas.finish_layer(layer);
 * ```
 */
class Layer {
    lv_layer_t m_layer{};

public:
    /// Default constructor - creates an uninitialized layer
    Layer() noexcept {
        lv_layer_init(&m_layer);
    }

    /// Get the underlying layer pointer
    [[nodiscard]] lv_layer_t* get() noexcept { return &m_layer; }
    [[nodiscard]] const lv_layer_t* get() const noexcept { return &m_layer; }

    /// Implicit conversion to lv_layer_t* for LVGL API compatibility
    operator lv_layer_t*() noexcept { return &m_layer; }

    /// Reset the layer to a drawable state
    Layer& reset() noexcept {
        lv_layer_reset(&m_layer);
        return *this;
    }

    // ==================== Properties ====================

    /// Get the buffer area (absolute coordinates)
    [[nodiscard]] const lv_area_t& buf_area() const noexcept {
        return m_layer.buf_area;
    }

    /// Get the clip area
    [[nodiscard]] const lv_area_t& clip_area() const noexcept {
        return m_layer._clip_area;
    }

    /// Get the draw buffer
    [[nodiscard]] lv_draw_buf_t* draw_buf() const noexcept {
        return m_layer.draw_buf;
    }

    /// Get the color format
    [[nodiscard]] lv_color_format_t color_format() const noexcept {
        return m_layer.color_format;
    }

    /// Get opacity
    [[nodiscard]] lv_opa_t opa() const noexcept {
        return m_layer.opa;
    }

    /// Set opacity
    Layer& opa(lv_opa_t opacity) noexcept {
        m_layer.opa = opacity;
        return *this;
    }

    /// Get parent layer
    [[nodiscard]] lv_layer_t* parent() const noexcept {
        return m_layer.parent;
    }

    // ==================== Static Creation ====================

    /// Create a new layer on a parent layer (allocates memory)
    [[nodiscard]] static lv_layer_t* create(
        lv_layer_t* parent_layer,
        lv_color_format_t color_format,
        const lv_area_t* area) noexcept {
        return lv_draw_layer_create(parent_layer, color_format, area);
    }

    /// Initialize a layer with a parent
    static void init(
        lv_layer_t* layer,
        lv_layer_t* parent_layer,
        lv_color_format_t color_format,
        const lv_area_t* area) noexcept {
        lv_draw_layer_init(layer, parent_layer, color_format, area);
    }

    /// Allocate buffer for a layer
    [[nodiscard]] static void* alloc_buf(lv_layer_t* layer) noexcept {
        return lv_draw_layer_alloc_buf(layer);
    }

    /// Go to a specific position in the layer
    [[nodiscard]] static void* goto_xy(lv_layer_t* layer, int32_t x, int32_t y) noexcept {
        return lv_draw_layer_go_to_xy(layer, x, y);
    }

    // ==================== Instance Methods ====================

    /// Initialize this layer with a parent layer
    Layer& init_with_parent(lv_layer_t* parent_layer,
                            lv_color_format_t cf,
                            const lv_area_t* area) noexcept {
        lv_draw_layer_init(&m_layer, parent_layer, cf, area);
        return *this;
    }

    /// Allocate buffer for this layer
    [[nodiscard]] void* alloc_buf() noexcept {
        return lv_draw_layer_alloc_buf(&m_layer);
    }

    /// Go to a specific position in this layer
    [[nodiscard]] void* goto_xy(int32_t x, int32_t y) noexcept {
        return lv_draw_layer_go_to_xy(&m_layer, x, y);
    }

    // ==================== Additional Accessors ====================

    /// Get user data
    [[nodiscard]] void* user_data() const noexcept {
        return m_layer.user_data;
    }

    /// Set user data
    Layer& user_data(void* data) noexcept {
        m_layer.user_data = data;
        return *this;
    }

    /// Get physical clip area
    [[nodiscard]] const lv_area_t& phy_clip_area() const noexcept {
        return m_layer.phy_clip_area;
    }

    /// Check if all tasks are added
    [[nodiscard]] bool all_tasks_added() const noexcept {
        return m_layer.all_tasks_added;
    }

    /// Set all tasks added flag
    Layer& all_tasks_added(bool added) noexcept {
        m_layer.all_tasks_added = added;
        return *this;
    }

    /// Get recolor
    [[nodiscard]] lv_color32_t recolor() const noexcept {
        return m_layer.recolor;
    }

    /// Set recolor
    Layer& recolor(lv_color32_t c) noexcept {
        m_layer.recolor = c;
        return *this;
    }
};

} // namespace lv
