#pragma once

#include <random>

namespace ns {

class RNG {
public:
    static RNG& instance();
    void seed(unsigned int s);
    int range(int min, int max);
    float range_float(float min, float max);
    bool chance(float percent);

private:
    RNG();
    std::mt19937 rng_;
};

} // namespace ns
