#include "ContactListener.h"
#include "Collidable.h"
#include "Player.h"
#include "GameScreen.h"
#include "InfectedHazmat.h"
#include "Elevator.h"
#include "FlyingInfected.h"




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

		if (subject == "p_axe_att" )
		{
			other_c->m_body->ApplyLinearImpulseToCenter((GameScreen::player->looking_right ? b2Vec2{ 100.f, 0.0f } : b2Vec2{ -100.0f, 0.0f }), true);
			if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR")
			{
				PlaySound(SoundManager::sounds["axe_solid_hit"]);
			}
		}


		if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR" || other == "INFECTED_H")
		{
			if (subject == "p_feet")
			{
				player_floor_contacts++;
			}
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

	}
	#pragma region infected hazmat 

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
			if (subject == "INFECTED_H" && other == "p_axe_att" && !(e->state == InfectedHazmatState::Hurting))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
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
	#pragma endregion infected hazmat 

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
			if (subject == "FLYING_INF" && other == "p_axe_att" && !(e->state == FlyingInfectedStates::Hurting))
			{
				e->TakeDmg(GameScreen::player->axe_dmg);
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
	//Elevator 
	{
		std::string other = "";
		Elevator* e = nullptr;
		if (c1->m_colliderTag == ELEVATOR)
		{
			e = static_cast<Elevator*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == ELEVATOR)
		{
			e = static_cast<Elevator*>(c2);
			other = nameA;
		}
		if (other == "PLAYER")
		{
			e->player_in_sensor = true;
		}
	}
	//Elevator Switch
	{
		std::string other = "";
		ElevatorCallSwitch* e = nullptr;
		if (c1->m_colliderTag == ELEVATOR_CALL_SW)
		{
			e = static_cast<ElevatorCallSwitch*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == ELEVATOR_CALL_SW)
		{
			e = static_cast<ElevatorCallSwitch*>(c2);
			other = nameA;
		}
		if (other == "PLAYER")
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
		std::string subject = "";
		std::string other = "";
		if (c1->m_colliderTag == PLAYER)
		{
			other = nameB;
			subject = nameA;
		}
		else if (c2->m_colliderTag == PLAYER)
		{
			other = nameA;
			subject = nameB;
		}

		if (other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR" || other == "INFECTED_H")
		{
			if (subject == "p_feet")
			{
				player_floor_contacts--;
			}
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
		}

	}
	#pragma region infected hazmat 
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

	#pragma endregion

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

	//Elevator 
	{
		std::string other = "";
		Elevator* e = nullptr;
		if (c1->m_colliderTag == ELEVATOR)
		{
			e = static_cast<Elevator*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == ELEVATOR)
		{
			e = static_cast<Elevator*>(c2);
			other = nameA;
		}
		if (other == "PLAYER")
		{
			e->player_in_sensor = false;
		}
	}
	//Elevator Switch
	{
		std::string other = "";
		ElevatorCallSwitch* e = nullptr;
		if (c1->m_colliderTag == ELEVATOR_CALL_SW)
		{
			e = static_cast<ElevatorCallSwitch*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == ELEVATOR_CALL_SW)
		{
			e = static_cast<ElevatorCallSwitch*>(c2);
			other = nameA;
		}
		if (other == "PLAYER")
		{
			e->player_in_sensor = false;
		}
	}
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
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


	//Infected hazmat
	{
		std::string other = "";
		InfectedHazmat* e = nullptr;
		if (c1->m_colliderTag == INFECTED_H)
		{
			e = static_cast<InfectedHazmat*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == INFECTED_H)
		{
			e = static_cast<InfectedHazmat*>(c2);
			other = nameA;
		}

		if (other == "FIREAXE" && contact->IsTouching() &&
			(!contact->GetFixtureA()->IsSensor() && !contact->GetFixtureB()->IsSensor()))
		{
			float dmg_impulse = std::fmaxf(impulse->normalImpulses[0], impulse->normalImpulses[1]);
			if (dmg_impulse > 10)
			{
				e->TakeDmg(dmg_impulse);
			}
		}
	}
	//Flying infected
	{
		std::string other = "";
		FlyingInfected* e = nullptr;
		if (c1->m_colliderTag == FLYING_INF)
		{
			e = static_cast<FlyingInfected*>(c1);
			other = nameB;
		}
		else if (c2->m_colliderTag == FLYING_INF)
		{
			e = static_cast<FlyingInfected*>(c2);
			other = nameA;
		}

		if (other == "FIREAXE" && contact->IsTouching() &&
			(!contact->GetFixtureA()->IsSensor() && !contact->GetFixtureB()->IsSensor()))
		{
			
			float dmg_impulse = std::fmaxf(impulse->normalImpulses[0], impulse->normalImpulses[1]);
			if (dmg_impulse > 10)
			{
				e->TakeDmg(dmg_impulse * 1.5f);
			}

		}
	}
}


void DestructionListener::SayGoodbye(b2Joint* joint)
{
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
