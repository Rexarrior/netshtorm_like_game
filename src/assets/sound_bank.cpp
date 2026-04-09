#include "sound_bank.h"
#include "asset_manager.h"
#include <iostream>

namespace ns {

SoundBank& SoundBank::instance() {
    static SoundBank inst;
    return inst;
}

void SoundBank::register_sound(const std::string& key) {
    registered_sounds_[key] = 0;
}

void SoundBank::play(const std::string& key) const {
    auto it = registered_sounds_.find(key);
    if (it == registered_sounds_.end()) return;
    auto& am = AssetManager::instance();
    Sound snd = am.get_sound(key);
    if (IsSoundValid(snd)) {
        PlaySound(snd);
    }
}

} // namespace ns
