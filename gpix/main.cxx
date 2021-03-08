
#include <gtkmm.h>

using cr_t = Cairo::RefPtr<Cairo::Context>;

const int W = 800;
const int H = 600;

bool draw(const cr_t& cr) {
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, W, H);
    cr->fill();

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
    area->signal_draw().connect([] (const cr_t& cr) { return draw(cr); });
    fixed->put(*area, 1, 1);

    window->show_all();

    Gtk::Main::run(*window);

    return 0;
}
