#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <lv/lv.hpp>

// ============================================================
// Headless LVGL display for runtime tests
// ============================================================

static constexpr int DISP_W = 320;
static constexpr int DISP_H = 240;
static lv_color_t s_buf[DISP_W * 10];
static bool s_lv_initialized = false;
static lv_display_t* s_disp = nullptr;

static void flush_cb(lv_display_t* d, const lv_area_t*, uint8_t*) {
    lv_display_flush_ready(d);
}

struct LvglFixture {
    LvglFixture() {
        if (!s_lv_initialized) {
            lv_init();
            s_disp = lv_display_create(DISP_W, DISP_H);
            lv_display_set_flush_cb(s_disp, flush_cb);
            lv_display_set_buffers(s_disp, s_buf, nullptr, sizeof(s_buf),
                                   LV_DISPLAY_RENDER_MODE_PARTIAL);
            s_lv_initialized = true;
        }
        // Clean screen children between tests
        lv_obj_clean(screen());
    }

    lv_obj_t* screen() { return lv_display_get_screen_active(s_disp); }

    void update_layout() { lv_obj_update_layout(screen()); }
};

// ============================================================
// Zero-cost: sizeof checks
// ============================================================

TEST_CASE("zero-cost: pointer-sized types") {
    CHECK(sizeof(lv::ObjectView) == sizeof(void*));
    CHECK(sizeof(lv::ObjectRef) == sizeof(void*));
    CHECK(sizeof(lv::Label) == sizeof(void*));
    CHECK(sizeof(lv::Button) == sizeof(void*));
    CHECK(sizeof(lv::Slider) == sizeof(void*));
    CHECK(sizeof(lv::Box) == sizeof(void*));
}

// ============================================================
// ObjectRef
// ============================================================

TEST_CASE("ObjectRef: construction") {
    LvglFixture lv;

    SUBCASE("from raw pointer") {
        auto ref = lv::ref(lv.screen());
        CHECK(ref.get() == lv.screen());
    }

    SUBCASE("from ObjectView") {
        lv::ObjectView view(lv.screen());
        lv::ObjectRef ref(view);
        CHECK(ref.get() == lv.screen());
    }

    SUBCASE("null") {
        auto ref = lv::ref(nullptr);
        CHECK(ref.get() == nullptr);
    }
}

TEST_CASE("ObjectRef: fluent API") {
    LvglFixture lv;
    auto box = lv::Box::create(lv.screen());

    SUBCASE("ObjectMixin methods") {
        box.size(100, 50).hide().show();
        lv.update_layout();
        CHECK(box.get_width() == 100);
        CHECK(box.get_height() == 50);
    }

    SUBCASE("StyleMixin methods") {
        box.bg_opa(LV_OPA_COVER).radius(8).padding(10);
        CHECK(box.get_bg_opa() == LV_OPA_COVER);
        CHECK(box.get_radius() == 8);
        CHECK(box.get_pad_top() == 10);
    }

    SUBCASE("shadow styles") {
        box.shadow_width(10).shadow_opa(LV_OPA_50).shadow_spread(5);
        CHECK(box.get_shadow_width() == 10);
        CHECK(box.get_shadow_opa() == LV_OPA_50);
        CHECK(box.get_shadow_spread() == 5);
    }

    SUBCASE("outline styles") {
        box.outline_width(3).outline_opa(LV_OPA_COVER).outline_pad(2);
        CHECK(box.get_outline_width() == 3);
        CHECK(box.get_outline_opa() == LV_OPA_COVER);
        CHECK(box.get_outline_pad() == 2);
    }
}

// ============================================================
// child()/parent() return ObjectRef
// ============================================================

TEST_CASE("child/parent return ObjectRef") {
    LvglFixture lv;
    auto parent = lv::Box::create(lv.screen());
    auto child = lv::Box::create(parent);

    SUBCASE("child() returns ObjectRef with fluent API") {
        parent.child(0).hide();
        CHECK(parent.child(0).has_flag(LV_OBJ_FLAG_HIDDEN));
    }

    SUBCASE("parent() returns ObjectRef with fluent API") {
        child.parent().bg_opa(LV_OPA_COVER);
        CHECK(child.parent().get_bg_opa() == LV_OPA_COVER);
    }

    SUBCASE("chained navigation") {
        parent.child(0).parent().bg_opa(LV_OPA_50);
        CHECK(parent.get_bg_opa() == LV_OPA_50);
    }
}

// ============================================================
// Event::target() returns ObjectRef
// ============================================================

TEST_CASE("Event::target returns ObjectRef") {
    LvglFixture lv;
    auto btn = lv::Button::create(lv.screen());

    // Stateless lambda — verifies target() returns ObjectRef with fluent API
    btn.on_click([](lv::Event e) {
        e.target().bg_opa(LV_OPA_COVER);
    });

    CHECK(btn.get() != nullptr);
}

// ============================================================
// Style getters
// ============================================================

TEST_CASE("style getters match setters") {
    LvglFixture lv;
    auto box = lv::Box::create(lv.screen());

    box.bg_opa(LV_OPA_80);
    CHECK(box.get_bg_opa() == LV_OPA_80);

    box.border_width(3);
    CHECK(box.get_border_width() == 3);

    box.pad_top(15).pad_bottom(20).pad_left(25).pad_right(30);
    CHECK(box.get_pad_top() == 15);
    CHECK(box.get_pad_bottom() == 20);
    CHECK(box.get_pad_left() == 25);
    CHECK(box.get_pad_right() == 30);

    box.radius(12);
    CHECK(box.get_radius() == 12);

    box.opa(LV_OPA_70);
    CHECK(box.get_opa() == LV_OPA_70);

    box.translate_x(10).translate_y(20);
    CHECK(box.get_translate_x() == 10);
    CHECK(box.get_translate_y() == 20);

    box.margin_top(5).margin_bottom(10);
    CHECK(box.get_margin_top() == 5);
    CHECK(box.get_margin_bottom() == 10);

    box.min_width(50).max_width(200);
    CHECK(box.get_min_width() == 50);
    CHECK(box.get_max_width() == 200);
}

TEST_CASE("deprecated aliases forward correctly") {
    LvglFixture lv;
    auto box = lv::Box::create(lv.screen());

    box.opa(LV_OPA_60);
    box.translate_x(42);
    box.translate_y(84);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    CHECK(box.get_style_opa() == LV_OPA_60);
    CHECK(box.get_style_translate_x() == 42);
    CHECK(box.get_style_translate_y() == 84);
#pragma GCC diagnostic pop
}

// ============================================================
// Style with part selector
// ============================================================

TEST_CASE("style getters with part selector") {
    LvglFixture lv;
    auto slider = lv::Slider::create(lv.screen());

    slider.bg_opa(LV_OPA_50, LV_PART_INDICATOR);
    CHECK(slider.get_bg_opa(LV_PART_INDICATOR) == LV_OPA_50);
}

// ============================================================
// Visibility
// ============================================================

TEST_CASE("hide/show/visible") {
    LvglFixture lv;
    auto box = lv::Box::create(lv.screen());

    box.hide();
    CHECK(box.has_flag(LV_OBJ_FLAG_HIDDEN));

    box.show();
    CHECK_FALSE(box.has_flag(LV_OBJ_FLAG_HIDDEN));

    box.visible(false);
    CHECK(box.has_flag(LV_OBJ_FLAG_HIDDEN));

    box.visible(true);
    CHECK_FALSE(box.has_flag(LV_OBJ_FLAG_HIDDEN));
}

// ============================================================
// Flags
// ============================================================

TEST_CASE("add_flag/remove_flag") {
    LvglFixture lv;
    auto box = lv::Box::create(lv.screen());

    box.add_flag(LV_OBJ_FLAG_HIDDEN);
    CHECK(box.has_flag(LV_OBJ_FLAG_HIDDEN));

    box.remove_flag(LV_OBJ_FLAG_HIDDEN);
    CHECK_FALSE(box.has_flag(LV_OBJ_FLAG_HIDDEN));

    box.clickable(false);
    CHECK_FALSE(box.has_flag(LV_OBJ_FLAG_CLICKABLE));

    box.clickable(true);
    CHECK(box.has_flag(LV_OBJ_FLAG_CLICKABLE));
}

// ============================================================
// Grid layout
// ============================================================

#if LV_USE_GRID
TEST_CASE("grid layout methods") {
    LvglFixture lv;
    auto cont = lv::Box::create(lv.screen());

    static const int32_t col_dsc[] = {100, 200, LV_GRID_TEMPLATE_LAST};
    static const int32_t row_dsc[] = {50, 50, LV_GRID_TEMPLATE_LAST};

    cont.grid_dsc(col_dsc, row_dsc);

    auto child = lv::Box::create(cont);
    child.grid_cell(LV_GRID_ALIGN_STRETCH, 0, 1,
                    LV_GRID_ALIGN_STRETCH, 0, 1);

    lv.update_layout();
    CHECK(child.get_width() > 0);
}
#endif

// ============================================================
// Slider/Switch bind with State<T>
// ============================================================

#if LV_USE_OBSERVER
TEST_CASE("Slider::bind(State<int>)") {
    LvglFixture lv;
    auto slider = lv::Slider::create(lv.screen());
    lv::State<int> state{50};

    slider.bind(state);
    CHECK(slider.get() != nullptr);
}

TEST_CASE("Switch::bind(State<bool>)") {
    LvglFixture lv;
    auto sw = lv::Switch::create(lv.screen());
    lv::State<bool> state{false};

    sw.bind(state);
    CHECK(sw.get() != nullptr);
}

TEST_CASE("Label::bind_text(State<int>)") {
    LvglFixture lv;
    auto label = lv::Label::create(lv.screen());
    lv::State<int> state{42};

    label.bind_text(state, "Value: %d");
    CHECK(label.get() != nullptr);
}
#endif

// ============================================================
// Timer
// ============================================================

TEST_CASE("Timer once() disables auto-delete") {
    LvglFixture lv;
    int fire_count = 0;

    {
        lv::Timer timer([](lv_timer_t* t) {
            auto* count = static_cast<int*>(lv_timer_get_user_data(t));
            (*count)++;
        }, 1, &fire_count);
        timer.once();

        // Make the timer ready and fire it
        lv_timer_ready(timer.get());
        lv_tick_inc(2);
        lv_timer_handler();

        // Timer fired
        CHECK(fire_count == 1);
        // Timer still valid (auto-delete disabled, LVGL paused it instead)
        CHECK(timer.get() != nullptr);
    }
    // Timer destructor runs here — would crash if LVGL had already freed it
    CHECK(fire_count == 1);
}

TEST_CASE("TimerRef is copyable") {
    LvglFixture lv;
    lv::Timer timer([](lv_timer_t*) {}, 100);

    auto ref1 = lv::timer_ref(timer);
    auto ref2 = ref1; // copy
    CHECK(ref1.get() == ref2.get());

    ref1.pause();
    CHECK(ref1.is_paused());
    ref2.resume();
    CHECK_FALSE(ref2.is_paused());
}

TEST_CASE("TimerRef from raw pointer") {
    LvglFixture lv;
    lv::Timer timer([](lv_timer_t*) {}, 100);

    auto ref = lv::timer_ref(timer.get());
    CHECK(ref.get() == timer.get());
    ref.period(200);
}

// ============================================================
// lv::ref() free function
// ============================================================

TEST_CASE("lv::ref() wraps raw pointer") {
    LvglFixture lv;
    lv_obj_t* raw = lv.screen();

    auto r = lv::ref(raw);
    CHECK(r.get() == raw);
    CHECK(r.child_count() == lv_obj_get_child_count(raw));
}

// ============================================================
// Component::root() returns ObjectRef
// ============================================================

class TestComponent : public lv::Component<TestComponent> {
public:
    lv::ObjectView build(lv::ObjectView parent) {
        return lv::Box::create(parent).size(100, 50);
    }
};

TEST_CASE("Component::root() returns ObjectRef") {
    LvglFixture lv;
    TestComponent comp;
    comp.mount(lv::ObjectView(lv.screen()));

    comp.root().hide();
    CHECK(comp.root().has_flag(LV_OBJ_FLAG_HIDDEN));

    comp.root().show();
    CHECK_FALSE(comp.root().has_flag(LV_OBJ_FLAG_HIDDEN));
}
