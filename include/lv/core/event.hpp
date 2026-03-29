#pragma once

/**
 * @file event.hpp
 * @brief Zero-cost event handling for LVGL objects
 *
 * Provides type-safe, zero-allocation event callbacks using:
 * 1. Stateless lambdas (convertible to function pointer) - ZERO overhead
 * 2. Member function pointers with instance - ZERO overhead (uses user_data)
 * 3. Function pointers - ZERO overhead
 *
 * ## Usage Examples
 *
 * @code
 * // 1. Stateless lambda with lv_event_t* (C-style)
 * btn.on(lv::kEvent::clicked, [](lv_event_t* e) { ... });
 *
 * // 2. Stateless lambda with lv::Event (C++ style)
 * btn.on(lv::kEvent::clicked, [](lv::Event e) {
 *     auto target = e.target();  // Returns ObjectRef
 * });
 *
 * // 3. Function pointer with lv_event_t*
 * void my_handler(lv_event_t* e) { ... }
 * btn.on(lv::kEvent::clicked, my_handler);
 *
 * // 4. Function pointer with lv::Event (template syntax required)
 * void my_handler(lv::Event e) { ... }
 * btn.on<&my_handler>(lv::kEvent::clicked);  // Note: function as template arg
 *
 * // 5. Member function pointer
 * class MyApp {
 *     void on_click(lv::Event e) { ... }
 * };
 * btn.on<&MyApp::on_click>(lv::kEvent::clicked, this);
 * @endcode
 *
 * ## Why function pointers with lv::Event need template syntax
 *
 * For zero-cost callbacks, the function address must be known at compile time.
 * With `btn.on(code, my_handler)`, the pointer value is a runtime argument.
 * With `btn.on<&my_handler>(code)`, it's a compile-time template parameter.
 *
 * If you try `btn.on(code, my_handler)` with `void my_handler(lv::Event)`,
 * you'll get a clear compile error explaining the alternatives.
 */

#include <lvgl.h>
#include <type_traits>
#include <concepts>
#include <cstring>
#include <utility>
#include "object.hpp"  // For ObjectView


namespace lv {

// Forward declaration — full definition in objectref.hpp
class ObjectRef;

// ==================== Event Types ====================

/// Raw event data type (alias for lv_event_t)
using EventData = lv_event_t;

// ==================== Type Traits ====================

/// Check if F is a stateless lambda convertible to function pointer
template<typename F, typename... Args>
concept StatelessCallable = std::is_convertible_v<F, void(*)(Args...)>;

/// Check if F is invocable with given args
template<typename F, typename... Args>
concept Invocable = std::is_invocable_v<F, Args...>;

/// Check if F is a callable (lambda) that captures state
template<typename F, typename... Args>
concept CapturingCallable = std::is_invocable_v<F, Args...> && !std::is_convertible_v<F, void(*)(Args...)>;

/**
 * @brief Non-owning wrapper for LVGL event
 *
 * Provides a clean C++ interface to lv_event_t.
 * Zero overhead - just wraps the pointer.
 *
 * Usage in callbacks:
 * @code
 * void on_click(lv::Event e) {
 *     auto code = e.code();
 * }
 * @endcode
 */
class Event {
    lv_event_t* m_event;

public:
    // ==================== Constructors ====================

    constexpr explicit Event(lv_event_t* e) noexcept : m_event(e) {}

    /// Get raw pointer for C API interop
    [[nodiscard]] constexpr lv_event_t* get() const noexcept { return m_event; }
    [[nodiscard]] constexpr operator lv_event_t*() const noexcept { return m_event; }

    /// Get the target object (original object that received the event)
    [[nodiscard]] inline ObjectRef target() const noexcept;

    /// Get the current target (may differ during bubbling)
    [[nodiscard]] inline ObjectRef current_target() const noexcept;

    /// Get the event code
    [[nodiscard]] lv_event_code_t code() const noexcept {
        return lv_event_get_code(m_event);
    }

    /// Get user data
    [[nodiscard]] void* user_data() const noexcept {
        return lv_event_get_user_data(m_event);
    }

    /// Get user data as typed pointer
    template<typename T>
    [[nodiscard]] T* user_data_as() const noexcept {
        return static_cast<T*>(lv_event_get_user_data(m_event));
    }

    /// Get event parameter
    [[nodiscard]] void* param() const noexcept {
        return lv_event_get_param(m_event);
    }

    /// Get event parameter as typed pointer
    template<typename T>
    [[nodiscard]] T* param_as() const noexcept {
        return static_cast<T*>(lv_event_get_param(m_event));
    }

    /// Get the input device that triggered the event
    [[nodiscard]] lv_indev_t* indev() const noexcept {
        return lv_event_get_indev(m_event);
    }

    /// Stop event bubbling
    void stop_bubbling() const noexcept {
        lv_event_stop_bubbling(m_event);
    }

    /// Stop event processing
    void stop_processing() const noexcept {
        lv_event_stop_processing(m_event);
    }

    // ==================== Code Checks ====================

    [[nodiscard]] bool is_clicked() const noexcept {
        return code() == LV_EVENT_CLICKED;
    }

    [[nodiscard]] bool is_pressed() const noexcept {
        return code() == LV_EVENT_PRESSED;
    }

    [[nodiscard]] bool is_released() const noexcept {
        return code() == LV_EVENT_RELEASED;
    }

    [[nodiscard]] bool is_value_changed() const noexcept {
        return code() == LV_EVENT_VALUE_CHANGED;
    }

    [[nodiscard]] bool is_focused() const noexcept {
        return code() == LV_EVENT_FOCUSED;
    }

    [[nodiscard]] bool is_defocused() const noexcept {
        return code() == LV_EVENT_DEFOCUSED;
    }
};

/// Check if F is a stateless lambda taking lv::Event
template<typename F>
concept StatelessEventCallable = std::is_convertible_v<F, void(*)(Event)>;

// ==================== Event Callback Helpers ====================

namespace detail {

/**
 * @brief Trampoline for stateless lambdas taking lv::Event
 *
 * Wraps a void(*)(Event) function pointer to void(*)(lv_event_t*).
 * Zero overhead - the conversion happens at the call site.
 */
template<void(*Fn)(Event)>
struct EventTrampoline {
    static void callback(lv_event_t* e) {
        Fn(Event(e));
    }
};

/**
 * @brief Trampoline for member function callbacks (lv_event_t* signature)
 *
 * This creates a static function that can be passed to LVGL,
 * which retrieves the instance from user_data and calls the member function.
 * The member function pointer is encoded as a template parameter (zero storage).
 */
template<auto MemFn, typename T>
struct MemberTrampoline {
    static void callback(lv_event_t* e) {
        auto* instance = static_cast<T*>(lv_event_get_user_data(e));
        (instance->*MemFn)(e);
    }
};

/**
 * @brief Trampoline for member functions with lv::Event signature
 */
template<auto MemFn, typename T>
struct MemberTrampolineEvent {
    static void callback(lv_event_t* e) {
        auto* instance = static_cast<T*>(lv_event_get_user_data(e));
        (instance->*MemFn)(Event(e));
    }
};

/**
 * @brief Trampoline for member functions that don't need the event
 */
template<auto MemFn, typename T>
struct MemberTrampolineNoArg {
    static void callback(lv_event_t* e) {
        auto* instance = static_cast<T*>(lv_event_get_user_data(e));
        (instance->*MemFn)();
    }
};

} // namespace detail


/**
 * @brief Mixin class providing event handling methods for widgets
 *
 * This is a CRTP mixin that adds event handling to widget classes.
 * Include this in widget wrappers to enable fluent event registration.
 *
 * Usage:
 *   class Button : public ObjectView, public EventMixin<Button> { ... };
 *
 * ## Convenience Helpers
 *
 * Shorthand methods are provided for common UI widget events:
 * - Input: on_click, on_pressed, on_released, on_single/double/triple_clicked, on_long_pressed
 * - Focus: on_focused, on_defocused
 * - Hover: on_hover_over, on_hover_leave
 * - Scroll: on_scroll, on_scroll_begin, on_scroll_end, on_gesture
 * - Value: on_value_changed
 *
 * For less common events (rotary, key, indev_reset, draw events, etc.), use the
 * generic on() method with lv::kEvent:: constants.
 *
 * ## Display Events Warning
 *
 * Events in kEvent like render_*, refr_*, flush_*, vsync_* are **display events**
 * that fire on lv_display_t objects, NOT widgets. Registering them on a widget
 * via this mixin will compile but the callback will never fire.
 *
 * For display events, use: lv_display_add_event_cb(display, callback, code, user_data)
 */
template<typename Derived>
class EventMixin {
private:
    [[nodiscard]] lv_obj_t* obj() noexcept {
        return static_cast<Derived*>(this)->get();
    }

public:
    // ==================== Zero-Cost Callbacks ====================

    /**
     * @brief Add event callback using stateless lambda or function pointer (lv_event_t*)
     *
     * Zero overhead - the lambda is converted to a function pointer.
     * Only works with captureless lambdas.
     *
     * @code
     * btn.on(lv::Event::clicked, [](lv_event_t* e) {
     *     // handle click (C-style)
     * });
     * @endcode
     */
    template<typename F>
        requires StatelessCallable<F, lv_event_t*>
    Derived& on(lv_event_code_t code, F&& fn) noexcept {
        lv_obj_add_event_cb(obj(), +fn, code, nullptr);
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Add event callback using stateless lambda taking lv::Event
     *
     * Zero overhead - the lambda is converted to a function pointer,
     * wrapped to convert lv_event_t* to lv::Event.
     *
     * @warning ONLY stateless (captureless) lambdas are supported!
     * The passed callable is NOT stored - instead, a default-constructed
     * instance F{} is invoked. This works because stateless lambdas are
     * default-constructible and all instances are identical. If you need
     * to capture state, use the member function overload:
     *   btn.on<&MyClass::handler>(event_code, this);
     *
     * @note For function pointers taking lv::Event, use the explicit overload
     * that stores and invokes the actual pointer.
     *
     * @code
     * // OK: stateless lambda
     * btn.on(lv::kEvent::clicked, [](lv::Event e) {
     *     auto target = e.target();
     * });
     *
     * // ERROR: capturing lambda - use member function instead
     * int counter = 0;
     * btn.on(lv::kEvent::clicked, [&counter](lv::Event e) { counter++; }); // Won't compile
     * @endcode
     */
    template<typename F>
        requires (StatelessEventCallable<F> && !std::is_pointer_v<std::decay_t<F>>)
    Derived& on(lv_event_code_t code, F&& /*fn*/) noexcept {
        // Safety checks for the trampoline pattern
        static_assert(std::is_empty_v<F>,
            "Callable must be stateless (no captures). Use member function pattern: "
            "btn.on<&MyClass::handler>(code, this)");
        static_assert(std::is_default_constructible_v<F>,
            "Callable must be default constructible");

        // Trampoline: default-construct F and invoke it
        // This is safe because stateless lambdas are all identical instances
        auto wrapper = [](lv_event_t* e) {
            F{}(Event(e));
        };
        lv_obj_add_event_cb(obj(), +wrapper, code, nullptr);
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Add event callback using function pointer taking lv::Event
     *
     * Zero overhead - function pointer is a template parameter, so the
     * trampoline is generated at compile time with no runtime lookup.
     *
     * @code
     * void my_handler(lv::Event e) {
     *     auto target = e.target();
     * }
     * btn.on<&my_handler>(lv::kEvent::clicked);
     * @endcode
     */
    template<void(*Fn)(Event)>
    Derived& on(lv_event_code_t code) noexcept {
        lv_obj_add_event_cb(obj(), &detail::EventTrampoline<Fn>::callback, code, nullptr);
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Overload for function pointers taking lv::Event passed as argument
     *
     * This overload exists only to provide a clear compile-time error message.
     * Use the template syntax for zero-cost: btn.on<&my_handler>(code);
     */
    template<typename Dummy = void>
    Derived& on(lv_event_code_t, void(*)(Event)) noexcept {
        static_assert(!std::is_same_v<Dummy, void>,
            "Cannot pass void(*)(lv::Event) as argument - use template syntax instead: "
            "btn.on<&my_handler>(code); "
            "Or use: btn.on(code, [](lv::Event e) { ... }); "
            "Or use: btn.on(code, my_c_handler); // void my_c_handler(lv_event_t*)");
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Deleted overload for capturing lambdas - provides helpful error message
     */
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on(lv_event_code_t code, F&& fn) = delete;  // Use member function: btn.on<&MyClass::handler>(event, this)

    /// @deprecated Use on() instead
    template<typename F>
        requires StatelessCallable<F, lv_event_t*>
    [[deprecated("Use on() instead of on_event()")]]
    Derived& on_event(lv_event_code_t code, F&& fn) noexcept {
        return on(code, std::forward<F>(fn));
    }

    /**
     * @brief Add a runtime-selected LVGL event callback with explicit user_data
     *
     * This is the low-level escape hatch for systems like interpreters and
     * dynamic UI runtimes that cannot encode the callback target as a template
     * parameter. No allocation is performed; the caller owns `user_data` and
     * is responsible for keeping it alive until the callback is removed or the
     * object is deleted.
     */
    Derived& on(lv_event_code_t code, lv_event_cb_t cb, void* user_data) noexcept {
        lv_obj_add_event_cb(obj(), cb, code, user_data);
        return *static_cast<Derived*>(this);
    }

    /// Convenience alias for explicitly low-level callback registration.
    Derived& on_raw(lv_event_code_t code, lv_event_cb_t cb, void* user_data = nullptr) noexcept {
        return on(code, cb, user_data);
    }

    /**
     * @brief Add event callback using member function pointer
     *
     * Zero overhead - member function pointer is template parameter,
     * instance is stored in LVGL's user_data.
     *
     * Supports multiple signatures:
     * - void(lv::Event) - recommended
     * - void(lv_event_t*) - legacy C-style
     * - void() - when event data not needed
     *
     * @code
     * class MyApp {
     *     void on_click(lv::Event e) { ... }
     * };
     * btn.on<&MyApp::on_click>(lv::Event::clicked, this);
     * @endcode
     */
    template<auto MemFn, typename T>
        requires std::is_member_function_pointer_v<decltype(MemFn)>
    Derived& on(lv_event_code_t code, T* instance) noexcept {
        // Detect the signature and use appropriate trampoline
        if constexpr (std::is_invocable_v<decltype(MemFn), T*, Event>) {
            using Trampoline = detail::MemberTrampolineEvent<MemFn, T>;
            lv_obj_add_event_cb(obj(), &Trampoline::callback, code, instance);
        } else if constexpr (std::is_invocable_v<decltype(MemFn), T*, lv_event_t*>) {
            using Trampoline = detail::MemberTrampoline<MemFn, T>;
            lv_obj_add_event_cb(obj(), &Trampoline::callback, code, instance);
        } else {
            using Trampoline = detail::MemberTrampolineNoArg<MemFn, T>;
            lv_obj_add_event_cb(obj(), &Trampoline::callback, code, instance);
        }
        return *static_cast<Derived*>(this);
    }

    /// @deprecated Use on() instead
    template<auto MemFn, typename T>
        requires std::is_member_function_pointer_v<decltype(MemFn)>
    [[deprecated("Use on() instead of on_event()")]]
    Derived& on_event(lv_event_code_t code, T* instance) noexcept {
        return on<MemFn>(code, instance);
    }

    /**
     * @brief Add event callback using member function (no event param)
     *
     * For member functions that don't need the event parameter.
     *
     * @code
     * class MyApp {
     *     void handle_click() { ... }
     * };
     * btn.on_simple<&MyApp::handle_click>(lv::Event::clicked, this);
     * @endcode
     */
    template<auto MemFn, typename T>
        requires std::is_member_function_pointer_v<decltype(MemFn)>
    [[deprecated("Use on() instead — it auto-detects void() signatures")]]
    Derived& on_simple(lv_event_code_t code, T* instance) noexcept {
        using Trampoline = detail::MemberTrampolineNoArg<MemFn, T>;
        lv_obj_add_event_cb(obj(), &Trampoline::callback, code, instance);
        return *static_cast<Derived*>(this);
    }

    /// @deprecated Use on_simple() instead
    template<auto MemFn, typename T>
        requires std::is_member_function_pointer_v<decltype(MemFn)>
    [[deprecated("Use on_simple() instead of on_event_simple()")]]
    Derived& on_event_simple(lv_event_code_t code, T* instance) noexcept {
        return on_simple<MemFn>(code, instance);
    }

    // ==================== Send ====================

    /// Send an event to this object (for programmatic event simulation)
    lv_result_t send_event(lv_event_code_t code, void* param = nullptr) noexcept {
        return lv_obj_send_event(obj(), code, param);
    }

    // ==================== Removal ====================

    /// Remove callback descriptor by index
    [[nodiscard]] bool remove_event(uint32_t index) noexcept {
        return lv_obj_remove_event(obj(), index);
    }

    /// Remove a specific event descriptor
    [[nodiscard]] bool remove_event_dsc(lv_event_dsc_t* dsc) noexcept {
        return lv_obj_remove_event_dsc(obj(), dsc);
    }

    /// Remove all callbacks matching event callback function
    uint32_t remove_event_cb(lv_event_cb_t cb) noexcept {
        return lv_obj_remove_event_cb(obj(), cb);
    }

    /// Remove callbacks matching event callback function and user_data
    uint32_t remove_event_cb(lv_event_cb_t cb, void* user_data) noexcept {
        return lv_obj_remove_event_cb_with_user_data(obj(), cb, user_data);
    }

    // ==================== Convenience Methods ====================

    /// Shorthand for clicked event (stateless lambda with lv_event_t* or lv::Event)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_click(F&& fn) noexcept {
        return on(LV_EVENT_CLICKED, std::forward<F>(fn));
    }

    /// Shorthand for clicked event with member function
    template<auto MemFn, typename T>
    Derived& on_click(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_CLICKED, instance);
    }

    /// Deleted: capturing lambda - use member function instead
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on_click(F&& fn) = delete;  // Use member function: btn.on_click<&MyClass::handler>(this)

    /// Shorthand for value_changed event (stateless lambda with lv_event_t* or lv::Event)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_value_changed(F&& fn) noexcept {
        return on(LV_EVENT_VALUE_CHANGED, std::forward<F>(fn));
    }

    /// Shorthand for value_changed event with member function
    template<auto MemFn, typename T>
    Derived& on_value_changed(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_VALUE_CHANGED, instance);
    }

    /// Deleted: capturing lambda - use member function instead
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on_value_changed(F&& fn) = delete;  // Use member function: btn.on_value_changed<&MyClass::handler>(this)

    /// Shorthand for pressed event (stateless lambda with lv_event_t* or lv::Event)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_pressed(F&& fn) noexcept {
        return on(LV_EVENT_PRESSED, std::forward<F>(fn));
    }

    /// Shorthand for released event (stateless lambda with lv_event_t* or lv::Event)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_released(F&& fn) noexcept {
        return on(LV_EVENT_RELEASED, std::forward<F>(fn));
    }

    /// Deleted: capturing lambda - use member function instead
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on_pressed(F&& fn) = delete;  // Use member function: btn.on<&MyClass::handler>(lv::kEvent::pressed, this)

    /// Deleted: capturing lambda - use member function instead
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on_released(F&& fn) = delete;  // Use member function: btn.on<&MyClass::handler>(lv::kEvent::released, this)

    /// Shorthand for focused event (stateless lambda with lv_event_t* or lv::Event)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_focused(F&& fn) noexcept {
        return on(LV_EVENT_FOCUSED, std::forward<F>(fn));
    }

    /// Shorthand for focused event with member function
    template<auto MemFn, typename T>
    Derived& on_focused(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_FOCUSED, instance);
    }

    /// Shorthand for defocused event (stateless lambda with lv_event_t* or lv::Event)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_defocused(F&& fn) noexcept {
        return on(LV_EVENT_DEFOCUSED, std::forward<F>(fn));
    }

    /// Shorthand for defocused event with member function
    template<auto MemFn, typename T>
    Derived& on_defocused(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_DEFOCUSED, instance);
    }

    /// Deleted: capturing lambda - use member function instead
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on_focused(F&& fn) = delete;  // Use member function: btn.on_focused<&MyClass::handler>(this)

    /// Deleted: capturing lambda - use member function instead
    template<typename F>
        requires (CapturingCallable<F, lv_event_t*> && !StatelessEventCallable<F>)
    Derived& on_defocused(F&& fn) = delete;  // Use member function: btn.on_defocused<&MyClass::handler>(this)

    // ==================== Multi-click Events ====================

    /// Shorthand for single_clicked event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_single_clicked(F&& fn) noexcept {
        return on(LV_EVENT_SINGLE_CLICKED, std::forward<F>(fn));
    }

    /// Shorthand for single_clicked event with member function
    template<auto MemFn, typename T>
    Derived& on_single_clicked(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_SINGLE_CLICKED, instance);
    }

    /// Shorthand for double_clicked event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_double_clicked(F&& fn) noexcept {
        return on(LV_EVENT_DOUBLE_CLICKED, std::forward<F>(fn));
    }

    /// Shorthand for double_clicked event with member function
    template<auto MemFn, typename T>
    Derived& on_double_clicked(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_DOUBLE_CLICKED, instance);
    }

    /// Shorthand for triple_clicked event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_triple_clicked(F&& fn) noexcept {
        return on(LV_EVENT_TRIPLE_CLICKED, std::forward<F>(fn));
    }

    /// Shorthand for triple_clicked event with member function
    template<auto MemFn, typename T>
    Derived& on_triple_clicked(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_TRIPLE_CLICKED, instance);
    }

    /// Shorthand for long_pressed event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_long_pressed(F&& fn) noexcept {
        return on(LV_EVENT_LONG_PRESSED, std::forward<F>(fn));
    }

    /// Shorthand for long_pressed event with member function
    template<auto MemFn, typename T>
    Derived& on_long_pressed(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_LONG_PRESSED, instance);
    }

    // ==================== Hover Events ====================

    /// Shorthand for hover_over event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_hover_over(F&& fn) noexcept {
        return on(LV_EVENT_HOVER_OVER, std::forward<F>(fn));
    }

    /// Shorthand for hover_over event with member function
    template<auto MemFn, typename T>
    Derived& on_hover_over(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_HOVER_OVER, instance);
    }

    /// Shorthand for hover_leave event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_hover_leave(F&& fn) noexcept {
        return on(LV_EVENT_HOVER_LEAVE, std::forward<F>(fn));
    }

    /// Shorthand for hover_leave event with member function
    template<auto MemFn, typename T>
    Derived& on_hover_leave(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_HOVER_LEAVE, instance);
    }

    // ==================== Scroll & Gesture Events ====================

    /// Shorthand for scroll event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_scroll(F&& fn) noexcept {
        return on(LV_EVENT_SCROLL, std::forward<F>(fn));
    }

    /// Shorthand for scroll event with member function
    template<auto MemFn, typename T>
    Derived& on_scroll(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_SCROLL, instance);
    }

    /// Shorthand for scroll_begin event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_scroll_begin(F&& fn) noexcept {
        return on(LV_EVENT_SCROLL_BEGIN, std::forward<F>(fn));
    }

    /// Shorthand for scroll_begin event with member function
    template<auto MemFn, typename T>
    Derived& on_scroll_begin(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_SCROLL_BEGIN, instance);
    }

    /// Shorthand for scroll_end event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_scroll_end(F&& fn) noexcept {
        return on(LV_EVENT_SCROLL_END, std::forward<F>(fn));
    }

    /// Shorthand for scroll_end event with member function
    template<auto MemFn, typename T>
    Derived& on_scroll_end(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_SCROLL_END, instance);
    }

    /// Shorthand for gesture event (stateless lambda)
    template<typename F>
        requires StatelessCallable<F, lv_event_t*> || StatelessEventCallable<F>
    Derived& on_gesture(F&& fn) noexcept {
        return on(LV_EVENT_GESTURE, std::forward<F>(fn));
    }

    /// Shorthand for gesture event with member function
    template<auto MemFn, typename T>
    Derived& on_gesture(T* instance) noexcept {
        return on<MemFn>(LV_EVENT_GESTURE, instance);
    }

    // ==================== Notes ====================
    //
    // Capturing lambdas are intentionally NOT supported.
    // When you try to use a capturing lambda, you'll get a compile error
    // pointing you to use a member function instead:
    //
    //   btn.on_click<&MyClass::handler>(this);
    //
    // This is zero-cost and matches how C handles stateful callbacks (via user_data).
};


// ==================== Utility Functions (deprecated, use Event class) ====================

/// Get the target object from an event
[[deprecated("Use Event::target() instead")]]
inline ObjectRef event_target(lv_event_t* e) noexcept;

/// Get the current target object from an event (during bubbling)
[[deprecated("Use Event::current_target() instead")]]
inline ObjectRef event_current_target(lv_event_t* e) noexcept;

/// Get the event code
[[deprecated("Use Event::code() instead")]]
inline lv_event_code_t event_code(lv_event_t* e) noexcept {
    return lv_event_get_code(e);
}

/// Get event parameter as typed pointer
template<typename T>
[[deprecated("Use Event::param_as<T>() instead")]]
inline T* event_param(lv_event_t* e) noexcept {
    return static_cast<T*>(lv_event_get_param(e));
}

/// Stop event propagation
[[deprecated("Use Event::stop_bubbling() instead")]]
inline void event_stop_bubbling(lv_event_t* e) noexcept {
    lv_event_stop_bubbling(e);
}

/// Stop event processing
[[deprecated("Use Event::stop_processing() instead")]]
inline void event_stop_processing(lv_event_t* e) noexcept {
    lv_event_stop_processing(e);
}

} // namespace lv

// Deferred include: provides ObjectRef definition and Event::target()/current_target()
// bodies. Pragma-once guards prevent circular inclusion.
#include "objectref.hpp"
