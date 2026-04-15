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
 * // Get the fluent API on an existing lv_obj_t*
 * auto wrapped = lv::ref(raw_ptr);
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
    /// Default constructor (null until assigned via create() or wrap).
    ///
    /// Use this for deferred-init members:
    ///   lv::Box m_panel{};           // null here
    ///   m_panel = lv::Box::create(parent);  // real widget later
    constexpr Box() noexcept : ObjectView(nullptr) {}

    /// Wrap an existing lv_obj_t* as a Box (does NOT create a new object)
    /// Usage: auto box = lv::Box(lv::wrap, existing_obj);
    constexpr Box(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    /// Box wraps the generic lv_obj, so this returns &lv_obj_class.
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_obj_class; }

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

    // size(), width(), height(), fill(), fill_width(), fill_height(),
    // pos(), x(), y(), center(), align(), grow(), scrollable()
    // are inherited from ObjectMixin<Box>.
};

} // namespace lv
