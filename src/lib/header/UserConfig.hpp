#pragma once

#include <string>
#include <unordered_map>

namespace UltralightWebCursorM
{

extern std::filesystem::path g_sdkInitialPath;
extern std::filesystem::path g_htmlInitialPath;

class UserConfig
{
public:
    /**
     * @brief Constructor. Automatically initializes the configuration file path 
     *        (defaults to ~/.config/cursorfx/config.ini).
     */
    UserConfig();

    /**
     * @brief Default destructor.
     */
    ~UserConfig() = default;

    /**
     * @brief Loads the configuration file. If the file does not exist, 
     *        it automatically initializes with default parameters and saves it.
     * @return true if loading or initialization succeeds; false otherwise.
     */
    bool load();

    /**
     * @brief Saves the current configuration data to the disk file 
     *        (automatically creates parent directories if they don't exist).
     * @return true if saving succeeds; false otherwise.
     */
    bool save();

    /**
     * @brief Sets a custom generic key-value pair configuration.
     * @param key The name of the configuration item.
     * @param value The value of the configuration item.
     */
    void setKeyValue(const std::string& key,const std::string& path);


     /**
     * @brief Retrieves the value of a specified key, returning a default value if it does not exist.
     * @param key The name of the configuration item.
     * @param def The default value to return if the key is not found.
     * @return The value associated with the key, or the default value.
     */
    std::string readKeyValue(const std::string& key) const;

private:
    std::string configPath_;                            // Full path of the configuration file.
    std::unordered_map<std::string, std::string> data_;  // Hash map storing the INI configuration data.
};

} // namespace UltralightWebCursorM
