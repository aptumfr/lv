#pragma once

/**
 * @file draw_task.hpp
 * @brief Wrapper for LVGL draw tasks and draw system utilities
 */

#include <lvgl.h>

namespace lv {

/**
 * @brief Non-owning view of a draw task
 *
 * Draw tasks represent individual drawing operations in LVGL's
 * deferred rendering pipeline. This wrapper provides read-only
 * access to task properties.
 */
class DrawTaskView {
    lv_draw_task_t* m_task;

public:
    constexpr DrawTaskView() noexcept : m_task(nullptr) {}
    constexpr explicit DrawTaskView(lv_draw_task_t* task) noexcept : m_task(task) {}

    [[nodiscard]] lv_draw_task_t* get() const noexcept { return m_task; }
    [[nodiscard]] explicit operator bool() const noexcept { return m_task != nullptr; }

    // ==================== Task Properties ====================

    /// Get the task type
    [[nodiscard]] lv_draw_task_type_t type() const noexcept {
        return m_task ? lv_draw_task_get_type(m_task) : LV_DRAW_TASK_TYPE_NONE;
    }

    /// Get the draw area
    void get_area(lv_area_t* area) const noexcept {
        if (m_task && area) {
            lv_draw_task_get_area(m_task, area);
        }
    }

    /// Get the draw descriptor (type-specific)
    [[nodiscard]] void* draw_dsc() const noexcept {
        return m_task ? lv_draw_task_get_draw_dsc(m_task) : nullptr;
    }

    /// Get draw descriptor as specific type
    template<typename T>
    [[nodiscard]] T* draw_dsc_as() const noexcept {
        return static_cast<T*>(draw_dsc());
    }

    // ==================== Type-Specific Accessors ====================

    /// Get as fill descriptor (if type is FILL)
    [[nodiscard]] lv_draw_fill_dsc_t* fill_dsc() const noexcept {
        return m_task ? lv_draw_task_get_fill_dsc(m_task) : nullptr;
    }

    /// Get as border descriptor (if type is BORDER)
    [[nodiscard]] lv_draw_border_dsc_t* border_dsc() const noexcept {
        return m_task ? lv_draw_task_get_border_dsc(m_task) : nullptr;
    }

    /// Get as box shadow descriptor (if type is BOX_SHADOW)
    [[nodiscard]] lv_draw_box_shadow_dsc_t* box_shadow_dsc() const noexcept {
        return m_task ? lv_draw_task_get_box_shadow_dsc(m_task) : nullptr;
    }

    /// Get as label descriptor (if type is LABEL)
    [[nodiscard]] lv_draw_label_dsc_t* label_dsc() const noexcept {
        return m_task ? lv_draw_task_get_label_dsc(m_task) : nullptr;
    }

    /// Get as image descriptor (if type is IMAGE)
    [[nodiscard]] lv_draw_image_dsc_t* image_dsc() const noexcept {
        return m_task ? lv_draw_task_get_image_dsc(m_task) : nullptr;
    }

    /// Get as line descriptor (if type is LINE)
    [[nodiscard]] lv_draw_line_dsc_t* line_dsc() const noexcept {
        return m_task ? lv_draw_task_get_line_dsc(m_task) : nullptr;
    }

    /// Get as arc descriptor (if type is ARC)
    [[nodiscard]] lv_draw_arc_dsc_t* arc_dsc() const noexcept {
        return m_task ? lv_draw_task_get_arc_dsc(m_task) : nullptr;
    }

    /// Get as triangle descriptor (if type is TRIANGLE)
    [[nodiscard]] lv_draw_triangle_dsc_t* triangle_dsc() const noexcept {
        return m_task ? lv_draw_task_get_triangle_dsc(m_task) : nullptr;
    }
};

namespace draw {

// ==================== Draw System Functions ====================

/**
 * @brief Wait for all draw operations to finish
 *
 * In asynchronous rendering mode, this blocks until all pending
 * draw tasks are complete. With LV_USE_OS == 0, returns immediately.
 */
inline void wait_for_finish() noexcept {
    lv_draw_wait_for_finish();
}

/// Trigger draw task dispatching
inline void dispatch() noexcept {
    lv_draw_dispatch();
}

/// Request a new dispatch cycle (call from draw units when done)
inline void dispatch_request() noexcept {
    lv_draw_dispatch_request();
}

/// Get the total number of registered draw units
[[nodiscard]] inline uint32_t get_unit_count() noexcept {
    return lv_draw_get_unit_count();
}

/// Get the number of tasks depending on a given task
[[nodiscard]] inline uint32_t get_dependent_count(lv_draw_task_t* task) noexcept {
    return lv_draw_get_dependent_count(task);
}

// ==================== Task Retrieval (for custom draw units) ====================

/// Get an available draw task for a draw unit
[[nodiscard]] inline lv_draw_task_t* get_available_task(
    lv_layer_t* layer,
    lv_draw_task_t* t_prev,
    uint8_t draw_unit_id) noexcept {
    return lv_draw_get_available_task(layer, t_prev, draw_unit_id);
}

/// Get the next available draw task
[[nodiscard]] inline lv_draw_task_t* get_next_available_task(
    lv_layer_t* layer,
    lv_draw_task_t* t_prev,
    uint8_t draw_unit_id) noexcept {
    return lv_draw_get_next_available_task(layer, t_prev, draw_unit_id);
}

// ==================== Draw Unit Management (advanced) ====================

/// Create a custom draw unit
[[nodiscard]] inline void* create_unit(size_t size) noexcept {
    return lv_draw_create_unit(size);
}

/// Add a draw task to a layer
[[nodiscard]] inline lv_draw_task_t* add_task(
    lv_layer_t* layer,
    const lv_area_t* coords,
    lv_draw_task_type_t type) noexcept {
    return lv_draw_add_task(layer, coords, type);
}

/// Finalize task creation (sends event, assigns to draw unit)
inline void finalize_task_creation(lv_layer_t* layer, lv_draw_task_t* task) noexcept {
    lv_draw_finalize_task_creation(layer, task);
}

/// Send an event to draw units
inline void unit_send_event(const char* name, lv_event_code_t code, void* param) noexcept {
    lv_draw_unit_send_event(name, code, param);
}

} // namespace draw

} // namespace lv
