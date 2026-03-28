/**
 * @file zero_cost_test.cpp
 * @brief Test to verify zero-cost abstractions
 *
 * Compile with: g++ -O2 -S -o zero_cost_test.s zero_cost_test.cpp
 * Then compare the assembly for C vs C++ versions
 */

#include <lv/lv.hpp>

// ============================================================
// TEST 1: Basic widget creation
// ============================================================

// C version
extern "C" void test_c_label(lv_obj_t* parent) {
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, "Hello");
    lv_obj_set_width(label, 100);
}

// C++ version - should produce identical assembly
void test_cpp_label(lv_obj_t* parent) {
    lv::Label::create(parent)
        .text("Hello")
        .width(100);
}

// ============================================================
// TEST 2: Event handling with member function
// ============================================================

struct Handler {
    int counter = 0;
    void on_click(lv::Event) { counter++; }
};

// C version
static void c_event_cb(lv_event_t* e) {
    auto* h = static_cast<Handler*>(lv_event_get_user_data(e));
    h->counter++;
}

extern "C" void test_c_event(lv_obj_t* parent, Handler* h) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_add_event_cb(btn, c_event_cb, LV_EVENT_CLICKED, h);
}

// C++ version
void test_cpp_event(lv_obj_t* parent, Handler* h) {
    lv::Button::create(parent)
        .on_click<&Handler::on_click>(h);
}

// ============================================================
// TEST 3: Style setting
// ============================================================

// C version
extern "C" void test_c_style(lv_obj_t* obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_radius(obj, 8, 0);
    lv_obj_set_style_pad_all(obj, 16, 0);
}

// C++ version
void test_cpp_style(lv_obj_t* parent) {
    lv::Box::create(parent)
        .bg_color(lv::rgb(0xFF0000))
        .radius(8)
        .padding(16);
}

// ============================================================
// TEST 4: Flex layout
// ============================================================

// C version
extern "C" lv_obj_t* test_c_flex(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(cont, 10, 0);
    return cont;
}

// C++ version
lv_obj_t* test_cpp_flex(lv_obj_t* parent) {
    auto cont = lv::hbox(lv::ObjectView(parent))
        .gap(10)
        .align_items(lv::kFlexAlign::center);
    return cont.get();
}

// ============================================================
// TEST 5: Color creation
// ============================================================

// C version
extern "C" lv_color_t test_c_color() {
    return lv_color_hex(0x3498db);
}

// C++ version
lv_color_t test_cpp_color() {
    return lv::rgb(0x3498db);
}

// ============================================================
// TEST 6: Sizeof checks (compile-time)
// ============================================================

static_assert(sizeof(lv::ObjectView) == sizeof(lv_obj_t*), "ObjectView must be pointer-sized");
static_assert(sizeof(lv::ObjectRef) == sizeof(lv_obj_t*), "ObjectRef must be pointer-sized");
static_assert(sizeof(lv::Label) == sizeof(lv_obj_t*), "Label must be pointer-sized");
static_assert(sizeof(lv::Button) == sizeof(lv_obj_t*), "Button must be pointer-sized");
static_assert(sizeof(lv::Slider) == sizeof(lv_obj_t*), "Slider must be pointer-sized");
static_assert(sizeof(lv::Chart) == sizeof(lv_obj_t*), "Chart must be pointer-sized");

int main() {
    return 0;
}
