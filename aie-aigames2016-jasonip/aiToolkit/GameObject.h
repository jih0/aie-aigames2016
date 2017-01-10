#pragma once

#include <vector>
#include "Blackboard.h"

class Behaviour;

struct Vector2 {
	float x, y;
};

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
	virtual void update(GameObject* target, float deltaTime);

	// const glm::vec2& getPosition() const { return m_position; }
	
	Blackboard& getBlackboard() { return m_blackboard; }
protected:

	// glm::vec2	m_position;

	float m_x, m_y;

	Blackboard				m_blackboard;
	std::vector<Behaviour*> m_behaviours;
};

