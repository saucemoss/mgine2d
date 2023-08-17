#include "AxePickup.h"
#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"
#include "Dialogue.h"


AxePickup::AxePickup(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_kinematicBody, AXEPICKUP)
{
	InitAnimations();
	EnitityManager::Add(this);
	m_fixture->SetSensor(true);
	state = AxePickupState::unused;
}

AxePickup::~AxePickup()
{
	EnitityManager::Remove(this);
}

void AxePickup::Draw(int l)
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

void AxePickup::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor && state != AxePickupState::used)
	{
		auto spritePosX = center_pos().x;
		auto spritePosY = center_pos().y;
		Rectangle source = { spritePosX,spritePosY,settings::tileSize,settings::tileSize };
		GameScreen::shaders->ApplyOutline(*sprite, CurrentFrame(), source, { 0,0 }, 0.0f);
		if (!DialogueManager::DialogueExhausted(8))
		{
			DialogueManager::StartDialogue(8, center_pos());
		}
	}

	switch (state)
	{
	case AxePickupState::unused:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)))
		{
			SetAnimation("USED");
			state = AxePickupState::used;
			PlaySound(SoundManager::sounds["glass_shatter"]);
			ParticleEmitter* p = new ParticleEmitter(pos());
			GameScreen::Particles->Add(DefinedEmitter::dust, p);
			p->EmitParticles();p->EmitParticles();
			GameScreen::player->m_has_axe = true;
			GameScreen::add_trauma(0.9f);
		}
		break;
	case AxePickupState::used:
		if (!DialogueManager::DialogueExhausted(9))
		{
			DialogueManager::StartDialogue(9, center_pos());
		}
		break;
	}

}

void AxePickup::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeAxePickupAnimations();
	SetAnimation("UNUSED");
}
