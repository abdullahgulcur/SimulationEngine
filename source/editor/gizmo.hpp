#pragma once

class Editor;

class Gizmo {

private:

public:

	Editor* editor;
	unsigned int cubeVAO;
	unsigned int lineRendererProgramID;

	Gizmo(Editor* editor);

	void createGizmoVAO();

	void drawAABB_Box(const glm::mat4& MVP);
};