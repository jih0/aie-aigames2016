#pragma once

#include "Behaviour.h"
#include <vector>
#include "Condition.h"

class Transition;

// abstract class
class State {

	friend class FiniteStateMachine;

public:

	State() : m_timer(0) {}
	virtual ~State() {
		for (auto transition : m_transitions)
			delete transition;
	}

	// pure virtual
	virtual void	update(GameObject* gameObject, float deltaTime) = 0;

	// triggers for enter / exit
	virtual void	onEnter() {}
	virtual void	onExit() {}

	const float* getTimerPtr() const { return &m_timer; }
	float getTimer() const { return m_timer; }

	void addTransition(Transition* transition) {
		m_transitions.push_back(transition);
	}

	Transition* getTriggeredTransition();

protected:

	float	m_timer;
	std::vector<Transition*>	m_transitions;
};

class FiniteStateMachine : public Behaviour {
public:

	FiniteStateMachine() : m_currentState(nullptr) {}
	virtual ~FiniteStateMachine() {
		for (auto state : m_states)
			delete state;
	}

	// add new states, takes ownership
	void addState(State* state) { m_states.push_back(state); }

	// set our starting state
	void setInitialState(State* state) { if (m_currentState == nullptr) m_currentState = state; }

	virtual bool execute(GameObject* gameObject, float deltaTime);

protected:

	State* m_currentState;

	std::vector<State*> m_states;
};

class Transition {
public:

	Transition(State* target, Condition* condition) : m_target(target), m_condition(condition) {}
	~Transition() { delete m_condition; }

	State* getTargetState() { return m_target; }
	bool hasTriggered() { return m_condition->test(); }

private:

	State*		m_target;
	Condition*	m_condition;
};