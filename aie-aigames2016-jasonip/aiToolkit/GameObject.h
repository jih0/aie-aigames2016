#pragma once

#include <vector>

class Behaviour;

class GameObject
{
public:
	GameObject();
	~GameObject();

	// movement function
	void setPosition(float x, float y)		{ m_x = x; m_y = y; }
	void getPosition(float* x, float* y) const	{ *x = m_x; *y = m_y; }
	void translate(float x, float y)		{ m_x += x; m_y += y; }

	// add a behaviour
	void addBehaviour(Behaviour* behaviour);

	// update game object and execute behaviour
	virtual void update(float deltaTime);

protected:
	float m_x, m_y;

	std::vector<Behaviour*> m_behaviours;
};

