#pragma once

#include <Ultralight/Ultralight.h>
#include <string>


namespace CursorFX {


class UltralightHtmlEffect{

public:

    UltralightHtmlEffect();


    ~UltralightHtmlEffect();


    bool initialize(
        const std::string& file
    );


    void update();


    void draw();



private:

    ultralight::RefPtr<
        ultralight::Renderer
    > renderer_;


    ultralight::RefPtr<
        ultralight::View
    > view_;


    std::string filePath_;


};



}