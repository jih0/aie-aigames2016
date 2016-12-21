#pragma once

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