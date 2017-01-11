#pragma once

#include "GameObject.h"

class Condition {
public:

	Condition() {}
	virtual ~Condition() {}

	virtual bool test(GameObject* gameObject) const = 0;
};

class DeadCondition : public Condition {
public:

	DeadCondition() {}
	virtual ~DeadCondition() {}

	virtual bool test(GameObject* gameObject) const {
		bool alive;
		gameObject->getBlackboard().get("isAlive", alive);
		return !alive;
	}
};

class TargetKilledCondition : public Condition {
public:

	TargetKilledCondition(std::vector<GameObject>* targets) { m_targets = targets; }
	virtual ~TargetKilledCondition() {}

	virtual bool test(GameObject* gameObject) const {
		bool attacking;
		gameObject->getBlackboard().get("attacking", attacking);

		if (attacking) {
			int index;
			gameObject->getBlackboard().get("targetIndex", index);

			bool tAlive;
			m_targets->at(index).getBlackboard().get("isAlive", tAlive);

			if (tAlive) {
				gameObject->getBlackboard().set("attacking", true);
				return false;
			}
			else { 
				gameObject->getBlackboard().set("attacking", false);
				return true; 
			}
		}
		else {
			return false;
		}
	}

private:
	std::vector<GameObject>*	m_targets;
};

class FloatRangeCondition : public Condition {
public:

	FloatRangeCondition(const float* value, float min, float max)
		: m_value(value), m_max(max), m_min(min) {
	}
	virtual ~FloatRangeCondition() {}

	virtual bool test(GameObject* gameObject) const {
		return(m_min <= *m_value) && (m_max >= *m_value);
	}
private:

	const float*	m_value;
	float			m_min, m_max;
};

class FloatGreaterCondition : public Condition {
public:

	FloatGreaterCondition(const float* value, float compare)
		: m_value(value), m_compare(compare) {
	}
	virtual ~FloatGreaterCondition() {}

	virtual bool test(GameObject* gameObject) const {
		return *m_value > m_compare;
	}
private:

	const float*	m_value;
	float			m_compare;
};

class WithinRangeCondition : public Condition {
public:

	WithinRangeCondition(GameObject* target, float range)
		: m_target(target), m_range(range) {}
	WithinRangeCondition(std::vector<GameObject>* targets, float range)
		: m_targets(targets), m_range(range) {}
	virtual ~WithinRangeCondition() {}

	virtual bool test(GameObject* gameObject) const {

		// get my position
		float x = 0, y = 0;
		gameObject->getPosition(&x, &y);

		bool targeted = true;
		int i = 0;
		float cDistance = 500.0f; // minimising function
		float distance = 0.0f;

		for (auto& targets : *m_targets) {
			//int health;
			//targets.getBlackboard().get("HP", health);

			bool alive;
			targets.getBlackboard().get("isAlive", alive);

			// if target is flagged as dead, then ignore
			if (!alive) {
				// if target's health is zero, skip
				//targets.getBlackboard().set("isAlive", false);
				targeted = false;
				++i;
			}
			else {
				// get target position
				float tx = 0, ty = 0;
				//m_target->getPosition(&tx, &ty);
				targets.getPosition(&tx, &ty);

				// get distance between gameObject and potential target
				distance = sqrt((tx - x)*(tx - x) + (ty - y)*(ty - y));

				//gameObject->getBlackboard().set("target", &m_target, false);
				int c = 0;
				float cx = 0, cy = 0;
				gameObject->getBlackboard().get("targetIndex", c);

				m_targets->at(c).getPosition(&cx, &cy);
				cDistance = sqrt((cx - x)*(cx - x) + (cy - y)*(cy - y)); // squared distance to the current "closest" target

				if (distance < cDistance)
				{
					// if distance to "potential" target is closer than the "closest" target, update "closest" target
					cDistance = distance;
					targeted = true;
					gameObject->getBlackboard().set("targetIndex", i);
				}
				else {}
				++i;
			}
		}

		if (targeted) {
			if (cDistance <= m_range) {
				gameObject->getBlackboard().set("attacking", true);
				return true;
			}
			else {
				gameObject->getBlackboard().set("attacking", false);
				return false;
			}
		}
		else {
			gameObject->getBlackboard().set("attacking", false);
			return false;
		}
	}

		
private:

	std::vector<GameObject>* m_targets;
	GameObject* m_target;
	float m_range;
};

class NoneWithinRangeCondition : public Condition {
public:

	NoneWithinRangeCondition(GameObject* target, float range)
		: m_target(target), m_range(range) {}
	NoneWithinRangeCondition(std::vector<GameObject>* targets, float range)
		: m_targets(targets), m_range(range) {}
	virtual ~NoneWithinRangeCondition() {}

	virtual bool test(GameObject* gameObject) const {

		// get my position
		float x = 0, y = 0;
		gameObject->getPosition(&x, &y);

		float distance = 0.0f;

		for (auto& targets : *m_targets) {
			bool alive;
			targets.getBlackboard().get("isAlive", alive);

			// if target is flagged as dead, then ignore
			if (!alive) {
				// if target's health is zero, skip
				//targets.getBlackboard().set("isAlive", false);
				//				targeted = false;
				continue;
			}
			else {
				// get target position
				float tx = 0, ty = 0;
				//m_target->getPosition(&tx, &ty);
				targets.getPosition(&tx, &ty);

				// get distance between gameObject and potential target
				distance = sqrt((tx - x)*(tx - x) + (ty - y)*(ty - y));

				if (distance < m_range)
				{
					return false;
					gameObject->getBlackboard().set("attacking", true);
				}
				else {}
			}
		}
		return true;
		gameObject->getBlackboard().set("attacking", false);
	}


private:

	std::vector<GameObject>* m_targets;
	GameObject* m_target;
	float m_range;
};

class NotCondition : public Condition {
public:

	NotCondition(const Condition* condition) : m_condition(condition) {}
	virtual ~NotCondition() {}

	virtual bool test(GameObject* gameObject) const {
		return !m_condition->test(gameObject);
	}

private:

	const Condition* m_condition;
};