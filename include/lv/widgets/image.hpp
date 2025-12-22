#pragma once

/**
 * @file image.hpp
 * @brief Zero-cost wrapper for LVGL image widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Image widget wrapper
 *
 * Displays images from various sources.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Image : public ObjectView,
            public ObjectMixin<Image>,
              public EventMixin<Image>,
              public StyleMixin<Image> {
public:
    constexpr Image() noexcept : ObjectView(nullptr) {}
    constexpr Image(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    [[nodiscard]] static Image create(lv_obj_t* parent) {
        return Image(wrap, lv_image_create(parent));
    }
    [[nodiscard]] static Image create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Source ====================

    /// Set image source (image descriptor, path, or symbol)
    Image& src(const void* source) noexcept {
        lv_image_set_src(m_obj, source);
        return *this;
    }

    /// Get image source
    [[nodiscard]] const void* src() const noexcept {
        return lv_image_get_src(m_obj);
    }

    // ==================== Transform ====================

    /// Set rotation angle (0.1 degree units, 3600 = 360 degrees)
    Image& rotation(int32_t angle) noexcept {
        lv_image_set_rotation(m_obj, angle);
        return *this;
    }

    /// Get rotation angle
    [[nodiscard]] int32_t rotation() const noexcept {
        return lv_image_get_rotation(m_obj);
    }

    /// Set scale (256 = 100%, 512 = 200%, etc.)
    Image& scale(int32_t zoom) noexcept {
        lv_image_set_scale(m_obj, zoom);
        return *this;
    }

    /// Set scale X
    Image& scale_x(int32_t zoom) noexcept {
        lv_image_set_scale_x(m_obj, zoom);
        return *this;
    }

    /// Set scale Y
    Image& scale_y(int32_t zoom) noexcept {
        lv_image_set_scale_y(m_obj, zoom);
        return *this;
    }

    /// Get scale
    [[nodiscard]] int32_t scale() const noexcept {
        return lv_image_get_scale(m_obj);
    }

    /// Set pivot point for rotation/scaling
    Image& pivot(int32_t x, int32_t y) noexcept {
        lv_image_set_pivot(m_obj, x, y);
        return *this;
    }

    // ==================== Offset ====================

    /// Set offset X (for tiled/scrollable images)
    Image& offset_x(int32_t x) noexcept {
        lv_image_set_offset_x(m_obj, x);
        return *this;
    }

    /// Set offset Y
    Image& offset_y(int32_t y) noexcept {
        lv_image_set_offset_y(m_obj, y);
        return *this;
    }

    /// Get offset X
    [[nodiscard]] int32_t offset_x() const noexcept {
        return lv_image_get_offset_x(m_obj);
    }

    /// Get offset Y
    [[nodiscard]] int32_t offset_y() const noexcept {
        return lv_image_get_offset_y(m_obj);
    }

    // ==================== Appearance ====================

    /// Enable/disable anti-aliasing for transformations
    Image& antialias(bool enable) noexcept {
        lv_image_set_antialias(m_obj, enable);
        return *this;
    }

    /// Check if anti-aliasing is enabled
    [[nodiscard]] bool antialias() const noexcept {
        return lv_image_get_antialias(m_obj);
    }

    /// Set inner alignment
    Image& inner_align(lv_image_align_t align) noexcept {
        lv_image_set_inner_align(m_obj, align);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Image& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    // ==================== Style Shortcuts ====================

    /// Set recolor (tint)
    Image& recolor(lv_color_t color) noexcept {
        lv_obj_set_style_image_recolor(m_obj, color, 0);
        return *this;
    }

    /// Set recolor opacity
    Image& recolor_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_image_recolor_opa(m_obj, opa, 0);
        return *this;
    }

    /// Set image opacity
    Image& image_opa(lv_opa_t opa) noexcept {
        lv_obj_set_style_image_opa(m_obj, opa, 0);
        return *this;
    }

    // ==================== Blend Mode ====================

    /// Set blend mode
    Image& blend_mode(lv_blend_mode_t mode) noexcept {
        lv_image_set_blend_mode(m_obj, mode);
        return *this;
    }

    /// Get blend mode
    [[nodiscard]] lv_blend_mode_t blend_mode() const noexcept {
        return lv_image_get_blend_mode(m_obj);
    }

};

} // namespace lv
