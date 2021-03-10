
#include <gtkmm.h>

using cr_t = Cairo::RefPtr<Cairo::Context>;

const int W = 1024;
const int H = 1024;
const int nx = 512;
const int ny = 512;
const double wx = double{W} / nx;
const double wy = double{H} / ny;

uint32_t color = 0;
uint32_t pixels[nx][ny];

/**
 * <https://en.wikipedia.org/wiki/Bresenham's_line_algorithm>
 */
void line(int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while(1) {
        pixels[x0][y0] = color;
        if(x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if(e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if(e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}



void draw() {
}

bool area_draw(const cr_t& cr) {
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, W, H);
    cr->fill();

    draw();

    for(int y = 0; y < nx; y++) for(int x = 0; x < ny; x++) {
        uint32_t pixel = pixels[x][y];
        if(pixel == 0) continue;
        double r = ((pixel >> 0) & 0xFF) / 256.0;
        double g = ((pixel >> 8) & 0xFF) / 256.0;
        double b = ((pixel >> 16) & 0xFF) / 256.0;
        cr->set_source_rgb(r, g, b);
        cr->rectangle(wx * x, wy * y, wx, wy);
        cr->fill();
    }

    cr->set_source_rgb(0, 0, 0);
    cr->set_line_width(0.5);

    for(int x = 0; x <= nx; x++) {
        cr->move_to(wx * x, 0);
        cr->line_to(wx * x, H);
    }
    cr->stroke();

    for(int y = 0; y <= ny; y++) {
        cr->move_to(0, wy * y);
        cr->line_to(W, wy * y);
    }
    cr->stroke();

    return true;
}

int main(int argc, const char* argv[]) {
    auto gtk = new Gtk::Main();

    auto window = new Gtk::Window();
    window->set_border_width(0);
    window->set_resizable(false);
    window->set_position(Gtk::WIN_POS_CENTER);

    auto fixed = Gtk::manage(new Gtk::Fixed());
    fixed->set_size_request(W + 2, H + 2);
    window->add(*fixed);

    auto area = Gtk::manage(new Gtk::DrawingArea);
    area->set_size_request(W, H);
    area->signal_draw().connect([] (const cr_t& cr) { return area_draw(cr); });
    fixed->put(*area, 1, 1);

    window->show_all();

    Gtk::Main::run(*window);

    return 0;
}
