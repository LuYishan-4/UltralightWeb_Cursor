#pragma once


struct wl_display;
struct wl_surface;

namespace CursorFX {


class WaylandBackend { 
public:

    WaylandBackend();


    virtual ~WaylandBackend();


    bool initialize();

    void setPosition(int x, int y);

    void present();

private:

    struct wl_display* display_ = nullptr;

    struct wl_surface* surface_ = nullptr;

    int x_ = 0;

    int y_ = 0;

};

}
