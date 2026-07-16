#pragma once


#include <memory>
#include <string>


namespace CursorFX {


class UltralightRenderer;
class Config;



class GUIManager {


public:


    GUIManager();
    ~GUIManager();
    bool initialize(
        Config& config
    );

    void show();
    void hide();



    void update();



private:

};



}