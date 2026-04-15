#pragma once

/**
 * @file lv.hpp
 * @brief Main header for lv:: C++20 bindings for LVGL
 *
 * Include this single header to get access to all lv:: bindings.
 *
 * Features:
 * - Zero-cost abstractions (no overhead vs raw LVGL C API)
 * - Embedded-friendly (minimal/no heap allocations)
 * - Modern C++20 API (concepts, CRTP, fluent interfaces)
 * - Reactive state with LVGL observer integration
 *
 * @code
 * #include <lv/lv.hpp>
 *
 * class MyApp : public lv::Component<MyApp> {
 *     lv::State<int> counter{0};
 *
 * public:
 *     lv::ObjectView build(lv::ObjectView parent) {
 *         auto root = lv::vbox(parent).padding(16).gap(8);
 *
 *         lv::Label::create(root)
 *             .bind_text(counter, "Count: %d");
 *
 *         lv::Button::create(root)
 *             .text("Click me")
 *             .on_click<&MyApp::on_click>(this);
 *
 *         return root;
 *     }
 *
 * private:
 *     void on_click(lv::Event) {
 *         ++counter;
 *     }
 * };
 * @endcode
 */

// LVGL base
#include <lvgl.h>

// Core
#include "core/version.hpp"
#include "core/constants.hpp"
#include "core/object.hpp"
#include "core/event.hpp"
#include "core/style.hpp"
#include "core/objectref.hpp"
#include "core/color.hpp"
#include "core/math.hpp"
#include "core/font.hpp"
#include "core/display.hpp"
#include "core/app.hpp"
#include "core/component.hpp"
#include "core/anim.hpp"
#include "core/anim_timeline.hpp"
#include "core/theme.hpp"
#include "core/screen.hpp"
#include "core/indev.hpp"
#include "core/focus.hpp"
#include "core/timer.hpp"
#include "core/image.hpp"
#include "core/fs.hpp"
#include "core/font_loader.hpp"
#include "core/string_utils.hpp"
#include "core/async.hpp"

#include "core/log.hpp"

// Misc utilities
#include "misc/gradient.hpp"

// Internationalization (requires LV_USE_TRANSLATION)
#if LV_USE_TRANSLATION
#include "core/translation.hpp"
#endif

// Snapshot (requires LV_USE_SNAPSHOT)
#if LV_USE_SNAPSHOT
#include "core/snapshot.hpp"
#endif

// Grid navigation (requires LV_USE_GRIDNAV)
#if LV_USE_GRIDNAV
#include "core/gridnav.hpp"
#endif

// System monitor (requires LV_USE_SYSMON)
#if LV_USE_SYSMON
#include "others/sysmon.hpp"
#endif

// Monkey testing (requires LV_USE_MONKEY)
#if LV_USE_MONKEY
#include "others/monkey.hpp"
#endif

// Fragment lifecycle (requires LV_USE_FRAGMENT)
#if LV_USE_FRAGMENT
#include "others/fragment.hpp"
#endif

// Layout
#if LV_USE_FLEX
#include "layout/flex.hpp"
#endif
#if LV_USE_GRID
#include "layout/grid.hpp"
#endif

// Widgets - Basic (Box is always available: plain lv_obj_create wrapper)
#include "widgets/box.hpp"
#if LV_USE_LABEL
#include "widgets/label.hpp"
#endif
#if LV_USE_BUTTON
#include "widgets/button.hpp"
#endif
#if LV_USE_IMAGE
#include "widgets/image.hpp"
#endif
#if LV_USE_LINE
#include "widgets/line.hpp"
#endif
#if LV_USE_LED
#include "widgets/led.hpp"
#endif

// Widgets - Input
#if LV_USE_SWITCH
#include "widgets/switch.hpp"
#endif
#if LV_USE_SLIDER
#include "widgets/slider.hpp"
#endif
#if LV_USE_DROPDOWN
#include "widgets/dropdown.hpp"
#endif
#if LV_USE_CHECKBOX
#include "widgets/checkbox.hpp"
#endif
#if LV_USE_ROLLER
#include "widgets/roller.hpp"
#endif
#if LV_USE_TEXTAREA
#include "widgets/textarea.hpp"
#endif
#if LV_USE_SPINBOX
#include "widgets/spinbox.hpp"
#endif
#if LV_USE_KEYBOARD
#include "widgets/keyboard.hpp"
#endif
#if LV_USE_BUTTONMATRIX
#include "widgets/buttonmatrix.hpp"
#endif

// Widgets - Display
#if LV_USE_ARC
#include "widgets/arc.hpp"
#endif
#if LV_USE_BAR
#include "widgets/bar.hpp"
#endif
#if LV_USE_SPINNER
#include "widgets/spinner.hpp"
#endif
#if LV_USE_CHART
#include "widgets/chart.hpp"
#endif
#if LV_USE_SCALE
#include "widgets/scale.hpp"
#endif
#if LV_USE_TABLE
#include "widgets/table.hpp"
#endif
#if LV_USE_CANVAS
#include "widgets/canvas.hpp"
#endif
#if LV_USE_ANIMIMG
#include "widgets/animimage.hpp"
#endif
#if LV_USE_SPAN
#include "widgets/span.hpp"
#endif

// Widgets - Containers
#if LV_USE_LIST
#include "widgets/list.hpp"
#endif
#if LV_USE_MENU
#include "widgets/menu.hpp"
#endif
#if LV_USE_TABVIEW
#include "widgets/tabview.hpp"
#endif
#if LV_USE_TILEVIEW
#include "widgets/tileview.hpp"
#endif
#if LV_USE_WIN
#include "widgets/win.hpp"
#endif
#if LV_USE_MSGBOX
#include "widgets/msgbox.hpp"
#endif
#if LV_USE_CALENDAR
#include "widgets/calendar.hpp"
#endif
#if LV_USE_IMAGEBUTTON
#include "widgets/imagebutton.hpp"
#endif

// Widgets - Other (conditional)
#if LV_USE_FILE_EXPLORER
#include "widgets/file_explorer.hpp"
#endif
#if LV_USE_ARCLABEL
#include "widgets/arclabel.hpp"
#endif
#if LV_USE_IME_PINYIN
#include "widgets/ime_pinyin.hpp"
#endif
#if LV_USE_3DTEXTURE
#include "widgets/texture3d.hpp"
#endif
#if LV_USE_LOTTIE
#include "widgets/lottie.hpp"
#endif

// Libs (optional)
#if LV_USE_QRCODE
#include "libs/qrcode.hpp"
#endif
#if LV_USE_BARCODE
#include "libs/barcode.hpp"
#endif
#if LV_USE_GIF
#include "libs/gif.hpp"
#endif
#if LV_USE_GLTF
#include "libs/gltf.hpp"
#endif
#if LV_USE_SVG
#include "libs/svg.hpp"
#endif

// State (requires LV_USE_OBSERVER)
#if LV_USE_OBSERVER
#include "core/state.hpp"
#endif

/**
 * @namespace lv
 * @brief Modern C++20 bindings for LVGL
 *
 * The lv:: namespace provides zero-cost C++ wrappers for LVGL.
 *
 * ## Key Components
 *
 * ### Object Wrappers
 * - `ObjectView` - Non-owning base wrapper with getters and tree navigation (zero overhead)
 * - `ObjectRef` - Non-owning reference with full fluent API (ObjectMixin + EventMixin + StyleMixin)
 * - `Object` - Owning wrapper with RAII (move-only)
 * - `lv::ref(ptr)` - Free function to get an ObjectRef from a raw lv_obj_t*
 *
 * ObjectView is the base class for all widgets and serves as the parameter type.
 * Its parent()/child() methods return ObjectRef for seamless tree navigation.
 * ObjectRef adds the fluent setters (size, style, events) and is returned by
 * methods that hand out sub-objects (root(), add_button(), tab_bar(), etc.).
 *
 * ### Widgets
 * - `Label` - Text display
 * - `Button` - Clickable button
 * - `Flex` - Flex layout container
 *
 * ### Layout DSL
 * - `hbox()` - Horizontal flex container
 * - `vbox()` - Vertical flex container
 *
 * ### Reactive State
 * - `State<T>` - Reactive value (triggers UI updates)
 * - `IntState`, `BoolState`, `ColorState` - Common specializations
 *
 * ### Components
 * - `Component<Derived>` - CRTP base for UI components
 * - `ScreenComponent<Derived>` - Full-screen component
 *
 * ### Styling
 * - `Style` - RAII style object
 * - `StyleMixin` - Inline style setters
 * - `colors::` - Common color constants
 *
 * ### Events
 * - `EventMixin` - Zero-cost event callbacks
 * - `on_click()`, `on_value_changed()`, etc.
 *
 * ## Memory Model
 *
 * All wrappers are designed for zero or minimal heap allocation:
 * - Widget wrappers are just pointers (8 bytes)
 * - State<T> embeds lv_subject_t (no heap)
 * - Style embeds lv_style_t (no heap)
 * - Event callbacks use user_data (no heap)
 *
 * ## Thread Safety
 *
 * LVGL is single-threaded. All lv:: operations must be called
 * from the same thread as lv_timer_handler().
 */
namespace lv {

// ==================== Version Info ====================

/// Library version
constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

/// Get version string
constexpr const char* version() noexcept {
    return "0.1.0";
}

// Screen helpers are provided by core/screen.hpp

/// Get the layer_top (always on top)
inline ObjectRef layer_top() noexcept {
    return ObjectRef(lv_layer_top());
}

/// Get the layer_sys (system layer)
inline ObjectRef layer_sys() noexcept {
    return ObjectRef(lv_layer_sys());
}

// ==================== Display Helpers ====================

/// Get default display width
inline int32_t display_width() noexcept {
    return lv_display_get_horizontal_resolution(lv_display_get_default());
}

/// Get default display height
inline int32_t display_height() noexcept {
    return lv_display_get_vertical_resolution(lv_display_get_default());
}

// ==================== Timer/Task Helpers ====================

/// Run LVGL timer handler (call periodically)
inline uint32_t timer_handler() noexcept {
    return lv_timer_handler();
}

/// Get elapsed milliseconds
inline uint32_t tick_get() noexcept {
    return lv_tick_get();
}

/// Advance LVGL tick by given milliseconds (for testing or custom tick sources)
inline void tick_inc(uint32_t ms) noexcept {
    lv_tick_inc(ms);
}

} // namespace lv
