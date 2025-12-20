# lv:: C++20 Bindings for LVGL - Architecture Document

## Overview

This library provides **zero-cost C++20 abstractions** over LVGL's C API, enabling modern, type-safe, declarative UI development while maintaining full compatibility with LVGL's retained-mode widget system.

The design philosophy is: **C++ is the only UI language** - no QML, no XML, no code generation, no moc. Just pure C++20 with CMake.

---

## Core Design Principles

### 1. Zero-Cost Abstractions

Every wrapper class is exactly `sizeof(void*)` - a single pointer to the underlying `lv_obj_t*`. There is no runtime overhead compared to using the C API directly.

```cpp
static_assert(sizeof(lv::Button) == sizeof(void*));
static_assert(sizeof(lv::Label) == sizeof(void*));
```

The compiler inlines all wrapper methods, producing identical assembly to hand-written C code.

### 2. Fluent Builder API

All widget configuration uses method chaining for readable, declarative UI code:

```cpp
lv::Button(parent)
    .text("Click me")
    .size(120, 40)
    .bg_color(lv::colors::blue())
    .on_click<&MyClass::handle_click>(this);
```

### 3. Type-Safe Event Handling

Events use compile-time member function pointers instead of string-based bindings:

```cpp
// Type-safe: compiler verifies method signature
button.on_click<&MyApp::on_button_clicked>(this);

// Also supports stateless lambdas (no captures, zero allocation)
button.on_click([](lv::Event e) {
    // handle click
});
```

This is **refactor-safe** - renaming a method updates all references via the compiler.

### 4. RAII Where Appropriate

- `lv::Object` - Owning wrapper, deletes widget in destructor
- `lv::ObjectView` - Non-owning view (default for most widgets)
- `lv::Timer` - RAII timer management
- `lv::Style` - RAII style management

Most widgets use non-owning semantics because LVGL's parent-child hierarchy manages lifetime.

### 5. No Hidden Allocations

The library avoids heap allocations in the wrapper layer. State management (`lv::State<T>`) embeds the LVGL subject directly, avoiding indirection.

---

## Architecture Layers

```
┌─────────────────────────────────────────────────────────┐
│                    Application Code                     │
│         (Components, ViewModels, Business Logic)        │
├─────────────────────────────────────────────────────────┤
│                   lv:: C++ Wrappers                     │
│    ┌─────────────┬─────────────┬─────────────────────┐  │
│    │   Widgets   │   Layouts   │   Core Services     │  │
│    │  (34 types) │ (Flex/Grid) │ (Event/State/Timer) │  │
│    └─────────────┴─────────────┴─────────────────────┘  │
├─────────────────────────────────────────────────────────┤
│                      LVGL C API                         │
│              (Retained-mode widget system)              │
├─────────────────────────────────────────────────────────┤
│                   Display Backend                       │
│               (SDL / X11 / Framebuffer)                 │
└─────────────────────────────────────────────────────────┘
```

---

## Module Breakdown

### Core (`include/lv/core/`)

| File | Purpose |
|------|---------|
| `object.hpp` | Base `ObjectView`/`Object` classes + global constants (State, Part, Flag, Direction, Align, etc.) |
| `event.hpp` | Type-safe event handling with `EventMixin<Derived>` CRTP |
| `style.hpp` | Style management with `StyleMixin<Derived>` CRTP |
| `color.hpp` | Color utilities (`hex()`, `rgb()`, `colors::` namespace) |
| `font.hpp` | Font handling, `DynamicFont` for runtime TTF loading |
| `timer.hpp` | RAII `Timer` wrapper |
| `anim.hpp` | Animation system with path callbacks |
| `screen.hpp` | Screen management, `Navigator` for screen stack |
| `state.hpp` | Reactive `State<T>` using LVGL observer system |
| `component.hpp` | CRTP `Component<Derived>` base for custom widgets |
| `string_utils.hpp` | String utilities including `lv::snprintf()` |
| `image.hpp` | Image handling utilities |
| `indev.hpp` | Input device wrappers |
| `focus.hpp` | Focus group (`lv_group_t`) RAII wrapper for keyboard/encoder navigation |
| `theme.hpp` | Theme application |
| `translation.hpp` | i18n support |

### Widgets (`include/lv/widgets/`)

34 widget wrappers, each following the same pattern:

```cpp
class WidgetName : public ObjectView,
                   public EventMixin<WidgetName>,
                   public StyleMixin<WidgetName> {
public:
    explicit WidgetName(ObjectView parent);

    // Widget-specific configuration methods
    WidgetName& property(value) noexcept;

    // Getters where needed
    [[nodiscard]] Type property() const noexcept;
};
```

**Wrapped widgets**: Label, Button, Switch, Slider, Checkbox, Dropdown, Roller, Textarea, Spinbox, Arc, Bar, Chart, Table, List, Menu, TabView, TileView, Calendar, Keyboard, ButtonMatrix, Canvas, LED, Line, Spinner, Scale, Span, Window, MsgBox, ImageButton, AnimImage, Box, FileExplorer (conditional)

### Layouts (`include/lv/layout/`)

| File | Purpose |
|------|---------|
| `flex.hpp` | Flexbox layout (`hbox()`, `vbox()`) with gap, alignment, grow |
| `grid.hpp` | CSS Grid layout with `fr()` units, spanning, alignment |

### Display (`include/lv/display/`)

| File | Purpose |
|------|---------|
| `display.hpp` | Base display interface |
| `x11_display.hpp` | X11 backend for Linux desktop |
| `sdl_display.hpp` | SDL2 backend for cross-platform |
| `fb_display.hpp` | Framebuffer backend for embedded |

---

## Constants and Type System

### Global Constants (k* Namespaces)

All LVGL `LV_*` constants are wrapped in `k`-prefixed namespaces with snake_case values:

```cpp
// Object states (for styling)
lv::kState::checked, lv::kState::pressed, lv::kState::disabled

// Object parts (for styling specific areas)
lv::kPart::main, lv::kPart::indicator, lv::kPart::knob

// Object flags (behavior modifiers)
lv::kFlag::scrollable, lv::kFlag::clickable, lv::kFlag::hidden

// Direction
lv::kDirection::left, lv::kDirection::horizontal, lv::kDirection::all

// Alignment
lv::kAlign::center, lv::kAlign::top_left, lv::kAlign::bottom_right

// Text alignment
lv::kTextAlign::left, lv::kTextAlign::center, lv::kTextAlign::right

// And more: kBorderSide, kGradientDirection, kBlendMode, kBaseDirection, kScrollSnap
```

### Widget-Specific Constants (Nested)

Constants specific to a widget are nested inside the class:

```cpp
// Chart
lv::Chart::Type::line, lv::Chart::Type::bar, lv::Chart::Type::scatter
lv::Chart::Axis::primary_y, lv::Chart::Axis::secondary_y
lv::Chart::UpdateMode::shift, lv::Chart::UpdateMode::circular

// Grid
lv::Grid::fr(1)  // fractional unit
lv::Grid::content
lv::Grid::template_last
lv::Grid::Align::start, lv::Grid::Align::center
```

### Type Aliases (Value Types Only)

Type aliases are used for value types, NOT for pointers:

```cpp
// Good - value types
using Color = lv_color_t;
using Opacity = lv_opa_t;
using ColorHSV = lv_color_hsv_t;
using EventData = lv_event_t;
using AnimData = lv_anim_t;

// Bad - would hide pointer nature (we don't do this)
// using ChartSeries = lv_chart_series_t*;  // NO!
```

---

## Event System

### Architecture

Events use a CRTP mixin (`EventMixin<Derived>`) that provides:

1. **Free function callbacks** (stateless lambdas)
2. **Member function pointer callbacks** (type-safe, zero allocation)

```cpp
template<typename Derived>
class EventMixin {
public:
    // Stateless lambda
    template<auto Fn>
    Derived& on_click() noexcept;

    // Member function pointer
    template<auto MemFn, typename T>
    Derived& on_click(T* instance) noexcept;

    // Generic event registration
    template<auto Fn>
    Derived& on_event(lv_event_code_t code) noexcept;

    template<auto MemFn, typename T>
    Derived& on_event(lv_event_code_t code, T* instance) noexcept;
};
```

### Supported Events

- `on_click()`, `on_pressed()`, `on_released()`
- `on_value_changed()`
- `on_focused()`, `on_defocused()`
- `on_scroll()`, `on_scroll_end()`
- Generic `on_event()` for any LVGL event code

### Implementation Detail

The system stores a pointer to the instance in LVGL's user data and uses a static trampoline function that casts and calls the member function. No heap allocation occurs.

---

## Reactive State System

### State<T>

A reactive container that triggers UI updates when changed:

```cpp
lv::State<int> counter{0};

// In component build()
lv::Label(parent)
    .bind_text(counter, "Count: %d");

// Later, UI auto-updates
counter.set(counter.get() + 1);
```

### Integration with LVGL Observer

`State<T>` wraps `lv_subject_t` from LVGL's observer system:
- Changes notify all bound widgets
- No heap allocation (subject embedded in State object)
- Supports int, bool, color, pointer types

---

## Layout System

### Flexbox (`lv::Flex`)

```cpp
auto row = lv::hbox(parent)
    .gap(10)
    .padding(16)
    .align_items(lv::align::center)
    .justify_content(lv::align::space_between);

auto col = lv::vbox(parent)
    .gap(8)
    .wrap(true);

// Child flex properties
child.flex_grow(1);
```

### Grid (`lv::Grid`)

```cpp
static int32_t cols[] = {lv::Grid::fr(1), lv::Grid::fr(2), lv::Grid::template_last};
static int32_t rows[] = {50, lv::Grid::content, lv::Grid::template_last};

auto grid = lv::grid(parent)
    .dsc_array(cols, rows)
    .gap(4);

// Place children
lv::grid_cell(child).col(0).row(0).col_span(2);
```

---

## Component Pattern

### CRTP Base Class

```cpp
template<typename Derived>
class Component {
public:
    void mount(ObjectView parent);
    void unmount();

protected:
    // Override in derived class
    virtual ObjectView build(ObjectView parent) = 0;
};
```

### Usage

```cpp
class CounterView : public lv::Component<CounterView> {
    lv::State<int> count{0};

public:
    lv::ObjectView build(lv::ObjectView parent) override {
        auto root = lv::vbox(parent).padding(16).gap(8);

        lv::Label(root).bind_text(count, "Count: %d");

        lv::Button(root)
            .text("Increment")
            .on_click<&CounterView::on_increment>(this);

        return root;
    }

private:
    void on_increment(lv::Event) {
        count.set(count.get() + 1);
    }
};
```

---

## Screen Navigation

### Navigator

Stack-based screen management:

```cpp
lv::Navigator nav;

// Push screen with animation
nav.push<SettingsScreen>(lv::screen_anim::move_left);

// Pop back
nav.pop(lv::screen_anim::move_right);

// Replace current
nav.replace<HomeScreen>();
```

---

## Styling System

### Inline Styles (via StyleMixin)

```cpp
button
    .bg_color(lv::colors::blue())
    .text_color(lv::colors::white())
    .radius(8)
    .padding(12);
```

### Style Objects (Reusable)

```cpp
lv::Style button_style;
button_style
    .bg_color(lv::colors::blue())
    .radius(8)
    .padding(12);

button.add_style(&button_style);
```

---

## Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Classes | PascalCase | `Chart`, `Button`, `Label` |
| Type aliases | PascalCase | `Color`, `Opacity`, `EventData` |
| Constant structs | PascalCase | `ObjState`, `Part`, `Flag` |
| Constant values | snake_case | `ObjState::checked`, `Part::main` |
| Functions | snake_case | `hex()`, `rgb()`, `snprintf()` |
| Namespaces | snake_case | `colors`, `symbol`, `fonts` |
| Member methods | snake_case | `bg_color()`, `fill_width()` |

### Rules

1. **Don't hide pointers** - `lv_chart_series_t*` stays as-is, not aliased
2. **Full names** - `GradientDirection` not `GradDir`
3. **Trailing underscore for reserved words** - `default_`, `auto_`

---

## File Organization

```
include/lv/
├── lv.hpp                 # Main header (includes everything)
├── core/
│   ├── object.hpp         # ObjectView, Object, constants
│   ├── event.hpp          # EventMixin, Event
│   ├── style.hpp          # StyleMixin, Style
│   ├── color.hpp          # Color utilities
│   ├── font.hpp           # Font handling
│   ├── timer.hpp          # Timer wrapper
│   ├── anim.hpp           # Animation
│   ├── screen.hpp         # Screen, Navigator
│   ├── state.hpp          # Reactive State<T>
│   ├── component.hpp      # Component base
│   ├── string_utils.hpp   # String utilities
│   └── ...
├── widgets/
│   ├── label.hpp
│   ├── button.hpp
│   ├── chart.hpp
│   └── ... (34 widgets)
├── layout/
│   ├── flex.hpp           # hbox, vbox
│   └── grid.hpp           # CSS Grid
└── display/
    ├── x11_display.hpp
    ├── sdl_display.hpp
    └── fb_display.hpp
```

---

## Example: Complete Application

```cpp
#include <lv/lv.hpp>

class MyApp : public lv::Component<MyApp> {
    lv::State<int> counter{0};

public:
    lv::ObjectView build(lv::ObjectView parent) override {
        auto root = lv::vbox(parent)
            .fill()
            .padding(20)
            .gap(16)
            .align_items(lv::align::center);

        lv::Label(root)
            .text("Hello, LVGL!")
            .text_color(lv::colors::blue());

        lv::Label(root)
            .bind_text(counter, "Counter: %d");

        auto buttons = lv::hbox(root).gap(10);

        lv::Button(buttons)
            .text(LV_SYMBOL_MINUS)
            .on_click<&MyApp::decrement>(this);

        lv::Button(buttons)
            .text(LV_SYMBOL_PLUS)
            .on_click<&MyApp::increment>(this);

        return root;
    }

private:
    void increment(lv::Event) { counter.set(counter.get() + 1); }
    void decrement(lv::Event) { counter.set(counter.get() - 1); }
};

int main() {
    lv::init();
    lv::SDLDisplay display(480, 320);

    MyApp app;
    app.mount(lv::screen_active());

    lv::run();
    return 0;
}
```

---

## What's NOT Wrapped (By Design)

| Item | Reason |
|------|--------|
| `lv_chart_series_t*` | Raw pointer - should be visible |
| `LV_SYMBOL_*` macros | Required for string concatenation |
| Low-level draw API | Rarely needed for UI apps |
| File system API | Platform-specific |

---

## Implementation Details

### CRTP Mixin Pattern

Instead of virtual functions, we use the Curiously Recurring Template Pattern (CRTP) for zero-overhead polymorphism:

```cpp
template<typename Derived>
class EventMixin {
    lv_obj_t* obj() noexcept {
        return static_cast<Derived*>(this)->get();  // Compile-time dispatch
    }
public:
    Derived& on_click(/*...*/) {
        // ...
        return *static_cast<Derived*>(this);  // Return derived type
    }
};

class Button : public ObjectView,
               public ObjectMixin<Button>,    // Gets size(), width(), etc.
               public EventMixin<Button>,     // Gets on_click(), on_event(), etc.
               public StyleMixin<Button> {    // Gets bg_color(), padding(), etc.
    // All methods return Button& for fluent chaining
};
```

### Trampoline Functions

Member function callbacks compile to ~8 instructions, identical to hand-written C:

```cpp
template<auto MemFn, typename T>
struct MemberTrampoline {
    static void callback(lv_event_t* e) {
        auto* instance = static_cast<T*>(lv_event_get_user_data(e));
        (instance->*MemFn)(e);
    }
};
```

The member function pointer `MemFn` is a template parameter, resolved at compile-time with zero runtime storage.

### ComponentData and Magic Tags

Components store their `this` pointer in the root widget's `user_data` using a `ComponentData` wrapper with a magic tag for safe identification:

```cpp
struct ComponentData {
    static constexpr uint32_t MAGIC = 0x4C56434D;  // "LVCM" in ASCII

    uint32_t magic;      // Verification tag
    void* component;     // Component's 'this' pointer
    void* user_payload;  // User's custom data (no collision!)
};
```

The magic value `0x4C56434D` spells "LVCM" (**LV** **C**omponent **M**agic) in ASCII, making it identifiable in memory dumps.

This enables safe component lookup from event callbacks:

```cpp
static Derived* from_event(lv_event_t* e) noexcept {
    lv_obj_t* target = lv_event_get_current_target_obj(e);

    while (target) {
        void* data = lv_obj_get_user_data(target);
        if (ComponentData::is_valid(data)) {
            auto* comp_data = static_cast<ComponentData*>(data);
            return static_cast<Derived*>(comp_data->component);
        }
        target = lv_obj_get_parent(target);
    }
    return nullptr;
}
```

### Component User Payload

To store custom data without clobbering the ComponentData, use `set_user_payload()`:

```cpp
class MyComponent : public lv::Component<MyComponent> {
    struct Context { int value; };
    Context ctx{42};

    lv::ObjectView build(lv::ObjectView parent) {
        set_user_payload(&ctx);  // Safe: stores in ComponentData::user_payload
        // ...
    }

    void later() {
        auto* c = get_user_payload_as<Context>();
    }
};
```

---

## Future Considerations

1. **Reconciler/diff engine** - For efficient virtual tree updates
2. **Key-based identity** - For list item stability
3. **Frame scheduling** - Coalesce state changes per frame

---

## References

- LVGL Documentation: https://docs.lvgl.io/
- [Naming Conventions](NAMING_CONVENTIONS.md)
- [API Guide](CPP_API_GUIDE.md)
