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

namespace lv {

// ==================== Wrap Tag ====================

/// Tag type for wrapping existing lv_obj_t* without creating new object
struct wrap_t {};

/// Tag instance for wrapping existing lv_obj_t*
/// Usage: auto box = lv::Box(lv::wrap, existing_ptr);
inline constexpr wrap_t wrap{};

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

    /// Implicit conversion to lv_obj_t* for C API interop
    [[nodiscard]] constexpr operator lv_obj_t*() const noexcept { return m_obj; }

    /// Check if the view points to a valid object
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return m_obj != nullptr; }

    /// Check if two views point to the same object
    [[nodiscard]] constexpr bool operator==(const ObjectView& other) const noexcept {
        return m_obj == other.m_obj;
    }

    // ==================== Common Object Operations ====================

    /// Set user data pointer (zero-cost way to attach C++ object)
    ObjectView& user_data(void* data) noexcept {
        lv_obj_set_user_data(m_obj, data);
        return *this;
    }

    /// Get user data pointer
    [[nodiscard]] void* user_data() const noexcept {
        return lv_obj_get_user_data(m_obj);
    }

    /// Get user data as typed pointer
    template<typename T>
    [[nodiscard]] T* user_data_as() const noexcept {
        return static_cast<T*>(lv_obj_get_user_data(m_obj));
    }

    // ==================== Getters (non-fluent, always public) ====================

    /// Check if object has state
    [[nodiscard]] bool has_state(lv_state_t state) const noexcept {
        return lv_obj_has_state(m_obj, state);
    }

    /// Check if object has flag
    [[nodiscard]] bool has_flag(lv_obj_flag_t flag) const noexcept {
        return lv_obj_has_flag(m_obj, flag);
    }

    // ==================== Parent/Child ====================

    /// Get parent object
    [[nodiscard]] ObjectView parent() const noexcept {
        return ObjectView(lv_obj_get_parent(m_obj));
    }

    /// Get child count
    [[nodiscard]] uint32_t child_count() const noexcept {
        return lv_obj_get_child_count(m_obj);
    }

    /// Get child by index
    [[nodiscard]] ObjectView child(int32_t idx) const noexcept {
        return ObjectView(lv_obj_get_child(m_obj, idx));
    }

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
    ~Object() {
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
        return static_cast<Derived*>(this)->get();
    }

    [[nodiscard]] lv_obj_t* obj() const noexcept {
        return static_cast<const Derived*>(this)->get();
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

    /// Set position relative to parent
    Derived& pos(int32_t x, int32_t y) noexcept {
        lv_obj_set_pos(obj(), x, y);
        return *static_cast<Derived*>(this);
    }

    /// Set X position
    Derived& x(int32_t x) noexcept {
        lv_obj_set_x(obj(), x);
        return *static_cast<Derived*>(this);
    }

    /// Set Y position
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

    /// Set flex grow factor (for children of flex containers)
    Derived& grow(uint8_t factor = 1) noexcept {
        lv_obj_set_flex_grow(obj(), factor);
        return *static_cast<Derived*>(this);
    }

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
     * WARNING: If this object is a Component root, calling user_data() will
     * overwrite the ComponentData pointer and break Component::from_event().
     * For components, use Component::set_user_payload() instead.
     *
     * @see Component::set_user_payload() for safe user data on component roots
     */
    Derived& user_data(void* data) noexcept {
        lv_obj_set_user_data(obj(), data);
        return *static_cast<Derived*>(this);
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
    Derived& add_style(lv_style_t* style, lv_style_selector_t selector = 0) noexcept {
        lv_obj_add_style(obj(), style, selector);
        return *static_cast<Derived*>(this);
    }

    /// Remove a style from the object
    Derived& remove_style(lv_style_t* style, lv_style_selector_t selector = 0) noexcept {
        lv_obj_remove_style(obj(), style, selector);
        return *static_cast<Derived*>(this);
    }

    /// Remove all styles
    Derived& remove_all_styles() noexcept {
        lv_obj_remove_style_all(obj());
        return *static_cast<Derived*>(this);
    }

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

    // ==================== Alignment ====================

    /// Align to another object
    Derived& align_to(ObjectView base, lv_align_t align, int32_t x_ofs = 0, int32_t y_ofs = 0) noexcept {
        lv_obj_align_to(obj(), base.get(), align, x_ofs, y_ofs);
        return *static_cast<Derived*>(this);
    }

    // ==================== Geometry Getters ====================

    /// Get object width
    [[nodiscard]] int32_t get_width() const noexcept {
        return lv_obj_get_width(obj());
    }

    /// Get object height
    [[nodiscard]] int32_t get_height() const noexcept {
        return lv_obj_get_height(obj());
    }

    /// Get content width (excluding padding)
    [[nodiscard]] int32_t content_width() const noexcept {
        return lv_obj_get_content_width(obj());
    }

    /// Get content height (excluding padding)
    [[nodiscard]] int32_t content_height() const noexcept {
        return lv_obj_get_content_height(obj());
    }

    /// Get object coordinates
    void get_coords(lv_area_t* area) const noexcept {
        lv_obj_get_coords(obj(), area);
    }

    // ==================== Scroll Getters ====================

    /// Get horizontal scroll position
    [[nodiscard]] int32_t scroll_x() const noexcept {
        return lv_obj_get_scroll_x(obj());
    }

    /// Get vertical scroll position
    [[nodiscard]] int32_t scroll_y() const noexcept {
        return lv_obj_get_scroll_y(obj());
    }

    // ==================== Extended Draw Size ====================

    /// Calculate the extra draw size needed for a part (shadow, outline, etc.)
    [[nodiscard]] int32_t calculate_ext_draw_size(lv_part_t part = LV_PART_MAIN) const noexcept {
        return lv_obj_calculate_ext_draw_size(obj(), part);
    }

    /// Trigger a refresh of the extended draw size
    Derived& refresh_ext_draw_size() noexcept {
        lv_obj_refresh_ext_draw_size(obj());
        return *static_cast<Derived*>(this);
    }
};

} // namespace lv
