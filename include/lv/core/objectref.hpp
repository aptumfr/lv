#pragma once

/**
 * @file objectref.hpp
 * @brief Full-featured non-owning reference to an LVGL object
 *
 * ObjectRef combines ObjectView with ObjectMixin, EventMixin, and StyleMixin,
 * providing the complete fluent API on a non-owning pointer.
 *
 * Use ObjectRef as the return type for methods that hand out sub-objects
 * users may want to manipulate (e.g. root(), add_button(), tab_bar()).
 *
 * Size: sizeof(void*) — zero overhead via Empty Base Optimization.
 */

#include "object.hpp"
#include "event.hpp"
#include "style.hpp"

namespace lv {

class ObjectRef : public ObjectView,
                  public ObjectMixin<ObjectRef>,
                  public EventMixin<ObjectRef>,
                  public StyleMixin<ObjectRef> {
public:
    using ObjectView::ObjectView; // inherit lv_obj_t* and default constructors

    /// Convert from any ObjectView (or ObjectView-derived type)
    constexpr ObjectRef(ObjectView v) noexcept : ObjectView(v.get()) {}

    /// Parent/child navigation returns ObjectRef for continued chaining
    [[nodiscard]] ObjectRef parent() const noexcept {
        return ObjectRef(lv_obj_get_parent(get()));
    }

    [[nodiscard]] ObjectRef child(int32_t idx) const noexcept {
        return ObjectRef(lv_obj_get_child(get(), idx));
    }
};

static_assert(sizeof(ObjectRef) == sizeof(void*),
    "ObjectRef must be exactly pointer-sized for zero overhead");

// ==================== Deferred definitions from event.hpp ====================
// These break the circular dependency: event.hpp forward-declares ObjectRef,
// objectref.hpp provides the bodies after ObjectRef is complete.

inline ObjectRef Event::target() const noexcept {
    return ObjectRef(lv_event_get_target_obj(m_event));
}

inline ObjectRef Event::current_target() const noexcept {
    return ObjectRef(lv_event_get_current_target_obj(m_event));
}

[[deprecated("Use Event::target() instead")]]
inline ObjectRef event_target(lv_event_t* e) noexcept {
    return ObjectRef(lv_event_get_target_obj(e));
}

[[deprecated("Use Event::current_target() instead")]]
inline ObjectRef event_current_target(lv_event_t* e) noexcept {
    return ObjectRef(lv_event_get_current_target_obj(e));
}

} // namespace lv
