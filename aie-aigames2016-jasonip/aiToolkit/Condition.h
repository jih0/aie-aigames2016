#pragma once

#include "GameObject.h"

class Condition {
public:

	Condition() {}
	virtual ~Condition() {}

	virtual bool test(GameObject* gameObject) const = 0;
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

	WithinRangeCondition(const GameObject* target, float range)
		: m_target(target), m_range(range) {}
//	WithinRangeCondition(const std::vector<GameObject>* targets, float range) 
//		: m_targets(targets), m_range(range) {}
	virtual ~WithinRangeCondition() {}

	virtual bool test(GameObject* gameObject) const {

		// get my position
		float x = 0, y = 0;
		gameObject->getPosition(&x, &y);

//		for (auto& targets : *m_targets) {
			// get target position
			float tx = 0, ty = 0;
			m_target->getPosition(&tx, &ty);

			// compare the two and get the distance between them
			float xDiff = tx - x;
			float yDiff = ty - y;
			float distance = sqrt(xDiff*xDiff + yDiff*yDiff);

			if (distance <= m_range) {
				//gameObject->getBlackboard().set("target", &m_target, false);
				return true;
			}
			else { return false; }
//		}

	}
		
private:

	const std::vector<GameObject>* m_targets;
	const GameObject* m_target;
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