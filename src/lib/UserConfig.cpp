#include "header/UserConfig.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Quick/PluginPath/PluginPath.hpp"
#include <algorithm>

namespace fs = std::filesystem;


fs::path g_sdkInitialPath;
fs::path g_htmlInitialPath;


namespace UltralightWebCursorM
{
    fs::path g_sdkInitialPath;
    fs::path g_htmlInitialPath;

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
        "resources";

    data_.clear();
    if(configPath_.empty()) return false;
    std::ifstream file(configPath_);
    if(!file.is_open()){
        //inital start config
        data_["html"] = g_htmlInitialPath/  "default"/ "index.html";
        data_["sdk"]  =  g_sdkInitialPath.string();
        data_["blacklist"] = "";
        data_["width"] = "128";
        data_["height"] = "128";
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
    data_[key] = path;
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
std::vector<std::string>UserConfig::getBlacklist() const{
    std::vector<std::string> result;
    auto it = data_.find("blacklist");
    if(it == data_.end() || it->second.empty())
        return result;

    std::stringstream ss(it->second);
    std::string item;


    while(std::getline(ss, item, ','))
    {
        if(!item.empty())
            result.push_back(item);
    }


    return result;
}





void UserConfig::appendBlacklist(const std::string& app) {
    if (app.empty()) return;

    std::string current_list = data_["blacklist"]; 

    if (current_list.empty()) {
        data_["blacklist"] = app; 
        save();
        return;
    }

    size_t pos = current_list.find(app);
    while (pos != std::string::npos) {
        bool match_start = (pos == 0 || current_list[pos - 1] == ',');
        bool match_end = (pos + app.length() == current_list.length() || current_list[pos + app.length()] == ',');
        
        if (match_start && match_end) {
            return; 
        }
        pos = current_list.find(app, pos + 1);
    }
    current_list += "," + app;
    data_["blacklist"] = current_list; 
    save();
}

void UserConfig::removeBlacklist(const std::string& app){
    std::string current_list = data_["blacklist"];
    if (current_list.empty()) return;

    std::string new_value;
    std::string token;
    std::stringstream ss(current_list);

    while (std::getline(ss, token, ',')) {
        if (token == app) continue; 
        if (!new_value.empty())new_value += ",";
        new_value += token;
    }
    data_["blacklist"] = new_value;
    save();
}
bool UserConfig::uploadTheme(const std::string& path,const std::string& themeName){
    namespace fs = std::filesystem;
    fs::path src(path);
    if(!fs::exists(src))return false;
    if(!fs::is_directory(src))return false;

    fs::path resources =
        g_sdkInitialPath /
        "resources";

    fs::create_directories(resources);

    fs::path dst =resources /themeName;

 
    if(fs::exists(dst))return false;
    
    fs::copy(
        src,
        dst,
        fs::copy_options::recursive
    );

    return true;
}
void UserConfig::setTheme(
    const std::string& themeName
)
{
    data_["html"] =
        (
            g_htmlInitialPath /
            themeName /
            "index.html"
        ).string();
}
std::string UserConfig::currentTheme() const{
    auto it = data_.find("html");
    if(it == data_.end() || it->second.empty()){
        return "default";
    }
    std::filesystem::path htmlPath =
        it->second;
    auto themePath =htmlPath.parent_path();
    if(themePath.filename().empty()){
        return "default";
    }
    return themePath.filename().string();
}
}
