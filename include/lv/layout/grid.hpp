#pragma once

/**
 * @file grid.hpp
 * @brief Zero-cost grid layout wrapper for LVGL
 *
 * Provides a C++ DSL for LVGL's CSS-like grid layout system.
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"
#include <initializer_list>

namespace lv {

/**
 * @brief Grid container wrapper
 *
 * Provides a fluent API for CSS-like grid layouts.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Grid : public ObjectView,
             public ObjectMixin<Grid>,
             public EventMixin<Grid>,
             public StyleMixin<Grid> {
public:
    // Grid declares its own align() for grid column/row alignment,
    // which would otherwise hide ObjectMixin<Grid>::align() for
    // object positioning (C++ name hiding applies to all base
    // overloads). Re-expose the object-positioning overloads so
    // users can still call e.g. Grid(parent).align(LV_ALIGN_CENTER, 0, 0).
    // Overload resolution distinguishes the two by argument type
    // (lv_align_t vs lv_grid_align_t).
    using ObjectMixin<Grid>::align;

    // ==================== Nested Types ====================

    /// Fractional unit for grid tracks (like CSS fr)
    static constexpr int32_t fr(uint8_t x) noexcept {
        return LV_GRID_FR(x);
    }

    /// Content-sized track
    static constexpr int32_t content = LV_GRID_CONTENT;

    /// Grid template terminator
    static constexpr int32_t template_last = LV_GRID_TEMPLATE_LAST;

    /// Grid alignment constants
    struct Align {
        static constexpr auto start = LV_GRID_ALIGN_START;
        static constexpr auto center = LV_GRID_ALIGN_CENTER;
        static constexpr auto end = LV_GRID_ALIGN_END;
        static constexpr auto stretch = LV_GRID_ALIGN_STRETCH;
        static constexpr auto space_evenly = LV_GRID_ALIGN_SPACE_EVENLY;
        static constexpr auto space_around = LV_GRID_ALIGN_SPACE_AROUND;
        static constexpr auto space_between = LV_GRID_ALIGN_SPACE_BETWEEN;
    };

    // ==================== Constructors ====================

    /// Create a grid container
    explicit Grid(ObjectView parent)
        : ObjectView(lv_obj_create(parent)) {
        lv_obj_set_layout(m_obj, LV_LAYOUT_GRID);
        lv_obj_set_size(m_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    Grid(lv::wrap_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    // ==================== Grid Template ====================

    /// Set column and row descriptors (arrays must remain valid, end with LV_GRID_TEMPLATE_LAST)
    Grid& dsc_array(const int32_t col_dsc[], const int32_t row_dsc[]) noexcept {
        lv_obj_set_grid_dsc_array(m_obj, col_dsc, row_dsc);
        return *this;
    }

    /// Set column descriptor only
    Grid& columns(const int32_t col_dsc[]) noexcept {
        lv_obj_set_style_grid_column_dsc_array(m_obj, col_dsc, 0);
        return *this;
    }

    /// Set row descriptor only
    Grid& rows(const int32_t row_dsc[]) noexcept {
        lv_obj_set_style_grid_row_dsc_array(m_obj, row_dsc, 0);
        return *this;
    }

    // ==================== Alignment ====================

    /// Set grid alignment for columns and rows
    Grid& align(lv_grid_align_t col_align, lv_grid_align_t row_align) noexcept {
        lv_obj_set_grid_align(m_obj, col_align, row_align);
        return *this;
    }

    /// Set column alignment
    Grid& column_align(lv_grid_align_t a) noexcept {
        lv_obj_set_style_grid_column_align(m_obj, a, 0);
        return *this;
    }

    /// Set row alignment
    Grid& row_align(lv_grid_align_t a) noexcept {
        lv_obj_set_style_grid_row_align(m_obj, a, 0);
        return *this;
    }

    // ==================== Gap ====================

    /// Set gap between cells (both column and row)
    Grid& gap(int32_t g) noexcept {
        lv_obj_set_style_pad_row(m_obj, g, 0);
        lv_obj_set_style_pad_column(m_obj, g, 0);
        return *this;
    }

    /// Set column gap only
    Grid& column_gap(int32_t g) noexcept {
        lv_obj_set_style_pad_column(m_obj, g, 0);
        return *this;
    }

    /// Set row gap only
    Grid& row_gap(int32_t g) noexcept {
        lv_obj_set_style_pad_row(m_obj, g, 0);
        return *this;
    }

    // size(), width(), height(), size_content(), fill_width(), fill_height(), fill()
    // are inherited from ObjectMixin<Grid>.
};

// ==================== Grid Cell Placement ====================

/**
 * @brief Helper to place a child in a grid cell
 *
 * Usage:
 *   lv::grid_cell(button)
 *       .col(0).row(0)
 *       .col_span(2)
 *       .align(LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_CENTER);
 */
class GridCell {
    lv_obj_t* m_obj;
    lv_grid_align_t m_col_align = LV_GRID_ALIGN_STRETCH;
    int32_t m_col_pos = 0;
    int32_t m_col_span = 1;
    lv_grid_align_t m_row_align = LV_GRID_ALIGN_STRETCH;
    int32_t m_row_pos = 0;
    int32_t m_row_span = 1;

public:
    explicit GridCell(ObjectView obj) noexcept : m_obj(obj.get()) {}

    /// Set column position
    GridCell& col(int32_t pos) noexcept {
        m_col_pos = pos;
        apply();
        return *this;
    }

    /// Set row position
    GridCell& row(int32_t pos) noexcept {
        m_row_pos = pos;
        apply();
        return *this;
    }

    /// Set column and row position
    GridCell& pos(int32_t c, int32_t r) noexcept {
        m_col_pos = c;
        m_row_pos = r;
        apply();
        return *this;
    }

    /// Set column span
    GridCell& col_span(int32_t span) noexcept {
        m_col_span = span;
        apply();
        return *this;
    }

    /// Set row span
    GridCell& row_span(int32_t span) noexcept {
        m_row_span = span;
        apply();
        return *this;
    }

    /// Set column alignment
    GridCell& col_align(lv_grid_align_t a) noexcept {
        m_col_align = a;
        apply();
        return *this;
    }

    /// Set row alignment
    GridCell& row_align(lv_grid_align_t a) noexcept {
        m_row_align = a;
        apply();
        return *this;
    }

    /// Set both alignments
    GridCell& align(lv_grid_align_t col, lv_grid_align_t row) noexcept {
        m_col_align = col;
        m_row_align = row;
        apply();
        return *this;
    }

    /// Center in cell
    GridCell& center() noexcept {
        return align(LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_CENTER);
    }

    /// Stretch to fill cell (default)
    GridCell& stretch() noexcept {
        return align(LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);
    }

private:
    void apply() noexcept {
        lv_obj_set_grid_cell(m_obj, m_col_align, m_col_pos, m_col_span,
                             m_row_align, m_row_pos, m_row_span);
    }
};

/// Create a grid cell helper for positioning
inline GridCell grid_cell(ObjectView obj) noexcept {
    return GridCell(obj);
}

// ==================== Factory Function ====================

/// Create a grid container
inline Grid grid(ObjectView parent) {
    return Grid(parent);
}

} // namespace lv
