#pragma once

/**
 * @file imagebutton.hpp
 * @brief Zero-cost wrapper for LVGL image button widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Image Button widget wrapper
 *
 * A button with different images for each state.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class ImageButton : public ObjectView,
            public ObjectMixin<ImageButton>,
                    public EventMixin<ImageButton>,
                    public StyleMixin<ImageButton> {
public:
    constexpr ImageButton() noexcept : ObjectView(nullptr) {}
    constexpr ImageButton(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_imagebutton_class; }

    [[nodiscard]] static ImageButton create(lv_obj_t* parent) {
        return ImageButton(wrap, lv_imagebutton_create(parent));
    }
    [[nodiscard]] static ImageButton create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Sources ====================

    /// Set image source for a state
    ImageButton& src(lv_imagebutton_state_t state, const void* src_left,
                     const void* src_mid, const void* src_right) noexcept {
        lv_imagebutton_set_src(m_obj, state, src_left, src_mid, src_right);
        return *this;
    }

    /// Set released state images
    ImageButton& src_released(const void* src_left, const void* src_mid, const void* src_right) noexcept {
        return src(LV_IMAGEBUTTON_STATE_RELEASED, src_left, src_mid, src_right);
    }

    /// Set pressed state images
    ImageButton& src_pressed(const void* src_left, const void* src_mid, const void* src_right) noexcept {
        return src(LV_IMAGEBUTTON_STATE_PRESSED, src_left, src_mid, src_right);
    }

    /// Set disabled state images
    ImageButton& src_disabled(const void* src_left, const void* src_mid, const void* src_right) noexcept {
        return src(LV_IMAGEBUTTON_STATE_DISABLED, src_left, src_mid, src_right);
    }

    /// Set checked released state images
    ImageButton& src_checked_released(const void* src_left, const void* src_mid, const void* src_right) noexcept {
        return src(LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, src_left, src_mid, src_right);
    }

    /// Set checked pressed state images
    ImageButton& src_checked_pressed(const void* src_left, const void* src_mid, const void* src_right) noexcept {
        return src(LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, src_left, src_mid, src_right);
    }

    /// Set checked disabled state images
    ImageButton& src_checked_disabled(const void* src_left, const void* src_mid, const void* src_right) noexcept {
        return src(LV_IMAGEBUTTON_STATE_CHECKED_DISABLED, src_left, src_mid, src_right);
    }

    /// Get image source for state
    [[nodiscard]] const void* get_src_left(lv_imagebutton_state_t state) const noexcept {
        return lv_imagebutton_get_src_left(m_obj, state);
    }

    [[nodiscard]] const void* get_src_middle(lv_imagebutton_state_t state) const noexcept {
        return lv_imagebutton_get_src_middle(m_obj, state);
    }

    [[nodiscard]] const void* get_src_right(lv_imagebutton_state_t state) const noexcept {
        return lv_imagebutton_get_src_right(m_obj, state);
    }

    // ==================== State ====================

    /// Make checkable
    ImageButton& checkable(bool enable = true) noexcept {
        if (enable) {
            lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CHECKABLE);
        } else {
            lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_CHECKABLE);
        }
        return *this;
    }

    /// Check if checked
    [[nodiscard]] bool is_checked() const noexcept {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    // ==================== Size ====================

    /// Set size
    ImageButton& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
