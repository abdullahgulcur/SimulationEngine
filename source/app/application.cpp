#include "application.hpp"
#include <iostream>

Application::Application() {
	editor = new Editor;
}

void Application::start() {

	do {
		editor->run();
	} while (editor->window->getOpen(editor));
}

