#pragma once

/**
 * @file transition.hpp
 * @brief Owning wrapper for LVGL style transition descriptors
 *
 * Style transitions smoothly animate style property changes when an object
 * enters or leaves a state (e.g. pressed, focused). A transition descriptor
 * names the properties to animate, the duration, delay, and easing curve.
 *
 * LVGL stores two pointers that must outlive the target Style:
 *   - The `props[]` array (zero-terminated list of lv_style_prop_t).
 *   - The lv_style_transition_dsc_t itself.
 *
 * TransitionDsc owns both — the props array is embedded, and the dsc is a
 * member. The class is non-copyable AND non-movable so the pointers LVGL
 * stores remain valid for the lifetime of the object.
 *
 * Usage:
 * @code
 *   // Animate bg_color and shadow_width over 300ms when pressed state toggles.
 *   static const lv::TransitionDsc hover_trans(
 *       { LV_STYLE_BG_COLOR, LV_STYLE_SHADOW_WIDTH },
 *       300);
 *
 *   style_pressed.transition(hover_trans)
 *                .bg_color(lv::rgb(0x202020));
 * @endcode
 */

#include <lvgl.h>
#include <cstddef>
#include <cstdint>

namespace lv {

/**
 * @brief Owning LVGL style transition descriptor.
 *
 * @tparam N Number of style properties animated. Deduced from the init list.
 *
 * @note The props array is stored inline and zero-terminated. LVGL requires
 *       the terminator; TransitionDsc adds it automatically (`N + 1` slots).
 *
 * @warning Non-copyable and non-movable. LVGL keeps a pointer to
 *          `&m_dsc` (via `lv_style_set_transition`) and the dsc keeps a
 *          pointer to `m_props`. Moving would invalidate both.
 *          Keep instances static, member, or otherwise pointer-stable.
 */
template<std::size_t N>
class TransitionDsc {
    static_assert(N > 0, "TransitionDsc must animate at least one property");

    lv_style_prop_t m_props[N + 1];      // + 1 for the required 0 terminator
    lv_style_transition_dsc_t m_dsc{};

public:
    /// Construct from a braced list of style properties.
    /// @param props       Array of properties to animate.
    /// @param duration_ms Animation duration in milliseconds.
    /// @param delay_ms    Delay before the animation starts.
    /// @param path_cb     Easing curve (defaults to LVGL's ease-in-out).
    /// @param user_data   Opaque pointer forwarded to the path callback.
    TransitionDsc(const lv_style_prop_t (&props)[N],
                  uint32_t duration_ms,
                  uint32_t delay_ms = 0,
                  lv_anim_path_cb_t path_cb = lv_anim_path_ease_in_out,
                  void* user_data = nullptr) noexcept {
        for (std::size_t i = 0; i < N; ++i) m_props[i] = props[i];
        m_props[N] = static_cast<lv_style_prop_t>(0);
        lv_style_transition_dsc_init(&m_dsc, m_props, path_cb,
                                     duration_ms, delay_ms, user_data);
    }

    /// Get the underlying descriptor pointer.
    [[nodiscard]] const lv_style_transition_dsc_t* get() const noexcept {
        return &m_dsc;
    }

    // Non-copyable, non-movable — see @warning above.
    TransitionDsc(const TransitionDsc&) = delete;
    TransitionDsc& operator=(const TransitionDsc&) = delete;
    TransitionDsc(TransitionDsc&&) = delete;
    TransitionDsc& operator=(TransitionDsc&&) = delete;
};

// CTAD: lv::TransitionDsc{{LV_STYLE_BG_COLOR, LV_STYLE_RADIUS}, 300} deduces N=2
template<std::size_t N>
TransitionDsc(const lv_style_prop_t (&)[N], uint32_t) -> TransitionDsc<N>;

template<std::size_t N>
TransitionDsc(const lv_style_prop_t (&)[N], uint32_t, uint32_t) -> TransitionDsc<N>;

template<std::size_t N>
TransitionDsc(const lv_style_prop_t (&)[N], uint32_t, uint32_t,
              lv_anim_path_cb_t) -> TransitionDsc<N>;

template<std::size_t N>
TransitionDsc(const lv_style_prop_t (&)[N], uint32_t, uint32_t,
              lv_anim_path_cb_t, void*) -> TransitionDsc<N>;

} // namespace lv
