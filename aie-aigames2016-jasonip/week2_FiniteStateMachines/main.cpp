#include "week2_FiniteStateMachinesApp.h"

int main() {
	
	auto app = new week2_FiniteStateMachinesApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}