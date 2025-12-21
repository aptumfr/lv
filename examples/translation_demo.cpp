/**
 * @file translation_demo.cpp
 * @brief Internationalization (i18n) example
 *
 * Demonstrates:
 * - Static translation tables
 * - Language switching at runtime
 * - Using lv::tr() for translatable text
 * - Runtime font loading for extended Latin characters
 *
 * Font Setup:
 *   This demo requires a TTF font with extended Latin characters.
 *   Place a font file (e.g., DejaVuSans.ttf) in:
 *     - ./fonts/DejaVuSans.ttf (relative to executable)
 *
 *   On Linux: cp /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf fonts/
 *   On Windows: Copy C:\Windows\Fonts\arial.ttf to fonts\
 *   On macOS: cp /System/Library/Fonts/Helvetica.ttc fonts/
 */

#include <lv/lv.hpp>

#if LV_USE_TRANSLATION

// ==================== Translation Data ====================

// Supported languages
static const char* g_languages[] = {"en", "de", "fr", "es", nullptr};

// Translation tags (keys)
static const char* g_tags[] = {
    "app_title",
    "greeting",
    "settings",
    "language",
    "dark_mode",
    "notifications",
    "volume",
    "save",
    "cancel",
    "welcome_msg",
    nullptr
};

// Translations: ordered by tag, then by language (en, de, fr, es)
static const char* g_translations[] = {
    // app_title
    "Translation Demo", "Ubersetzungs-Demo", "Demo de Traduction", "Demo de Traduccion",
    // greeting
    "Hello!", "Hallo!", "Bonjour!", "Hola!",
    // settings
    "Settings", "Einstellungen", "Parametres", "Configuracion",
    // language
    "Language", "Sprache", "Langue", "Idioma",
    // dark_mode
    "Dark Mode", "Dunkelmodus", "Mode Sombre", "Modo Oscuro",
    // notifications
    "Notifications", "Benachrichtigungen", "Notifications", "Notificaciones",
    // volume
    "Volume", "Lautstarke", "Volume", "Volumen",
    // save
    "Save", "Speichern", "Enregistrer", "Guardar",
    // cancel
    "Cancel", "Abbrechen", "Annuler", "Cancelar",
    // welcome_msg
    "Welcome to the app!\nSelect a language above.",
    "Willkommen in der App!\nWahlen Sie oben eine Sprache.",
    "Bienvenue dans l'app!\nSelectionnez une langue ci-dessus.",
    "Bienvenido a la app!\nSeleccione un idioma arriba.",
};

// Language display names for the dropdown
static const char* const g_language_names = "English\nDeutsch\nFrancais\nEspanol";

// ==================== App Class ====================

class TranslationApp {
    // Widget references (non-owning views)
    lv::Label m_title_label;
    lv::Label m_greeting_label;
    lv::Label m_settings_label;
    lv::Label m_language_label;
    lv::Label m_dark_mode_label;
    lv::Label m_notifications_label;
    lv::Label m_volume_label;
    lv::Label m_welcome_label;
    lv::Button m_save_btn;
    lv::Button m_cancel_btn;
    lv::Dropdown m_language_dropdown;
    lv::Switch m_dark_mode_switch;
    lv::Switch m_notifications_switch;

    // Font resources (RAII managed)
    lv::DynamicFont m_main_font;
    lv::DynamicFont m_title_font;

public:
    void create() {
        // Initialize translations
        lv::translation::add_static(g_languages, g_tags, g_translations);
        lv::translation::set_language("en");

        // Try to load TTF fonts for extended character support
        load_fonts();

        auto screen = lv::screen_active();
        screen.bg_color(lv::colors::white());  // white() exists in C API

        // Apply TTF font to screen if loaded
        if (m_main_font) {
            screen.font(m_main_font.get());
        }

        auto content = lv::vbox(screen)
            .fill()
            .padding(16)
            .gap(12);

        // Title
        m_title_label = lv::Label::create(content)
            .text(lv::tr("app_title"))
            .text_color(lv::rgb(0x2196F3));  // blue
        if (m_title_font) {
            m_title_label.font(m_title_font.get());
        }

        // Language selector row
        auto lang_row = lv::hbox(content)
            .fill_width()
            .gap(10)
            .align_items(lv::align::center);

        m_language_label = lv::Label::create(lang_row)
            .text(lv::tr("language"));

        m_language_dropdown = lv::Dropdown::create(lang_row)
            .options(g_language_names)
            .selected(0)
            .on_value_changed<&TranslationApp::on_language_changed>(this);

        // Greeting
        m_greeting_label = lv::Label::create(content)
            .text(lv::tr("greeting"))
            .text_color(lv::rgb(0x4CAF50))  // green
            .font(m_title_font ? m_title_font.get() : lv::fonts::montserrat_20);

        // Settings section
        m_settings_label = lv::Label::create(content)
            .text(lv::tr("settings"))
            .text_color(lv::rgb(0x404040));  // dark gray

        // Dark mode row
        auto dark_row = lv::hbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::rgb(0xf5f5f5))
            .radius(8)
            .align_items(lv::align::center);
        m_dark_mode_label = lv::Label::create(dark_row)
            .text(lv::tr("dark_mode"))
            .grow(1);
        m_dark_mode_switch = lv::Switch::create(dark_row);

        // Notifications row
        auto notif_row = lv::hbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::rgb(0xf5f5f5))
            .radius(8)
            .align_items(lv::align::center);
        m_notifications_label = lv::Label::create(notif_row)
            .text(lv::tr("notifications"))
            .grow(1);
        m_notifications_switch = lv::Switch::create(notif_row).on();

        // Volume row
        auto volume_row = lv::hbox(content)
            .fill_width()
            .padding(10)
            .bg_color(lv::rgb(0xf5f5f5))
            .radius(8)
            .gap(10)
            .align_items(lv::align::center);
        m_volume_label = lv::Label::create(volume_row)
            .text(lv::tr("volume"));
        lv::Slider::create(volume_row)
            .width(150)
            .value(70);

        // Welcome message
        m_welcome_label = lv::Label::create(content)
            .text(lv::tr("welcome_msg"))
            .text_color(lv::rgb(0x808080));  // gray

        // Buttons row
        auto buttons = lv::hbox(content)
            .fill_width()
            .gap(10);

        m_save_btn = lv::Button::create(buttons)
            .text(lv::tr("save"))
            .grow(1);

        m_cancel_btn = lv::Button::create(buttons)
            .text(lv::tr("cancel"))
            .grow(1);
    }

private:
    void load_fonts() {
        // Try relative paths (portable across platforms)
        static const char* font_paths[] = {
            "A:fonts/DejaVuSans.ttf",
            "A:../fonts/DejaVuSans.ttf",
            "A:../../fonts/DejaVuSans.ttf",
            nullptr
        };

        for (const char* const* path = font_paths; *path; ++path) {
            if (m_main_font.load_from_file(*path, 14)) {
                LV_LOG_USER("Loaded font: %s", *path);
                m_title_font.load_from_file(*path, 20);
                return;
            }
        }

        LV_LOG_WARN("TTF font not found. Place DejaVuSans.ttf in ./fonts/");
    }

    void on_language_changed() {
        uint32_t selected = m_language_dropdown.selected();

        const char* lang_codes[] = {"en", "de", "fr", "es"};
        if (selected < 4) {
            lv::translation::set_language(lang_codes[selected]);
            update_texts();
        }
    }

    void update_texts() {
        // Update all labels with new translations
        m_title_label.text(lv::tr("app_title"));
        m_greeting_label.text(lv::tr("greeting"));
        m_settings_label.text(lv::tr("settings"));
        m_language_label.text(lv::tr("language"));
        m_dark_mode_label.text(lv::tr("dark_mode"));
        m_notifications_label.text(lv::tr("notifications"));
        m_volume_label.text(lv::tr("volume"));
        m_welcome_label.text(lv::tr("welcome_msg"));

        // Update button labels (set_text updates existing label)
        m_save_btn.set_text(lv::tr("save"));
        m_cancel_btn.set_text(lv::tr("cancel"));
    }
};

#endif // LV_USE_TRANSLATION

// ==================== Main ====================

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Translation Demo", 400, 480);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 480);
#else
    #error "No display backend enabled"
#endif

#if LV_USE_TRANSLATION
    TranslationApp app;
    app.create();
#else
    lv::Label::create(lv::screen_active())
        .text("Enable LV_USE_TRANSLATION for this demo")
        .center();
#endif

    lv::run();
    return 0;
}
