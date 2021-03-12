#ifndef __VIEW_H__
#define __VIEW_H__

#include <gtkmm.h>

struct view_t {
    int w;
    int h;

    uint32_t ev_button;
    double2 motion_xy, button_xy;

    struct event_t {
        GdkEventType type = GDK_NOTHING;
        guint32 time = 0;
        double2 xy;
        guint button = 0;
    };

    Gtk::Window* window;

    using cr_t = Cairo::RefPtr<Cairo::Context>;

    virtual void on_draw(const cr_t& cr) const = 0;

    bool on_draw_internal(const cr_t& cr) {
        cr->set_source_rgb(1, 1, 1);
        cr->rectangle(0, 0, w, h);
        cr->fill();

        on_draw(cr);

        return true;
    }

    view_t(int w, int h) : w(w), h(h) {
        auto gtk = new Gtk::Main();

        window = new Gtk::Window();
        window->set_border_width(0);
        window->set_resizable(false);
        window->set_position(Gtk::WIN_POS_CENTER);

        auto fixed = Gtk::manage(new Gtk::Fixed());
        fixed->set_size_request(w + 2, h + 2);
        window->add(*fixed);

        auto area = Gtk::manage(new Gtk::DrawingArea);
        area->set_size_request(w, h);
        area->signal_draw().connect([this] (const cr_t& cr) { return on_draw_internal(cr); });
        fixed->put(*area, 1, 1);

        window->show_all();
    }

    void run() {
        Gtk::Main::run(*window);
    }
};

#endif // __VIEW_H__
