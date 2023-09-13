#include "ContactListener.h"
#include "Collidable.h"
#include "Player.h"
#include "GameScreen.h"
#include "InfectedHazmat.h"
#include "Elevator.h"
#include "FlyingInfected.h"
#include "Ribbs.h"
#include "Leggy.h"
#include "Football.h"
#include "HeadSpit.h"
#include "NPCSecurityGuy.h"
#include "WoodCrate.h"
#include "WhiteCoatArm.h"
#include "Terminal.h"
#include "AxePickup.h"
#include "Gate.h"
#include "BioBomb.h"
#include "BossGlass.h"
#include "Switch.h"
#include "FrogBoss.h"
#include "BossHook.h"
#include "MediPod.h"
#include "SecretFog.h"
#include "NPCDoc1.h"
#include "Ivan.h"
#include "Rollo.h"

std::map<ColliderTag, std::string> ContactListener::ColStrMap;


ContactListener::ContactListener()
{
	ColStrMap[ColliderTag::UNASSIGNED] = "UNASSIGNED";
	ColStrMap[ColliderTag::SOLID] = "SOLID";
	ColStrMap[ColliderTag::PLAYER] = "PLAYER";
	ColStrMap[ColliderTag::LEVEL_PORTAL] = "LEVEL_PORTAL";
	ColStrMap[ColliderTag::DOOR] = "DOOR";
	ColStrMap[ColliderTag::ELEVATOR] = "ELEVATOR";
	ColStrMap[ColliderTag::ELEVATOR_CALL_SW] = "ELEVATOR_CALL_SW";
	ColStrMap[ColliderTag::M_BLOCK] = "M_BLOCK";
	ColStrMap[ColliderTag::W_CRATE] = "W_CRATE";
	ColStrMap[ColliderTag::INFECTED_H] = "INFECTED_H";
	ColStrMap[ColliderTag::FIREAXE] = "FIREAXE";
	ColStrMap[ColliderTag::FLYING_INF] = "FLYING_INF";
	ColStrMap[ColliderTag::RIBBS] = "RIBBS";
	ColStrMap[ColliderTag::LEGGY] = "LEGGY";
	ColStrMap[ColliderTag::FOOTB] = "FOOTB";
	ColStrMap[ColliderTag::HSPIT] = "HSPIT";
	ColStrMap[ColliderTag::NPC] = "NPC";
	ColStrMap[ColliderTag::WCARM] = "WCARM";
	ColStrMap[ColliderTag::SHARD] = "SHARD";
	ColStrMap[ColliderTag::ACID] = "ACID";
	ColStrMap[ColliderTag::TERMNIAL] = "TERMNIAL";
	ColStrMap[ColliderTag::AXEPICKUP] = "AXEPICKUP";
	ColStrMap[ColliderTag::GATE] = "GATE";
	ColStrMap[ColliderTag::BIOBOMB] = "BIOBOMB";
	ColStrMap[ColliderTag::BOSSGLASS] = "BOSSGLASS";
	ColStrMap[ColliderTag::SWITCH] = "SWITCH";
	ColStrMap[ColliderTag::FBOSS] = "FBOSS";
	ColStrMap[ColliderTag::BOSSHOOK] = "BOSSHOOK";
	ColStrMap[ColliderTag::MPOD] = "MPOD";
	ColStrMap[ColliderTag::SFOG] = "SFOG";
	ColStrMap[ColliderTag::IVAN] = "IVAN";
	ColStrMap[ColliderTag::BIOBREATH] = "BIOBREATH";
	ColStrMap[ColliderTag::ROLLO] = "ROLLO";
}

void ContactListener::BeginContact(b2Contact* contact)
{
	std::string nameA;
	std::string nameB;
	auto bodyUserData1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	auto bodyUserData2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
	auto c1 = reinterpret_cast<Collidable*>(bodyUserData1);
	auto c2 = reinterpret_cast<Collidable*>(bodyUserData2);
	auto fa_userdata = reinterpret_cast<FixtureUserData*>(contact->GetFixtureA()->GetUserData().pointer);
	auto fb_userdata = reinterpret_cast<FixtureUserData*>(contact->GetFixtureB()->GetUserData().pointer);

	if (fa_userdata != nullptr)
	{
		nameA = fa_userdata->name;
	}
	else
	{
		nameA = ColStrMap[c1->m_colliderTag];
	}
		
	if (fb_userdata != nullptr)
	{
		nameB = fb_userdata->name;
	}
	else
	{
		nameB = ColStrMap[c2->m_colliderTag];
	}
	
	//DEBUG contacts lines
	//std::cout << nameA + " collided with " + nameB << std::endl;


	//Player 
	{
		std::string other = "";
		std::string subject = "";
		Collidable* other_c = nullptr;
		b2Fixture* other_fixture = nullptr;
		if (c1->m_colliderTag == PLAYER)
		{
			other = nameB;
			subject = nameA;
			other_c = c2;
			other_fixture = contact->GetFixtureB();
		}
		else if (c2->m_colliderTag == PLAYER)
		{
			other = nameA;
			subject = nameB;
			other_c = c1;
			other_fixture = contact->GetFixtureA();
		}

		if (subject == "p_axe_att")
		{
			
			if (other == "SOLID" || other == "M_BLOCK" || other == "ELEVATOR")
			{
				PlaySound(SoundManager::sounds["axe_solid_hit"]);
			}

			if (other == "W_CRATE")
			{
				WoodCrate* wc = static_cast<WoodCrate*>(other_c);
				wc->TakeDmg(1);

				b2WorldManifold worldManifold;
				ParticleEmitter* p = new ParticleEmitter(wc->pos());
				GameScreen::Particles->Add(DefinedEmitter::dust, p);

				for (int i = 0; i < 5; i++)
				{
					p->EmitParticles();
				}
				GameScreen::add_trauma(0.6f);
			}
			if (other == "GATE")
			{
				std::cout << "gate hit wtf" << std::endl;
				Gate* g = static_cast<Gate*>(other_c);
				g->TakeDmg(1);
				ParticleEmitter* p = new ParticleEmitter(g->pos());
				GameScreen::Particles->Add(DefinedEmitter::dust, p);

				for (int i = 0; i < 5; i++)
				{
					p->EmitParticles();
				}
				GameScreen::add_trauma(0.6f);
			}
			if (other == "BIOBOMB")
			{
				BioBomb* bb = static_cast<BioBomb*>(other_c);
				bb->axed = true;
				ParticleEmitter* p = new ParticleEmitter(bb->pos());
				GameScreen::Particles->Add(DefinedEmitter::dust, p);

				for (int i = 0; i < 5; i++)
				{
					p->EmitParticles();
				}
				GameScreen::add_trauma(0.6f);
				PlaySound(SoundManager::sounds["hit7"]);
				PlaySound(SoundManager::sounds["swish9"]);

			}
			if (!other_fixture->IsSensor())
			{
				other_c->m_body->ApplyLinearImpulseToCenter((GameScreen::player->looking_right ? b2Vec2{ 200.f, 0.0f } : b2Vec2{ -200.0f, 0.0f }), true);
			}
		}


		if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR")
		{
			if (subject == "p_feet")
			{
				player_floor_contacts++;
			}
		}

		if (other == "SOLID" && subject == "p_feet")
		{
			GameScreen::player->is_standing_on_solid = true;
		}


		if (other == "SOLID" || other == "M_BLOCK")
		{
			if (subject == "p_l_s")
			{
				player_left_wall_contacts++;
			}
			if (subject == "p_r_s")
			{
				player_right_wall_contacts++;
			}
			if (subject == "p_lg_s")
			{
				player_left_grab_contacts++;
			}
			if (subject == "p_rg_s")
			{
				player_right_grab_contacts++;
			}
		}

		if (other == "FIREAXE" && subject == "PLAYER" && !GameScreen::player->m_has_axe && !GameScreen::player->axe_just_thrown)
		{
			if (GameScreen::player->axe != nullptr)
			{
				GameScreen::player->axe->m_destroy = true;
				GameScreen::player->axe = nullptr;
			}
			GameScreen::player->m_has_axe = true;
			PlaySound(SoundManager::sounds["axe_pickup"]);
			FireAxe* axe;
			if (other_c->m_colliderTag == FIREAXE)
			{
				axe = static_cast<FireAxe*>(other_c);
				axe->m_destroy = true;
			}
		}

		if (subject == "PLAYER" && other == "ACID")
		{
			GameScreen::player->take_dmg(10);
			ParticleEmitter* p = new ParticleEmitter(GameScreen::player->pos());
			GameScreen::Particles->Add(DefinedEmitter::dust, p);

			for (int i = 0; i < 5; i++)
			{
				p->EmitParticles();
			}
			GameScreen::add_trauma(0.6f);
			if(GameScreen::player->current_hp > 0)
			GameScreen::player->new_pos = GameScreen::player->last_safe_pos;
		}

		if (subject == "PLAYER" && other == "BIOBREATH")
		{
			GameScreen::player->take_dmg(10);
			ParticleEmitter* p = new ParticleEmitter(GameScreen::player->pos());
			GameScreen::Particles->Add(DefinedEmitter::dust, p);
		}

		if (other_c != nullptr)
		{
			other_c->player_in_sensor = true;
		}


	}
	//infected hazmat 
	{
		std::string other = "";
		std::string subject = "";
		InfectedHazmat* e = nullptr;
		if (c1->m_colliderTag == INFECTED_H)
		{
			e = static_cast<InfectedHazmat*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == INFECTED_H)
		{
			e = static_cast<InfectedHazmat*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "INFECTED_H" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);
				
			}

			if (other == "SOLID" && subject == "proxi")
			{
				e->solid_contacts++;
			}

			if (subject == "ih_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "ih_agro")
				{
					PlaySound(SoundManager::sounds["agro1"]);
					e->player_in_agro = true;
				}
				if (subject == "ih_att")
				{
					e->player_in_dmg_zone = true;
				}
				if (subject == "ih_r_s")
				{
					e->right_player_touch = true;
				}
				if (subject == "ih_l_s")
				{
					e->left_player_touch = true;
				}
			}
		}
	}

	//ivan
	{
		std::string other = "";
		std::string subject = "";
		Ivan* e = nullptr;
		if (c1->m_colliderTag == IVAN)
		{
			e = static_cast<Ivan*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == IVAN)
		{
			e = static_cast<Ivan*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "IVAN" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);

			}

			if (other == "SOLID" && subject == "proxi")
			{
				e->solid_contacts++;
			}

			if (subject == "ih_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "ih_agro")
				{
					PlaySound(SoundManager::sounds["agro1"]);
					e->player_in_agro = true;
				}
				if (subject == "ih_att")
				{
					e->player_in_dmg_zone = true;
				}
				if (subject == "ih_r_s")
				{
					e->right_player_touch = true;
				}
				if (subject == "ih_l_s")
				{
					e->left_player_touch = true;
				}
			}
		}
	}

	//rollo
	{
		std::string other = "";
		std::string subject = "";
		Rollo* e = nullptr;
		if (c1->m_colliderTag == ROLLO)
		{
			e = static_cast<Rollo*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == ROLLO)
		{
			e = static_cast<Rollo*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "ROLLO" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);

			}

			if (other == "SOLID" && subject == "proxi")
			{
				e->solid_contacts++;
			}

			if (subject == "rol_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "rol_agro")
				{
					PlaySound(SoundManager::sounds["agro1"]);
					e->player_in_agro = true;
				}
				if (subject == "rol_att")
				{
					e->player_in_dmg_zone = true;
				}
				if (subject == "rol_r_s")
				{
					e->right_player_touch = true;
				}
				if (subject == "rol_l_s")
				{
					e->left_player_touch = true;
				}
			}
		}
	}

	//WCARM 
	{
		std::string other = "";
		std::string subject = "";
		WhiteCoatArm* e = nullptr;
		if (c1->m_colliderTag == WCARM)
		{
			e = static_cast<WhiteCoatArm*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == WCARM)
		{
			e = static_cast<WhiteCoatArm*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "WCARM" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);

			}

			if (subject == "wca_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "wca_agro")
				{
					e->player_in_agro = true;
				}
				if (subject == "wca_att")
				{
					e->player_in_dmg_zone = true;
				}
			}
		}
	}
	//frog boss fboss
	{
		std::string other = "";
		std::string subject = "";
		FrogBoss* e = nullptr;
		if (c1->m_colliderTag == FBOSS)
		{
			e = static_cast<FrogBoss*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == FBOSS)
		{
			e = static_cast<FrogBoss*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (e != nullptr)
		{
			if ((subject == "FBOSS" || subject == "fb_stand_body"|| subject == "fb_run_body") && other == "p_axe_att")
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);
			}

			if (subject == "fboss_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "fb_run_att")
				{
					e->player_in_dmg_zone = true;
				}
				if (subject == "fb_close_att")
				{
					e->player_in_dmg_zone = true;
				}
				if (subject == "fboss_feet")
				{
					//e->player_in_dmg_zone = true;
				}
				if (subject == "fb_swirl_att")
				{
					e->player_in_dmg_zone = true;
				}
			}

			if (subject == "fb_run_att" && other == "SOLID")
			{
				if (!e->changed_sides)
				{
					e->looking_right = !e->looking_right;
					e->changed_sides = true;
				}
			}
			
		}
	}


	//ribbs 
	{
		std::string other = "";
		std::string subject = "";
		Ribbs* e = nullptr;
		if (c1->m_colliderTag == RIBBS)
		{
			e = static_cast<Ribbs*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == RIBBS)
		{
			e = static_cast<Ribbs*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "RIBBS" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);

			}

			if (subject == "rib_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "rib_agro")
				{
					PlaySound(SoundManager::sounds["monster-2"]);
					e->player_in_agro = true;
				}
				if (subject == "rib_att")
				{
					e->player_in_dmg_zone = true;
				}
			}
		}
	}
	// footb 
	{
		std::string other = "";
		std::string subject = "";
		Football* e = nullptr;
		if (c1->m_colliderTag == FOOTB)
		{
			e = static_cast<Football*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == FOOTB)
		{
			e = static_cast<Football*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "FOOTB" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);
			}

			if (other == "PLAYER")
			{
				if (subject == "footb_att")
				{
					e->player_in_dmg_zone = true;
				}
			}
		}
	}

	// hspit 
	{
		std::string other = "";
		std::string subject = "";
		HeadSpit* e = nullptr;
		if (c1->m_colliderTag == HSPIT)
		{
			e = static_cast<HeadSpit*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == HSPIT)
		{
			e = static_cast<HeadSpit*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "HSPIT" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);
			}

			if (other == "PLAYER")
			{
				if (subject == "hspit_att")
				{
					e->player_in_dmg_zone = true;
				}
			}
		}
	}

	//leggy 
	{
		std::string other = "";
		std::string subject = "";
		Leggy* e = nullptr;
		if (c1->m_colliderTag == LEGGY)
		{
			e = static_cast<Leggy*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == LEGGY)
		{
			e = static_cast<Leggy*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "LEGGY" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);
			}

			if (subject == "leg_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts++;
			}

			if (other == "PLAYER")
			{
				if (subject == "leg_agro")
				{
					PlaySound(SoundManager::sounds["agro1"]);
					e->player_in_agro = true;
				}
				if (subject == "leg_att")
				{
					e->player_in_dmg_zone = true;
				}
			}
		}
	}

	//flying infected
	{

		std::string other = "";
		std::string subject = "";
		FlyingInfected* e = nullptr;
		if (c1->m_colliderTag == FLYING_INF)
		{
			e = static_cast<FlyingInfected*>(c1);
			subject = nameA;
			other = nameB;
		}
		else if (c2->m_colliderTag == FLYING_INF)
		{
			e = static_cast<FlyingInfected*>(c2);
			subject = nameB;
			other = nameA;
		}


		if (e != nullptr)
		{
			if (other == "PLAYER")
			{
				if (subject == "fi_agro")
				{
					e->player_in_agro = true;
					PlaySound(SoundManager::sounds["agro2"]);
				}
				if (subject == "fi_att")
				{
					e->player_in_dmg_zone = true;
				}
			}

			if (subject == "FLYING_INF")
			{
				if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR")
				{
					e->ground_contacts++;
				}
			}
			if (subject == "FLYING_INF" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);
			}
			if (subject == "wingflap" && other == "PLAYER")
			{
				e->player_in_wingflap = true;
			}
		}
	}

	//LevelPortal 
	{
		std::string other = "";
		LevelPortal* e = nullptr;
		if (c1->m_colliderTag == LEVEL_PORTAL)
		{
			e = static_cast<LevelPortal*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == LEVEL_PORTAL)
		{
			e = static_cast<LevelPortal*>(c2);
			other = nameA;
		}
		if (other == "PLAYER")
		{
			auto ref = e->m_iid_reference;
			auto& level = GameScreen::LevelMgr->ldtkWorld->getLevel(e->m_to_level);	
			auto& layer = level.getLayer("Entities");
			auto& cp = layer.getEntity(ref);
			Vector2 newPos{ (cp.getPosition().x) ,
							(cp.getPosition().y) };

			if (e->is_active)
			{
					GameScreen::LevelMgr->next_level = (e->m_to_level);
					GameScreen::LevelMgr->new_player_pos = newPos;
			}
		}
	}


	//Door
	{
		std::string other = "";
		std::string subject = "";
		Door* e = nullptr;
		if (c1->m_colliderTag == DOOR)
		{
			e = static_cast<Door*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == DOOR)
		{
			e = static_cast<Door*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (other == "PLAYER" && subject == "door_sensor")
		{
			e->player_in_sensor = true;
		}
	}

	//NPC security guy
	{
		std::string other = "";
		std::string subject = "";
		NPCSecurityGuy* e = nullptr;
		if (c1->m_colliderTag == NPC)
		{
			e = static_cast<NPCSecurityGuy*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == NPC)
		{
			e = static_cast<NPCSecurityGuy*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (other == "PLAYER" && subject == "npc_sensor")
		{
			e->looking_right = false;
			e->player_in_sensor = true;
		}
		if (other == "INFECTED_H" && subject == "npc_fear_sensor")
		{
			e->enemy_in_sensor = true;
		}
	}

	//NPC doc 1
	{
		std::string other = "";
		std::string subject = "";
		NPCDoc1* e = nullptr;
		if (c1->m_colliderTag == NPC_DOC1)
		{
			e = static_cast<NPCDoc1*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == NPC_DOC1)
		{
			e = static_cast<NPCDoc1*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (other == "PLAYER" && subject == "npc_sensor")
		{
			e->player_in_sensor = true;
		}
	}

	//Fireaxe
	{
		std::string other = "";
		std::string subject = "";
		Collidable* other_c = nullptr;
		FireAxe* axe = nullptr;
		if (c1->m_colliderTag == FIREAXE)
		{
			axe = static_cast<FireAxe*>(c1);
			other = nameB;
			subject = nameA;
			other_c = c2;
		}
		else if (c2->m_colliderTag == FIREAXE)
		{
			axe = static_cast<FireAxe*>(c2);
			other = nameA;
			subject = nameB;
			other_c = c1;
		}

		if(subject=="FIREAXE" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
		{
			PlaySound(SoundManager::sounds["axe_solid_hit"]);
		}
		if (subject == "FIREAXE" && other == "SWITCH")
		{
			Switch* sw = static_cast<Switch*>(other_c);
			sw->axed = true;
		}
	}
}

void ContactListener::EndContact(b2Contact* contact)
{
	std::string nameA;
	std::string nameB;
	auto bodyUserData1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	auto bodyUserData2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
	auto c1 = reinterpret_cast<Collidable*>(bodyUserData1);
	auto c2 = reinterpret_cast<Collidable*>(bodyUserData2);
	auto fa_userdata = reinterpret_cast<FixtureUserData*>(contact->GetFixtureA()->GetUserData().pointer);
	auto fb_userdata = reinterpret_cast<FixtureUserData*>(contact->GetFixtureB()->GetUserData().pointer);
	if (fa_userdata != nullptr)
	{
		nameA = fa_userdata->name;
	}
	else
	{
		nameA = ColStrMap[c1->m_colliderTag];
	}

	if (fb_userdata != nullptr)
	{
		nameB = fb_userdata->name;
	}
	else
	{
		nameB = ColStrMap[c2->m_colliderTag];
	}

	//Player
	{
		std::string other = "";
		std::string subject = "";
		Collidable* other_c = nullptr;
		b2Fixture* other_fixture = nullptr;
		if (c1->m_colliderTag == PLAYER)
		{
			other = nameB;
			subject = nameA;
			other_c = c2;
			other_fixture = contact->GetFixtureB();
		}
		else if (c2->m_colliderTag == PLAYER)
		{
			other = nameA;
			subject = nameB;
			other_c = c1;
			other_fixture = contact->GetFixtureA();
		}

		if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR")
		{
			if (subject == "p_feet")
			{
				player_floor_contacts--;
			}
		}

		if (other == "SOLID" && subject == "p_feet")
		{
			GameScreen::player->is_standing_on_solid = false;
		}

		if (other == "SOLID" || other == "M_BLOCK")
		{
			if (subject == "p_l_s")
			{
				player_left_wall_contacts--;
			}
			if (subject == "p_r_s")
			{
				player_right_wall_contacts--;
			}
			if (subject == "p_lg_s")
			{
				player_left_grab_contacts--;
			}
			if (subject == "p_rg_s")
			{
				player_right_grab_contacts--;
			}
		}
		if (other_c != nullptr)
		{
			other_c->player_in_sensor = false;
		}


	}

	//frog boss fboss
	{
		std::string other = "";
		std::string subject = "";
		FrogBoss* e = nullptr;
		if (c1->m_colliderTag == FBOSS)
		{
			e = static_cast<FrogBoss*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == FBOSS)
		{
			e = static_cast<FrogBoss*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{

			if (subject == "fboss_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{
				if (subject == "fb_run_att")
				{
					e->player_in_dmg_zone = false;
				}
				if (subject == "fb_close_att")
				{
					e->player_in_dmg_zone = false;
				}
				if (subject == "fboss_feet")
				{
					//e->player_in_dmg_zone = false;
				}
				if (subject == "fb_swirl_att")
				{
					e->player_in_dmg_zone = false;
				}
			}
			//if (subject == "fb_run_att" && other == "SOLID")
			//{
			//	e->wall_contacts--;
			//}
		}
	}

	//infected hazmat 
	{
		std::string other = "";
		std::string subject = "";
		InfectedHazmat* e = nullptr;
		if (c1->m_colliderTag == INFECTED_H)
		{
			e = static_cast<InfectedHazmat*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == INFECTED_H)
		{
			e = static_cast<InfectedHazmat*>(c2);
			other = nameA;
			subject = nameB;
		}

		if (e != nullptr)
		{
			if (other == "SOLID" && subject == "proxi")
			{
				e->solid_contacts--;
			}

			if (subject == "ih_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{

				if (subject == "ih_agro")
				{
					e->player_in_agro = false;
				}
				if (subject == "ih_att")
				{
					e->player_in_dmg_zone = false;
				}
				if (subject == "ih_r_s")
				{
					e->right_player_touch = false;
				}
				if (subject == "ih_l_s")
				{
					e->left_player_touch = false;
				}
			}
		}
	}

	//ivan
	{
		std::string other = "";
		std::string subject = "";
		Ivan* e = nullptr;
		if (c1->m_colliderTag == IVAN)
		{
			e = static_cast<Ivan*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == IVAN)
		{
			e = static_cast<Ivan*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{

			if (other == "SOLID" && subject == "proxi")
			{
				e->solid_contacts--;
			}

			if (subject == "ih_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{
				if (subject == "ih_agro")
				{
					PlaySound(SoundManager::sounds["agro1"]);
					e->player_in_agro = false;
				}
				if (subject == "ih_att")
				{
					e->player_in_dmg_zone = false;
				}
				if (subject == "ih_r_s")
				{
					e->right_player_touch = false;
				}
				if (subject == "ih_l_s")
				{
					e->left_player_touch = false;
				}
			}
		}
	}

	//rollo
	{
		std::string other = "";
		std::string subject = "";
		Rollo* e = nullptr;
		if (c1->m_colliderTag == ROLLO)
		{
			e = static_cast<Rollo*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == ROLLO)
		{
			e = static_cast<Rollo*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "ROLLO" && other == "p_axe_att" && (e->state != EnemyState::Hurting && e->state != EnemyState::Dying))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
				GameScreen::add_trauma(0.6f);

			}

			if (other == "SOLID" && subject == "proxi")
			{
				e->solid_contacts--;
			}

			if (subject == "rol_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{
				if (subject == "rol_agro")
				{
					PlaySound(SoundManager::sounds["agro1"]);
					e->player_in_agro = false;
				}
				if (subject == "rol_att")
				{
					e->player_in_dmg_zone = false;
				}
				if (subject == "rol_r_s")
				{
					e->right_player_touch = false;
				}
				if (subject == "rol_l_s")
				{
					e->left_player_touch = false;
				}
			}
		}
	}

	//leggy 
	{
		std::string other = "";
		std::string subject = "";
		Leggy* e = nullptr;
		if (c1->m_colliderTag == LEGGY)
		{
			e = static_cast<Leggy*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == LEGGY)
		{
			e = static_cast<Leggy*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "LEGGY" && other == "p_axe_att" && e->state != EnemyState::Hurting)
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
			}

			if (subject == "leg_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{
				if (subject == "leg_agro")
				{
					e->player_in_agro = false;
				}
				if (subject == "leg_att")
				{
					e->player_in_dmg_zone = false;
				}
			}
		}
	}

	// hspit 
	{
		std::string other = "";
		std::string subject = "";
		HeadSpit* e = nullptr;
		if (c1->m_colliderTag == HSPIT)
		{
			e = static_cast<HeadSpit*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == HSPIT)
		{
			e = static_cast<HeadSpit*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (other == "PLAYER")
			{
				if (subject == "hspit_att")
				{
					e->player_in_dmg_zone = false;
				}
			}
		}
	}

	// footb 
	{
		std::string other = "";
		std::string subject = "";
		Football* e = nullptr;
		if (c1->m_colliderTag == FOOTB)
		{
			e = static_cast<Football*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == FOOTB)
		{
			e = static_cast<Football*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "FOOTB" && other == "p_axe_att" && e->state != EnemyState::Hurting)
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
			}

			if (other == "PLAYER")
			{
				if (subject == "footb_att")
				{
					e->player_in_dmg_zone = false;
				}
			}
		}
	}

	//WCARM 
	{
		std::string other = "";
		std::string subject = "";
		WhiteCoatArm* e = nullptr;
		if (c1->m_colliderTag == WCARM)
		{
			e = static_cast<WhiteCoatArm*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == WCARM)
		{
			e = static_cast<WhiteCoatArm*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{

			if (subject == "wca_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{
				if (subject == "wca_agro")
				{
					e->player_in_agro = false;
				}
				if (subject == "wca_att")
				{
					e->player_in_dmg_zone = false;
				}
			}
		}
	}

	//ribbs 
	{
		std::string other = "";
		std::string subject = "";
		Ribbs* e = nullptr;
		if (c1->m_colliderTag == RIBBS)
		{
			e = static_cast<Ribbs*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == RIBBS)
		{
			e = static_cast<Ribbs*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (subject == "RIBBS" && other == "p_axe_att" && !(e->state == EnemyState::Hurting))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
			}

			if (subject == "rib_feet" && (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
			{
				e->ground_contacts--;
			}

			if (other == "PLAYER")
			{
				if (subject == "rib_agro")
				{
					e->player_in_agro = false;
				}
				if (subject == "rib_att")
				{
					e->player_in_dmg_zone = false;
				}
				if (subject == "rib_r_s")
				{
					e->right_player_touch = false;
				}
				if (subject == "rib_l_s")
				{
					e->left_player_touch = false;
				}
			}
		}
	}

	//flying infected
	{
		std::string other = "";
		std::string subject = "";
		FlyingInfected* e = nullptr;
		if (c1->m_colliderTag == FLYING_INF)
		{
			e = static_cast<FlyingInfected*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == FLYING_INF)
		{
			e = static_cast<FlyingInfected*>(c2);
			other = nameA;
			subject = nameB;
		}


		if (e != nullptr)
		{
			if (other == "PLAYER")
			{
				if (subject == "fi_agro")
				{
					e->player_in_agro = false;
				}
				if (subject == "fi_att")
				{
					e->player_in_dmg_zone = false;
				}
			}

			if (subject == "FLYING_INF")
			{
				if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR")
				{
					e->ground_contacts--;
				}
			}

			if (subject == "wingflap" && other == "PLAYER")
			{
				e->player_in_wingflap = false;
			}
		}
	}

	
	//Door
	{
		std::string other = "";
		std::string subject = "";
		Door* e = nullptr;
		if (c1->m_colliderTag == DOOR)
		{
			e = static_cast<Door*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == DOOR)
		{
			e = static_cast<Door*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (other == "PLAYER" && subject == "door_sensor")
		{
			e->player_in_sensor = false;
		}
	}
	
	//NPC security guy
	{
		std::string other = "";
		std::string subject = "";
		NPCSecurityGuy* e = nullptr;
		if (c1->m_colliderTag == NPC)
		{
			e = static_cast<NPCSecurityGuy*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == NPC)
		{
			e = static_cast<NPCSecurityGuy*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (other == "PLAYER" && subject == "npc_sensor")
		{
			e->looking_right = true;
			e->player_in_sensor = false;
		}
	}

	//NPC doc 1
	{
		std::string other = "";
		std::string subject = "";
		NPCDoc1* e = nullptr;
		if (c1->m_colliderTag == NPC_DOC1)
		{
			e = static_cast<NPCDoc1*>(c1);
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == NPC_DOC1)
		{
			e = static_cast<NPCDoc1*>(c2);
			other = nameA;
			subject = nameB;
		}
		if (other == "PLAYER" && subject == "npc_sensor")
		{
			e->player_in_sensor = false;
		}
	}
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	if (GameScreen::player->is_dashing)
	{
		auto bodyUserData1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		auto bodyUserData2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		auto c1 = reinterpret_cast<Collidable*>(bodyUserData1);
		auto c2 = reinterpret_cast<Collidable*>(bodyUserData2);

		if(c1->m_colliderTag==PLAYER && c2->m_colliderTag!=SOLID)

		contact->SetEnabled(false);
		
	}


}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	std::string nameA;
	std::string nameB;
	auto bodyUserData1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	auto bodyUserData2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
	auto c1 = reinterpret_cast<Collidable*>(bodyUserData1);
	auto c2 = reinterpret_cast<Collidable*>(bodyUserData2);
	auto fa_userdata = reinterpret_cast<FixtureUserData*>(contact->GetFixtureA()->GetUserData().pointer);
	auto fb_userdata = reinterpret_cast<FixtureUserData*>(contact->GetFixtureB()->GetUserData().pointer);
	if (fa_userdata != nullptr)
	{
		nameA = fa_userdata->name;
	}
	else
	{
		nameA = ColStrMap[c1->m_colliderTag];
	}

	if (fb_userdata != nullptr)
	{
		nameB = fb_userdata->name;
	}
	else
	{
		nameB = ColStrMap[c2->m_colliderTag];
	}


	//axe
	std::string other = "";
	std::string subject = "";
	Collidable* other_c = nullptr;
	FixtureUserData* user_data = nullptr;
	Enemy* e = nullptr;

	if (c1->m_colliderTag == FIREAXE)
	{
		other = nameB;
		subject = nameA;
		other_c = c2;

	}
	else if (c2->m_colliderTag == FIREAXE)
	{
		other = nameA;
		subject = nameB;
		other_c = c1;
	}
	e = static_cast<Enemy*>(other_c);

	if (e != nullptr )
	{
		user_data = reinterpret_cast<FixtureUserData*>(e->m_fixtureDef.userData.pointer);
	}

	if (e != nullptr && user_data != nullptr && user_data->tag == ENEMY_GROUP)
	{
		float dmg_impulse = std::fmaxf(impulse->normalImpulses[0], impulse->normalImpulses[1]);
		if (dmg_impulse > 20)
		{
			e->TakeDmg(dmg_impulse * 1.4f);
			GameScreen::add_trauma(0.6f);
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);
			Vector2 contact_point = { worldManifold.points[0].x * settings::PPM ,
										worldManifold.points[0].y * settings::PPM };
			ParticleEmitter* p = new ParticleEmitter(contact_point);
			GameScreen::Particles->Add(DefinedEmitter::dust, p);
			for (int i = 0; i < 5; i++)
			{
				p->EmitParticles();

			}
		}
	}



	//wood crate
	{
		std::string other = "";
		WoodCrate* e = nullptr;
		if (c1->m_colliderTag == W_CRATE)
		{
			e = static_cast<WoodCrate*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == W_CRATE)
		{
			e = static_cast<WoodCrate*>(c2);
			other = nameA;
		}


		if (other == "FIREAXE"  &&
			(!contact->GetFixtureA()->IsSensor() && !contact->GetFixtureB()->IsSensor()))
		{
		float dmg_impulse = std::fmaxf(impulse->normalImpulses[0], impulse->normalImpulses[1]);
			if (dmg_impulse > 20)
			{
				e->TakeDmg(dmg_impulse);
				GameScreen::add_trauma(0.6f);
				b2WorldManifold worldManifold;
				contact->GetWorldManifold(&worldManifold);
				Vector2 contact_point = { worldManifold.points[0].x * settings::PPM ,
											worldManifold.points[0].y * settings::PPM };
				ParticleEmitter* p = new ParticleEmitter(contact_point);
				GameScreen::Particles->Add(DefinedEmitter::dust, p);
				for (int i = 0; i < 5; i++)
				{
					p->EmitParticles();

				}
			}
		}
	}

	//GATE
	{
		std::string other = "";
		Gate* e = nullptr;
		if (c1->m_colliderTag == GATE)
		{
			e = static_cast<Gate*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == GATE)
		{
			e = static_cast<Gate*>(c2);
			other = nameA;
		}

		if (other == "FIREAXE" && contact->IsTouching() &&
			(!contact->GetFixtureA()->IsSensor() && !contact->GetFixtureB()->IsSensor()))
		{
			float dmg_impulse = std::fmaxf(impulse->normalImpulses[0], impulse->normalImpulses[1]);
			if (dmg_impulse > 20)
			{
				e->TakeDmg(dmg_impulse);
				GameScreen::add_trauma(0.6f);
			}
		}
	}

	
	
}


void DestructionListener::SayGoodbye(b2Joint* joint)
{
	//auto fa_userdata = reinterpret_cast<FixtureUserData*>(joint->GetUserData().pointer);
	//	if (fa_userdata != nullptr)
	//	{
	//		std::cout << "TO DELETE";
	//		std::cout << fa_userdata->name << std::endl;
	//	}
}

void DestructionListener::SayGoodbye(b2Fixture* fixture)
{
	//auto fa_userdata = reinterpret_cast<FixtureUserData*>(fixture->GetUserData().pointer);
	//if (fa_userdata != nullptr)
	//{
	//	//std::cout << "TO DELETE";
	//	//std::cout << fa_userdata->name << std::endl;
	//}
}


bool ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	std::string nameA;
	std::string nameB;
	auto bodyUserData1 = fixtureA->GetBody()->GetUserData().pointer;
	auto bodyUserData2 = fixtureB->GetBody()->GetUserData().pointer;
	auto c1 = reinterpret_cast<Collidable*>(bodyUserData1);
	auto c2 = reinterpret_cast<Collidable*>(bodyUserData2);
	auto fa_userdata = reinterpret_cast<FixtureUserData*>(fixtureA->GetUserData().pointer);
	auto fb_userdata = reinterpret_cast<FixtureUserData*>(fixtureB->GetUserData().pointer);
	std::string other = "";
	std::string subject = "";
	Collidable* other_c = nullptr;
	if (fa_userdata != nullptr)
	{
		nameA = fa_userdata->name;
	}
	else
	{
		nameA = ContactListener::ColStrMap[c1->m_colliderTag];
	}

	if (fb_userdata != nullptr)
	{
		nameB = fb_userdata->name;
	}
	else
	{
		nameB = ContactListener::ColStrMap[c2->m_colliderTag];
	}

	//player collision filter
	{
		if (c1->m_colliderTag == PLAYER)
		{
			other = nameB;
			subject = nameA;
			other_c = c2;
		}
		else if (c2->m_colliderTag == PLAYER)
		{
			other = nameA;
			subject = nameB;
			other_c = c1;
		}

		if (subject == "PLAYER" && (other == "FOOTB" || other == "SHARD" || other == "FBOSS"))
		{
			return false;
		}
		if (subject == "PLAYER" && other != "SOLID" && GameScreen::player->is_dashing)
		{
			return false;
		}
	}
	

	if (c1->m_colliderTag == NPC)
	{
		other = nameB;
		subject = nameA;
		other_c = c2;
	}
	else if (c2->m_colliderTag == NPC)
	{
		other = nameA;
		subject = nameB;
		other_c = c1;
	}
	if (subject == "NPC" && other != "SOLID")
	{
		return false;
	}

	if (c1->m_colliderTag == BIOBOMB)
	{
		other = nameB;
		subject = nameA;
		other_c = c2;
	}
	else if (c2->m_colliderTag == BIOBOMB)
	{
		other = nameA;
		subject = nameB;
		other_c = c1;
	}



	return true;

	//auto fa_userdata = reinterpret_cast<FixtureUserData*>(fixtureA->GetUserData().pointer);
	//auto fb_userdata = reinterpret_cast<FixtureUserData*>(fixtureB->GetUserData().pointer);
	//ContactGroup a = UNASSIGNED_GROUP;
	//ContactGroup b = UNASSIGNED_GROUP;
	//if (fa_userdata != nullptr) a = fa_userdata->tag;
	//if (fb_userdata != nullptr) b = fb_userdata->tag;

	//if (Group(a, b, PLAYER_GROUP, ENEMY_GROUP))
	//{
	//	return false;
	//}
	//else
	//{
	//	return true;
	//}
}

bool ContactFilter::Group(ContactGroup a, ContactGroup b, ContactGroup compare1, ContactGroup compare2)
{
	if (a == UNASSIGNED_GROUP || b == UNASSIGNED_GROUP)
	{
		return false;
	}
	else
	{
		return ((a == compare1 && b == compare2) || (a == compare2 && b == compare1));
	}
	
}
