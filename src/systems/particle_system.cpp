#include "particle_system.h"
#include <algorithm>

namespace ns {

void ParticleSystem::update(float dt) {
    for (auto& p : particles_) {
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.lifetime -= dt;
    }
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const Particle& p) { return p.lifetime <= 0; }),
        particles_.end()
    );
}

void ParticleSystem::emit(Vector2 position, Vector2 velocity, Color color, float size, float lifetime) {
    Particle p;
    p.position = position;
    p.velocity = velocity;
    p.color = color;
    p.size = size;
    p.lifetime = lifetime;
    p.max_lifetime = lifetime;
    particles_.push_back(p);
}

void ParticleSystem::emit_burst(Vector2 position, int count, Color color, float speed, float size, float lifetime) {
    for (int i = 0; i < count; ++i) {
        float angle = static_cast<float>(i) / count * 6.28318f;
        Vector2 vel = {cosf(angle) * speed, sinf(angle) * speed};
        emit(position, vel, color, size, lifetime);
    }
}

void ParticleSystem::render() {
    for (const auto& p : particles_) {
        float alpha = p.lifetime / p.max_lifetime;
        Color c = {p.color.r, p.color.g, p.color.b, static_cast<unsigned char>(p.color.a * alpha)};
        DrawCircleV(p.position, p.size, c);
    }
}

void ParticleSystem::clear() { particles_.clear(); }

} // namespace ns