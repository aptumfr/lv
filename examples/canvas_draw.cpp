/**
 * @file canvas_draw.cpp
 * @brief Demonstrates the Canvas widget with the draw API
 *
 * Shows how to use lv::DrawBuf, lv::Layer, and the various draw
 * descriptors (FillDsc, LineDsc, ArcDsc, LabelDsc, RectDsc) to
 * create custom graphics on a canvas.
 */

#include <lv/lv.hpp>
#include <lv/draw/draw.hpp>

class CanvasDrawDemo {
    // Canvas buffer - must outlive the canvas
    lv::DrawBuf m_buf;
    lv::Canvas m_canvas;

public:
    CanvasDrawDemo() : m_buf(300, 300, LV_COLOR_FORMAT_ARGB8888) {}

    void create(lv::ObjectView parent) {
        // Create canvas
        m_canvas = lv::Canvas::create(parent)
            .size(300, 300)
            .center();

        // Attach buffer and clear to white
        m_canvas.draw_buf(m_buf.get());
        m_canvas.fill_bg(lv::colors::white());

        // Draw graphics
        draw_graphics();
    }

private:
    void draw_graphics() {
        // Create a layer for drawing
        lv::Layer layer;
        m_canvas.init_layer(layer);

        // 1. Draw filled rectangles with different styles
        draw_rectangles(layer);

        // 2. Draw lines
        draw_lines(layer);

        // 3. Draw arcs
        draw_arcs(layer);

        // 4. Draw text
        draw_text(layer);

        // Finish the layer to render everything
        m_canvas.finish_layer(layer);
    }

    void draw_rectangles(lv::Layer& layer) {
        // Simple filled rectangle
        lv::FillDsc fill1;
        fill1.color(lv::rgb(255, 100, 100))  // Light red
             .radius(10)
             .opa(lv::opa::cover);
        lv::draw::fill(layer, fill1, lv::area(10, 10, 80, 60));

        // Rectangle with border
        lv::RectDsc rect1;
        rect1.bg_color(lv::rgb(100, 200, 100))  // Light green
             .bg_opa(lv::opa::cover)
             .border_color(lv::rgb(0, 100, 0))  // Dark green
             .border_width(3)
             .radius(5);
        lv::draw::rect(layer, rect1, lv::area(100, 10, 180, 60));

        // Rectangle with shadow
        lv::RectDsc rect2;
        rect2.bg_color(lv::rgb(100, 100, 255))  // Light blue
             .bg_opa(lv::opa::cover)
             .shadow_color(lv::colors::black())
             .shadow_width(10)
             .shadow_ofs(5, 5)
             .shadow_opa(lv::opa::_50)
             .radius(8);
        lv::draw::rect(layer, rect2, lv::area(200, 10, 280, 60));

        // Just a border (no fill)
        lv::BorderDsc border1;
        border1.color(lv::rgb(150, 0, 150))  // Purple
               .width(2)
               .radius(0);
        lv::draw::border(layer, border1, lv::area(10, 70, 80, 100));
    }

    void draw_lines(lv::Layer& layer) {
        // Simple line
        lv::LineDsc line1;
        line1.points(10, 120, 100, 180)
             .color(lv::colors::black())
             .width(2);
        lv::draw::line(layer, line1);

        // Thick rounded line
        lv::LineDsc line2;
        line2.points(120, 120, 200, 180)
             .color(lv::rgb(0, 100, 200))
             .width(8)
             .rounded();
        lv::draw::line(layer, line2);

        // Dashed line
        lv::LineDsc line3;
        line3.points(220, 120, 280, 180)
             .color(lv::rgb(200, 100, 0))
             .width(3)
             .dash(10, 5);
        lv::draw::line(layer, line3);

        // Cross pattern using convenience function
        lv::draw::line(layer, 10, 190, 100, 190, lv::rgb(100, 100, 100), 1);
        lv::draw::line(layer, 55, 185, 55, 195, lv::rgb(100, 100, 100), 1);
    }

    void draw_arcs(lv::Layer& layer) {
        // Simple arc (quarter circle)
        lv::ArcDsc arc1;
        arc1.center(180, 220)
            .radius(30)
            .angles(0, 90)
            .color(lv::rgb(255, 0, 0))  // Red
            .width(5);
        lv::draw::arc(layer, arc1);

        // Arc with rounded ends
        lv::ArcDsc arc2;
        arc2.center(250, 220)
            .radius(25)
            .angles(45, 315)
            .color(lv::rgb(0, 150, 0))  // Green
            .width(8)
            .rounded();
        lv::draw::arc(layer, arc2);

        // Full circle (360 degrees)
        lv::ArcDsc arc3;
        arc3.center(70, 220)
            .radius(20)
            .angles(0, 360)
            .color(lv::rgb(0, 0, 200))  // Blue
            .width(4);
        lv::draw::arc(layer, arc3);

        // Using convenience function
        lv::draw::arc(layer, 120, 220, 15, 180, 360, lv::rgb(150, 0, 150), 6);
    }

    void draw_text(lv::Layer& layer) {
        // Simple text
        lv::LabelDsc label1;
        label1.text("Canvas Draw API")
              .font(&lv_font_montserrat_16)
              .color(lv::colors::black())
              .align(LV_TEXT_ALIGN_CENTER);
        lv::draw::label(layer, label1, lv::area(10, 260, 290, 290));

        // Colored text
        lv::LabelDsc label2;
        label2.text("Shapes")
              .font(&lv_font_montserrat_14)
              .color(lv::rgb(100, 100, 100));
        lv::draw::label(layer, label2, lv::area(10, 105, 100, 120));

        // Using convenience function
        lv::draw::label(layer, "Lines", lv::area(10, 200, 100, 215),
                        &lv_font_montserrat_12, lv::rgb(100, 100, 100));

        lv::draw::label(layer, "Arcs", lv::area(10, 245, 100, 260),
                        &lv_font_montserrat_12, lv::rgb(100, 100, 100));
    }
};

int main() {
    lv::init();

    // Create display
    lv::X11Display display("Canvas Draw Demo", 400, 400);

    // Create demo
    static CanvasDrawDemo demo;
    demo.create(lv::screen_active());

    // Run
    lv::run();

    return 0;
}
