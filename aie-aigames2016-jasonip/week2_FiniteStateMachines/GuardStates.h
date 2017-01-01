#pragma once

#include "State.h"

class AttackState : public State {
public:

	AttackState(GameObject* target, float speed) : m_target(target), m_speed(speed) {}
	virtual ~AttackState() {}

	virtual void	update(GameObject* gameObject, float deltaTime) {
		if (m_target == nullptr)
			return;

		// get target position
		float tx = 0, ty = 0;
		m_target->getPosition(&tx, &ty);

		// get my position
		float x = 0, y = 0;
		gameObject->getPosition(&x, &y);

		// compare the two and get the distance between them
		float xDiff = tx - x;
		float yDiff = ty - y;
		float distance = sqrt(xDiff*xDiff + yDiff*yDiff);

		// if not at the target then move towards them
		if (distance > 0) {
			// need to make the difference the length of 1 (normalize)
			// this is so movement can be "pixels per second"
			xDiff /= distance;
			yDiff /= distance;

			// move to target (can overshoot!)
			gameObject->translate(xDiff * m_speed * deltaTime, yDiff * m_speed * deltaTime);
		}
	}

private:
	float		m_speed;
	GameObject*	m_target;
};

class IdleState : public State {
public:

	IdleState() {}
	virtual ~IdleState() {}

	virtual void	update(GameObject* gameObject, float deltaTime) {}
};

class PatrolState : public State {
public:

	PatrolState(float speed) : m_currentTarget(0), m_speed(speed) {}
	virtual ~PatrolState() {}

	void addWaypoint(float x, float y) {
		Location loc = { x, y };
		m_locations.push_back(loc);
	}

	virtual void update(GameObject* gameObject, float deltaTime) {
		
		if (m_locations.empty())
			return;

		Location loc = m_locations[m_currentTarget];

		// get my position
		float x = 0, y = 0;
		gameObject->getPosition(&x, &y);

		// compare the two and get the distance between them
		float xDiff = loc.x - x;
		float yDiff = loc.y - y;
		float distance = sqrt(xDiff*xDiff + yDiff*yDiff);

		// if not at the target then move towards them
		if (distance > 10) {
			// need to make the difference the length of 1 (normalize)
			// this is so movement can be "pixels per second"
			xDiff /= distance;
			yDiff /= distance;

			// move to target (can overshoot!)
			gameObject->translate(xDiff * m_speed * deltaTime, yDiff * m_speed * deltaTime);
		}
		else {
			// go to next target
			if (++m_currentTarget >= m_locations.size())
				m_currentTarget = 0;
		}
	}

protected:

	struct Location {
		float x, y;
	};

	float					m_speed;
	unsigned int			m_currentTarget;
	std::vector<Location>	m_locations;
};