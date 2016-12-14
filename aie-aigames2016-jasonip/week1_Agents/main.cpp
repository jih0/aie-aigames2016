#include "week1_AgentsApp.h"

int main() {
	
	auto app = new week1_AgentsApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}