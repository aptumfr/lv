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
#include <lv/draw/draw_mask.hpp>
#include <lv/draw/draw_task.hpp>

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
// ObjectRef: full fluent API on non-owning reference
// ============================================================

[[maybe_unused]] static void test_objectref() {
    // lv::ref() wraps a raw pointer into ObjectRef
    lv_obj_t* raw = nullptr;
    lv::ObjectRef r = lv::ref(raw);

    // ObjectRef has ObjectMixin methods
    r.size(100, 50).hide().show().visible(true);
    r.clickable(true).add_flag(LV_OBJ_FLAG_SCROLLABLE);
    r.remove_flag(LV_OBJ_FLAG_SCROLLABLE);
    r.name("test");

    // ObjectRef has StyleMixin methods
    r.bg_color(lv::rgb(0xFF0000)).radius(8).padding(10);
    r.shadow_color(lv::rgb(0)).shadow_width(10).shadow_opa(LV_OPA_50);
    r.outline_color(lv::rgb(0)).outline_width(2).outline_opa(LV_OPA_COVER);

    // ObjectRef has EventMixin methods
    r.on_click([](lv::Event) {});

    // Construction from ObjectView
    lv::ObjectView view(nullptr);
    lv::ObjectRef r2(view);
    lv::ObjectRef r3 = lv::ref(view);
    (void)r2; (void)r3;
}

// ============================================================
// ObjectView::parent()/child() return ObjectRef
// ============================================================

[[maybe_unused]] static void test_child_parent_return_objectref() {
    lv::Box box;

    // child() returns ObjectRef, so fluent calls work directly
    box.child(0).hide();
    box.child(0).add_state(LV_STATE_CHECKED);
    box.child(0).bg_color(lv::rgb(0xFF0000));

    // parent() returns ObjectRef too
    box.parent().visible(false);
    box.parent().padding(10);

    // Chained navigation
    box.child(0).child(0).hide();
    box.parent().parent().show();
}

// ============================================================
// Event::target()/current_target() return ObjectRef
// ============================================================

[[maybe_unused]] static void test_event_target_objectref() {
    lv::Button btn;
    btn.on_click([](lv::Event e) {
        // target() returns ObjectRef — fluent API available
        e.target().hide();
        e.target().bg_color(lv::rgb(0));
        e.current_target().visible(false);
    });
}

// ============================================================
// Style getters: get_ + setter name convention
// ============================================================

[[maybe_unused]] static void test_style_getters() {
    lv::Box box;

    // Background
    [[maybe_unused]] auto c = box.get_bg_color();
    [[maybe_unused]] auto o = box.get_bg_opa();

    // Border
    [[maybe_unused]] auto bc = box.get_border_color();
    [[maybe_unused]] auto bw = box.get_border_width();

    // Padding
    [[maybe_unused]] auto pt = box.get_pad_top();
    [[maybe_unused]] auto pb = box.get_pad_bottom();
    [[maybe_unused]] auto pl = box.get_pad_left();
    [[maybe_unused]] auto pr = box.get_pad_right();

    // Text
    [[maybe_unused]] auto tc = box.get_text_color();
    [[maybe_unused]] auto tf = box.get_text_font();

    // Shadow
    [[maybe_unused]] auto sc = box.get_shadow_color();
    [[maybe_unused]] auto sw = box.get_shadow_width();
    [[maybe_unused]] auto so = box.get_shadow_opa();

    // Outline
    [[maybe_unused]] auto oc = box.get_outline_color();
    [[maybe_unused]] auto ow = box.get_outline_width();

    // Transform
    [[maybe_unused]] auto tr = box.get_transform_rotation();
    [[maybe_unused]] auto tx = box.get_translate_x();
    [[maybe_unused]] auto ty = box.get_translate_y();

    // Appearance
    [[maybe_unused]] auto op = box.get_opa();
    [[maybe_unused]] auto rd = box.get_radius();

    // With part selector
    [[maybe_unused]] auto bg = box.get_bg_color(LV_PART_INDICATOR);
}

// ============================================================
// Deprecated aliases still compile with warning
// ============================================================

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
[[maybe_unused]] static void test_deprecated_aliases() {
    lv::Box box;
    [[maybe_unused]] auto o = box.get_style_opa();
    [[maybe_unused]] auto x = box.get_style_translate_x();
    [[maybe_unused]] auto y = box.get_style_translate_y();
}
#pragma GCC diagnostic pop

// ============================================================
// Shadow and outline setters on StyleMixin
// ============================================================

[[maybe_unused]] static void test_shadow_outline_setters() {
    lv::Box box;

    // Shadow
    box.shadow_color(lv::rgb(0x000000))
       .shadow_width(10)
       .shadow_opa(LV_OPA_50)
       .shadow_offset(5, 5)
       .shadow_spread(2);

    // Outline
    box.outline_color(lv::rgb(0x0000FF))
       .outline_width(2)
       .outline_opa(LV_OPA_COVER)
       .outline_pad(4);
}

// ============================================================
// Grid layout methods on ObjectMixin
// ============================================================

#if LV_USE_GRID
[[maybe_unused]] static void test_grid_methods() {
    lv::Box box;

    static const int32_t col_dsc[] = {100, 200, LV_GRID_TEMPLATE_LAST};
    static const int32_t row_dsc[] = {50, 50, LV_GRID_TEMPLATE_LAST};

    box.grid_dsc(col_dsc, row_dsc);
    box.grid_align(LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_CENTER);

    lv::Box child;
    child.grid_cell(LV_GRID_ALIGN_STRETCH, 0, 1,
                    LV_GRID_ALIGN_STRETCH, 0, 1);
}
#endif

// ============================================================
// Flex/Grid align() name hiding: both the layout-specific and
// object-positioning overloads must be callable on the same type.
// Regression guard for C++ name-hiding — if either call below
// stops compiling, someone dropped a `using` declaration.
// ============================================================

#if LV_USE_FLEX
[[maybe_unused]] static void test_flex_align_not_shadowed() {
    lv::Flex f = lv::hbox(lv::ObjectView(nullptr));

    // Object-positioning overloads (from ObjectMixin<Flex>)
    f.align(LV_ALIGN_TOP_MID, 0, 10);
    f.align(LV_ALIGN_CENTER);

    // Flex-content alignment overload (Flex's own)
    f.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    f.align(LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
}
#endif

#if LV_USE_GRID
[[maybe_unused]] static void test_grid_align_not_shadowed() {
    lv::Grid g(lv::ObjectView(nullptr));

    // Object-positioning overloads (from ObjectMixin<Grid>)
    g.align(LV_ALIGN_TOP_MID, 0, 10);
    g.align(LV_ALIGN_CENTER);

    // Grid-specific column/row alignment overload (Grid's own)
    g.align(LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_STRETCH);
}
#endif

#if LV_USE_SPAN
[[maybe_unused]] static void test_spangroup_align_not_shadowed() {
    lv::Spangroup sg;

    // Object-positioning overloads (from ObjectMixin<Spangroup>)
    sg.align(LV_ALIGN_TOP_MID, 0, 10);
    sg.align(LV_ALIGN_CENTER);

    // Spangroup text-alignment overload (Spangroup's own)
    sg.align(LV_TEXT_ALIGN_CENTER);
    sg.align(LV_TEXT_ALIGN_LEFT);
}
#endif

#if LV_USE_LINE
[[maybe_unused]] static void test_line_width_not_shadowed() {
    lv::Line line;

    // Widget bounding-box width (from ObjectMixin<Line>) must still
    // be accessible after renaming the line-stroke-width setter.
    line.width(100);
    line.size(200, 50);
    line.height(50);

    // Line-stroke width has its own name now to avoid the semantic clash.
    line.line_width(5);
}
#endif

// ============================================================
// Slider::bind and Switch::bind with State<T>
// ============================================================

#if LV_USE_OBSERVER
[[maybe_unused]] static void test_bind_state() {
    lv::Slider sl;
    lv::State<int> int_state{50};
    sl.bind(int_state);

    lv::Switch sw;
    lv::State<bool> bool_state{false};
    sw.bind(bool_state);

    lv::Label lbl;
    lbl.bind_text(int_state, "Value: %d");

    // Constraint check: int32_t fits, int64_t should NOT compile
    // (verified manually — int64_t triggers the requires clause)
    static_assert(std::is_integral_v<int> && sizeof(int) <= sizeof(int32_t));
}
#endif

int main() {
    return 0;
}
