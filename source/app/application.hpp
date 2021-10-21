#pragma once

#include "editor.hpp"

class Application {

private:

	Editor editor;

public:

	Application();

	void run();

	Editor getEditor();

	void setEditor(Editor editor);

};