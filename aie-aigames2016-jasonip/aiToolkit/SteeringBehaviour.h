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

class SteeringBehaviour : public Behaviour {
public:

	SteeringBehaviour() {}
	virtual ~SteeringBehaviour() {}
	
	void addForce(SteeringForce* force, float weight = 1.0f) {
		m_forces.push_back({ force, weight });
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