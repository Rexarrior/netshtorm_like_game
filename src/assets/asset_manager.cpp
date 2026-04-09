#include "asset_manager.h"
#include <iostream>

#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "No filesystem library available"
#endif

namespace ns {

AssetManager& AssetManager::instance() {
    static AssetManager inst;
    return inst;
}

void AssetManager::load_texture(const std::string& key, const std::string& path) {
    Texture2D tex = LoadTexture(path.c_str());
    if (tex.id == 0) {
        std::cerr << "[AssetManager] WARNING: Failed to load texture: " << path << std::endl;
    }
    textures_[key] = tex;
}

void AssetManager::load_textures_from_dir(const std::string& dir, const std::string& prefix) {
    if (!fs::exists(dir)) {
        std::cerr << "[AssetManager] Directory not found: " << dir << std::endl;
        return;
    }
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".png" || ext == ".jpg") {
                std::string stem = entry.path().stem().string();
                std::string key = prefix.empty() ? stem : prefix + "_" + stem;
                load_texture(key, entry.path().string());
            }
        }
    }
}

Texture2D AssetManager::get_texture(const std::string& key) const {
    auto it = textures_.find(key);
    if (it != textures_.end()) return it->second;
    return Texture2D{0};
}

bool AssetManager::has_texture(const std::string& key) const {
    return textures_.find(key) != textures_.end();
}

void AssetManager::load_sound(const std::string& key, const std::string& path) {
    Sound snd = LoadSound(path.c_str());
    sounds_[key] = snd;
}

Sound AssetManager::get_sound(const std::string& key) const {
    auto it = sounds_.find(key);
    if (it != sounds_.end()) return it->second;
    return Sound{0};
}

void AssetManager::load_music(const std::string& key, const std::string& path) {
    Music mus = LoadMusicStream(path.c_str());
    music_[key] = mus;
}

Music AssetManager::get_music(const std::string& key) const {
    auto it = music_.find(key);
    if (it != music_.end()) return it->second;
    return Music{0};
}

void AssetManager::unload_all() {
    for (auto& [key, tex] : textures_) UnloadTexture(tex);
    for (auto& [key, snd] : sounds_) UnloadSound(snd);
    for (auto& [key, mus] : music_) UnloadMusicStream(mus);
    textures_.clear();
    sounds_.clear();
    music_.clear();
}

} // namespace ns
