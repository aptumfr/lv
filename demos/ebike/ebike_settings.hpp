#pragma once

/**
 * @file ebike_settings.hpp
 * @brief Settings screen - C++ port
 */

#include <lv/lv.hpp>
#include "ebike_private.hpp"
#include "translations/lv_i18n.h"

namespace ebike {

/**
 * @brief Settings page with sliders, switches, and dropdowns
 */
class SettingsPage {
    lv::Box m_root;

public:
    void create(lv::ObjectView parent, lv_subject_t* language_subject) {
        m_root = lv::Box::create(parent)
            .fill()
            .bg_opa(lv::opa::transp)
            .flex_flow(lv::kFlexFlow::row);

        create_left_cont();
        create_right_cont(language_subject);
    }

    [[nodiscard]] lv::Box& root() { return m_root; }

private:
    void create_left_cont() {
        auto left_cont = lv::Box::create(m_root)
            .size(164, lv::pct(100))
            .bg_opa(lv::opa::transp)
            .remove_flag(lv::kFlag::scrollable);

        lv::Label::create(left_cont)
            .text(_("SETTINGS"))
            .text_font(font_medium())
            .align(lv::kAlign::top_left, 24, 16);

        lv::Image::create(left_cont)
            .src(&img_ebike_settings_large)
            .align(lv::kAlign::bottom_mid);
    }

    void create_right_cont(lv_subject_t* language_subject) {
        auto right_cont = lv::Box::create(m_root)
            .fill()
            .bg_opa(lv::opa::transp)
            .grow()
            .padding_ver(12)
            .pad_right(8)
            .gap(8)
            .flex_flow(lv::kFlexFlow::column);

        // Scrollbar styling
        right_cont.part_width(3, lv::kPart::scrollbar)
                  .padding_ver(8, lv::kPart::scrollbar)
                  .radius(2, lv::kPart::scrollbar)
                  .bg_opa(lv::opa::_40, lv::kPart::scrollbar)
                  .bg_color(turquoise(), lv::kPart::scrollbar);

        // Settings items
        create_dropdown(right_cont, _("Language"), "English\n简体中文\nعربي", language_subject);
        create_switch(right_cont, _("Bluetooth"));
        create_switch(right_cont, _("Lights"));
        create_slider(right_cont, _("Brightness"));
        create_slider(right_cont, _("Volume"));
        create_slider(right_cont, _("Max. speed"));
        create_slider(right_cont, _("Light level"));
    }

    void create_slider(lv::ObjectView parent, const char* title) {
        auto cont = lv::Box::create(parent)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .text_font(font_small())
            .padding(10)
            .pad_column(4)
            .flex_flow(lv::kFlexFlow::column);

        lv::Label::create(cont)
            .text(title)
            .fill_width();

        lv::Slider::create(cont)
            .size(lv::pct(100), 4)
            .ext_click_area(24)
            .bg_opa(lv::opa::_30)
            .radius(lv::kRadius::circle)
            .bg_color(turquoise())
            .bg_color(turquoise(), lv::kPart::indicator)
            .radius(lv::kRadius::circle, lv::kPart::indicator)
            .padding(8, lv::kPart::knob)
            .radius(lv::kRadius::circle, lv::kPart::knob)
            .border_width(4, lv::kPart::knob)
            .border_color(turquoise(), lv::kPart::knob)
            .bg_color(lv::colors::black(), lv::kPart::knob)
            .margin_top(12);
    }

    void create_switch(lv::ObjectView parent, const char* title) {
        auto cont = lv::Box::create(parent)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .text_font(font_small())
            .padding(10)
            .pad_column(4)
            .flex_flow(lv::kFlexFlow::row)
            .flex_align(lv::kFlexAlign::space_between, lv::kFlexAlign::center, lv::kFlexAlign::center);

        lv::Label::create(cont)
            .text(title)
            .fill_width();

        lv::Switch::create(cont)
            .size(40, 24)
            .ext_click_area(32)
            .radius(lv::kRadius::circle)
            .bg_color(turquoise())
            .radius(lv::kRadius::circle, lv::kPart::knob)
            .bg_color(lv::colors::black(), lv::kPart::knob)
            .padding(-2, lv::kPart::knob);
    }

    void create_dropdown(lv::ObjectView parent, const char* title, const char* options, lv_subject_t* subject) {
        auto cont = lv::Box::create(parent)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .text_font(font_small())
            .padding(10)
            .pad_column(4)
            .flex_flow(lv::kFlexFlow::row)
            .flex_align(lv::kFlexAlign::space_between, lv::kFlexAlign::center, lv::kFlexAlign::center);

        lv::Label::create(cont).text(title);

        auto dd = lv::Dropdown::create(cont)
            .options(options)
            .width(150)
            .bg_color(turquoise())
            .bg_opa(lv::opa::_40)
            .radius(4)
            .padding(8);

        dd.symbol(&img_ebike_dropdown_icon);

        if (subject) {
            dd.bind_value(subject);
        }

        // Style the dropdown list - compound selectors (part | state)
        constexpr lv_style_selector_t selected_checked = lv::kPart::selected | static_cast<lv_style_selector_t>(lv::kState::checked);
        constexpr lv_style_selector_t selected_pressed = lv::kPart::selected | static_cast<lv_style_selector_t>(lv::kState::pressed);

        auto list = dd.list();
        list.bg_color(lv::colors::black())
            .bg_opa(lv::opa::cover)
            .bg_color(turquoise(), selected_checked)
            .bg_opa(lv::opa::_20, selected_checked)
            .bg_color(turquoise(), selected_pressed)
            .bg_opa(lv::opa::_40, selected_pressed)
            .radius(4)
            .text_line_space(16)
            .text_font(font_small())
            .padding(16);
    }
};

} // namespace ebike
