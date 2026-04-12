# lv:: C++ API Guide

This guide explains the C++ wrapper API for LVGL and how it differs from the C API.

## Table of Contents

1. [Philosophy](#philosophy)
2. [Widget Creation](#widget-creation)
3. [Fluent API (Method Chaining)](#fluent-api-method-chaining)
4. [Colors](#colors)
5. [Styles](#styles)
6. [Animations](#animations)
7. [Events](#events)
8. [Object Wrapping](#object-wrapping)
9. [Common Patterns](#common-patterns)
10. [UI Automation](#ui-automation)

---

## Philosophy

The C++ wrapper aims to be:
- **Zero-cost**: No runtime overhead compared to C API
- **Type-safe**: Leverages C++ type system
- **Fluent**: Method chaining for readable, concise code
- **RAII-friendly**: Automatic resource management where appropriate

---

## Widget Creation

### C API
```c
lv_obj_t* btn = lv_button_create(parent);
lv_obj_set_size(btn, 100, 50);
lv_obj_set_pos(btn, 10, 10);

lv_obj_t* label = lv_label_create(btn);
lv_label_set_text(label, "Click me");
```

### C++ API
```cpp
auto btn = lv::Button::create(parent)
    .size(100, 50)
    .pos(10, 10);

auto label = lv::Label::create(btn)
    .text("Click me");
```

All widgets follow the pattern:
- `WidgetName::create(parent)` - Creates a new widget
- Returns a wrapper object that can be chained

---

## Fluent API (Method Chaining)

Most setters return `*this` to allow chaining:

### C API
```c
lv_obj_t* arc = lv_arc_create(parent);
lv_arc_set_rotation(arc, 270);
lv_arc_set_bg_angles(arc, 0, 360);
lv_arc_set_value(arc, 75);
lv_obj_set_size(arc, 150, 150);
lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
lv_obj_set_style_arc_width(arc, 10, LV_PART_MAIN);
lv_obj_set_style_arc_width(arc, 10, LV_PART_INDICATOR);
```

### C++ API
```cpp
auto arc = lv::Arc::create(parent)
    .rotation(270)
    .bg_angles(0, 360)
    .value(75)
    .size(150, 150)
    .remove_style(nullptr, lv::kPart::knob)
    .arc_width(10, lv::kPart::main)
    .arc_width(10, lv::kPart::indicator);
```

---

## Colors

### Creating Colors

| C API | C++ API | Description |
|-------|---------|-------------|
| `lv_color_hex(0xFF0000)` | `lv::rgb(0xFF0000)` | Color from hex value |
| `lv_color_make(255, 0, 0)` | `lv::rgb(255, 0, 0)` | Color from RGB components |
| `lv_color_white()` | `lv::white()` | White color |
| `lv_color_black()` | `lv::black()` | Black color |

### Color Conversions

| C API | C++ API | Description |
|-------|---------|-------------|
| `lv_color_hsv_to_rgb(h, s, v)` | `lv::hsv_to_rgb(h, s, v)` | HSV to RGB |
| `lv_color_to_hsv(color)` | `lv::rgb_to_hsv(color)` | RGB to HSV |

### Examples

```cpp
// Hex color
auto red = lv::rgb(0xFF0000);

// RGB components
auto green = lv::rgb(0, 255, 0);

// HSV to RGB
auto hue_color = lv::hsv_to_rgb(120, 100, 100);  // Pure green

// Get HSV from RGB
auto hsv = lv::rgb_to_hsv(some_color);
uint16_t hue = hsv.h;
```

### Naming Convention

The color API follows the `source_to_target` naming pattern:
- `lv::hsv_to_rgb()` - Convert HSV to RGB
- `lv::rgb_to_hsv()` - Convert RGB to HSV
- `lv::rgb()` - Create RGB color (default format)

---

## Styles

### Style Objects

| C API | C++ API |
|-------|---------|
| `lv_style_t style; lv_style_init(&style);` | `lv::Style style;` |
| `lv_style_set_bg_color(&style, color)` | `style.bg_color(color)` |
| `lv_obj_add_style(obj, &style, sel)` | `obj.add_style(style, sel)` |

### C API
```c
static lv_style_t style;
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0x000000));
lv_style_set_bg_opa(&style, LV_OPA_50);
lv_style_set_radius(&style, 10);

lv_obj_add_style(obj, &style, LV_PART_MAIN);
```

### C++ API
```cpp
lv::Style style;
style.bg_color(lv::rgb(0x000000))
     .bg_opa(lv::kOpa::_50)
     .radius(10);

obj.add_style(style, lv::kPart::main);
```

### Inline Styles

For one-off styling, use direct style methods on objects:

```cpp
// These set styles directly on the object
arc.arc_color(lv::rgb(0xFF0000))      // lv::kPart::main by default
   .arc_width(10);

// With specific part/state selector
arc.arc_color(lv::rgb(0x00FF00), lv::kPart::indicator);
```

### Removing Styles

```cpp
obj.remove_all_styles();                    // Remove all styles (blank canvas)
obj.remove_style(nullptr, lv::kPart::knob); // Remove styles from specific part
```

---

## Animations

### C API
```c
lv_anim_t anim;
lv_anim_init(&anim);
lv_anim_set_var(&anim, obj);
lv_anim_set_values(&anim, 0, 100);
lv_anim_set_duration(&anim, 500);
lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_arc_set_value);
lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
lv_anim_start(&anim);
```

### C++ API
```cpp
lv::Anim()
    .var(obj)
    .values(0, 100)
    .duration(500)
    .exec([](void* var, int32_t v) {
        lv::Arc(lv::wrap, static_cast<lv_obj_t*>(var)).value(v);
    })
    .ease_out()
    .start();
```

### Animation Methods

| Method | Description |
|--------|-------------|
| `.var(obj)` | Set target object |
| `.values(start, end)` | Set start and end values |
| `.duration(ms)` | Animation duration in milliseconds |
| `.delay(ms)` | Delay before starting |
| `.exec(callback)` | Execution callback |
| `.ease_in()` | Ease-in path |
| `.ease_out()` | Ease-out path |
| `.ease_in_out()` | Ease-in-out path |
| `.linear()` | Linear path |
| `.overshoot()` | Overshoot path |
| `.bounce()` | Bounce path |
| `.repeat(n)` | Number of repetitions |
| `.repeat_infinite()` | Repeat forever |
| `.playback(delay_ms)` | Playback (reverse) with optional delay |
| `.start()` | Start the animation |

---

## Events

### C API
```c
void btn_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Handle click
    }
}

lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, user_data);
```

### C++ API

The C++ wrapper uses `lv::Event` instead of `lv_event_t*` and provides event constants in the `lv::Event` class.

```cpp
// Stateless lambda (zero cost)
btn.on_click([](lv::Event e) {
    auto target = e.target();  // Get target object
    auto code = e.code();      // Get event code
});

// With explicit event type
btn.on(lv::kEvent::clicked, [](lv::Event e) {
    // Handle click
});

// Member function (zero cost, recommended for stateful callbacks)
btn.on_click<&MyApp::handle_click>(this);
```

### Event Constants

Use `lv::kEvent::` constants instead of `LV_EVENT_*` macros:

| C Macro | C++ Constant |
|---------|--------------|
| `LV_EVENT_CLICKED` | `lv::kEvent::clicked` |
| `LV_EVENT_PRESSED` | `lv::kEvent::pressed` |
| `LV_EVENT_RELEASED` | `lv::kEvent::released` |
| `LV_EVENT_VALUE_CHANGED` | `lv::kEvent::value_changed` |
| `LV_EVENT_FOCUSED` | `lv::kEvent::focused` |
| `LV_EVENT_DEFOCUSED` | `lv::kEvent::defocused` |
| `LV_EVENT_SCROLL` | `lv::kEvent::scroll` |
| `LV_EVENT_KEY` | `lv::kEvent::key` |
| `LV_EVENT_DELETE` | `lv::kEvent::delete_` |

### lv::Event Methods

The `lv::Event` wrapper provides these zero-cost methods:

| Method | C Equivalent |
|--------|--------------|
| `e.target()` | `lv_event_get_target_obj(e)` |
| `e.current_target()` | `lv_event_get_current_target_obj(e)` |
| `e.code()` | `lv_event_get_code(e)` |
| `e.user_data()` | `lv_event_get_user_data(e)` |
| `e.param()` | `lv_event_get_param(e)` |
| `e.stop_bubbling()` | `lv_event_stop_bubbling(e)` |
| `e.stop_processing()` | `lv_event_stop_processing(e)` |

### Common Event Shortcuts

| Method | Event |
|--------|-------|
| `.on_click(cb)` | `lv::kEvent::clicked` |
| `.on_pressed(cb)` | `lv::kEvent::pressed` |
| `.on_released(cb)` | `lv::kEvent::released` |
| `.on_value_changed(cb)` | `lv::kEvent::value_changed` |
| `.on_focused(cb)` | `lv::kEvent::focused` |
| `.on_defocused(cb)` | `lv::kEvent::defocused` |
| `.on(code, cb)` | Any event code |

### Capturing Lambdas (Not Supported)

**Capturing lambdas are intentionally not supported** because they cannot be converted to C function pointers without runtime overhead.

```cpp
// This will NOT compile:
int counter = 0;
btn.on_click([&counter](lv::Event e) { counter++; });
// ERROR: Use member function: btn.on_click<&MyClass::handler>(this)
```

**Use member functions instead** (zero cost, same as C user_data pattern):

```cpp
class MyApp {
    int counter = 0;

    void handle_click(lv::Event e) {
        counter++;  // Access member variable directly
    }

public:
    void setup(lv::Button& btn) {
        // Zero-cost: 'this' is passed as LVGL user_data
        btn.on_click<&MyApp::handle_click>(this);

        // Or with explicit event type
        btn.on<&MyApp::handle_click>(lv::kEvent::clicked, this);
    }
};
```

Member function signatures supported:
- `void handler(lv::Event e)` - recommended
- `void handler(lv_event_t* e)` - legacy C-style
- `void handler()` - when event data not needed

---

## Object Wrapping

The wrapper exposes four related pointer-sized types for working with LVGL
objects. Knowing which one you want saves a surprising amount of confusion —
they differ on whether they own the underlying `lv_obj_t`, whether they have
fluent setters, and where each one typically comes from.

### Cheatsheet

| Type                             | Owns?             | Fluent setters?       | Fluent getters? | Where it comes from                                                                          |
|----------------------------------|-------------------|-----------------------|-----------------|----------------------------------------------------------------------------------------------|
| `ObjectView`                     | No                | **No** (getters only) | Yes             | Function parameter accepting any widget; `Component::build()` return                         |
| `ObjectRef`                      | No                | Yes                   | Yes             | `lv::ref(ptr)`, `widget.parent()`, `widget.child(i)`, `Msgbox::add_title()`, `layer_top()` …   |
| `Object`                         | Yes (RAII)        | **No** (getters only) | Yes             | `lv::Object obj(parent)` direct construction — rare, prefer typed widgets below              |
| `Box` / `Label` / `Button` / …    | No (parent cleans) | Yes                   | Yes             | `Box::create(parent)`, `Box{wrap, existing}`, `Box{}` (deferred init)                          |

Read the columns as "which mixin does it inherit?":

- `ObjectView` is the base class. Nothing but getters.
- `ObjectRef` = `ObjectView` + `ObjectMixin` + `StyleMixin` + `EventMixin`.
  The full fluent API on a non-owning handle.
- `Object` = `ObjectView` + RAII destructor. Owns the `lv_obj_t`, but no
  fluent setters (intentionally — you usually want a typed widget instead).
- Widget wrappers (`Box`, `Label`, `Button`, …) = `ObjectView` + all three
  mixins + widget-specific methods. Non-owning views over an LVGL object
  whose lifetime is managed by its parent.

All four types are exactly pointer-sized — they store nothing but an
`lv_obj_t*`. Passing them around is just as cheap as passing a raw pointer.

### Picking the right type

**When declaring a function parameter**: use `lv::ObjectView` if you only
need to read state, or `lv::ObjectRef` if you need to set anything.
`ObjectView` accepts any widget by slicing (because every widget inherits
from it); `ObjectRef` has an implicit constructor from `ObjectView` for the
same reason.

```cpp
// Read-only helper — takes any widget
int32_t bottom_of(lv::ObjectView w) {
    return w.get_y() + w.get_height();
}

// Setter helper — needs fluent setters, so takes ObjectRef
void stamp_theme(lv::ObjectRef w) {
    w.bg_color(lv::rgb(0x333333))
     .radius(8)
     .padding(10);
}

stamp_theme(my_button);   // Box, Button, Arc, … all convert implicitly
```

**When declaring a member**: use the typed widget (`lv::Box m_panel;`).
It's zero-cost, gives you the full fluent API, and is self-documenting.

```cpp
class MyComponent : public lv::Component<MyComponent> {
    lv::Box m_panel{};      // deferred init, null until set in build()
    lv::Label m_title{};
    // ...
};
```

**When you have a raw `lv_obj_t*`**: wrap it with `lv::ref(ptr)` to get an
`ObjectRef` with the full fluent API. Don't construct a typed widget unless
you know the object really is that type (use `Widget{lv::wrap, ptr}` then).

```cpp
lv_obj_t* raw = some_c_api_that_returns_a_pointer();
lv::ref(raw).bg_color(lv::rgb(0xFF0000)).radius(4);
```

**When you want RAII for a bare `lv_obj`**: `lv::Object obj(parent)`
constructs one and deletes it in the destructor. Rarely what you want —
typed widgets (`lv::Box`, etc.) are usually more appropriate even though
they don't own by default, because LVGL cleans up via the parent-child
tree.

### Wrapping existing objects

To wrap an existing `lv_obj_t*` pointer into a typed widget:

```cpp
// Using the wrap tag
lv_obj_t* raw_ptr = lv_label_create(parent);
auto label = lv::Label(lv::wrap, raw_ptr);

// Now use the C++ API
label.text("Hello").center();
```

Or to get the full fluent API without committing to a specific widget type:

```cpp
lv::ref(raw_ptr).bg_color(lv::rgb(0)).padding(10);
```

### Getting the raw pointer

Every wrapper exposes `.get()` for the underlying `lv_obj_t*`:

```cpp
auto btn = lv::Button::create(parent);
lv_obj_t* raw = btn.get();  // Get underlying lv_obj_t*
```

There's also an implicit conversion `operator lv_obj_t*()` that lets widgets
be passed directly to C API functions — **but be careful**: passing an owned
widget to a destructive C call like `lv_obj_delete()` will free it without
the wrapper knowing, leading to use-after-free when the wrapper's destructor
later runs. Prefer the C++ wrapper (`.del()`, `.set_parent()`, etc.) for
anything that affects ownership or lifetime. For read-only C API calls
without a wrapper equivalent, pass `.get()` explicitly so the intent is
visible at the call site.

---

## Common Patterns

### Creating a Styled Container

```cpp
auto panel = lv::Box::create(parent)
    .size(200, 150)
    .center()
    .bg_color(lv::rgb(0x202020))
    .bg_opa(lv::kOpa::cover)
    .radius(10)
    .pad_all(10)
    .flex_flow(lv::kFlexFlow::column)
    .flex_align(lv::kFlexAlign::center, lv::kFlexAlign::center, lv::kFlexAlign::center);
```

### Creating an Arc Gauge

```cpp
auto gauge = lv::Arc::create(parent)
    .remove_all_styles()
    .size(150, 150)
    .rotation(135)
    .bg_angles(0, 270)
    .value(75)
    .arc_width(15, lv::kPart::main)
    .arc_width(15, lv::kPart::indicator)
    .arc_color(lv::rgb(0x333333), lv::kPart::main)
    .arc_color(lv::rgb(0x00FF00), lv::kPart::indicator)
    .arc_rounded(true);
```

### Animated Value Change

```cpp
void animate_to_value(lv::Arc& arc, int32_t target) {
    lv::Anim()
        .var(arc.get())
        .values(arc.value(), target)
        .duration(300)
        .exec([](void* var, int32_t v) {
            lv::Arc(lv::wrap, static_cast<lv_obj_t*>(var)).value(v);
        })
        .ease_out()
        .start();
}
```

### Using Styles for Consistency

```cpp
class MyApp {
    lv::Style m_button_style;
    lv::Style m_panel_style;

public:
    void init_styles() {
        m_button_style
            .bg_color(lv::rgb(0x2196F3))
            .radius(8)
            .pad_all(12);

        m_panel_style
            .bg_color(lv::rgb(0x1E1E1E))
            .radius(12)
            .border_width(1)
            .border_color(lv::rgb(0x333333));
    }

    lv::Button create_styled_button(lv::ObjectView parent) {
        return lv::Button::create(parent)
            .add_style(m_button_style);
    }
};
```

---

## UI Automation

The C++ wrapper provides built-in support for object naming through the `name()` method, wrapping LVGL's `lv_obj_set_name()` / `lv_obj_get_name()`.

### Prerequisites

Enable object naming in `lv_conf.h`:
```c
#define LV_USE_OBJ_NAME 1
```

When disabled, `name()` calls are no-ops with zero overhead (compile-time eliminated via `if constexpr`).

### Setting Object Names

Use `name()` to assign identifiers to widgets for automation and debugging:

```cpp
// Widgets with visible text are usually auto-identifiable by their content
lv::Button::create(parent)
    .text("Login");  // Can be found by text content

// Widgets without text need explicit names
lv::Image::create(parent)
    .src(&icon_settings)
    .name("settings_icon");

lv::Slider::create(parent)
    .range(0, 100)
    .value(50)
    .name("volume_slider");

lv::Arc::create(parent)
    .rotation(270)
    .bg_angles(0, 360)
    .name("progress_arc");
```

### C API Comparison

| C API | C++ API |
|-------|---------|
| `lv_obj_set_name(obj, "id")` | `obj.name("id")` |
| `lv_obj_get_name(obj)` | `obj.get_name()` |

### Methods

| Method | Description |
|--------|-------------|
| `.name(str)` | Set object name (returns `*this` for chaining) |
| `.get_name()` | Get object name (returns `nullptr` if not set) |

### Feature Detection

The `lv::has_obj_name` compile-time constant indicates whether object naming is enabled:

```cpp
if constexpr (lv::has_obj_name) {
    // Object naming is available
}
```

### When to Use

| Widget Type | Needs `name()`? | Reason |
|-------------|-----------------|--------|
| Button with text | No | Identifiable by text content |
| Label | No | Identifiable by text content |
| Checkbox | No | Identifiable by text content |
| Image | Yes | No text content |
| Slider | Yes | No text content |
| Arc/Gauge | Yes | No text content |
| Canvas | Yes | No text content |
| Identical siblings | Yes | Disambiguate multiple widgets of same type |

---

## Constants

The C++ wrapper provides namespaced constants as an alternative to LVGL's C-style macros.
Both can be used interchangeably.

### C API vs C++ API Constants

| C Macro | C++ Constant |
|---------|--------------|
| `LV_ALIGN_CENTER` | `lv::kAlign::center` |
| `LV_PART_MAIN` | `lv::kPart::main` |
| `LV_PART_INDICATOR` | `lv::kPart::indicator` |
| `LV_PART_KNOB` | `lv::kPart::knob` |
| `LV_STATE_PRESSED` | `lv::kState::pressed` |
| `LV_STATE_CHECKED` | `lv::kState::checked` |
| `LV_STATE_DISABLED` | `lv::kState::disabled` |
| `LV_OPA_50` | `lv::kOpa::_50` |
| `LV_OPA_COVER` | `lv::kOpa::cover` |
| `LV_OPA_TRANSP` | `lv::kOpa::transp` |
| `LV_DIR_HOR` | `lv::kDir::hor` |
| `LV_DIR_VER` | `lv::kDir::ver` |
| `LV_FLEX_FLOW_ROW` | `lv::kFlexFlow::row` |
| `LV_FLEX_FLOW_COLUMN` | `lv::kFlexFlow::column` |
| `LV_FLEX_ALIGN_CENTER` | `lv::kFlexAlign::center` |
| `LV_FLEX_ALIGN_SPACE_BETWEEN` | `lv::kFlexAlign::space_between` |
| `LV_EVENT_CLICKED` | `lv::kEvent::clicked` |
| `LV_EVENT_VALUE_CHANGED` | `lv::kEvent::value_changed` |
| `LV_CHART_TYPE_LINE` | `lv::Chart::Type::line` |
| `LV_CHART_AXIS_PRIMARY_Y` | `lv::Chart::Axis::primary_y` |
| `LV_ARC_MODE_NORMAL` | `lv::Arc::Mode::normal` |
| `LV_BAR_MODE_RANGE` | `lv::Bar::Mode::range` |
| `LV_SLIDER_MODE_RANGE` | `lv::Slider::Mode::range` |
| `LV_SIZE_CONTENT` | `lv::Size::content` |
| `LV_PCT(50)` | `lv::pct(50)` |

### Examples

```cpp
// C-style (still works)
arc.align(LV_ALIGN_CENTER);
slider.add_style(style, LV_PART_KNOB | LV_STATE_PRESSED);
obj.bg_opa(LV_OPA_50);

// C++ style (more readable)
arc.align(lv::kAlign::center);
slider.add_style(style, lv::kPart::knob | lv::kState::pressed);
obj.bg_opa(lv::kOpa::_50);

// Flex layout
container.flex_flow(lv::kFlexFlow::column)
         .flex_align(lv::kFlexAlign::center,
                     lv::kFlexAlign::center,
                     lv::kFlexAlign::center);

// Chart
chart.type(lv::Chart::Type::line)
     .range(lv::Chart::Axis::primary_y, 0, 100);

// Size with percentage
panel.width(lv::pct(80))
     .height(lv::Size::content);
```

### Available Namespaces

| Namespace | Contains |
|-----------|----------|
| `lv::kAlign` | Alignment constants (center, top_left, etc.) |
| `lv::kPart` | Object parts (main, indicator, knob, etc.) |
| `lv::kState` | Object states (pressed, checked, disabled, etc.) |
| `lv::kOpa` | Opacity values (_0 to _100, transp, cover) |
| `lv::kDir` | Directions (left, right, top, bottom, hor, ver) |
| `lv::kFlexFlow` | Flex flow modes (row, column, wrap variants) |
| `lv::kFlexAlign` | Flex alignment (start, end, center, space_*) |
| `lv::kGridAlign` | Grid alignment |
| `lv::kTextAlign` | Text alignment (left, center, right) |
| `lv::kEvent` | Event types (clicked, pressed, value_changed, etc.) |
| `lv::kArcMode` | Arc modes (normal, symmetrical, reverse) |
| `lv::kBarMode` | Bar modes (normal, symmetrical, range) |
| `lv::kSliderMode` | Slider modes |
| `lv::kChartType` | Chart types (line, bar, scatter) |
| `lv::kChartAxis` | Chart axes |
| `lv::kLabelLongMode` | Label long modes (wrap, dot, scroll, clip) |
| `lv::kGradDir` | Gradient directions (hor, ver) |
| `lv::kBorderSide` | Border sides |
| `lv::kScrollbarMode` | Scrollbar modes |
| `lv::kBlendMode` | Blend modes |
| `lv::kSize` | Size helpers (content, pct()) |

---

## Quick Reference

### Widget Creation
```cpp
lv::Label::create(parent)      // Label
lv::Button::create(parent)     // Button
lv::Arc::create(parent)        // Arc
lv::Bar::create(parent)        // Bar
lv::Slider::create(parent)     // Slider
lv::Switch::create(parent)     // Switch
lv::Checkbox::create(parent)   // Checkbox
lv::Dropdown::create(parent)   // Dropdown
lv::Roller::create(parent)     // Roller
lv::Chart::create(parent)      // Chart
lv::Box::create(parent)        // Generic container
```

### Common Methods (all widgets)
```cpp
.size(w, h)           // Set size
.width(w)             // Set width
.height(h)            // Set height
.pos(x, y)            // Set position
.x(x)                 // Set X position
.y(y)                 // Set Y position
.center()             // Center in parent
.align(align, x, y)   // Align with offset
.hidden(bool)         // Show/hide
.clickable(bool)      // Enable/disable clicks
.scrollable(bool)     // Enable/disable scrolling
.get()                // Get raw lv_obj_t*
```

### Style Methods
```cpp
.bg_color(color)      // Background color
.bg_opa(opa)          // Background opacity
.radius(r)            // Corner radius
.border_width(w)      // Border width
.border_color(c)      // Border color
.pad_all(p)           // Padding (all sides)
.pad_hor(p)           // Horizontal padding
.pad_ver(p)           // Vertical padding
```

### Colors
```cpp
lv::rgb(0xFF0000)           // From hex
lv::rgb(255, 0, 0)          // From RGB
lv::white()                 // White
lv::black()                 // Black
lv::hsv_to_rgb(h, s, v)     // HSV to RGB
lv::rgb_to_hsv(color)       // RGB to HSV
```
