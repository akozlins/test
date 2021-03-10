
#include "wavefront.h"

wavefront_t wavefront;

#include <gtkmm.h>

using cr_t = Cairo::RefPtr<Cairo::Context>;

const int W = 1024;
const int H = 1024;
const int nx = 512;
const int ny = 512;
const double wx = double{W} / nx;
const double wy = double{H} / ny;

struct color_t {
    double r = 0, g = 0, b = 0, a = 0;
} color;
color_t pixels[nx][ny];

/**
 * <https://en.wikipedia.org/wiki/Bresenham's_line_algorithm>
 */
void line(int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? +1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? -1 : -1;
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

void raster(int x0, int y0, int x1l, int x1r, int y1) {
    if(x1l > x1r) std::swap(x1l, x1r);
    int y = y1, sy = y0 < y1 ? +1 : -1;
    int xl = x1l;
    int xr = x1r;
    while(1) {
        for(int x = xl; x <= xr; x++) pixels[x][y] = color;
        if(y == y0) break;

        y -= sy;
        double r = double(y - y0) / (y1 - y0);
        xl = x0 + (x1l - x0) * r;
        xr = x0 + (x1r - x0) * r;
    }
}

void raster(int x0, int y0, int x1, int y1, int x2, int y2) {
    if(y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); }
    // y0 <= y2
    if(y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
    // y0 <= y1, y2
    if(y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
    // y0 <= y1 <= y2
    if(y1 == y2) {
        raster(x0, y0, x1, x2, y1);
        return;
    }
    // y0 <= y1 < y2
    int xm = x0 + (x2 - x0) * double(y1 - y0) / (y2 - y0);
    raster(x0, y0, x1, xm, y1);
    raster(x2, y2, x1, xm, y1);
}



void draw() {
    for(auto& f : wavefront.f) {
        auto n = (f.v[1] - f.v[0]).cross(f.v[2] - f.v[0]).norm();
        double c = n.dot({0,0,1});
        if(c < 0) continue;
        color = { c, c, c, 1 };
        raster((1+f.v[0].x)*nx/2, (1+f.v[0].y)*ny/2, (1+f.v[1].x)*nx/2, (1+f.v[1].y)*ny/2, (1+f.v[2].x)*nx/2, (1+f.v[2].y)*ny/2);
    }
}

bool area_draw(const cr_t& cr) {
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, W, H);
    cr->fill();

    draw();

    for(int y = 0; y < nx; y++) for(int x = 0; x < ny; x++) {
        auto& pixel = pixels[x][ny-y];
        if(pixel.a == 0) continue;
        cr->set_source_rgba(pixel.r, pixel.g, pixel.b, pixel.a);
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
    wavefront.init(argv[1]);

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
