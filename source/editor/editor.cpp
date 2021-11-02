#include "editor.hpp"

Editor::Editor() {
}

void Editor::startEditorScreen() {

	window.setTitle("Fury");
	window.startGLFW();
	render.startGLEW();
	window.startGLOptions();
	fileSystem.initFileSystem();
	scene.setEditor(this);
	scene.initSceneGraph();

	window.setEditor(this);
	render.setEditor(this);
	editorGUI.setEditor(this);
	editorGUI.setFiles(&fileSystem.files);

	//editorGUI.setSceneList(sceneList[0]);
	editorGUI.initImGui();
	editorCamera.setWindow(window.GLFW_window);
	editorCamera.setMode(window.mode);
	window.renderToTexture();

	//scene.start();
}

void Editor::stayOpen() {

	window.handleCallBacks();

	editorCamera.computeMatricesFromInputs();

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);
	glViewport(0, 0, window.mode->width, window.mode->height);

	window.clear();

	editorGUI.newFrameImGui();

	//scene.update();

	//drawAllAxis();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	editorGUI.createPanels();
	editorGUI.renderImGui();

	window.end();
}

void Editor::drawAllAxis() {

	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((const GLfloat*)&editorCamera.ProjectionMatrix[0]);
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 MV = editorCamera.ViewMatrix * ModelMatrix;
	glLoadMatrixf((const GLfloat*)&MV[0]);

	glUseProgram(0);

	glBegin(GL_LINES);
	glm::vec3 origin(0, 0, 0);
	glColor3f(1, 0, 0);
	glVertex3fv(&origin.x);
	origin += glm::vec3(1, 0, 0) * 100.0f;
	glVertex3fv(&origin.x);
	origin -= glm::vec3(1, 0, 0) * 100.0f;
	glColor3f(0, 1, 0);
	glVertex3fv(&origin.x);
	origin += glm::vec3(0, 1, 0) * 100.0f;
	glVertex3fv(&origin.x);
	origin -= glm::vec3(0, 1, 0) * 100.0f;
	glColor3f(0, 0, 1);
	glVertex3fv(&origin.x);
	origin += glm::vec3(0, 0, 1) * 100.0f;
	glVertex3fv(&origin.x);
	glEnd();
}

void Editor::setRender(Render render) { this->render = render; }

//Render Editor::getRender() { return render; }

void Editor::setWindow(Window window) { this->window = window; }

//Window Editor::getWindow() { return window; }

void Editor::setEditorCamera(EditorCamera editorCamera) { this->editorCamera = editorCamera; }

//EditorCamera& Editor::getEditorCamera() { return editorCamera; }

//void Editor::addScene(Scene scene) { sceneList.push_back(scene); }