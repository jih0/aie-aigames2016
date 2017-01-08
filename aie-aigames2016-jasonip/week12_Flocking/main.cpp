#include "week12_FlockingApp.h"

int main() {
	
	auto app = new week12_FlockingApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}