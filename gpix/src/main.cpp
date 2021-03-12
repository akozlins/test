
#include "wavefront.h"
wavefront_t wavefront;

#include "view.h"

#include "canvas.h"
canvas_t canvas(256, 256);


struct canvas_view_t : view_t {
    canvas_view_t(int w, int h) : view_t(w, h) {
    }

    void on_draw(const cr_t& cr) const {
        canvas.clear();

        for(auto& f : wavefront.f) {
            auto n = (f.v[1] - f.v[0]).cross(f.v[2] - f.v[0]).norm();
            int c = 255 * n.dot({0,0,1});
            if(c < 0) continue;
            int3_t vi[3];
            for(int i = 0; i < 3; i++) {
                auto v = f.v[i];
                v = v / (1 - v.z/2);
                vi[i] = { 128 * (v.x + 1), 128 * (v.y + 1), 128 * (v.z + 1) };
            }
            canvas.raster(vi[0], vi[1], vi[2], { c, c, c, 255 });
        }

        for(int y = 0; y < canvas.w; y++) for(int x = 0; x < canvas.h; x++) {
            auto& pixel = canvas.pixels[canvas.w * y + x];
            if(pixel.a == 0) continue;
            cr->set_source_rgba(pixel.r/255., pixel.g/255., pixel.b/255., pixel.a/255.);
            cr->rectangle(x, y, 1, 1);
            cr->fill();
        }

        cr->set_source_rgb(0, 0, 0);
        cr->set_line_width(0.1);

        for(int x = 0; x <= canvas.w; x++) {
            cr->move_to(x, 0);
            cr->line_to(x, canvas.h);
        }
        cr->stroke();

        for(int y = 0; y <= canvas.h; y++) {
            cr->move_to(0, y);
            cr->line_to(canvas.w, y);
        }
        cr->stroke();

        for(auto& f : wavefront.f) {
            int3_t vi[3];
            for(int i = 0; i < 3; i++) {
                vi[i] = { 128 * (f.v[i].x + 1), 128 * (f.v[i].y + 1), 128 * (f.v[i].z + 1) };
            }
            cr->move_to(vi[0].x + 0.5, vi[0].y + 0.5);
            cr->line_to(vi[1].x + 0.5, vi[1].y + 0.5);
            cr->move_to(vi[1].x + 0.5, vi[1].y + 0.5);
            cr->line_to(vi[2].x + 0.5, vi[2].y + 0.5);
            cr->move_to(vi[2].x + 0.5, vi[2].y + 0.5);
            cr->line_to(vi[0].x + 0.5, vi[0].y + 0.5);
            cr->close_path();
        }
        cr->stroke();
    }
};

int main(int argc, const char* argv[]) {
    if(argc != 2) {
        printf("usage: %s <wavefront.obj>\n", argv[0]);
        return 1;
    }

    wavefront.init(argv[1]);

    canvas_view_t view(1024, 1024);
    view.run();

    return 0;
}
