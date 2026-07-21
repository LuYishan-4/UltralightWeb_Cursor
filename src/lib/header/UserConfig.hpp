#pragma once

#include <unordered_map>
#include <string>


namespace CursorFX
{



class UserConfig
{

public:

    UserConfig();


    bool load();

    bool save();


    // --- HTML 主題路徑 ---

    void setHtmlPath(
        const std::string& path
    );

    std::string htmlPath() const;


    void setSdkPath(
        const std::string& path
    );

    std::string sdkPath() const;



    void setValue(
        const std::string& key,
        const std::string& value
    );

    std::string value(
        const std::string& key,
        const std::string& defaultValue = ""
    ) const;


private:

    std::string configPath_;

    std::unordered_map<std::string, std::string> data_;

};


}