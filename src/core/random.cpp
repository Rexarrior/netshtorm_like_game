#include "random.h"
#include <chrono>

namespace ns {

RNG::RNG() : rng_(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())) {}

RNG& RNG::instance() {
    static RNG inst;
    return inst;
}

void RNG::seed(unsigned int s) {
    rng_.seed(s);
}

int RNG::range(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

float RNG::range_float(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng_);
}

bool RNG::chance(float percent) {
    return range_float(0.0f, 100.0f) < percent;
}

} // namespace ns
