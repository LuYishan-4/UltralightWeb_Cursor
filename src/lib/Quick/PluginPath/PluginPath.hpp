
#pragma once

#include <filesystem>

namespace KWin
{

class PluginPath
{
public:
    static std::filesystem::path dataDir();
};

}