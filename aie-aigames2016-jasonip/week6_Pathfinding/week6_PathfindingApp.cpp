#include "week6_PathfindingApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

week6_PathfindingApp::week6_PathfindingApp() {

}

week6_PathfindingApp::~week6_PathfindingApp() {

}

bool week6_PathfindingApp::startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);

	m_spriteSheet.load("./textures/roguelikeSheet_transparent.png");
	m_charSpriteSheet.load("./textures/roguelikeChar_transparent.png");
	m_map.load("./map/map1.png");

	auto pixels = m_map.getPixels();
	auto channels = m_map.getFormat();

	// create nodes, using map1.png
	for (unsigned int x = 0; x < m_map.getWidth(); ++x) {
		for (unsigned int y = 0; y < m_map.getHeight(); ++y) {
			
			int index = (y * m_map.getWidth() + x) * channels;

			if (pixels[index+0] == 0 &&
				pixels[index+1] == 0 &&
				pixels[index+2] == 0) continue; // ignore it if the pixel is black
			
			if (pixels[index + 0] == 0 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 255) continue; // ignore it if the pixel is blue

			if (pixels[index + 0] == 255 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 255) continue; // ignore it if the pixel is pink
									  
			//if (rand() % 100 < 40) continue;

			MyNode* node = new MyNode();
			node->x = float(x * 20 + 10); // 20 pixel seperation starting 100 pixels in
			node->y = getWindowHeight() - float(y * 20 + 10);

			m_nodes.push_back(node);
		}
	}

	// create nodes, using rand() function
	//for (int x = 0; x < 50; ++x) {
	//	for (int y = 0; y < 25; ++y) {

	//		if (rand() % 100 < 40) continue;

	//		MyNode* node = new MyNode();
	//		node->x = float(x * 20 + 100); // 20 pixel seperation starting 100 pixels in
	//		node->y = float(y * 20 + 100);

	//		m_nodes.push_back(node);
	//	}
	//}

	// create edges/links
	for (auto a : m_nodes) {
		for (auto b : m_nodes) {
			if (a == b) continue;

			float x = b->x - a->x;
			float y = b->y - a->y;
			float sqrDist = x * x + y * y;

			if (sqrDist <= (30 * 30)) {
				Pathfinding::Edge edge;
				edge.cost = sqrDist;
				edge.target = b;

				a->edges.push_back(edge);
			}
		}
	}

	// place random medkits
	for (int i = 0; i < 5; ++i) {
		MyNode* node = m_nodes[rand() % m_nodes.size()];
		node->flags |= Pathfinding::Node::MEDKIT;
	}

	// example of a lambda object: identical to heuristicManhattan
	// square brackets represent that it's a lambda function
	auto myFunc = [](Pathfinding::Node* a, Pathfinding::Node* b) -> float{
		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;
		return (e->x - s->x) + (e->y - s->y);
	};

	// perform search
	m_start = m_nodes[rand() % m_nodes.size()];
	m_end = m_nodes[rand() % m_nodes.size()];
	
	//Pathfinding::Search::dijkstra(m_start, m_end, m_path);
	Pathfinding::Search::aStar(m_start, m_end, m_path, myFunc);

	m_player.getBlackboard().set("path", &m_path);
	m_player.getBlackboard().set("speed", 50.0f);
	m_player.setPosition(m_start->x, m_start->y);
	
	m_player.addBehaviour(&m_pathBehaviour);
	m_pathBehaviour.nodes = &m_nodes;

	return true;
}

void week6_PathfindingApp::shutdown() {

	for (auto node : m_nodes)
		delete node;

	delete m_font;
	delete m_2dRenderer;
}

void week6_PathfindingApp::update(float deltaTime) {

	m_player.update(deltaTime);

	// input example
	aie::Input* input = aie::Input::getInstance();

	if (input->wasKeyPressed(aie::INPUT_KEY_D)) {
		m_start = m_nodes[rand() % m_nodes.size()];
		m_end = m_nodes[rand() % m_nodes.size()];

		Pathfinding::Search::dijkstra(m_start, m_end, m_path);
		//Pathfinding::Search::dijkstraFindFlags(m_start, Pathfinding::Node::MEDKIT, m_path);
	}

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void week6_PathfindingApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw map
	auto pixels = m_map.getPixels();
	auto channels = m_map.getFormat();

	for (unsigned int x = 0; x < m_map.getWidth(); ++x) {
		for (unsigned int y = 0; y < m_map.getHeight(); ++y) {

			// sprite offsets for spriteSheet 
			float pw = 1.0f / m_spriteSheet.getWidth();
			float ph = 1.0f / m_spriteSheet.getHeight();
			float w = pw * 16.0f;
			float h = ph * 16.0f;

			int index = (y * m_map.getWidth() + x) * channels;

			if (pixels[index + 0] == 0 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 0) {

				// draw black thing
				m_2dRenderer->setRenderColour(1, 1, 1);
				m_2dRenderer->setUVRect((5 * w + 5 * pw), 0, w, h);
			}
			else if (pixels[index + 0] == 255 &&
				pixels[index + 1] == 255 &&
				pixels[index + 2] == 255) {

				// draw light brown thing
				m_2dRenderer->setRenderColour(1, 1, 1);
				m_2dRenderer->setUVRect(((5 * w + 5 * pw)), (2 * h + 2 * ph), w, h);
			}

			float posX = float(x * 20 + 10); // 20 pixel seperation starting 10 pixels in
			float posY = getWindowHeight() - float(y * 20 + 10);
			
			m_2dRenderer->drawSprite(&m_spriteSheet, // reference to texture, passing nullptr draws a square
				posX, posY,
				20, 20, // width, height
				0, // rotation
				10 // depth
			);
		}
	}

	// sprite offsets for charSpriteSheet 
	float pw = 1.0f / m_charSpriteSheet.getWidth();
	float ph = 1.0f / m_charSpriteSheet.getHeight();
	float w = pw * 16.0f;
	float h = ph * 16.0f;

	//draw character
	m_2dRenderer->setRenderColour(1, 1, 1);
	float x, y;
	m_player.getPosition(&x, &y);
	//m_2dRenderer->drawCircle(x, y, 5);
	m_2dRenderer->setUVRect((0 * w + 0 * pw), (11 * h + 11 * ph), w, h);
	m_2dRenderer->drawSprite(&m_charSpriteSheet, // reference to texture, passing nullptr draws a square
		x, y,
		20, 20, // width, height
		0, // rotation
		8 // depth
	);

/*	// draw character weapon, maybe only when in attackState?
	m_2dRenderer->setUVRect((46 * w + 46 * pw), (7 * h + 7 * ph), w, h);
	m_2dRenderer->drawSprite(&m_charSpriteSheet, // reference to texture, passing nullptr draws a square
		x, y,
		20, 20, // width, height
		0, // rotation
		7 // depth
	); */

/*	for (auto node : m_nodes) {

		if (node == m_start) {
			m_2dRenderer->setRenderColour(0, 1, 0);
			m_2dRenderer->drawBox(node->x, node->y, 8, 8);
		}
		else if (node == m_end) {
			m_2dRenderer->setRenderColour(0, 0, 1);
			m_2dRenderer->drawBox(node->x, node->y, 8, 8);
		}
		else {
			m_2dRenderer->setRenderColour(1, 0, 0);
			m_2dRenderer->drawBox(node->x, node->y, 4, 4);
		}


		// draw edges
		m_2dRenderer->setRenderColour(1, 1, 0);
		for (auto edge : node->edges) {
			MyNode* target = (MyNode*)edge.target;
			
			m_2dRenderer->drawLine(node->x, node->y, target->x, target->y, 1, 1);
		}

	}

	// draw path
	m_2dRenderer->setRenderColour(0, 1, 1);
	for (auto node : m_path) {

		MyNode* s = (MyNode*)node;
		MyNode* e = (MyNode*)node->previous;

		if (e != nullptr) {
			m_2dRenderer->drawLine(s->x, s->y, e->x, e->y, 3, 0);
		}
	} */

	//m_2dRenderer->setRenderColour(1, 1, 1);
	//m_2dRenderer->drawSprite(nullptr, // reference to texture, passing nullptr draws a square
	//	getWindowWidth() * 0.50f, getWindowHeight() * 0.5f,
	//	1280, 720, // width, height
	//	0, // rotation
	//	10 // depth
	//);
	
	//output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

bool PathBehaviour::execute(GameObject* gameObject, float deltaTime) {
	
	std::list<Pathfinding::Node*>* path = nullptr;
	if (gameObject->getBlackboard().get("path", &path) == false)
		return false;

	float speed = 0;
	gameObject->getBlackboard().get("speed", speed);

	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	MyNode* first = (MyNode*)path->front();

	// distance to first
	float xDiff = first->x - x;
	float yDiff = first->y - y;

	float distance = sqrt(xDiff*xDiff + yDiff*yDiff);

	// if not at the target then move towards them
	if (distance > 5) { // squared distance, so this is 5 pixels
		xDiff /= distance;
		yDiff /= distance;

		// move to target (can overshoot!)
		gameObject->translate(xDiff * speed * deltaTime, yDiff * speed * deltaTime);
	}
	else {
		// at the node, remove it and move to the next
		path->pop_front();
		// if it was the last one, pick new path
		if (path->empty()) {

			// random end node
			bool found = false;
			do {
				auto end = (*nodes)[rand() % nodes->size()];

				found = Pathfinding::Search::aStar(first, end, *path, MyNode::heuristicDistanceSqr);
			} while (found == false);
		}
	}
	return true;
}