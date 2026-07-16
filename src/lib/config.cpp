#include"header/UserConfig.hpp"


#include <fstream>

#include <filesystem>

#include <cstdlib>


namespace CursorFX {



Config::Config()
{

    const char* home =
        getenv("HOME");


    path_ =
        std::string(home)
        +
        "/.config/cursorfx/config.ini";

}




bool Config::load()
{

    std::ifstream file(
        path_
    );


    if(!file)
        return false;



    std::string key;
    std::string value;


    while(file >> key >> value)
    {

        values_[key]=value;

    }



    return true;

}





bool Config::save()
{

    std::filesystem::create_directories(
        std::filesystem::path(path_).parent_path()
    );


    std::ofstream file(
        path_
    );


    for(auto& [k,v]:values_)
    {

        file
        << k
        << " "
        << v
        << "\n";

    }


    return true;

}




std::string Config::get(
    const std::string& key
)
{

    auto it = values_.find(key);


    if(it != values_.end())
    {
        return it->second;
    }


    return "";

}




void Config::set(
    const std::string& key,
    const std::string& value
)
{

    values_[key]=value;

}



}