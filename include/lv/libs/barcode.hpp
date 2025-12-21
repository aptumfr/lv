#pragma once

/**
 * @file barcode.hpp
 * @brief Zero-cost wrapper for LVGL Barcode widget
 *
 * Requires LV_USE_BARCODE enabled in lv_conf.h
 */

#include <lvgl.h>

#if LV_USE_BARCODE

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Barcode encoding types
 */
struct BarcodeEncoding {
    static constexpr auto code128_gs1 = LV_BARCODE_ENCODING_CODE128_GS1;
    static constexpr auto code128_raw = LV_BARCODE_ENCODING_CODE128_RAW;
};

/**
 * @brief Barcode widget wrapper
 *
 * Generates and displays Code 128 barcodes.
 *
 * Example:
 * @code
 *   lv::Barcode(parent)
 *       .scale(2)
 *       .dark_color(lv::colors::black())
 *       .light_color(lv::colors::white())
 *       .data("12345678")
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Barcode : public ObjectView,
                public EventMixin<Barcode>,
                public StyleMixin<Barcode> {
public:
    /// Default constructor (null widget)
    Barcode() noexcept : ObjectView(nullptr) {}

    /// Create a barcode widget as child of parent
    explicit Barcode(ObjectView parent)
        : ObjectView(lv_barcode_create(parent)) {}

    /// Wrap existing barcode object
    struct wrap_t {};
    static constexpr wrap_t wrap{};

    Barcode(wrap_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    // ==================== Configuration ====================

    /**
     * @brief Set dark color (the bars)
     * @param color Dark color
     */
    Barcode& dark_color(lv_color_t color) noexcept {
        lv_barcode_set_dark_color(m_obj, color);
        return *this;
    }

    /**
     * @brief Set light color (the background/spaces)
     * @param color Light color
     */
    Barcode& light_color(lv_color_t color) noexcept {
        lv_barcode_set_light_color(m_obj, color);
        return *this;
    }

    /**
     * @brief Set scale factor
     * @param scale Scale factor (1 = minimum width)
     */
    Barcode& scale(uint16_t scale) noexcept {
        lv_barcode_set_scale(m_obj, scale);
        return *this;
    }

    /**
     * @brief Set barcode direction
     * @param dir LV_DIR_HOR (horizontal) or LV_DIR_VER (vertical)
     */
    Barcode& direction(lv_dir_t dir) noexcept {
        lv_barcode_set_direction(m_obj, dir);
        return *this;
    }

    /// Set horizontal direction
    Barcode& horizontal() noexcept {
        return direction(LV_DIR_HOR);
    }

    /// Set vertical direction
    Barcode& vertical() noexcept {
        return direction(LV_DIR_VER);
    }

    /**
     * @brief Set tiled mode
     * @param tiled true for tiled mode
     */
    Barcode& tiled(bool tiled) noexcept {
        lv_barcode_set_tiled(m_obj, tiled);
        return *this;
    }

    /**
     * @brief Set encoding type
     * @param encoding BarcodeEncoding::code128_gs1 or BarcodeEncoding::code128_raw
     */
    Barcode& encoding(lv_barcode_encoding_t encoding) noexcept {
        lv_barcode_set_encoding(m_obj, encoding);
        return *this;
    }

    // ==================== Data ====================

    /**
     * @brief Set barcode data
     * @param text Text to encode
     * @return true if successful
     */
    bool data(const char* text) noexcept {
        return lv_barcode_update(m_obj, text) == LV_RESULT_OK;
    }

    // ==================== Getters ====================

    /// Get dark color
    [[nodiscard]] lv_color_t get_dark_color() const noexcept {
        return lv_barcode_get_dark_color(m_obj);
    }

    /// Get light color
    [[nodiscard]] lv_color_t get_light_color() const noexcept {
        return lv_barcode_get_light_color(m_obj);
    }

    /// Get scale factor
    [[nodiscard]] uint16_t get_scale() const noexcept {
        return lv_barcode_get_scale(m_obj);
    }

    /// Get encoding type
    [[nodiscard]] lv_barcode_encoding_t get_encoding() const noexcept {
        return lv_barcode_get_encoding(m_obj);
    }
};

} // namespace lv

#endif // LV_USE_BARCODE
