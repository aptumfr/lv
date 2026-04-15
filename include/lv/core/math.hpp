#pragma once

/**
 * @file math.hpp
 * @brief Zero-cost wrappers for LVGL's fixed-point math helpers.
 *
 * These are the same helpers LVGL uses internally. Prefer them over
 * `<cmath>` on embedded targets where the standard library is heavy or
 * unavailable.
 */

#include <lvgl.h>
#include <cstdint>

namespace lv::math {

// ==================== Constants ====================

/// Scale factor for `trigo_sin`/`trigo_cos`. Output is in the range
/// `[-LV_TRIGO_SIN_MAX, LV_TRIGO_SIN_MAX]`. Shift by `trigo_shift` to
/// multiply a value by cos/sin without overflowing.
inline constexpr int32_t trigo_sin_max = LV_TRIGO_SIN_MAX;
inline constexpr int32_t trigo_shift   = LV_TRIGO_SHIFT;

/// Max input value for `bezier3` (time scaled to `[0, bezier_val_max]`).
inline constexpr int32_t bezier_val_max = LV_BEZIER_VAL_MAX;

// ==================== Helpers ====================

/// Linear map: map @p x from `[min_in, max_in]` onto `[min_out, max_out]`.
[[nodiscard]] inline int32_t map(int32_t x,
                                 int32_t min_in, int32_t max_in,
                                 int32_t min_out, int32_t max_out) noexcept {
    return lv_map(x, min_in, max_in, min_out, max_out);
}

/// Integer power (`base^exp`).
[[nodiscard]] inline int64_t pow(int64_t base, int8_t exp) noexcept {
    return lv_pow(base, exp);
}

/// Uniform random in `[min, max]` (inclusive).
[[nodiscard]] inline uint32_t rand(uint32_t min, uint32_t max) noexcept {
    return lv_rand(min, max);
}

/// Seed LVGL's RNG.
inline void rand_seed(uint32_t seed) noexcept {
    lv_rand_set_seed(seed);
}

/// Fast sine on degrees `[-32767, 32767]`. Result is scaled by
/// `trigo_sin_max`, so divide by `trigo_sin_max` for `[-1, 1]`.
[[nodiscard]] inline int32_t trigo_sin(int16_t angle_deg) noexcept {
    return lv_trigo_sin(angle_deg);
}

/// Fast cosine on degrees `[-32767, 32767]`. Result is scaled by
/// `trigo_sin_max`.
[[nodiscard]] inline int32_t trigo_cos(int16_t angle_deg) noexcept {
    return lv_trigo_cos(angle_deg);
}

/// Cubic Bezier interpolation. @p t in `[0, bezier_val_max]`.
[[nodiscard]] inline int32_t bezier3(int32_t t,
                                     int32_t u0, uint32_t u1,
                                     int32_t u2, int32_t u3) noexcept {
    return lv_bezier3(t, u0, u1, u2, u3);
}

} // namespace lv::math
