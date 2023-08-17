#include "Terminal.h"
#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"


Terminal::Terminal(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_kinematicBody, TERMNIAL)
{
	InitAnimations();
	EnitityManager::Add(this);
	m_fixture->SetSensor(true);
	state = TerminalState::Idle;
}

Terminal::~Terminal()
{
	EnitityManager::Remove(this);
}

void Terminal::Draw(int l)
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

void Terminal::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor && state != TerminalState::Pass)
	{
		auto spritePosX = center_pos().x ;
		auto spritePosY = center_pos().y ;
		Rectangle source = { spritePosX,spritePosY,settings::tileSize,settings::tileSize };
		GameScreen::shaders->ApplyOutline(*sprite, CurrentFrame(), source, { 0,0 }, 0.0f);
	}

	switch (state)
	{
	case TerminalState::Idle:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)))
		{
			SetAnimation("TERM_PASS");
			state = TerminalState::Pass;
			PlaySound(SoundManager::sounds["call_sw"]);
			PlaySound(SoundManager::sounds["pc_work"]);
		}
		break;
	case TerminalState::Pass:
		if (AnimationEnded())
		{
			FreezeFrame("TERM_PASS", 14);
			StopSound(SoundManager::sounds["pc_work"]);
		}
		break;
	}

}

void Terminal::InitAnimations()
{
	sprite = TextureLoader::GetTexture("TERMINAL");
	animations->InitializeTerminalAnimations();
	SetAnimation("TERM_IDLE");
}
