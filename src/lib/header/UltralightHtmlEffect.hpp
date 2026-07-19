#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>


namespace CursorFX
{


class LocalLoadListener;



class UltralightHtmlEffect
{

public:

    UltralightHtmlEffect();

    ~UltralightHtmlEffect();


    bool initialize(
        const std::string& path,
        const std::string& perpath
    );


    bool load(
        const std::string& path
    );


    void move(
        int x,
        int y,
        bool pressed = false
    );


    // 每個 frame 呼叫一次，驅動 Ultralight Update()/Render()
    void update();


    // --- 給外部（KWin Effect）拉取渲染結果用 ---

    // 目前渲染出來的 ARGB pixel buffer；尚未渲染出東西時回傳 nullptr
    const uint8_t* pixels() const;

    int width() const
    {
        return width_;
    }

    int height() const
    {
        return height_;
    }

    // stride（每一行的 byte 數，不一定等於 width*4，需另外對齊）
    int stride() const
    {
        return stride_;
    }

    bool isLoaded() const
    {
        return is_loaded_;
    }


private:

    ultralight::RefPtr<ultralight::App> app_;
    ultralight::RefPtr<ultralight::Renderer> renderer_;
    ultralight::RefPtr<ultralight::View> view_;

    std::unique_ptr<LocalLoadListener> listener_;

    bool is_loaded_ = false;

    int width_ = 64;
    int height_ = 64;
    int stride_ = 0;

    std::vector<uint8_t> pixel_buffer_;

};


}
