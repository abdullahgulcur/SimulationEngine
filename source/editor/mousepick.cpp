#include "mousepick.hpp"
#include "editor.hpp"

void MousePick::init() {

	pickingProgramID = ShaderNS::loadShaders("source/shader/ObjectPick.vert", "source/shader/ObjectPick.frag");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	modelMatrixID = glGetUniformLocation(pickingProgramID, "M");
	viewMatrixID = glGetUniformLocation(pickingProgramID, "V");
	projectionMatrixID = glGetUniformLocation(pickingProgramID, "P");
}

int MousePick::detectAndGetEntityId(Editor* editor, float x, float y, float width, float height, float mouseX, float mouseY) {

	glBindFramebuffer(GL_FRAMEBUFFER, editor->sceneCamera->FBO);
	glViewport(0, 0, editor->editorGUI->sceneRegion.x, editor->editorGUI->sceneRegion.y);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pickingProgramID);

	for (int i = 0; i < editor->scene->entities.size(); i++) {
	
		MeshRenderer* meshRendererComp = editor->scene->entities[i]->getComponent<MeshRenderer>();
		Transform* transform = editor->scene->entities[i]->transform;

		if (meshRendererComp != nullptr) {

			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &transform->model[0][0]);
			glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &editor->sceneCamera->ViewMatrix[0][0]);
			glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &editor->sceneCamera->ProjectionMatrix[0][0]);

			int r = (i & 0x000000FF) >> 0;
			int g = (i & 0x0000FF00) >> 8;
			int b = (i & 0x00FF0000) >> 16;

			glUniform4f(pickingColorID, r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

			glBindVertexArray(meshRendererComp->mesh->VAO);
			glDrawElements(GL_TRIANGLES, meshRendererComp->mesh->indiceSize, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);
		}
	}

	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	unsigned char data[4];
	glReadPixels(mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Convert the color back to an integer ID
	int pickedID =
		data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;

	//if (pickedID == 0x00ffffff || pickedID >= editor->scene->entities.size())
	//	editor->editorGUI.lastSelectedEntity = NULL;
	//else
	//	editor->editorGUI.lastSelectedEntity = editor->scene->entities[pickedID];

	if (pickedID == 0x00ffffff || pickedID >= editor->scene->entities.size())
		return -1;
	else
		return pickedID;
}