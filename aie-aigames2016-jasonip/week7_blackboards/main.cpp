#include "BlackboardsApp.h"

int main() {
	
	auto app = new BlackboardsApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}