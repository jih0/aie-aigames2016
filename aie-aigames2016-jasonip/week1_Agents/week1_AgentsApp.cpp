#include "week1_AgentsApp.h"
#include "Font.h"
#include "Input.h"

week1_AgentsApp::week1_AgentsApp() {

}

week1_AgentsApp::~week1_AgentsApp() {

}

bool week1_AgentsApp::startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);

	m_keyboardBehaviour.setSpeed(400);

	m_player.addBehaviour(&m_keyboardBehaviour);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);
	
	m_followBehaviour.setSpeed(100);
	m_followBehaviour.setTarget(&m_player);

	m_enemy.addBehaviour(&m_followBehaviour);
	m_enemy.setPosition(getWindowWidth() * 0.25f, getWindowHeight() * 0.25f);
	
	return true;
}

void week1_AgentsApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void week1_AgentsApp::update(float deltaTime) {

	m_player.update(deltaTime);
	m_enemy.update(deltaTime);

	// input example
	aie::Input* input = aie::Input::getInstance();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void week1_AgentsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	
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