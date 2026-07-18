#include "header/CursorFX.hpp"

#include "header/UserConfig.hpp"
#include "header/UltralightHtmlEffect.hpp"
#include "header/MouseUpload.hpp"

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

    mouse_ = createKdeMouseUploader();

    if(!mouse_->initialize())
    {
        std::cerr
            << "[MouseUploader] initialize failed "<<"\n";

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
            std::cout << "[CursorEngine] poll mouse: " << pt.x << "," << pt.y << "\n";
            effect_->move(pt.x, pt.y);
        }
    }

    effect_->update();
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