#include "week3_SteeringBehavioursApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

week3_SteeringBehavioursApp::week3_SteeringBehavioursApp() {

}

week3_SteeringBehavioursApp::~week3_SteeringBehavioursApp() {

}

bool week3_SteeringBehavioursApp::startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);

	// set up player
	m_keyboardBehaviour.setSpeed(400);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);
	m_player.addBehaviour(&m_keyboardBehaviour);

	// set up steering properties
	m_seek.setTarget(&m_player);
	m_flee.setTarget(&m_player);

	m_steeringBehaviour.addForce(&m_seek, 0.5f);
	m_steeringBehaviour.addForce(&m_wander, 0.5f);

	// set up enemies
	for (auto& enemy : m_enemies) {
		enemy.addBehaviour(&m_steeringBehaviour);

		enemy.setMaxForce(400);
		enemy.setMaxVelocity(200);

		enemy.setWanderData(100, 75, 25);

		enemy.setPosition(	rand() % getWindowWidth(), 
							rand() % getWindowHeight());
	}

	return true;
}

void week3_SteeringBehavioursApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void week3_SteeringBehavioursApp::update(float deltaTime) {

	m_player.update(deltaTime);
	
	for (auto& enemy : m_enemies) {
		enemy.update(deltaTime);
	}
	
	// input example
	aie::Input* input = aie::Input::getInstance();
	
	//if (input->wasKeyPressed(aie::INPUT_KEY_F))
	//	m_steeringBehaviour.setForce(&m_flee);

	//if (input->wasKeyPressed(aie::INPUT_KEY_S))
	//	m_steeringBehaviour.setForce(&m_seek);

	//if (input->wasKeyPressed(aie::INPUT_KEY_W))
	//	m_steeringBehaviour.setForce(&m_wander);

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
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

	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();

	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();

	m_player.setPosition(x, y);

	// draw the enemy as a red circle
	for (auto& enemy : m_enemies) {
		enemy.getPosition(&x, &y);
		m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawCircle(x, y, 10);

		x = fmod(x, (float)getWindowWidth());
		if (x < 0)
			x += getWindowWidth();

		y = fmod(y, (float)getWindowHeight());
		if (y < 0)
			y += getWindowHeight();

		enemy.setPosition(x, y);
	}
	
	//float radius = m_enemy.getWanderRadius();
	//float offset = m_enemy.getWanderOffset();

	//float vx = 0, vy = 0;
	//m_enemy.getVelocity(&vx, &vy);
	//
	//float dist = sqrt(vx * vx + vy * vy);
	//vx /= dist;
	//vy /= dist;

	//float wanderX = 0, wanderY = 0;
	//m_enemy.getWanderTarget(&wanderX, &wanderY);

	//m_2dRenderer->setRenderColour(0, 1, 0, 0.25f);
	//m_2dRenderer->drawCircle(x, y, radius);

	//m_2dRenderer->setRenderColour(1, 0, 1);
	//m_2dRenderer->drawCircle(x + vx * offset + wanderX, y + vy * offset + wanderY, 10);

	// draw some text
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "Use arrows for movement", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}