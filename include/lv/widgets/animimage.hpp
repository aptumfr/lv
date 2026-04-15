#pragma once

/**
 * @file animimage.hpp
 * @brief Zero-cost wrapper for LVGL animated image widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Animated Image widget wrapper
 *
 * An image that cycles through a sequence of frames.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class AnimImage : public ObjectView,
            public ObjectMixin<AnimImage>,
                  public EventMixin<AnimImage>,
                  public StyleMixin<AnimImage> {
public:
    constexpr AnimImage() noexcept : ObjectView(nullptr) {}
    constexpr AnimImage(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_animimg_class; }

    [[nodiscard]] static AnimImage create(lv_obj_t* parent) {
        return AnimImage(wrap, lv_animimg_create(parent));
    }
    [[nodiscard]] static AnimImage create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Source ====================

    /// Set image sources (array of image descriptors)
    AnimImage& src(const void* srcs[], uint8_t count) noexcept {
        lv_animimg_set_src(m_obj, srcs, count);
        return *this;
    }

    // ==================== Animation ====================

    /// Set animation duration (time to show all frames once)
    AnimImage& duration(uint32_t ms) noexcept {
        lv_animimg_set_duration(m_obj, ms);
        return *this;
    }

    /// Set repeat count (0 = infinite)
    AnimImage& repeat_count(uint32_t count) noexcept {
        lv_animimg_set_repeat_count(m_obj, count);
        return *this;
    }

    /// Start animation
    AnimImage& start() noexcept {
        lv_animimg_start(m_obj);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    AnimImage& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
