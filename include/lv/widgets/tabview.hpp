#pragma once

/**
 * @file tabview.hpp
 * @brief Zero-cost wrapper for LVGL tabview widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Tabview widget wrapper
 *
 * A container with tabs for switching between pages.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Tabview : public ObjectView,
            public ObjectMixin<Tabview>,
                public EventMixin<Tabview>,
                public StyleMixin<Tabview> {
public:
    constexpr Tabview() noexcept : ObjectView(nullptr) {}

    constexpr Tabview(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_tabview_class; }

    [[nodiscard]] static Tabview create(lv_obj_t* parent) {
        return Tabview(wrap, lv_tabview_create(parent));
    }

    [[nodiscard]] static Tabview create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Tabs ====================

    /// Add a tab (returns the tab content object)
    [[nodiscard]] ObjectRef add_tab(const char* name) noexcept {
        return ObjectRef(lv_tabview_add_tab(m_obj, name));
    }

    /// Rename a tab
    Tabview& rename_tab(uint32_t idx, const char* name) noexcept {
        lv_tabview_rename_tab(m_obj, idx, name);
        return *this;
    }

    /// Get tab count
    [[nodiscard]] uint32_t tab_count() const noexcept {
        return lv_tabview_get_tab_count(m_obj);
    }

    // ==================== Selection ====================

    /// Set active tab
    Tabview& active(uint32_t idx) noexcept {
        lv_tabview_set_active(m_obj, idx, LV_ANIM_OFF);
        return *this;
    }

    /// Set active tab with animation
    Tabview& active_anim(uint32_t idx) noexcept {
        lv_tabview_set_active(m_obj, idx, LV_ANIM_ON);
        return *this;
    }

    /// Get active tab index
    [[nodiscard]] uint32_t active() const noexcept {
        return lv_tabview_get_tab_active(m_obj);
    }

    // ==================== Tab Bar ====================

    /// Get tab bar object (for styling)
    [[nodiscard]] ObjectRef tab_bar() const noexcept {
        return ObjectRef(lv_tabview_get_tab_bar(m_obj));
    }

    /// Set tab bar position
    Tabview& tab_bar_position(lv_dir_t dir) noexcept {
        lv_tabview_set_tab_bar_position(m_obj, dir);
        return *this;
    }

    /// Tab bar on top
    Tabview& tab_bar_top() noexcept {
        return tab_bar_position(LV_DIR_TOP);
    }

    /// Tab bar on bottom
    Tabview& tab_bar_bottom() noexcept {
        return tab_bar_position(LV_DIR_BOTTOM);
    }

    /// Tab bar on left
    Tabview& tab_bar_left() noexcept {
        return tab_bar_position(LV_DIR_LEFT);
    }

    /// Tab bar on right
    Tabview& tab_bar_right() noexcept {
        return tab_bar_position(LV_DIR_RIGHT);
    }

    /// Set tab bar size
    Tabview& tab_bar_size(int32_t size) noexcept {
        lv_tabview_set_tab_bar_size(m_obj, size);
        return *this;
    }

    // ==================== Content ====================

    /// Get content container
    [[nodiscard]] ObjectRef content() const noexcept {
        return ObjectRef(lv_tabview_get_content(m_obj));
    }

    // ==================== Size ====================

    /// Set size
    Tabview& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Fill parent
    Tabview& fill() noexcept {
        lv_obj_set_size(m_obj, LV_PCT(100), LV_PCT(100));
        return *this;
    }
};

} // namespace lv
