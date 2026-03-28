#pragma once

/**
 * @file wrap.hpp
 * @brief Tag type for wrapping existing C objects without creating new ones
 */

namespace lv {

/// Tag type for wrapping existing C objects
struct wrap_t {};

/// Tag instance
/// Usage: auto box = lv::Box(lv::wrap, existing_ptr);
/// Prefer lv::ref(ptr) for generic object access.
inline constexpr wrap_t wrap{};

} // namespace lv
