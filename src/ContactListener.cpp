#include "ContactListener.h"
#include "Collidable.h"
#include "Player.h"
#include "GameScreen.h"
#include "InfectedHazmat.h"




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


	//Player contacts
	{
		std::string other = "";
		if (c1->m_colliderTag == PLAYER)
		{
			other = nameB;
		}
		else if (c2->m_colliderTag == PLAYER)
		{
			other = nameA;
		}


		if ((other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR") && contact->IsTouching())
		{
			if (nameA == "feet_sensor" || nameB == "feet_sensor")
			{
				player_floor_contacts++;
			}
		}

		if (other == "SOLID" || other == "M_BLOCK")
		{
			if (nameA == "left_sensor" || nameB == "left_sensor")
			{
				player_left_wall_contacts++;
			}
			if (nameA == "right_sensor" || nameB == "right_sensor")
			{
				player_right_wall_contacts++;
			}
		}

		if (other == "FIREAXE" && (nameA == "PLAYER" || nameB == "PLAYER"))
		{
			GameScreen::player->axe->m_destroy = true;	
			GameScreen::player->m_has_axe = true;
		}
	}
	
	//Enemy contacts
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

	//Player contacts
	{
		std::string other = "";
		if (c1->m_colliderTag == PLAYER)
		{
			other = nameB;
		}
		else if (c2->m_colliderTag == PLAYER)
		{
			other = nameA;
		}

		if ((other == "SOLID" || other == "M_BLOCK" || other == "W_CRATE" || other == "ELEVATOR"))
		{
			if (nameA == "feet_sensor" || nameB == "feet_sensor")
			{
				player_floor_contacts--;
			}
		}

		if (other == "SOLID" || other == "M_BLOCK")
		{
			if (nameA == "left_sensor" || nameB == "left_sensor")
			{
				player_left_wall_contacts--;
			}
			if (nameA == "right_sensor" || nameB == "right_sensor")
			{
				player_right_wall_contacts--;
			}
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


	//Enemy contacts
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
			//std::cout << impulse->normalImpulses[0] << std::endl;
			//std::cout << impulse->normalImpulses[1] << std::endl;
			if (impulse->normalImpulses[0] > 40.0f || impulse->normalImpulses[1] > 40.0f)
			{
				e->Die();
			}
		}
	}
}

