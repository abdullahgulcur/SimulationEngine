#include "editor.hpp"

Editor::Editor() {
}

void Editor::startEditorScreen() {

	window.setTitle("Engine");
	window.startGLFW();
	render.startGLEW();
	window.startGLOptions();
	fileSystem.initFileSystem();

	window.setEditor(this);
	render.setEditor(this);
	editorGUI.setEditor(this);
	//editorGUI.initImGui();
	editorCamera.setWindow(window.getGLFWwindow());
	editorCamera.setMode(window.getGLFWvidmode());
	window.renderToTexture();

	model.loadModel("assets/3d/swat.obj");

	/// <summary>
	/// ///////
	/// </summary>
	/// 
	/// 
	/// 

	Material mat;
	programID = mat.LoadShaders("source/shader/tri.vs", "source/shader/tri.fs");
	//projectionMatrixID = glGetUniformLocation(programID, "P");
	//viewMatrixID = glGetUniformLocation(programID, "V");
	//modelMatrixID = glGetUniformLocation(programID, "M");
	mvpMatrixID = glGetUniformLocation(programID, "MVP");

	
}

void Editor::stayOpen() {

	window.handleCallBacks();


	editorCamera.computeMatricesFromInputs();

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);
	glViewport(0, 0, window.getGLFWvidmode()->width, window.getGLFWvidmode()->height);

	window.clear();

	editorGUI.newFrameImGui();

	glUseProgram(programID);
	glm::mat4 ProjectionMatrix = editorCamera.ProjectionMatrix;
	glm::mat4 ViewMatrix = editorCamera.ViewMatrix;
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &MVP[0][0]);

	for (int i = 0; i < model.meshes.size(); i++) {

		glBindVertexArray(model.meshes[i].VAO);
		glDrawElements(GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

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

Render Editor::getRender() { return render; }

void Editor::setWindow(Window window) { this->window = window; }

Window Editor::getWindow() { return window; }

void Editor::setEditorCamera(EditorCamera editorCamera) { this->editorCamera = editorCamera; }

EditorCamera& Editor::getEditorCamera() { return editorCamera; }