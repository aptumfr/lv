/**
 * @file navigation.cpp
 * @brief Navigation example demonstrating screen transitions
 *
 * Demonstrates:
 * - Multiple screens with Navigator
 * - Screen transitions and animations
 * - Menu-based navigation
 * - Back button handling
 * - Member function pointer callbacks
 * - C++ symbol constants (lv::symbol::*)
 */

#include <lv/lv.hpp>

class NavigationDemo {
    lv::Navigator m_nav;
    lv::Screen m_home;
    lv::Screen m_settings;
    lv::Screen m_profile;
    lv::Screen m_about;

public:
    void create() {
        create_home_screen();
        create_settings_screen();
        create_profile_screen();
        create_about_screen();

        // Set home as root
        m_nav.set_root(m_home);
    }

private:
    void go_back(lv::Event) { m_nav.back(300); }
    void go_to_settings(lv::Event) { m_nav.push(m_settings, lv::screen_anim::move_left, 300); }
    void go_to_profile(lv::Event) { m_nav.push(m_profile, lv::screen_anim::move_left, 300); }
    void go_to_about(lv::Event) { m_nav.push(m_about, lv::screen_anim::move_left, 300); }

    // Generic nav button helper - takes member function pointer for destination
    template<auto DestMethod>
    void create_nav_button(lv::Flex& parent, const char* text, const char* icon) {
        auto btn = lv::hbox(parent)
            .fill_width()
            .padding(15)
            .gap(10)
            .bg_color(lv::rgb(0xf5f5f5))
            .radius(8)
            .align_items(lv::align::center);
        btn.clickable(true);
        btn.on_click<DestMethod>(this);

        lv::Label::create(btn).text(icon);
        lv::Label::create(btn).text(text).grow(1);
        lv::Label::create(btn).text(lv::symbol::right);
    }

    void create_home_screen() {
        m_home.bg_color(lv::colors::white())  // white() exists in C API
              .scrollbar_mode(lv::kScrollbarMode::off);

        auto content = lv::vbox(m_home)
            .fill()
            .padding(20)
            .gap(15);

        // Header
        lv::Label::create(content)
            .text("Home")
            .text_color(lv::rgb(0x2196F3));  // blue

        lv::Label::create(content)
            .text("Welcome to the Navigation Demo!\nSelect a destination:");

        // Navigation buttons - using single helper with different destinations
        create_nav_button<&NavigationDemo::go_to_settings>(content, "Settings", lv::symbol::settings);
        create_nav_button<&NavigationDemo::go_to_profile>(content, "Profile", lv::symbol::home);
        create_nav_button<&NavigationDemo::go_to_about>(content, "About", lv::symbol::list);
    }

    void create_settings_screen() {
        m_settings.bg_color(lv::rgb(0xf0f0f0))
                  .scrollbar_mode(lv::kScrollbarMode::off);

        auto content = lv::vbox(m_settings)
            .fill()
            .padding(20)
            .gap(15);

        create_header(content, "Settings");

        // Settings options - using LV_SYMBOL_* macros for string concatenation
        auto wifi_row = lv::hbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(8);
        lv::Label::create(wifi_row).text(LV_SYMBOL_WIFI " WiFi").grow(1);
        lv::Switch::create(wifi_row).on();

        auto bt_row = lv::hbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(8);
        lv::Label::create(bt_row).text(LV_SYMBOL_BLUETOOTH " Bluetooth").grow(1);
        lv::Switch::create(bt_row).off();

        auto volume_row = lv::vbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(8)
            .gap(5);
        lv::Label::create(volume_row).text(LV_SYMBOL_VOLUME_MAX " Volume");
        lv::Slider::create(volume_row)
            .fill_width()
            .value(70);

        auto brightness_row = lv::vbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(8)
            .gap(5);
        lv::Label::create(brightness_row).text("Brightness");
        lv::Slider::create(brightness_row)
            .fill_width()
            .value(50);
    }

    void create_profile_screen() {
        m_profile.bg_color(lv::rgb(0xe8f4fd))
                 .scrollbar_mode(lv::kScrollbarMode::off);

        auto content = lv::vbox(m_profile)
            .fill()
            .padding(20)
            .gap(15);

        create_header(content, "Profile");

        // Profile card
        auto card = lv::vbox(content)
            .fill_width()
            .padding(20)
            .gap(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(12)
            .align_items(lv::align::center);

        // Avatar placeholder
        auto avatar = lv::vbox(card)
            .size(80, 80)
            .bg_color(lv::rgb(0x2196F3))  // blue
            .radius(lv::kRadius::circle)
            .align_items(lv::align::center);
        lv::Label::create(avatar)
            .text(lv::symbol::home)
            .text_color(lv::colors::white())  // white() exists in C API
            .center();

        lv::Label::create(card)
            .text("John Doe")
            .text_color(lv::rgb(0x404040));  // dark gray

        lv::Label::create(card)
            .text("john.doe@example.com");

        // Stats
        auto stats = lv::hbox(content)
            .fill_width()
            .gap(10);

        create_stat_box(stats, "Projects", "42");
        create_stat_box(stats, "Followers", "1.2K");
        create_stat_box(stats, "Following", "89");
    }

    void create_about_screen() {
        m_about.bg_color(lv::rgb(0xfff8e1))
               .scrollbar_mode(lv::kScrollbarMode::off);

        auto content = lv::vbox(m_about)
            .fill()
            .padding(20)
            .gap(15);

        create_header(content, "About");

        auto card = lv::vbox(content)
            .fill_width()
            .padding(20)
            .gap(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(12);

        lv::Label::create(card)
            .text("lv:: C++20 Bindings")
            .text_color(lv::rgb(0x2196F3));  // blue

        lv::Label::create(card)
            .text("Version: 0.1.0");

        lv::Label::create(card)
            .text("Modern C++ wrapper for LVGL\nwith zero-cost abstractions.");

        lv::Label::create(card)
            .text("Features:\n"
                  "- Fluent API\n"
                  "- Reactive state\n"
                  "- Component system\n"
                  "- Type-safe events");
    }

    void create_header(lv::Flex& parent, const char* title) {
        auto header = lv::hbox(parent)
            .fill_width()
            .gap(10)
            .align_items(lv::align::center);

        // Back button
        lv::Button::create(header)
            .text(lv::symbol::left)
            .on_click<&NavigationDemo::go_back>(this);

        lv::Label::create(header)
            .text(title)
            .text_color(lv::rgb(0x2196F3))  // blue
            .grow(1);
    }

    void create_stat_box(lv::Flex& parent, const char* label, const char* value) {
        auto box = lv::vbox(parent)
            .grow(1)
            .padding(10)
            .bg_color(lv::colors::white())  // white() exists in C API
            .radius(8)
            .align_items(lv::align::center);

        lv::Label::create(box)
            .text(value)
            .text_color(lv::rgb(0x2196F3));  // blue

        lv::Label::create(box)
            .text(label);
    }
};

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Navigation Demo", 480, 480);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 480);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    NavigationDemo demo;
    demo.create();

    lv::run();
    return 0;
}
