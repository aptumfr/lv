#pragma once

/**
 * @file qrcode.hpp
 * @brief Zero-cost wrapper for LVGL QR code widget
 *
 * Requires LV_USE_QRCODE enabled in lv_conf.h
 */

#include <lvgl.h>

#if LV_USE_QRCODE

#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"
#include <string_view>

namespace lv {

/**
 * @brief QR Code widget wrapper
 *
 * Generates and displays QR codes from text or binary data.
 *
 * Example:
 * @code
 *   lv::QRCode(parent)
 *       .size(150)
 *       .dark_color(lv::colors::black())
 *       .light_color(lv::colors::white())
 *       .data("https://lvgl.io")
 *       .center();
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class QRCode : public ObjectView,
               public EventMixin<QRCode>,
               public StyleMixin<QRCode> {
public:
    /// Default constructor (null widget)
    QRCode() noexcept : ObjectView(nullptr) {}

    /// Create a QR code widget as child of parent
    explicit QRCode(ObjectView parent)
        : ObjectView(lv_qrcode_create(parent)) {}

    /// Wrap existing QR code object
    struct wrap_t {};
    static constexpr wrap_t wrap{};

    QRCode(wrap_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    // ==================== Configuration ====================

    /**
     * @brief Set QR code size (width and height)
     * @param size Size in pixels
     */
    QRCode& size(int32_t size) noexcept {
        lv_qrcode_set_size(m_obj, size);
        return *this;
    }

    /**
     * @brief Set dark color (the data modules)
     * @param color Dark color
     */
    QRCode& dark_color(lv_color_t color) noexcept {
        lv_qrcode_set_dark_color(m_obj, color);
        return *this;
    }

    /**
     * @brief Set light color (the background)
     * @param color Light color
     */
    QRCode& light_color(lv_color_t color) noexcept {
        lv_qrcode_set_light_color(m_obj, color);
        return *this;
    }

    /**
     * @brief Enable or disable quiet zone
     *
     * Quiet zone is the blank margin around the QR code.
     *
     * @param enable true to enable quiet zone
     */
    QRCode& quiet_zone(bool enable) noexcept {
        lv_qrcode_set_quiet_zone(m_obj, enable);
        return *this;
    }

    // ==================== Data ====================

    /**
     * @brief Set QR code data from string
     * @param text Text to encode
     */
    QRCode& data(const char* text) noexcept {
        lv_qrcode_set_data(m_obj, text);
        return *this;
    }

    /**
     * @brief Set QR code data from string_view
     * @param text Text to encode
     */
    QRCode& data(std::string_view text) noexcept {
        // lv_qrcode_update handles the length, so we can use it directly
        lv_qrcode_update(m_obj, text.data(), static_cast<uint32_t>(text.size()));
        return *this;
    }

    /**
     * @brief Set QR code data from binary data
     * @param data Binary data to encode
     * @param len Length in bytes
     * @return true if successful
     */
    bool update(const void* data, uint32_t len) noexcept {
        return lv_qrcode_update(m_obj, data, len) == LV_RESULT_OK;
    }
};

} // namespace lv

#endif // LV_USE_QRCODE
