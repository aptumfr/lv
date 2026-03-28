#pragma once

/**
 * @file component.hpp
 * @brief Zero-overhead CRTP component base class
 *
 * Provides a React/SwiftUI-like component pattern using CRTP
 * (Curiously Recurring Template Pattern) for zero virtual call overhead.
 */

#include <lvgl.h>
#include "objectref.hpp"

namespace lv {

/**
 * @brief CRTP base class for UI components
 *
 * Components encapsulate UI logic and state. The derived class implements
 * build() which returns the root object (any ObjectView-convertible type).
 *
 * Features:
 * - Zero virtual call overhead (CRTP static dispatch)
 * - Component lookup via event-descriptor scanning (no user_data collision)
 * - Mount/unmount lifecycle management
 * - user_data() on component roots is freely usable by application code
 *
 * Size: sizeof(void*) = 8 bytes on 64-bit (just the root pointer)
 *
 * Ownership lookup: root_delete_cb is registered on every component root.
 * Its callback address &Component<Derived>::root_delete_cb is unique per
 * Derived type. owner_from_obj() scans event descriptors to find it and
 * extracts the component pointer from the event's user_data. This avoids
 * probing lv_obj_t::user_data entirely, eliminating UB.
 *
 * Usage:
 * @code
 * class MyComponent : public lv::Component<MyComponent> {
 *     lv::State<int> counter{0};
 *
 * public:
 *     lv::ObjectView build(lv::ObjectView parent) {
 *         auto root = lv::vbox(parent).padding(10).gap(5);
 *
 *         lv::Label::create(root)
 *             .bind_text(counter, "Count: %d");
 *
 *         lv::Button::create(root)
 *             .text("Increment")
 *             .on_click<&MyComponent::on_increment>(this);
 *
 *         return root;
 *     }
 *
 * private:
 *     void on_increment(lv_event_t*) {
 *         ++counter;
 *     }
 * };
 *
 * // Usage:
 * MyComponent comp;
 * comp.mount(lv::screen_active());
 * @endcode
 *
 * @tparam Derived The derived component class (CRTP pattern)
 */
template<typename Derived>
class Component {
protected:
    lv_obj_t* m_root = nullptr;

private:
    /// Delete-event hook: nulls m_root when LVGL deletes the root externally
    /// (e.g. screen auto_del). Prevents double-delete in destructor/unmount.
    static void root_delete_cb(lv_event_t* e) noexcept {
        auto* derived = static_cast<Derived*>(lv_event_get_user_data(e));
        if (!derived) return;

        auto* self = static_cast<Component*>(derived);
        if (self->m_root == lv_event_get_current_target_obj(e)) {
            self->m_root = nullptr;
        }
    }

    static void attach_root_delete_hook(lv_obj_t* root, Derived* owner) noexcept {
        lv_obj_add_event_cb(root, &Component::root_delete_cb, LV_EVENT_DELETE, owner);
    }

    static void rebind_root_delete_hook(lv_obj_t* root, Derived* old_owner,
                                        Derived* new_owner) noexcept {
        lv_obj_remove_event_cb_with_user_data(root, &Component::root_delete_cb, old_owner);
        attach_root_delete_hook(root, new_owner);
    }

    /// Scan event descriptors on obj for root_delete_cb; return owning Derived*.
    /// The callback address &Component::root_delete_cb is unique per Derived type,
    /// so this is type-safe. Uses only public, stable LVGL APIs.
    [[nodiscard]] static Derived* owner_from_obj(lv_obj_t* obj) noexcept {
        if (!obj) return nullptr;
        const uint32_t n = lv_obj_get_event_count(obj);
        for (uint32_t i = 0; i < n; ++i) {
            lv_event_dsc_t* d = lv_obj_get_event_dsc(obj, i);
            if (!d) continue;
            if (lv_event_dsc_get_cb(d) == &Component::root_delete_cb) {
                return static_cast<Derived*>(lv_event_dsc_get_user_data(d));
            }
        }
        return nullptr;
    }

public:
    Component() = default;

    // Non-copyable (components manage UI state)
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    // Moveable
    Component(Component&& other) noexcept
        : m_root(other.m_root) {
        other.m_root = nullptr;
        if (m_root) {
            auto* old_owner = static_cast<Derived*>(&other);
            auto* new_owner = static_cast<Derived*>(this);
            rebind_root_delete_hook(m_root, old_owner, new_owner);
        }
    }

    Component& operator=(Component&& other) noexcept {
        if (this != &other) {
            unmount();
            m_root = other.m_root;
            other.m_root = nullptr;
            if (m_root) {
                auto* old_owner = static_cast<Derived*>(&other);
                auto* new_owner = static_cast<Derived*>(this);
                rebind_root_delete_hook(m_root, old_owner, new_owner);
            }
        }
        return *this;
    }

    /// Destructor unmounts if mounted
    ~Component() {
        unmount();
    }

    // ==================== Lifecycle ====================

    /**
     * @brief Mount the component to a parent
     *
     * Calls the derived class's build() method and stores the result.
     * A delete-event hook is registered on the root to track external deletion.
     * The root's user_data is not touched — it remains available for application use.
     *
     * @param parent The parent object (ObjectView)
     */
    void mount(ObjectView parent) {
        if (m_root) {
            unmount();
        }

        // Call derived class build() - CRTP static dispatch
        ObjectView root = static_cast<Derived*>(this)->build(parent);
        m_root = root.get();

        if (m_root) {
            attach_root_delete_hook(m_root, static_cast<Derived*>(this));

            // Call optional lifecycle hook
            if constexpr (requires { static_cast<Derived*>(this)->on_mount(); }) {
                static_cast<Derived*>(this)->on_mount();
            }
        }
    }

    /**
     * @brief Unmount the component
     *
     * Deletes the root object (LVGL cascades to children).
     */
    void unmount() {
        if (m_root) {
            // Call optional lifecycle hook.
            // Note: the hook may delete m_root externally, which triggers
            // root_delete_cb and nulls m_root. We must recheck afterward.
            if constexpr (requires { static_cast<Derived*>(this)->on_unmount(); }) {
                static_cast<Derived*>(this)->on_unmount();
            }

            if (m_root) {
                lv_obj_delete(m_root);
                m_root = nullptr;
            }
        }
    }

    /**
     * @brief Check if component is mounted
     */
    [[nodiscard]] bool is_mounted() const noexcept {
        return m_root != nullptr;
    }

    /**
     * @brief Check if component is visible
     */
    [[nodiscard]] bool is_visible() const noexcept {
        return m_root && !lv_obj_has_flag(m_root, LV_OBJ_FLAG_HIDDEN);
    }

    /**
     * @brief Hide the component (keeps UI in memory)
     */
    void hide() noexcept {
        if (m_root) {
            lv_obj_add_flag(m_root, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /**
     * @brief Show the component
     */
    void show() noexcept {
        if (m_root) {
            lv_obj_remove_flag(m_root, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /**
     * @brief Set visibility
     */
    void visible(bool v) noexcept {
        if (v) show(); else hide();
    }

    /**
     * @brief Get the root object as ObjectRef
     */
    [[nodiscard]] ObjectRef root() const noexcept {
        return ObjectRef(m_root);
    }

    // ==================== Static Helpers ====================

    /**
     * @brief Get component instance from an event
     *
     * Walks up the widget tree scanning event descriptors on each node
     * for the root_delete_cb unique to this Component<Derived> type.
     * O(events) per node, type-safe (only matches this specific Derived type).
     *
     * @code
     * void handle_event(lv_event_t* e) {
     *     auto* self = MyComponent::from_event(e);
     *     if (self) self->do_something();
     * }
     * @endcode
     *
     * @return Pointer to component, or nullptr if not found
     */
    [[nodiscard]] static Derived* from_event(lv_event_t* e) noexcept {
        lv_obj_t* target = lv_event_get_current_target_obj(e);
        while (target) {
            Derived* owner = owner_from_obj(target);
            if (owner) return owner;
            target = lv_obj_get_parent(target);
        }
        return nullptr;
    }

    /**
     * @brief Get component instance from a component root object
     *
     * @return Pointer to component, or nullptr if object is not a component root
     */
    [[nodiscard]] static Derived* from_obj(ObjectView obj) noexcept {
        return owner_from_obj(obj.get());
    }
};

namespace detail {
    struct SizeCheckComponent : Component<SizeCheckComponent> {
        ObjectView build(ObjectView) { return ObjectView(nullptr); }
    };
    static_assert(sizeof(SizeCheckComponent) == sizeof(void*),
                  "Component should be exactly one pointer (m_root)");
}

/**
 * @brief Mixin for components that need screen integration
 *
 * Use this when the component represents a full screen.
 * Tracks the screen object separately from m_root to prevent leaks.
 *
 * Non-moveable: screen-level components manage LVGL screen lifetime
 * and should not be moved.
 *
 * Destruction order:
 *   ~ScreenComponent calls unmount() (fires on_unmount, deletes m_root),
 *   then deletes m_screen. ~Component runs unmount() again (m_root==nullptr, no-op).
 */
template<typename Derived>
class ScreenComponent : public Component<Derived> {
    lv_obj_t* m_screen = nullptr;

    static void screen_delete_cb(lv_event_t* e) noexcept {
        auto* self = static_cast<ScreenComponent*>(
            static_cast<Derived*>(lv_event_get_user_data(e)));
        if (self->m_screen == lv_event_get_current_target_obj(e)) {
            self->m_screen = nullptr;
        }
    }

public:
    ScreenComponent() = default;
    ScreenComponent(ScreenComponent&&) = delete;
    ScreenComponent& operator=(ScreenComponent&&) = delete;

    ~ScreenComponent() {
        // unmount() fires on_unmount() lifecycle hook and deletes m_root.
        // Then we clean up the screen object itself.
        this->unmount();
        if (m_screen) {
            lv_obj_delete(m_screen);
            m_screen = nullptr;
        }
    }

    /**
     * @brief Mount as a new screen
     *
     * Creates a new screen object and mounts the component to it.
     * Any previously mounted screen is unmounted first.
     */
    ObjectRef mount_screen() {
        unmount_screen();
        m_screen = lv_obj_create(nullptr);
        lv_obj_add_event_cb(m_screen, &ScreenComponent::screen_delete_cb,
                           LV_EVENT_DELETE, static_cast<Derived*>(this));
        this->mount(ObjectView(m_screen));
        return ObjectRef(m_screen);
    }

    /**
     * @brief Unmount screen and clean up both root and screen objects
     */
    void unmount_screen() {
        this->unmount();   // deletes m_root (child)
        if (m_screen) {
            lv_obj_delete(m_screen);
            m_screen = nullptr;
        }
    }

    /**
     * @brief Mount and load as active screen
     */
    void mount_and_load() {
        ObjectView screen = mount_screen();
        lv_screen_load(screen);
    }

    /**
     * @brief Mount and load with animation
     */
    void mount_and_load_anim(lv_screen_load_anim_t anim, uint32_t time,
                              uint32_t delay = 0, bool auto_del = true) {
        ObjectView screen = mount_screen();
        lv_screen_load_anim(screen, anim, time, delay, auto_del);
    }

    /// Get the screen object
    [[nodiscard]] ObjectRef screen() const noexcept {
        return ObjectRef(m_screen);
    }
};

} // namespace lv
