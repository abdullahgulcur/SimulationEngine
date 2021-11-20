#include "input.hpp"

bool Input::keyPressed(char keyCode) {

	return ImGui::IsKeyPressed(keyCode);
}

bool Input::keyDown(char keyCode) {

	return ImGui::IsKeyDown(keyCode);
}

bool Input::keyReleased(char keyCode) {

	return ImGui::IsKeyReleased(keyCode);
}

bool Input::mouseClicked(int button) {

	return ImGui::IsMouseClicked(button);
}

bool Input::mouseDown(int button) {

	Input::mouseAnyButtonDown = true;
	return ImGui::IsMouseDown(button);
}

bool Input::mouseDoubleClicked(int button)
{
	return ImGui::IsMouseDoubleClicked(button);
}

bool Input::mouseReleased(int button)
{
	Input::mouseAnyButtonDown = false;
	return ImGui::IsMouseReleased(button);
}

float Input::mouseScroolDelta()
{
	ImGuiIO& io = ImGui::GetIO();
	return io.MouseWheel;
}

glm::vec2 Input::getMousePosition()
{
	ImVec2 pos = ImGui::GetMousePos();
	return glm::vec2(pos.x, pos.y);
}

void Input::setMousePosition(glm::vec2 mousePos)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(mousePos.x, mousePos.y);
}