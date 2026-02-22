#pragma once

/**
 * @file widget_cast.hpp
 * @brief Type-safe widget downcasting and parent search
 *
 * Provides widget_cast<T>(ObjectView) and find_parent<T>(ObjectView)
 * for safe downcasting of LVGL object pointers. Uses lv_obj_check_type()
 * at runtime (one pointer comparison) with zero allocation.
 *
 * Only widgets that register a detail::widget_lv_class<> specialization
 * are supported. Attempting to cast to an unregistered type produces a
 * compile-time error (the primary template is deleted).
 *
 * Layout wrappers (Flex, Grid) are intentionally unregistered because
 * they share lv_obj_class with Box; a runtime check cannot distinguish
 * them, so widget_cast is not meaningful for layout types.
 */

#include <lvgl.h>
#include <optional>
#include "object.hpp"
#include "wrap.hpp"

namespace lv {

/**
 * @brief Type-safe downcast of an ObjectView to a concrete widget type.
 *
 * Returns std::nullopt if the underlying lv_obj_t is null or not the
 * requested widget type.  Cost: one pointer comparison via
 * lv_obj_check_type(), zero allocation.
 *
 * @code
 * if (auto slider = lv::widget_cast<lv::Slider>(obj)) {
 *     slider->value(50);  // safe — type verified
 * }
 * @endcode
 */
template<typename Widget>
[[nodiscard]] std::optional<Widget> widget_cast(ObjectView obj) noexcept {
    lv_obj_t* raw = obj.get();
    if (raw && lv_obj_check_type(raw, detail::widget_lv_class<Widget>()))
        return Widget(wrap, raw);
    return std::nullopt;
}

/**
 * @brief Walk the parent chain and return the first ancestor of the
 *        requested widget type, or std::nullopt.
 *
 * @code
 * if (auto tab = lv::find_parent<lv::Tabview>(child)) {
 *     // child is inside a Tabview
 * }
 * @endcode
 */
template<typename Widget>
[[nodiscard]] std::optional<Widget> find_parent(ObjectView obj) noexcept {
    for (lv_obj_t* p = lv_obj_get_parent(obj.get()); p; p = lv_obj_get_parent(p)) {
        if (lv_obj_check_type(p, detail::widget_lv_class<Widget>()))
            return Widget(wrap, p);
    }
    return std::nullopt;
}

} // namespace lv
