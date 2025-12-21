#pragma once

/**
 * @file box.hpp
 * @brief Zero-cost wrapper for basic styled container object
 *
 * A Box is a simple styled container (lv_obj) with fluent API.
 * Use for custom-styled containers that aren't flex layouts.
 *
 * @code
 * // Create a box using the static factory
 * auto box = lv::Box::create(parent)
 *     .size(100, 50)
 *     .bg_color(lv::rgb(0x2196F3));  // blue
 *
 * // Wrap an existing lv_obj_t*
 * auto wrapped = lv::Box(lv::wrap, raw_ptr);
 * @endcode
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Basic styled container widget
 *
 * A simple object wrapper with full styling support.
 * Use when you need a custom-styled container without flex layout.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Box : public ObjectView,
            public ObjectMixin<Box>,
            public EventMixin<Box>,
            public StyleMixin<Box> {
public:
    /// Default constructor (null/invalid box)
    constexpr Box() noexcept : ObjectView(nullptr) {}

    /// Wrap an existing lv_obj_t* as a Box (does NOT create a new object)
    /// Usage: auto box = lv::Box(lv::wrap, existing_obj);
    constexpr Box(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    // Default copy/move - copies the pointer, does NOT create new lv_obj
    Box(const Box&) = default;
    Box& operator=(const Box&) = default;
    Box(Box&&) = default;
    Box& operator=(Box&&) = default;

    /**
     * @brief Create a new Box widget
     *
     * This is the preferred way to create widgets. It clearly indicates
     * that a new LVGL object is being created.
     *
     * @param parent Parent object (any ObjectView-derived type or lv_obj_t*)
     * @return New Box instance
     *
     * @code
     * auto box = lv::Box::create(parent)
     *     .size(100, 50)
     *     .bg_color(lv::rgb(0xF44336));  // red
     * @endcode
     */
    [[nodiscard]] static Box create(lv_obj_t* parent) {
        Box box;
        box.m_obj = lv_obj_create(parent);
        lv_obj_remove_flag(box.m_obj, LV_OBJ_FLAG_SCROLLABLE);
        return box;
    }

    [[nodiscard]] static Box create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Size ====================

    /// Set size in pixels
    Box& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Set width in pixels
    Box& width(int32_t w) noexcept {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    /// Set height in pixels
    Box& height(int32_t h) noexcept {
        lv_obj_set_height(m_obj, h);
        return *this;
    }

    /// Fill parent width
    Box& fill_width() noexcept {
        lv_obj_set_width(m_obj, LV_PCT(100));
        return *this;
    }

    /// Fill parent height
    Box& fill_height() noexcept {
        lv_obj_set_height(m_obj, LV_PCT(100));
        return *this;
    }

    /// Fill parent (both dimensions)
    Box& fill() noexcept {
        lv_obj_set_size(m_obj, LV_PCT(100), LV_PCT(100));
        return *this;
    }

    // ==================== Position ====================

    /// Set position
    Box& pos(int32_t x, int32_t y) noexcept {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /// Set X position
    Box& x(int32_t x) noexcept {
        lv_obj_set_x(m_obj, x);
        return *this;
    }

    /// Set Y position
    Box& y(int32_t y) noexcept {
        lv_obj_set_y(m_obj, y);
        return *this;
    }

    /// Center in parent
    Box& center() noexcept {
        lv_obj_center(m_obj);
        return *this;
    }

    /// Align relative to parent with offset
    Box& align(lv_align_t align, int32_t x_ofs, int32_t y_ofs) noexcept {
        lv_obj_align(m_obj, align, x_ofs, y_ofs);
        return *this;
    }

    /// Set alignment mode (without changing position)
    Box& align(lv_align_t align) noexcept {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    // ==================== Flex Child ====================

    /// Set flex grow factor (when child of flex container)
    Box& grow(uint8_t g = 1) noexcept {
        lv_obj_set_flex_grow(m_obj, g);
        return *this;
    }

    // ==================== Scrolling ====================

    /// Enable/disable scrolling
    Box& scrollable(bool v = true) noexcept {
        if (v) {
            lv_obj_add_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
        } else {
            lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        return *this;
    }
};

} // namespace lv
