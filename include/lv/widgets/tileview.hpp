#pragma once

/**
 * @file tileview.hpp
 * @brief Zero-cost wrapper for LVGL tileview widget
 */

#include <lvgl.h>
#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Tileview widget wrapper
 *
 * A container with swipeable tiles arranged in a grid.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Tileview : public ObjectView,
            public ObjectMixin<Tileview>,
                 public EventMixin<Tileview>,
                 public StyleMixin<Tileview> {
public:
    constexpr Tileview() noexcept : ObjectView(nullptr) {}

    constexpr Tileview(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_tileview_class; }

    [[nodiscard]] static Tileview create(lv_obj_t* parent) {
        return Tileview(wrap, lv_tileview_create(parent));
    }

    [[nodiscard]] static Tileview create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Tiles ====================

    /// Add a tile at position (returns the tile content object)
    [[nodiscard]] ObjectRef add_tile(uint8_t col, uint8_t row, lv_dir_t dir) noexcept {
        return ObjectRef(lv_tileview_add_tile(m_obj, col, row, dir));
    }

    /// Get active tile
    [[nodiscard]] ObjectRef active_tile() const noexcept {
        return ObjectRef(lv_tileview_get_tile_active(m_obj));
    }

    /// Set active tile by object
    Tileview& active_tile(lv_obj_t* tile, bool anim = false) noexcept {
        lv_tileview_set_tile(m_obj, tile, anim ? LV_ANIM_ON : LV_ANIM_OFF);
        return *this;
    }

    /// Set active tile by position
    Tileview& active_tile_by_index(uint32_t col, uint32_t row, bool anim = false) noexcept {
        lv_tileview_set_tile_by_index(m_obj, col, row, anim ? LV_ANIM_ON : LV_ANIM_OFF);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Tileview& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /// Fill parent
    Tileview& fill() noexcept {
        lv_obj_set_size(m_obj, LV_PCT(100), LV_PCT(100));
        return *this;
    }
};

} // namespace lv
