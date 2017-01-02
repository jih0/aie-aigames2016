#include "week3_SteeringBehavioursApp.h"

int main() {
	
	auto app = new week3_SteeringBehavioursApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}