#include "header/UltralightHtmlEffect.hpp"

#include <AppCore/AppCore.h>
#include <Ultralight/Ultralight.h>

#include <filesystem>
#include <iostream>
#include <cstring>


namespace CursorFX
{


class LocalLoadListener :
    public ultralight::LoadListener
{

public:

    explicit LocalLoadListener(bool* flag)
        : loaded_(flag)
    {
    }

    void OnFinishLoading(
        ultralight::View*,
        uint64_t,
        bool is_main_frame,
        const ultralight::String& url
    )
    {
        if(is_main_frame)
        {
            std::cout << "[Ultralight] loaded " << url.utf8().data() << "\n";
            *loaded_ = true;
        }
    }

    void OnFailLoading(
        ultralight::View*,
        uint64_t,
        bool is_main_frame,
        const ultralight::String&,
        const ultralight::String& desc,
        const ultralight::String&
    )
    {
        if(is_main_frame)
        {
            std::cerr << "[Ultralight] failed " << desc.utf8().data() << "\n";
            *loaded_ = false;
        }
    }

private:

    bool* loaded_;
};


UltralightHtmlEffect::UltralightHtmlEffect()
{
}


UltralightHtmlEffect::~UltralightHtmlEffect()
{
    listener_.reset();
    wayland_.reset();

    view_ = nullptr;
    renderer_ = nullptr;
    app_ = nullptr;
}


bool UltralightHtmlEffect::initialize(
    const std::string& path,
    const std::string& perpath
)
{
    std::cout << "[Ultralight] init\n";

    // --- Platform Config ---
    ultralight::Config config;
    config.resource_path_prefix = ultralight::String("resources/");

    auto& platform = ultralight::Platform::instance();
    platform.set_config(config);
    platform.set_font_loader(ultralight::GetPlatformFontLoader());
    platform.set_file_system(
        ultralight::GetPlatformFileSystem(ultralight::String(perpath.c_str()))
    );


    ultralight::Settings settings;

    app_ = ultralight::App::Create(settings);
    if(!app_)
    {
        std::cerr << "App failed\n";
        return false;
    }

    renderer_ = app_->renderer();
    if(!renderer_)
    {
        std::cerr << "Renderer failed\n";
        return false;
    }

    ultralight::ViewConfig vc;
    vc.is_transparent = true;

    view_ = renderer_->CreateView(width_, height_, vc, nullptr);
    if(!view_)
    {
        std::cerr << "View failed\n";
        return false;
    }


    listener_ = std::make_unique<LocalLoadListener>(&is_loaded_);
    view_->set_load_listener(listener_.get());


    wayland_ = std::make_unique<WaylandBackend>();
    if(!wayland_->initialize())
    {
        std::cerr << "[Wayland] initialize failed (cursor output disabled)\n";
        wayland_.reset();
    }

    std::cout << "[Ultralight] init OK\n";

    return load(path);
}


bool UltralightHtmlEffect::load(
    const std::string& path
)
{
    auto p = std::filesystem::absolute(path);
    std::cout << "[Ultralight] HTML=" << p << "\n";

    std::string url = "file://" + p.string();

    view_->LoadURL(ultralight::String(url.c_str()));

    return true;
}


void UltralightHtmlEffect::move(
    int x,
    int y
)
{
    if(!view_)
        return;

    std::string js =
        "window.moveCursor(" + std::to_string(x) + "," + std::to_string(y) + ");";

    view_->EvaluateScript(ultralight::String(js.c_str()));
}


void UltralightHtmlEffect::update()
{
    if(!renderer_ || !view_)
        return;


    if(wayland_)
        wayland_->pump();

    renderer_->Update();

    if(is_loaded_)
        view_->set_needs_paint(true);

    renderer_->Render();

    auto surface = view_->surface();
    if(!surface)
        return;

    auto bitmap_surface = dynamic_cast<ultralight::BitmapSurface*>(surface);
    if(!bitmap_surface)
        return;

    auto bitmap = bitmap_surface->bitmap();
    if(!bitmap)
        return;

    bitmap->LockPixels();

    uint8_t* pixels = static_cast<uint8_t*>(bitmap->raw_pixels());
    std::cout<<"\n w"<<"\n";

    if(pixels && wayland_)
    {
        wayland_->setCursor(
            bitmap->width(),
            bitmap->height(),
            pixels,
            bitmap->width() / 2,
            bitmap->height() / 2
        );
    }

    bitmap->UnlockPixels();
}


}