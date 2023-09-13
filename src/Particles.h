#ifndef PARTICLES_H
#define PARTICLES_H

#include <raylib.h>
#include <vector>
#include <string>
#include <iostream>
#include "Animations.h"
#include <raymath.h>


enum ParticleShape
{
    rectangle, circle, circle_lines, line, pixel, texture
};

enum DefinedEmitter
{
    dust, blood, steam, fog, acid_bursts, acid_head_burst, acid_projectile, 
    ambient_particles, acid_explosion, ambient_particles_foreground, glass_explosion, white_burst,
    energy_orb, breath_proj, breath_small_anticipation, dash_shadow, dash_shadow_left
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Vector2 impulse;
    float lifetime;
    float rotation;
    float size;
    Color color;
    float gravity;
    ParticleShape shape;
    bool fade;
    bool clockwise_rot = GetRandomValue(0, 1) == 0 ? true : false;
    bool animated = false;
    float frame_timer = 0.0f;
    int currentFrame = 0;
};

class ParticleEmitter {
public:
    ParticleEmitter(Vector2 pos)
        : m_position(pos)
    {
        m_emitter_life = 1.0f;
        m_forever = false;
        m_shape = ParticleShape::rectangle;
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
        m_spawn_radius = 0.0f;
        m_rotation_speed = 0.0f;
        m_rotation = false;
        m_emittion_counter = 0.0f;
        m_fade_in = false;
        m_fade_in_out = false;
        m_direction_vector = { 0,0 };
        m_spawn_impulse = { 0,0 };
        m_spawn_impulse_min = { 0,0 };
        m_spawn_impulse_max = { 0,0 };
        m_parallaxOffset = { 0,0 };
    }
    ~ParticleEmitter()
    {
        //UnloadTexture(textrue);
    }

    bool m_destroy = false;
    void set_draw_level(int layer)
    {
        draw_layer_level = layer;
    }
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
    void spawn_impulse(Vector2 impulse)
    {
        m_spawn_impulse = impulse;
    }
    void spawn_impulse(Vector2 impulse_min, Vector2 impulse_max)
    {
        m_spawn_impulse_min = impulse_min;
        m_spawn_impulse_max = impulse_max;
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
    void spawn_radius(float radius)
    {
        m_spawn_radius = radius;
    }
    void rotation_speed(float speed)
    {
        m_rotation_speed = speed;
    }
    void random_rotation(bool random_rotation)
    {
        m_rand_rotation = random_rotation;
    }
    void emittion_counter(float counter)
    {
        m_emittion_counter = counter;
    }
    void fade_in(bool fade_in)
    {
        m_fade_in = fade_in;
    }
    void fade_in_out(bool fade_in_out)
    {
        m_fade_in_out = fade_in_out;
    }
    void direction_vector(Vector2 vec)
    {
        m_direction_vector = vec;
    }
    void position(Vector2 pos)
    {
        m_position = pos;
    }
    void texture(Texture2D texture)
    {
        m_textrue = texture;
    }
    void animated()
    {
        m_animated = true;
    }
    void random_texture_rect()
    {
        m_randomTextureRect = true;
    }
    void paralax_offset(float o)
    {
        m_parallaxOffset.x = o;
        m_parallaxOffset.y = o;
    }

    void EmitParticles() {
        for (int i = 0; i < m_maxParticles; ++i) {
            Particle particle;
            particle.position = position();
            particle.velocity = {
             static_cast<float>(GetRandomValue(-m_spread, m_spread)),
             static_cast<float>(GetRandomValue(-m_spread, m_spread))
            };
            particle.lifetime = m_lifetime;
            particle.rotation = m_rotation;
            if (m_rand_rotation)
            {
                particle.rotation = GetRandomValue(0,360);
            }
            particle.fade = m_fade;
            particle.gravity = m_gravity;
            particle.shape = m_shape;
            particle.color = m_color;
            particle.size = size();
            particle.animated = m_animated;

            if (m_shape == ParticleShape::texture && m_randomTextureRect) {
                int frameWidth = m_textrue.width / 4;
                int frameHeight = m_textrue.height / 1;
                int randFrame = GetRandomValue(0, 4 * 1 - 1);
                particle.currentFrame = randFrame;
            }

            m_particles.push_back(particle);
        }
    }

    void UpdateParticles(float dt) {

        if (m_emittion_counter != 0.0f)
        {
            m_emittion_clock += dt;
            if (m_emittion_clock >= m_emittion_counter)
            {
                EmitParticles();
                m_emittion_clock = 0.0f;
            }
        }

        for (int i = 0; i < m_particles.size(); ++i) {
            Particle& particle = m_particles[i];
            particle.position.x += particle.velocity.x * m_speed * dt;
            particle.position.y += particle.velocity.y * m_speed * dt;
            particle.velocity.y += particle.gravity * dt;

            if (m_direction_vector.x != 0.0f)
            {
                particle.position.x += m_direction_vector.x;
            }
            if (m_direction_vector.y != 0.0f)
            {
                particle.position.y += m_direction_vector.y;
            }

            particle.lifetime -= dt;
            if (m_rotation_speed != 0.0f)
            {
                particle.clockwise_rot ? particle.rotation += m_rotation_speed * dt * 1000
                                       : particle.rotation -= m_rotation_speed * dt * 1000;
            }


            if (particle.animated)
            {
                particle.frame_timer += dt;
                if (particle.frame_timer >= 0.08f)
                {
                    particle.frame_timer = 0.0f;
                    particle.currentFrame > 4 ? particle.currentFrame = 0 : particle.currentFrame++;
                }
            }

            // Remove dead particles
            if (particle.lifetime <= 0) {
                m_particles.erase(m_particles.begin() + i);
                --i;
            }
        }
    }

    void DrawParticles(Vector2 cam_pos) {
        for (const auto& particle : m_particles) 
        {
            Color color;
            float alpha = particle.color.a;

            if (m_fade_in_out)
            {
                if (particle.lifetime < m_lifetime / 2) {
                    alpha = particle.lifetime / (m_lifetime / 2);
                }
                else {
                    alpha = 1.0f - (particle.lifetime - m_lifetime / 2) / (m_lifetime / 2);
                }
                color = { particle.color.r, particle.color.g, particle.color.b, static_cast<unsigned char>(alpha * particle.color.a) };
            }
            else if (m_fade_in)
            {
                alpha = particle.lifetime * -1 / m_lifetime;  
                color = { particle.color.r, particle.color.g, particle.color.b, static_cast<unsigned char>(alpha * particle.color.a) };
            }
            else if (particle.fade)
            {
                alpha = particle.lifetime / m_lifetime;
                color = { particle.color.r, particle.color.g, particle.color.b, static_cast<unsigned char>(alpha * particle.color.a) };
            }
            else
            {
                color = particle.color;
            }
            
            switch (particle.shape)
            {
            case ParticleShape::pixel:
                DrawPixelV(particle.position, color);
                break;
            case ParticleShape::circle:
                DrawCircleV(particle.position, particle.size / 2, color);
                break;
            case ParticleShape::circle_lines:
                DrawPolyLinesEx(particle.position, 10, particle.size / 2, particle.rotation, 0.8f, color);
                break;
            case ParticleShape::rectangle:
                DrawRectanglePro(Rectangle{ particle.position.x, particle.position.y, particle.size, particle.size },
                    Vector2{ particle.size / 2, particle.size / 2 }, particle.rotation, color);
                break;
            case ParticleShape::line:
                break;
            case ParticleShape::texture:
      
                int frameWidth = m_textrue.width;
                int frameHeight = m_textrue.height;
                Rectangle sourceRect = { 0,0, frameWidth, frameHeight};
                if (m_animated || m_randomTextureRect)
                {
                    frameWidth = m_textrue.width / 4;  // Adjust numFramesPerRow accordingly
                    frameHeight = m_textrue.height / 1;  // Adjust numFramesPerCol accordingly
                    sourceRect = {
                        static_cast<float>(particle.currentFrame % 4) * frameWidth,
                        static_cast<float>(particle.currentFrame / 1) * frameHeight,
                        static_cast<float>(frameWidth),
                        static_cast<float>(frameHeight)
                    };
                }

                Vector2 parallaxed = Vector2Multiply(cam_pos, m_parallaxOffset);
                DrawTexturePro(m_textrue, sourceRect,
                    { particle.position.x+parallaxed.x, particle.position.y + parallaxed.y, particle.size, particle.size },
                    Vector2{ particle.size / 2, particle.size / 2 }, particle.rotation, color);
                break;
            }
            
        }
    }
    float m_emitter_life;
    bool m_forever = false;
    int draw_layer_level = 1;
private:
    Texture2D m_textrue;
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
    float m_rotation_speed;
    bool m_rand_rotation;
    bool m_fade = true;
    float m_spawn_radius;
    float m_emittion_counter;
    float m_emittion_clock;
    bool m_fade_in;
    bool m_fade_in_out;
    bool m_animated = false;
    bool m_randomTextureRect;
    Vector2 m_parallaxOffset;
    Vector2 m_direction_vector;
    Vector2 m_spawn_impulse;
    Vector2 m_spawn_impulse_min;
    Vector2 m_spawn_impulse_max;
    

    float size()
    {
        if (m_size_max == 0.0f && m_size_min == 0.0f)
        {
            return m_size;
        }
        else
        {
            return m_size_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max - m_size_min)));
        }
    }

    Vector2 position()
    {
        if (m_spawn_radius == 0.0f)
        {
            return m_position;
        }
        else
        {
            float posX = -m_spawn_radius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_spawn_radius + m_spawn_radius)));
            float posY = -m_spawn_radius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_spawn_radius + m_spawn_radius)));
            return { m_position.x + posX, m_position.y + posY };
        }
    }

    std::vector<Particle> m_particles;
};

class ParticlesManager
{
public:
    ParticlesManager();

    static RenderTexture2D particlesRenderT;
    static Texture2D ParticlesTexture;

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
                delete emitters[i];
                Remove(emitters[i]);
            }
        }
    }
    static void Draw(int draw_layer, Vector2 cam_pos)
    {
        BeginTextureMode(particlesRenderT);
        ClearBackground(BLANK);
        for (ParticleEmitter* e : emitters)
        {
            if (e->draw_layer_level = draw_layer)
            {
                e->DrawParticles(cam_pos);

            }
        }
        EndTextureMode();
        ParticlesTexture = particlesRenderT.texture;

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

    static void UnloadEmitters()
    {
        std::cout << emitters.size() << std::endl;
        for (int i = 0; i < emitters.size(); i++)
        {
            emitters[i]->set_forever(false);
        }
    }

    static void Add(DefinedEmitter emmiter, ParticleEmitter* ptr)
    {
        switch (emmiter)
        {
        case dust:
            ptr->shape(ParticleShape::rectangle);
            ptr->size(2.0f, 4.0f);
            ptr->speed(5.0f);
            ptr->spread(4.0f);
            ptr->color(WHITE);
            ptr->howmany(3);
            ptr->gravity(7.0f);
            ptr->particle_lifetime(0.8f);
            ptr->emmiter_lifetime(0.8f);
            ptr->spawn_radius(2.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(5.0f);
            ptr->fade(true);
            Add(ptr);
            break;
        case blood:
            ptr->shape(ParticleShape::rectangle);
            ptr->size(1.0f, 3.0f);
            ptr->speed(5.0f);
            ptr->spread(10.0f);
            ptr->color(RED);
            ptr->howmany(50);
            ptr->gravity(45.0f);
            ptr->particle_lifetime(0.8f);
            ptr->emmiter_lifetime(0.8f);
            ptr->fade(true);
            ptr->spawn_radius(4.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(5.0f);
            Add(ptr);
            break;
        case ambient_particles:
            ptr->shape(ParticleShape::rectangle);
            ptr->size(2.0f, 4.0f);
            ptr->speed(2.0f);
            ptr->spread(8.0f);
            ptr->color({ 170,255,255,60 });
            ptr->howmany(25);
            ptr->gravity(1.0f);
            ptr->set_forever(true);
            ptr->particle_lifetime(7.0f);
            //ptr->emmiter_lifetime(0.8f);
            ptr->fade_in_out(true);
            ptr->spawn_radius(200.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(0.05f);
            ptr->emittion_counter(0.5f);
            ptr->direction_vector({ -0.4f, -0.2f });
            Add(ptr);
            break;
        case ambient_particles_foreground:
            ptr->shape(ParticleShape::texture);
            ptr->texture(*TextureLoader::GetTexture("DUST_TEST"));
            ptr->random_texture_rect();
            ptr->size(10.0f, 150.0f);
            ptr->speed(0.1f);
            ptr->spread(3.0f);
            ptr->color({ 120,200,200,50 });
            ptr->howmany(8);
            ptr->gravity(1.1f);
            ptr->set_forever(true);
            ptr->particle_lifetime(10.0f);
            ptr->fade_in_out(true);
            ptr->spawn_radius(500.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(0.005f);
            ptr->emittion_counter(0.5f);
            ptr->direction_vector({ -0.1f, -0.1f });
            ptr->paralax_offset(0.55f);
            Add(ptr);
            break;
        case acid_bursts:
            ptr->shape(ParticleShape::circle_lines); 
            ptr->size(1.0f, 5.0f);
            ptr->speed(2.0f);
            ptr->color({ 0,200,40,80 });
            ptr->howmany(6);
            ptr->gravity(3.0f);
            ptr->set_forever(true);
            ptr->particle_lifetime(1.5f);
            ptr->spawn_radius(8.0f);
            ptr->emittion_counter(0.3f);
            Add(ptr);
            break;
        case acid_head_burst:
            ptr->shape(ParticleShape::circle);
            ptr->speed(5.0f);
            ptr->spread(3.0f);
            ptr->gravity(4.0f);
            ptr->particle_lifetime(1.8f);
            ptr->emmiter_lifetime(1.8f);
            ptr->size(1.0f, 5.0f);
            ptr->color({ 0,200,40,255 });
            ptr->howmany(30);
            ptr->spawn_radius(4.0f);
            Add(ptr);
            break;
        case acid_projectile:
            ptr->shape(ParticleShape::circle);
            ptr->speed(5.0f);
            ptr->spread(3.0f);
            ptr->gravity(4.0f);
            ptr->particle_lifetime(1.8f);
            ptr->emmiter_lifetime(1.8f);
            ptr->size(1.0f, 5.0f);
            ptr->color({ 0,200,40,255 });
            ptr->howmany(3);
            ptr->spawn_radius(4.0f);
            Add(ptr);
            break;
        case acid_explosion:
            ptr->shape(ParticleShape::circle);
            ptr->speed(8.0f);
            ptr->spread(6.0f);
            ptr->gravity(3.0f);
            ptr->particle_lifetime(2.2f);
            ptr->emmiter_lifetime(2.2f);
            ptr->size(2.0f, 6.0f);
            ptr->color({ 0,200,40,255 });
            ptr->howmany(50);
            ptr->spawn_radius(6.0f);
            Add(ptr);
            break;
        case white_burst:
            ptr->shape(ParticleShape::rectangle);
            ptr->speed(20.0f);
            ptr->spread(20.0f);
            ptr->gravity(0.0f);
            ptr->particle_lifetime(0.4f);
            ptr->emmiter_lifetime(0.4f);
            ptr->size(4.0f, 4.0f);
            ptr->color(Fade(RAYWHITE, 0.7f));
            ptr->howmany(100);
            ptr->spawn_radius(1.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(0.6f);
            Add(ptr);
            break;
        case glass_explosion:
            ptr->shape(ParticleShape::texture);
            ptr->texture(*TextureLoader::GetTexture("GLASS"));
            ptr->speed(8.0f);
            ptr->spread(7.0f);
            ptr->gravity(6.0f);
            ptr->particle_lifetime(2.5f);
            ptr->emmiter_lifetime(2.5f);
            ptr->size(1.0f, 32.0f);
            ptr->color(Fade(WHITE, 0.8f));
            ptr->howmany(200);
            ptr->spawn_radius(32.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(0.1f);
            ptr->random_texture_rect();
            Add(ptr);
            break;
        case energy_orb:
            ptr->shape(ParticleShape::texture);
            ptr->texture(*TextureLoader::GetTexture("ORBS"));
            ptr->speed(2.0f);
            ptr->spread(2.0f);
            ptr->gravity(4.0f);
            ptr->particle_lifetime(1.0f);
            ptr->emmiter_lifetime(1.0f);
            ptr->size(3.0f, 5.0f);
            ptr->color(WHITE);
            ptr->howmany(1);
            ptr->spawn_radius(1.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(0.5f);
            ptr->animated();
            ptr->random_texture_rect();
            ptr->fade(true);
            ptr->fade_in(false);
            ptr->fade_in_out(false);
            Add(ptr);
            break;
        case breath_proj:
            ptr->shape(ParticleShape::rectangle);
            ptr->speed(4.0f);
            ptr->spread(4.0f);
            ptr->gravity(5.0f);
            ptr->particle_lifetime(2.5f);
            ptr->emmiter_lifetime(2.5f);
            ptr->size(1.0f, 4.0f);
            ptr->color({ 52,28,39,255 });
            ptr->howmany(10);
            ptr->spawn_radius(3.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(5.0f);
            Add(ptr);
            break;
        case breath_small_anticipation:
            ptr->shape(ParticleShape::rectangle);
            ptr->speed(4.0f);
            ptr->spread(4.0f);
            ptr->gravity(5.0f);
            ptr->particle_lifetime(2.5f);
            ptr->emmiter_lifetime(2.5f);
            ptr->size(1.0f, 4.0f);
            ptr->color({ 52,28,39,255 });
            ptr->howmany(1);
            ptr->spawn_radius(3.0f);
            ptr->random_rotation(true);
            ptr->rotation_speed(5.0f);
            Add(ptr);
            break;
        case dash_shadow:
            ptr->shape(ParticleShape::texture);
            ptr->texture(*TextureLoader::GetTexture("DASH"));
            ptr->speed(0.0f);
            ptr->spread(0.0f);
            ptr->gravity(8.0f);
            ptr->particle_lifetime(0.6f);
            ptr->emmiter_lifetime(0.6f);
            ptr->size(32.0f, 32.0f);
            ptr->color(Fade(WHITE, 0.8f));
            ptr->howmany(1);
            ptr->spawn_radius(0.0f);
            ptr->fade(true);
            ptr->fade_in(false);
            ptr->fade_in_out(false);
            Add(ptr);
            break;
        case dash_shadow_left:
            ptr->shape(ParticleShape::texture);
            ptr->texture(*TextureLoader::GetTexture("DASHL"));
            ptr->speed(0.0f);
            ptr->spread(0.0f);
            ptr->gravity(8.0f);
            ptr->particle_lifetime(0.6f);
            ptr->emmiter_lifetime(0.6f);
            ptr->size(32.0f, 32.0f);
            ptr->color(Fade(WHITE, 0.8f));
            ptr->howmany(1);
            ptr->spawn_radius(0.0f);
            ptr->fade(true);
            ptr->fade_in(false);
            ptr->fade_in_out(false);
            Add(ptr);
            break;
        }

    }

    static void UpdateTextureSize(int x, int y)
    {
        UnloadRenderTexture(particlesRenderT);
        particlesRenderT = LoadRenderTexture(x, y);
    }

    static std::vector<ParticleEmitter*> emitters;
};

#endif