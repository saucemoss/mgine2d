#include <raylib.h>
#include <vector>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
};

class ParticleEmitter {
public:
    ParticleEmitter(Vector2 position, int maxParticles, float lifetime, float speed, float spread, bool forever, float emition_time)
        : position(position), maxParticles(maxParticles), lifetime(lifetime), speed(speed), spread(spread), 
          forever(forever), emition_time(emition_time) {}

    bool m_destroy = false;

    void EmitParticles() {
        for (int i = 0; i < maxParticles; ++i) {
            Particle particle;
            particle.position = position;
            particle.velocity = {
             static_cast<float>(GetRandomValue(-spread, spread)),
             static_cast<float>(GetRandomValue(-spread, spread))
            };
            particle.lifetime = lifetime;
            particles.push_back(particle);
        }
    }

    void UpdateParticles(float deltaTime) {
        for (int i = 0; i < particles.size(); ++i) {
            Particle& particle = particles[i];
            particle.position.x += particle.velocity.x * speed * deltaTime;
            particle.position.y += particle.velocity.y * speed * deltaTime;
            particle.lifetime -= deltaTime;

            // Remove dead particles
            if (particle.lifetime <= 0) {
                particles.erase(particles.begin() + i);
                --i;
            }
        }
    }

    void DrawParticles() {
        for (const auto& particle : particles) {
            float alpha = particle.lifetime / lifetime;
            Color color = { 255, 255, 255, static_cast<unsigned char>(alpha * 255) };
            DrawPixelV(particle.position, color);
        }
    }
    bool forever = false;
    float emition_time;
private:
    Vector2 position;
    int maxParticles;
    float lifetime;
    float speed;
    float spread;
    std::vector<Particle> particles;
};

class ParticlesManager
{
public:
    ParticlesManager();
    static void Update(float dt)
    {
        for (ParticleEmitter* e : emitters)
        {
            e->emition_time -= dt;
            e->UpdateParticles(dt);
        }
        for (int i = 0; i < emitters.size(); i++)
        {
            if (!emitters[i]->forever && emitters[i]->emition_time <= 0.0f)
            {
                Remove(emitters[i]);
            }
        }
    }
    static void Draw(int draw_layer)
    {
        for (ParticleEmitter* e : emitters)
        {
            e->DrawParticles();
        }

    }
    static void Add(ParticleEmitter* e)
    {
        emitters.push_back(e);
    }
    static void Remove(ParticleEmitter* e)
    {
        std::vector<ParticleEmitter*>::iterator it = std::find(emitters.begin(), emitters.end(), e);
        if (it != emitters.end())
            emitters.erase(it);
    }

    static std::vector<ParticleEmitter*> emitters;

};

