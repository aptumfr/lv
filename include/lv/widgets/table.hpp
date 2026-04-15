#pragma once

/**
 * @file table.hpp
 * @brief Zero-cost wrapper for LVGL table widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Table widget wrapper
 *
 * A grid of cells for displaying tabular data.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Table : public ObjectView,
            public ObjectMixin<Table>,
              public EventMixin<Table>,
              public StyleMixin<Table> {
public:
    constexpr Table() noexcept : ObjectView(nullptr) {}
    constexpr Table(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_table_class; }

    [[nodiscard]] static Table create(lv_obj_t* parent) {
        return Table(wrap, lv_table_create(parent));
    }
    [[nodiscard]] static Table create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Structure ====================

    /// Set row count
    Table& row_count(uint32_t count) noexcept {
        lv_table_set_row_count(m_obj, count);
        return *this;
    }

    /// Get row count
    [[nodiscard]] uint32_t row_count() const noexcept {
        return lv_table_get_row_count(m_obj);
    }

    /// Set column count
    Table& column_count(uint32_t count) noexcept {
        lv_table_set_column_count(m_obj, count);
        return *this;
    }

    /// Get column count
    [[nodiscard]] uint32_t column_count() const noexcept {
        return lv_table_get_column_count(m_obj);
    }

    // ==================== Cell Content ====================

    /// Set cell value
    Table& cell_value(uint32_t row, uint32_t col, const char* txt) noexcept {
        lv_table_set_cell_value(m_obj, row, col, txt);
        return *this;
    }

    /// Set cell value with format string
    template<typename... Args>
    Table& cell_value_fmt(uint32_t row, uint32_t col, const char* fmt, Args... args) noexcept {
        lv_table_set_cell_value_fmt(m_obj, row, col, fmt, args...);
        return *this;
    }

    /// Get cell value
    [[nodiscard]] const char* cell_value(uint32_t row, uint32_t col) const noexcept {
        return lv_table_get_cell_value(m_obj, row, col);
    }

    // ==================== Column Width ====================

    /// Set column width
    Table& column_width(uint32_t col, int32_t w) noexcept {
        lv_table_set_column_width(m_obj, col, w);
        return *this;
    }

    /// Get column width
    [[nodiscard]] int32_t column_width(uint32_t col) const noexcept {
        return lv_table_get_column_width(m_obj, col);
    }

    // ==================== Cell Control ====================

    /// Set cell control flags
    Table& set_cell_ctrl(uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl) noexcept {
        lv_table_set_cell_ctrl(m_obj, row, col, ctrl);
        return *this;
    }

    /// Check if cell has control flag
    [[nodiscard]] bool has_cell_ctrl(uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl) const noexcept {
        return lv_table_has_cell_ctrl(m_obj, row, col, ctrl);
    }

    // ==================== Selection ====================

    /// Get selected cell
    void selected_cell(uint32_t* row, uint32_t* col) const noexcept {
        lv_table_get_selected_cell(m_obj, row, col);
    }

    // ==================== Size ====================

    /// Set size
    Table& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
