/**
 * @file api_smoke_test.cpp
 * @brief Compile-only API smoke test
 *
 * This file does NOT run — it only needs to compile.
 * It catches overload ambiguities, missing methods, and name
 * collisions across the ObjectView / ObjectMixin / widget hierarchy.
 *
 * Build with: g++ -std=c++20 -fsyntax-only -I include -I ../lvgl -I . tests/api_smoke_test.cpp
 */

#include <lv/lv.hpp>
#include <lv/core/verify.hpp>
#include <lv/core/widget_cast.hpp>
#include <lv/draw/draw_mask.hpp>
#include <lv/draw/draw_task.hpp>
#include <span>
#include <optional>

// ============================================================
// user_data: no ambiguity on widgets or raw ObjectView
// ============================================================

[[maybe_unused]] static void test_user_data() {
    lv::Button btn;
    void* ptr = nullptr;

    // Widget setter (returns Button&, from ObjectMixin)
    btn.user_data(ptr);

    // Widget getter (from ObjectMixin)
    void* a = btn.user_data();

    // Widget typed getter (from ObjectMixin)
    int* b = btn.user_data_as<int>();

    // Raw ObjectView getter (get_ prefix to avoid ambiguity with ObjectMixin)
    lv::ObjectView obj(nullptr);
    void* c = obj.get_user_data();
    int* d = obj.get_user_data<int>();

    (void)a; (void)b; (void)c; (void)d;
}

// ============================================================
// user_data on Grid (regression: was ambiguous after adding ObjectMixin)
// ============================================================

[[maybe_unused]] static void test_grid_user_data() {
    lv::Grid grid(lv::ObjectView(nullptr));
    void* ptr = nullptr;

    grid.user_data(ptr);
    void* a = grid.user_data();
    int* b = grid.user_data_as<int>();

    (void)a; (void)b;
}

// ============================================================
// Inherited ObjectMixin methods on Box, Textarea, Slider, Grid
// (regression: were duplicated, now inherited)
// ============================================================

[[maybe_unused]] static void test_inherited_methods() {
    lv::Box box;
    box.size(100, 50);
    box.width(100);
    box.height(50);
    box.fill();
    box.fill_width();
    box.fill_height();
    box.pos(0, 0);
    box.x(0);
    box.y(0);
    box.center();
    box.grow();
    box.scrollable(false);

    lv::Textarea ta;
    ta.size(200, 100);
    ta.width(200);
    ta.height(100);
    ta.fill_width();

    lv::Slider sl;
    sl.width(200);
    sl.fill_width();

    lv::Grid grid(lv::ObjectView(nullptr));
    grid.size(300, 200);
    grid.width(300);
    grid.height(200);
    grid.size_content();
    grid.fill();
    grid.fill_width();
    grid.fill_height();
}

// ============================================================
// Fluent chaining returns correct type
// ============================================================

[[maybe_unused]] static void test_fluent_chaining() {
    lv::Button btn;

    // Each method returns Button&, allowing further Button-specific calls
    btn.size(100, 50).text("OK").on_click([](lv::Event) {});

    lv::Box box;
    box.fill().bg_color(lv::rgb(0xFF0000)).padding(10);
}

// ============================================================
// Grid uses global lv::wrap_t (not its own)
// ============================================================

[[maybe_unused]] static void test_grid_wrap() {
    lv_obj_t* raw = nullptr;
    lv::Grid grid(lv::wrap, raw);
    (void)grid;
}

// ============================================================
// Event API overloads
// ============================================================

struct Dummy {
    void on_click(lv::Event) {}
    void on_value(lv_event_t*) {}
    void on_simple() {}
};

[[maybe_unused]] static void test_event_overloads() {
    lv::Button btn;
    Dummy d;

    // Stateless lambda (lv_event_t*)
    btn.on(LV_EVENT_CLICKED, [](lv_event_t*) {});

    // Stateless lambda (lv::Event)
    btn.on(LV_EVENT_CLICKED, [](lv::Event) {});

    // Member function (lv::Event)
    btn.on<&Dummy::on_click>(LV_EVENT_CLICKED, &d);

    // Member function (lv_event_t*)
    btn.on<&Dummy::on_value>(LV_EVENT_CLICKED, &d);

    // Member function (void())
    btn.on<&Dummy::on_simple>(LV_EVENT_CLICKED, &d);

    // Convenience shorthands
    btn.on_click([](lv::Event) {});
    btn.on_click<&Dummy::on_click>(&d);
}

// ============================================================
// ObjectView getters: parent().get_width(), child().get_height()
// (regression: getters were only on ObjectMixin, not ObjectView)
// ============================================================

[[maybe_unused]] static void test_objectview_getters() {
    lv::Button btn;

    // Geometry getters on ObjectView returned by parent()/child()
    int32_t w = btn.parent().get_width();
    int32_t h = btn.parent().get_height();
    int32_t cw = btn.child(0).content_width();
    int32_t ch = btn.child(0).content_height();

    // Scroll getters
    int32_t sx = btn.parent().scroll_x();
    int32_t sy = btn.parent().scroll_y();

    // Extended draw size
    int32_t eds = btn.parent().calculate_ext_draw_size();

    // user_data on bare ObjectView
    void* ud = btn.parent().get_user_data();

    (void)w; (void)h; (void)cw; (void)ch;
    (void)sx; (void)sy; (void)eds; (void)ud;
}

// ============================================================
// LVGL 9.5+ gated APIs (regression: must compile when available)
// ============================================================

#if LV_VERSION_AT_LEAST(9, 5, 0)
[[maybe_unused]] static void test_lvgl_9_5_apis() {
    lv::Button btn;

    // kState::alt
    [[maybe_unused]] lv_state_t alt = lv::kState::alt;

    // ObjectMixin::remove_theme
    btn.remove_theme();
    btn.remove_theme(lv::kPart::main);

    // ObjectMixin::radio_button / is_radio_button
    btn.radio_button(true);
    [[maybe_unused]] bool rb = btn.is_radio_button();

    // Display::rotate_point
    lv::Display disp = lv::Display::get_default();
    lv_point_t pt = {10, 20};
    disp.rotate_point(&pt);

    // MaskRectDsc
    lv::MaskRectDsc mask;
    mask.area(0, 0, 100, 100).radius(10).keep_outside(true);
    [[maybe_unused]] auto r = mask.get_radius();

    // DrawTaskView::mask_rect_dsc
    lv::DrawTaskView dtv(nullptr);
    [[maybe_unused]] auto* dsc = dtv.mask_rect_dsc();

    // kChartType::curve
#if LV_USE_CHART
    [[maybe_unused]] auto curve = lv::kChartType::curve;
#endif

    // Theme create/copy/delete
    lv::Theme theme = lv::theme_create();
    lv::Theme theme2 = lv::theme_create();
    lv::theme_copy(theme2, theme);
    lv::theme_delete(theme2);
    lv::theme_delete(theme);

    // ObjectMixin::bind_style_prop (requires LV_USE_OBSERVER)
#if LV_USE_OBSERVER
    {
        lv::Button btn2;
        lv_subject_t subj;
        lv_subject_init_int(&subj, 0);
        [[maybe_unused]] auto* obs = btn2.bind_style_prop(LV_STYLE_WIDTH, lv::kPart::main, &subj);
        lv_subject_deinit(&subj);
    }
#endif

    // ArcLabel overflow and text_angle
#if LV_USE_ARCLABEL
    {
        lv::ArcLabel al = lv::ArcLabel::create(lv::screen_active());
        al.overflow(lv::ArcLabelOverflow::clip);
        [[maybe_unused]] auto ov = al.get_overflow();
        [[maybe_unused]] auto ta = al.get_text_angle();
    }
#endif

    // Indev gesture and key remap
    {
        lv::Indev indev;
        indev.gesture_min_velocity(10);
        indev.gesture_min_distance(20);
        indev.key_remap_cb(nullptr);
    }

    // Group user_data
    {
        lv::Group grp;
        grp.user_data(nullptr);
        [[maybe_unused]] void* ud = grp.user_data();
        [[maybe_unused]] int* typed_ud = grp.user_data_as<int>();
    }
}
#endif

// ============================================================
// widget_cast: type-safe downcasting
// ============================================================

[[maybe_unused]] static void test_widget_cast() {
    lv::Button btn;

    // widget_cast returns std::optional<Widget>
    std::optional<lv::Button> maybe_btn = lv::widget_cast<lv::Button>(btn);
    (void)maybe_btn;

    // widget_cast on null ObjectView returns nullopt
    lv::ObjectView null_obj;
    std::optional<lv::Slider> maybe_slider = lv::widget_cast<lv::Slider>(null_obj);
    (void)maybe_slider;

    // widget_cast to wrong type returns nullopt (verified at runtime)
    std::optional<lv::Label> maybe_label = lv::widget_cast<lv::Label>(btn);
    (void)maybe_label;
}

// ============================================================
// find_parent: walk parent chain for a widget type
// ============================================================

[[maybe_unused]] static void test_find_parent() {
    lv::Button btn;

    // find_parent returns std::optional<Widget>
    std::optional<lv::Tabview> maybe_tv = lv::find_parent<lv::Tabview>(btn);
    (void)maybe_tv;
}

// ============================================================
// for_each_child: safe child iteration
// ============================================================

[[maybe_unused]] static void test_for_each_child() {
    lv::Box box;

    // Lambda-based iteration — compiles with both ObjectView and widget types
    lv::for_each_child(box, [](lv::ObjectView child) {
        [[maybe_unused]] int32_t w = child.get_width();
        (void)w;
    });
}

// ============================================================
// std::span overloads
// ============================================================

[[maybe_unused]] static void test_span_overloads() {
    // Line: span<const lv_point_precise_t>
#if LV_USE_LINE
    {
        lv::Line line;
        lv_point_precise_t pts[] = {{0, 0}, {100, 100}};
        std::span<const lv_point_precise_t> sp(pts);
        line.points(sp);
    }
#endif

    // Chart: span<const int32_t>
#if LV_USE_CHART
    {
        lv::Chart chart;
        lv_chart_series_t* ser = nullptr;
        int32_t vals[] = {10, 20, 30};
        std::span<const int32_t> sv(vals);
        chart.set_series_values(ser, sv);
    }
#endif

    // Calendar: span<lv_calendar_date_t>
#if LV_USE_CALENDAR
    {
        lv::Calendar cal;
        lv_calendar_date_t dates[] = {{2026, 1, 1}, {2026, 12, 25}};
        std::span<lv_calendar_date_t> sd(dates);
        cal.highlighted_dates(sd);
    }
#endif
}

int main() {
    return 0;
}
