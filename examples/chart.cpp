/**
 * @file chart.cpp
 * @brief Chart example demonstrating lv::Chart widget
 *
 * Demonstrates:
 * - Line chart with multiple series
 * - Real-time data updates
 * - Animation and styling
 */

#include <lv/lv.hpp>
#include <cmath>
#include <cstdlib>
#include <memory>

// Generate sine wave data
static void generate_sine_data(lv_chart_series_t* ser, lv::Chart& chart, int offset) {
    for (int i = 0; i < 20; i++) {
        int32_t value = static_cast<int32_t>(50 + 40 * std::sin((i + offset) * 0.3));
        chart.set_next_value(ser, value);
    }
}

// Generate random data
static void generate_random_data(lv_chart_series_t* ser, lv::Chart& chart) {
    for (int i = 0; i < 20; i++) {
        int32_t value = 20 + (std::rand() % 60);
        chart.set_next_value(ser, value);
    }
}

#if LV_USE_OBSERVER

class ChartDemo : public lv::Component<ChartDemo> {
    lv::Chart* m_chart = nullptr;
    lv_chart_series_t* m_ser1 = nullptr;
    lv_chart_series_t* m_ser2 = nullptr;
    std::unique_ptr<lv::Timer> m_timer;
    int m_tick = 0;

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent)
            .fill()
            .padding(16)
            .gap(12);

        // Title
        lv::Label::create(root)
            .text("Real-time Chart Demo")
            .text_color(lv::rgb(0x2196F3));  // blue

        // Create chart
        auto chart = lv::Chart::create(root)
            .size(360, 200)
            .type(lv::Chart::Type::line)
            .point_count(20)
            .range(lv::Chart::Axis::primary_y, 0, 100)
            .div_lines(5, 4)
            .update_mode(lv::Chart::UpdateMode::shift);

        m_chart = new lv::Chart(lv::wrap, chart.get());

        // Add series
        m_ser1 = m_chart->add_series(lv::rgb(0x2196F3), lv::Chart::Axis::primary_y);  // blue
        m_ser2 = m_chart->add_series(lv::rgb(0xF44336), lv::Chart::Axis::primary_y);  // red

        // Initial data
        generate_sine_data(m_ser1, *m_chart, 0);
        generate_random_data(m_ser2, *m_chart);

        // Axis labels
        lv::Label::create(root)
            .text("Blue: Sine wave | Red: Random");

        // Controls
        auto controls = lv::hbox(root)
            .fill_width()
            .gap(10)
            .align_items(lv::align::center);

        lv::Button::create(controls)
            .text("Start")
            .on_click<&ChartDemo::on_start>(this);

        lv::Button::create(controls)
            .text("Stop")
            .on_click<&ChartDemo::on_stop>(this);

        lv::Button::create(controls)
            .text("Reset")
            .on_click<&ChartDemo::on_reset>(this);

        // Chart type selector
        auto type_row = lv::hbox(root)
            .fill_width()
            .gap(10)
            .align_items(lv::align::center);

        lv::Label::create(type_row).text("Type:");

        lv::Dropdown::create(type_row)
            .options("Line\nBar\nScatter")
            .selected(0)
            .on_value_changed<&ChartDemo::on_type_changed>(this);

        return root;
    }

    ~ChartDemo() {
        delete m_chart;
    }

private:
    void on_start(lv::Event) {
        if (!m_timer) {
            m_timer = std::make_unique<lv::Timer>(timer_cb, 100, this);
        }
    }

    void on_stop(lv::Event) {
        m_timer.reset();
    }

    void on_reset(lv::Event) {
        m_tick = 0;
        generate_sine_data(m_ser1, *m_chart, 0);
        generate_random_data(m_ser2, *m_chart);
        m_chart->refresh();
    }

    void on_type_changed(lv::Event e) {
        auto dropdown = lv::Dropdown(lv::wrap, e.target());
        uint32_t sel = dropdown.selected();

        constexpr lv_chart_type_t types[] = {
            lv::Chart::Type::line,
            lv::Chart::Type::bar,
            lv::Chart::Type::scatter
        };

        if (sel < 3) {
            m_chart->type(types[sel]);
        }
    }

    static void timer_cb(lv_timer_t* timer) {
        auto* self = static_cast<ChartDemo*>(lv::Timer(timer).user_data());
        self->m_tick++;

        // Add new sine point
        int32_t sine_val = static_cast<int32_t>(50 + 40 * std::sin(self->m_tick * 0.3));
        self->m_chart->set_next_value(self->m_ser1, sine_val);

        // Add new random point
        int32_t rand_val = 20 + (std::rand() % 60);
        self->m_chart->set_next_value(self->m_ser2, rand_val);

        self->m_chart->refresh();
    }
};

#endif // LV_USE_OBSERVER

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Chart Demo", 480, 420);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 420);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

#if LV_USE_OBSERVER
    ChartDemo demo;
    demo.mount(lv::screen_active());
#else
    lv::Label::create(lv::screen_active())
        .text("Enable LV_USE_OBSERVER for this demo")
        .center();
#endif

    lv::run();
    return 0;
}
