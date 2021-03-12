
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

const int nz = 1024;

struct color_t {
    double r = 0, g = 0, b = 0, a = 0;
};

color_t pixels[nx][ny];
int zbuffer[nx][ny];

void draw(const int3_t& p, const color_t& color) {
    if(!(0 <= p.x && p.x < nx) || !(0 <= p.y && p.y < ny) || p.z < 0) {
        printf("F [] draw: (%d, %d, %d) out of bounds\n", p.x, p.y, p.z);
        return;
    }
    if(p.z >= zbuffer[p.x][p.y]) {
        zbuffer[p.x][p.y] = p.z;
        pixels[p.x][p.y] = color;
    }
}

/**
 * <https://en.wikipedia.org/wiki/Bresenham's_line_algorithm>
 */
void line(int2_t p0, const int2_t& p1, const color_t& color) {
    int dx = std::abs(p1.x - p0.x);
    int sx = p0.x < p1.x ? +1 : -1;
    int dy = -std::abs(p1.y - p0.y);
    int sy = p0.y < p1.y ? +1 : -1;
    int err = dx + dy;

    while(1) {
        draw(p0, color);
        if(p0 == p1) break;
        int e2 = 2 * err;
        if(e2 >= dy) {
            err += dy;
            p0.x += sx;
        }
        if(e2 <= dx) {
            err += dx;
            p0.y += sy;
        }
    }
}

void raster2(const int3_t& p0, int3_t p1l, int3_t p1r, const color_t& color) {
    assert(p1l.y == p1r.y);
    if(p1l.x > p1r.x) std::swap(p1l, p1r);
    int y = p1l.y, sy = p0.y < p1l.y ? +1 : -1;
    int xl = p1l.x, zl = p1l.z;
    int xr = p1r.x, zr = p1r.z;
    while(1) {
        for(int x = xl; x <= xr; x++) {
            int z = zl; if(xl != xr) z += (zr - zl) * (x - xl) / (xr - xl);
            draw({x, y, z}, color);
        }
        if(y == p0.y) break;

        y -= sy;
        double r = double(y - p0.y) / (p1l.y - p0.y);
        xl = p0.x + (p1l.x - p0.x) * r;
        xr = p0.x + (p1r.x - p0.x) * r;
        zl = p0.z + (p1l.z - p0.z) * r;
        zr = p0.z + (p1r.z - p0.z) * r;
    }
}

void raster(int3_t p0, int3_t p1, int3_t p2, const color_t& color) {
    if(p0.y > p2.y) std::swap(p0, p2);
    // y0 <= y2
    if(p0.y > p1.y) std::swap(p0, p1);
    // y0 <= y1, y2
    if(p1.y > p2.y) std::swap(p1, p2);
    // y0 <= y1 <= y2
    if(p1.y == p2.y) {
        raster2(p0, p1, p2, color);
        return;
    }
    // y0 <= y1 < y2
    int xm = p0.x + (p2.x - p0.x) * double(p1.y - p0.y) / (p2.y - p0.y);
    int zm = p0.z + (p2.z - p0.z) * double(p1.y - p0.y) / (p2.y - p0.y);
    raster2(p0, p1, {xm,p1.y,zm}, color);
    raster2(p2, p1, {xm,p1.y,zm}, color);
}



void draw() {
    for(auto& f : wavefront.f) {
        auto n = (f.v[1] - f.v[0]).cross(f.v[2] - f.v[0]).norm();
        double c = n.dot({0,0,1});
        if(c < 0) continue;
        int3_t v[3] = {
            { (1+f.v[0].x)*nx/2, (1+f.v[0].y)*ny/2, (1+f.v[0].z)*nz/2 },
            { (1+f.v[1].x)*nx/2, (1+f.v[1].y)*ny/2, (1+f.v[0].z)*nz/2 },
            { (1+f.v[2].x)*nx/2, (1+f.v[2].y)*ny/2, (1+f.v[0].z)*nz/2 },
        };
        raster(v[0], v[1], v[2], { c, c, c, 1 });
//        line(v[0], v[1], { 0, 0, 0, 1});
//        line(v[1], v[2], { 0, 0, 0, 1});
//        line(v[2], v[0], { 0, 0, 0, 1});
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
