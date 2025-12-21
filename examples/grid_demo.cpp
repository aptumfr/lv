/**
 * @file grid_demo.cpp
 * @brief Grid layout demonstration
 *
 * Demonstrates:
 * - CSS-like grid layout system
 * - Fractional units (fr)
 * - Grid alignment
 * - Cell spanning
 * - Responsive-like patterns
 */

#include <lv/lv.hpp>

static lv::Color get_color(int index) {
    static const uint32_t color_hexes[] = {
        0xF44336,  // red
        0x2196F3,  // blue
        0x4CAF50,  // green
        0xFF9800,  // orange
        0x9C27B0,  // purple
        0x009688,  // teal
        0xE91E63,  // pink
        0x3F51B5,  // indigo
        0x00BCD4,  // cyan
    };
    return lv::rgb(color_hexes[index % 9]);
}

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Grid Layout Demo", 480, 580);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 580);
#else
    #error "No display backend enabled"
#endif

    auto screen = lv::screen_active();
    screen.bg_color(lv::rgb(0xf5f5f5));

    auto content = lv::vbox(screen)
        .fill()
        .padding(12)
        .gap(12);

    // Title
    lv::Label::create(content)
        .text("Grid Layout Examples")
        .text_color(lv::rgb(0x2196F3));  // blue

    // ========== Example 1: Basic 3-column grid ==========
    lv::Label::create(content).text("1. Basic 3-column (1fr each):");

    // Column definitions: 1fr, 1fr, 1fr
    static int32_t col_dsc1[] = {lv::Grid::fr(1), lv::Grid::fr(1), lv::Grid::fr(1), lv::Grid::template_last};
    static int32_t row_dsc1[] = {40, 40, lv::Grid::template_last};

    auto grid1 = lv::grid(content)
        .fill_width()
        .dsc_array(col_dsc1, row_dsc1)
        .gap(4);

    for (int i = 0; i < 6; i++) {
        char buf[16];
        lv::snprintf(buf, sizeof(buf), "Cell %d", i + 1);
        auto cell = lv::vbox(grid1)
            .bg_color(get_color(i))
            .radius(4)
            .padding(4);
        lv::Label::create(cell).text(buf).text_color(lv::colors::white()).center();  // white() exists in C API
        lv::grid_cell(cell).col(i % 3).row(i / 3);
    }

    // ========== Example 2: Unequal columns ==========
    lv::Label::create(content).text("2. Unequal columns (1fr, 2fr, 1fr):");

    // 1fr, 2fr, 1fr - middle column is twice as wide
    static int32_t col_dsc2[] = {lv::Grid::fr(1), lv::Grid::fr(2), lv::Grid::fr(1), lv::Grid::template_last};
    static int32_t row_dsc2[] = {50, lv::Grid::template_last};

    auto grid2 = lv::grid(content)
        .fill_width()
        .dsc_array(col_dsc2, row_dsc2)
        .gap(4);

    const char* labels2[] = {"Left", "Center (2x)", "Right"};
    for (int i = 0; i < 3; i++) {
        auto cell = lv::vbox(grid2)
            .bg_color(get_color(i + 3))
            .radius(4)
            .padding(4);
        lv::Label::create(cell).text(labels2[i]).text_color(lv::colors::white()).center();  // white() exists in C API
        lv::grid_cell(cell).col(i).row(0);
    }

    // ========== Example 3: Fixed + Flexible ==========
    lv::Label::create(content).text("3. Fixed + Flexible (80px, 1fr, 80px):");

    // Fixed 80px, flexible, fixed 80px
    static int32_t col_dsc3[] = {80, lv::Grid::fr(1), 80, lv::Grid::template_last};
    static int32_t row_dsc3[] = {50, lv::Grid::template_last};

    auto grid3 = lv::grid(content)
        .fill_width()
        .dsc_array(col_dsc3, row_dsc3)
        .gap(4);

    const char* labels3[] = {"Fixed", "Flexible", "Fixed"};
    for (int i = 0; i < 3; i++) {
        auto cell = lv::vbox(grid3)
            .bg_color(get_color(i + 6))
            .radius(4)
            .padding(4);
        lv::Label::create(cell).text(labels3[i]).text_color(lv::colors::white()).center();  // white() exists in C API
        lv::grid_cell(cell).col(i).row(0);
    }

    // ========== Example 4: Dashboard-style ==========
    lv::Label::create(content).text("4. Dashboard layout:");

    // 2 columns, multiple rows
    static int32_t col_dsc4[] = {lv::Grid::fr(1), lv::Grid::fr(1), lv::Grid::template_last};
    static int32_t row_dsc4[] = {60, 40, 40, lv::Grid::template_last};

    auto grid4 = lv::grid(content)
        .fill_width()
        .dsc_array(col_dsc4, row_dsc4)
        .gap(4);

    // Wide header spanning 2 columns
    auto header = lv::vbox(grid4)
        .bg_color(lv::rgb(0x3F51B5))  // indigo
        .radius(4)
        .padding(8);
    lv::Label::create(header).text("Header (spans 2 cols)").text_color(lv::colors::white()).center();  // white() exists in C API
    lv::grid_cell(header).col(0).row(0).col_span(2);

    // Stats boxes
    const char* stats[] = {"Users: 1.2K", "Sales: $5.4K", "Orders: 89", "Rating: 4.5"};
    int positions[][2] = {{0, 1}, {1, 1}, {0, 2}, {1, 2}};
    for (int i = 0; i < 4; i++) {
        auto cell = lv::vbox(grid4)
            .bg_color(get_color(i))
            .radius(4)
            .padding(4);
        lv::Label::create(cell).text(stats[i]).text_color(lv::colors::white()).center();  // white() exists in C API
        lv::grid_cell(cell).col(positions[i][0]).row(positions[i][1]);
    }

    lv::run();
    return 0;
}
