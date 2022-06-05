#include "application.hpp"
#include <iostream>

Application::Application() {}

void Application::start() {

	do {
		editor.run();
	} while (editor.window->getOpen(&editor));
}

