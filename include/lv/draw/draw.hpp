#pragma once

/**
 * @file draw.hpp
 * @brief Complete LVGL draw API wrapper
 *
 * This header provides C++ wrappers for LVGL's low-level drawing API,
 * primarily used with the Canvas widget.
 *
 * Example usage with Canvas:
 * ```cpp
 * #include <lv/lv.hpp>
 * #include <lv/draw/draw.hpp>
 *
 * // Create canvas with buffer
 * lv::DrawBuf buf(200, 200, LV_COLOR_FORMAT_ARGB8888);
 * auto canvas = lv::Canvas::create(parent).size(200, 200);
 * canvas.draw_buf(buf.get());
 * canvas.fill_bg(lv::color::white());
 *
 * // Draw on canvas
 * lv::Layer layer;
 * canvas.init_layer(layer);
 *
 * // Draw a filled rectangle
 * lv::FillDsc fill;
 * fill.color(lv::color::red()).radius(10);
 * lv::draw::fill(layer, fill, lv::area(10, 10, 50, 50));
 *
 * // Draw a line
 * lv::LineDsc line;
 * line.points(0, 0, 100, 100).color(lv::color::blue()).width(3).rounded();
 * lv::draw::line(layer, line);
 *
 * // Draw an arc
 * lv::ArcDsc arc;
 * arc.center(100, 100).radius(40).angles(0, 270)
 *    .color(lv::color::green()).width(5).rounded();
 * lv::draw::arc(layer, arc);
 *
 * // Draw a triangle
 * lv::TriangleDsc tri;
 * tri.points(50, 10, 10, 90, 90, 90).color(lv::color::blue());
 * lv::draw::triangle(layer, tri);
 *
 * // Draw text
 * lv::LabelDsc text;
 * text.text("Hello").font(&lv_font_montserrat_16).color(lv::color::black());
 * lv::draw::label(layer, text, lv::area(10, 150, 190, 180));
 *
 * canvas.finish_layer(layer);
 * ```
 */

// Buffer management
#include "draw_buf.hpp"

// Layer management
#include "layer.hpp"

// Geometric primitives
#include "primitives.hpp"

// Drawing descriptors and functions
#include "draw_rect.hpp"     // FillDsc, BorderDsc, BoxShadowDsc, RectDsc
#include "draw_line.hpp"     // LineDsc
#include "draw_arc.hpp"      // ArcDsc
#include "draw_triangle.hpp" // TriangleDsc
#include "draw_label.hpp"    // LabelDsc, LetterDsc
#include "draw_image.hpp"    // ImageDsc
#include "draw_task.hpp"     // DrawTaskView, draw system utilities

// Note: lv_draw_mask_rect_dsc_t is internal to LVGL (private header)
// Note: lv_draw_vector API is large and not yet wrapped
