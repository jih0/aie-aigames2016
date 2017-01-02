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

	void setVelocity(float x, float y) { m_vx = x; m_vy = y; }
	void getVelocity(float* x, float* y) const { *x = m_vx; *y = m_vy; }

	void setMaxVelocity(float velocity) { m_maxVelocity = velocity; }
	float getMaxVelocity() const { return m_maxVelocity; }

	void setMaxForce(float force) { m_maxForce = force; }
	float getMaxForce() const { return m_maxForce; }

	void addForce(float x, float y) { m_vx += x; m_vy += y; }

	// const glm::vec2& getPosition() const { return m_position; }
	
	// wander data
	void setWanderData(float offset, float radius, float jitter) {
		m_wanderOffset = offset;
		m_wanderRadius = radius;
		m_wanderJitter = jitter;
	}
	float getWanderOffset() const { return m_wanderOffset; }
	float getWanderRadius() const { return m_wanderRadius; }
	float getWanderJitter() const { return m_wanderJitter; }

	void getWanderTarget(float* x, float* y) const { *x = m_wanderX; *y = m_wanderY; }
	void setWanderTarget(float x, float y) { m_wanderX = x; m_wanderY = y; }

protected:

	// glm::vec2	m_position;

	float m_x, m_y;

	float m_vx, m_vy;
	float m_maxVelocity;
	float m_maxForce;

	// wander data
	float m_wanderOffset;
	float m_wanderRadius; // the bigger the radius, the faster the object will turn
	float m_wanderJitter; // the bigger the jitter, the bigger the movement
	float m_wanderX, m_wanderY;

	std::vector<Behaviour*> m_behaviours;
};

