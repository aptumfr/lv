# lv:: Modern C++20 Bindings for LVGL

Zero-cost, type-safe C++20 wrapper for the [LVGL](https://lvgl.io) embedded graphics library.

## Quick Start

```cpp
#include <lv/lv.hpp>

int main() {
    lv::init();
    lv::X11Display display("Hello", 480, 320);

    lv::Label::create(lv::screen_active())
        .text("Hello, LVGL!")
        .center();

    lv::run();
}
```

## Features

- **Zero-cost abstractions** - Every wrapper is `sizeof(void*)`, producing identical assembly to C
- **Type-safe events** - Compile-time verified callbacks with member function pointers
- **Fluent API** - Method chaining for declarative, readable UI code
- **Reactive state** - `State<T>` with automatic UI updates via LVGL's observer system
- **Modern C++20** - Concepts, `[[nodiscard]]`, `noexcept`, no exceptions
- **Header-only** - Easy integration, just add to include path
- **35 widgets wrapped** - Full coverage of LVGL widget set

## Example: Counter App

```cpp
class CounterApp : public lv::Component<CounterApp> {
    lv::State<int> m_count{0};

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent).fill().center_content();

        lv::Label::create(root)
            .bind_text(m_count, "Count: %d");

        auto buttons = lv::hbox(root).gap(10);

        lv::Button::create(buttons)
            .text("-")
            .on_click<&CounterApp::decrement>(this);

        lv::Button::create(buttons)
            .text("+")
            .on_click<&CounterApp::increment>(this);

        return root;
    }

private:
    void increment(lv::Event) { ++m_count; }
    void decrement(lv::Event) { --m_count; }
};
```

## Building

### Directory Structure

Place LVGL as a sibling directory:
```
parent/
├── lvgl/          # LVGL 9.4 source (git clone -b release/v9.4 https://github.com/lvgl/lvgl)
└── lv/            # This project
```

Or specify a custom path with `-DLVGL_DIR`.

### Build Commands

```bash
cd lv
cmake -B build
cmake --build build
./build/examples/hello
```

With custom LVGL path:
```bash
cmake -B build -DLVGL_DIR=/path/to/lvgl
```

### Requirements

- C++20 compiler (GCC 11+, Clang 14+, MSVC 2022+)
- CMake 3.20+
- LVGL 9.4
- X11 or SDL2 (for display backend in examples/demos)

## Documentation

- [Architecture](docs/ARCHITECTURE.md) - Design principles and patterns
- [API Guide](docs/CPP_API_GUIDE.md) - Complete API reference with C comparison
- [Naming Conventions](docs/NAMING_CONVENTIONS.md) - Consistent naming rules

## Design Philosophy

**C++ is the only UI language** - no QML, no XML, no code generation, no moc. Just pure C++20.

```cpp
// Type-safe: compiler verifies method exists and signature matches
button.on_click<&MyApp::handle_click>(this);

// Zero allocation: stateless lambdas decay to function pointers
button.on_click([](lv::Event e) { /* ... */ });

// Reactive: UI auto-updates when state changes
lv::State<int> value{0};
label.bind_text(value, "Value: %d");
value.set(42);  // Label updates automatically
```

## License

AGPL-3.0-only
