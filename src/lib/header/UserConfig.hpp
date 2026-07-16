#pragma once

#include <string>
#include <map>

namespace CursorFX {


class Config {

public:

    Config();


    bool load();


    bool save();



    std::string get(
        const std::string& key
    );


    void set(
        const std::string& key,
        const std::string& value
    );



private:

    std::string path_;


    std::map<
        std::string,
        std::string
    > values_;


};



}