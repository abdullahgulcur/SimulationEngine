#include "mousepick.hpp"
#include "editor.hpp"

MousePick::MousePick() {

}

void MousePick::init() {

	pickingProgramID = Shader::loadShaders("source/shader/ObjectPick.vertex", "source/shader/ObjectPick.frag");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	modelMatrixID = glGetUniformLocation(pickingProgramID, "M");
	viewMatrixID = glGetUniformLocation(pickingProgramID, "V");
	projectionMatrixID = glGetUniformLocation(pickingProgramID, "P");
}

void MousePick::detect(Editor* editor, float x, float y, float width, float height, float mouseX, float mouseY) {

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pickingProgramID);

	//glEnableVertexAttribArray(0);

	// Draw the 100 monkeys, each with a slighly different color
	for (int i = 0; i < editor->scene.meshRendererComponents.size(); i++) {

		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &editor->scene.entities[editor->scene.meshRendererComponents[i].entID].transform->model[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &editor->editorCamera.ViewMatrix[0][0]);
		glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &editor->editorCamera.ProjectionMatrix[0][0]);

		// Convert "i", the integer mesh ID, into an RGB color
		int r = (editor->scene.meshRendererComponents[i].entID & 0x000000FF) >> 0;
		int g = (editor->scene.meshRendererComponents[i].entID & 0x0000FF00) >> 8;
		int b = (editor->scene.meshRendererComponents[i].entID & 0x00FF0000) >> 16;

		// OpenGL expects colors to be in [0,1], so divide by 255.
		glUniform4f(pickingColorID, r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

		//// 1rst attribute buffer : vertices
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//// Index buffer
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		//// Draw the triangles !
		//glDrawElements(
		//	GL_TRIANGLES,      // mode
		//	indices.size(),    // count
		//	GL_UNSIGNED_SHORT,   // type
		//	(void*)0           // element array buffer offset
		//);

		glBindVertexArray(editor->scene.meshRendererComponents[i].VAO);
		glDrawElements(GL_TRIANGLES, editor->scene.meshRendererComponents[i].indiceSize, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

	//glDisableVertexAttribArray(0);


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

	if (pickedID == 0x00ffffff) { // Full white, must be the background !
		//message = "background";
		editor->editorGUI.lastSelectedEntityID = -1;
	}
	else {
		/*std::ostringstream oss;
		oss << "mesh " << pickedID;
		int x = pickedID;
		std::cout << "oruspu cocugu " << pickedID << "\n";*/
		//message = oss.str();

		editor->editorGUI.lastSelectedEntityID = pickedID;
	}


}