#pragma once

#include <cstdint>
#include <cstddef>
#include <wayland-client.h>


namespace CursorFX
{


class WaylandBackend
{

public:

    WaylandBackend();

    ~WaylandBackend();


    bool initialize();



    void pump();


    bool setCursor(
        int width,
        int height,
        void* pixels,
        int hotspot_x,
        int hotspot_y
    );



    static void registryGlobal(
        void* data,
        wl_registry* registry,
        uint32_t name,
        const char* interface,
        uint32_t version
    );

    static void registryRemove(
        void* data,
        wl_registry* registry,
        uint32_t name
    );

    static void seatCapabilities(
        void* data,
        wl_seat* seat,
        uint32_t capabilities
    );

    static void seatName(
        void* data,
        wl_seat* seat,
        const char* name
    );

    static void pointerEnter(
        void* data,
        wl_pointer* pointer,
        uint32_t serial,
        wl_surface* surface,
        wl_fixed_t sx,
        wl_fixed_t sy
    );

    static void pointerLeave(
        void* data,
        wl_pointer* pointer,
        uint32_t serial,
        wl_surface* surface
    );

    static void pointerMotion(
        void* data,
        wl_pointer* pointer,
        uint32_t time,
        wl_fixed_t sx,
        wl_fixed_t sy
    );

    static void pointerButton(
        void* data,
        wl_pointer* pointer,
        uint32_t serial,
        uint32_t time,
        uint32_t button,
        uint32_t state
    );

    static void pointerAxis(
        void* data,
        wl_pointer* pointer,
        uint32_t time,
        uint32_t axis,
        wl_fixed_t value
    );


private:

    void destroyBuffer();


    wl_display* display_ = nullptr;
    wl_registry* registry_ = nullptr;
    wl_compositor* compositor_ = nullptr;
    wl_shm* shm_ = nullptr;
    wl_seat* seat_ = nullptr;
    wl_pointer* pointer_ = nullptr;
    wl_surface* cursor_surface_ = nullptr;

    uint32_t serial_ = 0;

    wl_buffer* cursor_buffer_ = nullptr;
    void* shm_data_ = nullptr;
    int shm_fd_ = -1;
    size_t shm_size_ = 0;

};


}
