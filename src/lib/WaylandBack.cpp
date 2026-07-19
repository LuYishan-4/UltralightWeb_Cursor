#include "header/WaylandBack.hpp"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>


namespace CursorFX
{


static const wl_registry_listener registry_listener =
{
    WaylandBackend::registryGlobal,
    WaylandBackend::registryRemove
};


static const wl_seat_listener seat_listener =
{
    WaylandBackend::seatCapabilities,
    WaylandBackend::seatName
};


static const wl_pointer_listener pointer_listener =
{
    WaylandBackend::pointerEnter,
    WaylandBackend::pointerLeave,
    WaylandBackend::pointerMotion,
    WaylandBackend::pointerButton,
    WaylandBackend::pointerAxis
};


static int create_shm_file(size_t size)
{
#ifdef SYS_memfd_create
    int fd = syscall(SYS_memfd_create, "cursorfx", 0);
#else
    int fd = -1;
#endif

    if(fd < 0)
        return -1;

    if(ftruncate(fd, size) < 0)
    {
        close(fd);
        return -1;
    }

    return fd;
}


WaylandBackend::WaylandBackend()
{
}


WaylandBackend::~WaylandBackend()
{
    destroyBuffer();

    if(pointer_)
        wl_pointer_destroy(pointer_);

    if(seat_)
        wl_seat_destroy(seat_);

    if(cursor_surface_)
        wl_surface_destroy(cursor_surface_);

    if(compositor_)
        wl_compositor_destroy(compositor_);

    if(shm_)
        wl_shm_destroy(shm_);

    if(registry_)
        wl_registry_destroy(registry_);

    if(display_)
        wl_display_disconnect(display_);
}


void WaylandBackend::destroyBuffer()
{
    if(cursor_buffer_)
    {
        wl_buffer_destroy(cursor_buffer_);
        cursor_buffer_ = nullptr;
    }

    if(shm_data_)
    {
        munmap(shm_data_, shm_size_);
        shm_data_ = nullptr;
    }

    if(shm_fd_ >= 0)
    {
        close(shm_fd_);
        shm_fd_ = -1;
    }

    shm_size_ = 0;
}


bool WaylandBackend::initialize()
{
    display_ = wl_display_connect(nullptr);

    if(!display_)
    {
        std::cerr << "cannot connect wayland\n";
        return false;
    }

    registry_ = wl_display_get_registry(display_);

    wl_registry_add_listener(registry_, &registry_listener, this);

   
    wl_display_roundtrip(display_);
    wl_display_roundtrip(display_);

    if(!compositor_ || !shm_)
    {
        std::cerr << "missing compositor/shm\n";
        return false;
    }

    cursor_surface_ = wl_compositor_create_surface(compositor_);

    return true;
}


void WaylandBackend::pump()
{
    if(!display_)
        return;


    wl_display_dispatch_pending(display_);
    wl_display_flush(display_);
}


void WaylandBackend::registryGlobal(
    void* data,
    wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version
)
{
    auto self = static_cast<WaylandBackend*>(data);

    if(strcmp(interface, wl_compositor_interface.name) == 0)
    {
        self->compositor_ = static_cast<wl_compositor*>(
            wl_registry_bind(registry, name, &wl_compositor_interface, 4)
        );
    }
    else if(strcmp(interface, wl_shm_interface.name) == 0)
    {
        self->shm_ = static_cast<wl_shm*>(
            wl_registry_bind(registry, name, &wl_shm_interface, 1)
        );
    }
    else if(strcmp(interface, wl_seat_interface.name) == 0)
    {
        self->seat_ = static_cast<wl_seat*>(
            wl_registry_bind(registry, name, &wl_seat_interface, 5)
        );

        wl_seat_add_listener(self->seat_, &seat_listener, self);
    }
}


void WaylandBackend::registryRemove(
    void*,
    wl_registry*,
    uint32_t
)
{
}


void WaylandBackend::seatCapabilities(
    void* data,
    wl_seat* seat,
    uint32_t caps
)
{
    auto self = static_cast<WaylandBackend*>(data);

    if(caps & WL_SEAT_CAPABILITY_POINTER)
    {
        self->pointer_ = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(self->pointer_, &pointer_listener, self);
    }
}


void WaylandBackend::seatName(
    void*,
    wl_seat*,
    const char*
)
{
}


void WaylandBackend::pointerEnter(
    void* data,
    wl_pointer*,
    uint32_t serial,
    wl_surface*,
    wl_fixed_t,
    wl_fixed_t
)
{
    auto self = static_cast<WaylandBackend*>(data);
    self->serial_ = serial;
}


void WaylandBackend::pointerLeave(
    void*,
    wl_pointer*,
    uint32_t,
    wl_surface*
)
{
}


void WaylandBackend::pointerMotion(
    void*,
    wl_pointer*,
    uint32_t,
    wl_fixed_t,
    wl_fixed_t
)
{
}


void WaylandBackend::pointerButton(
    void* data,
    wl_pointer*,
    uint32_t serial,
    uint32_t,
    uint32_t,
    uint32_t
)
{
    auto self = static_cast<WaylandBackend*>(data);
    self->serial_ = serial;
}


void WaylandBackend::pointerAxis(
    void*,
    wl_pointer*,
    uint32_t,
    uint32_t,
    wl_fixed_t
)
{
}


bool WaylandBackend::setCursor(
    int width,
    int height,
    void* pixels,
    int hotspot_x,
    int hotspot_y
)
{
    if(!pointer_ || !serial_)
    {

        std::cerr << "no pointer serial\n";
        return false;
    }

    int stride = width * 4;
    int size = stride * height;

    int fd = create_shm_file(size);
    if(fd < 0)
        return false;

    void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED)
    {
        close(fd);
        return false;
    }

    memcpy(data, pixels, size);

    wl_shm_pool* pool = wl_shm_create_pool(shm_, fd, size);

    wl_buffer* buffer = wl_shm_pool_create_buffer(
        pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888
    );

    wl_shm_pool_destroy(pool);

    wl_surface_attach(cursor_surface_, buffer, 0, 0);
    wl_surface_damage_buffer(cursor_surface_, 0, 0, width, height);
    wl_surface_commit(cursor_surface_);

    wl_pointer_set_cursor(
        pointer_, serial_, cursor_surface_, hotspot_x, hotspot_y
    );

    wl_display_flush(display_);

    destroyBuffer();

    cursor_buffer_ = buffer;
    shm_data_ = data;
    shm_fd_ = fd;
    shm_size_ = size;

    return true;
}


}
