#include "AssessmentApp.h"

int main() {
	
	auto app = new AssessmentApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}