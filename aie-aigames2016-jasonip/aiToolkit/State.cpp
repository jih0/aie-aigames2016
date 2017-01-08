#include "State.h"

Transition* State::getTriggeredTransition(GameObject* gameObject) {

	for (auto transition : m_transitions) {
		if (transition->hasTriggered(gameObject))
			return transition;
	}

	return nullptr;
}

bool FiniteStateMachine::execute(GameObject* gameObject, float deltaTime) {
	if (m_currentState != nullptr) {

		gameObject->getBlackboard().get("stateTimer", m_currentState->m_timer);
		Transition* transition = m_currentState->getTriggeredTransition(gameObject);

		if (transition != nullptr) {

			m_currentState->onExit(gameObject);

			m_currentState = transition->getTargetState();

//			m_currentState->m_timer = 0;
			gameObject->getBlackboard().set("stateTimer", 0.0f);
			m_currentState->onEnter(gameObject);
		}

		// accumulate time and update state
//		m_currentState->m_timer += deltaTime;
		float timer;
		gameObject->getBlackboard().get("stateTimer", timer);
		gameObject->getBlackboard().set("stateTimer", timer + deltaTime);
		m_currentState->update(gameObject, deltaTime);

		return true;
	}

	return false;
}