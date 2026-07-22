#include "header/UserConfig.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Quick/PluginPath/PluginPath.hpp"

namespace fs = std::filesystem;


fs::path g_sdkInitialPath;
fs::path g_htmlInitialPath;


namespace UltralightWebCursorM
{


UserConfig::UserConfig(){
    const char* home = std::getenv("HOME");
    if(home)configPath_ = std::string(home) + "/.config/cursorfx/config.ini";
}

bool UserConfig::load(){
    auto base = KWin::PluginPath::dataDir();
    g_sdkInitialPath =        
        base /
        "sdk" /
        "ultralight-free-sdk-1.4.0-linux-x64";
   g_htmlInitialPath =
        g_sdkInitialPath /
        "resources" /
        "index.html";
    data_.clear();
    if(configPath_.empty()) return false;
    std::ifstream file(configPath_);
    if(!file.is_open()){
        //inital start config
        data_["html"] = g_htmlInitialPath.string();
        data_["sdk"]  =  g_sdkInitialPath.string();
        
        return save();
    }

    std::string line;

    //load to ini
    while(std::getline(file, line))
    {
        auto pos = line.find('=');

        if(pos == std::string::npos)
            continue;
        data_[line.substr(0, pos)] = line.substr(pos + 1);
    }
    return true;
}


bool UserConfig::save()
{
    if(configPath_.empty())return false;
    fs::create_directories(fs::path(configPath_).parent_path());
    std::ofstream file(configPath_);

    if(!file.is_open())return false;
    

    for(const auto& [k, v] : data_)
    {
        file << k << "=" << v << "\n";
    }
    return true;
}


void UserConfig::setKeyValue(const std::string& key,const std::string& path){
    std::error_code ec; 
    if (fs::exists(path, ec) && !ec && fs::is_regular_file(path, ec)){
        data_[key] = path;
    }else{
        std::cerr << "[UserConfig] not find file: " << path << "\n";
    }
}

std::string UserConfig::readKeyValue(const std::string& key) const{
    auto it = data_.find(key);
    if(it == data_.end())return "";
    if(!fs::exists(it->second)){
        std::cerr
            << "[UserConfig] warning： not found: "
            << it->second
            << "\n";
        return "";
    }

    return it->second;
}

} // namespace UltralightWebCursorM
