#include "week2_FiniteStateMachinesApp.h"
#include "Font.h"
#include "Input.h"

#include "GuardStates.h"

week2_FiniteStateMachinesApp::week2_FiniteStateMachinesApp() {

}

week2_FiniteStateMachinesApp::~week2_FiniteStateMachinesApp() {

}

bool week2_FiniteStateMachinesApp::startup() {
	
	m_2dRenderer = new aie::Renderer2D();
	
	m_font = new aie::Font("./font/consolas.ttf", 32);

	m_keyboardBehaviour.setSpeed(400);

	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);

	m_player.addBehaviour(&m_keyboardBehaviour);

	// guard
	m_enemy.addBehaviour(&m_guardFSM);

	// created new states
	auto attackState = new AttackState(&m_player, 150);
	auto idleState = new IdleState();
	auto patrolState = new PatrolState(75);

	patrolState->addWaypoint(getWindowWidth() * 0.25f, getWindowHeight() * 0.25f);
	patrolState->addWaypoint(getWindowWidth() * 0.25f, getWindowHeight() * 0.75f);
	patrolState->addWaypoint(getWindowWidth() * 0.75f, getWindowHeight() * 0.75f);
	patrolState->addWaypoint(getWindowWidth() * 0.75f, getWindowHeight() * 0.25f);
	
	// setup conditions that will trigger transition
	Condition* attackTimerCondition = new FloatGreaterCondition(attackState->getTimerPtr(), 5);
	Condition* idleTimerCondition = new FloatGreaterCondition(idleState->getTimerPtr(), 2);
	Condition* distanceCondition = new WithinRangeCondition(&m_player, &m_enemy, 200);
	auto outsideRangeCondition = new NotCondition(distanceCondition);

	// add transitions
	auto attackToIdleTransition = new Transition(idleState, attackTimerCondition);
	auto toAttackTransition = new Transition(attackState, distanceCondition);
	auto idleToPatrolTransition = new Transition(patrolState, idleTimerCondition);

	// attack to idle
	attackState->addTransition(attackToIdleTransition);

	// idle to attack
	idleState->addTransition(idleToPatrolTransition);
	idleState->addTransition(toAttackTransition);

	// patrol to attack
	patrolState->addTransition(toAttackTransition);

	// store everything in the state machine
	m_guardFSM.addState(attackState);
	m_guardFSM.addState(idleState);
	m_guardFSM.addState(patrolState);

	m_guardFSM.addCondition(distanceCondition);
	m_guardFSM.addCondition(idleTimerCondition);
	m_guardFSM.addCondition(attackTimerCondition);
	m_guardFSM.addCondition(outsideRangeCondition);

	m_guardFSM.addTransition(attackToIdleTransition);
	m_guardFSM.addTransition(toAttackTransition);
	m_guardFSM.addTransition(idleToPatrolTransition);

	m_guardFSM.setInitialState(patrolState);
	
	return true;
}

void week2_FiniteStateMachinesApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void week2_FiniteStateMachinesApp::update(float deltaTime) {

	m_player.update(deltaTime);
	m_enemy.update(deltaTime);

	// input example
	aie::Input* input = aie::Input::getInstance();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void week2_FiniteStateMachinesApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float x = 0, y = 0;

	//draw the player as a green circle
	m_player.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(x, y, 10);

	// draw the enemy as a red circle
	m_enemy.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(1, 0, 0);
	m_2dRenderer->drawCircle(x, y, 10);
	
	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}