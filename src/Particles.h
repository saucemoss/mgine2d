#include <raylib.h>
#include <vector>
#include <string>

enum ParticleShape 
{
    rectangle, circle, line, pixel
};

enum DefinedEmitter
{
    dust, blood, steam, fog, acid_burst
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float rotation;
    float size;
    Color color;
    float gravity;
    ParticleShape shape;
    bool fade;
};

class ParticleEmitter {
public:
    ParticleEmitter(Vector2 pos)
        : m_position(pos)
    {
        m_emitter_life = 1.0f;
        m_forever = false;
        m_shape = rectangle;
        m_size = 1.0f;
        m_gravity = 0.0f;
        m_color = { 255,255,255,255 };
        m_maxParticles = 1;
        m_lifetime = 1.0f;
        m_speed = 1.0f;
        m_spread = 1.0f;
        m_fade = true;
        m_rotation = 0.0f;
        m_size_min, m_size_max = 0.0f;
    }

    bool m_destroy = false;
    void shape(ParticleShape shape)
    {
        m_shape = shape;
    }
    void size(float size)
    {
        m_size = size;
    }
    void size(float size_min, float size_max)
    {
        m_size_min = size_min;
        m_size_max = size_max;
    }
    void gravity(float gravity)
    {
        m_gravity = gravity;
    }
    void color(Color color)
    {
        m_color = color;
    }
    void speed(float speed)
    {
        m_speed = speed;
    }
    void spread(float spread)
    {
        m_spread = spread;
    }
    void howmany(int max_number)
    {
        m_maxParticles = max_number;
    }
    void particle_lifetime(float lifetime)
    {
        m_lifetime = lifetime;
    }
    void emmiter_lifetime(float lifetime)
    {
        m_emitter_life = lifetime;
    }
    void set_forever(bool forever)
    {
        m_forever = forever;
    }
    void fade(bool fade)
    {
        m_fade = fade;
    }
    void rotation(float rotation)
    {
        m_rotation = rotation;
    }

    void EmitParticles() {
        for (int i = 0; i < m_maxParticles; ++i) {
            Particle particle;
            particle.position = m_position;
            particle.velocity = {
             static_cast<float>(GetRandomValue(-m_spread, m_spread)),
             static_cast<float>(GetRandomValue(-m_spread, m_spread))
            };
            particle.lifetime = m_lifetime;
            particle.rotation = m_rotation;
            particle.fade = m_fade;
            particle.gravity = m_gravity;
            particle.shape = m_shape;
            particle.color = m_color;
            particle.size = size();

            m_particles.push_back(particle);
        }
    }

    void UpdateParticles(float deltaTime) {
        for (int i = 0; i < m_particles.size(); ++i) {
            Particle& particle = m_particles[i];
            particle.position.x += particle.velocity.x * m_speed * deltaTime;
            particle.position.y += particle.velocity.y * m_speed * deltaTime;
            particle.lifetime -= deltaTime;
            particle.velocity.y += particle.gravity * deltaTime;

            // Remove dead particles
            if (particle.lifetime <= 0) {
                m_particles.erase(m_particles.begin() + i);
                --i;
            }
        }
    }

    void DrawParticles() {
        for (const auto& particle : m_particles) 
        {
            Color color;
            if (particle.fade)
            {
                float alpha = particle.lifetime / m_lifetime;
                color = { particle.color.r, particle.color.g, particle.color.b, static_cast<unsigned char>(alpha * 255) };
            }
            else
            {
                color = particle.color;
            }
            
            switch (particle.shape)
            {
            case pixel:
                DrawPixelV(particle.position, color);
                break;
            case circle:
                DrawCircleV(particle.position, particle.size / 2, color);
                break;
            case rectangle:
                DrawRectanglePro(Rectangle{ particle.position.x, particle.position.y, particle.size, particle.size },
                    Vector2{ particle.size / 2, particle.size / 2 }, particle.rotation, color);
                break;
            case line:
                break;
            }
            
        }
    }
    float m_emitter_life;
    bool m_forever = false;
private:
    ParticleShape m_shape;
    float m_size, m_size_min, m_size_max;
    float m_gravity = 0.0f;
    Color m_color;
    Vector2 m_position;
    int m_maxParticles;
    float m_lifetime;
    float m_speed;
    float m_spread;
    float m_rotation;
    bool m_fade = true;

    float size()
    {
        if (m_size_max == 0.0f && m_size_min == 0.0f)
        {
            return m_size;
        }
        else
        {
            float temp = float(rand());
            return m_size_min + static_cast <float> (temp) / (static_cast <float> (RAND_MAX / (m_size_max - m_size_min)));
        }
    }
    
    
    std::vector<Particle> m_particles;
};

class ParticlesManager
{
public:
    ParticlesManager();
    static void Update(float dt)
    {
        for (ParticleEmitter* e : emitters)
        {
            e->m_emitter_life -= dt;
            e->UpdateParticles(dt);
        }
        for (int i = 0; i < emitters.size(); i++)
        {
            if (!emitters[i]->m_forever && emitters[i]->m_emitter_life <= 0.0f)
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

    static void Add(DefinedEmitter emmiter, ParticleEmitter* ptr)
    {
        switch (emmiter)
        {
        case dust:
            ptr->shape(rectangle);
            ptr->size(2.0f, 4.0f);
            ptr->speed(6.0f);
            ptr->spread(5.0f);
            ptr->color(WHITE);
            ptr->howmany(5);
            ptr->gravity(8.0f);
            ptr->particle_lifetime(1.0f);
            ptr->emmiter_lifetime(1.0f);
            ptr->fade(true);
            Add(ptr);
            break;
        case blood:
            ptr->shape(rectangle);
            ptr->size(1.0f, 3.0f);
            ptr->speed(5.0f);
            ptr->spread(10.0f);
            ptr->color(RED);
            ptr->howmany(50);
            ptr->gravity(45.0f);
            ptr->particle_lifetime(0.8f);
            ptr->emmiter_lifetime(0.8f);
            ptr->fade(true);
            Add(ptr);
            break;
        }

    }

    static std::vector<ParticleEmitter*> emitters;

};

