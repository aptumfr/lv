#pragma once

/**
 * @file chart.hpp
 * @brief Zero-cost wrapper for LVGL chart widget
 */

#include <lvgl.h>
#include "../core/version.hpp"
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Chart widget wrapper
 *
 * A chart for displaying data series (line, bar, scatter).
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Chart : public ObjectView,
            public ObjectMixin<Chart>,
              public EventMixin<Chart>,
              public StyleMixin<Chart> {
public:
    // ==================== Nested Types ====================

    /// Chart type constants
    struct Type {
        static constexpr auto none = LV_CHART_TYPE_NONE;
        static constexpr auto line = LV_CHART_TYPE_LINE;
        static constexpr auto bar = LV_CHART_TYPE_BAR;
        static constexpr auto scatter = LV_CHART_TYPE_SCATTER;
#if LV_VERSION_AT_LEAST(9, 5, 0)
        static constexpr auto curve = LV_CHART_TYPE_CURVE;  ///< @since LVGL 9.5
#endif
    };

    /// Chart axis constants
    struct Axis {
        static constexpr auto primary_y = LV_CHART_AXIS_PRIMARY_Y;
        static constexpr auto secondary_y = LV_CHART_AXIS_SECONDARY_Y;
        static constexpr auto primary_x = LV_CHART_AXIS_PRIMARY_X;
        static constexpr auto secondary_x = LV_CHART_AXIS_SECONDARY_X;
    };

    /// Chart update mode constants
    struct UpdateMode {
        static constexpr auto shift = LV_CHART_UPDATE_MODE_SHIFT;
        static constexpr auto circular = LV_CHART_UPDATE_MODE_CIRCULAR;
    };

    // ==================== Constructors ====================

    constexpr Chart() noexcept : ObjectView(nullptr) {}
    constexpr Chart(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_chart_class; }

    [[nodiscard]] static Chart create(lv_obj_t* parent) {
        return Chart(wrap, lv_chart_create(parent));
    }
    [[nodiscard]] static Chart create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Type ====================

    /// Set chart type
    Chart& type(lv_chart_type_t t) noexcept {
        lv_chart_set_type(m_obj, t);
        return *this;
    }

    /// Line chart
    Chart& type_line() noexcept {
        return type(LV_CHART_TYPE_LINE);
    }

    /// Bar chart
    Chart& type_bar() noexcept {
        return type(LV_CHART_TYPE_BAR);
    }

    /// Scatter chart
    Chart& type_scatter() noexcept {
        return type(LV_CHART_TYPE_SCATTER);
    }

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Bézier curve chart (LVGL 9.5+, requires LV_USE_VECTOR_GRAPHIC)
    Chart& type_curve() noexcept {
        return type(LV_CHART_TYPE_CURVE);
    }
#endif

    /// Get chart type
    [[nodiscard]] lv_chart_type_t get_type() const noexcept {
        return lv_chart_get_type(m_obj);
    }

    // ==================== Data ====================

    /// Set point count
    Chart& point_count(uint32_t count) noexcept {
        lv_chart_set_point_count(m_obj, count);
        return *this;
    }

    /// Get point count
    [[nodiscard]] uint32_t point_count() const noexcept {
        return lv_chart_get_point_count(m_obj);
    }

    /// Add data series
    [[nodiscard]] lv_chart_series_t* add_series(lv_color_t color, lv_chart_axis_t axis = LV_CHART_AXIS_PRIMARY_Y) noexcept {
        return lv_chart_add_series(m_obj, color, axis);
    }

    /// Remove data series
    Chart& remove_series(lv_chart_series_t* series) noexcept {
        lv_chart_remove_series(m_obj, series);
        return *this;
    }

    /// Set next value in series
    Chart& set_next_value(lv_chart_series_t* series, int32_t value) noexcept {
        lv_chart_set_next_value(m_obj, series, value);
        return *this;
    }

    /// Set value at index
    Chart& set_value_by_id(lv_chart_series_t* series, uint32_t id, int32_t value) noexcept {
        lv_chart_set_value_by_id(m_obj, series, id, value);
        return *this;
    }

    /// Set all values in series from array
    Chart& set_ext_y_array(lv_chart_series_t* series, int32_t array[]) noexcept {
        lv_chart_set_ext_y_array(m_obj, series, array);
        return *this;
    }

    /// Get Y array of series
    [[nodiscard]] int32_t* get_y_array(lv_chart_series_t* series) noexcept {
        return lv_chart_get_y_array(m_obj, series);
    }

    /// Get Y array of series (alternate naming)
    [[nodiscard]] int32_t* get_series_y_array(lv_chart_series_t* ser) noexcept {
        return lv_chart_get_series_y_array(m_obj, ser);
    }

    /// Get X array of series (for scatter charts)
    [[nodiscard]] int32_t* get_series_x_array(lv_chart_series_t* ser) noexcept {
        return lv_chart_get_series_x_array(m_obj, ser);
    }

    /// Get series color
    [[nodiscard]] lv_color_t get_series_color(const lv_chart_series_t* series) noexcept {
        return lv_chart_get_series_color(m_obj, series);
    }

    /// Get next series (pass nullptr to get first)
    [[nodiscard]] lv_chart_series_t* get_series_next(lv_chart_series_t* prev = nullptr) noexcept {
        return lv_chart_get_series_next(m_obj, prev);
    }

    /// Get first series (convenience)
    [[nodiscard]] lv_chart_series_t* get_first_series() noexcept {
        return lv_chart_get_series_next(m_obj, nullptr);
    }

    // ==================== Range ====================

    /// Set axis range (alias for set_axis_range)
    Chart& range(lv_chart_axis_t axis, int32_t min, int32_t max) noexcept {
        lv_chart_set_range(m_obj, axis, min, max);
        return *this;
    }

    /// Set axis range
    Chart& axis_range(lv_chart_axis_t axis, int32_t min, int32_t max) noexcept {
        lv_chart_set_axis_range(m_obj, axis, min, max);
        return *this;
    }

    /// Set axis minimum value
    Chart& axis_min_value(lv_chart_axis_t axis, int32_t min) noexcept {
        lv_chart_set_axis_min_value(m_obj, axis, min);
        return *this;
    }

    /// Set axis maximum value
    Chart& axis_max_value(lv_chart_axis_t axis, int32_t max) noexcept {
        lv_chart_set_axis_max_value(m_obj, axis, max);
        return *this;
    }

    /// Set primary Y range
    Chart& range_y(int32_t min, int32_t max) noexcept {
        return range(LV_CHART_AXIS_PRIMARY_Y, min, max);
    }

    /// Set secondary Y range
    Chart& range_y2(int32_t min, int32_t max) noexcept {
        return range(LV_CHART_AXIS_SECONDARY_Y, min, max);
    }

    // ==================== Appearance ====================

    /// Set update mode
    Chart& update_mode(lv_chart_update_mode_t mode) noexcept {
        lv_chart_set_update_mode(m_obj, mode);
        return *this;
    }

    /// Circular update mode
    Chart& update_circular() noexcept {
        return update_mode(LV_CHART_UPDATE_MODE_CIRCULAR);
    }

    /// Shift update mode
    Chart& update_shift() noexcept {
        return update_mode(LV_CHART_UPDATE_MODE_SHIFT);
    }

    /// Get update mode
    [[nodiscard]] lv_chart_update_mode_t get_update_mode() const noexcept {
        return lv_chart_get_update_mode(m_obj);
    }

    /// Set division lines
    Chart& div_lines(uint8_t hdiv, uint8_t vdiv) noexcept {
        lv_chart_set_div_line_count(m_obj, hdiv, vdiv);
        return *this;
    }

    /// Set horizontal division line count
    Chart& hor_div_lines(uint32_t cnt) noexcept {
        lv_chart_set_hor_div_line_count(m_obj, cnt);
        return *this;
    }

    /// Get horizontal division line count
    [[nodiscard]] uint32_t hor_div_lines() const noexcept {
        return lv_chart_get_hor_div_line_count(m_obj);
    }

    /// Set vertical division line count
    Chart& ver_div_lines(uint32_t cnt) noexcept {
        lv_chart_set_ver_div_line_count(m_obj, cnt);
        return *this;
    }

    /// Get vertical division line count
    [[nodiscard]] uint32_t ver_div_lines() const noexcept {
        return lv_chart_get_ver_div_line_count(m_obj);
    }

    /// Hide a series
    Chart& hide_series(lv_chart_series_t* series, bool hide = true) noexcept {
        lv_chart_hide_series(m_obj, series, hide);
        return *this;
    }

    /// Set series color
    Chart& series_color(lv_chart_series_t* series, lv_color_t color) noexcept {
        lv_chart_set_series_color(m_obj, series, color);
        return *this;
    }

    /// Set X start point
    Chart& x_start_point(lv_chart_series_t* ser, uint32_t id) noexcept {
        lv_chart_set_x_start_point(m_obj, ser, id);
        return *this;
    }

    /// Get X start point
    [[nodiscard]] uint32_t x_start_point(lv_chart_series_t* ser) const noexcept {
        return lv_chart_get_x_start_point(m_obj, ser);
    }

    /// Get point position by ID
    void point_pos_by_id(lv_chart_series_t* ser, uint32_t id, lv_point_t* p_out) noexcept {
        lv_chart_get_point_pos_by_id(m_obj, ser, id, p_out);
    }

    /// Get first point center offset
    [[nodiscard]] int32_t first_point_center_offset() noexcept {
        return lv_chart_get_first_point_center_offset(m_obj);
    }

    /// Get pressed point index
    [[nodiscard]] uint32_t pressed_point() const noexcept {
        return lv_chart_get_pressed_point(m_obj);
    }

    /// Refresh chart
    Chart& refresh() noexcept {
        lv_chart_refresh(m_obj);
        return *this;
    }

    // ==================== Cursor ====================

    /// Add cursor
    [[nodiscard]] lv_chart_cursor_t* add_cursor(lv_color_t color, lv_dir_t dir) noexcept {
        return lv_chart_add_cursor(m_obj, color, dir);
    }

    /// Set cursor position
    Chart& set_cursor_pos(lv_chart_cursor_t* cursor, lv_point_t* pos) noexcept {
        lv_chart_set_cursor_pos(m_obj, cursor, pos);
        return *this;
    }

    /// Set cursor to point
    Chart& set_cursor_point(lv_chart_cursor_t* cursor, lv_chart_series_t* series, uint32_t point_id) noexcept {
        lv_chart_set_cursor_point(m_obj, cursor, series, point_id);
        return *this;
    }

    /// Set cursor X position
    Chart& cursor_pos_x(lv_chart_cursor_t* cursor, int32_t x) noexcept {
        lv_chart_set_cursor_pos_x(m_obj, cursor, x);
        return *this;
    }

    /// Set cursor Y position
    Chart& cursor_pos_y(lv_chart_cursor_t* cursor, int32_t y) noexcept {
        lv_chart_set_cursor_pos_y(m_obj, cursor, y);
        return *this;
    }

    /// Remove cursor
    Chart& remove_cursor(lv_chart_cursor_t* cursor) noexcept {
        lv_chart_remove_cursor(m_obj, cursor);
        return *this;
    }

    // ==================== Series Values ====================

    /// Set all values in a series
    Chart& set_all_values(lv_chart_series_t* ser, int32_t value) noexcept {
        lv_chart_set_all_values(m_obj, ser, value);
        return *this;
    }

    /// Set series values from array
    Chart& set_series_values(lv_chart_series_t* ser, const int32_t values[], size_t values_cnt) noexcept {
        lv_chart_set_series_values(m_obj, ser, values, values_cnt);
        return *this;
    }

    /// Set series value by ID
    Chart& set_series_value_by_id(lv_chart_series_t* ser, uint32_t id, int32_t value) noexcept {
        lv_chart_set_series_value_by_id(m_obj, ser, id, value);
        return *this;
    }

    /// Set external Y array for series
    Chart& set_series_ext_y_array(lv_chart_series_t* ser, int32_t array[]) noexcept {
        lv_chart_set_series_ext_y_array(m_obj, ser, array);
        return *this;
    }

    /// Set external X array for series
    Chart& set_series_ext_x_array(lv_chart_series_t* ser, int32_t array[]) noexcept {
        lv_chart_set_series_ext_x_array(m_obj, ser, array);
        return *this;
    }

    /// Set next value for scatter chart (x and y)
    Chart& set_next_value2(lv_chart_series_t* ser, int32_t x_value, int32_t y_value) noexcept {
        lv_chart_set_next_value2(m_obj, ser, x_value, y_value);
        return *this;
    }

    /// Set series value by ID for scatter chart (x and y)
    Chart& set_series_value_by_id2(lv_chart_series_t* ser, uint32_t id, int32_t x_value, int32_t y_value) noexcept {
        lv_chart_set_series_value_by_id2(m_obj, ser, id, x_value, y_value);
        return *this;
    }

    /// Set series values for scatter chart (x and y arrays)
    Chart& set_series_values2(lv_chart_series_t* ser, const int32_t x_values[], const int32_t y_values[], size_t values_cnt) noexcept {
        lv_chart_set_series_values2(m_obj, ser, x_values, y_values, values_cnt);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Chart& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv
