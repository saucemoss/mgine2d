#pragma once
#include "Box2D.h"
#include "Collidable.h"
#include <map>

class ContactListener : public b2ContactListener
{
public:
    ContactListener();
    static std::map<ColliderTag, std::string> ColStrMap;
    int player_floor_contacts = 0;
    int player_left_wall_contacts = 0;
    int player_right_wall_contacts = 0;

private:
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

class DestructionListener : public b2DestructionListener
{
    void SayGoodbye(b2Joint* joint);
    void SayGoodbye(b2Fixture* fixture);
};

class ContactFilter : public b2ContactFilter
{
    bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
    bool Group(ContactGroup a, ContactGroup b, ContactGroup compare1, ContactGroup compare2);
};