#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <lv/lv.hpp>
#include <string>

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
            lv::init();
            s_disp = lv::Display::create(DISP_W, DISP_H)
                         .flush_cb(flush_cb)
                         .buffers(s_buf, nullptr, sizeof(s_buf),
                                  LV_DISPLAY_RENDER_MODE_PARTIAL)
                         .get();
            s_lv_initialized = true;
        }
        // Clean screen children between tests
        lv::ObjectView(screen()).clean();
    }

    lv_obj_t* screen() { return lv::Display(s_disp).screen_active().get(); }

    void update_layout() { lv::ref(screen()).update_layout(); }
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

TEST_CASE("get_x/get_y and layout-vs-style dichotomy") {
    LvglFixture lv;
    auto box = lv::Box::create(lv.screen()).size(50, 30);

    // Set position via the fluent style setter
    box.pos(75, 42);

    // Style getters reflect the set value immediately
    CHECK(box.get_style_x() == 75);
    CHECK(box.get_style_y() == 42);
    CHECK(box.get_style_width() == 50);
    CHECK(box.get_style_height() == 30);

    // Layout-resolved getters require a layout pass
    lv.update_layout();
    CHECK(box.get_x() == 75);
    CHECK(box.get_y() == 42);
    CHECK(box.get_width() == 50);
    CHECK(box.get_height() == 30);
}

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
            lv::Timer wrapped(t); // non-owning wrap
            auto* count = static_cast<int*>(wrapped.user_data());
            (*count)++;
        }, 1, &fire_count);
        timer.once();

        // Make the timer ready and fire it
        timer.ready();
        lv::tick_inc(2);
        lv::timer_handler();

        // Timer fired
        CHECK(fire_count == 1);
        // Timer still valid (auto-delete disabled, LVGL paused it instead)
        CHECK(timer.get() != nullptr);
    }
    // Timer destructor runs here �� would crash if LVGL had already freed it
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

TEST_CASE("const TimerRef mutating methods") {
    LvglFixture lv;
    lv::Timer timer([](lv_timer_t*) {}, 100);

    const auto ref = lv::timer_ref(timer);
    ref.pause();
    CHECK(ref.is_paused());
    ref.resume();
    CHECK_FALSE(ref.is_paused());
    ref.period(50);
    ref.ready();
    ref.reset();
}

// ============================================================
// lv::ref() free function
// ============================================================

TEST_CASE("lv::ref() wraps raw pointer") {
    LvglFixture lv;
    auto scr = lv::ObjectView(lv.screen());

    auto r = lv::ref(scr.get());
    CHECK(r.get() == scr.get());
    CHECK(r.child_count() == scr.child_count());
}

// ============================================================
// Component::root() returns ObjectRef
// ============================================================

class TestComponent : public lv::Component<TestComponent> {
public:
    bool mount_called = false;
    bool unmount_called = false;

    lv::ObjectView build(lv::ObjectView parent) {
        return lv::Box::create(parent).size(100, 50);
    }

    void on_mount() { mount_called = true; }
    void on_unmount() { unmount_called = true; }
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

// ============================================================
// Phase 1 — Core lifecycle & ownership
// ============================================================

TEST_CASE("Object RAII") {
    LvglFixture lv;

    SUBCASE("constructor creates, destructor deletes") {
        uint32_t before = lv::ObjectView(lv.screen()).child_count();
        {
            lv::Object obj(lv.screen());
            CHECK(obj.get() != nullptr);
            CHECK(lv::ObjectView(lv.screen()).child_count() == before + 1);
        }
        CHECK(lv::ObjectView(lv.screen()).child_count() == before);
    }

    SUBCASE("move constructor transfers ownership") {
        lv::Object a(lv.screen());
        lv_obj_t* raw = a.get();
        lv::Object b(std::move(a));
        CHECK(b.get() == raw);
        CHECK(a.get() == nullptr);
    }

    SUBCASE("move assignment transfers ownership") {
        lv::Object a(lv.screen());
        lv::Object b(lv.screen());
        lv_obj_t* raw_a = a.get();
        uint32_t before = lv::ObjectView(lv.screen()).child_count();
        b = std::move(a);
        CHECK(b.get() == raw_a);
        CHECK(a.get() == nullptr);
        // old b was deleted
        CHECK(lv::ObjectView(lv.screen()).child_count() == before - 1);
    }

    SUBCASE("release surrenders ownership") {
        uint32_t before = lv::ObjectView(lv.screen()).child_count();
        lv::ObjectView released(nullptr);
        {
            lv::Object obj(lv.screen());
            released = lv::ObjectView(obj.release());
            CHECK(obj.get() == nullptr);
        }
        // Object was NOT deleted because ownership was released
        CHECK(lv::ObjectView(lv.screen()).child_count() == before + 1);
        released.del(); // manual cleanup
    }

    SUBCASE("reset replaces managed object") {
        lv::Object obj(lv.screen());
        lv::Object second(lv.screen());
        auto* second_raw = second.release();
        obj.reset(second_raw);
        CHECK(obj.get() == second_raw);
        // first was deleted by reset
    }
}

TEST_CASE("Component lifecycle") {
    LvglFixture lv;

    SUBCASE("mount/unmount/is_mounted") {
        TestComponent comp;
        CHECK_FALSE(comp.is_mounted());

        comp.mount(lv::ObjectView(lv.screen()));
        CHECK(comp.is_mounted());
        CHECK(comp.mount_called);
        CHECK(lv::ObjectView(lv.screen()).child_count() == 1);

        comp.unmount();
        CHECK_FALSE(comp.is_mounted());
        CHECK(comp.unmount_called);
        CHECK(lv::ObjectView(lv.screen()).child_count() == 0);
    }

    SUBCASE("destructor auto-unmounts") {
        {
            TestComponent comp;
            comp.mount(lv::ObjectView(lv.screen()));
            CHECK(lv::ObjectView(lv.screen()).child_count() == 1);
        }
        CHECK(lv::ObjectView(lv.screen()).child_count() == 0);
    }

    SUBCASE("external deletion tracked") {
        TestComponent comp;
        comp.mount(lv::ObjectView(lv.screen()));
        comp.root().del(); // simulate external deletion
        CHECK_FALSE(comp.is_mounted());
    }

    SUBCASE("from_obj recovers component pointer") {
        TestComponent comp;
        comp.mount(lv::ObjectView(lv.screen()));
        auto* recovered = TestComponent::from_obj(comp.root());
        CHECK(recovered == &comp);
    }

    SUBCASE("remount replaces old root") {
        TestComponent comp;
        comp.mount(lv::ObjectView(lv.screen()));
        CHECK(lv::ObjectView(lv.screen()).child_count() == 1);
        comp.mount(lv::ObjectView(lv.screen())); // remount
        CHECK(lv::ObjectView(lv.screen()).child_count() == 1);
        CHECK(comp.is_mounted());
    }

    SUBCASE("hide/show/visible") {
        TestComponent comp;
        comp.mount(lv::ObjectView(lv.screen()));
        CHECK(comp.is_visible());
        comp.hide();
        CHECK_FALSE(comp.is_visible());
        comp.show();
        CHECK(comp.is_visible());
        comp.visible(false);
        CHECK_FALSE(comp.is_visible());
    }
}

class TestScreenComp : public lv::ScreenComponent<TestScreenComp> {
public:
    lv::ObjectView build(lv::ObjectView parent) {
        return lv::Box::create(parent).size(100, 50);
    }
};

TEST_CASE("ScreenComponent") {
    LvglFixture lv;
    auto original_screen = lv::screen_active();

    SUBCASE("mount_screen creates screen + mounts") {
        TestScreenComp comp;
        auto scr = comp.mount_screen();
        CHECK(scr.get() != nullptr);
        CHECK(comp.is_mounted());
        CHECK(comp.screen().get() != nullptr);
        CHECK(comp.screen().get() != original_screen.get());
        // Restore
        lv::screen_load(original_screen);
    }

    SUBCASE("unmount_screen cleans both") {
        TestScreenComp comp;
        comp.mount_screen();
        comp.unmount_screen();
        CHECK_FALSE(comp.is_mounted());
        CHECK(comp.screen().get() == nullptr);
        lv::screen_load(original_screen);
    }

    SUBCASE("mount_and_load activates screen") {
        TestScreenComp comp;
        comp.mount_and_load();
        CHECK(comp.is_mounted());
        CHECK(lv::screen_active().get() == comp.screen().get());
        lv::screen_load(original_screen);
    }
}

// ============================================================
// Phase 1 — State<T> & observers
// ============================================================

#if LV_USE_OBSERVER
TEST_CASE("State<int> set/get round-trip") {
    LvglFixture lv;
    lv::State<int> state{42};

    CHECK(state.get() == 42);
    state.set(100);
    CHECK(state.get() == 100);
}

TEST_CASE("State<bool> set/get round-trip") {
    LvglFixture lv;
    lv::State<bool> state{false};

    CHECK(state.get() == false);
    state.set(true);
    CHECK(state.get() == true);
}

TEST_CASE("State<int> increment/decrement operators") {
    LvglFixture lv;
    lv::State<int> state{10};

    ++state;
    CHECK(state.get() == 11);

    --state;
    CHECK(state.get() == 10);

    state += 5;
    CHECK(state.get() == 15);

    state -= 3;
    CHECK(state.get() == 12);

    state.increment(10);
    CHECK(state.get() == 22);

    state.decrement(2);
    CHECK(state.get() == 20);
}

struct ObserverHelper {
    int last_value = -1;
    void on_change(int v) { last_value = v; }
};

TEST_CASE("State observer fires on change") {
    LvglFixture lv;
    lv::State<int> state{0};
    ObserverHelper helper;

    state.observe<&ObserverHelper::on_change>(&helper);
    // Observer fires immediately with initial value
    CHECK(helper.last_value == 0);

    state.set(42);
    CHECK(helper.last_value == 42);

    state.set(99);
    CHECK(helper.last_value == 99);
}

TEST_CASE("observe_obj auto-removes when widget deleted") {
    LvglFixture lv;
    lv::State<int> state{0};
    int observed = -1;

    auto box = lv::Box::create(lv.screen());
    state.observe_obj([](lv_observer_t* obs, lv_subject_t* subj) {
        auto* p = lv::observer_get_user_data<int>(obs);
        *p = lv::subject_get_int(subj);
    }, box, &observed);

    state.set(10);
    CHECK(observed == 10);

    // Delete the widget — observer should be removed
    box.del();
    state.set(20);
    // observed should NOT have been updated to 20
    CHECK(observed == 10);
}
#endif // LV_USE_OBSERVER

// ============================================================
// Phase 2 — Events
// ============================================================

TEST_CASE("Event: stateless lambda with lv_event_t*") {
    LvglFixture lv;
    auto btn = lv::Button::create(lv.screen());
    int result = 0;

    btn.on_raw(LV_EVENT_CLICKED, [](lv_event_t* e) {
        lv::Event evt(e);
        auto* r = evt.user_data_as<int>();
        *r = 1;
    }, &result);

    btn.send_event(LV_EVENT_CLICKED);
    CHECK(result == 1);
}

TEST_CASE("Event: stateless lambda with lv::Event") {
    LvglFixture lv;
    auto btn = lv::Button::create(lv.screen());

    btn.on_click([](lv::Event e) {
        e.target().bg_opa(LV_OPA_50);
    });

    btn.send_event(LV_EVENT_CLICKED);
    CHECK(btn.get_bg_opa() == LV_OPA_50);
}

struct EventTestHelper {
    int click_count = 0;
    lv_event_code_t last_code = LV_EVENT_ALL;

    void on_click_event(lv::Event e) {
        click_count++;
        last_code = e.code();
    }

    void on_click_raw(lv_event_t*) {
        click_count++;
    }

    void on_click_simple() {
        click_count++;
    }
};

TEST_CASE("Event: member function pointer") {
    LvglFixture lv;
    EventTestHelper helper;

    SUBCASE("member fn with lv::Event") {
        auto btn = lv::Button::create(lv.screen());
        btn.on<&EventTestHelper::on_click_event>(LV_EVENT_CLICKED, &helper);
        btn.send_event(LV_EVENT_CLICKED);
        CHECK(helper.click_count == 1);
        CHECK(helper.last_code == LV_EVENT_CLICKED);
    }

    SUBCASE("member fn with lv_event_t*") {
        auto btn = lv::Button::create(lv.screen());
        btn.on<&EventTestHelper::on_click_raw>(LV_EVENT_CLICKED, &helper);
        btn.send_event(LV_EVENT_CLICKED);
        CHECK(helper.click_count == 1);
    }

    SUBCASE("member fn with void()") {
        auto btn = lv::Button::create(lv.screen());
        btn.on<&EventTestHelper::on_click_simple>(LV_EVENT_CLICKED, &helper);
        btn.send_event(LV_EVENT_CLICKED);
        CHECK(helper.click_count == 1);
    }
}

TEST_CASE("Event: on_click/on_value_changed convenience") {
    LvglFixture lv;

    SUBCASE("on_click fires") {
        auto btn = lv::Button::create(lv.screen());
        int result = 0;
        btn.on_raw(LV_EVENT_CLICKED, [](lv_event_t* e) {
            lv::Event evt(e);
            *evt.user_data_as<int>() = 42;
        }, &result);
        btn.send_event(LV_EVENT_CLICKED);
        CHECK(result == 42);
    }

    SUBCASE("on_value_changed fires") {
        auto slider = lv::Slider::create(lv.screen());
        int result = 0;
        slider.on_raw(LV_EVENT_VALUE_CHANGED, [](lv_event_t* e) {
            lv::Event evt(e);
            *evt.user_data_as<int>() = 99;
        }, &result);
        slider.send_event(LV_EVENT_VALUE_CHANGED);
        CHECK(result == 99);
    }
}

TEST_CASE("Event: target matches widget") {
    LvglFixture lv;
    auto btn = lv::Button::create(lv.screen());
    bool target_matched = false;

    btn.on_raw(LV_EVENT_CLICKED, [](lv_event_t* e) {
        lv::Event evt(e);
        *evt.user_data_as<bool>() = (evt.target().get() == evt.current_target().get());
    }, &target_matched);

    btn.send_event(LV_EVENT_CLICKED);
    CHECK(target_matched);
}

// ============================================================
// Phase 3 — Widget creation
// ============================================================

#if LV_USE_LABEL
TEST_CASE("Label widget") {
    LvglFixture lv;

    SUBCASE("text/get_text") {
        auto label = lv::Label::create(lv.screen());
        label.text("Hello World");
        CHECK(std::string(label.get_text()) == "Hello World");
    }

    SUBCASE("text_static") {
        auto label = lv::Label::create(lv.screen());
        static const char* txt = "Static Text";
        label.text_static(txt);
        CHECK(std::string(label.get_text()) == "Static Text");
    }

    SUBCASE("text_fmt") {
        auto label = lv::Label::create(lv.screen());
        label.text_fmt("Value: %d", 42);
        CHECK(std::string(label.get_text()) == "Value: 42");
    }

    SUBCASE("long_mode") {
        auto label = lv::Label::create(lv.screen());
        label.text("test").text_wrap();
        CHECK(label.get() != nullptr);
    }
}
#endif

#if LV_USE_BUTTON
TEST_CASE("Button widget") {
    LvglFixture lv;

#if LV_USE_LABEL
    SUBCASE("text creates child label") {
        auto btn = lv::Button::create(lv.screen());
        btn.text("Click");
        CHECK(btn.get_label().get() != nullptr);
        CHECK(std::string(btn.get_label().get_text()) == "Click");
    }
#endif

    SUBCASE("checkable/toggle") {
        auto btn = lv::Button::create(lv.screen());
        btn.checkable();
        CHECK(btn.has_flag(LV_OBJ_FLAG_CHECKABLE));

        CHECK_FALSE(btn.checked());
        btn.toggle();
        CHECK(btn.checked());
        btn.toggle();
        CHECK_FALSE(btn.checked());
        btn.checked(true);
        CHECK(btn.checked());
    }

    SUBCASE("disabled/enabled") {
        auto btn = lv::Button::create(lv.screen());
        btn.disabled();
        CHECK(btn.has_state(LV_STATE_DISABLED));
        btn.enabled();
        CHECK_FALSE(btn.has_state(LV_STATE_DISABLED));
    }
}
#endif

#if LV_USE_SLIDER
TEST_CASE("Slider widget") {
    LvglFixture lv;

    SUBCASE("value") {
        auto slider = lv::Slider::create(lv.screen());
        slider.value(75);
        CHECK(slider.value() == 75);
    }

    SUBCASE("range") {
        auto slider = lv::Slider::create(lv.screen());
        slider.range(10, 200);
        CHECK(slider.min() == 10);
        CHECK(slider.max() == 200);
    }

    SUBCASE("mode symmetrical") {
        auto slider = lv::Slider::create(lv.screen());
        slider.mode_symmetrical();
        slider.range(-50, 50);
        slider.value(25);
        CHECK(slider.value() == 25);
    }

    SUBCASE("mode range") {
        auto slider = lv::Slider::create(lv.screen());
        slider.mode_range();
        // Must set main value before left_value — LVGL clamps left ≤ value
        slider.value(80);
        slider.left_value(20);
        CHECK(slider.left_value() == 20);
        CHECK(slider.value() == 80);
    }
}
#endif

#if LV_USE_SWITCH
TEST_CASE("Switch widget") {
    LvglFixture lv;

    SUBCASE("on/off/toggle") {
        auto sw = lv::Switch::create(lv.screen());
        CHECK_FALSE(sw.is_on());
        sw.on();
        CHECK(sw.is_on());
        sw.off();
        CHECK_FALSE(sw.is_on());
        sw.toggle();
        CHECK(sw.is_on());
        sw.toggle();
        CHECK_FALSE(sw.is_on());
    }

    SUBCASE("orientation") {
        auto sw = lv::Switch::create(lv.screen());
        sw.horizontal();
        CHECK(sw.get_orientation() == LV_SWITCH_ORIENTATION_HORIZONTAL);
        sw.vertical();
        CHECK(sw.get_orientation() == LV_SWITCH_ORIENTATION_VERTICAL);
        sw.orientation_auto();
        CHECK(sw.get_orientation() == LV_SWITCH_ORIENTATION_AUTO);
    }
}
#endif

#if LV_USE_CHECKBOX
TEST_CASE("Checkbox widget") {
    LvglFixture lv;

    SUBCASE("text/get_text") {
        auto cb = lv::Checkbox::create(lv.screen());
        cb.text("Accept");
        CHECK(std::string(cb.get_text()) == "Accept");
    }

    SUBCASE("checked/unchecked/toggle") {
        auto cb = lv::Checkbox::create(lv.screen());
        CHECK_FALSE(cb.is_checked());
        cb.checked();
        CHECK(cb.is_checked());
        cb.unchecked();
        CHECK_FALSE(cb.is_checked());
        cb.toggle();
        CHECK(cb.is_checked());
    }
}
#endif

#if LV_USE_DROPDOWN
TEST_CASE("Dropdown widget") {
    LvglFixture lv;

    SUBCASE("options/count") {
        auto dd = lv::Dropdown::create(lv.screen());
        dd.options("Alpha\nBeta\nGamma");
        CHECK(dd.option_count() == 3);
    }

    SUBCASE("selected") {
        auto dd = lv::Dropdown::create(lv.screen());
        dd.options("A\nB\nC");
        dd.selected(2);
        CHECK(dd.selected() == 2);
    }

    SUBCASE("add/clear") {
        auto dd = lv::Dropdown::create(lv.screen());
        dd.clear_options();
        CHECK(dd.option_count() == 0);
        dd.add_option("First");
        dd.add_option("Second");
        CHECK(dd.option_count() == 2);
    }

    SUBCASE("open/close") {
        auto dd = lv::Dropdown::create(lv.screen());
        dd.options("A\nB");
        dd.open();
        CHECK(dd.is_open());
        dd.close();
        CHECK_FALSE(dd.is_open());
    }
}
#endif

#if LV_USE_ARC
TEST_CASE("Arc widget") {
    LvglFixture lv;

    SUBCASE("value/range") {
        auto arc = lv::Arc::create(lv.screen());
        arc.range(0, 200);
        arc.value(150);
        CHECK(arc.value() == 150);
        CHECK(arc.min_value() == 0);
        CHECK(arc.max_value() == 200);
    }

    SUBCASE("mode") {
        auto arc = lv::Arc::create(lv.screen());
        arc.mode_normal();
        CHECK(arc.get_mode() == LV_ARC_MODE_NORMAL);
        arc.mode_reverse();
        CHECK(arc.get_mode() == LV_ARC_MODE_REVERSE);
        arc.mode_symmetrical();
        CHECK(arc.get_mode() == LV_ARC_MODE_SYMMETRICAL);
    }

    SUBCASE("rotation") {
        auto arc = lv::Arc::create(lv.screen());
        arc.rotation(90);
        CHECK(arc.rotation() == 90);
    }
}
#endif

#if LV_USE_BAR
TEST_CASE("Bar widget") {
    LvglFixture lv;

    SUBCASE("value") {
        auto bar = lv::Bar::create(lv.screen());
        bar.value(60);
        CHECK(bar.value() == 60);
    }

    SUBCASE("range") {
        auto bar = lv::Bar::create(lv.screen());
        bar.range(10, 500);
        CHECK(bar.min_value() == 10);
        CHECK(bar.max_value() == 500);
    }

    SUBCASE("mode") {
        auto bar = lv::Bar::create(lv.screen());
        bar.mode_normal();
        CHECK(bar.get_mode() == LV_BAR_MODE_NORMAL);
        bar.mode_symmetrical();
        CHECK(bar.get_mode() == LV_BAR_MODE_SYMMETRICAL);
        bar.mode_range();
        CHECK(bar.get_mode() == LV_BAR_MODE_RANGE);
    }
}
#endif

#if LV_USE_LIST
TEST_CASE("List widget") {
    LvglFixture lv;

    SUBCASE("add_text") {
        auto list = lv::List::create(lv.screen());
        auto text_item = list.add_text("Header");
        CHECK(text_item.get() != nullptr);
    }

    SUBCASE("add_button and button_text") {
        auto list = lv::List::create(lv.screen());
        auto btn = list.add_button("Item 1");
        CHECK(btn.get() != nullptr);
        CHECK(std::string(lv::List::button_text(btn.get())) == "Item 1");
    }
}
#endif

#if LV_USE_TABVIEW
TEST_CASE("Tabview widget") {
    LvglFixture lv;

    SUBCASE("add_tab/count") {
        auto tv = lv::Tabview::create(lv.screen());
        auto tab1 = tv.add_tab("Tab 1");
        auto tab2 = tv.add_tab("Tab 2");
        CHECK(tv.tab_count() == 2);
        CHECK(tab1.get() != nullptr);
        CHECK(tab2.get() != nullptr);
    }

    SUBCASE("active tab") {
        auto tv = lv::Tabview::create(lv.screen());
        (void)tv.add_tab("A");
        (void)tv.add_tab("B");
        (void)tv.add_tab("C");
        tv.active(1);
        CHECK(tv.active() == 1);
        tv.active(2);
        CHECK(tv.active() == 2);
    }

    SUBCASE("tab_bar/content access") {
        auto tv = lv::Tabview::create(lv.screen());
        (void)tv.add_tab("Tab");
        CHECK(tv.tab_bar().get() != nullptr);
        CHECK(tv.content().get() != nullptr);
    }
}
#endif

#if LV_USE_MSGBOX
TEST_CASE("Msgbox widget") {
    LvglFixture lv;

    SUBCASE("add_title/text/footer_button") {
        auto mb = lv::Msgbox::create(lv.screen());
        auto title = mb.add_title("Alert");
        auto text = mb.add_text("Something happened");
        auto btn = mb.add_footer_button("OK");
        CHECK(title.get() != nullptr);
        CHECK(text.get() != nullptr);
        CHECK(btn.get() != nullptr);
    }

    SUBCASE("header/footer/content") {
        auto mb = lv::Msgbox::create(lv.screen());
        (void)mb.add_title("Title");
        (void)mb.add_text("Body");
        (void)mb.add_footer_button("OK");
        CHECK(mb.header().get() != nullptr);
        CHECK(mb.footer().get() != nullptr);
        CHECK(mb.content().get() != nullptr);
    }
}
#endif

#if LV_USE_TILEVIEW
TEST_CASE("Tileview widget") {
    LvglFixture lv;

    SUBCASE("add_tile") {
        auto tv = lv::Tileview::create(lv.screen());
        auto tile0 = tv.add_tile(0, 0, LV_DIR_RIGHT);
        auto tile1 = tv.add_tile(1, 0, LV_DIR_LEFT);
        CHECK(tile0.get() != nullptr);
        CHECK(tile1.get() != nullptr);
    }

    SUBCASE("active_tile") {
        auto tv = lv::Tileview::create(lv.screen()).size(200, 200);
        auto tile0 = tv.add_tile(0, 0, LV_DIR_RIGHT);
        (void)tv.add_tile(1, 0, LV_DIR_LEFT);
        // Set the active tile explicitly
        tv.active_tile(tile0.get());
        lv.update_layout();
        auto active = tv.active_tile();
        CHECK(active.get() == tile0.get());
    }
}
#endif

#if LV_USE_WIN
TEST_CASE("Window widget") {
    LvglFixture lv;

    SUBCASE("header/content") {
        auto win = lv::Window::create(lv.screen());
        CHECK(win.header().get() != nullptr);
        CHECK(win.content().get() != nullptr);
    }

    SUBCASE("add_title") {
        auto win = lv::Window::create(lv.screen());
        auto title = win.add_title("My Window");
        CHECK(title.get() != nullptr);
    }
}
#endif

// ============================================================
// Phase 4 — Layout
// ============================================================

#if LV_USE_FLEX
TEST_CASE("Flex: hbox/vbox") {
    LvglFixture lv;

    SUBCASE("hbox children laid out horizontally") {
        auto row = lv::hbox(lv::ObjectView(lv.screen())).size(200, 50).gap(0).padding(0);
        auto c1 = lv::Box::create(row).size(40, 40);
        auto c2 = lv::Box::create(row).size(40, 40);
        lv.update_layout();

        lv_area_t a1, a2;
        c1.get_coords(&a1);
        c2.get_coords(&a2);
        // c2 should be to the right of c1
        CHECK(a2.x1 >= a1.x2);
    }

    SUBCASE("vbox children laid out vertically") {
        auto col = lv::vbox(lv::ObjectView(lv.screen())).size(100, 200).gap(0).padding(0);
        auto c1 = lv::Box::create(col).size(40, 40);
        auto c2 = lv::Box::create(col).size(40, 40);
        lv.update_layout();

        lv_area_t a1, a2;
        c1.get_coords(&a1);
        c2.get_coords(&a2);
        // c2 should be below c1
        CHECK(a2.y1 >= a1.y2);
    }

    SUBCASE("gap spacing") {
        auto row = lv::hbox(lv::ObjectView(lv.screen())).size(300, 50).gap(20).padding(0);
        auto c1 = lv::Box::create(row).size(40, 40);
        auto c2 = lv::Box::create(row).size(40, 40);
        lv.update_layout();

        lv_area_t a1, a2;
        c1.get_coords(&a1);
        c2.get_coords(&a2);
        int32_t spacing = a2.x1 - a1.x2;
        CHECK(spacing >= 20);
    }

    SUBCASE("grow distribution") {
        auto row = lv::hbox(lv::ObjectView(lv.screen())).size(200, 50).gap(0).padding(0);
        auto c1 = lv::Box::create(row).height(40).grow(1);
        auto c2 = lv::Box::create(row).height(40).grow(1);
        lv.update_layout();

        CHECK(c1.get_width() > 0);
        CHECK(c2.get_width() > 0);
        // Both should get approximately equal width
        int32_t diff = c1.get_width() - c2.get_width();
        CHECK(((diff >= -2) && (diff <= 2)));
    }

    SUBCASE("row_wrap wraps children") {
        auto row = lv::hbox_wrap(lv::ObjectView(lv.screen())).size(100, 200).gap(0).padding(0);
        // 3 children of 40px in 100px container => wrap
        auto c1 = lv::Box::create(row).size(40, 40);
        auto c2 = lv::Box::create(row).size(40, 40);
        auto c3 = lv::Box::create(row).size(40, 40);
        lv.update_layout();

        lv_area_t a1, a3;
        c1.get_coords(&a1);
        c3.get_coords(&a3);
        // c3 should have wrapped to next line
        CHECK(a3.y1 >= a1.y2);
        (void)c2;
    }
}
#endif

#if LV_USE_GRID
TEST_CASE("Grid extended") {
    LvglFixture lv;

    SUBCASE("grid factory, col_span, gap") {
        static const int32_t col_dsc[] = {80, 80, 80, LV_GRID_TEMPLATE_LAST};
        static const int32_t row_dsc[] = {40, 40, LV_GRID_TEMPLATE_LAST};

        auto grid = lv::Box::create(lv.screen());
        grid.grid_dsc(col_dsc, row_dsc).size(300, 120);

        // Child spanning 2 columns
        auto child = lv::Box::create(grid);
        child.grid_cell(LV_GRID_ALIGN_STRETCH, 0, 2,
                        LV_GRID_ALIGN_STRETCH, 0, 1);

        lv.update_layout();
        CHECK(child.get_width() > 80); // Should span 2 columns
    }
}
#endif

// ============================================================
// Phase 5 — Style & Animation
// ============================================================

TEST_CASE("Style RAII") {
    LvglFixture lv;

    SUBCASE("create, set properties, apply to widget, verify") {
        lv::Style style;
        style.bg_opa(LV_OPA_COVER).radius(10).pad_all(5);

        auto box = lv::Box::create(lv.screen());
        box.add_style(style.get());

        CHECK(box.get_bg_opa() == LV_OPA_COVER);
        CHECK(box.get_radius() == 10);
        CHECK(box.get_pad_top() == 5);
    }

    SUBCASE("move semantics") {
        lv::Style a;
        a.bg_opa(LV_OPA_80);

        lv::Style b(std::move(a));
        auto box = lv::Box::create(lv.screen());
        box.add_style(b.get());
        CHECK(box.get_bg_opa() == LV_OPA_80);
    }

    SUBCASE("reset clears") {
        lv::Style style;
        style.bg_opa(LV_OPA_COVER);
        style.reset();

        auto box = lv::Box::create(lv.screen());
        box.add_style(style.get());
        // After reset, the style should not affect the widget's default values
        // (bg_opa won't be LV_OPA_COVER since the style was reset)
        CHECK(box.get() != nullptr);
    }
}

TEST_CASE("Anim builder") {
    LvglFixture lv;

    SUBCASE("builder pattern with values, duration, path, start") {
        auto box = lv::Box::create(lv.screen()).size(50, 50);

        auto* a = lv::Anim()
            .exec_translate_x(box)
            .values(0, 100)
            .duration(500)
            .linear()
            .start();
        CHECK(a != nullptr);

        lv::anim_delete(box);
    }

    SUBCASE("anim helpers") {
        auto box = lv::Box::create(lv.screen()).size(50, 50);
        auto* a = lv::anim_x(box, 0, 100).duration(200).linear().start();
        CHECK(a != nullptr);
        lv::anim_delete(box);
    }

    SUBCASE("exec_opa with tick-forward") {
        auto box = lv::Box::create(lv.screen()).size(50, 50);
        lv::Anim()
            .exec_opa(box)
            .values(0, 255)
            .duration(100)
            .linear()
            .start();

        // Advance time
        lv::tick_inc(110);
        lv::timer_handler();

        // After full duration, opa should be at end value
        CHECK(box.get_opa() >= 200); // Allow some tolerance

        lv::anim_delete(box);
    }
}

// ============================================================
// Phase 6 — Display, Screen, Navigator
// ============================================================

TEST_CASE("Display") {
    LvglFixture lv;

    SUBCASE("get_default, width/height match fixture") {
        auto disp = lv::Display::get_default();
        CHECK(disp.get() != nullptr);
        CHECK(disp.width() == DISP_W);
        CHECK(disp.height() == DISP_H);
    }

    SUBCASE("screen_active") {
        auto disp = lv::Display::get_default();
        auto scr = disp.screen_active();
        CHECK(scr.get() == lv.screen());
    }

    SUBCASE("layer_top/sys") {
        auto disp = lv::Display::get_default();
        CHECK(disp.layer_top().get() != nullptr);
        CHECK(disp.layer_sys().get() != nullptr);
    }

    SUBCASE("display_width/display_height helpers") {
        CHECK(lv::display_width() == DISP_W);
        CHECK(lv::display_height() == DISP_H);
    }

    SUBCASE("layer helpers") {
        CHECK(lv::layer_top().get() != nullptr);
        CHECK(lv::layer_sys().get() != nullptr);
    }
}

TEST_CASE("Screen create/load/is_active") {
    LvglFixture lv;
    auto original = lv::screen_active();

    SUBCASE("Screen creation and load") {
        lv::Screen scr;
        CHECK(scr.get() != nullptr);
        CHECK_FALSE(scr.is_active());
        scr.load();
        CHECK(scr.is_active());
        // Restore
        lv::screen_load(original);
    }

    SUBCASE("screen_active returns wrapper") {
        auto active = lv::screen_active();
        CHECK(active.get() == original.get());
        CHECK(active.is_active());
    }

    SUBCASE("screen_create helper") {
        auto scr = lv::screen_create();
        CHECK(scr.get() != nullptr);
        lv::ObjectView(scr).del();
    }
}

TEST_CASE("Navigator") {
    LvglFixture lv;
    auto original = lv::screen_active();

    SUBCASE("push/depth/can_back") {
        lv::Navigator nav;
        lv::Screen s1;
        lv::Screen s2;

        CHECK(nav.depth() == 0);
        CHECK_FALSE(nav.can_back());

        nav.push(s1);
        CHECK(nav.depth() == 1);
        CHECK_FALSE(nav.can_back());
        CHECK(nav.current().get() == s1.get());

        nav.push(s2);
        CHECK(nav.depth() == 2);
        CHECK(nav.can_back());
        CHECK(nav.current().get() == s2.get());

        lv::screen_load(original);
    }

    SUBCASE("back") {
        lv::Navigator nav;
        lv::Screen s1;
        lv::Screen s2;

        nav.push(s1);
        nav.push(s2);
        CHECK(nav.depth() == 2);

        bool went_back = nav.back(0);
        CHECK(went_back);
        CHECK(nav.depth() == 1);
        CHECK(nav.current().get() == s1.get());

        went_back = nav.back(0);
        CHECK_FALSE(went_back); // can't go back from root

        lv::screen_load(original);
    }

    SUBCASE("set_root") {
        lv::Navigator nav;
        lv::Screen s1;
        lv::Screen s2;

        nav.push(s1);
        nav.push(s2);

        lv::Screen s3;
        nav.set_root(s3);
        CHECK(nav.depth() == 1);
        CHECK(nav.current().get() == s3.get());
        CHECK_FALSE(nav.can_back());

        lv::screen_load(original);
    }
}
