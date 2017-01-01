#pragma once

#include "GameObject.h"

class Condition {
public:

	Condition() {}
	virtual ~Condition() {}

	virtual bool test() const = 0;
};

class FloatRangeCondition : public Condition {
public:

	FloatRangeCondition(const float* value, float min, float max)
		: m_value(value), m_max(max), m_min(min) {
	}
	virtual ~FloatRangeCondition() {}

	virtual bool test() const {
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

	virtual bool test() const {
		return *m_value > m_compare;
	}
private:

	const float*	m_value;
	float			m_compare;
};

class WithinRangeCondition : public Condition {
public:

	WithinRangeCondition(const GameObject* targetA, const GameObject* targetB, float range)
		: m_targetA(targetA), m_targetB(targetB), m_range(range) {}
	virtual ~WithinRangeCondition() {}

	virtual bool test() const {
		// get target position
		float tx = 0, ty = 0;
		m_targetA->getPosition(&tx, &ty);

		// get my position
		float x = 0, y = 0;
		m_targetB->getPosition(&x, &y);

		// compare the two and get the distance between them
		float xDiff = tx - x;
		float yDiff = ty - y;
		float distance = sqrt(xDiff*xDiff + yDiff*yDiff);

		return distance <= m_range;
	}

private:

	const GameObject* m_targetA;
	const GameObject* m_targetB;
	float m_range;
};

class NotCondition : public Condition {
public:

	NotCondition(const Condition* condition) : m_condition(condition) {}
	virtual ~NotCondition() {}

	virtual bool test() const {
		return !m_condition->test();
	}

private:

	const Condition* m_condition;
};