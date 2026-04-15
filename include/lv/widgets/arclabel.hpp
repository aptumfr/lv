#pragma once

/**
 * @file arclabel.hpp
 * @brief Zero-cost wrapper for LVGL Arc Label widget
 *
 * Requires LV_USE_ARCLABEL enabled in lv_conf.h
 */

#include <lvgl.h>
#include "../core/version.hpp"

#if LV_USE_ARCLABEL

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

#if LV_VERSION_AT_LEAST(9, 5, 0)
/**
 * @brief Arc label overflow mode (LVGL 9.5+)
 */
struct ArcLabelOverflow {
    static constexpr auto visible  = LV_ARCLABEL_OVERFLOW_VISIBLE;   ///< Show full text, may overflow
    static constexpr auto ellipsis = LV_ARCLABEL_OVERFLOW_ELLIPSIS;  ///< Show ... when text overflows
    static constexpr auto clip     = LV_ARCLABEL_OVERFLOW_CLIP;      ///< Clip text at arc boundary
};
#endif

/**
 * @brief Arc label direction
 */
struct ArcLabelDirection {
    static constexpr auto clockwise = LV_ARCLABEL_DIR_CLOCKWISE;
    static constexpr auto counter_clockwise = LV_ARCLABEL_DIR_COUNTER_CLOCKWISE;
};

/**
 * @brief Arc label text alignment
 */
struct ArcLabelAlign {
    static constexpr auto default_ = LV_ARCLABEL_TEXT_ALIGN_DEFAULT;
    static constexpr auto leading = LV_ARCLABEL_TEXT_ALIGN_LEADING;
    static constexpr auto center = LV_ARCLABEL_TEXT_ALIGN_CENTER;
    static constexpr auto trailing = LV_ARCLABEL_TEXT_ALIGN_TRAILING;
};

/**
 * @brief Arc Label widget wrapper
 *
 * Displays text along an arc/circular path. Useful for gauges and dials.
 *
 * Example:
 * @code
 *   lv::ArcLabel(parent)
 *       .text("SPEED")
 *       .radius(100)
 *       .angle_start(180)
 *       .angle_size(180)
 *       .direction(lv::ArcLabelDirection::clockwise)
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class ArcLabel : public ObjectView,
            public ObjectMixin<ArcLabel>,
                 public EventMixin<ArcLabel>,
                 public StyleMixin<ArcLabel> {
public:
    constexpr ArcLabel() noexcept : ObjectView(nullptr) {}
    constexpr ArcLabel(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_arclabel_class; }

    [[nodiscard]] static ArcLabel create(lv_obj_t* parent) {
        return ArcLabel(wrap, lv_arclabel_create(parent));
    }
    [[nodiscard]] static ArcLabel create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Text ====================

    /// Set text (LVGL copies the string)
    ArcLabel& text(const char* txt) noexcept {
        lv_arclabel_set_text(m_obj, txt);
        return *this;
    }

    /// Set formatted text
    template<typename... Args>
    ArcLabel& text_fmt(const char* fmt, Args... args) noexcept {
        lv_arclabel_set_text_fmt(m_obj, fmt, args...);
        return *this;
    }

    /// Set static text (string must remain valid)
    ArcLabel& text_static(const char* txt) noexcept {
        lv_arclabel_set_text_static(m_obj, txt);
        return *this;
    }

    /// Enable recoloring with #RRGGBB# syntax
    ArcLabel& recolor(bool enable = true) noexcept {
        lv_arclabel_set_recolor(m_obj, enable);
        return *this;
    }

    // ==================== Arc Configuration ====================

    /**
     * @brief Set arc radius
     * @param r Radius in pixels
     */
    ArcLabel& radius(uint32_t r) noexcept {
        lv_arclabel_set_radius(m_obj, r);
        return *this;
    }

    /**
     * @brief Set start angle (0 = right, 90 = bottom)
     * @param angle Start angle in degrees
     */
    ArcLabel& angle_start(lv_value_precise_t angle) noexcept {
        lv_arclabel_set_angle_start(m_obj, angle);
        return *this;
    }

    /**
     * @brief Set angle size (arc span)
     * @param size Angle size in degrees
     */
    ArcLabel& angle_size(lv_value_precise_t size) noexcept {
        lv_arclabel_set_angle_size(m_obj, size);
        return *this;
    }

    /**
     * @brief Set rotation offset
     * @param offset Rotation angle
     */
    ArcLabel& offset(int32_t offset) noexcept {
        lv_arclabel_set_offset(m_obj, offset);
        return *this;
    }

    /**
     * @brief Set arc direction
     * @param dir ArcLabelDirection::clockwise or counter_clockwise
     */
    ArcLabel& direction(lv_arclabel_dir_t dir) noexcept {
        lv_arclabel_set_dir(m_obj, dir);
        return *this;
    }

    /// Set clockwise direction
    ArcLabel& clockwise() noexcept {
        return direction(LV_ARCLABEL_DIR_CLOCKWISE);
    }

    /// Set counter-clockwise direction
    ArcLabel& counter_clockwise() noexcept {
        return direction(LV_ARCLABEL_DIR_COUNTER_CLOCKWISE);
    }

    // ==================== Center Offset ====================

    /// Set center X offset
    ArcLabel& center_offset_x(uint32_t x) noexcept {
        lv_arclabel_set_center_offset_x(m_obj, x);
        return *this;
    }

    /// Set center Y offset
    ArcLabel& center_offset_y(uint32_t y) noexcept {
        lv_arclabel_set_center_offset_y(m_obj, y);
        return *this;
    }

    /// Set center offset (both X and Y)
    ArcLabel& center_offset(uint32_t x, uint32_t y) noexcept {
        lv_arclabel_set_center_offset_x(m_obj, x);
        lv_arclabel_set_center_offset_y(m_obj, y);
        return *this;
    }

    // ==================== Alignment ====================

    /// Set vertical text alignment
    ArcLabel& vertical_align(lv_arclabel_text_align_t align) noexcept {
        lv_arclabel_set_text_vertical_align(m_obj, align);
        return *this;
    }

    /// Set horizontal text alignment
    ArcLabel& horizontal_align(lv_arclabel_text_align_t align) noexcept {
        lv_arclabel_set_text_horizontal_align(m_obj, align);
        return *this;
    }

    // ==================== Overflow (LVGL 9.5+) ====================

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Set overflow mode (LVGL 9.5+)
    ArcLabel& overflow(lv_arclabel_overflow_t mode) noexcept {
        lv_arclabel_set_overflow(m_obj, mode);
        return *this;
    }
#endif

    // ==================== Getters ====================

    [[nodiscard]] lv_value_precise_t get_angle_start() const noexcept {
        return lv_arclabel_get_angle_start(m_obj);
    }

    [[nodiscard]] lv_value_precise_t get_angle_size() const noexcept {
        return lv_arclabel_get_angle_size(m_obj);
    }

    [[nodiscard]] lv_arclabel_dir_t get_direction() const noexcept {
        return lv_arclabel_get_dir(m_obj);
    }

    [[nodiscard]] bool get_recolor() const noexcept {
        return lv_arclabel_get_recolor(m_obj);
    }

    [[nodiscard]] uint32_t get_radius() const noexcept {
        return lv_arclabel_get_radius(m_obj);
    }

    [[nodiscard]] uint32_t get_center_offset_x() const noexcept {
        return lv_arclabel_get_center_offset_x(m_obj);
    }

    [[nodiscard]] uint32_t get_center_offset_y() const noexcept {
        return lv_arclabel_get_center_offset_y(m_obj);
    }

    [[nodiscard]] lv_arclabel_text_align_t get_vertical_align() const noexcept {
        return lv_arclabel_get_text_vertical_align(m_obj);
    }

    [[nodiscard]] lv_arclabel_text_align_t get_horizontal_align() const noexcept {
        return lv_arclabel_get_text_horizontal_align(m_obj);
    }

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Get overflow mode (LVGL 9.5+)
    [[nodiscard]] lv_arclabel_overflow_t get_overflow() const noexcept {
        return lv_arclabel_get_overflow(m_obj);
    }

    /// Get rendered text angle in degrees (LVGL 9.5+)
    [[nodiscard]] lv_value_precise_t get_text_angle() const noexcept {
        return lv_arclabel_get_text_angle(m_obj);
    }
#endif
};

} // namespace lv

#endif // LV_USE_ARCLABEL
