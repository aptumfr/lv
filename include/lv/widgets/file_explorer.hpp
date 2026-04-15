#pragma once

/**
 * @file file_explorer.hpp
 * @brief Zero-cost wrapper for LVGL file explorer widget
 *
 * Requires LV_USE_FILE_EXPLORER to be enabled in lv_conf.h.
 */

#include <lvgl.h>

#if LV_USE_FILE_EXPLORER

#include "../core/objectref.hpp"

namespace lv {

// ==================== File Explorer Sort ====================

namespace explorer_sort {
    constexpr auto none = LV_EXPLORER_SORT_NONE;
    constexpr auto kind = LV_EXPLORER_SORT_KIND;
} // namespace explorer_sort

// ==================== Quick Access Directories ====================

#if LV_FILE_EXPLORER_QUICK_ACCESS
namespace explorer_dir {
    constexpr auto home = LV_EXPLORER_HOME_DIR;
    constexpr auto music = LV_EXPLORER_MUSIC_DIR;
    constexpr auto pictures = LV_EXPLORER_PICTURES_DIR;
    constexpr auto video = LV_EXPLORER_VIDEO_DIR;
    constexpr auto docs = LV_EXPLORER_DOCS_DIR;
    constexpr auto fs = LV_EXPLORER_FS_DIR;
} // namespace explorer_dir
#endif

/**
 * @brief File Explorer widget wrapper
 *
 * A file browser widget for navigating directories and selecting files.
 *
 * Requires LV_USE_FILE_EXPLORER=1 in lv_conf.h.
 *
 * Usage:
 *   auto explorer = lv::FileExplorer::create(parent);
 *   explorer.open_dir("/home");
 *   explorer.on_value_changed([](lv::Event e) {
 *       auto fe = lv::FileExplorer(lv::wrap, e.target());
 *       const char* file = fe.selected_file();
 *   });
 */
class FileExplorer : public ObjectView,
                     public ObjectMixin<FileExplorer>,
                     public EventMixin<FileExplorer>,
                     public StyleMixin<FileExplorer> {
public:
    constexpr FileExplorer() noexcept : ObjectView(nullptr) {}
    constexpr FileExplorer(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// LVGL class pointer (for `is<T>() / as<T>()` type identification).
    static constexpr const lv_obj_class_t* class_ptr() noexcept { return &lv_file_explorer_class; }

    [[nodiscard]] static FileExplorer create(lv_obj_t* parent) {
        return FileExplorer(wrap, lv_file_explorer_create(parent));
    }
    [[nodiscard]] static FileExplorer create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Navigation ====================

    /// Open a directory
    FileExplorer& open_dir(const char* path) noexcept {
        lv_file_explorer_open_dir(m_obj, path);
        return *this;
    }

    /// Get current path
    [[nodiscard]] const char* current_path() const noexcept {
        return lv_file_explorer_get_current_path(m_obj);
    }

    /// Get selected file name
    [[nodiscard]] const char* selected_file() const noexcept {
        return lv_file_explorer_get_selected_file_name(m_obj);
    }

    // ==================== Configuration ====================

    /// Set sort mode
    FileExplorer& sort(lv_file_explorer_sort_t s) noexcept {
        lv_file_explorer_set_sort(m_obj, s);
        return *this;
    }

    /// Sort by kind (directories first)
    FileExplorer& sort_by_kind() noexcept {
        return sort(LV_EXPLORER_SORT_KIND);
    }

    /// No sorting
    FileExplorer& sort_none() noexcept {
        return sort(LV_EXPLORER_SORT_NONE);
    }

    /// Get current sort mode
    [[nodiscard]] lv_file_explorer_sort_t get_sort() const noexcept {
        return lv_file_explorer_get_sort(m_obj);
    }

    /// Show/hide back button
    FileExplorer& show_back_button(bool show = true) noexcept {
        lv_file_explorer_show_back_button(m_obj, show);
        return *this;
    }

    /// Hide back button
    FileExplorer& hide_back_button() noexcept {
        return show_back_button(false);
    }

    // ==================== Quick Access ====================

#if LV_FILE_EXPLORER_QUICK_ACCESS
    /// Set quick access path
    FileExplorer& quick_access_path(lv_file_explorer_dir_t dir, const char* path) noexcept {
        lv_file_explorer_set_quick_access_path(m_obj, dir, path);
        return *this;
    }

    /// Set home directory
    FileExplorer& home_dir(const char* path) noexcept {
        return quick_access_path(LV_EXPLORER_HOME_DIR, path);
    }

    /// Set music directory
    FileExplorer& music_dir(const char* path) noexcept {
        return quick_access_path(LV_EXPLORER_MUSIC_DIR, path);
    }

    /// Set pictures directory
    FileExplorer& pictures_dir(const char* path) noexcept {
        return quick_access_path(LV_EXPLORER_PICTURES_DIR, path);
    }

    /// Set video directory
    FileExplorer& video_dir(const char* path) noexcept {
        return quick_access_path(LV_EXPLORER_VIDEO_DIR, path);
    }

    /// Set documents directory
    FileExplorer& docs_dir(const char* path) noexcept {
        return quick_access_path(LV_EXPLORER_DOCS_DIR, path);
    }

    /// Get quick access area
    [[nodiscard]] ObjectRef quick_access_area() const noexcept {
        return ObjectRef(lv_file_explorer_get_quick_access_area(m_obj));
    }

    /// Get places list
    [[nodiscard]] ObjectRef places_list() const noexcept {
        return ObjectRef(lv_file_explorer_get_places_list(m_obj));
    }

    /// Get device list
    [[nodiscard]] ObjectRef device_list() const noexcept {
        return ObjectRef(lv_file_explorer_get_device_list(m_obj));
    }
#endif

    // ==================== Sub-objects ====================

    /// Get file table (lv_table)
    [[nodiscard]] ObjectRef file_table() const noexcept {
        return ObjectRef(lv_file_explorer_get_file_table(m_obj));
    }

    /// Get header area
    [[nodiscard]] ObjectRef header() const noexcept {
        return ObjectRef(lv_file_explorer_get_header(m_obj));
    }

    /// Get path label
    [[nodiscard]] ObjectRef path_label() const noexcept {
        return ObjectRef(lv_file_explorer_get_path_label(m_obj));
    }

    // ==================== Size ====================

    /// Set size
    FileExplorer& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv

#endif // LV_USE_FILE_EXPLORER
