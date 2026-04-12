#pragma once

/**
 * @file ebike_home.hpp
 * @brief Home screen with speedometer - C++ port
 *
 * Demonstrates:
 * - lv::Timer with type-safe member function callbacks
 * - lv::IntState for reactive bindings
 * - C++ opacity constants (lv::opa::*)
 * - C++ size helpers (lv::pct, lv::kSize::content)
 */

#include <lv/lv.hpp>
#include "ebike_private.hpp"
#include "translations/lv_i18n.h"

namespace ebike {

/**
 * @brief Home page with speedometer, roller digits, and info cards
 */
class HomePage {
    lv::Box m_root;
    lv::Box m_left_cont;
    lv::Box m_right_cont;

    // Speedometer components
    lv::Image m_scale;
    lv::Arc m_arc;
    lv::Roller m_roller_10;
    lv::Roller m_roller_1;

    // Reactive state for speed updates
    lv::IntState m_speed_arc{0};
    lv::IntState m_speed_roller{0};

    // RAII timer - automatically deleted in destructor
    lv::Timer m_anim_timer{};

public:
    void init() {
        // IntState is initialized in member declaration
    }

    void deinit() {
        // Release the current timer (if any) by move-assigning an empty Timer
        m_anim_timer = lv::Timer{};
    }

    void create(lv::ObjectView parent) {
        m_root = lv::Box::create(parent)
            .fill()
            .bg_opa(lv::opa::transp)
            .flex_flow(lv::kFlexFlow::row)
            .flex_main_place(lv::kFlexAlign::space_evenly);

        create_left_cont();
        create_right_cont();

        // Size the containers
        m_left_cont.size(lv::pct(40), lv::pct(100))
                   .min_width(220);
        m_right_cont.size(lv::pct(40), lv::pct(100))
                    .min_width(150);

        // Start animation timer with type-safe member function callback
        m_anim_timer = lv::Timer::create<&HomePage::on_anim_tick>(2000, this);
    }

    [[nodiscard]] lv::Box& root() { return m_root; }

private:
    void on_anim_tick() {
        int32_t v = lv_rand(0, 90);

        // Animate the arc using typed helper
        lv::Anim()
            .exec_int_subject(m_speed_arc.subject())
            .values(m_speed_arc.get(), v)
            .duration(1000)
            .start();

        // Update roller
        m_speed_roller.set(v);
    }

    void create_left_cont() {
        m_left_cont = lv::Box::create(m_root)
            .bg_opa(lv::opa::transp)
            .remove_flag(lv::kFlag::scrollable);

        // Scale image
        m_scale = lv::Image::create(m_left_cont)
            .src(&img_ebike_scale)
            .align(lv::kAlign::left_mid);

        // Arc for speed indicator
        m_arc = lv::Arc::create(m_left_cont)
            .size(440, 440)
            .bg_angles(0, 90)
            .rotation(130)
            .range(-20, 110)
            .align(lv::kAlign::left_mid, 52, 0)
            .remove_flag(lv::kFlag::clickable);

        m_arc.arc_width(20)
             .bg_opa(lv::opa::_0, lv::kPart::knob)
             .arc_opa(lv::opa::_0)
             .indicator_width(20)
             .indicator_color(turquoise())
             .bind_value(m_speed_arc.subject());

        // Speed labels around the arc
        create_speed_labels();

        // Dashboard center with rollers
        create_dashboard_center();

        // Bottom info box
        create_bottom_info();
    }

    /// Context struct for typed observer data (avoids reinterpret_cast)
    struct SpeedLabelContext {
        int32_t label_value;
    };

    void create_speed_labels() {
        // Static array for observer contexts (lives for app lifetime)
        static SpeedLabelContext contexts[5];

        for (int i = 0; i < 5; i++) {
            auto obj = lv::Box::create(m_left_cont)
                .size(40, 50)
                .bg_opa(lv::opa::transp)
                .align(lv::kAlign::left_mid, -10, 0)
                .transform_pivot_x(260)
                .transform_pivot_y(lv::pct(50))
                .transform_rotation(-260 + i * 150);

            auto label = lv::Label::create(obj)
                .align(lv::kAlign::right_mid)
                .text_font(font_medium())
                .transform_pivot_x(lv::pct(100))
                .transform_pivot_y(lv::pct(50))
                .text_fmt("%d", (i + 1) * 20);

            // Store label value in typed context struct
            contexts[i].label_value = (i + 1) * 20;

            // Add observer for zoom effect based on speed
            m_speed_arc.observe_obj(
                [](lv_observer_t* observer, lv_subject_t* subject) {
                    lv::Label lbl(lv::wrap, lv::observer_get_target_obj(observer));
                    auto* ctx = lv::observer_get_user_data<SpeedLabelContext>(observer);
                    int32_t speed = lv::subject_get_int(subject);

                    int32_t diff = LV_ABS(ctx->label_value - speed);
                    uint32_t zoom = lv_map(diff, 0, 20, 512, 256);
                    lbl.transform_scale(zoom);
                },
                label.get(),
                &contexts[i]);
        }
    }

    void create_dashboard_center() {
        auto dashboard_cont = lv::Box::create(m_left_cont)
            .size(lv::pct(100), 240)
            .bg_opa(lv::opa::transp)
            .align(lv::kAlign::left_mid)
            .remove_flag(lv::kFlag::clickable)
            .pad_left(110);

        // Top icons
        auto top_cont = lv::Box::create(dashboard_cont)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .flex_flow(lv::kFlexFlow::row)
            .flex_align(lv::kFlexAlign::space_between, lv::kFlexAlign::center, lv::kFlexAlign::center);

        lv::Image::create(top_cont).src(&img_ebike_arrow_left);
        lv::Image::create(top_cont).src(&img_ebike_lamp);
        lv::Image::create(top_cont).src(&img_ebike_arrow_right);

        // Roller container for speed digits
        auto roller_cont = lv::Box::create(dashboard_cont)
            .size(lv::kSize::content, lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .center();

        // Tens digit roller
        m_roller_10 = create_speed_roller(roller_cont, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
        m_roller_10.align(lv::kAlign::left_mid);
        m_speed_roller.observe_obj(
            [](lv_observer_t* observer, lv_subject_t* subject) {
                lv::Roller roller(lv::wrap, lv::observer_get_target_obj(observer));
                int32_t speed = lv::subject_get_int(subject);
                roller.selected_anim(speed / 10);
            },
            m_roller_10);

        // Ones digit roller (repeated digits for circular effect)
        const char* opts2 = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n"
                           "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n"
                           "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n"
                           "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
        m_roller_1 = create_speed_roller(roller_cont, opts2);
        m_roller_1.align(lv::kAlign::left_mid, 50, 0);
        m_speed_roller.observe_obj(
            [](lv_observer_t* observer, lv_subject_t* subject) {
                lv::Roller roller(lv::wrap, lv::observer_get_target_obj(observer));
                int32_t speed = lv::subject_get_int(subject);
                roller.selected_anim(speed);
            },
            m_roller_1);
    }

    lv::Roller create_speed_roller(lv::ObjectView parent, const char* opts) {
        auto roller = lv::Roller::create(parent)
            .options(opts, LV_ROLLER_MODE_NORMAL)
            .visible_rows(1);

        roller.size(60, 140)  // Explicit height to prevent cropping
              .text_font(font_speed())
              .text_align(lv::kTextAlign::center)
              .anim_duration(1000)
              .bg_opa(lv::opa::_0)
              .bg_opa(lv::opa::_0, lv::kPart::selected);

        return roller;
    }

    void create_bottom_info() {
        auto bottom_cont = lv::Box::create(m_left_cont)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .align(lv::kAlign::left_mid, 0, 105)
            .flex_flow(lv::kFlexFlow::row)
            .pad_left(110);

        create_info_box(bottom_cont, &img_ebike_clock, " 03:58 PM", _("March 29"));
    }

    void create_info_box(lv::ObjectView parent, const void* icon, const char* big_text, const char* small_text) {
        auto cont = lv::Box::create(parent)
            .height(lv::pct(100))
            .bg_opa(lv::opa::transp)
            .grow()
            .flex_flow(lv::kFlexFlow::row)
            .flex_align(lv::kFlexAlign::center, lv::kFlexAlign::center, lv::kFlexAlign::center)
            .size(lv::pct(100), lv::kSize::content)
            .center();

        lv::Image::create(cont).src(icon);

        lv::Label::create(cont)
            .text(big_text)
            .text_font(font_medium());

        lv::Label::create(cont)
            .text(small_text)
            .text_font(font_small())
            .add_flag(lv::kFlag::flex_in_new_track)
            .margin_left(32);
    }

    void create_right_cont() {
        m_right_cont = lv::Box::create(m_root)
            .bg_opa(lv::opa::transp)
            .flex_flow(lv::kFlexFlow::column)
            .padding_ver(12)
            .padding_hor(8)
            .gap(8);

        // Battery card
        create_battery_card();

        // Distance card
        create_distance_card();
    }

    void create_battery_card() {
        auto battery = lv::Bar::create(m_right_cont)
            .fill()
            .value(82)
            .grow()
            .scrollable()
            .radius(12)
            .flex_flow(lv::kFlexFlow::row)
            .scroll_snap_x(lv::kScrollSnap::center)
            .border_color(lime())
            .border_width(4)
            .bg_opa(lv::opa::_0)
            .padding(10)
            .pad_column(16)
            .text_color(lv::colors::black())
            .bg_color(lime(), lv::kPart::indicator)
            .radius(6, lv::kPart::indicator)
            .scrollbar_mode(lv::kScrollbarMode::off);

        create_card_labels(battery, "82", "%", _("Battery"));
        create_card_labels(battery, "29:31", "", _("Battery"));

        create_bullets(battery);
    }

    void create_distance_card() {
        auto dist = lv::Box::create(m_right_cont)
            .fill()
            .radius(12)
            .bg_color(turquoise())
            .grow()
            .scrollable()
            .padding_ver(14)
            .padding_hor(4)
            .text_color(lv::colors::black())
            .pad_column(16)
            .flex_flow(lv::kFlexFlow::row)
            .scroll_snap_x(lv::kScrollSnap::center)
            .scrollbar_mode(lv::kScrollbarMode::off);

        create_card_labels(dist, "16.4", "km", _("Distance today"));
        create_card_labels(dist, "20.1", "km/h", _("Speed today"));
        create_card_labels(dist, "43:12", "", _("Time today"));

        create_bullets(dist);
    }

    void create_card_labels(lv::ObjectView parent, const char* value, const char* unit, const char* title) {
        auto cont = lv::Box::create(parent)
            .fill()
            .bg_opa(lv::opa::transp);

        auto center_cont = lv::Box::create(cont)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .center()
            .flex_flow(lv::kFlexFlow::row)
            .flex_align(lv::kFlexAlign::center, lv::kFlexAlign::start, lv::kFlexAlign::center);

        lv::Label::create(center_cont)
            .text(value)
            .text_font(font_large());

        lv::Label::create(center_cont)
            .text(unit)
            .text_font(font_small());

        lv::Label::create(cont)
            .text(title)
            .text_font(font_small())
            .align(lv::kAlign::bottom_mid, 0, -4);
    }

    void create_bullets(lv::ObjectView parent) {
        uint32_t cnt = parent.child_count();

        auto cont = lv::Box::create(parent)
            .size(lv::kSize::content, lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .add_flag(lv::kFlag::ignore_layout)
            .add_flag(lv::kFlag::floating)
            .align(lv::kAlign::top_mid)
            .padding(4)
            .gap(8)
            .flex_flow(lv::kFlexFlow::row);

        for (uint32_t i = 0; i < cnt; i++) {
            lv::Box::create(cont)
                .size(5, 5)
                .radius(lv::kRadius::circle)
                .bg_color(lv::rgb(0x000000))
                .bg_opa(lv::opa::_50)
                .bg_opa(lv::opa::cover, lv::kState::checked)
                .transform_width(2, lv::kState::checked)
                .transform_height(2, lv::kState::checked);
        }

        // Mark first bullet as checked
        cont.child(0).add_state(lv::kState::checked);

        // Add scroll handler using on_scroll_end with user_data
        lv::ref(parent).on_raw(lv::kEvent::scroll_end, [](lv_event_t* e) {
            lv::Event event(e);
            auto main_cont = event.target();
            auto bullet_cont = lv::ref(static_cast<lv_obj_t*>(event.user_data()));

            int32_t idx = main_cont.scroll_x() / main_cont.content_width();

            uint32_t child_cnt = bullet_cont.child_count();
            for (uint32_t i = 0; i < child_cnt; i++) {
                bullet_cont.child(i).remove_state(lv::kState::checked);
            }
            bullet_cont.child(idx).add_state(lv::kState::checked);
        }, cont.get());
    }
};

} // namespace ebike
