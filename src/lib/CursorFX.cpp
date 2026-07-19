#include "header/CursorFX.hpp"

#include "header/UserConfig.hpp"
#include "header/UltralightHtmlEffect.hpp"
#include "header/MouseUpload.hpp"
#include "header/WaylandBack.hpp"

#include <iostream>
#include <filesystem>


namespace CursorFX
{


CursorEngine::CursorEngine()
{
}


CursorEngine::~CursorEngine()
{
    effect_.reset();
    mouse_.reset();
    wayland_.reset();
    config_.reset();
}


bool CursorEngine::initialize()
{
    std::cout << "[CursorEngine] initialize\n";

    config_ = std::make_unique<UserConfig>();

    if(!config_->load())
    {
        std::cout << "[Config] create default\n";
    }

    std::string html = config_->htmlPath();

    if(html.empty())
    {
        std::cerr << "[Config] html path empty\n";
        return false;
    }

    enabled_ = config_->value("enabled", "true") == "true";


    std::filesystem::path root = std::filesystem::current_path();

    std::filesystem::path sdk =
        root / "sdk/ultralight-free-sdk-1.4.0-linux-x64";

    if(!std::filesystem::exists(sdk))
    {
        std::cerr << "[SDK] not found: " << sdk << "\n";
        return false;
    }

    std::cout << "[SDK] " << sdk << "\n";

    effect_ = std::make_unique<UltralightHtmlEffect>();

    if(!effect_->initialize(html, sdk.string()))
    {
        std::cerr << "[Effect] initialize failed\n";
        return false;
    }

    // *** 新增：CursorEngine 自己建立並初始化 WaylandBackend ***
    // （UltralightHtmlEffect 現在不再管顯示，只負責渲染出 pixel buffer）
    wayland_ = std::make_unique<WaylandBackend>();

    if(!wayland_->initialize())
    {
        std::cerr << "[Wayland] initialize failed（測試模式將看不到游標圖案，但渲染仍會跑）\n";
        wayland_.reset();
    }

    mouse_ = createKdeMouseUploader();

    if(!mouse_->initialize())
    {
        std::cerr << "[MouseUploader] initialize failed\n";

        mouse_.reset();
    }

    std::cout << "[CursorEngine] ready\n";

    return true;
}


void CursorEngine::run()
{
    if(!enabled_ || !effect_)
        return;

    if(mouse_)
    {
        MousePoint pt;

        if(mouse_->poll(pt))
        {
            std::cout
                << "[CursorEngine] poll mouse: "
                << pt.x << "," << pt.y
                << " pressed=" << pt.pressed
                << "\n";

            effect_->move(pt.x, pt.y, pt.pressed);
        }
    }

    // 讓 wayland 那邊也有機會處理排隊中的事件
    if(wayland_)
        wayland_->pump();

    effect_->update();

    // *** 新增：從 effect_ 拉出渲染結果，自己推去顯示 ***
    const uint8_t* pixels = effect_->pixels();

    if(wayland_ && pixels)
    {
        wayland_->setCursor(
            effect_->width(),
            effect_->height(),
            const_cast<uint8_t*>(pixels),
            effect_->width() / 2,
            effect_->height() / 2
        );
    }
}


void CursorEngine::move(
    int x,
    int y
)
{
    if(!enabled_ || !effect_)
        return;

    effect_->move(x, y);
}


bool CursorEngine::setTheme(
    const std::string& path
)
{
    if(!effect_)
        return false;

    if(!effect_->load(path))
        return false;

    if(config_)
    {
        config_->setHtmlPath(path);
        config_->save();
    }

    return true;
}


void CursorEngine::enable(
    bool value
)
{
    enabled_ = value;

    if(config_)
    {
        config_->setValue("enabled", value ? "true" : "false");
        config_->save();
    }
}


}
