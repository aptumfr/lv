/**
 * @file animation.cpp
 * @brief Animation showcase demonstrating all easing functions
 *
 * Demonstrates:
 * - All animation easing functions (linear, ease_in, ease_out, etc.)
 * - AnimationDemo class pattern (no globals)
 * - Constexpr table for easing definitions
 * - Typed exec helpers (exec_x)
 * - Timer::create<MemFn> for periodic auto-run
 * - lv::Event callback signature
 * - lv::opa::* and layout_none() for consistent C++ API
 */

#include <lv/lv.hpp>
#include <array>

class AnimationDemo {
    // Easing configuration - add/remove entries here to change the demo
    struct EasingConfig {
        const char* name;
        lv::anim_path_cb path;
        uint32_t color_hex;  // Color as hex value
    };

    static constexpr std::array kEasings = {
        EasingConfig{"Linear",     lv::anim_path::linear,      0x2196F3},  // blue
        EasingConfig{"Ease In",    lv::anim_path::ease_in,     0xF44336},  // red
        EasingConfig{"Ease Out",   lv::anim_path::ease_out,    0x4CAF50},  // green
        EasingConfig{"Ease InOut", lv::anim_path::ease_in_out, 0x9C27B0},  // purple
        EasingConfig{"Overshoot",  lv::anim_path::overshoot,   0xFF9800},  // orange
        EasingConfig{"Bounce",     lv::anim_path::bounce,      0x009688},  // teal
        EasingConfig{"Step",       lv::anim_path::step,        0xE91E63},  // pink
    };

    static constexpr size_t kBoxCount = kEasings.size();
    static_assert(kBoxCount == 7, "Update kBoxCount if easings table changes");

    struct AnimBox {
        lv::Box track;
        lv::Box box;
        lv::anim_path_cb path;
        int margin = 3;
    };

    std::array<AnimBox, kBoxCount> m_boxes{};
    bool m_running = false;
    int m_completed = 0;

    // Timer for auto-run feature
    lv::Timer m_auto_timer{nullptr};
    bool m_auto_enabled = false;

public:
    void create(lv::ObjectView parent) {
        auto content = lv::vbox(parent)
            .fill()
            .padding(8)
            .gap(6);

        // Title
        lv::Label::create(content)
            .text("Animation Easing Functions")
            .text_color(lv::rgb(0x2196F3));  // blue

        // Create boxes for each easing type from the constexpr table
        for (size_t i = 0; i < kEasings.size(); ++i) {
            const auto& cfg = kEasings[i];
            m_boxes[i] = create_anim_box(content, cfg.name, cfg.path, lv::rgb(cfg.color_hex));
        }

        // Control buttons
        auto controls = lv::hbox(content)
            .fill_width()
            .gap(10);

        lv::Button::create(controls)
            .text("Run")
            .grow(1)
            .on_click<&AnimationDemo::on_run>(this);

        lv::Button::create(controls)
            .text("Reset")
            .grow(1)
            .on_click<&AnimationDemo::on_reset>(this);

        lv::Button::create(controls)
            .text("Auto")
            .grow(1)
            .on_click<&AnimationDemo::on_toggle_auto>(this);

        // Info label
        lv::Label::create(content)
            .text("Run: play once | Auto: repeat every 5s\n"
                  "Watch how easings accelerate differently!")
            .text_color(lv::rgb(0x808080));  // gray
    }

private:
    AnimBox create_anim_box(lv::Flex& parent, const char* name, lv::anim_path_cb path, lv::Color color) {
        auto row = lv::hbox(parent)
            .fill_width()
            .gap(10)
            .height(60)
            .padding(8)
            .align_items(lv::align::center);

        // Label for the easing name
        lv::Label::create(row)
            .text(name)
            .width(90);

        // Track background - using lv::opa::* and layout_none()
        auto track = lv::Box::create(row)
            .grow(1)
            .height(34)
            .padding(0)
            .bg_color(lv::rgb(0xC0C0C0))  // light gray
            .bg_opa(lv::opa::_20)
            .radius(4)
            .border_width(0)
            .layout_none();

        // Animated box - using lv::opa::cover and layout_none()
        auto box = lv::Box::create(track)
            .size(30, 28)
            .pos(3, 3)
            .bg_color(color)
            .bg_opa(lv::opa::cover)
            .radius(4)
            .border_width(0)
            .layout_none()
            .grow(0);

        return {track, box, path, 3};
    }

    void run_animations() {
        if (m_running) return;
        m_running = true;
        m_completed = 0;

        for (auto& ab : m_boxes) {
            ab.track.update_layout();
            ab.box.update_layout();

            int track_w = ab.track.get_width();
            if (track_w == 0) track_w = 260;
            int box_w = ab.box.get_width();
            int start = ab.margin;
            int end = track_w - box_w - ab.margin;
            if (end < start) end = start;

            // Use typed exec_x helper
            lv::Anim()
                .exec_x(ab.box)
                .values(start, end)
                .duration(1500)
                .path(ab.path)
                .playback(200)
                .user_data(this)
                .on_complete([](lv::AnimData* a) {
                    auto* self = static_cast<AnimationDemo*>(a->user_data);
                    self->m_completed++;
                    if (self->m_completed >= static_cast<int>(kBoxCount)) {
                        self->m_running = false;
                    }
                })
                .start();
        }
    }

    void reset_positions() {
        for (auto& ab : m_boxes) {
            ab.box.x(ab.margin);
            lv::anim_delete(ab.box);
        }
        m_running = false;
        m_completed = 0;
    }

    void on_run(lv::Event) {
        run_animations();
    }

    void on_reset(lv::Event) {
        reset_positions();
    }

    void on_toggle_auto(lv::Event) {
        m_auto_enabled = !m_auto_enabled;
        if (m_auto_enabled) {
            // Create timer with member function callback - showcases Timer::create<MemFn>
            m_auto_timer = lv::Timer::create<&AnimationDemo::on_auto_tick>(5000, this);
            run_animations();  // Start immediately
        } else {
            m_auto_timer.del();
        }
    }

    // Timer callback - called every 5 seconds when auto-run is enabled
    void on_auto_tick() {
        reset_positions();
        run_animations();
    }
};

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Animation Showcase", 400, 640);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 720);
#else
    #error "No display backend enabled"
#endif

    AnimationDemo demo;
    demo.create(lv::screen_active());

    lv::run();
    return 0;
}
