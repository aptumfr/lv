/**
 * @file form.cpp
 * @brief Form/Input example with validation
 *
 * Demonstrates:
 * - Textarea for text input
 * - Keyboard widget integration
 * - Form validation
 * - Dropdown and checkbox inputs
 */

#include <lv/lv.hpp>
#include <cstring>

// Simple validation
static bool is_valid_email(const char* email) {
    if (!email || strlen(email) < 5) return false;
    const char* at = strchr(email, '@');
    if (!at || at == email) return false;
    const char* dot = strchr(at, '.');
    return dot && dot > at + 1 && strlen(dot) > 1;
}

static bool is_valid_password(const char* pwd) {
    return pwd && strlen(pwd) >= 6;
}

class FormApp {
    lv::Textarea m_email_ta;
    lv::Textarea m_password_ta;
    lv::Checkbox m_terms_cb;
    lv::Label m_status_lbl;
    lv::Keyboard m_keyboard;

public:
    void create() {
        auto screen = lv::screen_active();
        screen.bg_color(lv::colors::white());  // white() exists in C API

        auto content = lv::vbox(screen)
            .fill()
            .padding(16)
            .gap(12);

        // Title
        lv::Label::create(content)
            .text("Registration Form")
            .text_color(lv::rgb(0x2196F3));  // blue

        // Email field
        lv::Label::create(content)
            .text("Email:")
            .text_color(lv::rgb(0x404040));  // dark gray

        m_email_ta = lv::Textarea::create(content)
            .one_line(true)
            .placeholder("Enter your email")
            .max_length(50)
            .fill_width()
            .on_focused<&FormApp::on_textarea_focus>(this)
            .on_defocused<&FormApp::on_textarea_defocus>(this);

        // Password field
        lv::Label::create(content)
            .text("Password:")
            .text_color(lv::rgb(0x404040));  // dark gray

        m_password_ta = lv::Textarea::create(content)
            .one_line(true)
            .placeholder("Min 6 characters")
            .password_mode(true)
            .max_length(30)
            .fill_width()
            .on_focused<&FormApp::on_textarea_focus>(this)
            .on_defocused<&FormApp::on_textarea_defocus>(this);

        // Country dropdown
        lv::Label::create(content)
            .text("Country:")
            .text_color(lv::rgb(0x404040));  // dark gray

        lv::Dropdown::create(content)
            .options("United States\nUnited Kingdom\nCanada\nGermany\nFrance\nJapan\nOther")
            .fill_width();

        // Terms checkbox
        m_terms_cb = lv::Checkbox::create(content)
            .text("I agree to the terms and conditions");

        // Submit button
        lv::Button::create(content)
            .text("Register")
            .fill_width()
            .on_click<&FormApp::on_submit>(this);

        // Status label
        m_status_lbl = lv::Label::create(content)
            .text("");

        // Keyboard (hidden by default)
        m_keyboard = lv::Keyboard::create(screen)
            .fill_width()
            .height(200)
            .align_bottom()
            .hide();
    }

private:
    void on_textarea_focus(lv::Event e) {
        m_keyboard.textarea(e.target()).show();
    }

    void on_textarea_defocus(lv::Event) {
        m_keyboard.hide();
    }

    void on_submit(lv::Event) {
        const char* email = m_email_ta.text();
        const char* password = m_password_ta.text();
        bool terms = m_terms_cb.is_checked();

        // Validate
        if (!is_valid_email(email)) {
            m_status_lbl.text("Invalid email address")
                .text_color(lv::rgb(0xF44336));  // red
            return;
        }

        if (!is_valid_password(password)) {
            m_status_lbl.text("Password must be at least 6 characters")
                .text_color(lv::rgb(0xF44336));  // red
            return;
        }

        if (!terms) {
            m_status_lbl.text("Please accept the terms")
                .text_color(lv::rgb(0xF44336));  // red
            return;
        }

        // Success
        m_status_lbl.text("Registration successful!")
            .text_color(lv::rgb(0x4CAF50));  // green
    }
};

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Form Demo", 400, 580);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 580);
#else
    #error "No display backend enabled"
#endif

    FormApp app;
    app.create();

    lv::run();
    return 0;
}
