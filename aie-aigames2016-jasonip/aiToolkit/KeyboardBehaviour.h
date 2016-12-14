#pragma once
#include "Behaviour.h"

class KeyboardBehaviour :
	public Behaviour
{
public:
	KeyboardBehaviour();
	~KeyboardBehaviour();

	virtual bool execute(GameObject* gameObject, float deltaTime);

	void setSpeed(float speed) { m_speed = speed; }

private:
	float m_speed;
};

