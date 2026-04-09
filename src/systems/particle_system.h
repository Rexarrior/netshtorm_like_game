#pragma once

#include <raylib.h>
#include <vector>

namespace ns {

struct Particle {
    Vector2 position{0, 0};
    Vector2 velocity{0, 0};
    float lifetime = 1.0f;
    float max_lifetime = 1.0f;
    Color color{255, 255, 255, 255};
    float size = 2.0f;
};

class ParticleSystem {
public:
    void update(float dt);
    void emit(Vector2 position, Vector2 velocity, Color color, float size, float lifetime);
    void emit_burst(Vector2 position, int count, Color color, float speed, float size, float lifetime);
    void render();
    void clear();

private:
    std::vector<Particle> particles_;
};

} // namespace ns