#pragma once

class Interactable
{
public:
	virtual ~Interactable() = default;
	virtual void Interract() = 0;
	bool interacted;
};