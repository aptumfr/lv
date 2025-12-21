/**
 * @file settings.cpp
 * @brief Settings screen example demonstrating multiple widgets
 *
 * This example shows:
 * - Switch, Slider, Dropdown, Checkbox widgets
 * - Multiple components with show/hide
 * - Reactive state bindings
 * - Settings-style UI layout
 */

#include <lv/lv.hpp>

// ==================== Setting Row Helper ====================

/// Creates a row with label on left and widget on right
class SettingRow {
public:
    static lv::Flex create(lv::ObjectView parent, const char* label_text) {
        auto row = lv::hbox(parent)
            .fill_width()
            .padding_hor(5)
            .padding_ver(8)
            .align_items(lv::align::center);

        lv::Label::create(row)
            .text(label_text)
            .grow(1);

        return row;
    }
};

// ==================== Settings Screen ====================

class SettingsScreen : public lv::Component<SettingsScreen> {
    lv::State<int> m_brightness{75};
    lv::State<int> m_volume{50};

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent)
            .fill()
            .padding(15)
            .gap(5);

        // Header
        lv::Label::create(root)
            .text("Settings")
            .font(lv::fonts::montserrat_24)
            .center_text();


        // Dark Mode toggle
        auto dark_row = SettingRow::create(root, "Dark Mode");
        lv::Switch::create(dark_row)
            .on_value_changed<&SettingsScreen::on_dark_mode_changed>(this);

        // Notifications toggle
        auto notif_row = SettingRow::create(root, "Notifications");
        lv::Switch::create(notif_row)
            .on(true);  // Default on

        // Brightness slider
        auto bright_row = SettingRow::create(root, "Brightness");
        lv::Slider::create(bright_row)
            .width(150)
            .range(0, 100)
            .value(m_brightness.get())
            .on_value_changed<&SettingsScreen::on_brightness_changed>(this);

        // Volume slider
        auto vol_row = SettingRow::create(root, "Volume");
        lv::Slider::create(vol_row)
            .width(150)
            .range(0, 100)
            .value(m_volume.get())
            .on_value_changed<&SettingsScreen::on_volume_changed>(this);

        // Language dropdown
        auto lang_row = SettingRow::create(root, "Language");
        lv::Dropdown::create(lang_row)
            .width(120)
            .dir_up()
            .options_static("English\nFrench\nGerman\nSpanish\nJapanese");

        // Theme dropdown
        auto theme_row = SettingRow::create(root, "Theme");
        lv::Dropdown::create(theme_row)
            .width(120)
            .dir_up()
            .options_static("System\nLight\nDark\nBlue");

        // Checkboxes section
        lv::Label::create(root)
            .text("Privacy")
            .font(lv::fonts::montserrat_18);

        lv::Checkbox::create(root)
            .text("Share analytics")
            .checked(true);

        lv::Checkbox::create(root)
            .text("Show online status");

        lv::Checkbox::create(root)
            .text("Remember login")
            .checked(true);

        // Bottom buttons
        auto buttons = lv::hbox(root)
            .fill_width()
            .gap(20)
            .justify(lv::align::center);

        lv::Button::create(buttons)
            .text("Reset")
            .size(100, 40)
            .on_click<&SettingsScreen::on_reset>(this);

        lv::Button::create(buttons)
            .text("Save")
            .size(100, 40)
            .bg(lv::rgb(0x2196F3))  // blue
            .on_click<&SettingsScreen::on_save>(this);

        return root;
    }

private:
    void on_dark_mode_changed(lv::Event e) {
        bool is_on = lv::Switch(lv::wrap, e.target()).is_on();
        LV_LOG_USER("Dark mode: %s", is_on ? "ON" : "OFF");
    }

    void on_brightness_changed(lv::Event e) {
        int32_t val = lv::Slider(lv::wrap, e.target()).value();
        m_brightness.set(val);
        LV_LOG_USER("Brightness: %d", val);
    }

    void on_volume_changed(lv::Event e) {
        int32_t val = lv::Slider(lv::wrap, e.target()).value();
        m_volume.set(val);
        LV_LOG_USER("Volume: %d", val);
    }

    void on_reset(lv::Event) {
        LV_LOG_USER("Reset clicked");
        m_brightness.set(75);
        m_volume.set(50);
    }

    void on_save(lv::Event) {
        LV_LOG_USER("Save clicked - Brightness: %d, Volume: %d",
                    m_brightness.get(), m_volume.get());
    }
};

// ==================== Main ====================

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Settings Example", 400, 600);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 600);
#else
    #error "No display backend enabled"
#endif

    SettingsScreen settings;
    settings.mount(lv::screen_active());

    lv::run();
}
