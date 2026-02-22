#pragma once

/**
 * @file gltf.hpp
 * @brief Zero-cost wrapper for LVGL glTF 3D model viewer
 *
 * Requires LV_USE_GLTF enabled in lv_conf.h
 */

#include <lvgl.h>

#if LV_USE_GLTF

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Anti-aliasing modes for glTF viewer
 */
struct GLTFAntiAliasing {
    static constexpr auto off = LV_GLTF_AA_MODE_OFF;
    static constexpr auto on = LV_GLTF_AA_MODE_ON;
    static constexpr auto dynamic_ = LV_GLTF_AA_MODE_DYNAMIC;  // Only when no movement
};

/**
 * @brief Background modes for glTF viewer
 */
struct GLTFBackgroundMode {
    static constexpr auto solid = LV_GLTF_BG_MODE_SOLID;
    static constexpr auto environment = LV_GLTF_BG_MODE_ENVIRONMENT;
};

/**
 * @brief Animation speed presets
 */
struct GLTFAnimSpeed {
    static constexpr uint32_t tenth = LV_GLTF_ANIM_SPEED_TENTH;
    static constexpr uint32_t quarter = LV_GLTF_ANIM_SPEED_QUARTER;
    static constexpr uint32_t half = LV_GLTF_ANIM_SPEED_HALF;
    static constexpr uint32_t normal = LV_GLTF_ANIM_SPEED_NORMAL;
    static constexpr uint32_t x2 = LV_GLTF_ANIM_SPEED_2X;
    static constexpr uint32_t x3 = LV_GLTF_ANIM_SPEED_3X;
    static constexpr uint32_t x4 = LV_GLTF_ANIM_SPEED_4X;
};

/**
 * @brief glTF 3D model viewer widget wrapper
 *
 * Displays 3D glTF models with camera control, lighting, and animation.
 *
 * Example:
 * @code
 *   lv::GLTF(parent)
 *       .size(400, 300)
 *       .load_model("A:/models/cube.gltf")
 *       .yaw(45)
 *       .pitch(30)
 *       .distance(2.0f)
 *       .antialiasing(lv::GLTFAntiAliasing::on)
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class GLTF : public ObjectView,
             public EventMixin<GLTF>,
             public StyleMixin<GLTF> {
public:
    /// Default constructor (null widget)
    GLTF() noexcept : ObjectView(nullptr) {}

    /// Create a glTF viewer widget as child of parent
    explicit GLTF(ObjectView parent)
        : ObjectView(lv_gltf_create(parent)) {}

    /// Wrap existing glTF object
    GLTF(lv::wrap_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    // ==================== Model Loading ====================

    /**
     * @brief Load a glTF model from file
     * @param path File path to .gltf or .glb file
     * @return Pointer to loaded model, or nullptr on failure
     */
    lv_gltf_model_t* load_model(const char* path) noexcept {
        return lv_gltf_load_model_from_file(m_obj, path);
    }

    /**
     * @brief Set environment for IBL (Image-Based Lighting)
     * @param env Environment pointer (can be shared across viewers)
     */
    GLTF& environment(lv_gltf_environment_t* env) noexcept {
        lv_gltf_set_environment(m_obj, env);
        return *this;
    }

    /// Get number of loaded models
    [[nodiscard]] size_t model_count() const noexcept {
        return lv_gltf_get_model_count(m_obj);
    }

    /// Get model by index
    [[nodiscard]] lv_gltf_model_t* model(size_t index) const noexcept {
        return lv_gltf_get_model_by_index(m_obj, index);
    }

    /// Get primary (first) model
    [[nodiscard]] lv_gltf_model_t* primary_model() const noexcept {
        return lv_gltf_get_primary_model(m_obj);
    }

    // ==================== Camera Control ====================

    /// Set camera yaw (horizontal rotation in degrees)
    GLTF& yaw(float angle) noexcept {
        lv_gltf_set_yaw(m_obj, angle);
        return *this;
    }

    /// Get camera yaw
    [[nodiscard]] float get_yaw() const noexcept {
        return lv_gltf_get_yaw(m_obj);
    }

    /// Set camera pitch (vertical rotation in degrees)
    GLTF& pitch(float angle) noexcept {
        lv_gltf_set_pitch(m_obj, angle);
        return *this;
    }

    /// Get camera pitch
    [[nodiscard]] float get_pitch() const noexcept {
        return lv_gltf_get_pitch(m_obj);
    }

    /// Set camera distance from focal point
    GLTF& distance(float d) noexcept {
        lv_gltf_set_distance(m_obj, d);
        return *this;
    }

    /// Get camera distance
    [[nodiscard]] float get_distance() const noexcept {
        return lv_gltf_get_distance(m_obj);
    }

    /// Get camera distance in world units
    [[nodiscard]] float get_world_distance() const noexcept {
        return lv_gltf_get_world_distance(m_obj);
    }

    /// Set field of view (0 for orthographic)
    GLTF& fov(float degrees) noexcept {
        lv_gltf_set_fov(m_obj, degrees);
        return *this;
    }

    /// Get field of view
    [[nodiscard]] float get_fov() const noexcept {
        return lv_gltf_get_fov(m_obj);
    }

    /// Set active camera index
    GLTF& camera(uint32_t index) noexcept {
        lv_gltf_set_camera(m_obj, index);
        return *this;
    }

    /// Get active camera index
    [[nodiscard]] uint32_t get_camera() const noexcept {
        return lv_gltf_get_camera(m_obj);
    }

    /// Get number of cameras
    [[nodiscard]] uint32_t camera_count() const noexcept {
        return lv_gltf_get_camera_count(m_obj);
    }

    // ==================== Focal Point ====================

    /// Set focal point X coordinate
    GLTF& focal_x(float x) noexcept {
        lv_gltf_set_focal_x(m_obj, x);
        return *this;
    }

    /// Set focal point Y coordinate
    GLTF& focal_y(float y) noexcept {
        lv_gltf_set_focal_y(m_obj, y);
        return *this;
    }

    /// Set focal point Z coordinate
    GLTF& focal_z(float z) noexcept {
        lv_gltf_set_focal_z(m_obj, z);
        return *this;
    }

    /// Set all focal point coordinates
    GLTF& focal(float x, float y, float z) noexcept {
        lv_gltf_set_focal_x(m_obj, x);
        lv_gltf_set_focal_y(m_obj, y);
        lv_gltf_set_focal_z(m_obj, z);
        return *this;
    }

    /// Get focal point X
    [[nodiscard]] float get_focal_x() const noexcept {
        return lv_gltf_get_focal_x(m_obj);
    }

    /// Get focal point Y
    [[nodiscard]] float get_focal_y() const noexcept {
        return lv_gltf_get_focal_y(m_obj);
    }

    /// Get focal point Z
    [[nodiscard]] float get_focal_z() const noexcept {
        return lv_gltf_get_focal_z(m_obj);
    }

    /// Recenter camera on model
    GLTF& recenter(lv_gltf_model_t* model = nullptr) noexcept {
        lv_gltf_recenter(m_obj, model);
        return *this;
    }

    // ==================== Animation ====================

    /// Set animation speed (use GLTFAnimSpeed presets)
    GLTF& animation_speed(uint32_t speed) noexcept {
        lv_gltf_set_animation_speed(m_obj, speed);
        return *this;
    }

    /// Get animation speed
    [[nodiscard]] uint32_t get_animation_speed() const noexcept {
        return lv_gltf_get_animation_speed(m_obj);
    }

    // ==================== Visual Settings ====================

    /// Set background mode
    GLTF& background_mode(lv_gltf_bg_mode_t mode) noexcept {
        lv_gltf_set_background_mode(m_obj, mode);
        return *this;
    }

    /// Get background mode
    [[nodiscard]] lv_gltf_bg_mode_t get_background_mode() const noexcept {
        return lv_gltf_get_background_mode(m_obj);
    }

    /// Set background blur (0-100)
    GLTF& background_blur(uint32_t blur) noexcept {
        lv_gltf_set_background_blur(m_obj, blur);
        return *this;
    }

    /// Get background blur
    [[nodiscard]] uint32_t get_background_blur() const noexcept {
        return lv_gltf_get_background_blur(m_obj);
    }

    /// Set environment brightness
    GLTF& env_brightness(uint32_t brightness) noexcept {
        lv_gltf_set_env_brightness(m_obj, brightness);
        return *this;
    }

    /// Get environment brightness
    [[nodiscard]] uint32_t get_env_brightness() const noexcept {
        return lv_gltf_get_env_brightness(m_obj);
    }

    /// Set image exposure level
    GLTF& exposure(float value) noexcept {
        lv_gltf_set_image_exposure(m_obj, value);
        return *this;
    }

    /// Get image exposure
    [[nodiscard]] float get_exposure() const noexcept {
        return lv_gltf_get_image_exposure(m_obj);
    }

    /// Set anti-aliasing mode
    GLTF& antialiasing(lv_gltf_aa_mode_t mode) noexcept {
        lv_gltf_set_antialiasing_mode(m_obj, mode);
        return *this;
    }

    /// Get anti-aliasing mode
    [[nodiscard]] lv_gltf_aa_mode_t get_antialiasing() const noexcept {
        return lv_gltf_get_antialiasing_mode(m_obj);
    }

    // ==================== Raycasting ====================

    /// Get ray from 2D screen coordinate
    [[nodiscard]] lv_3dray_t ray_from_point(const lv_point_t& screen_pos) const noexcept {
        return lv_gltf_get_ray_from_2d_coordinate(m_obj, &screen_pos);
    }

    /// Get plane facing current camera
    [[nodiscard]] lv_3dplane_t view_plane(float distance) const noexcept {
        return lv_gltf_get_current_view_plane(m_obj, distance);
    }

    /// Convert world position to screen position
    [[nodiscard]] bool world_to_screen(const lv_3dpoint_t& world_pos, lv_point_t& screen_pos) const noexcept {
        return lv_gltf_world_to_screen(m_obj, world_pos, &screen_pos) == LV_RESULT_OK;
    }
};

// LVGL defines lv_gltf_class but omits the extern declaration in the public header.
extern "C" { extern const lv_obj_class_t lv_gltf_class; }

namespace detail {
    template<> inline const lv_obj_class_t*
    widget_lv_class<GLTF>() noexcept { return &lv_gltf_class; }
}

} // namespace lv

#endif // LV_USE_GLTF
