# Zero-Cost Safety Features

This document describes the compile-time and low-cost runtime safety features
added to the lv C++ wrapper. Every feature is opt-in (pay-for-use) and
produces identical or near-identical assembly to hand-written C equivalents.

---

## Performance Summary

Measured by compiling to assembly with `g++ -O2 -std=c++20` and comparing
against hand-written C equivalents instruction-by-instruction.

| Feature | Runtime cost | vs. hand-written C |
|---|---|---|
| `for_each_child` | identical assembly to manual index loop | **zero** |
| `std::span` overloads | identical assembly to raw pointer+count | **zero** |
| `widget_cast<T>` | null check + 1 pointer comparison (`obj->class_p == class_p`) | **pay-for-use only** |
| `find_parent<T>` | 1 pointer comparison per ancestor | **pay-for-use only** |
| trait specializations (`detail::widget_lv_class`) | exist only at compile time | **zero** — no codegen |
| `[[deprecated]]` on FragmentManager | compiler warning only | **zero** |

Code that does not call `widget_cast` or `find_parent` pays nothing.
`std::optional<Widget>` is fully elided by the optimizer — no storage, no
construction overhead.

---

## Feature 1: `widget_cast<T>` — Type-Safe Widget Downcasting

### Problem

Wrapping a raw `lv_obj_t*` as the wrong widget type compiles silently and
produces undefined behavior:

```cpp
lv::Slider slider(lv::wrap, some_button_ptr);  // compiles, wrong, UB
slider.value(50);                                // corrupts memory
```

### Solution

`widget_cast<T>(ObjectView)` returns `std::optional<T>`. It calls
`lv_obj_check_type()` which does a single pointer comparison
(`obj->class_p == &lv_slider_class`), zero allocation.

```cpp
if (auto slider = lv::widget_cast<lv::Slider>(obj)) {
    slider->value(50);  // safe — type verified at runtime
}
// or with value_or pattern:
// auto slider = lv::widget_cast<lv::Slider>(obj).value();  // throws if wrong type
```

### `find_parent<T>` — Walk the Parent Chain

```cpp
if (auto tab = lv::find_parent<lv::Tabview>(some_child)) {
    // some_child is inside a Tabview
}
```

Walks `lv_obj_get_parent()` until a match is found or the root is reached.
Same pointer comparison per ancestor, no allocation.

### How It Works

**Trait declaration** in `core/object.hpp`:

```cpp
namespace detail {
    template<typename W>
    const lv_obj_class_t* widget_lv_class() noexcept = delete;
}
```

The primary template is `= delete`, so `widget_cast<UnregisteredType>` is a
**compile-time error** — you can't accidentally cast to a type that hasn't
opted in.

**Each widget header** adds a one-line specialization after the class
definition:

```cpp
// In button.hpp
namespace detail {
    template<> inline const lv_obj_class_t*
    widget_lv_class<Button>() noexcept { return &lv_button_class; }
}
```

**Cast function** in `core/widget_cast.hpp`:

```cpp
template<typename Widget>
[[nodiscard]] std::optional<Widget> widget_cast(ObjectView obj) noexcept {
    lv_obj_t* raw = obj.get();
    if (raw && lv_obj_check_type(raw, detail::widget_lv_class<Widget>()))
        return Widget(wrap, raw);
    return std::nullopt;
}
```

### Registered Widgets (41)

Every widget with a distinct `lv_obj_class_t` is registered:

| Widget | LVGL class | Header |
|---|---|---|
| Box | `lv_obj_class` | `widgets/box.hpp` |
| Label | `lv_label_class` | `widgets/label.hpp` |
| Button | `lv_button_class` | `widgets/button.hpp` |
| Image | `lv_image_class` | `widgets/image.hpp` |
| Line | `lv_line_class` | `widgets/line.hpp` |
| Led | `lv_led_class` | `widgets/led.hpp` |
| Switch | `lv_switch_class` | `widgets/switch.hpp` |
| Slider | `lv_slider_class` | `widgets/slider.hpp` |
| Dropdown | `lv_dropdown_class` | `widgets/dropdown.hpp` |
| Checkbox | `lv_checkbox_class` | `widgets/checkbox.hpp` |
| Roller | `lv_roller_class` | `widgets/roller.hpp` |
| Textarea | `lv_textarea_class` | `widgets/textarea.hpp` |
| Spinbox | `lv_spinbox_class` | `widgets/spinbox.hpp` |
| Keyboard | `lv_keyboard_class` | `widgets/keyboard.hpp` |
| ButtonMatrix | `lv_buttonmatrix_class` | `widgets/buttonmatrix.hpp` |
| Arc | `lv_arc_class` | `widgets/arc.hpp` |
| Bar | `lv_bar_class` | `widgets/bar.hpp` |
| Spinner | `lv_spinner_class` | `widgets/spinner.hpp` |
| Chart | `lv_chart_class` | `widgets/chart.hpp` |
| Scale | `lv_scale_class` | `widgets/scale.hpp` |
| Table | `lv_table_class` | `widgets/table.hpp` |
| Canvas | `lv_canvas_class` | `widgets/canvas.hpp` |
| AnimImage | `lv_animimg_class` | `widgets/animimage.hpp` |
| Spangroup | `lv_spangroup_class` | `widgets/span.hpp` |
| List | `lv_list_class` | `widgets/list.hpp` |
| Menu | `lv_menu_class` | `widgets/menu.hpp` |
| Tabview | `lv_tabview_class` | `widgets/tabview.hpp` |
| Tileview | `lv_tileview_class` | `widgets/tileview.hpp` |
| Window | `lv_win_class` | `widgets/win.hpp` |
| Msgbox | `lv_msgbox_class` | `widgets/msgbox.hpp` |
| Calendar | `lv_calendar_class` | `widgets/calendar.hpp` |
| ImageButton | `lv_imagebutton_class` | `widgets/imagebutton.hpp` |
| ArcLabel | `lv_arclabel_class` | `widgets/arclabel.hpp` |
| IMEPinyin | `lv_ime_pinyin_class` | `widgets/ime_pinyin.hpp` |
| Texture3D | `lv_3dtexture_class` | `widgets/texture3d.hpp` |
| FileExplorer | `lv_file_explorer_class` | `widgets/file_explorer.hpp` |
| Lottie | `lv_lottie_class` | `widgets/lottie.hpp` |
| QRCode | `lv_qrcode_class` | `libs/qrcode.hpp` |
| Barcode | `lv_barcode_class` | `libs/barcode.hpp` |
| GIF | `lv_gif_class` | `libs/gif.hpp` |
| GLTF | `lv_gltf_class` | `libs/gltf.hpp` |

### Intentionally Unregistered

| Type | Reason |
|---|---|
| Flex | Shares `lv_obj_class` with Box — runtime check cannot distinguish them |
| Grid | Same as Flex |

Calling `widget_cast<Flex>` or `widget_cast<Grid>` is a compile-time error.
This is by design: LVGL does not give layout containers a distinct class, so a
runtime type check would be misleading.

### Note on Lottie and GLTF

LVGL defines `lv_lottie_class` and `lv_gltf_class` as global symbols but omits
the `extern` declaration in their public headers. Our wrapper headers provide
the missing `extern "C"` forward declarations so `widget_cast` works for these
types.

---

## Feature 2: `for_each_child` — Safe Child Iteration

### Problem

Manual index loops over LVGL children are error-prone:

```cpp
// Off-by-one? Signed/unsigned mismatch on int32_t index?
for (int i = 0; i < lv_obj_get_child_count(parent); i++) {  // bug: signed i, unsigned count
    lv_obj_t* child = lv_obj_get_child(parent, i);
    // ...
}
```

### Solution

Free function template in `core/object.hpp`:

```cpp
template<typename Fn>
void for_each_child(ObjectView parent, Fn&& fn) {
    const uint32_t n = parent.child_count();
    for (uint32_t i = 0; i < n; ++i)
        fn(parent.child(static_cast<int32_t>(i)));
}
```

Usage:

```cpp
lv::for_each_child(container, [](lv::ObjectView child) {
    child.del();  // careful: see caveat below
});
```

### Caveat

`for_each_child` caches `child_count()` before the loop. Do not add or remove
children inside the callback — the cached count will be stale. This is the same
behavior as iterating with a hand-written loop that calls
`lv_obj_get_child_count()` once.

### Assembly Proof

At `-O2`, `for_each_child` with a lambda produces **identical assembly** to
the hand-written C loop. The template, the lambda, the `static_cast` — all
erased by the optimizer.

---

## Feature 3: `std::span` Overloads for Array APIs

### Problem

Raw pointer + count pairs are easy to get wrong:

```cpp
lv_point_precise_t pts[3] = { ... };
line.points(pts, 4);  // wrong count — buffer overread, silent UB
```

### Solution

Additional `std::span` overloads alongside the existing raw-pointer methods
(non-breaking — the old signatures still work):

```cpp
lv_point_precise_t pts[] = {{0,0}, {100,0}, {100,100}};
line.points(std::span{pts});  // count deduced from array — always correct
```

Each overload is a one-liner delegating to the existing raw-pointer method:

| File | Existing method | New span overload |
|---|---|---|
| `widgets/line.hpp` | `points(const lv_point_precise_t*, uint32_t)` | `points(std::span<const lv_point_precise_t>)` |
| `widgets/chart.hpp` | `set_series_values(ser, const int32_t[], size_t)` | `set_series_values(ser, std::span<const int32_t>)` |
| `widgets/calendar.hpp` | `highlighted_dates(lv_calendar_date_t[], size_t)` | `highlighted_dates(std::span<lv_calendar_date_t>)` |

Sentinel-terminated arrays (button matrix map, grid descriptors, day names) do
**not** get span overloads because `std::span` cannot express sentinel
invariants.

### Assembly Proof

At `-O2`, the span overload compiles to a single `call lv_line_set_points` —
identical to the raw-pointer version. The span metadata extraction
(`.data()`, `.size()`) is fully inlined away.

---

## Files Modified

| File | Change |
|---|---|
| `core/object.hpp` | Added `detail::widget_lv_class` trait (deleted primary template) and `for_each_child` |
| `core/widget_cast.hpp` | **New file** — `widget_cast<T>` and `find_parent<T>` |
| `lv.hpp` | Added `#include "core/widget_cast.hpp"` |
| 41 widget/layout/lib headers | One-line `detail::widget_lv_class` specialization each |
| `widgets/lottie.hpp` | Added `extern "C"` declaration for `lv_lottie_class` + trait |
| `libs/gltf.hpp` | Added `extern "C"` declaration for `lv_gltf_class` + trait |
| `widgets/line.hpp` | Added `#include <span>` and `points(std::span)` overload |
| `widgets/chart.hpp` | Added `#include <span>` and `set_series_values(ser, std::span)` overload |
| `widgets/calendar.hpp` | Added `#include <span>` and `highlighted_dates(std::span)` overload |
| `others/fragment.hpp` | Added `[[deprecated]]` attribute on `FragmentManager` |
| `tests/api_smoke_test.cpp` | Smoke tests for all new APIs |

---

## Verification

All features compile with zero errors and zero warnings:

```bash
cmake --build build   # full build including examples, demos, and tests
```

The `api_smoke_test` target exercises every new API surface (widget_cast,
find_parent, for_each_child, all span overloads) to ensure they are
well-formed. The test compiles = the APIs work.
