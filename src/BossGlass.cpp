#include "BossGlass.h"
#include "GameScreen.h"
#include "Ribbs.h"

BossGlass::BossGlass(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_staticBody, BOSSGLASS)
{
	InitAnimations();
	state = BossGlassState::Idle;
	m_fixture->SetSensor(true);
	EnitityManager::Add(this);

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
			state = BossGlassState::Knocking;
			SetAnimation("SHATTER");
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

			ParticleEmitter* p = new ParticleEmitter(pos());
			GameScreen::Particles->Add(DefinedEmitter::glass_explosion, p);
			p->EmitParticles();
			state = BossGlassState::Shattered;

			//TODO:instantiate boss
			if (!boss_added)
			{
				Rectangle rect = { pos().x, pos().y, 14, 30 };
				GameScreen::LevelMgr->level_entities_safe.push_back(std::make_unique<Ribbs>(rect));
				GameScreen::LevelMgr->level_entities_safe.back().get()->m_draw_layers = 1;
				boss_added = true;
			}


		}
		break;
	case BossGlassState::Shattered:
		FreezeFrame("SHATTER", 10);
		break;
	}
}

void BossGlass::Draw(int l)
{
	auto spritePosX = center_pos().x - 64.0f;
	auto spritePosY = center_pos().y - 64.0f;

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
