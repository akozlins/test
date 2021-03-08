
#include <gtkmm.h>

using cr_t = Cairo::RefPtr<Cairo::Context>;

const int W = 800;
const int H = 800;
const int nx = 100;
const int ny = 100;
const double wx = double{W} / nx;
const double wy = double{H} / ny;

uint32_t pixels[nx][ny];

void draw() {
}

bool area_draw(const cr_t& cr) {
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, W, H);
    cr->fill();

    draw();

    for(int y = 0; y < 100; y++) for(int x = 0; x < 100; x++) {
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
    cr->set_line_width(1);

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
};

int main() {
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