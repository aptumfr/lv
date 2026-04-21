#pragma once

/**
 * @file object.hpp
 * @brief Zero-cost RAII wrappers for LVGL objects
 *
 * Provides ObjectView (non-owning) and Object (owning) wrappers for lv_obj_t.
 * ObjectView is a thin wrapper with zero overhead - just a pointer.
 * Object adds RAII semantics for automatic cleanup.
 */

#include <lvgl.h>
#include <utility>
#include <cstdint>
#include <concepts>
#include "wrap.hpp"
#include "version.hpp"

namespace lv {

// Forward declarations
class Style;
class ObjectRef;
class Display;

// ==================== Type Traits ====================

namespace detail {

/**
 * @brief Constrains type-identification templates on ObjectView.
 *
 * A wrapper type @p T participates in `is<T>()` / `inherits_from<T>()` /
 * `as<T>()` only if it exposes a static `class_ptr()` that returns the
 * LVGL class it wraps.
 *
 * @note The following wrappers do NOT satisfy this concept — deliberately:
 *
 *   - `lv::Flex`, `lv::Grid`, `lv::Screen` — all facades over
 *     `&lv_obj_class` (the same class `lv::Box` uses), so a class-
 *     level check can't distinguish them. If they satisfied the
 *     concept, `obj.is<Flex>()` would return `true` for any plain
 *     container regardless of layout, masking real logic bugs.
 *     Detect flex/grid via `get_layout()`, and screens via
 *     `parent() == nullptr`.
 *
 *   - `lv::Lottie`, `lv::GLTF` — LVGL builds their class pointers
 *     privately (no `extern` declaration), so they cannot be
 *     referenced from user code. Manufacturing a `class_ptr()` for
 *     these would be a fake contract.
 *
 * Attempting `obj.is<Flex>()` / `as<Screen>()` / etc. on an opt-out
 * type fails to compile with a clear concept-constraint error.
 */
template<typename T>
concept has_lv_class = requires {
    { T::class_ptr() } -> std::convertible_to<const lv_obj_class_t*>;
};

} // namespace detail

// ==================== Feature Detection ====================

/// Compile-time check for object naming support (for UI automation)
#ifdef LV_USE_OBJ_NAME
inline constexpr bool has_obj_name = LV_USE_OBJ_NAME;
#else
inline constexpr bool has_obj_name = false;
#endif

// ==================== Symbol Constants ====================

namespace symbol {
    constexpr const char* left = LV_SYMBOL_LEFT;
    constexpr const char* right = LV_SYMBOL_RIGHT;
    constexpr const char* up = LV_SYMBOL_UP;
    constexpr const char* down = LV_SYMBOL_DOWN;
    constexpr const char* ok = LV_SYMBOL_OK;
    constexpr const char* close = LV_SYMBOL_CLOSE;
    constexpr const char* plus = LV_SYMBOL_PLUS;
    constexpr const char* minus = LV_SYMBOL_MINUS;
    constexpr const char* home = LV_SYMBOL_HOME;
    constexpr const char* settings = LV_SYMBOL_SETTINGS;
    constexpr const char* wifi = LV_SYMBOL_WIFI;
    constexpr const char* bluetooth = LV_SYMBOL_BLUETOOTH;
    constexpr const char* volume_max = LV_SYMBOL_VOLUME_MAX;
    constexpr const char* volume_mid = LV_SYMBOL_VOLUME_MID;
    constexpr const char* mute = LV_SYMBOL_MUTE;
    constexpr const char* play = LV_SYMBOL_PLAY;
    constexpr const char* pause = LV_SYMBOL_PAUSE;
    constexpr const char* stop = LV_SYMBOL_STOP;
    constexpr const char* prev = LV_SYMBOL_PREV;
    constexpr const char* next = LV_SYMBOL_NEXT;
    constexpr const char* refresh = LV_SYMBOL_REFRESH;
    constexpr const char* edit = LV_SYMBOL_EDIT;
    constexpr const char* trash = LV_SYMBOL_TRASH;
    constexpr const char* save = LV_SYMBOL_SAVE;
    constexpr const char* file = LV_SYMBOL_FILE;
    constexpr const char* folder = LV_SYMBOL_DIRECTORY;
    constexpr const char* upload = LV_SYMBOL_UPLOAD;
    constexpr const char* download = LV_SYMBOL_DOWNLOAD;
    constexpr const char* copy = LV_SYMBOL_COPY;
    constexpr const char* cut = LV_SYMBOL_CUT;
    constexpr const char* paste = LV_SYMBOL_PASTE;
    constexpr const char* warning = LV_SYMBOL_WARNING;
    constexpr const char* list = LV_SYMBOL_LIST;
    constexpr const char* power = LV_SYMBOL_POWER;
    constexpr const char* eye_open = LV_SYMBOL_EYE_OPEN;
    constexpr const char* eye_close = LV_SYMBOL_EYE_CLOSE;
}

/**
 * @brief Non-owning view of an LVGL object
 *
 * This is the base class for all widget wrappers. It holds a raw pointer
 * to lv_obj_t without managing its lifetime. Use this when the object
 * lifetime is managed by LVGL's parent-child relationship.
 *
 * Size: sizeof(void*) - typically 4 or 8 bytes
 * Overhead: Zero - just a pointer wrapper
 */
class ObjectView {
protected:
    lv_obj_t* m_obj;

public:
    /// Construct from raw LVGL object pointer
    constexpr explicit ObjectView(lv_obj_t* obj) noexcept : m_obj(obj) {}

    /// Default constructor creates null view
    constexpr ObjectView() noexcept : m_obj(nullptr) {}

    /// Get the underlying LVGL object pointer
    [[nodiscard]] constexpr lv_obj_t* get() const noexcept { return m_obj; }

    /// Implicit conversion to lv_obj_t* for C API interop.
    ///
    /// @warning This conversion is a sharp edge. It lets any widget be
    /// passed to any C API that takes `lv_obj_t*`, including destructive
    /// ones like `lv_obj_delete()` — which will free the underlying
    /// object without the owning wrapper (lv::Object, lv::Component,
    /// lv::ScreenComponent) knowing about it, leading to use-after-free
    /// the next time the wrapper's destructor runs.
    ///
    /// It can also cause surprising overload resolution when a function
    /// has both `f(ObjectView)` and `f(lv_obj_t*)` candidates, and
    /// interacts awkwardly with null comparisons (`widget == nullptr`
    /// routes through this operator, while `if (widget)` routes through
    /// `explicit operator bool`).
    ///
    /// Rules of thumb:
    ///   - For destructive or ownership-affecting C calls
    ///     (lv_obj_delete, lv_obj_set_parent, etc.), prefer the C++
    ///     wrapper (.del(), .set_parent(), etc.) so owning wrappers
    ///     stay in sync.
    ///   - For read-only C API calls that have no wrapper equivalent,
    ///     pass `.get()` explicitly so the intent is visible at the
    ///     call site.
    [[nodiscard]] constexpr operator lv_obj_t*() const noexcept { return m_obj; }

    /// Check if the view points to a valid object
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return m_obj != nullptr; }

    /// Check if two views point to the same object
    [[nodiscard]] constexpr bool operator==(const ObjectView& other) const noexcept {
        return m_obj == other.m_obj;
    }

    // ==================== User Data ====================

    /// Get user data pointer
    [[nodiscard]] void* get_user_data() const noexcept {
        return lv_obj_get_user_data(m_obj);
    }

    /// Get user data as typed pointer
    template<typename T>
    [[nodiscard]] T* get_user_data() const noexcept {
        return static_cast<T*>(lv_obj_get_user_data(m_obj));
    }

    // ==================== Getters (non-fluent, always public) ====================

    /// Check if object has state
    [[nodiscard]] bool has_state(lv_state_t state) const noexcept {
        return lv_obj_has_state(m_obj, state);
    }

    /// Get the full state bitmask (LV_STATE_* flags OR'd together)
    [[nodiscard]] lv_state_t get_state() const noexcept {
        return lv_obj_get_state(m_obj);
    }

    /// Check if object has flag
    [[nodiscard]] bool has_flag(lv_obj_flag_t flag) const noexcept {
        return lv_obj_has_flag(m_obj, flag);
    }

    // ==================== Type Identification ====================

    /// Get the object's LVGL class pointer.
    [[nodiscard]] const lv_obj_class_t* get_class() const noexcept {
        return lv_obj_get_class(m_obj);
    }

    /// Check that the object's exact class matches @p cls (no inheritance).
    /// Equivalent to LVGL's `lv_obj_check_type`.
    [[nodiscard]] bool check_type(const lv_obj_class_t* cls) const noexcept {
        return lv_obj_check_type(m_obj, cls);
    }

    /// Check that the object's class is @p cls or any descendant of it.
    /// Equivalent to LVGL's `lv_obj_has_class`.
    [[nodiscard]] bool has_class(const lv_obj_class_t* cls) const noexcept {
        return lv_obj_has_class(m_obj, cls);
    }

    /// Exact-type check for a C++ widget wrapper.
    ///
    /// Usage: `if (child.is<lv::Label>()) { ... }`
    ///
    /// Constrained to wrappers that expose `T::class_ptr()` — most
    /// distinct LVGL widgets. See `detail::has_lv_class` for which
    /// families deliberately opt out (Flex/Grid/Screen/Lottie/GLTF).
    template<detail::has_lv_class T>
    [[nodiscard]] bool is() const noexcept {
        return lv_obj_check_type(m_obj, T::class_ptr());
    }

    /// Inheritance-aware check for a C++ widget wrapper.
    /// True if the object is of type @p T or any widget derived from it.
    template<detail::has_lv_class T>
    [[nodiscard]] bool inherits_from() const noexcept {
        return lv_obj_has_class(m_obj, T::class_ptr());
    }

    /// Typed conversion to a C++ widget wrapper.
    ///
    /// Returns a valid @p T wrapping this object if the exact class
    /// matches; otherwise returns a null @p T (`T(wrap, nullptr)`).
    ///
    /// Usage:
    /// @code
    ///   if (auto lbl = child.as<lv::Label>()) lbl.text("hi");
    /// @endcode
    ///
    /// @note The mismatch path uses the `(wrap_t, lv_obj_t*)` constructor
    ///       with a null pointer — NOT the default constructor — because
    ///       some wrappers (e.g. `lv::Screen`) create a new LVGL object
    ///       in their default constructor, which would leak on mismatch.
    ///       Every type satisfying `has_lv_class` provides the wrap
    ///       constructor (that's the insertion point for `class_ptr()`).
    template<detail::has_lv_class T>
    [[nodiscard]] T as() const noexcept {
        return is<T>() ? T(wrap, m_obj) : T(wrap, nullptr);
    }

    // ==================== Parent/Child ====================

    /// Get parent object
    [[nodiscard]] inline ObjectRef parent() const noexcept;

    /// Get the screen containing this object
    [[nodiscard]] inline ObjectRef screen() const noexcept;

    /// Get the display containing this object
    [[nodiscard]] inline Display display() const noexcept;

    /// Get child count
    [[nodiscard]] uint32_t child_count() const noexcept {
        return lv_obj_get_child_count(m_obj);
    }

    /// Get child by index
    [[nodiscard]] inline ObjectRef child(int32_t idx) const noexcept;

    /// Get child by index, considering only children of a given LVGL class
    [[nodiscard]] inline ObjectRef child_by_type(int32_t idx,
                                                 const lv_obj_class_t* class_p) const noexcept;

    /// Get the number of children of a given LVGL class
    [[nodiscard]] uint32_t child_count_by_type(const lv_obj_class_t* class_p) const noexcept {
        return lv_obj_get_child_count_by_type(m_obj, class_p);
    }

    /// Get a sibling relative to this object (0 = self, -1 = previous, 1 = next)
    [[nodiscard]] inline ObjectRef sibling(int32_t idx) const noexcept;

    /// Get a sibling relative to this object, considering only siblings of a given LVGL class
    [[nodiscard]] inline ObjectRef sibling_by_type(int32_t idx,
                                                   const lv_obj_class_t* class_p) const noexcept;

    /// Get this object's index within its parent (-1 if no parent)
    [[nodiscard]] int32_t get_index() const noexcept {
        return lv_obj_get_index(m_obj);
    }

    /// Get this object's index among siblings of a given LVGL class (-1 if not found or no parent)
    [[nodiscard]] int32_t get_index_by_type(const lv_obj_class_t* class_p) const noexcept {
        return lv_obj_get_index_by_type(m_obj, class_p);
    }

    /// Find a descendant by name using breadth-first search
    [[nodiscard]] inline ObjectRef find_by_name(const char* name) const noexcept;

    /// Find a descendant by name path relative to this object
    [[nodiscard]] inline ObjectRef child_by_name(const char* name_path) const noexcept;

    // ==================== Deletion ====================

    /// Delete the LVGL object (use with caution - invalidates this view)
    void del() noexcept {
        if (m_obj) {
            lv_obj_delete(m_obj);
            m_obj = nullptr;
        }
    }

    /// Delete all children
    void clean() noexcept {
        lv_obj_clean(m_obj);
    }

    // ==================== Geometry Getters ====================
    //
    // Two flavors of geometry accessors are exposed:
    //
    //   get_x()        — LAYOUT-RESOLVED x (what LVGL has actually placed)
    //   get_style_x()  — STYLE x (the last value written via pos()/x())
    //
    // LVGL resolves layout lazily, so a value just written via `pos()` /
    // `x()` / `y()` will not be reflected in the layout-resolved getters
    // until LVGL has run a layout pass (typically on the next refresh
    // or after an explicit `update_layout()` call).
    //
    // Use the layout-resolved getters when you want to know where the
    // object actually is on screen. Use the style getters when you want
    // to read back the exact value you just set without waiting for
    // layout. This dichotomy mirrors LVGL's own C API.

    /// Get object x position (layout-resolved).
    [[nodiscard]] int32_t get_x() const noexcept {
        return lv_obj_get_x(m_obj);
    }

    /// Get object y position (layout-resolved).
    [[nodiscard]] int32_t get_y() const noexcept {
        return lv_obj_get_y(m_obj);
    }

    /// Get object width (layout-resolved).
    [[nodiscard]] int32_t get_width() const noexcept {
        return lv_obj_get_width(m_obj);
    }

    /// Get object height (layout-resolved).
    [[nodiscard]] int32_t get_height() const noexcept {
        return lv_obj_get_height(m_obj);
    }

    /// Get the style x value (last set, NOT layout-resolved).
    [[nodiscard]] int32_t get_style_x() const noexcept {
        return lv_obj_get_style_x(m_obj, LV_PART_MAIN);
    }

    /// Get the style y value (last set, NOT layout-resolved).
    [[nodiscard]] int32_t get_style_y() const noexcept {
        return lv_obj_get_style_y(m_obj, LV_PART_MAIN);
    }

    /// Get the style width value (last set, NOT layout-resolved).
    [[nodiscard]] int32_t get_style_width() const noexcept {
        return lv_obj_get_style_width(m_obj, LV_PART_MAIN);
    }

    /// Get the style height value (last set, NOT layout-resolved).
    [[nodiscard]] int32_t get_style_height() const noexcept {
        return lv_obj_get_style_height(m_obj, LV_PART_MAIN);
    }

    /// Get content width (excluding padding)
    [[nodiscard]] int32_t content_width() const noexcept {
        return lv_obj_get_content_width(m_obj);
    }

    /// Get content height (excluding padding)
    [[nodiscard]] int32_t content_height() const noexcept {
        return lv_obj_get_content_height(m_obj);
    }

    /// Get object coordinates
    void get_coords(lv_area_t* area) const noexcept {
        lv_obj_get_coords(m_obj, area);
    }

    // ==================== Scroll Getters ====================

    /// Get horizontal scroll position
    [[nodiscard]] int32_t scroll_x() const noexcept {
        return lv_obj_get_scroll_x(m_obj);
    }

    /// Get vertical scroll position
    [[nodiscard]] int32_t scroll_y() const noexcept {
        return lv_obj_get_scroll_y(m_obj);
    }

    // ==================== Extended Draw Size ====================

    /// Calculate the extra draw size needed for a part (shadow, outline, etc.)
    [[nodiscard]] int32_t calculate_ext_draw_size(lv_part_t part = LV_PART_MAIN) const noexcept {
        return lv_obj_calculate_ext_draw_size(m_obj, part);
    }

};


/**
 * @brief Owning wrapper for LVGL object with RAII semantics
 *
 * Object owns the lv_obj_t and will delete it in destructor.
 * Move-only to prevent double deletion.
 *
 * Note: In LVGL, parent owns children. When you create a widget with a parent,
 * the parent will delete children automatically. Use release() to transfer
 * ownership to LVGL's parent-child system.
 *
 * Size: sizeof(void*) - typically 4 or 8 bytes
 * Overhead: Just destructor call (inlined)
 */
class Object : public ObjectView {
public:
    /// Create a basic object with parent
    explicit Object(lv_obj_t* parent)
        : ObjectView(lv_obj_create(parent)) {}

    /// Take ownership of existing object
    struct adopt_t {};
    static constexpr adopt_t adopt{};

    Object(adopt_t, lv_obj_t* obj) noexcept
        : ObjectView(obj) {}

    /// Destructor deletes the LVGL object
    ~Object() noexcept {
        if (m_obj) {
            lv_obj_delete(m_obj);
        }
    }

    // Non-copyable
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    // Move-only
    Object(Object&& other) noexcept : ObjectView(other.m_obj) {
        other.m_obj = nullptr;
    }

    Object& operator=(Object&& other) noexcept {
        if (this != &other) {
            if (m_obj) {
                lv_obj_delete(m_obj);
            }
            m_obj = other.m_obj;
            other.m_obj = nullptr;
        }
        return *this;
    }

    /// Release ownership and return raw pointer
    /// After this call, the object is no longer managed
    [[nodiscard]] lv_obj_t* release() noexcept {
        auto* p = m_obj;
        m_obj = nullptr;
        return p;
    }

    /// Reset to manage a different object (deletes current if any)
    void reset(lv_obj_t* obj = nullptr) noexcept {
        if (m_obj) {
            lv_obj_delete(m_obj);
        }
        m_obj = obj;
    }
};

// ==================== Zero-Cost Verification ====================

// Static assertions to verify zero-overhead abstraction at compile time
// These ensure ObjectView is exactly the size of a pointer
static_assert(sizeof(ObjectView) == sizeof(void*),
    "ObjectView must be exactly pointer-sized for zero overhead");
static_assert(sizeof(Object) == sizeof(void*),
    "Object must be exactly pointer-sized for zero overhead");

// ==================== Object Mixin for Widget Fluent API ====================

/**
 * @brief CRTP mixin providing common fluent object methods
 *
 * This mixin eliminates duplication of size(), width(), height(), pos(),
 * align(), center(), grow(), etc. across all widget wrappers.
 *
 * Widgets inherit from this and get proper fluent return types automatically.
 *
 * Example:
 * @code
 * class MyWidget : public ObjectView,
 *                  public ObjectMixin<MyWidget>,
 *                  public EventMixin<MyWidget> {
 *     // size(), width(), etc. return MyWidget& automatically
 * };
 * @endcode
 */
template<typename Derived>
class ObjectMixin {
private:
    [[nodiscard]] lv_obj_t* obj() noexcept {
        lv_obj_t* p = static_cast<Derived*>(this)->get();
        LV_ASSERT_NULL(p);
        return p;
    }

    [[nodiscard]] lv_obj_t* obj() const noexcept {
        lv_obj_t* p = static_cast<const Derived*>(this)->get();
        LV_ASSERT_NULL(p);
        return p;
    }

public:
    // ==================== Size ====================

    /// Set size in pixels
    Derived& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(obj(), w, h);
        return *static_cast<Derived*>(this);
    }

    /// Set width in pixels
    Derived& width(int32_t w) noexcept {
        lv_obj_set_width(obj(), w);
        return *static_cast<Derived*>(this);
    }

    /// Set height in pixels
    Derived& height(int32_t h) noexcept {
        lv_obj_set_height(obj(), h);
        return *static_cast<Derived*>(this);
    }

    /// Set size to content
    Derived& size_content() noexcept {
        lv_obj_set_size(obj(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        return *static_cast<Derived*>(this);
    }

    /// Set width to percentage of parent
    Derived& width_pct(int32_t pct) noexcept {
        lv_obj_set_width(obj(), LV_PCT(pct));
        return *static_cast<Derived*>(this);
    }

    /// Set height to percentage of parent
    Derived& height_pct(int32_t pct) noexcept {
        lv_obj_set_height(obj(), LV_PCT(pct));
        return *static_cast<Derived*>(this);
    }

    /// Fill parent width (100%)
    Derived& fill_width() noexcept {
        lv_obj_set_width(obj(), LV_PCT(100));
        return *static_cast<Derived*>(this);
    }

    /// Fill parent height (100%)
    Derived& fill_height() noexcept {
        lv_obj_set_height(obj(), LV_PCT(100));
        return *static_cast<Derived*>(this);
    }

    /// Fill parent (100% width and height)
    Derived& fill() noexcept {
        lv_obj_set_size(obj(), LV_PCT(100), LV_PCT(100));
        return *static_cast<Derived*>(this);
    }

    // ==================== Position ====================
    //
    // These write the local style x/y. LVGL resolves layout lazily, so
    // the value returned by `get_x()` / `get_y()` will NOT reflect what
    // you just set until LVGL has run a layout pass (typically on the
    // next refresh or after an explicit `update_layout()` call).
    //
    // If you need to read back the value immediately after setting it:
    //   - call `update_layout()` first, then `get_x()` / `get_y()`, or
    //   - use `get_style_x()` / `get_style_y()` to read the style value
    //     directly without waiting for layout.

    /// Set position relative to parent.
    /// @note Layout-resolved `get_x()`/`get_y()` will not update until a
    ///       layout pass runs. Use `get_style_x()`/`get_style_y()` for
    ///       immediate read-back. See the Position section note above.
    Derived& pos(int32_t x, int32_t y) noexcept {
        lv_obj_set_pos(obj(), x, y);
        return *static_cast<Derived*>(this);
    }

    /// Set X position.
    /// @note Layout-resolved `get_x()` will not update until a layout
    ///       pass runs. Use `get_style_x()` for immediate read-back.
    Derived& x(int32_t x) noexcept {
        lv_obj_set_x(obj(), x);
        return *static_cast<Derived*>(this);
    }

    /// Set Y position.
    /// @note Layout-resolved `get_y()` will not update until a layout
    ///       pass runs. Use `get_style_y()` for immediate read-back.
    Derived& y(int32_t y) noexcept {
        lv_obj_set_y(obj(), y);
        return *static_cast<Derived*>(this);
    }

    // ==================== Alignment ====================

    /// Align relative to parent with offset
    Derived& align(lv_align_t alignment, int32_t x_ofs, int32_t y_ofs) noexcept {
        lv_obj_align(obj(), alignment, x_ofs, y_ofs);
        return *static_cast<Derived*>(this);
    }

    /// Set alignment mode (without changing position)
    Derived& align(lv_align_t alignment) noexcept {
        lv_obj_set_align(obj(), alignment);
        return *static_cast<Derived*>(this);
    }

    /// Center in parent
    Derived& center() noexcept {
        lv_obj_center(obj());
        return *static_cast<Derived*>(this);
    }

    // ==================== Flex Layout ====================

#if LV_USE_FLEX
    /// Set flex grow factor (for children of flex containers)
    Derived& grow(uint8_t factor = 1) noexcept {
        lv_obj_set_flex_grow(obj(), factor);
        return *static_cast<Derived*>(this);
    }
#endif

    // ==================== Visibility ====================

    /// Hide the object
    Derived& hide() noexcept {
        lv_obj_add_flag(obj(), LV_OBJ_FLAG_HIDDEN);
        return *static_cast<Derived*>(this);
    }

    /// Show the object
    Derived& show() noexcept {
        lv_obj_remove_flag(obj(), LV_OBJ_FLAG_HIDDEN);
        return *static_cast<Derived*>(this);
    }

    /// Set visibility
    Derived& visible(bool v) noexcept {
        if (v) lv_obj_remove_flag(obj(), LV_OBJ_FLAG_HIDDEN);
        else lv_obj_add_flag(obj(), LV_OBJ_FLAG_HIDDEN);
        return *static_cast<Derived*>(this);
    }

    // ==================== Flags ====================

    /// Set clickable flag
    Derived& clickable(bool v = true) noexcept {
        if (v) lv_obj_add_flag(obj(), LV_OBJ_FLAG_CLICKABLE);
        else lv_obj_remove_flag(obj(), LV_OBJ_FLAG_CLICKABLE);
        return *static_cast<Derived*>(this);
    }

    /// Add object flags
    Derived& add_flag(lv_obj_flag_t flag) noexcept {
        lv_obj_add_flag(obj(), flag);
        return *static_cast<Derived*>(this);
    }

    /// Remove object flags
    Derived& remove_flag(lv_obj_flag_t flag) noexcept {
        lv_obj_remove_flag(obj(), flag);
        return *static_cast<Derived*>(this);
    }

    // ==================== State ====================

    /// Add state flags
    Derived& add_state(lv_state_t state) noexcept {
        lv_obj_add_state(obj(), state);
        return *static_cast<Derived*>(this);
    }

    /// Remove state flags
    Derived& remove_state(lv_state_t state) noexcept {
        lv_obj_remove_state(obj(), state);
        return *static_cast<Derived*>(this);
    }

    // ==================== User Data ====================

    /**
     * @brief Set user data pointer
     *
     * Safe to use on any object, including component roots.
     * Components use event-descriptor scanning for ownership lookup
     * and do not occupy user_data.
     */
    Derived& user_data(void* data) noexcept {
        lv_obj_set_user_data(obj(), data);
        return *static_cast<Derived*>(this);
    }

    /// Get user data pointer
    [[nodiscard]] void* user_data() const noexcept {
        return lv_obj_get_user_data(obj());
    }

    /// Get user data as typed pointer
    template<typename T>
    [[nodiscard]] T* user_data_as() const noexcept {
        return static_cast<T*>(lv_obj_get_user_data(obj()));
    }

    // ==================== Scrolling ====================

    /// Enable/disable scrolling
    Derived& scrollable(bool v = true) noexcept {
        if (v) lv_obj_add_flag(obj(), LV_OBJ_FLAG_SCROLLABLE);
        else lv_obj_remove_flag(obj(), LV_OBJ_FLAG_SCROLLABLE);
        return *static_cast<Derived*>(this);
    }

    /// Set scroll direction
    Derived& scroll_dir(lv_dir_t dir) noexcept {
        lv_obj_set_scroll_dir(obj(), dir);
        return *static_cast<Derived*>(this);
    }

    /// Set scrollbar mode
    Derived& scrollbar_mode(lv_scrollbar_mode_t mode) noexcept {
        lv_obj_set_scrollbar_mode(obj(), mode);
        return *static_cast<Derived*>(this);
    }

    /// Set horizontal scroll snap
    Derived& scroll_snap_x(lv_scroll_snap_t snap) noexcept {
        lv_obj_set_scroll_snap_x(obj(), snap);
        return *static_cast<Derived*>(this);
    }

    /// Set vertical scroll snap
    Derived& scroll_snap_y(lv_scroll_snap_t snap) noexcept {
        lv_obj_set_scroll_snap_y(obj(), snap);
        return *static_cast<Derived*>(this);
    }

    // ==================== Extended Click Area ====================

    /// Set extended click area (pixels outside object bounds that respond to clicks)
    Derived& ext_click_area(int32_t size) noexcept {
        lv_obj_set_ext_click_area(obj(), size);
        return *static_cast<Derived*>(this);
    }

    // ==================== Flex Layout ====================

#if LV_USE_FLEX
    /// Set flex flow direction
    Derived& flex_flow(lv_flex_flow_t flow) noexcept {
        lv_obj_set_flex_flow(obj(), flow);
        return *static_cast<Derived*>(this);
    }

    /// Set flex alignment (main, cross, and track alignment)
    Derived& flex_align(lv_flex_align_t main, lv_flex_align_t cross, lv_flex_align_t track) noexcept {
        lv_obj_set_flex_align(obj(), main, cross, track);
        return *static_cast<Derived*>(this);
    }
#endif

#if LV_USE_GRID
    // ==================== Grid Layout ====================

    /// Set grid column and row descriptors (arrays must remain valid for widget lifetime)
    Derived& grid_dsc(const int32_t* col_dsc, const int32_t* row_dsc) noexcept {
        lv_obj_set_grid_dsc_array(obj(), col_dsc, row_dsc);
        return *static_cast<Derived*>(this);
    }

    /// Place this widget in a grid cell of its parent
    Derived& grid_cell(lv_grid_align_t col_align, int32_t col, uint8_t col_span,
                       lv_grid_align_t row_align, int32_t row, uint8_t row_span) noexcept {
        lv_obj_set_grid_cell(obj(), col_align, col, col_span, row_align, row, row_span);
        return *static_cast<Derived*>(this);
    }

    /// Set grid alignment
    Derived& grid_align(lv_grid_align_t col_align, lv_grid_align_t row_align) noexcept {
        lv_obj_set_grid_align(obj(), col_align, row_align);
        return *static_cast<Derived*>(this);
    }
#endif

    // ==================== Layout ====================

    /// Invalidate (request redraw)
    Derived& invalidate() noexcept {
        lv_obj_invalidate(obj());
        return *static_cast<Derived*>(this);
    }

    /// Update layout now
    Derived& update_layout() noexcept {
        lv_obj_update_layout(obj());
        return *static_cast<Derived*>(this);
    }

    /// Set layout type (use kLayout::none, kLayout::flex, kLayout::grid)
    Derived& layout(lv_layout_t l) noexcept {
        lv_obj_set_layout(obj(), l);
        return *static_cast<Derived*>(this);
    }

    /// Disable layout (children use absolute positioning)
    Derived& layout_none() noexcept {
        lv_obj_set_layout(obj(), LV_LAYOUT_NONE);
        return *static_cast<Derived*>(this);
    }

    // ==================== Parent/Child ====================

    /// Move to new parent
    Derived& set_parent(ObjectView new_parent) noexcept {
        lv_obj_set_parent(obj(), new_parent);
        return *static_cast<Derived*>(this);
    }

    // ==================== Style ====================

    /// Add a style to the object
    Derived& add_style(const lv_style_t* style, lv_style_selector_t selector = 0) noexcept {
        lv_obj_add_style(obj(), style, selector);
        return *static_cast<Derived*>(this);
    }

    /// Prevent adding temporary Style objects (would leave dangling style pointer in LVGL)
    Derived& add_style(Style&&, lv_style_selector_t = 0) noexcept = delete;

    /// Remove a style from the object
    Derived& remove_style(const lv_style_t* style, lv_style_selector_t selector = 0) noexcept {
        lv_obj_remove_style(obj(), style, selector);
        return *static_cast<Derived*>(this);
    }

    /// Prevent removing temporary Style objects (nonsensical / likely bug)
    Derived& remove_style(Style&&, lv_style_selector_t = 0) noexcept = delete;

    /// Remove all styles
    Derived& remove_all_styles() noexcept {
        lv_obj_remove_style_all(obj());
        return *static_cast<Derived*>(this);
    }

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Remove theme styles from this object (LVGL 9.5+)
    Derived& remove_theme(lv_style_selector_t selector = 0) noexcept {
        lv_obj_remove_theme(obj(), selector);
        return *static_cast<Derived*>(this);
    }
#endif

    // ==================== Radio Button ====================

#if LV_VERSION_AT_LEAST(9, 5, 0)
    /// Enable/disable radio button behavior (LVGL 9.5+)
    /// When enabled, checking this object unchecks siblings in the same parent
    Derived& radio_button(bool en = true) noexcept {
        lv_obj_set_radio_button(obj(), en);
        return *static_cast<Derived*>(this);
    }

    /// Check if radio button behavior is enabled (LVGL 9.5+)
    [[nodiscard]] bool is_radio_button() const noexcept {
        return lv_obj_is_radio_button(obj());
    }
#endif

    // ==================== Style Binding ====================

#if LV_VERSION_AT_LEAST(9, 5, 0) && LV_USE_OBSERVER
    /// Bind a style property to a reactive subject (LVGL 9.5+)
    /// The property updates automatically when the subject value changes
    lv_observer_t* bind_style_prop(lv_style_prop_t prop, lv_style_selector_t selector,
                                   lv_subject_t* subject) noexcept {
        return lv_obj_bind_style_prop(obj(), prop, selector, subject);
    }
#endif

    // ==================== Scroll Operations ====================

    /// Scroll by given offset
    Derived& scroll_by(int32_t x, int32_t y, lv_anim_enable_t anim_en) noexcept {
        lv_obj_scroll_by(obj(), x, y, anim_en);
        return *static_cast<Derived*>(this);
    }

    /// Scroll to given coordinates
    Derived& scroll_to(int32_t x, int32_t y, lv_anim_enable_t anim_en) noexcept {
        lv_obj_scroll_to(obj(), x, y, anim_en);
        return *static_cast<Derived*>(this);
    }

    /// Scroll to a horizontal position
    Derived& scroll_to_x(int32_t x, lv_anim_enable_t anim_en) noexcept {
        lv_obj_scroll_to_x(obj(), x, anim_en);
        return *static_cast<Derived*>(this);
    }

    /// Scroll to a vertical position
    Derived& scroll_to_y(int32_t y, lv_anim_enable_t anim_en) noexcept {
        lv_obj_scroll_to_y(obj(), y, anim_en);
        return *static_cast<Derived*>(this);
    }

    /// Update scroll snap
    Derived& update_snap(lv_anim_enable_t anim_en) noexcept {
        lv_obj_update_snap(obj(), anim_en);
        return *static_cast<Derived*>(this);
    }

    // ==================== Z-Order ====================

    /// Move object to foreground (on top of siblings)
    Derived& move_foreground() noexcept {
        lv_obj_move_foreground(obj());
        return *static_cast<Derived*>(this);
    }

    /// Move object to background (behind siblings)
    Derived& move_background() noexcept {
        lv_obj_move_background(obj());
        return *static_cast<Derived*>(this);
    }

    /// Move object to a specific sibling index (-1 counts from the back)
    Derived& move_to_index(int32_t index) noexcept {
        lv_obj_move_to_index(obj(), index);
        return *static_cast<Derived*>(this);
    }

    /// Swap sibling positions with another object
    Derived& swap(ObjectView other) noexcept {
        lv_obj_swap(obj(), other.get());
        return *static_cast<Derived*>(this);
    }

    // ==================== Alignment ====================

    /// Align to another object
    Derived& align_to(ObjectView base, lv_align_t align, int32_t x_ofs = 0, int32_t y_ofs = 0) noexcept {
        lv_obj_align_to(obj(), base.get(), align, x_ofs, y_ofs);
        return *static_cast<Derived*>(this);
    }

    // ==================== Extended Draw Size ====================

    /// Trigger a refresh of the extended draw size
    Derived& refresh_ext_draw_size() noexcept {
        lv_obj_refresh_ext_draw_size(obj());
        return *static_cast<Derived*>(this);
    }

    // ==================== Object Naming ====================

    /// Set object name (requires LV_USE_OBJ_NAME in lv_conf.h)
    /// Useful for widget identification in UI automation and debugging
    Derived& name([[maybe_unused]] const char* name) noexcept {
#if LV_USE_OBJ_NAME
        lv_obj_set_name(obj(), name);
#endif
        return *static_cast<Derived*>(this);
    }

    /// Get object name (returns nullptr if not set or LV_USE_OBJ_NAME is disabled)
    [[nodiscard]] const char* get_name() const noexcept {
#if LV_USE_OBJ_NAME
        return lv_obj_get_name(obj());
#else
        return nullptr;
#endif
    }
};

} // namespace lv

// Deferred include: provides ObjectRef definition and ObjectView::parent()/child()
// bodies. Pragma-once guards prevent circular inclusion.
#include "objectref.hpp"
