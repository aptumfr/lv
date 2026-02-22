#pragma once

/**
 * @file calendar.hpp
 * @brief Zero-cost wrapper for LVGL calendar widget
 */

#include <lvgl.h>
#include <span>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Calendar widget wrapper
 *
 * A calendar for date selection.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Calendar : public ObjectView,
            public ObjectMixin<Calendar>,
                 public EventMixin<Calendar>,
                 public StyleMixin<Calendar> {
public:
    constexpr Calendar() noexcept : ObjectView(nullptr) {}

    constexpr Calendar(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    [[nodiscard]] static Calendar create(lv_obj_t* parent) {
        return Calendar(wrap, lv_calendar_create(parent));
    }

    [[nodiscard]] static Calendar create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Date ====================

    /// Set currently shown date (month/year view)
    Calendar& shown_date(uint32_t year, uint32_t month) noexcept {
        lv_calendar_set_showed_date(m_obj, year, month);
        return *this;
    }

    /// Get shown date
    [[nodiscard]] lv_calendar_date_t shown_date() const noexcept {
        return *lv_calendar_get_showed_date(m_obj);
    }

    /// Set today's date (highlighted)
    Calendar& today_date(uint32_t year, uint32_t month, uint32_t day) noexcept {
        lv_calendar_set_today_date(m_obj, year, month, day);
        return *this;
    }

    /// Get today's date
    [[nodiscard]] lv_calendar_date_t today_date() const noexcept {
        return *lv_calendar_get_today_date(m_obj);
    }

    /// Get pressed date
    [[nodiscard]] lv_result_t pressed_date(lv_calendar_date_t* date) const noexcept {
        return lv_calendar_get_pressed_date(m_obj, date);
    }

    // ==================== Highlighted Days ====================

    /// Set highlighted dates (array must remain valid)
    Calendar& highlighted_dates(lv_calendar_date_t dates[], size_t count) noexcept {
        lv_calendar_set_highlighted_dates(m_obj, dates, count);
        return *this;
    }

    /// Set highlighted dates from span (array must remain valid)
    Calendar& highlighted_dates(std::span<lv_calendar_date_t> dates) noexcept {
        return highlighted_dates(dates.data(), dates.size());
    }

    /// Get highlighted dates
    [[nodiscard]] const lv_calendar_date_t* highlighted_dates() const noexcept {
        return lv_calendar_get_highlighted_dates(m_obj);
    }

    /// Get highlighted dates count
    [[nodiscard]] size_t highlighted_dates_count() const noexcept {
        return lv_calendar_get_highlighted_dates_num(m_obj);
    }

    // ==================== Day Names ====================

    /// Set custom day names (array of 7 strings, must remain valid)
    Calendar& day_names(const char* names[]) noexcept {
        lv_calendar_set_day_names(m_obj, names);
        return *this;
    }

    // ==================== Header ====================

    /// Add header with month/year and navigation arrows
    ObjectView header_arrow() noexcept {
        return ObjectView(lv_calendar_header_arrow_create(m_obj));
    }

    /// Add dropdown header for month/year selection
    ObjectView header_dropdown() noexcept {
        return ObjectView(lv_calendar_header_dropdown_create(m_obj));
    }

    // ==================== Size ====================

    /// Set size
    Calendar& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

namespace detail {
    template<> inline const lv_obj_class_t*
    widget_lv_class<Calendar>() noexcept { return &lv_calendar_class; }
}

} // namespace lv
