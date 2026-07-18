#include "header/UserConfig.hpp"
#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;


namespace CursorFX
{


static fs::path getProjectRoot()
{
    char buffer[4096];

    ssize_t len =
        readlink(
            "/proc/self/exe",
            buffer,
            sizeof(buffer)-1
        );


    if(len <= 0)
        return fs::current_path();


    buffer[len] = '\0';


    fs::path exe =
        buffer;


    // cursor-ani
    // src
    // build
    // project root

    return exe
        .parent_path() // src
        .parent_path() // build
        .parent_path(); // project
}


UserConfig::UserConfig()
{
    const char* home =
        std::getenv(
            "HOME"
        );


    if(home)
    {
        configPath_ =
            std::string(home)
            +
            "/.config/cursorfx/config.ini";
    }
}



bool UserConfig::load()
{
    data_.clear();


    std::ifstream file(
        configPath_
    );


    if(!file.is_open())
        return false;



    std::string line;


    while(
        std::getline(
            file,
            line
        )
    )
    {
        auto pos =
            line.find('=');


        if(
            pos ==
            std::string::npos
        )
            continue;


        data_[
            line.substr(0,pos)
        ]
        =
        line.substr(pos+1);
    }


    return true;
}



bool UserConfig::save()
{
    fs::create_directories(
        fs::path(
            configPath_
        )
        .parent_path()
    );


    std::ofstream file(
        configPath_
    );


    if(!file.is_open())
        return false;


    for(auto& [k,v] : data_)
    {
        file
        << k
        << "="
        << v
        << "\n";
    }


    return true;
}



void UserConfig::setHtmlPath(
    const std::string& path
)
{
    data_["html"] =
        path;
}



std::string UserConfig::htmlPath() const
{
    auto it =
        data_.find("html");


    if(it != data_.end())
    {
        if(
            fs::exists(
                it->second
            )
        )
        {
            return it->second;
        }
    }



    fs::path root =
        getProjectRoot();



    return (
        root /
        "testHtml/index.html"
    )
    .lexically_normal()
    .string();
}


void UserConfig::setValue(
    const std::string& key,
    const std::string& value
)
{
    data_[key] =
        value;
}



std::string UserConfig::value(
    const std::string& key,
    const std::string& def
) const
{
    auto it =
        data_.find(
            key
        );


    if(
        it ==
        data_.end()
    )
    {
        return def;
    }


    return it->second;
}


}