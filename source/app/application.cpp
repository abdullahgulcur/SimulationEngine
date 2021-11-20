#include "application.hpp"
#include <iostream>

Application::Application() {}

void Application::start() {

	editor.startEditorScreen();

	do {
		editor.run();
	} while (editor.window.getOpen(&editor));
}

