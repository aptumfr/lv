#pragma once

/**
 * @file texture3d.hpp
 * @brief Zero-cost wrapper for LVGL 3D Texture widget
 *
 * Requires LV_USE_3DTEXTURE enabled in lv_conf.h
 */

#include <lvgl.h>

#if LV_USE_3DTEXTURE

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief 3D Texture widget wrapper
 *
 * Displays a texture from a 3D graphics backend (e.g., OpenGL texture).
 * Useful for integrating 3D rendered content into LVGL UI.
 *
 * Example:
 * @code
 *   // Create widget and set OpenGL texture
 *   GLuint gl_texture = ...; // Your OpenGL texture
 *
 *   lv::Texture3D::create(parent)
 *       .size(400, 300)
 *       .src(static_cast<lv_3dtexture_id_t>(gl_texture))
 *       .flip(false, true)  // Flip vertically (common for OpenGL)
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Texture3D : public ObjectView,
                  public ObjectMixin<Texture3D>,
                  public EventMixin<Texture3D>,
                  public StyleMixin<Texture3D> {
public:
    constexpr Texture3D() noexcept : ObjectView(nullptr) {}
    constexpr Texture3D(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_3dtexture_class; }

    [[nodiscard]] static Texture3D create(lv_obj_t* parent) {
        return Texture3D(wrap, lv_3dtexture_create(parent));
    }
    [[nodiscard]] static Texture3D create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Source ====================

    /**
     * @brief Set the texture source
     *
     * The texture ID is backend-specific (e.g., OpenGL texture handle).
     * Widget size should be set to match the texture dimensions.
     *
     * @param id Texture handle from 3D backend
     */
    Texture3D& src(lv_3dtexture_id_t id) noexcept {
        lv_3dtexture_set_src(m_obj, id);
        return *this;
    }

    // ==================== Configuration ====================

    /**
     * @brief Set texture flipping
     *
     * @param h_flip Flip horizontally
     * @param v_flip Flip vertically (often needed for OpenGL textures)
     */
    Texture3D& flip(bool h_flip, bool v_flip) noexcept {
        lv_3dtexture_set_flip(m_obj, h_flip, v_flip);
        return *this;
    }

    /// Flip horizontally only
    Texture3D& flip_horizontal() noexcept {
        return flip(true, false);
    }

    /// Flip vertically only
    Texture3D& flip_vertical() noexcept {
        return flip(false, true);
    }

    /// No flipping
    Texture3D& no_flip() noexcept {
        return flip(false, false);
    }
};

} // namespace lv

#endif // LV_USE_3DTEXTURE
