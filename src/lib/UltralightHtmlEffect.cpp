#include "header/UltralightHtmlEffect.hpp"

#include <AppCore/AppCore.h>
#include <Ultralight/Ultralight.h>

#include <filesystem>
#include <iostream>
#include <cstring>
#include <vector>


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
    )override
    {
        if(is_main_frame)
        {
            std::cout
                << "[Ultralight][debug] OnFinishLoading url=\""
                << url.utf8().data()
                << "\"\n";

            *loaded_ = true;
        }
    }

    void OnFailLoading(
        ultralight::View*,
        uint64_t,
        bool is_main_frame,
        const ultralight::String& url,
        const ultralight::String& desc,
        const ultralight::String& error_domain,
        int error_code
    )override
    {
        if(is_main_frame)
        {
            std::cerr
                << "[Ultralight][debug] OnFailLoading url=\""
                << url.utf8().data()
                << "\" desc=\""
                << desc.utf8().data()
                << "\"\n";

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
    std::cout << "[Ultralight][debug] destructor\n";

    listener_.reset();

    view_ = nullptr;
    renderer_ = nullptr;
    app_ = nullptr;
}


bool UltralightHtmlEffect::initialize(
    const std::string& path,
    const std::string& perpath
)
{
    std::cout
        << "[Ultralight][debug] initialize() path=\""
        << path
        << "\" perpath=\""
        << perpath
        << "\"\n";

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
        std::cerr << "[Ultralight][debug] App::Create failed\n";
        return false;
    }

    std::cout << "[Ultralight][debug] App created OK\n";

    renderer_ = app_->renderer();
    if(!renderer_)
    {
        std::cerr << "[Ultralight][debug] renderer() returned null\n";
        return false;
    }

    std::cout << "[Ultralight][debug] renderer OK\n";

    ultralight::ViewConfig vc;
    vc.is_transparent = true;

    view_ = renderer_->CreateView(width_, height_, vc, nullptr);
    if(!view_)
    {
        std::cerr << "[Ultralight][debug] CreateView failed\n";
        return false;
    }

    std::cout
        << "[Ultralight][debug] view created "
        << width_ << "x" << height_
        << "\n";

    listener_ = std::make_unique<LocalLoadListener>(&is_loaded_);
    view_->set_load_listener(listener_.get());

    std::cout << "[Ultralight][debug] initialize() OK, loading html...\n";

    return load(path);
}


bool UltralightHtmlEffect::load(
    const std::string& path
)
{
    auto p = std::filesystem::absolute(path);

    std::cout << "[Ultralight][debug] load() HTML=" << p << "\n";

    if(!std::filesystem::exists(p))
    {
        std::cerr
            << "[Ultralight][debug] 1111: "
            << p
            << "\n";
    }

    std::string url = "file://" + p.string();

    is_loaded_ = false;

    view_->LoadURL(ultralight::String(url.c_str()));

    return true;
}


void UltralightHtmlEffect::move(
    int x,
    int y,
    bool pressed
)
{
    if(!view_)
    {
        std::cerr << "[Ultralight][debug] move() 但 view_ 是 null\n";
        return;
    }

    std::string js =
        "window.moveCursor(" +
        std::to_string(x) + "," +
        std::to_string(y) + "," +
        (pressed ? std::string("true") : std::string("false")) +
        ");";

    view_->EvaluateScript(ultralight::String(js.c_str()));
}


void UltralightHtmlEffect::update()
{
    static int frame_count = 0;
    frame_count++;

    // 每 60 frame（約 1 秒）印一次狀態，避免洗版
    const bool should_log = (frame_count % 60 == 0);

    if(!renderer_ || !view_)
    {
        std::cerr << "[Ultralight][debug] update() renderer_/view_ 為 null\n";
        return;
    }

    renderer_->Update();

    if(is_loaded_)
    {
        view_->set_needs_paint(true);
    }
    else if(should_log)
    {
        std::cout << "[Ultralight][debug] 尚未 loaded，跳過 paint\n";
    }

    renderer_->Render();

    auto surface = view_->surface();

    if(!surface)
    {
        if(should_log)
            std::cout << "[Ultralight][debug] surface() 是 null\n";

        return;
    }

    auto bitmap_surface = dynamic_cast<ultralight::BitmapSurface*>(surface);

    if(!bitmap_surface)
    {
        if(should_log)
            std::cout << "[Ultralight][debug] surface 不是 BitmapSurface\n";

        return;
    }

    auto bitmap = bitmap_surface->bitmap();

    if(!bitmap)
    {
        if(should_log)
            std::cout << "[Ultralight][debug] bitmap() 是 null\n";

        return;
    }

    bitmap->LockPixels();

    uint8_t* raw = static_cast<uint8_t*>(bitmap->raw_pixels());

    if(!raw)
    {
        std::cerr << "[Ultralight][debug] raw_pixels() 是 null\n";
        bitmap->UnlockPixels();
        return;
    }

    width_ = static_cast<int>(bitmap->width());
    height_ = static_cast<int>(bitmap->height());
    stride_ = static_cast<int>(bitmap->row_bytes());

    const size_t total_bytes = static_cast<size_t>(stride_) * static_cast<size_t>(height_);

    pixel_buffer_.resize(total_bytes);
    std::memcpy(pixel_buffer_.data(), raw, total_bytes);

    bitmap->UnlockPixels();

    if(should_log)
    {
        int visible_pixels = 0;

        for(int y = 0; y < height_; y++)
        {
            const uint8_t* row = pixel_buffer_.data() + y * stride_;

            for(int x = 0; x < width_; x++)
            {
                if(row[x * 4 + 3] != 0)
                    visible_pixels++;
            }
        }

        std::cout
            << "[Ultralight][debug] frame=" << frame_count
            << " loaded=" << is_loaded_
            << " size=" << width_ << "x" << height_
            << " stride=" << stride_
            << " visible_pixels=" << visible_pixels
            << "/" << (width_ * height_)
            << "\n";
    }
}


const uint8_t* UltralightHtmlEffect::pixels() const
{
    if(pixel_buffer_.empty())
        return nullptr;

    return pixel_buffer_.data();
}



}
