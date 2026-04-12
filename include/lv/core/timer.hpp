#pragma once

/**
 * @file timer.hpp
 * @brief Zero-cost timer wrapper for LVGL
 *
 * Provides C++ API for LVGL's timer system for periodic callbacks.
 *
 * ## Member Function Support
 *
 * Type-safe member function callbacks with zero overhead:
 *
 *   m_timer = lv::Timer::create<&MyClass::on_tick>(100, this);
 *   m_timer = lv::timer_periodic<&MyClass::on_tick>(100, this);
 */

#include <lvgl.h>
#include <cstdint>
#include <type_traits>

namespace lv {

namespace detail {

/// Trampoline for member functions with lv_timer_t* signature (zero storage)
template<auto MemFn, typename T>
struct TimerMemberTrampoline {
    static void callback(lv_timer_t* t) {
        auto* instance = static_cast<T*>(lv_timer_get_user_data(t));
        (instance->*MemFn)(t);
    }
};

/// Trampoline for member functions with void() signature (zero storage)
template<auto MemFn, typename T>
struct TimerMemberTrampolineNoArg {
    static void callback(lv_timer_t* t) {
        auto* instance = static_cast<T*>(lv_timer_get_user_data(t));
        (instance->*MemFn)();
    }
};

/// Detect void() member function signature
template<typename> struct is_void_member : std::false_type {};
template<typename C> struct is_void_member<void(C::*)()> : std::true_type {};
template<typename C> struct is_void_member<void(C::*)() noexcept> : std::true_type {};
template<typename C> struct is_void_member<void(C::*)() const> : std::true_type {};
template<typename C> struct is_void_member<void(C::*)() const noexcept> : std::true_type {};

template<auto MemFn>
inline constexpr bool is_void_member_v = is_void_member<decltype(MemFn)>::value;

} // namespace detail

/**
 * @brief Timer wrapper for periodic callbacks
 *
 * RAII wrapper for lv_timer_t with fluent API.
 *
 * Usage:
 *   lv::Timer timer([]() {
 *       // Called every 100ms
 *   }, 100);
 *
 *   // Or with user data:
 *   lv::Timer timer([](lv_timer_t* t) {
 *       auto* data = static_cast<MyData*>(lv_timer_get_user_data(t));
 *   }, 100, &my_data);
 */
class Timer {
    lv_timer_t* m_timer = nullptr;
    bool m_owned = true;

public:
    /// Default-construct an empty/null Timer (owns nothing).
    ///
    /// Use this for deferred-init members: `lv::Timer m_timer{};` then
    /// move-assign later once you know the callback and period.
    constexpr Timer() noexcept : m_timer(nullptr), m_owned(false) {}

    /// Create timer with callback and period
    Timer(lv_timer_cb_t cb, uint32_t period_ms, void* user_data = nullptr) noexcept
        : m_timer(lv_timer_create(cb, period_ms, user_data)), m_owned(true) {}

    /// Wrap existing timer (non-owning).
    /// @deprecated Use lv::timer_ref(raw) / lv::TimerRef instead — TimerRef
    ///             is the dedicated non-owning handle. This constructor
    ///             remains for the default-constructed empty case, but
    ///             new callers should prefer TimerRef for non-owning wraps.
    [[deprecated("Use lv::TimerRef / lv::timer_ref() for non-owning timer handles; "
                 "use Timer{} for empty/null Timer members.")]]
    explicit Timer(lv_timer_t* timer) noexcept
        : m_timer(timer), m_owned(false) {}

    /// Create paused timer
    static Timer paused(lv_timer_cb_t cb, uint32_t period_ms, void* user_data = nullptr) noexcept {
        Timer t(cb, period_ms, user_data);
        t.pause();
        return t;
    }

    /// Create timer with member function callback (zero-cost trampoline)
    template<auto MemFn, typename T>
    [[nodiscard]] static Timer create(uint32_t period_ms, T* instance) noexcept {
        if constexpr (detail::is_void_member_v<MemFn>) {
            return Timer(&detail::TimerMemberTrampolineNoArg<MemFn, T>::callback, period_ms, instance);
        } else {
            return Timer(&detail::TimerMemberTrampoline<MemFn, T>::callback, period_ms, instance);
        }
    }

    ~Timer() {
        if (m_owned && m_timer) {
            lv_timer_delete(m_timer);
        }
    }

    // Non-copyable
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    // Moveable
    Timer(Timer&& other) noexcept : m_timer(other.m_timer), m_owned(other.m_owned) {
        other.m_timer = nullptr;
        other.m_owned = false;
    }

    Timer& operator=(Timer&& other) noexcept {
        if (this != &other) {
            if (m_owned && m_timer) {
                lv_timer_delete(m_timer);
            }
            m_timer = other.m_timer;
            m_owned = other.m_owned;
            other.m_timer = nullptr;
            other.m_owned = false;
        }
        return *this;
    }

    /// Get underlying timer pointer
    [[nodiscard]] lv_timer_t* get() noexcept { return m_timer; }
    [[nodiscard]] const lv_timer_t* get() const noexcept { return m_timer; }

    /// Check if valid
    [[nodiscard]] bool valid() const noexcept { return m_timer != nullptr; }
    explicit operator bool() const noexcept { return valid(); }

    // ==================== Configuration ====================

    /// Set period in milliseconds
    Timer& period(uint32_t ms) noexcept {
        lv_timer_set_period(m_timer, ms);
        return *this;
    }

    /// Set callback
    Timer& callback(lv_timer_cb_t cb) noexcept {
        lv_timer_set_cb(m_timer, cb);
        return *this;
    }

    /// Set user data
    Timer& user_data(void* data) noexcept {
        lv_timer_set_user_data(m_timer, data);
        return *this;
    }

    /// Get user data
    [[nodiscard]] void* user_data() const noexcept {
        return lv_timer_get_user_data(m_timer);
    }

    /// Set repeat count (-1 for infinite)
    Timer& repeat(int32_t count) noexcept {
        lv_timer_set_repeat_count(m_timer, count);
        return *this;
    }

    /// Run once then stop (Timer stays in control of deletion)
    /// LVGL's auto-delete is disabled so the RAII destructor handles cleanup.
    /// The timer is paused after firing — safe to destroy at any time.
    Timer& once() noexcept {
        lv_timer_set_repeat_count(m_timer, 1);
        lv_timer_set_auto_delete(m_timer, false);
        return *this;
    }


    /// Run forever (default)
    Timer& forever() noexcept {
        return repeat(-1);
    }

    // ==================== Control ====================

    /// Pause the timer
    Timer& pause() noexcept {
        lv_timer_pause(m_timer);
        return *this;
    }

    /// Resume the timer
    Timer& resume() noexcept {
        lv_timer_resume(m_timer);
        return *this;
    }

    /// Check if paused
    [[nodiscard]] bool is_paused() const noexcept {
        return lv_timer_get_paused(m_timer);
    }

    /// Reset timer (restart period)
    Timer& reset() noexcept {
        lv_timer_reset(m_timer);
        return *this;
    }

    /// Make timer ready (callback will be called on next lv_timer_handler)
    Timer& ready() noexcept {
        lv_timer_ready(m_timer);
        return *this;
    }

    /// Delete the timer (releases ownership)
    void del() noexcept {
        if (m_timer) {
            lv_timer_delete(m_timer);
            m_timer = nullptr;
            m_owned = false;
        }
    }

    /// Release ownership (timer won't be deleted in destructor)
    [[nodiscard]] lv_timer_t* release() noexcept {
        m_owned = false;
        return m_timer;
    }
};

// ==================== Timer Helpers ====================

/// Create a one-shot timer (runs callback once after delay)
inline Timer timer_once(lv_timer_cb_t cb, uint32_t delay_ms, void* user_data = nullptr) {
    Timer t(cb, delay_ms, user_data);
    t.once();
    return t;
}

/// Create a one-shot timer with member function callback
template<auto MemFn, typename T>
Timer timer_once(uint32_t delay_ms, T* instance) {
    Timer t = Timer::create<MemFn>(delay_ms, instance);
    t.once();
    return t;
}

/// Create a periodic timer
inline Timer timer_periodic(lv_timer_cb_t cb, uint32_t period_ms, void* user_data = nullptr) {
    return Timer(cb, period_ms, user_data);
}

/// Create a periodic timer with member function callback
template<auto MemFn, typename T>
Timer timer_periodic(uint32_t period_ms, T* instance) {
    return Timer::create<MemFn>(period_ms, instance);
}

/**
 * @brief Non-owning, copyable timer handle
 *
 * Like ObjectView for objects — holds a pointer without managing lifetime.
 * Safe to capture in lambdas for pause/resume control.
 */
class TimerRef {
    lv_timer_t* m_timer;

public:
    explicit constexpr TimerRef(lv_timer_t* t) noexcept : m_timer(t) {}

    /// Construct from owning Timer (rejects temporaries)
    TimerRef(Timer& t) noexcept : m_timer(t.get()) {}

    [[nodiscard]] lv_timer_t* get() const noexcept { return m_timer; }
    [[nodiscard]] explicit operator bool() const noexcept { return m_timer != nullptr; }

    const TimerRef& pause() const noexcept { lv_timer_pause(m_timer); return *this; }
    const TimerRef& resume() const noexcept { lv_timer_resume(m_timer); return *this; }
    [[nodiscard]] bool is_paused() const noexcept { return lv_timer_get_paused(m_timer); }
    const TimerRef& reset() const noexcept { lv_timer_reset(m_timer); return *this; }
    const TimerRef& period(uint32_t ms) const noexcept { lv_timer_set_period(m_timer, ms); return *this; }
    const TimerRef& ready() const noexcept { lv_timer_ready(m_timer); return *this; }

    /// Get user data pointer.
    [[nodiscard]] void* user_data() const noexcept {
        return lv_timer_get_user_data(m_timer);
    }

    /// Get user data as typed pointer.
    template<typename T>
    [[nodiscard]] T* user_data_as() const noexcept {
        return static_cast<T*>(lv_timer_get_user_data(m_timer));
    }
};

/// Get a non-owning, copyable timer handle
inline TimerRef timer_ref(lv_timer_t* t) noexcept {
    return TimerRef(t);
}

/// Get a non-owning, copyable handle from an owning Timer
inline TimerRef timer_ref(Timer& t) noexcept {
    return TimerRef(t);
}

/// Get idle percentage (0-100)
inline uint8_t timer_idle() noexcept {
    return lv_timer_get_idle();
}

/// Enable/disable timer handling
inline void timer_enable(bool enable) noexcept {
    lv_timer_enable(enable);
}

} // namespace lv
