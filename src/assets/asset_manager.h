#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns {

class AssetManager {
public:
    static AssetManager& instance();

    void load_texture(const std::string& key, const std::string& path);
    void load_textures_from_dir(const std::string& dir, const std::string& prefix);
    Texture2D get_texture(const std::string& key) const;
    bool has_texture(const std::string& key) const;

    void load_sound(const std::string& key, const std::string& path);
    Sound get_sound(const std::string& key) const;

    void load_music(const std::string& key, const std::string& path);
    Music get_music(const std::string& key) const;

    void unload_all();

private:
    AssetManager() = default;
    std::unordered_map<std::string, Texture2D> textures_;
    std::unordered_map<std::string, Sound> sounds_;
    std::unordered_map<std::string, Music> music_;
};

} // namespace ns
