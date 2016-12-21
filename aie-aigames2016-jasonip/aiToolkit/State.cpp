#include "State.h"

Transition* State::getTriggeredTransition() {

	for (auto transition : m_transitions) {
		if (transition->hasTriggered())
			return transition;
	}

	return nullptr;
}

bool FiniteStateMachine::execute(GameObject* gameObject, float deltaTime) {
	if (m_currentState != nullptr) {

		Transition* transition = m_currentState->getTriggeredTransition();

		if (transition != nullptr) {

			m_currentState->onExit();

			m_currentState = transition->getTargetState();

			m_currentState->m_timer = 0;
			m_currentState->onEnter();
		}

		// accumulate time and update state
		m_currentState->m_timer += deltaTime;
		m_currentState->update(gameObject, deltaTime);

		return true;
	}

	return false;
}