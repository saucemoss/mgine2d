#include "BossGlass.h"
#include "GameScreen.h"
#include "Ribbs.h"
#include "FrogBoss.h"

BossGlass::BossGlass(const Rectangle& rectangle)
	:
	Collidable({rectangle.x, rectangle.y - 320.0f, rectangle.width, 640.0f}, b2_staticBody, BOSSGLASS)
{
	InitAnimations();
	state = BossGlassState::Idle;
	m_fixture->SetSensor(true);
	EnitityManager::Add(this);
	delay_timer = 2.5f;
	
}

BossGlass::~BossGlass()
{
	EnitityManager::Remove(this);

}

void BossGlass::Update(float dt)
{
	SwitchFrames(dt);

	int anim_num = animation->GetCurrentFrameNum();

	switch (state)
	{
	case BossGlassState::Idle:
		if (player_entered_sensor)
		{
			if (level_unlocked)
			{
				for (Entity* e : EnitityManager::EntityList)
				{
					Door* d = dynamic_cast<Door*>(e);
					if (d != nullptr)
					{
						d->locked = true;
						PlaySound(SoundManager::sounds["robo5"]);
						GameScreen::add_trauma(1.0f);
						ParticleEmitter* p = new ParticleEmitter({ d->pos().x, d->pos().y - 20 });
						GameScreen::Particles->Add(DefinedEmitter::dust, p);
						p->EmitParticles();
						ParticleEmitter* p2 = new ParticleEmitter({ d->pos().x, d->pos().y + 20 });
						GameScreen::Particles->Add(DefinedEmitter::dust, p2);
						p2->EmitParticles();
					}
				}
				for (auto& l : GameScreen::LevelMgr->lights->m_lights)
				{
					l.SetColor({ 255, 0, 0, 255 });
				}
				level_unlocked = false;
			}

			delay_timer -= dt;
			if (delay_timer <= 0.0f)
			{
				state = BossGlassState::Knocking;
				SetAnimation("SHATTER");
				delay_timer = 2.5f;

			}


		}
		break;
	case BossGlassState::Knocking:

		if (anim_num == 2)
		{
			GameScreen::add_trauma(0.4f);
			PlaySound(SoundManager::sounds["glass_shatter"]);
			PlaySound(SoundManager::sounds["glass_thud"]);
		}
		if (anim_num == 4)
		{
			GameScreen::add_trauma(0.6f);
			PlaySound(SoundManager::sounds["glass_shatter"]);
			PlaySound(SoundManager::sounds["glass_thud2"]);
		}
		if (anim_num == 6)
		{
			GameScreen::add_trauma(0.8f);
			PlaySound(SoundManager::sounds["glass_shatter"]);
			PlaySound(SoundManager::sounds["glass_thud"]);
			PlaySound(SoundManager::sounds["glass_thud2"]);
		}
		if (anim_num == 9)
		{
			GameScreen::add_trauma(1.0f);
			PlaySound(SoundManager::sounds["glass_shatter"]);
			PlaySound(SoundManager::sounds["glass_explosion"]);
			PlaySound(SoundManager::sounds["glass_thud"]);
			PlaySound(SoundManager::sounds["glass_thud2"]);

			ParticleEmitter* p = new ParticleEmitter({ pos().x + 64.0f, pos().y});
			GameScreen::Particles->Add(DefinedEmitter::glass_explosion, p);
			p->EmitParticles();
			state = BossGlassState::Shattered;

			if (!boss_added)
			{
				Rectangle rect = { pos().x, pos().y, 128, 128 };
				GameScreen::LevelMgr->level_entities_safe.push_back(std::make_unique<FrogBoss>(rect));
				GameScreen::LevelMgr->level_entities_safe.back().get()->m_draw_layers = 1;
				boss_added = true;
			}


		}
		break;
	case BossGlassState::Shattered:
		FreezeFrame("SHATTER", 10);

		
		if (!level_unlocked)
		{
			bool lock_doors = false;
			for (Entity* e : EnitityManager::EntityList)
			{
				FrogBoss* fb = dynamic_cast<FrogBoss*>(e);
				if (fb != nullptr)
				{
					lock_doors = true;
				}
			}

			if (!lock_doors)
			{
				level_unlocked = true;
				for (Entity* e : EnitityManager::EntityList)
				{
					Door* d = dynamic_cast<Door*>(e);
					if (d != nullptr)
					{
						d->locked = false;
						PlaySound(SoundManager::sounds["robo3"]);
						d->FreezeFrame("D_CLOSE", 7);
						d->state = DoorState::Closed;
						ParticleEmitter* p = new ParticleEmitter({ d->pos().x, d->pos().y - 20 });
						GameScreen::Particles->Add(DefinedEmitter::dust, p);
						p->EmitParticles();
						ParticleEmitter* p2 = new ParticleEmitter({ d->pos().x, d->pos().y + 20 });
						GameScreen::Particles->Add(DefinedEmitter::dust, p2);
						p2->EmitParticles();
					}
				}
				for (auto& l : GameScreen::LevelMgr->lights->m_lights)
				{
					l.SetColor({ 0, 255, 0, 255 });
				}
			}

		}
		break;
	}
}

void BossGlass::Draw(int l)
{
	auto spritePosX = center_pos().x - 64.0f;
	auto spritePosY = center_pos().y + 256.0f;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		WHITE);

}

void BossGlass::InitAnimations()
{
	sprite = TextureLoader::GetTexture("B_GLASS");
	animations->InitializeBossGlassAnimations();
	FreezeFrame("SHATTER", 1);
}
