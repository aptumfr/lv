#pragma once

/**
 * @file ime_pinyin.hpp
 * @brief Zero-cost wrapper for LVGL Pinyin Input Method Editor
 *
 * Requires LV_USE_IME_PINYIN enabled in lv_conf.h
 */

#include <lvgl.h>

#if LV_USE_IME_PINYIN

#include "../core/objectref.hpp"

namespace lv {

/**
 * @brief Pinyin IME input modes
 */
struct IMEPinyinMode {
    static constexpr auto k26 = LV_IME_PINYIN_MODE_K26;        // 26-key full keyboard
    static constexpr auto k9 = LV_IME_PINYIN_MODE_K9;          // 9-key T9 style
    static constexpr auto k9_number = LV_IME_PINYIN_MODE_K9_NUMBER;  // 9-key number mode
};

/**
 * @brief Pinyin Input Method Editor widget wrapper
 *
 * Provides Chinese Pinyin input method with keyboard integration.
 *
 * Example:
 * @code
 *   auto kb = lv::Keyboard::create(parent);
 *
 *   lv::IMEPinyin::create(parent)
 *       .keyboard(kb)
 *       .mode(lv::IMEPinyinMode::k26);
 * @endcode
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class IMEPinyin : public ObjectView,
                  public ObjectMixin<IMEPinyin>,
                  public EventMixin<IMEPinyin>,
                  public StyleMixin<IMEPinyin> {
public:
    constexpr IMEPinyin() noexcept : ObjectView(nullptr) {}
    constexpr IMEPinyin(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_ime_pinyin_class; }

    [[nodiscard]] static IMEPinyin create(lv_obj_t* parent) {
        return IMEPinyin(wrap, lv_ime_pinyin_create(parent));
    }
    [[nodiscard]] static IMEPinyin create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Configuration ====================

    /**
     * @brief Set the keyboard to use with this IME
     * @param kb Pointer to keyboard widget
     */
    IMEPinyin& keyboard(ObjectView kb) noexcept {
        lv_ime_pinyin_set_keyboard(m_obj, kb.get());
        return *this;
    }

    /**
     * @brief Set custom Pinyin dictionary
     * @param dict Pointer to dictionary array
     */
    IMEPinyin& dict(lv_pinyin_dict_t* dict) noexcept {
        lv_ime_pinyin_set_dict(m_obj, dict);
        return *this;
    }

    /**
     * @brief Set input mode (26-key or 9-key)
     * @param mode IMEPinyinMode::k26, k9, or k9_number
     */
    IMEPinyin& mode(lv_ime_pinyin_mode_t mode) noexcept {
        lv_ime_pinyin_set_mode(m_obj, mode);
        return *this;
    }

    // ==================== Getters ====================

    /// Get the associated keyboard
    [[nodiscard]] ObjectRef get_keyboard() const noexcept {
        return ObjectRef(lv_ime_pinyin_get_kb(m_obj));
    }

    /// Get the candidate panel widget
    [[nodiscard]] ObjectRef get_candidate_panel() const noexcept {
        return ObjectRef(lv_ime_pinyin_get_cand_panel(m_obj));
    }

    /// Get the dictionary
    [[nodiscard]] const lv_pinyin_dict_t* get_dict() const noexcept {
        return lv_ime_pinyin_get_dict(m_obj);
    }
};

} // namespace lv

#endif // LV_USE_IME_PINYIN
