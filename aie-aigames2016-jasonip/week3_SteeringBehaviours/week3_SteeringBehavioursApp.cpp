#include "week3_SteeringBehavioursApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

#include "aiUtilities.h"
#include <ctime>

week3_SteeringBehavioursApp::week3_SteeringBehavioursApp() {

}

week3_SteeringBehavioursApp::~week3_SteeringBehavioursApp() {

}

bool week3_SteeringBehavioursApp::startup() {
	
	//time_t rawtime;
	//struct tm* timeinfo;

	//time(&rawtime);
	//timeinfo = localtime(&rawtime);
	//unsigned int seed = (unsigned int)timeinfo->tm_sec;

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);

	// set up player
	m_keyboardBehaviour.setSpeed(400);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);
	m_player.addBehaviour(&m_keyboardBehaviour);

	// set up steering properties
	m_seek.setTarget(&m_player);
	m_flee.setTarget(&m_player);

	m_avoid.setFeelerLength(50);

	//m_steeringBehaviour.addForce(&m_seek, 0.60f);
	//m_steeringBehaviour.addForce(&m_flee, 0.0f);

	//m_steeringBehaviour.addForce(&m_wander, 0.40f);
	//m_steeringBehaviour.addForce(&m_avoid, 2.0f);

	SteeringState* attackState = new SteeringState();
	attackState->addForce(&m_wander, 0.20f);
	attackState->addForce(&m_seek, 0.80f);
	attackState->addForce(&m_avoid, 2.0f);

	SteeringState* wanderState = new SteeringState();
	wanderState->addForce(&m_wander, 1.0f);
	wanderState->addForce(&m_avoid, 2.0f);

	auto withinRangeCondition = new WithinRangeCondition(&m_player, 200);
	auto notWithinRangeCondition = new NotCondition(withinRangeCondition);

	Transition* withinRange = new Transition(attackState, withinRangeCondition);
	Transition* notWithinRange = new Transition(wanderState, notWithinRangeCondition);

	attackState->addTransition(notWithinRange);
	wanderState->addTransition(withinRange);

	int i = 0;
	// set up enemies
	for (auto& enemy : m_enemies) {
		
		m_fsm[i].setInitialState(wanderState);
		
		enemy.addBehaviour(&m_fsm[i++]);

		Vector2* v = new Vector2();
		v->x = 0;
		v->y = 0;

		WanderData* wd = new WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->x = 0;
		wd->y = 0;

		enemy.getBlackboard().set("velocity", v, true);
		enemy.getBlackboard().set("maxForce", 300.f);
		enemy.getBlackboard().set("maxVelocity", 150.f);
		enemy.getBlackboard().set("wanderData", wd, true);

		enemy.setPosition(	float(rand() % getWindowWidth()), 
							float(rand() % getWindowHeight()));
	}

	// set up obstacles
	//srand(seed);
	for (int i = 0; i < 3; ++i)
	{
		Circle c;
		c.x = rand() % (getWindowWidth() - 100) + 50.f;
		c.y = rand() % (getWindowHeight() - 100) + 50.f;
		c.r = rand() % 40 + 40.f;

		m_obstacles.push_back(c);
		m_avoid.addObstacle(c.x, c.y, c.r, 0, 0);
	}

	return true;
}

void week3_SteeringBehavioursApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void week3_SteeringBehavioursApp::update(float deltaTime) {

	// game objects ask questions
	// game objects respond to questions
	// run arbitration

	m_player.update(deltaTime);

	for (auto& enemy : m_enemies) {
		enemy.update(deltaTime);
	}
	
	// input example
	aie::Input* input = aie::Input::getInstance();
	
	if (input->wasKeyPressed(aie::INPUT_KEY_F)) {
		m_steeringBehaviour.setWeightForForce(&m_flee, 0.6f);
		m_steeringBehaviour.setWeightForForce(&m_seek, 0.f);
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_S)) {
		m_steeringBehaviour.setWeightForForce(&m_flee, 0.f);
		m_steeringBehaviour.setWeightForForce(&m_seek, 0.6f);
	}

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void week3_SteeringBehavioursApp::screenWrap(float& x, float& y) {
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();

	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}

void week3_SteeringBehavioursApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float x = 0, y = 0;

	//draw the player as a green circle
	m_player.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(x, y, 10);

	screenWrap(x, y);
	m_player.setPosition(x, y);

//	float vx, vy;

	// draw the enemy as a red circle
	for (auto& enemy : m_enemies) {
		enemy.getPosition(&x, &y);
		m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawCircle(x, y, 10);

		screenWrap(x, y);
		enemy.setPosition(x, y);
	}
	
	float ix, iy;

	for (auto circle : m_obstacles) {
		if (rayCircleIntersection(0, 0, // start of the line
			x, y, // direction of the line
			circle.x, circle.y, circle.r, // circle to collide with
			ix, iy)) { // point of collision

			m_2dRenderer->setRenderColour(0, 1, 1);
			m_2dRenderer->drawCircle(ix, iy, 5);
		}
	}

	for (auto circle : m_obstacles) {
		m_2dRenderer->setRenderColour(1, 0, 1);
		m_2dRenderer->drawCircle(circle.x, circle.y, circle.r);
	}

	// draw some text
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "W for Wander, F for Flee, S for Seek", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}