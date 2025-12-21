#pragma once

/**
 * @file primitives.hpp
 * @brief Helper functions for LVGL geometric primitives
 */

#include <lvgl.h>

namespace lv {

/**
 * @brief Create an lv_area_t from coordinates
 *
 * Example:
 * ```cpp
 * auto area = lv::area(10, 10, 100, 100);  // x1, y1, x2, y2
 * ```
 */
[[nodiscard]] inline constexpr lv_area_t area(int32_t x1, int32_t y1, int32_t x2, int32_t y2) noexcept {
    return lv_area_t{x1, y1, x2, y2};
}

/**
 * @brief Create an lv_area_t from position and size
 *
 * Example:
 * ```cpp
 * auto area = lv::area_from_size(10, 10, 90, 90);  // x, y, width, height
 * ```
 */
[[nodiscard]] inline constexpr lv_area_t area_from_size(int32_t x, int32_t y, int32_t w, int32_t h) noexcept {
    return lv_area_t{x, y, x + w - 1, y + h - 1};
}

/**
 * @brief Create an lv_point_t
 *
 * Example:
 * ```cpp
 * auto pt = lv::point(50, 100);
 * ```
 */
[[nodiscard]] inline constexpr lv_point_t point(int32_t x, int32_t y) noexcept {
    return lv_point_t{x, y};
}

/**
 * @brief Create an lv_point_precise_t (for line endpoints, etc.)
 *
 * Example:
 * ```cpp
 * auto pt = lv::point_precise(50.5, 100.25);
 * ```
 */
[[nodiscard]] inline constexpr lv_point_precise_t point_precise(lv_value_precise_t x, lv_value_precise_t y) noexcept {
    return lv_point_precise_t{x, y};
}

/**
 * @brief Get area width
 */
[[nodiscard]] inline constexpr int32_t area_width(const lv_area_t& a) noexcept {
    return a.x2 - a.x1 + 1;
}

/**
 * @brief Get area height
 */
[[nodiscard]] inline constexpr int32_t area_height(const lv_area_t& a) noexcept {
    return a.y2 - a.y1 + 1;
}

/**
 * @brief Check if point is inside area
 */
[[nodiscard]] inline constexpr bool area_contains(const lv_area_t& a, int32_t x, int32_t y) noexcept {
    return x >= a.x1 && x <= a.x2 && y >= a.y1 && y <= a.y2;
}

} // namespace lv
