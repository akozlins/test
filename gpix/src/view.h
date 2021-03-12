#ifndef __VIEW_H__
#define __VIEW_H__

#include "util.h"

#include <gtkmm.h>

struct view_t {
    Gtk::DrawingArea* area;

    int w;
    int h;

    uint32_t ev_button = -1;
    double2_t motion_xy, button_xy;

    double2_t translate = { 0, h };
    double scale = 4;

    struct event_t {
        GdkEventType type = GDK_NOTHING;
        guint32 time = 0;
        double2_t xy;
        guint button = 0;
    };

    Gtk::Window* window;

    using cr_t = Cairo::RefPtr<Cairo::Context>;

    bool on_motion(GdkEventMotion* ev) {
        double2_t xy = { ev->x, ev->y };
        xy = xy - translate;

        if(ev_button == 1) {
            translate = translate + xy - button_xy;
        }
        else {
            return false;
        }

        motion_xy = xy;

        area->queue_draw();
        return true;
    }

    bool on_button(GdkEventButton* ev) {
        double2_t xy = { ev->x, ev->y };
        xy = xy - translate;

        button_xy = xy;

        if(ev->type == GDK_BUTTON_PRESS) {
            ev_button = ev->button;
        }
        if(ev->type == GDK_BUTTON_RELEASE) {
            ev_button = -1;
        }

        return true;
    }

    bool on_scroll(GdkEventScroll* ev) {
        double2_t xy = { ev->x, ev->y };
        xy = xy - translate;

        switch(ev->direction) {
        case GDK_SCROLL_UP: {
            if(scale > 1024) return false;
            double f = std::pow(2, 0.125);
            translate = translate + xy * (1 - f);
            scale *= f;
            break;
        }
        case GDK_SCROLL_DOWN: {
            if(scale < 2) return false;
            double f = 1 / std::pow(2, 0.125);
            translate = translate + xy * (1 - f);
            scale *= f;
            break;
        }
        default:
            return false;
        }

        area->queue_draw();

        return true;
    }

    virtual void on_draw(const cr_t& cr) const = 0;

    bool on_draw_internal(const cr_t& cr) {
        cr->set_source_rgb(1, 1, 1);
        cr->rectangle(0, 0, w, h);
        cr->fill();

        cr->save();
        cr->translate(translate.x, translate.y);
        cr->scale(+scale, -scale);
        on_draw(cr);
        cr->restore();

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

        area = Gtk::manage(new Gtk::DrawingArea);
        area->set_size_request(w, h);

        area->signal_draw                ().connect(sigc::mem_fun(*this, &view_t::on_draw_internal));

        area->add_events(Gdk::POINTER_MOTION_MASK |
                         Gdk::BUTTON_PRESS_MASK |
                         Gdk::BUTTON_RELEASE_MASK |
                         Gdk::SCROLL_MASK);
        area->signal_motion_notify_event ().connect(sigc::mem_fun(*this, &view_t::on_motion));
        area->signal_button_press_event  ().connect(sigc::mem_fun(*this, &view_t::on_button));
        area->signal_button_release_event().connect(sigc::mem_fun(*this, &view_t::on_button));
        area->signal_scroll_event        ().connect(sigc::mem_fun(*this, &view_t::on_scroll));

        fixed->put(*area, 1, 1);

        window->show_all();
    }

    void run() {
        Gtk::Main::run(*window);
    }
};

#endif // __VIEW_H__
