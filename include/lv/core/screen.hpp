#pragma once

/**
 * @file screen.hpp
 * @brief Screen management and navigation helpers for LVGL
 *
 * Provides C++ API for managing screens and transitions.
 */

#include <lvgl.h>
#include "objectref.hpp"
#include "anim.hpp"

namespace lv {

// ==================== Screen Helpers ====================

// Forward declaration
class Screen;

/// Get the active screen
inline Screen screen_active() noexcept;

/// Load a screen (make it active)
inline void screen_load(ObjectView scr) noexcept {
    lv_screen_load(scr.get());
}

/// Load a screen with animation
inline void screen_load_anim(ObjectView scr, lv_screen_load_anim_t anim, uint32_t time_ms,
                              uint32_t delay_ms = 0, bool auto_del = false) noexcept {
    lv_screen_load_anim(scr.get(), anim, time_ms, delay_ms, auto_del);
}

/// Create a new screen
inline ObjectRef screen_create() noexcept {
    return ObjectRef(lv_obj_create(nullptr));
}

// ==================== Screen Load Animations ====================

namespace screen_anim {
    constexpr auto none = LV_SCR_LOAD_ANIM_NONE;
    constexpr auto over_left = LV_SCR_LOAD_ANIM_OVER_LEFT;
    constexpr auto over_right = LV_SCR_LOAD_ANIM_OVER_RIGHT;
    constexpr auto over_top = LV_SCR_LOAD_ANIM_OVER_TOP;
    constexpr auto over_bottom = LV_SCR_LOAD_ANIM_OVER_BOTTOM;
    constexpr auto move_left = LV_SCR_LOAD_ANIM_MOVE_LEFT;
    constexpr auto move_right = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
    constexpr auto move_top = LV_SCR_LOAD_ANIM_MOVE_TOP;
    constexpr auto move_bottom = LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
    constexpr auto fade_in = LV_SCR_LOAD_ANIM_FADE_IN;
    constexpr auto fade_out = LV_SCR_LOAD_ANIM_FADE_OUT;
    constexpr auto out_left = LV_SCR_LOAD_ANIM_OUT_LEFT;
    constexpr auto out_right = LV_SCR_LOAD_ANIM_OUT_RIGHT;
    constexpr auto out_top = LV_SCR_LOAD_ANIM_OUT_TOP;
    constexpr auto out_bottom = LV_SCR_LOAD_ANIM_OUT_BOTTOM;
} // namespace screen_anim

/**
 * @brief Screen class for creating managed screens
 *
 * RAII wrapper for screen objects.
 * Inherits from mixins for full fluent API support.
 *
 * Usage:
 *   lv::Screen home;
 *   lv::Label::create(home).text("Home");
 *   home.load();  // Make active
 */
class Screen : public ObjectView,
               public ObjectMixin<Screen>,
               public EventMixin<Screen>,
               public StyleMixin<Screen> {
public:
    /// Create a new screen
    Screen() noexcept : ObjectView(lv_obj_create(nullptr)) {}

    /// Wrap existing screen object
    Screen(lv::wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    // ==================== Loading ====================

    /// Load this screen (make active).
    /// @note The previously active screen is NOT deleted — it stays in
    ///       memory and can be loaded again later. If you don't need it,
    ///       delete it yourself or use load_anim() with auto_del = true.
    void load() noexcept {
        lv_screen_load(m_obj);
    }

    /// Load with animation.
    /// @param auto_del If true, the old screen is deleted after the
    ///                 transition finishes.
    void load_anim(lv_screen_load_anim_t anim, uint32_t time_ms,
                   uint32_t delay_ms = 0, bool auto_del = false) noexcept {
        lv_screen_load_anim(m_obj, anim, time_ms, delay_ms, auto_del);
    }

    /// Slide in from left
    void slide_left(uint32_t time_ms = 300) noexcept {
        load_anim(LV_SCR_LOAD_ANIM_MOVE_LEFT, time_ms);
    }

    /// Slide in from right
    void slide_right(uint32_t time_ms = 300) noexcept {
        load_anim(LV_SCR_LOAD_ANIM_MOVE_RIGHT, time_ms);
    }

    /// Slide in from top
    void slide_top(uint32_t time_ms = 300) noexcept {
        load_anim(LV_SCR_LOAD_ANIM_MOVE_TOP, time_ms);
    }

    /// Slide in from bottom
    void slide_bottom(uint32_t time_ms = 300) noexcept {
        load_anim(LV_SCR_LOAD_ANIM_MOVE_BOTTOM, time_ms);
    }

    /// Fade in
    void fade_in(uint32_t time_ms = 300) noexcept {
        load_anim(LV_SCR_LOAD_ANIM_FADE_IN, time_ms);
    }

    // ==================== State ====================

    /// Check if this is the active screen
    [[nodiscard]] bool is_active() const noexcept {
        return m_obj == lv_screen_active();
    }

    // ==================== Styling (convenience aliases) ====================

    /// Set text font (alias for text_font from StyleMixin)
    Screen& font(const lv_font_t* f) noexcept {
        lv_obj_set_style_text_font(m_obj, f, 0);
        return *this;
    }
};

/// Get the active screen (implementation after Screen class)
inline Screen screen_active() noexcept {
    return Screen(lv::wrap, lv_screen_active());
}

/**
 * @brief Simple screen navigator
 *
 * Manages a stack of screens for back navigation.
 *
 * Usage:
 *   lv::Navigator nav;
 *   nav.push(home_screen);
 *   nav.push(settings_screen, lv::screen_anim::slide_left, 300);
 *   nav.back();  // Returns to home with reverse animation
 */
class Navigator {
    static constexpr size_t MAX_SCREENS = 8;
    ObjectView m_stack[MAX_SCREENS];
    size_t m_depth = 0;

public:
    Navigator() noexcept = default;

    /// Push a screen onto the stack and load it
    void push(ObjectView screen, lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_NONE,
              uint32_t time_ms = 0) noexcept {
        if (m_depth < MAX_SCREENS) {
            m_stack[m_depth++] = screen;
            if (anim == LV_SCR_LOAD_ANIM_NONE) {
                lv_screen_load(screen.get());
            } else {
                lv_screen_load_anim(screen.get(), anim, time_ms, 0, false);
            }
        }
    }

    /// Go back to previous screen
    bool back(uint32_t time_ms = 300) noexcept {
        if (m_depth > 1) {
            m_depth--;
            ObjectView prev = m_stack[m_depth - 1];
            // Use reverse animation
            lv_screen_load_anim(prev.get(), LV_SCR_LOAD_ANIM_MOVE_RIGHT, time_ms, 0, false);
            return true;
        }
        return false;
    }

    /// Get current screen
    [[nodiscard]] ObjectRef current() const noexcept {
        return m_depth > 0 ? m_stack[m_depth - 1] : ObjectRef(nullptr);
    }

    /// Get stack depth
    [[nodiscard]] size_t depth() const noexcept {
        return m_depth;
    }

    /// Check if we can go back
    [[nodiscard]] bool can_back() const noexcept {
        return m_depth > 1;
    }

    /// Clear the stack
    void clear() noexcept {
        m_depth = 0;
    }

    /// Set root screen (clears stack and sets single screen)
    void set_root(ObjectView screen) noexcept {
        m_depth = 1;
        m_stack[0] = screen;
        lv_screen_load(screen.get());
    }
};

} // namespace lv
