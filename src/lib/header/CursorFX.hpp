#pragma once

#include <memory>
#include <vector>
#include <string>


namespace CursorFX
{

class UserConfig;
class UltralightHtmlEffect;
class IMouseUploader;
class WaylandBackend;


class CursorEngine
{

public:

    CursorEngine();

    ~CursorEngine();


    bool initialize();


    void run();


    void move(
        int x,
        int y
    );


    bool setTheme(
        const std::string& path
    );


    void enable(
        bool value
    );


private:

    std::unique_ptr<UserConfig> config_;

    std::unique_ptr<UltralightHtmlEffect> effect_;

    std::unique_ptr<IMouseUploader> mouse_;


    std::unique_ptr<WaylandBackend> wayland_;


    bool enabled_ = true;

};


}
