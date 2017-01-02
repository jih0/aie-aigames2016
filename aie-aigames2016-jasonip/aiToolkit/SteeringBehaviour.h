#pragma once

#include "State.h"

struct Force {
	float x, y;
};

// abstract class
class SteeringForce {
public:

	SteeringForce() {}
	virtual ~SteeringForce() {}

	// pure virtual function
	virtual Force getForce(GameObject* gameObject) const = 0;
};

// weighted steering force struct
struct WeightedForce {
	SteeringForce* force;
	float weight;
};

// steering behaviour
class SteeringBehaviour : public Behaviour {
public:

	SteeringBehaviour() {}
	virtual ~SteeringBehaviour() {}
	
	void addForce(SteeringForce* force, float weight = 1.0f) {
		m_forces.push_back({ force, weight });
	}
	
	void setWeightForForce(SteeringForce* force, float weight) {
		for (auto& wf : m_forces) {
			if (wf.force == force)
				wf.weight = weight;
		}
	}

	virtual bool execute(GameObject* gameObject, float deltaTime) {
		
		Force force = { 0, 0 };

		for (auto& wf : m_forces) {
			Force temp = wf.force->getForce(gameObject);

			// accumulate forces
			force.x += temp.x * wf.weight;
			force.y += temp.y * wf.weight;
		}

		gameObject->addForce(force.x * deltaTime, force.y * deltaTime);
		return true;
	}
	
protected:

	std::vector<WeightedForce>	m_forces;
};

// steering behaviour state for a FSM
class SteeringState : public State {
public:

	SteeringState() {}
	virtual ~SteeringState() {}

	void addForce(SteeringForce* force, float weight = 1.0f) {
		m_forces.push_back({ force, weight });
	}

	void setWeightForForce(SteeringForce* force, float weight) {
		for (auto& wf : m_forces) {
			if (wf.force == force)
				wf.weight = weight;
		}
	}

	virtual void update(GameObject* gameObject, float deltaTime) {

		Force force = { 0, 0 };

		for (auto& wf : m_forces) {
			Force temp = wf.force->getForce(gameObject);

			// accumulate forces
			force.x += temp.x * wf.weight;
			force.y += temp.y * wf.weight;
		}

		gameObject->addForce(force.x * deltaTime, force.y * deltaTime);
	}

protected:

	std::vector<WeightedForce>	m_forces;
};

class SeekForce : public SteeringForce {
public:

	SeekForce(GameObject* target = nullptr) : m_target(nullptr) {}
	virtual ~SeekForce() {}

	void setTarget(GameObject* target) { m_target = target; };

	virtual Force getForce(GameObject* gameObject) const;
	
protected:

	GameObject* m_target;
};

class FleeForce : public SteeringForce {
public:

	FleeForce(GameObject* target = nullptr) : m_target(nullptr) {}
	virtual ~FleeForce() {}

	void setTarget(GameObject* target) { m_target = target; };

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject* m_target;
};

class PursueForce : public SteeringForce {
public:

	PursueForce(GameObject* target = nullptr) : m_target(nullptr) {}
	virtual ~PursueForce() {}

	void setTarget(GameObject* target) { m_target = target; };

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject* m_target;
};

class EvadeForce : public SteeringForce {
public:

	EvadeForce(GameObject* target = nullptr) : m_target(nullptr) {}
	virtual ~EvadeForce() {}

	void setTarget(GameObject* target) { m_target = target; };

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject* m_target;
};

class WanderForce : public SteeringForce {
public:
	
	WanderForce() {}
	virtual ~WanderForce() {}

	virtual Force getForce(GameObject* gameObject) const;

};

// circle obstacles struct
struct Obstacle {
	float x, y, r;
};

class ObstacleAvoidanceForce : public SteeringForce {
public:

	ObstacleAvoidanceForce() {}
	virtual ~ObstacleAvoidanceForce() {}

	void setFeelerLength(float length) { m_feelerLength = length; }

	void addObstacle(float x, float y, float r) {
		m_obstacles.push_back({ x, y, r });
	}

	void clearObstacles() { m_obstacles.clear(); }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	float					m_feelerLength;
	std::vector<Obstacle>	m_obstacles;
};