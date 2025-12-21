# lv:: C++ Bindings Naming Conventions

## Overview

This document defines the naming conventions for the lv:: C++20 bindings for LVGL.

## General Rules

| Element | Case | Example |
|---------|------|---------|
| Classes | `PascalCase` | `lv::Chart`, `lv::Grid`, `lv::Button` |
| Type aliases | `PascalCase` | `lv::Color`, `lv::Opacity`, `lv::Event` |
| Nested type structs | `PascalCase` | `lv::Chart::Type`, `lv::Grid::Align` |
| Global constant namespaces | `kPrefixed` | `lv::kState`, `lv::kPart`, `lv::kFlag` |
| Constant values | `snake_case` | `lv::Chart::Type::line`, `lv::kState::checked` |
| Functions | `snake_case` | `lv::Grid::fr(1)`, `lv::rgb(0xff0000)` |
| Namespaces | `snake_case` | `lv::colors`, `lv::symbol`, `lv::fonts` |
| Member functions | `snake_case` | `.bg_color()`, `.fill_width()`, `.on_click()` |

## Detailed Guidelines

### 1. Classes (PascalCase)

Widget and utility classes use PascalCase:

```cpp
lv::Label
lv::Button
lv::Chart
lv::Grid
lv::Screen
lv::Timer
lv::Style
```

### 2. Type Aliases (PascalCase)

Type aliases for LVGL C types use PascalCase:

```cpp
lv::Color       // alias for lv_color_t
lv::Opacity     // alias for lv_opa_t
lv::Event       // wrapper class for lv_event_t
lv::EventData   // alias for lv_event_t (raw)
lv::AnimData    // alias for lv_anim_t (raw)
```

### 3. Nested Type Structs (PascalCase)

Constants related to a specific class are nested inside it as PascalCase structs:

```cpp
class Chart {
    struct Type {
        static constexpr auto line = LV_CHART_TYPE_LINE;
        static constexpr auto bar = LV_CHART_TYPE_BAR;
    };
    struct Axis {
        static constexpr auto primary_y = LV_CHART_AXIS_PRIMARY_Y;
    };
};

// Usage:
lv::Chart::Type::line
lv::Chart::Axis::primary_y
```

### 4. Global Constant Namespaces (k-prefixed)

Constants used across multiple widgets are in `k`-prefixed namespaces with snake_case values:

```cpp
// Note: kState (not State) to avoid conflict with State<T> reactive state template
namespace kState {
    constexpr auto checked = LV_STATE_CHECKED;
    constexpr auto pressed = LV_STATE_PRESSED;
    constexpr auto disabled = LV_STATE_DISABLED;
}

namespace kPart {
    constexpr auto main = LV_PART_MAIN;
    constexpr auto indicator = LV_PART_INDICATOR;
}

namespace kFlag {
    constexpr auto scrollable = LV_OBJ_FLAG_SCROLLABLE;
    constexpr auto clickable = LV_OBJ_FLAG_CLICKABLE;
}

// Usage:
lv::kState::checked
lv::kPart::main
lv::kFlag::scrollable
```

### 5. Constant Values (snake_case)

Individual constant values inside namespaces use snake_case:

```cpp
lv::Chart::Type::line           // not Line
lv::Chart::Type::scatter        // not Scatter
lv::kState::checked             // not Checked
lv::kPart::indicator            // not Indicator
lv::Grid::template_last         // not TemplateLast
lv::screen_anim::move_left      // not MoveLeft
```

### 6. Functions (snake_case)

All functions, including static member functions, use snake_case:

```cpp
lv::Grid::fr(1)                 // fractional unit
lv::rgb(0xff0000)               // create color from hex value
lv::rgb(255, 0, 0)              // create color from RGB components
lv::hsv_to_rgb(h, s, v)         // convert HSV to RGB
lv::rgb_to_hsv(color)           // convert RGB to HSV
lv::screen_active()             // get active screen
lv::init()                      // initialize LVGL
```

### 7. Namespaces (snake_case)

Namespaces for grouping related items use snake_case:

```cpp
namespace lv::colors {
    inline Color red() { ... }
    inline Color blue() { ... }
}

namespace lv::symbol {
    constexpr const char* wifi = LV_SYMBOL_WIFI;
    constexpr const char* home = LV_SYMBOL_HOME;
}

namespace lv::fonts {
    inline const Font montserrat_14{...};
    inline const Font montserrat_20{...};
}

namespace lv::anim_path {
    constexpr anim_path_cb linear = lv_anim_path_linear;
    constexpr anim_path_cb ease_in = lv_anim_path_ease_in;
}
```

### 8. Member Functions (snake_case)

All member functions use snake_case for consistency:

```cpp
button.text("Click me")
      .bg_color(lv::rgb(0x2196F3))  // blue
      .fill_width()
      .on_click<&MyClass::handle_click>(this);

label.text("Hello")
     .text_color(lv::colors::white())
     .center();
```

## Summary Table

| What | Convention | Examples |
|------|------------|----------|
| Class name | `PascalCase` | `Chart`, `Button`, `Label` |
| Type alias | `PascalCase` | `Color`, `Event`, `Opacity` |
| Nested struct | `PascalCase` | `Chart::Type`, `Grid::Align` |
| Global struct | `PascalCase` | `ObjState`, `Part`, `Flag` |
| Constant value | `snake_case` | `Type::line`, `ObjState::checked` |
| Function | `snake_case` | `fr()`, `hex()`, `rgb()` |
| Namespace | `snake_case` | `colors`, `symbol`, `fonts` |
| Member method | `snake_case` | `bg_color()`, `fill_width()` |

## Exception: LV_SYMBOL_* Macros

`LV_SYMBOL_*` macros must be used directly when concatenating with string literals
because they are preprocessor macros, not `const char*` pointers:

```cpp
// This works (macro concatenation at compile time):
label.text(LV_SYMBOL_WIFI " WiFi");

// This does NOT work (can't concatenate const char* with string literal):
label.text(lv::symbol::wifi " WiFi");  // Error!

// Use lv::symbol::* only for standalone symbols:
label.text(lv::symbol::wifi);  // OK
```

## Rule: Don't Hide Pointers with Type Aliases

Type aliases should NOT be used to hide that something is a pointer. Pointers should
be visible in the code so readers understand the semantics:

```cpp
// BAD - hides pointer nature
using ChartSeries = lv_chart_series_t*;
ChartSeries m_ser1 = nullptr;  // Looks like a value type!

// GOOD - pointer is visible
lv_chart_series_t* m_ser1 = nullptr;  // Clear that it's a pointer
```

Type aliases ARE appropriate for:
- Value types: `using Color = lv_color_t;`
- Callback types: `using anim_path_cb = lv_anim_path_cb_t;`
