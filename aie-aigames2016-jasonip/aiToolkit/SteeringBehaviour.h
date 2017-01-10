#pragma once

#include "State.h"
#include "Pathfinding.h"

struct Force {
	float x, y;
};

struct WanderData {
	float offset;
	float radius;
	float jitter;
	float x, y;
	float timer;
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

class MyNode : public Pathfinding::Node {
public:

	MyNode() {}
	virtual ~MyNode() {}

	static float heuristicManhattan(Node* a, Node* b) {
		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		return (e->x - s->x) + (e->y - s->y);
	}

	static float heuristicDistanceSqr(Node* a, Node* b) {
		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		float x = e->x - s->x;
		float y = e->y - s->y;

		return (x * x) + (y * y);
	}

	static float heuristicDistance(Node* a, Node* b) {
		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		float x = e->x - s->x;
		float y = e->y - s->y;

		return sqrtf((x * x) + (y * y));
	}

	float x, y;
};

// steering 

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

//		gameObject->addForce(force.x * deltaTime, force.y * deltaTime);

		float maxVelocity = 0;
		gameObject->getBlackboard().get("maxVelocity", maxVelocity);

		Vector2* velocity = nullptr;
		gameObject->getBlackboard().get("velocity", &velocity);

		velocity->x += force.x * deltaTime;
		velocity->y += force.y * deltaTime;

		// ensure velocity is not above maximum velocity
		float magnitudeSqr = velocity->x * velocity->x + velocity->y * velocity->y;
		if (magnitudeSqr > (maxVelocity * maxVelocity)) {
			float magnitude = sqrt(magnitudeSqr);
			velocity->x = velocity->x / magnitude * maxVelocity;
			velocity->y = velocity->y / magnitude * maxVelocity;
		}

		gameObject->translate(velocity->x * deltaTime, velocity->y * deltaTime);

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

//		gameObject->addForce(force.x * deltaTime, force.y * deltaTime);

		float maxVelocity = 0;
		gameObject->getBlackboard().get("maxVelocity", maxVelocity);
		
		Vector2* velocity = nullptr;
		gameObject->getBlackboard().get("velocity", &velocity);

		velocity->x += force.x * deltaTime;
		velocity->y += force.y * deltaTime;
		
		// ensure velocity is not above maximum velocity
		float magnitudeSqr = velocity->x * velocity->x + velocity->y * velocity->y;
		if (magnitudeSqr > (maxVelocity * maxVelocity)) {
			float magnitude = sqrt(magnitudeSqr);
			velocity->x = velocity->x / magnitude * maxVelocity;
			velocity->y = velocity->y / magnitude * maxVelocity;
		}

		gameObject->translate(velocity->x * deltaTime, velocity->y * deltaTime);
	}

protected:

	std::vector<WeightedForce>	m_forces;
};

class PathBehaviour : public Behaviour {
public:

	PathBehaviour() {}
	virtual ~PathBehaviour() {}

	virtual bool execute(GameObject* gameObject, float deltaTime);

	std::vector<MyNode*>* nodes;
};

class PathState : public State {
public:

	PathState() {}
	virtual ~PathState() {}

	virtual void	onEnter(GameObject* gameObject) {}
	
	virtual void	onExit(GameObject* gameObject) {}

	virtual void update(GameObject* gameObject, float deltaTime);

	std::vector<MyNode*>* nodes;
};


class IdleForce : public SteeringForce {
public:

	IdleForce(GameObject* target = nullptr) : m_target(nullptr) {}
	virtual ~IdleForce() {}

	void setTarget(GameObject* target) { m_target = target; };

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject* m_target;
};

class SeekForce : public SteeringForce {
public:

	SeekForce(GameObject* target = nullptr) : m_target(nullptr), m_targets(nullptr) {}
	virtual ~SeekForce() {}

	void setTarget(GameObject* target) { m_target = target; }
	void setTarget(std::vector<GameObject>* vTarget) { m_targets = vTarget; }

	virtual Force getForce(GameObject* gameObject) const;
	
protected:

	GameObject*					m_target;
	std::vector<GameObject>*	m_targets;
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
	float w, h;
	enum {
		SPHERE,
		BOX
	} type;
};

class ObstacleAvoidanceForce : public SteeringForce {
public:

	ObstacleAvoidanceForce() {}
	virtual ~ObstacleAvoidanceForce() {}

	void setFeelerLength(float length) { m_feelerLength = length; }

	//void addObstacle(float x, float y, float r ) {
	//	m_obstacles.push_back({ x, y, r });
	//}

	// added 
	void addObstacle(float x, float y, float r, float w, float h) {
		if (w == 0 &&
			h == 0) {
			m_obstacles.push_back({ x, y, r, w, h, Obstacle::SPHERE });
		}
		else {
			m_obstacles.push_back({ x, y, r, w, h, Obstacle::BOX });
		}
	}

	void clearObstacles() { m_obstacles.clear(); }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	float					m_feelerLength;
	std::vector<Obstacle>	m_obstacles;
};

// Flocking Forces

class SeperationForce : public SteeringForce {
public:

	SeperationForce() {}
	virtual ~SeperationForce() {}

	void setEntities(std::vector<GameObject>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	std::vector<GameObject>*	m_entities;
	float						m_radius;
};

class CohesionForce: public SteeringForce {
public:

	CohesionForce() {}
	virtual ~CohesionForce() {}

	void setEntities(std::vector<GameObject>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	std::vector<GameObject>*	m_entities;
	float						m_radius;
};

class AlignmentForce : public SteeringForce {
public:

	AlignmentForce() {}
	virtual ~AlignmentForce() {}

	void setEntities(std::vector<GameObject>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	std::vector<GameObject>*	m_entities;
	float						m_radius;
};