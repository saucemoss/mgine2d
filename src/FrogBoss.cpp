#include "FrogBoss.h"
#include "SoundManager.h"
#include "GameScreen.h"
#include "Leggy.h"
#include "BioBomb.h"
#include "BossHook.h"

FrogBoss::FrogBoss(const Rectangle& rectangle) : Enemy({ rectangle.x, rectangle.y, 40, 40 }, FBOSS, b2_dynamicBody)
{
	InitAnimations();
	m_max_hp = 700;
	m_current_hp = m_max_hp;
	//Physics body cfg
	//add more mass 
	sprite_offset_192 = { -83 , -106 };
	sprite_offset_96 = { -26 , -51 };
	sprite_offset_32 = { -10 ,-2 };

	FixtureUserData* data = new FixtureUserData;
	data->tag = ENEMY_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
	m_fixture->SetDensity(100.0f);
	m_body->ResetMassData();
	m_body->SetBullet(true);
	//feet sensor
	m_feet_sensor = util::SimpleSensor(m_body, "fboss_feet", 1.0f, 0.3f, 0, 1.1f);

	m_body->SetLinearDamping(linear_dumping);
	standing_sensor = util::SimpleSensor(m_body, "fb_stand_body", 1.0f, 2.0f, 0.0f, -1.0f);
	speed = 8.0f;
	
	boss_state = FrogBossState::Roar;

	StatesStrMap[FrogBossState::AttackClose] = "AttackClose";
	StatesStrMap[FrogBossState::Decide] = "Decide";
	StatesStrMap[FrogBossState::Dying] = "Dying";
	StatesStrMap[FrogBossState::GroundPound] = "GroundPound";
	StatesStrMap[FrogBossState::Jump] = "Jump";
	StatesStrMap[FrogBossState::RangedHook] = "RangedHook";
	StatesStrMap[FrogBossState::Roar] = "Roar";
	StatesStrMap[FrogBossState::Run] = "Run";
	StatesStrMap[FrogBossState::SpawnBalls] = "SpawnBalls";

}

FrogBoss::~FrogBoss()
{
	EnitityManager::Remove(this);
}

void FrogBoss::Update(float dt)
{
	SwitchFrames(dt);
	dmg_counter += dt;
	if (dmg_counter >= 0.6f)
	{
		taking_dmg = false;
	}

	CheckTouchGround();

	player_pos = GameScreen::player->center_pos();
	LookingRight(player_pos);

	switch (boss_state)
	{
	case FrogBossState::Roar:
		UpdateRoar(dt);
		break;
	case FrogBossState::Run:
		UpdateRun(dt);
		break;
	case FrogBossState::AttackClose:
		UpdateAttack(dt);
		break;
	case FrogBossState::AttackCloseAnticipation:
		UpdateAnticipationCloseAttack(dt);
		break;
	case FrogBossState::GroundPound:
		UpdatePound(dt);
		break;
	case FrogBossState::SwirlAttackAnticipation:
		UpdateSwirlAttackAnticipation(dt);
		break;
	case FrogBossState::SwirlAttack:
		UpdateSwirlAttack(dt);
		break;
	case FrogBossState::HookAnticipation:
		UpdateAnticipationHookAttack(dt);
		break;
	case FrogBossState::RangedHook:
		UpdateHook(dt);
		break;
	case FrogBossState::Jump:
		UpdateJump(dt);
		break;
	case FrogBossState::SpawnBalls:
		UpdateBalls(dt);
		break;
	case FrogBossState::Dying:
		UpdateDie(dt);
		break;
	case FrogBossState::Decide:
		UpdateDecide(dt);
		break;
	}


}

void FrogBoss::InitAnimations()
{
	animations->InitializeBoss1Animations();
	SetAnimation("FB_ROAR");
}

void FrogBoss::Die(int death_option)
{

	SetAnimation("FB_ROAR");
	PlaySound(SoundManager::sounds["hurt4"]);
	PlaySound(SoundManager::sounds["slime2short"]);
	boss_state = FrogBossState::Dying;
}

void FrogBoss::TakeDmg(int dmg)
{
	if (!taking_dmg)
	{
		m_current_hp -= dmg;
		taking_dmg = true;
		dmg_counter = 0.0f;
		std::string dmgs[] = { "hit2","hit3","hit4" };
		SoundManager::PlayRandSounds(dmgs, 3);
		bleed_particles();
		if (m_current_hp <= 0)
		{

			Die(1);
		}
		else
		{
			PlaySound(SoundManager::sounds["hurt3"]);
		}
	}

}


void FrogBoss::UpdateRoar(float dt)
{
	if(AnimationEnded())
	boss_state = FrogBossState::Decide;
}

void FrogBoss::UpdateRun(float dt)
{
	ParticleEmitter* p = new ParticleEmitter({ pos().x + (looking_right? -32 : 32), pos().y + 16 });
	GameScreen::Particles->Add(DefinedEmitter::dust, p);
	p->EmitParticles();
	
	ParticleEmitter* p2 = new ParticleEmitter({ pos().x + (looking_right ? 16 : -16), pos().y + 16 });
	GameScreen::Particles->Add(DefinedEmitter::dust, p2);
	p2->EmitParticles();

	ParticleEmitter* p3 = new ParticleEmitter({ center_pos().x + (looking_right ? 50 : -20), center_pos().y + 32 });
	GameScreen::Particles->Add(DefinedEmitter::acid_projectile, p3);
	p3->EmitParticles();

	ParticleEmitter* p4 = new ParticleEmitter({ center_pos().x + (looking_right ? 50 : -20), center_pos().y });
	GameScreen::Particles->Add(DefinedEmitter::acid_projectile, p4);
	p4->EmitParticles();


	if (looking_right != side_looking_at_att_start)
	{
		m_body->DestroyFixture(run_att_sensor);
		RunSensor();
		side_looking_at_att_start = looking_right;
		changed_sides = false;
	}

	set_velocity_x(looking_right? speed : -speed);


	attack_counter += dt;
	if (attack_counter > 5.0f)
	{
		attack_counter = 0.0f;
		m_body->DestroyFixture(run_att_sensor);
		m_body->DestroyFixture(running_sensor);
		standing_sensor = util::SimpleSensor(m_body, "fb_stand_body", 1.0f, 2.0f, 0.0f, -1.0f);
		boss_state = FrogBossState::Jump;
		player_attacked = false;
	}

	if (player_in_dmg_zone)
	{
		DmgPlayer();
		attack_counter = 0.0f;
		m_body->DestroyFixture(run_att_sensor);
		m_body->DestroyFixture(running_sensor);
		standing_sensor = util::SimpleSensor(m_body, "fb_stand_body", 1.0f, 2.0f, 0.0f, -1.0f);
		boss_state = FrogBossState::Jump;
		player_attacked = false;
	}

	
}

void FrogBoss::UpdateJump(float dt)
{
	if (!jumped)
	{
		if (!looking_right)
		{
			m_body->ApplyForce(b2Vec2(GetRandomValue(0, 10) * m_body->GetMass() * 100.0f, -world->GetGravity().y * m_body->GetMass() * 10.0f), m_body->GetWorldCenter(), true);

		}
		else
		{
			m_body->ApplyForce(b2Vec2(GetRandomValue(-10, 0) * m_body->GetMass() * 100.0f, -world->GetGravity().y * m_body->GetMass() * 10.0f), m_body->GetWorldCenter(), true);

		}
		jumped = true;
	}
	if (is_touching_floor)
	{
		boss_state = FrogBossState::Decide;
		jumped = false;
	}
	else if (GameScreen::player->state == PlayerState::Hurting)
	{
		if (!looking_right)
		{
			m_body->ApplyForce(b2Vec2(GetRandomValue(0, 10) * m_body->GetMass() * 100.0f, -world->GetGravity().y * m_body->GetMass() * 10.0f), m_body->GetWorldCenter(), true);

		}
		else
		{
			m_body->ApplyForce(b2Vec2(GetRandomValue(-10, 0) * m_body->GetMass() * 100.0f, -world->GetGravity().y * m_body->GetMass() * 10.0f), m_body->GetWorldCenter(), true);

		}
		jumped = true;
	}

}

void FrogBoss::UpdateAnticipationCloseAttack(float dt)
{
	if (AnimationEnded())
	{
		close_att_sensor = util::SimpleSensor(m_body, "fb_close_att", 2.8f, 2.5f, (looking_right ? 0.2f : -0.2f));
		SetAnimation("FB_ATT1");
		boss_state = FrogBossState::AttackClose;
	}
}

void FrogBoss::UpdateAttack(float dt)
{

	if (animation->GetCurrentFrameNum() > 1 && player_in_dmg_zone)
	{
		DmgPlayer();
	}

	int fn = animation->GetCurrentFrameNum();

	ParticleEmitter* p = new ParticleEmitter({ pos().x + (looking_right ? 32.0f + fn * 3 : -32.0f - fn * 3), center_pos().y - fn * 5});
	GameScreen::Particles->Add(DefinedEmitter::dust, p);
	p->EmitParticles();

	looking_right = side_looking_at_att_start;

	attack_counter += dt;
	if (attack_counter >= random_timer)
	{
		boss_state = FrogBossState::Jump;
		m_body->DestroyFixture(close_att_sensor);
		attack_counter = 0.0f;
		player_attacked = false;
	}

	if (player_attacked)
	{
		boss_state = FrogBossState::Decide;
		m_body->DestroyFixture(close_att_sensor);
		attack_counter = 0.0f;
		player_attacked = false;
	}
	
}


void FrogBoss::UpdateAnticipationHookAttack(float dt)
{
	if (AnimationEnded())
	{
		SetAnimation("FB_SHOOT");
		boss_state = FrogBossState::RangedHook;
		Rectangle rect = Rectangle{ center_pos().x, center_pos().y + 16.0f, 30, 10 };
		LevelManager::level_entities_safe.push_back(std::make_unique<BossHook>(rect, looking_right));
	}
}

void FrogBoss::UpdateSwirlAttackAnticipation(float dt)
{
	looking_right = side_looking_at_att_start;
	if (AnimationEnded())
	{
		swirl_att_sensor = util::SimpleSensor(m_body, "fb_swirl_att", 0.4f, 2.2f, (looking_right ? 2.8f : -2.8f), -0.5f);
		SetAnimation("FB_SW_ATT");
		boss_state = FrogBossState::SwirlAttack;

	}
}

void FrogBoss::UpdateSwirlAttack(float dt)
{
	if (player_in_dmg_zone)
	{
		DmgPlayer();
	}

	looking_right = side_looking_at_att_start;

	for (int i = 0; i < 8; i++)
	{
		ParticleEmitter* p = new ParticleEmitter({ pos().x + (looking_right ? 36.0f : -36.0f), center_pos().y -20.0f + i * 8.0f });
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
	}
	set_velocity_x(looking_right ? speed/3 : -speed/3);

	attack_counter += dt;
	if (attack_counter >= random_timer)
	{
		boss_state = FrogBossState::Jump;
		m_body->DestroyFixture(swirl_att_sensor);
		attack_counter = 0.0f;
		player_attacked = false;
	}

	if (player_attacked)
	{
		boss_state = FrogBossState::Decide;
		m_body->DestroyFixture(swirl_att_sensor);
		attack_counter = 0.0f;
		player_attacked = false;
	}

}

void FrogBoss::UpdateHook(float dt)
{

	if (AnimationEnded())
	{
		boss_state = FrogBossState::Decide;
	}

}

void FrogBoss::UpdatePound(float dt)
{
	if (animation->GetCurrentFrameNum() == 7 && !pound)
	{
		pound = true;
		Rectangle rect;
		Rectangle rect2;
		
		GameScreen::add_trauma(0.8f);
		for (int i = 0; i < 3; i++)
		{
			rect = Rectangle{ center_pos().x + i * 32.0f, center_pos().y + 40.0f  , 16, 16 };
			LevelManager::level_entities_safe.push_back(std::make_unique<BioBomb>(rect));
			rect2 = Rectangle{ center_pos().x - i * 32.0f, center_pos().y + 40.0f  , 16, 16 };
			LevelManager::level_entities_safe.push_back(std::make_unique<BioBomb>(rect2));
		}
	}

	if (AnimationEnded())
	{
		boss_state = FrogBossState::Decide;
		pound = false;
	}
}

void FrogBoss::UpdateBalls(float dt)
{
	attack_counter += dt;
	if (attack_counter > 1.0f)
	{
		PlaySound(SoundManager::sounds["splat4"]);

		Rectangle rect = Rectangle{ center_pos().x + (looking_right ? -20.0f : 20.0f), center_pos().y - 32.0f  , 16, 16 };
		LevelManager::level_entities_safe.push_back(std::make_unique<BioBomb>(rect));
		BioBomb* bomb = reinterpret_cast<BioBomb*>(LevelManager::level_entities_safe.back().get());

		ParticleEmitter* p = new ParticleEmitter({ center_pos().x + (looking_right ? -20.0f : 20.0f), center_pos().y - 32.0f });
		GameScreen::Particles->Add(DefinedEmitter::acid_head_burst, p);
		p->EmitParticles();
		attack_counter = 0.0f;
	}
	random_timer -= dt;
	if(random_timer <= 0.0f) boss_state = FrogBossState::Decide;
}

void FrogBoss::UpdateDie(float dt)
{
	taking_dmg = false;

	Vector2 particle_rand_pos{ static_cast<float>(GetRandomValue(pos().x - 32.0f, pos().x + 32.0f)),
									static_cast<float>(GetRandomValue(pos().y - 32.0f, pos().y + 32.0f)) };

	ParticleEmitter* p = new ParticleEmitter(particle_rand_pos);
	GameScreen::Particles->Add(DefinedEmitter::acid_bursts, p);
	p->EmitParticles();

	Vector2 particle_rand_pos2{ static_cast<float>(GetRandomValue(pos().x - 32.0f, pos().x + 32.0f)),
									static_cast<float>(GetRandomValue(pos().y - 32.0f, pos().y + 32.0f)) };

	ParticleEmitter* p2 = new ParticleEmitter(particle_rand_pos2);
	GameScreen::Particles->Add(DefinedEmitter::blood, p2);
	p2->EmitParticles();

	Vector2 particle_rand_pos4{ static_cast<float>(GetRandomValue(pos().x - 32.0f, pos().x + 32.0f)),
								static_cast<float>(GetRandomValue(pos().y - 32.0f, pos().y + 32.0f)) };

	ParticleEmitter* p4 = new ParticleEmitter(particle_rand_pos4);
	GameScreen::Particles->Add(DefinedEmitter::dust, p4);
	p4->EmitParticles();

	if (is_touching_floor)
	{
		m_body->SetEnabled(false);

	}
	if (AnimationEnded())
	{
		m_destroy = true;
		Vector2 particle_rand_pos3;
		for (int i = 0; i < 15; i++)
		{
			particle_rand_pos3 = {	static_cast<float>(GetRandomValue(pos().x - 64.0f, pos().x + 64.0f)),
									static_cast<float>(GetRandomValue(pos().y - 64.0f, pos().y + 64.0f)) };

			ParticleEmitter* p3 = new ParticleEmitter(particle_rand_pos3);
			GameScreen::Particles->Add(DefinedEmitter::acid_explosion, p3); 
			p3->EmitParticles();
		}

	}
}

void FrogBoss::UpdateDecide(float dt)
{
	SelectAction();
	random_timer = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (4.0f)));
}

void FrogBoss::SelectAction()
{


	float dist = Vector2Distance(pos(), player_pos);
	(dist > 130.0f) ? player_far = true : player_far = false;
	(dist > 48.0f && dist < 130.0f) ? player_mid = true : player_mid = false;
	(dist < 48.0f) ? player_close = true : player_close = false;
	

	std::string out = "";
	player_far ? out += "Player Far: " + std::to_string(dist) : out += "";
	player_mid ? out += "Player Mid: " + std::to_string(dist) : out += "";
	player_close ? out += "Player Close: " + std::to_string(dist) : out += "";

	//std::cout << out << std::endl;

	int rand = GetRandomValue(1, 5);
	if (player_far)
	{
		switch (rand)
		{
		case 1: case 2:
			SetRun();
			break;
		case 3: case 4:
			boss_state = FrogBossState::HookAnticipation;
			SetAnimation("FB_SH_ANT");
			break;
		case 5:
			boss_state = FrogBossState::SpawnBalls;
			SetAnimation("FB_SPAWN");
			break;
		}
	}
	else if (player_mid)
	{
		switch (rand)
		{
		case 1: 
			boss_state = FrogBossState::Jump;
			SetAnimation("FB_ROAR");
			break;
		case 2:
			boss_state = FrogBossState::SwirlAttackAnticipation;
			SetAnimation("FB_SW_ANT");
			side_looking_at_att_start = looking_right;
			break;
		case 3:
			boss_state = FrogBossState::SpawnBalls;
			SetAnimation("FB_SPAWN");
			break;
		case 4:
			boss_state = FrogBossState::GroundPound;
			SetAnimation("FB_POUND");
			side_looking_at_att_start = looking_right;
			break;
		case 5:
			SetRun();
			break;
		}
	}
	else if (player_close)
	{
		switch (rand)
		{
		case 1: 
			boss_state = FrogBossState::AttackCloseAnticipation;
			PlayFromFrame(1,"FB_ANT");
			side_looking_at_att_start = looking_right;
			break;
		case 2: case 3:
			boss_state = FrogBossState::SwirlAttackAnticipation;
			SetAnimation("FB_SW_ANT");
			side_looking_at_att_start = looking_right;
			break;
		case 4:
			boss_state = FrogBossState::GroundPound;
			SetAnimation("FB_POUND");
			side_looking_at_att_start = looking_right;
			break;
		case 5:
			boss_state = FrogBossState::Jump;
			SetAnimation("FB_ROAR");
			break;
		}
	}
}

void FrogBoss::SetRun()
{
	SetAnimation("FB_RUN");
	running_sensor = util::SimpleSensor(m_body, "fb_run_body", 2.0f, 1.5f, 0.0f);
	side_looking_at_att_start = looking_right;
	m_body->DestroyFixture(standing_sensor);
	RunSensor();
	boss_state = FrogBossState::Run;
}

void FrogBoss::RunSensor()
{
	run_att_sensor = util::SimpleCircleSensor(m_body, "fb_run_att", 22.0f, (looking_right ? 22.0f : -22.0f), -10.0f);
}

void FrogBoss::LookingRight(Vector2 player_pos)
{
	if (boss_state != FrogBossState::Run && boss_state != FrogBossState::Decide)
	{
		if (player_pos.x > center_pos().x)
		{
			looking_right = true;
		}
		else if (player_pos.x < center_pos().x)
		{
			looking_right = false;
		}
	}

}

void FrogBoss::DmgPlayer()
{
	player_attacked = true;
	GameScreen::player->take_dmg(10);
	GameScreen::add_trauma(1.0f);
	PlaySound(SoundManager::sounds["hit1"]);
	ParticleEmitter* p = new ParticleEmitter(GameScreen::player->pos());
	GameScreen::Particles->Add(DefinedEmitter::dust, p);
	p->EmitParticles();
	
}


//unused
void FrogBoss::UpdateIdleState(float dt)
{
}

void FrogBoss::UpdateRunningState(float dt)
{
}

void FrogBoss::UpdateAttackingState(float dt)
{
}

void FrogBoss::UpdateHurtingState(float dt)
{
}

void FrogBoss::UpdateDyingState(float dt)
{
}
