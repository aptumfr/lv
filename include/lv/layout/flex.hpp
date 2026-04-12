#pragma once

/**
 * @file flex.hpp
 * @brief Zero-cost flex layout containers (hbox/vbox DSL)
 *
 * Maps C++ layout DSL to LVGL's flex layout system.
 * No overhead - directly calls LVGL flex APIs.
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Flex container wrapper
 *
 * Provides a fluent API for flex layout containers.
 * Use hbox() and vbox() factory functions to create.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Flex : public ObjectView,
             public ObjectMixin<Flex>,
             public EventMixin<Flex>,
             public StyleMixin<Flex> {
public:
    // Flex declares its own align() for flex-content alignment, which
    // would otherwise hide ObjectMixin<Flex>::align() for object
    // positioning (C++ name hiding applies to all base overloads).
    // Re-expose the object-positioning overloads so users can still
    // call e.g. hbox(parent).align(LV_ALIGN_TOP_MID, 0, 10).
    // Overload resolution distinguishes the two by argument type
    // (lv_align_t vs lv_flex_align_t).
    using ObjectMixin<Flex>::align;

    /// Create a flex container with specified flow
    explicit Flex(ObjectView parent, lv_flex_flow_t flow = LV_FLEX_FLOW_ROW)
        : ObjectView(lv_obj_create(parent)) {
        lv_obj_set_layout(m_obj, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(m_obj, flow);
        // Default: size to content, no scrolling
        lv_obj_set_size(m_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    /// Wrap existing object as flex container (for wrapping raw pointers)
    Flex(wrap_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    // ==================== Flow Direction ====================

    /// Set flex flow direction
    Flex& flow(lv_flex_flow_t f) noexcept {
        lv_obj_set_flex_flow(m_obj, f);
        return *this;
    }

    /// Row direction (horizontal)
    Flex& row() noexcept {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_ROW);
        return *this;
    }

    /// Column direction (vertical)
    Flex& column() noexcept {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN);
        return *this;
    }

    /// Row with wrap
    Flex& row_wrap() noexcept {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_ROW_WRAP);
        return *this;
    }

    /// Column with wrap
    Flex& column_wrap() noexcept {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN_WRAP);
        return *this;
    }

    /// Reverse row direction
    Flex& row_reverse() noexcept {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_ROW_REVERSE);
        return *this;
    }

    /// Reverse column direction
    Flex& column_reverse() noexcept {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN_REVERSE);
        return *this;
    }

    // ==================== Alignment ====================

    /// Set main axis alignment (justify-content)
    Flex& justify(lv_flex_align_t align) noexcept {
        lv_obj_set_flex_align(m_obj, align,
            lv_obj_get_style_flex_cross_place(m_obj, LV_PART_MAIN),
            lv_obj_get_style_flex_track_place(m_obj, LV_PART_MAIN));
        return *this;
    }

    /// Set cross axis alignment (align-items)
    Flex& align_items(lv_flex_align_t align) noexcept {
        lv_obj_set_flex_align(m_obj,
            lv_obj_get_style_flex_main_place(m_obj, LV_PART_MAIN),
            align,
            lv_obj_get_style_flex_track_place(m_obj, LV_PART_MAIN));
        return *this;
    }

    /// Set track alignment (align-content for wrapped)
    Flex& align_content(lv_flex_align_t align) noexcept {
        lv_obj_set_flex_align(m_obj,
            lv_obj_get_style_flex_main_place(m_obj, LV_PART_MAIN),
            lv_obj_get_style_flex_cross_place(m_obj, LV_PART_MAIN),
            align);
        return *this;
    }

    /// Set all alignments at once
    Flex& align(lv_flex_align_t main, lv_flex_align_t cross,
                lv_flex_align_t track = LV_FLEX_ALIGN_START) noexcept {
        lv_obj_set_flex_align(m_obj, main, cross, track);
        return *this;
    }

    /// Center children on both axes
    Flex& center_content() noexcept {
        lv_obj_set_flex_align(m_obj, LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        return *this;
    }

    /// Space children evenly
    Flex& space_evenly() noexcept {
        lv_obj_set_flex_align(m_obj, LV_FLEX_ALIGN_SPACE_EVENLY,
            lv_obj_get_style_flex_cross_place(m_obj, LV_PART_MAIN),
            lv_obj_get_style_flex_track_place(m_obj, LV_PART_MAIN));
        return *this;
    }

    /// Space children with equal gaps between
    Flex& space_between() noexcept {
        lv_obj_set_flex_align(m_obj, LV_FLEX_ALIGN_SPACE_BETWEEN,
            lv_obj_get_style_flex_cross_place(m_obj, LV_PART_MAIN),
            lv_obj_get_style_flex_track_place(m_obj, LV_PART_MAIN));
        return *this;
    }

    /// Space children with equal space around
    Flex& space_around() noexcept {
        lv_obj_set_flex_align(m_obj, LV_FLEX_ALIGN_SPACE_AROUND,
            lv_obj_get_style_flex_cross_place(m_obj, LV_PART_MAIN),
            lv_obj_get_style_flex_track_place(m_obj, LV_PART_MAIN));
        return *this;
    }

    // ==================== Gap / Spacing ====================

    /// Set gap between children (both row and column)
    Flex& gap(int32_t g) noexcept {
        lv_obj_set_style_pad_row(m_obj, g, 0);
        lv_obj_set_style_pad_column(m_obj, g, 0);
        return *this;
    }

    /// Set row gap only
    Flex& row_gap(int32_t g) noexcept {
        lv_obj_set_style_pad_row(m_obj, g, 0);
        return *this;
    }

    /// Set column gap only
    Flex& column_gap(int32_t g) noexcept {
        lv_obj_set_style_pad_column(m_obj, g, 0);
        return *this;
    }

    // size(), width(), height(), fill(), scrollable(), and grow() are
    // inherited from ObjectMixin<Flex>.
};


// ==================== Factory Functions ====================

/**
 * @brief Create a horizontal flex container (row)
 *
 * @code
 * auto row = lv::hbox(parent)
 *     .gap(10)
 *     .padding(8);
 *
 * lv::Button::create(row).text("A");
 * lv::Button::create(row).text("B");
 * @endcode
 */
[[nodiscard]] inline Flex hbox(ObjectView parent) {
    return Flex(parent, LV_FLEX_FLOW_ROW);
}

/**
 * @brief Create a vertical flex container (column)
 *
 * @code
 * auto col = lv::vbox(parent)
 *     .gap(10)
 *     .padding(8);
 *
 * lv::Label::create(col).text("Title");
 * lv::Button::create(col).text("Click");
 * @endcode
 */
[[nodiscard]] inline Flex vbox(ObjectView parent) {
    return Flex(parent, LV_FLEX_FLOW_COLUMN);
}

/**
 * @brief Create a horizontal flex container with wrap
 */
[[nodiscard]] inline Flex hbox_wrap(ObjectView parent) {
    return Flex(parent, LV_FLEX_FLOW_ROW_WRAP);
}

/**
 * @brief Create a vertical flex container with wrap
 */
[[nodiscard]] inline Flex vbox_wrap(ObjectView parent) {
    return Flex(parent, LV_FLEX_FLOW_COLUMN_WRAP);
}

} // namespace lv
