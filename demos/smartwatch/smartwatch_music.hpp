#pragma once
/**
 * @file smartwatch_music.hpp
 * @brief Music screen for smartwatch demo
 */

#include "smartwatch_private.hpp"

// Image declarations (must be outside namespace - C symbols)
LV_IMAGE_DECLARE(image_album_cover_1);
LV_IMAGE_DECLARE(image_album_cover_2);
LV_IMAGE_DECLARE(image_album_cover_3);
LV_IMAGE_DECLARE(image_album_cover_4);
LV_IMAGE_DECLARE(image_pause_icon);
LV_IMAGE_DECLARE(image_previous_icon);
LV_IMAGE_DECLARE(image_next_icon);
LV_IMAGE_DECLARE(image_volume_icon);

// Font declarations (must be outside namespace - C symbols)
LV_FONT_DECLARE(font_inter_regular_28);
LV_FONT_DECLARE(font_inter_regular_24);

namespace smartwatch {

class MusicScreen {
public:
    void create(DemoController& controller);
    [[nodiscard]] lv_obj_t* get() const { return m_screen; }

private:
    static constexpr int MUSIC_OBJECTS = 4;
    static constexpr int RADIUS_SMALL = 40;
    static constexpr int RADIUS_LARGE = 78;

    struct MusicInfo {
        uint32_t color;
        const void* cover;
        const char* artist;
        const char* track;
    };

    struct MusicObject {
        lv_obj_t* obj;
        int position;
    };

    void on_gesture(lv::Event e);
    void on_play_click(lv::Event e);
    void on_next_click(lv::Event e);
    void on_previous_click(lv::Event e);
    void on_drag(lv::Event e);

    void on_rotation(int32_t v);
    void on_rotation_complete();
    void on_radius(int32_t v);

    void animate_rotation(int32_t angle);
    void animate_radius(int32_t target);
    void rotate_objects(int32_t angle);

    lv_obj_t* m_screen = nullptr;
    lv_obj_t* m_label_time = nullptr;
    lv_obj_t* m_arc_volume = nullptr;
    lv_obj_t* m_cont_music = nullptr;
    lv_obj_t* m_button_play = nullptr;
    lv_obj_t* m_arc_progress = nullptr;
    lv_obj_t* m_button_previous = nullptr;
    lv_obj_t* m_button_next = nullptr;
    lv_obj_t* m_cont_info = nullptr;
    lv_obj_t* m_label_artist = nullptr;
    lv_obj_t* m_label_track = nullptr;
    lv_obj_t* m_image_volume = nullptr;

    DemoController* m_controller = nullptr;
    int32_t m_radius = RADIUS_SMALL;
    int32_t m_last_angle = 0;
    MusicObject m_objects[MUSIC_OBJECTS]{};

    lv::Style m_main_style;
    bool m_inited = false;

    static constexpr MusicInfo music_list[] = {
        {0x36A1C7, &image_album_cover_1, "Jeff Black", "Wired"},
        {0xFB4129, &image_album_cover_2, "Primal Scream", "Screamdelica"},
        {0x7b5845, &image_album_cover_3, "Bill Evanz", "Portrait in Jazz"},
        {0xF4E36A, &image_album_cover_4, "The Doors", "Morrison Hotel"},
    };

};

inline void MusicScreen::create(DemoController& controller) {
    m_controller = &controller;

    if (!m_inited) {
        m_main_style.text_color(lv::colors::white())
            .bg_color(lv::rgb(0x000000))
            .bg_opa(lv::opa::_100)
            .radius(lv::kRadius::circle)
            .translate_x(SCREEN_SIZE)
            .bg_grad_color(lv::rgb(0x000000))
            .bg_main_stop(0)
            .bg_grad_stop(255)
            .bg_grad_dir(lv::kGradDir::ver);
        m_inited = true;
    }

    auto screen = lv::Box::create(lv::screen_active());
    screen.remove_all_styles()
        .add_style(m_main_style.get())
        .fill()
        .scrollbar_mode(lv::kScrollbarMode::off)
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble)
        .add_flag(lv::kFlag::clickable);
    screen.on<&MusicScreen::on_gesture>(lv::kEvent::gesture, this);
    m_screen = screen.get();

    m_label_time = lv::Label::create(m_screen)
        .size(lv::kSize::content, lv::kSize::content)
        .pos(0, 20)
        .align(lv::kAlign::top_mid)
        .text("13:37")
        .text_font(&font_inter_regular_28)
        .get();

    // Volume arc
    m_arc_volume = lv::Arc::create(m_screen)
        .size(SCREEN_SIZE - 35, SCREEN_SIZE - 35)
        .align(lv::kAlign::center)
        .value(50)
        .bg_angles(345, 25)
        .mode(lv::kArcMode::reverse)
        .arc_width(12)
        .arc_color(lv::rgb(0xBBBBBB))
        .arc_opa(100)
        .indicator_arc_color(lv::rgb(0xFFFFFF))
        .indicator_arc_opa(lv::opa::cover)
        .indicator_arc_width(12)
        .knob_bg_opa(lv::opa::transp)
        .get();

    // Music controls container
    auto cont_music = lv::Box::create(m_screen);
    cont_music.remove_all_styles()
        .size(216, 80)
        .pos(0, 135)
        .align(lv::kAlign::center)
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable);
    m_cont_music = cont_music.get();

    // Play button
    auto button_play = lv::Box::create(m_cont_music);
    button_play.remove_all_styles()
        .size(68, 68)
        .align(lv::kAlign::bottom_mid)
        .remove_flag(lv::kFlag::scrollable)
        .radius(34)
        .bg_color(lv::rgb(0xFFFFFF))
        .bg_opa(50)
        .bg_image_src(&image_pause_icon);
    button_play.on_click<&MusicScreen::on_play_click>(this);
    m_button_play = button_play.get();

    // Progress arc
    m_arc_progress = lv::Arc::create(m_cont_music)
        .size(68, 68)
        .align(lv::kAlign::bottom_mid)
        .remove_flag(lv::kFlag::clickable)
        .value(50)
        .bg_angles(0, 360)
        .rotation(270)
        .arc_color(lv::rgb(0xBBBBBB))
        .arc_opa(100)
        .arc_width(3)
        .indicator_arc_color(lv::rgb(0x07D685))
        .indicator_arc_opa(lv::opa::cover)
        .indicator_arc_width(3)
        .indicator_arc_rounded(false)
        .knob_bg_opa(lv::opa::transp)
        .get();

    // Previous button
    auto button_previous = lv::Box::create(m_cont_music);
    button_previous.remove_all_styles()
        .size(56, 56)
        .remove_flag(lv::kFlag::scrollable)
        .radius(34)
        .bg_color(lv::rgb(0xFFFFFF))
        .bg_opa(50)
        .bg_image_src(&image_previous_icon);
    button_previous.on_click<&MusicScreen::on_previous_click>(this);
    m_button_previous = button_previous.get();

    // Next button
    auto button_next = lv::Box::create(m_cont_music);
    button_next.remove_all_styles()
        .size(56, 56)
        .align(lv::kAlign::top_right)
        .remove_flag(lv::kFlag::scrollable)
        .radius(34)
        .bg_color(lv::rgb(0xFFFFFF))
        .bg_opa(50)
        .bg_image_src(&image_next_icon);
    button_next.on_click<&MusicScreen::on_next_click>(this);
    m_button_next = button_next.get();

    // Album image container
    auto image_cont = lv::Box::create(m_screen);
    image_cont.remove_all_styles()
        .size(312, 156)
        .pos(-1, 66)
        .align(lv::kAlign::top_mid)
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble)
        .add_flag(lv::kFlag::clickable)
        .add_flag(lv::kFlag::click_focusable);
    image_cont.on<&MusicScreen::on_drag>(lv::kEvent::all, this);

    for (int i = 0; i < MUSIC_OBJECTS; i++) {
        m_objects[i].position = i * 90;
        m_objects[i].obj = lv::Image::create(image_cont)
            .src(music_list[i].cover)
            .x(m_objects[i].position)
            .align(lv::kAlign::top_mid)
            .remove_flag(lv::kFlag::clickable)
            .remove_flag(lv::kFlag::scrollable)
            .get();
    }

    // Info container
    auto cont_info = lv::vbox(lv::ObjectView(m_screen))
        .size(245, 55)
        .pos(4, 227)
        .align(lv::kFlexAlign::space_between, lv::kFlexAlign::center, lv::kFlexAlign::center)
        .remove_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable);
    m_cont_info = cont_info.get();

    m_label_artist = lv::Label::create(m_cont_info)
        .align(lv::kAlign::center)
        .text("Jeff Black")
        .text_font(&font_inter_regular_24)
        .get();

    m_label_track = lv::Label::create(m_cont_info)
        .align(lv::kAlign::center)
        .text("Wired")
        .text_font(&font_inter_regular_24)
        .opa(lv::opa::_80)
        .get();

    // Volume icon
    m_image_volume = lv::Image::create(m_screen)
        .src(&image_volume_icon)
        .align(lv::kAlign::right_mid)
        .pos(static_cast<int32_t>(SCREEN_SIZE * -0.1), static_cast<int32_t>(SCREEN_SIZE * 0.25))
        .add_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable)
        .get();

    rotate_objects(0);
}

inline void MusicScreen::rotate_objects(int32_t angle) {
    for (int i = 0; i < MUSIC_OBJECTS; i++) {
        m_objects[i].position = (m_objects[i].position + angle) % 360;
        int32_t x = (lv_trigo_cos(m_objects[i].position) * m_radius) / LV_TRIGO_SIN_MAX;
        int32_t y = (lv_trigo_sin(m_objects[i].position) * m_radius) / LV_TRIGO_SIN_MAX;

        auto obj = lv::ref(m_objects[i].obj);
        if (y > m_radius / 2) {
            obj.move_foreground();
            lv::Label(lv::wrap, m_label_artist).text(music_list[i].artist);
            lv::Label(lv::wrap, m_label_track).text(music_list[i].track);
            lv::ref(m_screen).bg_color(lv::rgb(music_list[i].color));
        } else if (y < -m_radius / 2) {
            obj.move_background();
        }

        int32_t min_scale = lv_map(m_radius, RADIUS_SMALL, RADIUS_LARGE, 128, 156);
        int32_t max_scale = lv_map(m_radius, RADIUS_SMALL, RADIUS_LARGE, 240, 256);
        lv::Image(lv::wrap, m_objects[i].obj).scale(lv_map(y, -m_radius, m_radius, min_scale, max_scale));
        obj.x(x);
    }
}

inline void MusicScreen::animate_rotation(int32_t angle) {
    m_last_angle = 0;
    lv::Anim()
        .var(this)
        .exec([](void* var, int32_t v) {
            static_cast<MusicScreen*>(var)->on_rotation(v);
        })
        .on_complete([](lv_anim_t* a) {
            static_cast<MusicScreen*>(a->var)->on_rotation_complete();
        })
        .values(0, angle)
        .duration(500)
        .ease_out()
        .start();
}

inline void MusicScreen::animate_radius(int32_t target) {
    lv::Anim()
        .var(this)
        .exec([](void* var, int32_t v) {
            static_cast<MusicScreen*>(var)->on_radius(v);
        })
        .values(m_radius, target)
        .duration(500)
        .ease_out()
        .start();
}

inline void MusicScreen::on_rotation(int32_t v) {
    int32_t delta = v - m_last_angle;
    m_last_angle = v;
    rotate_objects(delta);
}

inline void MusicScreen::on_rotation_complete() {
    int32_t end_angle = m_objects[0].position;
    if (end_angle % 90 != 0) {
        int32_t nearest = (end_angle + (end_angle >= 0 ? 45 : -45)) / 90 * 90;
        animate_rotation(nearest - end_angle);
    }
}

inline void MusicScreen::on_radius(int32_t v) {
    m_radius = v;
    rotate_objects(0);
}

// on_gesture implementation deferred to smartwatch_gestures.hpp
// to avoid incomplete type issues with forward-declared screen classes

inline void MusicScreen::on_play_click(lv::Event e) {
    (void)e;
    static bool playing = true;
    playing = !playing;
    lv::Arc(lv::wrap, m_arc_progress).value(playing ? 50 : 0);
}

inline void MusicScreen::on_next_click(lv::Event e) {
    (void)e;
    animate_rotation(90);
}

inline void MusicScreen::on_previous_click(lv::Event e) {
    (void)e;
    animate_rotation(-90);
}

inline void MusicScreen::on_drag(lv::Event e) {
    lv_event_code_t code = e.code();

    if (code == lv::kEvent::focused) {
        animate_radius(RADIUS_LARGE);
    }
    if (code == lv::kEvent::defocused) {
        animate_radius(RADIUS_SMALL);
    }
    if (code == lv::kEvent::pressing) {
        lv_indev_t* indev = e.indev();
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        rotate_objects(vect.x * -1);
    }
    if (code == lv::kEvent::released) {
        int32_t current = m_objects[0].position;
        int32_t nearest = (current + (current >= 0 ? 45 : -45)) / 90 * 90;
        animate_rotation(nearest - current);
    }
}

} // namespace smartwatch
