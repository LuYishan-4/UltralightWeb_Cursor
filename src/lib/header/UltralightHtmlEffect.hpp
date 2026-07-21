#pragma once

#include <Ultralight/Ultralight.h>

#include <filesystem>
#include <memory>
#include <vector>
#include <string>


namespace CursorFX
{


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


    void update();


    void move(
        int x,
        int y,
        bool pressed
    );

    void reload();

    void setEnabled(bool enabled);

    bool isEnabled() const;


    bool hasNewFrame() const;

    void clearNewFrame();


    const uint8_t* pixels() const;


    int width() const
    {
        return width_;
    }


    int height() const
    {
        return height_;
    }


    int stride() const
    {
        return stride_;
    }



private:


    ultralight::RefPtr<ultralight::Renderer> renderer_;


    ultralight::RefPtr<ultralight::View> view_;


    std::unique_ptr<ultralight::LoadListener> listener_;



    bool is_loaded_ = false;



    // 是否執行 Ultralight
    bool enabled_ = true;


    // bitmap 是否更新
    bool new_frame_ = false;



    int width_ = 64;

    int height_ = 64;

    int stride_ = 0;



    std::vector<uint8_t> pixel_buffer_;



    std::filesystem::path html_path_;


    std::filesystem::file_time_type html_time_;

};


}