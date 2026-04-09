#pragma once

#include <string>
#include <unordered_map>

namespace ns {

class SoundBank {
public:
    static SoundBank& instance();
    void register_sound(const std::string& key);
    void play(const std::string& key) const;

private:
    SoundBank() = default;
    std::unordered_map<std::string, int> registered_sounds_;
};

} // namespace ns
