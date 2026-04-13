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
#include <type_traits>
#include <utility>

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

    [[nodiscard]] ObjectRef screen() const noexcept {
        return ObjectRef(lv_obj_get_screen(get()));
    }

    [[nodiscard]] ObjectRef child(int32_t idx) const noexcept {
        return ObjectRef(lv_obj_get_child(get(), idx));
    }

    [[nodiscard]] ObjectRef child_by_type(int32_t idx, const lv_obj_class_t* class_p) const noexcept {
        return ObjectRef(lv_obj_get_child_by_type(get(), idx, class_p));
    }

    [[nodiscard]] ObjectRef sibling(int32_t idx) const noexcept {
        return ObjectRef(lv_obj_get_sibling(get(), idx));
    }

    [[nodiscard]] ObjectRef sibling_by_type(int32_t idx, const lv_obj_class_t* class_p) const noexcept {
        return ObjectRef(lv_obj_get_sibling_by_type(get(), idx, class_p));
    }

    [[nodiscard]] ObjectRef find_by_name(const char* name) const noexcept {
#if LV_USE_OBJ_NAME
        return ObjectRef(lv_obj_find_by_name(get(), name));
#else
        (void)name; // To remove compiler warning
        return ObjectRef(nullptr);
#endif
    }

    [[nodiscard]] ObjectRef child_by_name(const char* name_path) const noexcept {
#if LV_USE_OBJ_NAME
        return ObjectRef(lv_obj_get_child_by_name(get(), name_path));
#else
        (void)name_path; // To remove compiler warning
        return ObjectRef(nullptr);
#endif
    }
};

static_assert(sizeof(ObjectRef) == sizeof(void*),
    "ObjectRef must be exactly pointer-sized for zero overhead");

/// Get the full C++ interface for a raw LVGL object pointer
inline ObjectRef ref(lv_obj_t* obj) noexcept {
    return ObjectRef(obj);
}

template<typename T>
concept redundant_ref_target =
    std::is_base_of_v<ObjectView, std::remove_cvref_t<T>> &&
    !std::is_same_v<std::remove_cvref_t<T>, ObjectView>;

/// Passing an lv wrapper with the full fluent API to lv::ref() is redundant.
template<redundant_ref_target T>
[[deprecated("Unnecessary lv::ref() on an lv wrapper type; call methods directly on the wrapper")]]
constexpr decltype(auto) ref(T&& obj) noexcept {
    return std::forward<T>(obj);
}

// ==================== Deferred definitions from object.hpp ====================

inline ObjectRef ObjectView::parent() const noexcept {
    return ObjectRef(lv_obj_get_parent(m_obj));
}

inline ObjectRef ObjectView::screen() const noexcept {
    return ObjectRef(lv_obj_get_screen(m_obj));
}

inline ObjectRef ObjectView::child(int32_t idx) const noexcept {
    return ObjectRef(lv_obj_get_child(m_obj, idx));
}

inline ObjectRef ObjectView::child_by_type(int32_t idx, const lv_obj_class_t* class_p) const noexcept {
    return ObjectRef(lv_obj_get_child_by_type(m_obj, idx, class_p));
}

inline ObjectRef ObjectView::sibling(int32_t idx) const noexcept {
    return ObjectRef(lv_obj_get_sibling(m_obj, idx));
}

inline ObjectRef ObjectView::sibling_by_type(int32_t idx, const lv_obj_class_t* class_p) const noexcept {
    return ObjectRef(lv_obj_get_sibling_by_type(m_obj, idx, class_p));
}

inline ObjectRef ObjectView::find_by_name(const char* name) const noexcept {
#if LV_USE_OBJ_NAME
    return ObjectRef(lv_obj_find_by_name(m_obj, name));
#else
    (void)name; // To remove compiler warning
    return ObjectRef(nullptr);
#endif
}

inline ObjectRef ObjectView::child_by_name(const char* name_path) const noexcept {
#if LV_USE_OBJ_NAME
    return ObjectRef(lv_obj_get_child_by_name(m_obj, name_path));
#else
    (void)name_path; // To remove compiler warning
    return ObjectRef(nullptr);
#endif
}

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
