#include "header/UserConfig.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>


namespace fs = std::filesystem;


namespace CursorFX
{


UserConfig::UserConfig()
{
    const char* home = std::getenv("HOME");

    if(home)
    {
        configPath_ = std::string(home) + "/.config/cursorfx/config.ini";
    }
    
}


bool UserConfig::load()
{
    data_.clear();

    if(configPath_.empty())
        return false;

    std::ifstream file(configPath_);

    if(!file.is_open())
    {
        std::cout << "[UserConfig] unnnnnnnn）: " << configPath_ << "\n";
        return false;
    }

    std::string line;

    while(std::getline(file, line))
    {
        auto pos = line.find('=');

        if(pos == std::string::npos)
            continue;

        data_[line.substr(0, pos)] = line.substr(pos + 1);
    }

    std::cout << "[UserConfig] load OK: " << configPath_ << " (" << data_.size() << " 筆)\n";

    return true;
}


bool UserConfig::save()
{
    if(configPath_.empty())
    {
        std::cerr << "[UserConfig] cant load ：configPath_ is empty\n";
        return false;
    }

    fs::create_directories(fs::path(configPath_).parent_path());

    std::ofstream file(configPath_);

    if(!file.is_open())
    {
        std::cerr << "[UserConfig] cant read: " << configPath_ << "\n";
        return false;
    }

    for(const auto& [k, v] : data_)
    {
        file << k << "=" << v << "\n";
    }

    std::cout << "[UserConfig]save: " << configPath_ << "\n";

    return true;
}


void UserConfig::setHtmlPath(
    const std::string& path
)
{
    data_["html"] = path;
}


std::string UserConfig::htmlPath() const
{
    auto it = data_.find("html");

    if(it == data_.end())
        return "";

    if(!fs::exists(it->second))
    {
        std::cerr
            << "[UserConfig] waring：: "
            << it->second
            << "\n";

        return "";
    }

    return it->second;
}


void UserConfig::setSdkPath(
    const std::string& path
)
{
    data_["sdk"] = path;
}


std::string UserConfig::sdkPath() const
{
    auto it = data_.find("sdk");

    if(it == data_.end())
        return "";

    if(!fs::exists(it->second))
    {
    
        return "";
    }

    return it->second;
}


void UserConfig::setValue(
    const std::string& key,
    const std::string& value
)
{
    data_[key] = value;
}


std::string UserConfig::value(
    const std::string& key,
    const std::string& def
) const
{
    auto it = data_.find(key);

    if(it == data_.end())
        return def;

    return it->second;
}


}