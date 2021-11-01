#include "application.hpp"
#include <iostream>

Application::Application() {

	std::cout << "Engine started..." << std::endl;
}


void Application::run() {

	editor.startEditorScreen();

	do {

		editor.stayOpen();
	} while (editor.window.getOpen());

	//editor.editorGUI
	//glfw 
	// IMPORTANT ----------- DESTROY ALL THE THINGS !!!!!!
}

Editor Application::getEditor() { return editor; }

void Application::setEditor(Editor editor) { this->editor = editor; }

