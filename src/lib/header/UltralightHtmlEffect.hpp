#pragma once

#include <memory>
#include <string>

#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>

#include "WaylandBack.hpp"


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
        int y
    );



    void update();



private:
     ultralight::RefPtr<
        ultralight::App
    >
    app_;

    ultralight::RefPtr<
        ultralight::Renderer
    >
    renderer_;



    ultralight::RefPtr<
        ultralight::View
    >
    view_;



    std::unique_ptr<
        WaylandBackend
    >
    wayland_;



    std::unique_ptr<
        LocalLoadListener
    >
    listener_;



    bool is_loaded_ = false;


    int width_ = 64;

    int height_ = 64;


};


}