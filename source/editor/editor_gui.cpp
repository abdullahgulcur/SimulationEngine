#include "editor.hpp"
#include "editor_gui.hpp"
#include <source/include/imgui/imgui_internal.h>

EditorGUI::EditorGUI() {}

void EditorGUI::init(Editor* editor) {

	this->editor = editor;
	EditorGUI::initImGui();
}

void EditorGUI::initImGui() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		EditorGUI::setTheme();

	ImGui_ImplGlfw_InitForOpenGL(editor->window.GLFW_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	EditorGUI::loadTextures();
}

void EditorGUI::newFrameImGui() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	EditorGUI::setTransformOperation();
}

void EditorGUI::renderImGui() {

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void EditorGUI::destroyImGui() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorGUI::setTheme()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	//colors[ImGuiCol_ChildBg] = ImVec4(0.9f, 0.00f, 0.00f,1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.8f, 0.8f, 0.8f, 1.f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.15f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.CellPadding = ImVec2(6.00f, 6.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 0;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 0;
	style.ChildRounding = 4;
	style.FrameRounding = 4;
	style.PopupRounding = 2;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;
	style.IndentSpacing = 20;

	editorColors.textSelectedColor = ImVec4(0.2f, 0.72f, 0.95f, 1.f);
	editorColors.textUnselectedColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	editorColors.textColor = editorColors.textUnselectedColor;
}

void EditorGUI::loadTextures() {

	editorIcons.openFolderTextureID = TextureNS::loadDDS("resource/icons/folder_open.DDS");
	editorIcons.closedFolderTextureID = TextureNS::loadDDS("resource/icons/folder_closed.DDS");
	editorIcons.plusTextureID = TextureNS::loadDDS("resource/icons/plus.DDS");
	editorIcons.greaterTextureID = TextureNS::loadDDS("resource/icons/greater.DDS");
	editorIcons.gameObjectTextureID = TextureNS::loadDDS("resource/icons/gameobject.DDS");
	editorIcons.transformTextureID = TextureNS::loadDDS("resource/icons/transform.DDS");
	editorIcons.meshrendererTextureID = TextureNS::loadDDS("resource/icons/meshrenderer.DDS");
	editorIcons.lightTextureID = TextureNS::loadDDS("resource/icons/light.DDS");
	editorIcons.contextMenuTextureID = TextureNS::loadDDS("resource/icons/contextMenu.DDS");
	editorIcons.eyeTextureID = TextureNS::loadDDS("resource/icons/eye.DDS");
	editorIcons.materialTextureID = TextureNS::loadDDS("resource/icons/material.DDS");
	editorIcons.materialSmallTextureID = TextureNS::loadDDS("resource/icons/materialSmall.DDS");
	editorIcons.rigidbodyTextureID = TextureNS::loadDDS("resource/icons/rigidbody.DDS");
	editorIcons.colliderTextureID = TextureNS::loadDDS("resource/icons/meshcollider.DDS");
	editorIcons.startTextureID = TextureNS::loadDDS("resource/icons/start.DDS");
	editorIcons.stopTextureID = TextureNS::loadDDS("resource/icons/stop.DDS");
	editorIcons.pauseTextureID = TextureNS::loadDDS("resource/icons/pause.DDS");
	editorIcons.pmatSmallTextureID = TextureNS::loadDDS("resource/icons/pmatsmall.DDS");
}

void EditorGUI::handleInputs() {

	std::vector<FileNode>* files = &editor->fileSystem.files;

	if (Input::mouseDoubleClicked(0)) {

		fileSystemControlVars.entered = true;

		if (EditorGUI::mouseDistanceControl()) {

			if ((*files)[fileSystemControlVars.temp_lastClickedItemID].type == FileType::folder) {

				lastClickedItemID = fileSystemControlVars.temp_lastClickedItemID;
				fileSystemControlVars.subfolderCheckFlag = false;
			}
			else
				ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[fileSystemControlVars.temp_lastClickedItemID].path).c_str(), NULL, NULL, SW_RESTORE);
		}
	}

	if (Input::mouseReleased(0)) {

		if (materialChanged) {

			if (lastSelectedEntityID != -1) {
				
				MeshRenderer* meshRendererComp = editor->scene.entities[lastSelectedEntityID].getComponent<MeshRenderer>(); 
				editor->fileSystem.writeMaterialFile(editor->fileSystem.files[meshRendererComp->mat->fileAddr->id].path, *meshRendererComp->mat); //

				meshRendererComp->mat->deleteProgram();
				meshRendererComp->mat->compileShaders(editor->fileSystem.getVertShaderPath(meshRendererComp->mat->vertShaderFileAddr),
					editor->fileSystem.getFragShaderPath(meshRendererComp->mat->fragShaderFileAddr),
					editor->scene.dirLightTransforms.size(), editor->scene.pointLightTransforms.size());
			}
			else {
				
				Material::MaterialFile& mat = editor->fileSystem.getMaterialFile(fileSystemControlVars.lastSelectedItemID);
				editor->fileSystem.writeMaterialFile(editor->fileSystem.files[mat.fileAddr->id].path, mat);

				mat.deleteProgram();
				mat.compileShaders(editor->fileSystem.getVertShaderPath(mat.vertShaderFileAddr), editor->fileSystem.getFragShaderPath(mat.fragShaderFileAddr),
					editor->scene.dirLightTransforms.size(), editor->scene.pointLightTransforms.size());
			}

			materialChanged = false;
		}

		if (physicMaterialChanged) {

			if (lastSelectedEntityID != -1) {

				//MeshRenderer* meshRendererComp = editor->scene.entities[lastSelectedEntityID].getComponent<MeshRenderer>();
				//editor->fileSystem.writeMaterialFile(editor->fileSystem.files[meshRendererComp->mat->fileAddr->id].path, *meshRendererComp->mat);
			}
			else {

				Material::PhysicMaterialFile& mat = editor->fileSystem.getPhysicMaterialFile(fileSystemControlVars.lastSelectedItemID);
				editor->fileSystem.writePhysicMaterialFile(editor->fileSystem.files[mat.fileAddr->id].path, mat);
			}

			physicMaterialChanged = false;
		}

		if (fileSystemControlVars.entered) {

			fileSystemControlVars.mouseLeftPressed = false;
			fileSystemControlVars.entered = false;
			fileSystemControlVars.panelRightItemClicked = false;
			return;
		}

		if (EditorGUI::mouseDistanceControl() && !fileSystemControlVars.toggleClicked && !fileSystemControlVars.panelRightItemClicked)
			lastClickedItemID = fileSystemControlVars.temp_lastClickedItemID;

		fileSystemControlVars.folderLineClicked = false;
		ImGui::ResetMouseDragDelta();
		fileSystemControlVars.mouseLeftPressed = false;
		fileSystemControlVars.toggleClicked = false;
		fileSystemControlVars.panelRightItemClicked = false;
		fileSystemControlVars.fileTreeClicked = false;

		entityClicked = false;
	}

	if (Input::mouseReleased(1)) {

		if (!fileSystemControlVars.panelRightItemClicked)
			fileSystemControlVars.lastSelectedItemID = -1;
	}

	if (Input::mouseClicked(0)) {

		fileSystemControlVars.mouseLeftPressed = true;

		if (!fileSystemControlVars.panelRightItemTab && !inspectorHovered)
			fileSystemControlVars.lastSelectedItemID = -1;

		fileSystemControlVars.panelRightItemTab = false;

		if (!entityClicked && !inspectorHovered && !ImGuizmo::IsUsing())
			lastSelectedEntityID = -1;

		if (!ImGuizmo::IsUsing()) {

			ImVec2 mousePos = ImGui::GetMousePos();

			float mX = mousePos.x < scenePos.x || mousePos.x > scenePos.x + sceneRegion.x ? 0 : mousePos.x - scenePos.x;
			float mY = mousePos.y < scenePos.y || mousePos.y > scenePos.y + sceneRegion.y ? 0 : mousePos.y - scenePos.y;

			mX = (mX / sceneRegion.x) * 1920;
			mY = (mY / sceneRegion.y) * 1080;

			if(mX != 0 && mY != 0)
				editor->scene.mousepick.detect(editor, scenePos.x, scenePos.y, sceneRegion.x, sceneRegion.y, mX, 1080 - mY);
		}
	}

	if (ImGui::GetIO().KeyCtrl) {

		if (ImGui::IsKeyPressed('D')) {

			if (lastSelectedEntityID != -1)
				lastSelectedEntityID = editor->scene.duplicateEntity(lastSelectedEntityID);

			if (fileSystemControlVars.lastSelectedItemID != -1)
				fileSystemControlVars.lastSelectedItemID = editor->fileSystem.duplicateFile(fileSystemControlVars.lastSelectedItemID);
		}

		if (ImGui::IsKeyPressed('S')) {

			editor->scene.saveEditorProperties();
		}
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {

		if (lastSelectedEntityID != -1) {

			editor->scene.deleteEntityCompletely(lastSelectedEntityID);
			lastSelectedEntityID = -1;
		}

		if (fileSystemControlVars.lastSelectedItemID != -1) {

			editor->fileSystem.deleteFileCompletely(fileSystemControlVars.lastSelectedItemID);
			fileSystemControlVars.lastSelectedItemID = -1;
		}
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_KeyPadEnter))) {

		if (fileSystemControlVars.lastSelectedItemID != -1 && fileSystemControlVars.renameItemID == -1) {

			if ((*files)[fileSystemControlVars.lastSelectedItemID].type == FileType::folder)
				lastClickedItemID = fileSystemControlVars.lastSelectedItemID;
			else
				ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[fileSystemControlVars.lastSelectedItemID].path).c_str(), NULL, NULL, SW_RESTORE);
		}

		if (fileSystemControlVars.renameItemID != -1)
			fileSystemControlVars.renameItemID = -1;
	}
}

void EditorGUI::mainMenuBar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5,4));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void EditorGUI::secondaryMenuBar()
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.13f, 0.13f, 0.13f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 6));
	if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", viewport, ImGuiDir_Up, height + 6, window_flags)) {
		if (ImGui::BeginMenuBar()) {
			
			float width = ImGui::GetWindowSize().x;
			ImVec2 pos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(pos.x + width / 2 - 20, pos.y + 5));

			int frame_padding = 1;
			ImVec2 size = ImVec2(16.0f, 16.0f);
			ImVec2 uv0 = ImVec2(0.0f, 0.0f);
			ImVec2 uv1 = ImVec2(1.0f, 1.0f);
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

			if (!editor->gameStarted) {

				if (ImGui::ImageButton((ImTextureID)editorIcons.startTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
					editor->gameStarted = true;
			}
			else {

				if (ImGui::ImageButton((ImTextureID)editorIcons.stopTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
					editor->gameStarted = false;
			}
			ImGui::SameLine();

			if (ImGui::ImageButton((ImTextureID)editorIcons.pauseTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col)) {

			}

			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();


	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 4));
	if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height + 4, window_flags)) {
		if (ImGui::BeginMenuBar()) {
			ImGui::Text(statusMessage.c_str());
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void EditorGUI::createPanels() {

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking; //ImGuiWindowFlags_MenuBar

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	bool p_open = true;

	ImGui::Begin("EditorDockSpace", &p_open, window_flags);
	ImGui::PopStyleVar(1);

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	EditorGUI::mainMenuBar();
	EditorGUI::secondaryMenuBar();
	EditorGUI::createInspectorPanel();
	EditorGUI::createAppPanel();
	EditorGUI::createHierarchyPanel();
	EditorGUI::createFilesPanel();
	EditorGUI::createScenePanel();
	EditorGUI::createConsolePanel();

	EditorGUI::handleInputs();

	ImGui::End();
}

//----- SCENE VIEWPORT -----

void EditorGUI::createScenePanel() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Scene");

	scenePos = ImGui::GetCursorScreenPos();
	sceneRegion = ImGui::GetContentRegionAvail();

	ImGui::Image((ImTextureID)editor->window.textureColorbuffer, ImVec2(sceneRegion.x, sceneRegion.y), ImVec2(0, 1), ImVec2(1, 0));

	if (lastSelectedEntityID != -1) {

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();

		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		glm::mat4& model = editor->scene.entities[lastSelectedEntityID].transform->model;
		ImGuizmo::Manipulate(glm::value_ptr(editor->editorCamera.ViewMatrix), glm::value_ptr(editor->editorCamera.ProjectionMatrix),
			optype, ImGuizmo::LOCAL, glm::value_ptr(model));

		if (ImGuizmo::IsUsing()) {

			int optype = 0;

			switch (this->optype) {

			case ImGuizmo::OPERATION::TRANSLATE:
				optype = 0;
				break;
			case ImGuizmo::OPERATION::ROTATE:
				optype = 1;
				break;
			case ImGuizmo::OPERATION::SCALE:
				optype = 2;
				break;
			}

			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChildTransforms();
		}
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

void EditorGUI::setTransformOperation() {

	if(ImGui::IsKeyPressed('T'))
		optype = ImGuizmo::OPERATION::TRANSLATE;

	if (ImGui::IsKeyPressed('R'))
		optype = ImGuizmo::OPERATION::ROTATE;

	if (ImGui::IsKeyPressed('S'))
		optype = ImGuizmo::OPERATION::SCALE;
}

//----- CONSOLE MENU -----

void EditorGUI::createConsolePanel() {

	ImGui::Begin("Console");

	ImGui::End();
}

//----- INSPECTOR MENU -----

void EditorGUI::createInspectorPanel() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));

	ImGui::Begin("Inspector");

	ImGui::PopStyleVar();

	ImGui::Indent(6);

	if (lastSelectedEntityID != -1) {

		EditorGUI::showEntityName();

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 5));

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.23f, 0.23f, 0.23f, 1.f));

		int index = 0;
		EditorGUI::showTransformComponent(index++);

		if (MeshRenderer* meshRendererComp = editor->scene.entities[lastSelectedEntityID].getComponent<MeshRenderer>()) {

			MaterialFile& material = *meshRendererComp->mat;
			EditorGUI::showMeshRendererComponent(meshRendererComp, index++);
			EditorGUI::showMaterialProperties(material, index++);
		}

		if (Light* lightComp = editor->scene.entities[lastSelectedEntityID].getComponent<Light>())
			EditorGUI::showLightComponent(index++);

		if (Rigidbody* rigidbodyComp = editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>())
			EditorGUI::showRigidbodyComponent(index++);

		std::vector<BoxCollider*> boxColliderCompList = editor->scene.entities[lastSelectedEntityID].getComponents<BoxCollider>();
		int boxColliderCompIndex = 0;
		for(auto& comp: boxColliderCompList)
			EditorGUI::showBoxColliderComponent(comp, index++, boxColliderCompIndex++);

		std::vector<SphereCollider*> sphereColliderCompList = editor->scene.entities[lastSelectedEntityID].getComponents<SphereCollider>();
		int sphereColliderCompIndex = 0;
		for (auto& comp : sphereColliderCompList)
			EditorGUI::showSphereColliderComponent(comp, index++, sphereColliderCompIndex++);

		std::vector<CapsuleCollider*> capsuleColliderCompList = editor->scene.entities[lastSelectedEntityID].getComponents<CapsuleCollider>();
		int capsuleColliderCompIndex = 0;
		for (auto& comp : capsuleColliderCompList)
			EditorGUI::showCapsuleColliderComponent(comp, index++, capsuleColliderCompIndex++);

		std::vector<MeshCollider*> meshColliderCompList = editor->scene.entities[lastSelectedEntityID].getComponents<MeshCollider>();
		int meshColliderCompIndex = 0;
		for (auto& comp : meshColliderCompList)
			EditorGUI::showMeshColliderComponent(comp, index++, meshColliderCompIndex++);

		ImGui::PopStyleColor(); // for the separator

		EditorGUI::addComponentButton();
	}

	if (fileSystemControlVars.lastSelectedItemID != -1 && editor->fileSystem.files[fileSystemControlVars.lastSelectedItemID].type == FileType::material)
		EditorGUI::showMaterialProperties(editor->fileSystem.getMaterialFile(fileSystemControlVars.lastSelectedItemID), 0);

	if (fileSystemControlVars.lastSelectedItemID != -1 && editor->fileSystem.files[fileSystemControlVars.lastSelectedItemID].type == FileType::physicmaterial)
		EditorGUI::showPhysicMaterialProperties(editor->fileSystem.getPhysicMaterialFile(fileSystemControlVars.lastSelectedItemID));

	ImGui::Unindent(6);

	inspectorHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem |
		ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_ChildWindows);

	ImGui::End();
}

void EditorGUI::addComponentButton() {

	ImVec2 pos = ImGui::GetCursorPos();
	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 5));

	ImGui::Indent((width - 120) / 2);
	if (ImGui::Button("Add Component", ImVec2(120, 23))) {

		pos = ImVec2(ImGui::GetCursorScreenPos().x - 25, ImGui::GetCursorScreenPos().y);
		ImGui::SetNextWindowPos(pos);
		ImGui::OpenPopup("add_component_popup");

	}
	ImGui::Unindent((width - 120) / 2);

	ImGui::SetNextWindowSize(ImVec2(180, 180));

	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.f));
	if (ImGui::BeginPopupContextItem("add_component_popup"))
	{
		if (ImGui::Selectable("   Animation")) {

		}
		ImGui::Separator();

		if (ImGui::Selectable("   Animator")) {

		}
		ImGui::Separator();

		if (ImGui::Selectable("   Collider (Box)")) {

			BoxCollider* boxColliderComp = editor->scene.entities[lastSelectedEntityID].addComponent<BoxCollider>();
			boxColliderComp->pmat = &editor->fileSystem.physicmaterials["Default"];
		}

		ImGui::Separator();

		if (ImGui::Selectable("   Collider (Capsule)")) {

			CapsuleCollider* capsuleColliderComp = editor->scene.entities[lastSelectedEntityID].addComponent<CapsuleCollider>();
			capsuleColliderComp->pmat = &editor->fileSystem.physicmaterials["Default"];
		}

		ImGui::Separator();

		if (ImGui::Selectable("   Collider (Mesh)")) {

			MeshCollider* meshColliderComp = editor->scene.entities[lastSelectedEntityID].addComponent<MeshCollider>();
			meshColliderComp->pmat = &editor->fileSystem.physicmaterials["Default"];
		}

		ImGui::Separator();

		if (ImGui::Selectable("   Collider (Sphere)")) {

			SphereCollider* sphereColliderComp = editor->scene.entities[lastSelectedEntityID].addComponent<SphereCollider>();
			sphereColliderComp->pmat = &editor->fileSystem.physicmaterials["Default"];
		}

		ImGui::Separator();

		if (ImGui::Selectable("   Light")) {

			if (Light* lightComp = editor->scene.entities[lastSelectedEntityID].addComponent<Light>()) {

				Transform* lighTransform = editor->scene.entities[lastSelectedEntityID].transform;
				editor->scene.pointLightTransforms.push_back(lighTransform);
				editor->scene.recompileAllMaterials();
			}
			else
				statusMessage = "There is existing component in the same type!";
		}

		ImGui::Separator();

		if (ImGui::Selectable("   Mesh Renderer")) {

			if (MeshRenderer* meshRendererComp = editor->scene.entities[lastSelectedEntityID].addComponent<MeshRenderer>()) {

				meshRendererComp->mesh = &editor->fileSystem.meshes["Null"];
				meshRendererComp->mat = &editor->fileSystem.materials["Default"];
			}
			else
				statusMessage = "There is existing component in the same type!";
		}
		ImGui::Separator();

		if (ImGui::Selectable("   Rigidbody")) {

			if (Rigidbody* rigidbodyComp = editor->scene.entities[lastSelectedEntityID].addComponent<Rigidbody>()) {

				//if (MeshRenderer* meshRendererComp = editor->scene.entities[lastSelectedEntityID].getComponent<MeshRenderer>())
				//	editor->physics.addConvexMesh(meshRendererComp, editor->scene.entities[lastSelectedEntityID].transform, rigidbodyComp);
				//else
				//	statusMessage = "There is no mesh renderer component attached to that entity!";
			}
			else
				statusMessage = "There is existing component in the same type!";
			
		}
		ImGui::Separator();

		if (ImGui::Selectable("   Script")) {

		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleColor();
}

void EditorGUI::showEntityName() {

	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

	ImGui::Image((ImTextureID)editorIcons.gameObjectTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();

	//static bool active = true;
	//ImGui::Checkbox("##8", &active);
	//ImGui::SameLine();

	//int len = strlen(editor->scene.entities[lastSelectedEntityID].name);
	//char* str0 = new char[len + 1];
	//strcpy(str0, editor->scene.entities[lastSelectedEntityID].name);
	//str0[len] = '\0';
	//ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	//ImVec2 textSize = ImGui::CalcTextSize(str0);

	//if (ImGui::InputText("##9", str0, IM_ARRAYSIZE(str0), input_text_flags)) {

	//	if (strlen(str0) != 0)
	//		editor->scene.renameEntity(editor->scene.entities[lastSelectedEntityID].transform->id, str0);
	//	renameEntityID = -1;
	//}
	//delete[] str0;
	//ImGui::Separator();

	char str0[32] = "";
	strcat(str0, editor->scene.entities[lastSelectedEntityID].name);
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	ImVec2 textSize = ImGui::CalcTextSize(str0);

	if (ImGui::InputText("##9", str0, IM_ARRAYSIZE(str0), input_text_flags)) {

		if (strlen(str0) != 0)
			editor->scene.renameEntity(editor->scene.entities[lastSelectedEntityID].transform->id, str0);
		renameEntityID = -1;
	}

	ImGui::Separator();
}

void EditorGUI::showTransformComponent(int index) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = {'#', '#', '0', '\0'};
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.transformTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text(" Transform");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if(ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	EditorGUI::contextMenuPopup(ComponentType::Transform, 0);

	if (treeNodeOpen) {

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Position  X"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##0", &editor->scene.entities[lastSelectedEntityID].transform->localPosition.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##1", &editor->scene.entities[lastSelectedEntityID].transform->localPosition.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##2", &editor->scene.entities[lastSelectedEntityID].transform->localPosition.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::Text("Rotation  X"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##3", &editor->scene.entities[lastSelectedEntityID].transform->localRotation.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##4", &editor->scene.entities[lastSelectedEntityID].transform->localRotation.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##5", &editor->scene.entities[lastSelectedEntityID].transform->localRotation.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::Text("Scale     X"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##6", &editor->scene.entities[lastSelectedEntityID].transform->localScale.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##7", &editor->scene.entities[lastSelectedEntityID].transform->localScale.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine(); ImGui::PushItemWidth((width - 150) / 3);

		if (ImGui::DragFloat("##8", &editor->scene.entities[lastSelectedEntityID].transform->localScale.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showMeshRendererComponent(MeshRenderer* meshRendererComp, int index) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.meshrendererTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Mesh Renderer");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::MeshRenderer, 0)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		int size_meshes = editor->fileSystem.meshes.size();
		int size_mats = editor->fileSystem.materials.size();
		const char** meshNames = new const char* [size_meshes];
		const char** meshPaths = new const char* [size_meshes];
		const char** matPaths = new const char* [size_mats];
		const char** matNames = new const char* [size_mats];

		int meshIndex = 0;
		int matIndex = 0;

		meshNames[0] = "Null";
		meshPaths[0] = "Null";

		matPaths[0] = "Default";
		matNames[0] = "Default";

		int i = 1;
		for (auto& it : editor->fileSystem.meshes) {

			if (it.second.fileAddr == NULL)
				continue;

			meshNames[i] = editor->fileSystem.files[it.second.fileAddr->id].name.c_str();
			meshPaths[i] = editor->fileSystem.files[it.second.fileAddr->id].path.c_str();

			if (meshRendererComp->mesh->fileAddr == it.second.fileAddr)
				meshIndex = i;
			i++;
		}

		ImGui::Text("Mesh        ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
		ImGui::SetNextItemWidth(width - 115);

		if (ImGui::Combo("##0", &meshIndex, meshNames, size_meshes)) {
			meshRendererComp->mesh->meshRendererCompAddrs.erase(std::remove(meshRendererComp->mesh->meshRendererCompAddrs.begin(),
				meshRendererComp->mesh->meshRendererCompAddrs.end(), meshRendererComp), meshRendererComp->mesh->meshRendererCompAddrs.end());
			meshRendererComp->mesh = &editor->fileSystem.meshes[meshPaths[meshIndex]];
			meshRendererComp->mesh->meshRendererCompAddrs.push_back(meshRendererComp);
		}

		i = 1;
		for (auto& it : editor->fileSystem.materials) {

			if (it.second.fileAddr == NULL)
				continue;

			matPaths[i] = editor->fileSystem.files[it.second.fileAddr->id].path.c_str();
			matNames[i] = editor->fileSystem.files[it.second.fileAddr->id].name.c_str();

			if (meshRendererComp->mat->fileAddr == it.second.fileAddr)
				matIndex = i;
			i++;
		}

		ImGui::Text("Material    ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width - 115);

		if (ImGui::Combo("##1", &matIndex, matNames, size_mats)) {
			meshRendererComp->mat->meshRendererCompAddrs.erase(std::remove(meshRendererComp->mat->meshRendererCompAddrs.begin(),
				meshRendererComp->mat->meshRendererCompAddrs.end(), meshRendererComp), meshRendererComp->mat->meshRendererCompAddrs.end());
			meshRendererComp->mat = &editor->fileSystem.materials[matPaths[matIndex]];
			meshRendererComp->mat->meshRendererCompAddrs.push_back(meshRendererComp);
		}
		
		delete[] meshNames;
		delete[] meshPaths;
		delete[] matPaths;
		delete[] matNames;

		ImGui::PopStyleColor();
		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showLightComponent(int index) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.lightTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Light");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::Light, 0)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));
		ImGui::Text("Type");
		ImGui::SameLine(95);
		ImGui::PushItemWidth(80);

		int item = editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->lightType == LightType::DirectionalLight ? 0 : 1;

		const char* items[] = { "Directional", "Point" };

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));

		ImGui::SetNextItemWidth(width - 180);
		if (ImGui::Combo("##0", &item, items, IM_ARRAYSIZE(items))) {

			if (editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->lightType == LightType::DirectionalLight) {

				if (item == 1) {

					editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->lightType = LightType::PointLight;
					
					for (auto it = editor->scene.dirLightTransforms.begin(); it < editor->scene.dirLightTransforms.end(); it++) {

						if ((*it) == editor->scene.entities[lastSelectedEntityID].transform) {
							editor->scene.dirLightTransforms.erase(it);
							break;
						}
					}
					editor->scene.pointLightTransforms.push_back(editor->scene.entities[lastSelectedEntityID].transform);
					editor->scene.recompileAllMaterials();
				}
			}
			else {

				if (item == 0) {

					editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->lightType = LightType::DirectionalLight;
					
					for (auto it = editor->scene.pointLightTransforms.begin(); it < editor->scene.pointLightTransforms.end(); it++) {

						if (*it == editor->scene.entities[lastSelectedEntityID].transform) {
							editor->scene.pointLightTransforms.erase(it);
							break;
						}
					}
					editor->scene.dirLightTransforms.push_back(editor->scene.entities[lastSelectedEntityID].transform);
					editor->scene.recompileAllMaterials();
				}
			}
		}

		ImGui::PopStyleColor();
		ImGui::Text("Power");
		ImGui::SameLine(95);

		float power = editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->power;
		if (ImGui::DragFloat("##1", &power, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->power = power;

		ImGui::Text("Color");
		ImGui::SameLine(95);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));

		ImVec4 color = ImVec4(editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->color.x,
			editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->color.y,
			editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->color.z, 1.f);

		ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_NoAlpha;
		static ImVec4 backup_color;
		bool open_popup = ImGui::ColorButton("##2", color);
		if (open_popup)
		{
			ImGui::OpenPopup("mypicker");
			backup_color = color;
		}
		if (ImGui::BeginPopup("mypicker"))
		{
			ImGui::ColorPicker4("##3", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->color = glm::vec3(color.x, color.y, color.z);

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showMaterialProperties(MaterialFile& material, int index) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	if(material.fileAddr == NULL)
		ImGui::SetNextItemOpen(false);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 2;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(28);
	ImGui::Image((ImTextureID)editorIcons.materialSmallTextureID, ImVec2(16.0f, 16.0f), uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Material");

	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.f));

	if (treeNodeOpen) {

		std::vector<FileNode>& files = editor->fileSystem.files;

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		{
			std::unordered_map<std::string, ShaderFile>& vertShaders = editor->fileSystem.vertShaders;
			ImGui::Text("Vert Shader");

			int indiceSize = vertShaders.size();
			const char** shaderNames = new const char* [indiceSize];
			const char** shaderPaths = new const char* [indiceSize];
			shaderNames[0] = "Default\0";
			shaderPaths[0] = "source/shader/Default.vert\0";

			const char* vertShaderPath = editor->fileSystem.getVertShaderPath(material.vertShaderFileAddr);

			int index = 0;
			int i = 1;

			for (auto& shaderFile : vertShaders) {

				if (shaderFile.second.fileAddr == NULL)
					continue;

				shaderNames[i] = files[shaderFile.second.fileAddr->id].name.c_str();
				shaderPaths[i] = files[shaderFile.second.fileAddr->id].path.c_str();

				if(shaderFile.second.fileAddr == material.vertShaderFileAddr)
					index = i;

				i++;
			}

			ImGui::SameLine(115); ImGui::SetNextItemWidth(width - 115);

			if (ImGui::Combo("##0", &index, shaderNames, indiceSize)) {

				material.vertShaderFileAddr = vertShaders[shaderPaths[index]].fileAddr;
				materialChanged = true;
			}

			delete[] shaderNames;
			delete[] shaderPaths;
		}

		{
			std::unordered_map<std::string, ShaderFile>& fragShaders = editor->fileSystem.fragShaders;
			ImGui::Text("Frag Shader");

			int indiceSize = fragShaders.size();
			const char** shaderNames = new const char* [indiceSize];
			const char** shaderPaths = new const char* [indiceSize];
			shaderNames[0] = "Default";
			shaderPaths[0] = "source/shader/Default.frag\0";

			const char* fragShaderPath = editor->fileSystem.getFragShaderPath(material.fragShaderFileAddr);;

			int index = 0;
			int i = 1;

			for (auto& shaderFile : fragShaders) {

				if (shaderFile.second.fileAddr == NULL)
					continue;

				shaderNames[i] = files[shaderFile.second.fileAddr->id].name.c_str();
				shaderPaths[i] = files[shaderFile.second.fileAddr->id].path.c_str();

				if (shaderFile.second.fileAddr == material.fragShaderFileAddr)
					index = i;

				i++;
			}

			ImGui::SameLine(115); ImGui::SetNextItemWidth(width - 115);

			bool matShaderSourceChange = false;
			if (ImGui::Combo("##1", &index, shaderNames, indiceSize)) {

				material.fragShaderFileAddr = fragShaders[shaderPaths[index]].fileAddr;
				materialChanged = true;
				matShaderSourceChange = true;
			}

			if (index != 0) {

				int sampler2DCount = fragShaders[shaderPaths[index]].sampler2DNames.size();
				int floatCount = fragShaders[shaderPaths[index]].floatNames.size();

				if (matShaderSourceChange) {

					material.textureUnitFileAddrs.clear();
					material.textureUnits.clear();
					material.floatUnits.clear();

					for (int i = 0; i < sampler2DCount; i++) {
						material.textureUnits.push_back(editor->fileSystem.textures["whitetexture"].textureID);
						material.textureUnitFileAddrs.push_back(NULL);
					}

					for (int i = 0; i < floatCount; i++)
						material.floatUnits.push_back(0);
				}

				static bool popupFlag = true;

				for (int i = 0; i < sampler2DCount; i++) {

					ImGui::PushID(i);

					if (ImGui::ImageButton((ImTextureID)material.textureUnits[i], size, uv0, uv1, frame_padding, bg_col, tint_col)) {
						ImGui::OpenPopup("texture_menu_popup");
						popupFlag = true;
					}

					EditorGUI::textureMenuPopup(material, i, popupFlag);

					ImGui::PopID();
					ImGui::SameLine();
					pos = ImGui::GetCursorPos(); ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));
					ImGui::Text(fragShaders[shaderPaths[index]].sampler2DNames[i].c_str());
				}

				for (int i = 0; i < floatCount; i++) {

					ImGui::Text(fragShaders[shaderPaths[index]].floatNames[i].c_str());

					char temp[8];
					sprintf(temp, "##%d\0", i + 10);

					float val = material.floatUnits[i];
					if (ImGui::DragFloat(temp, &val, 0.1f, 0.0f, 0.0f, "%.2f"))
						materialChanged = true;
				}
			}
			else {

				if (matShaderSourceChange) {

					material.textureUnits.clear();
					material.textureUnitFileAddrs.clear();
					material.floatUnits.clear();
				}
			}

			delete[] shaderNames;
			delete[] shaderPaths;
		}

		//ImVec4 color = ImVec4(material.albedoColor.x, material.albedoColor.y, material.albedoColor.z, 1.0f);
		//ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_NoAlpha;
		//static ImVec4 backup_color;
		//bool open_popup = ImGui::ColorButton("##0", color);
		//if (open_popup)
		//{
		//	ImGui::OpenPopup("mypicker");
		//	backup_color = color;
		//}
		//if (ImGui::BeginPopup("mypicker"))
		//{
		//	ImGui::ColorPicker4("##1", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
		//	material.albedoColor = glm::vec3(color.x, color.y, color.z);
		//	materialChanged = true;
		//	ImGui::EndPopup();
		//}

		ImGui::TreePop();
	}

	ImGui::PopStyleColor();
	ImGui::Separator();
}

void EditorGUI::showRigidbodyComponent(int index) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.rigidbodyTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Rigidbody");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::Rigidbody, 0)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Mass                ");
		ImGui::SameLine();
		ImGui::DragFloat("##0", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->mass, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Text("Use Gravity         ");
		ImGui::SameLine();
		ImGui::Checkbox("##1", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->useGravity);

		ImGui::Text("Is Kinematic        ");
		ImGui::SameLine();
		ImGui::Checkbox("##2", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->isKinematic);

		ImGui::Text("Freeze Position    X");
		ImGui::SameLine();
		ImGui::Checkbox("##3", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->freezePos.x);
		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
		ImGui::Checkbox("##4", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->freezePos.y);
		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
		ImGui::Checkbox("##5", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->freezePos.z);

		ImGui::Text("Freeze Rotation    X");
		ImGui::SameLine();
		ImGui::Checkbox("##6", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->freezeRot.x);
		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
		ImGui::Checkbox("##7", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->freezeRot.y);
		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
		ImGui::Checkbox("##8", &editor->scene.entities[lastSelectedEntityID].getComponent<Rigidbody>()->freezeRot.z);

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showPhysicMaterialProperties(PhysicMaterialFile& mat) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	bool treeNodeOpen = ImGui::TreeNode("##0");

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.pmatSmallTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Physic Material");

	if (treeNodeOpen) {

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Dynamic Friction    "); ImGui::SameLine();
		float dynamicFriction = mat.pxmat->getDynamicFriction(); ImGui::PushItemWidth(width - 180);
		if (ImGui::DragFloat("##0", &dynamicFriction, 0.1f, 0.0f, 0.0f, "%.2f")) {

			physicMaterialChanged = true;
			mat.pxmat->setDynamicFriction(dynamicFriction);
		}

		ImGui::Text("Static Friction     "); ImGui::SameLine();
		float staticFriction = mat.pxmat->getStaticFriction(); ImGui::PushItemWidth(width - 180);
		if (ImGui::DragFloat("##1", &staticFriction, 0.1f, 0.0f, 0.0f, "%.2f")) {

			physicMaterialChanged = true;
			mat.pxmat->setStaticFriction(staticFriction);
		}

		ImGui::Text("Restitution         "); ImGui::SameLine();
		float restitution = mat.pxmat->getRestitution(); ImGui::PushItemWidth(width - 180);
		if (ImGui::DragFloat("##2", &restitution, 0.1f, 0.0f, 0.0f, "%.2f")) {
			
			physicMaterialChanged = true;
			mat.pxmat->setRestitution(restitution);
		}
	
		ImGui::Text("Friction Combine    "); ImGui::SameLine();
		int fcombineMode = mat.pxmat->getFrictionCombineMode();

		const char* items[] = { "Average", "Minimum", "Multiply", "Maximum" };
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));

		ImGui::SetNextItemWidth(width - 180);
		if (ImGui::Combo("##3", &fcombineMode, items, IM_ARRAYSIZE(items))) {

			physicMaterialChanged = true;
			mat.pxmat->setFrictionCombineMode(static_cast<PxCombineMode::Enum>(fcombineMode));
		}

		ImGui::Text("Restitution Combine "); ImGui::SameLine();
		int rcombineMode = mat.pxmat->getRestitutionCombineMode();

		ImGui::SetNextItemWidth(width - 180);
		if (ImGui::Combo("##4", &rcombineMode, items, IM_ARRAYSIZE(items))) {

			physicMaterialChanged = true;
			mat.pxmat->setRestitutionCombineMode(static_cast<PxCombineMode::Enum>(rcombineMode));
		}

		ImGui::PopStyleColor();
		ImGui::TreePop();
	}

	ImGui::Separator();
}

template<class T>
void EditorGUI::showColliderPhysicMaterial(T* colliderComp, float windowWidth, int frameIndex) {

	ImGui::Text("Material    "); ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.f));
	ImGui::SameLine(); ImGui::SetNextItemWidth(windowWidth - 115);

	std::unordered_map<std::string, PhysicMaterialFile>& physicmaterials = editor->fileSystem.physicmaterials;
	int indiceSize = physicmaterials.size();
	const char** pmatNames = new const char* [indiceSize];
	const char** pmatPaths = new const char* [indiceSize];
	pmatNames[0] = "Default";
	pmatPaths[0] = "Default";

	Collider* collider = dynamic_cast<Collider*>(colliderComp);
	const char* pmatPath = editor->fileSystem.getPhysicMaterialPath(collider->pmat->fileAddr);

	int index = 0;
	int i = 1;

	for (auto& pmatFile : physicmaterials) {

		if (pmatFile.second.fileAddr == NULL)
			continue;

		pmatNames[i] = editor->fileSystem.files[pmatFile.second.fileAddr->id].name.c_str();
		pmatPaths[i] = editor->fileSystem.files[pmatFile.second.fileAddr->id].path.c_str();

		if (pmatFile.second.fileAddr == collider->pmat->fileAddr)
			index = i;

		i++;
	}

	ImGui::SameLine(115); ImGui::SetNextItemWidth(windowWidth - 115);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + frameIndex;
	str[2] = indexChar;

	if (ImGui::Combo(str, &index, pmatNames, indiceSize)) {

		collider->pmat->colliderCompAddrs.erase(std::remove(collider->pmat->colliderCompAddrs.begin(),
			collider->pmat->colliderCompAddrs.end(), collider), collider->pmat->colliderCompAddrs.end());
		collider->pmat = &physicmaterials[pmatPaths[index]];
		collider->pmat->colliderCompAddrs.push_back(collider);
		physicMaterialChanged = true;
	}

	delete[] pmatNames;
	delete[] pmatPaths;

	ImGui::PopStyleColor();
}

void EditorGUI::showMeshColliderComponent(MeshCollider* meshColliderComp, int index, int meshColliderIndex) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.colliderTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Mesh Collider");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::MeshCollider, meshColliderIndex)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Trigger     "); ImGui::SameLine();
		ImGui::Checkbox("##0", &meshColliderComp->trigger);

		ImGui::Text("Convex      "); ImGui::SameLine();
		ImGui::Checkbox("##1", &meshColliderComp->convex);

		EditorGUI::showColliderPhysicMaterial(meshColliderComp, width, 2);

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showBoxColliderComponent(BoxCollider* boxColliderComp, int index, int boxColliderIndex) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.colliderTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Box Collider");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::BoxCollider, boxColliderIndex)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Trigger     "); ImGui::SameLine();
		ImGui::Checkbox("##0", &boxColliderComp->trigger);

		EditorGUI::showColliderPhysicMaterial(boxColliderComp, width, 1);

		ImGui::Text("Center     X"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##2", &boxColliderComp->center.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##3", &boxColliderComp->center.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##4", &boxColliderComp->center.z, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Text("Size       X"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##5", &boxColliderComp->size.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##6", &boxColliderComp->size.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##7", &boxColliderComp->size.z, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showSphereColliderComponent(SphereCollider* sphereColliderComp, int index, int sphereColliderIndex) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.colliderTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Sphere Collider");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::SphereCollider, sphereColliderIndex)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Trigger     "); ImGui::SameLine();
		ImGui::Checkbox("##0", &sphereColliderComp->trigger);

		EditorGUI::showColliderPhysicMaterial(sphereColliderComp, width, 1);

		ImGui::Text("Center     X"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##2", &sphereColliderComp->center.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##3", &sphereColliderComp->center.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##4", &sphereColliderComp->center.z, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Text("Radius     X"); ImGui::SameLine();
		ImGui::DragFloat("##5", &sphereColliderComp->radius, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showCapsuleColliderComponent(CapsuleCollider* capsuleColliderComp, int index, int capsuleColliderIndex) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	char str[4] = { '#', '#', '0', '\0' };
	char indexChar = '0' + index;
	str[2] = indexChar;
	bool treeNodeOpen = ImGui::TreeNode(str);

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)editorIcons.colliderTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Capsule Collider");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)editorIcons.contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::CapsuleCollider, capsuleColliderIndex)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Trigger     "); ImGui::SameLine();
		ImGui::Checkbox("##0", &capsuleColliderComp->trigger);

		EditorGUI::showColliderPhysicMaterial(capsuleColliderComp, width, 1);

		ImGui::Text("Center     X"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##2", &capsuleColliderComp->center.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##3", &capsuleColliderComp->center.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
		ImGui::PushItemWidth((width - 155) / 3);
		ImGui::DragFloat("##4", &capsuleColliderComp->center.z, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Text("Radius     X"); ImGui::SameLine();
		ImGui::DragFloat("##5", &capsuleColliderComp->radius, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::Text("Height     X"); ImGui::SameLine();
		ImGui::DragFloat("##6", &capsuleColliderComp->height, 0.1f, 0.0f, 0.0f, "%.2f");

		const char** axisNames = new const char* [3];
		axisNames[0] = "X Axis\0";
		axisNames[1] = "Y Axis\0";
		axisNames[2] = "Z Axis\0";

		ImGui::Text("Axis        "); ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.f));
		ImGui::SameLine(); ImGui::SetNextItemWidth(width - 115);
		if (ImGui::Combo("##7", &capsuleColliderComp->axis, axisNames, 3)) {

		}
		ImGui::PopStyleColor();
		delete[] axisNames;

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::textureMenuPopup(MaterialFile& material, int index, bool& flag) {

	if (!flag)
		return;

	int frame_padding = 2;
	ImVec2 size = ImVec2(128.0f, 128.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SetNextWindowSize(ImVec2(300, 635));

	if (ImGui::BeginPopup("texture_menu_popup"))
	{
		unsigned int iterateIndex = 0;
		for (auto& it : editor->fileSystem.textures) {

			ImGui::BeginGroup();
			{
				ImGui::PushID(iterateIndex);

				if (ImGui::ImageButton((ImTextureID)it.second.textureID, size, uv0, uv1, frame_padding, bg_col, tint_col)) {

					material.textureUnits[index] = it.second.textureID;
					material.textureUnitFileAddrs[index] = it.second.fileAddr;
					

					materialChanged = true;
					flag = false;
					ImGui::PopID();
					ImGui::EndGroup();
					ImGui::EndPopup();
					return;
				}

				char name[32];
				if (it.second.fileAddr != NULL)
					strcpy(name, editor->fileSystem.files[it.second.fileAddr->id].name.c_str());
				else
					strcpy(name, "White\0");

				ImVec2 pos = ImGui::GetCursorPos();
				ImVec2 textSize = ImGui::CalcTextSize(name);
				ImGui::SetCursorPos(ImVec2(pos.x + (128.f - textSize.x) / 2, pos.y));

				ImGui::Text(name);

				ImGui::PopID();
			}
			ImGui::EndGroup();

			if ((iterateIndex + 1) % 2 != 0)
				ImGui::SameLine();

			iterateIndex++;
		}
		ImGui::EndPopup();
	}
}

bool EditorGUI::contextMenuPopup(ComponentType type, int index) {

	ImGui::SetNextWindowSize(ImVec2(180, 95));

	if (ImGui::BeginPopup("context_menu_popup"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

		if (ImGui::Selectable("   Copy Values")) {

		}

		if (ImGui::Selectable("   Paste Values")) {

		}

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
		ImGui::Dummy(ImVec2(0, 1));

		if (ImGui::Selectable("   Reset")) {

		}

		if (ImGui::Selectable("   Remove")) {

			switch (type) {
			case ComponentType::Animation : {
				break;
			}
			case ComponentType::Animator : {
				break;
			}
			case ComponentType::BoxCollider : {

				BoxCollider* boxColliderComp = editor->scene.entities[lastSelectedEntityID].getComponents<BoxCollider>()[index];
				editor->scene.entities[lastSelectedEntityID].removeComponent(boxColliderComp);
				break;
			}
			case ComponentType::CapsuleCollider: {

				CapsuleCollider* capsuleColliderComp = editor->scene.entities[lastSelectedEntityID].getComponents<CapsuleCollider>()[index];
				editor->scene.entities[lastSelectedEntityID].removeComponent(capsuleColliderComp);
				break;
			}
			case ComponentType::Light: {

				LightType type = editor->scene.entities[lastSelectedEntityID].getComponent<Light>()->lightType;
				editor->scene.entities[lastSelectedEntityID].removeComponent<Light>();

				if (type == LightType::PointLight) {

					for (auto it = editor->scene.pointLightTransforms.begin(); it < editor->scene.pointLightTransforms.end(); it++) {

						if (*it == editor->scene.entities[lastSelectedEntityID].transform) {
							editor->scene.pointLightTransforms.erase(it);
							break;
						}
					}
				}
				else {

					for (auto it = editor->scene.dirLightTransforms.begin(); it < editor->scene.dirLightTransforms.end(); it++) {

						if (*it == editor->scene.entities[lastSelectedEntityID].transform) {
							editor->scene.dirLightTransforms.erase(it);
							break;
						}
					}
				}
				editor->scene.recompileAllMaterials();

				break;
			}
			case ComponentType::MeshCollider: {

				MeshCollider* meshColliderComp = editor->scene.entities[lastSelectedEntityID].getComponents<MeshCollider>()[index];
				editor->scene.entities[lastSelectedEntityID].removeComponent(meshColliderComp);
				break;
			}
			case ComponentType::MeshRenderer: {
				editor->scene.entities[lastSelectedEntityID].removeComponent<MeshRenderer>();
				break;
			}
			case ComponentType::Script: {
				break;
			}
			case ComponentType::SphereCollider: {

				SphereCollider* sphereColliderComp = editor->scene.entities[lastSelectedEntityID].getComponents<SphereCollider>()[index];
				editor->scene.entities[lastSelectedEntityID].removeComponent(sphereColliderComp);
				break;
			}
			case ComponentType::Rigidbody: {
				//editor->scene.entities[lastSelectedEntityID].removeComponent<Rigidbody>();
				break;
			}
			}

			ImGui::PopStyleColor();
			ImGui::EndPopup();

			return true;
		}

		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}

	return false;
}

void EditorGUI::createAppPanel() {

	ImVec2 mousePos = ImGui::GetMousePos();

	ImGui::Begin("Statistics");

	ImGui::Text("App average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Mouse x: %.1f y: %.1f", mousePos.x, mousePos.y);

	float mX = mousePos.x < scenePos.x || mousePos.x > scenePos.x + sceneRegion.x ? 0 : mousePos.x - scenePos.x;
	float mY = mousePos.y < scenePos.y || mousePos.y > scenePos.y + sceneRegion.y ? 0 : mousePos.y - scenePos.y;
	
	mX = (mX / sceneRegion.x) * 1920;
	mY = (mY / sceneRegion.y) * 1080;

	ImGui::Text("Mouse (Scene) x: %.1f y: %.1f", mX, mY);

	ImGui::End();
}

//----- HIERARCHY MENU -----

void EditorGUI::createHierarchyPanel() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	ImGui::Begin("Hierarchy");
	ImGui::PopStyleVar();

	EditorGUI::hiearchyCreateButton();
	
	ImVec2 size = ImGui::GetWindowSize();

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f));

	ImVec2 scrolling_child_size = ImVec2(size.x - 10, size.y - 54);
	ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);

	ImGui::SetNextItemOpen(true);

	bool treeNodeOpen = ImGui::TreeNode("##0");

	ImVec2 imgsize = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

	ImGui::SameLine(20);
	ImGui::Image((ImTextureID)editorIcons.eyeTextureID, imgsize, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text(editor->scene.name.c_str());

	if (treeNodeOpen)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE_ENTITY"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				editor->scene.moveEntity(payload_n, 0);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Unindent(15);
		EditorGUI::createSceneGraphRecursively(editor->scene.entities[0].transform);
		ImGui::TreePop();
	}

	ImGui::EndChild();

	ImGui::End();	
}

void EditorGUI::createSceneGraphRecursively(Transform* transform) {

	static bool showChildren = true;

	for (int i = 0; i < transform->children.size(); i++) {

		ImGui::Indent(15);

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool hasChildren = transform->children[i]->children.size() > 0;
		if (!hasChildren)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		else
			node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (lastSelectedEntityID != -1 && editor->scene.subEntityCheck(editor->scene.entities[lastSelectedEntityID].transform, transform->children[i])) {

			if (showChildren)
				ImGui::SetNextItemOpen(true);
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)transform->children[i]->id, node_flags, ""); // " %d", transform->children[i]->id

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {

			showChildren = false;
			entityClicked = true;

			if (!ImGui::IsItemToggledOpen())
				lastSelectedEntityID = transform->children[i]->id;
			else
				fileSystemControlVars.toggleClicked = true;
		}

		ImGui::PushID(transform->children[i]->id);
		ImGui::SetNextWindowSize(ImVec2(210, 95));

		if (ImGui::BeginPopupContextItem("scene_graph_popup"))
		{
			entityClicked = true;
			lastSelectedEntityID = transform->children[i]->id;

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

			if (ImGui::Selectable("   Create Empty")) {

				lastSelectedEntityID = editor->scene.newEntity(transform->children[i]->id, "Entity")->id;
				showChildren = true;

				ImGui::PopStyleColor();
				ImGui::EndPopup();
				ImGui::PopID();
				return;
			}

			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
			ImGui::Dummy(ImVec2(0, 1));

			if (ImGui::Selectable("   Rename")) {

				renameEntityID = transform->children[i]->id;

				ImGui::PopStyleColor();
				ImGui::EndPopup();
				ImGui::PopID();
				return;
			}

			if (ImGui::Selectable("   Duplicate")) {

				lastSelectedEntityID = editor->scene.duplicateEntity(transform->children[i]->id);

				ImGui::PopStyleColor();
				ImGui::EndPopup();
				ImGui::PopID();
				return;
			}

			if (ImGui::Selectable("   Delete")) {

				editor->scene.deleteEntityCompletely(lastSelectedEntityID);
				lastSelectedEntityID = -1;

				ImGui::PopStyleColor();
				ImGui::EndPopup();
				ImGui::PopID();
				return;
			}

			ImGui::PopStyleColor();
			ImGui::EndPopup();
		}
		ImGui::PopID();

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE_ENTITY", &transform->children[i]->id, sizeof(int));
			ImGui::Text(editor->scene.entities[transform->children[i]->id].name);
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE_ENTITY"))
			{
				showChildren = true;
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				editor->scene.moveEntity(payload_n, transform->children[i]->id);
				return;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();

		ImVec2 size = ImVec2(16.0f, 16.0f);
		ImVec2 uv0 = ImVec2(0.0f, 0.0f);
		ImVec2 uv1 = ImVec2(1.0f, 1.0f);
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

		ImGui::Image((ImTextureID)editorIcons.gameObjectTextureID, size, uv0, uv1, tint_col, border_col);

		ImGui::SameLine();

		if (fileSystemControlVars.lastSelectedItemID == transform->children[i]->id)
			editorColors.textColor = editorColors.textSelectedColor;
		else
			editorColors.textColor = editorColors.textUnselectedColor;

		ImVec2 pos = ImGui::GetCursorPos();

		if (renameEntityID == transform->children[i]->id) {

			char temp[3] = { '#','#', '\0' };
			char str0[32] = "";
			strcat(str0, editor->scene.entities[transform->children[i]->id].name);
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
			ImGui::SetKeyboardFocusHere(0);
			ImVec2 textSize = ImGui::CalcTextSize(str0);
			ImGui::SetCursorPos(ImVec2(pos.x - 5, pos.y - 2));

			if (ImGui::InputText(temp, str0, IM_ARRAYSIZE(str0), input_text_flags)) {

				if (strlen(str0) != 0)
					editor->scene.renameEntity(transform->children[i]->id, str0);
				renameEntityID = -1;
			}
		}
		else {

			if (lastSelectedEntityID == transform->children[i]->id)
				ImGui::TextColored(editorColors.textSelectedColor, editor->scene.entities[transform->children[i]->id].name);
			else
				ImGui::Text(editor->scene.entities[transform->children[i]->id].name);
		}

		if (nodeOpen && hasChildren)
			EditorGUI::createSceneGraphRecursively(transform->children[i]);

		ImGui::Unindent(15);
	}
}

void EditorGUI::hiearchyCreateButton() {

	if (ImGui::Button("Create", ImVec2(60, 20)))
		ImGui::OpenPopup("context_menu_scene_hierarchy_popup");

	ImGui::SetNextWindowSize(ImVec2(210, 100));

	if (ImGui::BeginPopup("context_menu_scene_hierarchy_popup"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

		if (ImGui::Selectable("   Entity")) {

			lastSelectedEntityID = editor->scene.newEntity(0, "Entity")->id;
			// include all the necessary end codes...
			ImGui::PopStyleColor();
			ImGui::EndPopup();
			return;
		}

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
		ImGui::Dummy(ImVec2(0, 1));

		if (ImGui::Selectable("   Sun")) {

			lastSelectedEntityID = editor->scene.newLight(0, "Sun", LightType::DirectionalLight);
			// include all the necessary end codes...
			ImGui::PopStyleColor();
			ImGui::EndPopup();
			return;
		}

		if (ImGui::Selectable("   Point Light")) {

			lastSelectedEntityID = editor->scene.newLight(0, "Point_Light", LightType::PointLight);
			// include all the necessary end codes...
			ImGui::PopStyleColor();
			ImGui::EndPopup();
			return;
		}

		p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
		ImGui::Dummy(ImVec2(0, 1));

		if (ImGui::Selectable("   Scene")) {

			//editor->addScene(Scene());

			// include all the necessary end codes...
			ImGui::PopStyleColor();
			ImGui::EndPopup();
			return;
		}

		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}

}

//----- FILE MENU -----

void EditorGUI::createFilesPanel() {

	static float lastMouseX;
	float mouseX = ImGui::GetMousePos().x;
	float mouseDeltaX = mouseX - lastMouseX;
	lastMouseX = mouseX;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

	ImGui::Begin("Files");

	ImGui::PopStyleVar();

	ImGui::Button("Create", ImVec2(60, 20));

	ImGui::SameLine(foldersPanelLeftPartWidth + 23);
	
	EditorGUI::showCurrentDirectoryText();

	ImVec2 size = ImGui::GetWindowSize();
	ImVec2 scrolling_child_size = ImVec2(foldersPanelLeftPartWidth, size.y - 54);

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f));

	ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(2);

	ImGui::SetNextItemOpen(true);

	if (ImGui::TreeNode("MyProject"))
	{
		ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
		EditorGUI::createFoldersRecursively(editor->fileSystem.rootFile);
		ImGui::TreePop();
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::Button(" ", ImVec2(3, ImGui::GetWindowSize().y - 53));

	if (ImGui::IsItemClicked())
		fileSystemControlVars.folderLineClicked = true;

	if (size.x > 400) {
		if (fileSystemControlVars.folderLineClicked) {

			if (foldersPanelLeftPartWidth > 200)
				foldersPanelLeftPartWidth += mouseDeltaX / 2;
			else
				foldersPanelLeftPartWidth = 200;

			if (size.x - foldersPanelLeftPartWidth >= 200)
				foldersPanelLeftPartWidth += mouseDeltaX / 2;
			else
				foldersPanelLeftPartWidth = size.x - 200;
		}
	}
	else
		foldersPanelLeftPartWidth = size.x / 2;

	ImGui::SameLine();
	ImVec2 scrolling_child_size_r = ImVec2(size.x - scrolling_child_size.x - 26, ImGui::GetWindowSize().y - 54);

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f));
	ImGui::BeginChild("scrolling_right", scrolling_child_size_r, true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);

	EditorGUI::createFilesPanelRightPart(scrolling_child_size_r);

	ImGui::EndChild();

	ImGui::End();
}

void EditorGUI::showCurrentDirectoryText() {

	if (lastClickedItemID == -1)
		return;

	std::vector<FileNode>* files = &editor->fileSystem.files;

	std::stack<File*>fileStack;
	File* iter = (*files)[lastClickedItemID].addr;

	if ((*files)[iter->id].type != FileType::folder)
		iter = iter->parent;

	while (iter->parent != NULL) {

		fileStack.push((*files)[iter->id].addr);
		iter = iter->parent;
	}
	while (fileStack.size() > 1) {

		File* popped = fileStack.top();
		fileStack.pop();

		if (fileSystemControlVars.temp_lastClickedItemID == popped->id && fileSystemControlVars.mouseLeftPressed && !fileSystemControlVars.fileTreeClicked)
			editorColors.textColor = editorColors.textSelectedColor;
		else
			editorColors.textColor = editorColors.textUnselectedColor;

		ImGui::TextColored(editorColors.textColor, (*files)[popped->id].name.c_str());
		if (ImGui::IsItemClicked()) {
			cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
			fileSystemControlVars.temp_lastClickedItemID = popped->id;
		}
		ImGui::SameLine(0, 0);

		ImVec2 uv_min = ImVec2(0.0f, 0.0f);
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImGui::Image((ImTextureID)editorIcons.greaterTextureID, ImVec2(16.f, 16.f), uv_min, uv_max, tint_col, border_col);

		ImGui::SameLine(0, 0);
	}
	File* popped = fileStack.top();
	fileStack.pop();

	ImGui::Text((*files)[popped->id].name.c_str());
	if (ImGui::IsItemClicked()) {
		cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
		fileSystemControlVars.temp_lastClickedItemID = popped->id;
	}

	if (fileSystemControlVars.lastSelectedItemID != -1 && (*files)[fileSystemControlVars.lastSelectedItemID].type != FileType::folder) {

		ImGui::SameLine();
		char line[] = " | \0";
		char fileName[32];
		strcpy(fileName, line);
		strcat(fileName, (*files)[fileSystemControlVars.lastSelectedItemID].name.c_str());
		strcat(fileName, (*files)[fileSystemControlVars.lastSelectedItemID].extension.c_str());
		ImGui::Text(fileName);
	}
}

void EditorGUI::createFoldersRecursively(File* file) {

	std::vector<FileNode>* files = &editor->fileSystem.files;

	for (int i = 0; i < file->subfiles.size(); i++) {

		if ((*files)[file->subfiles[i]->id].type != FileType::folder)
			continue;

		ImGui::Indent(ImGui::GetStyle().IndentSpacing);

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool hasSubFolder = editor->fileSystem.hasSubFolder(file->subfiles[i]);
		if (!hasSubFolder)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		else
			node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (editor->fileSystem.subfolderCheck((*files)[lastClickedItemID].addr, file->subfiles[i])) {

			if (!fileSystemControlVars.subfolderCheckFlag)
				ImGui::SetNextItemOpen(true);
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)file->subfiles[i]->id, node_flags, ""); //, " %d", file->subfiles[i]->id

		if (ImGui::IsItemClicked()) {

			fileSystemControlVars.fileTreeClicked = true;
			fileSystemControlVars.subfolderCheckFlag = true;
			cursorPosWhenFirstClickedItem = ImGui::GetMousePos();

			if (!ImGui::IsItemToggledOpen())
				fileSystemControlVars.temp_lastClickedItemID = file->subfiles[i]->id;
			else
				fileSystemControlVars.toggleClicked = true;
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE_FILE", &file->subfiles[i]->id, sizeof(int));
			ImGui::Text((*files)[file->subfiles[i]->id].name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE_FILE"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				editor->fileSystem.moveFile(payload_n, file->subfiles[i]->id);
				return;
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FILE"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				editor->fileSystem.moveFile(payload_n, file->subfiles[i]->id);
				return;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();

		ImVec2 size = ImVec2(16.0f, 16.0f);
		ImVec2 uv0 = ImVec2(0.0f, 0.0f);
		ImVec2 uv1 = ImVec2(1.0f, 1.0f);
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

		if (nodeOpen && hasSubFolder)
			ImGui::Image((ImTextureID)editorIcons.openFolderTextureID, size, uv0, uv1, tint_col, border_col);
		else
			ImGui::Image((ImTextureID)editorIcons.closedFolderTextureID, size, uv0, uv1, tint_col, border_col);

		ImGui::SameLine();

		if (lastClickedItemID == file->subfiles[i]->id)
			ImGui::TextColored(editorColors.textSelectedColor, (*files)[file->subfiles[i]->id].name.c_str());
		else
			ImGui::Text((*files)[file->subfiles[i]->id].name.c_str());

		if (nodeOpen && (*files)[file->subfiles[i]->id].type == FileType::folder)
			EditorGUI::createFoldersRecursively(file->subfiles[i]);

		ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
	}
}

void EditorGUI::createFilesPanelRightPart(ImVec2 area) {

	int maxElementsInRow = (int)((area.x + 26) / 100);

	if (lastClickedItemID == -1 || maxElementsInRow == 0)
		return;

	File* file = NULL;
	std::vector<FileNode>* files = &editor->fileSystem.files;

	if ((*files)[lastClickedItemID].type == FileType::folder)
		file = (*files)[lastClickedItemID].addr;
	else
		file = (*files)[lastClickedItemID].addr->parent;

	int frame_padding = 1;
	ImVec2 size = ImVec2(64.0f, 64.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 bg_col = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	for (int i = 0; i < file->subfiles.size(); i++) {

		//ImGui::Dummy(ImVec2(0, 30));

		ImGui::BeginGroup();
		{
			ImGui::PushID(i);

			if (fileSystemControlVars.lastSelectedItemID == file->subfiles[i]->id)
				tint_col = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
			else
				tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImVec2 pos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(pos.x + (100.f - 64.f) / 2, pos.y));

			if (ImGui::ImageButton((ImTextureID)(*files)[file->subfiles[i]->id].textureID, size, uv0, uv1, frame_padding, bg_col, tint_col)) {

				cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
				fileSystemControlVars.temp_lastClickedItemID = file->subfiles[i]->id;
				fileSystemControlVars.panelRightItemClicked = true;
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

				fileSystemControlVars.lastSelectedItemID = file->subfiles[i]->id;
				fileSystemControlVars.panelRightItemTab = true;
			}

			ImGui::SetNextWindowSize(ImVec2(210, 120));
			if (ImGui::BeginPopupContextItem("context_menu_item_popup"))
			{
				cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
				fileSystemControlVars.lastSelectedItemID = file->subfiles[i]->id;
				fileSystemControlVars.panelRightItemClicked = true;

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

				if (ImGui::Selectable("   Open")) {

					if ((*files)[fileSystemControlVars.lastSelectedItemID].type == FileType::folder)
						lastClickedItemID = fileSystemControlVars.lastSelectedItemID;
					else
						ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[fileSystemControlVars.lastSelectedItemID].path).c_str(), NULL, NULL, SW_RESTORE);

					// include all the necessary end codes...
					ImGui::PopStyleColor();
					ImGui::EndPopup();
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}

				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
				ImGui::Dummy(ImVec2(0, 1));

				if (ImGui::Selectable("   Delete")) {

					editor->fileSystem.deleteFileCompletely(fileSystemControlVars.lastSelectedItemID);
					fileSystemControlVars.lastSelectedItemID = -1;

					// include all the necessary end codes...
					ImGui::PopStyleColor();
					ImGui::EndPopup();
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}

				if (ImGui::Selectable("   Duplicate")) {

					editor->fileSystem.duplicateFile(fileSystemControlVars.lastSelectedItemID);

					// include all the necessary end codes...
					ImGui::PopStyleColor();
					ImGui::EndPopup();
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}

				if (ImGui::Selectable("   Rename"))
					fileSystemControlVars.renameItemID = file->subfiles[i]->id;

				p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
				ImGui::Dummy(ImVec2(0, 1));

				if (ImGui::Selectable("   Show in Explorer")) {

					ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[(*files)[fileSystemControlVars.lastSelectedItemID].addr->parent->id].path).c_str(), NULL, NULL, SW_RESTORE);

					// include all the necessary end codes...
					ImGui::PopStyleColor();
					ImGui::EndPopup();
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}
				ImGui::PopStyleColor();
				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_FILE", &file->subfiles[i]->id, sizeof(int));
				ImGui::Text((*files)[file->subfiles[i]->id].name.c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE_FILE"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					editor->fileSystem.moveFile(payload_n, file->subfiles[i]->id);

					// include all the necessary end codes...
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FILE"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					editor->fileSystem.moveFile(payload_n, file->subfiles[i]->id);

					// include all the necessary end codes...
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::PushItemWidth(64.0f);

			if (fileSystemControlVars.lastSelectedItemID == file->subfiles[i]->id)
				editorColors.textColor = editorColors.textSelectedColor;
			else
				editorColors.textColor = editorColors.textUnselectedColor;

			pos = ImGui::GetCursorPos();

			if (fileSystemControlVars.renameItemID == file->subfiles[i]->id) {

				char temp[3] = { '#','#', '\0' };
				char str0[32] = "";
				strcat(str0, (char*)(*files)[file->subfiles[i]->id].name.c_str());
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
				ImGui::SetKeyboardFocusHere(0);
				ImVec2 textSize = ImGui::CalcTextSize(str0);
				ImGui::SetCursorPos(ImVec2(pos.x + (100.f - textSize.x) / 2 - 5, pos.y - 2));

				if (ImGui::InputText(temp, str0, IM_ARRAYSIZE(str0), input_text_flags)) {

					if (strlen(str0) != 0)
						editor->fileSystem.rename(file->subfiles[i]->id, str0);
				}
			}
			else {

				int len = (*files)[file->subfiles[i]->id].name.length();

				if (len > 11) {

					char dots[] = "..\0";
					char fileName[10];
					strcpy(fileName, (*files)[file->subfiles[i]->id].name.substr(0, 7).c_str());
					strcat(fileName, dots);

					ImVec2 textSize = ImGui::CalcTextSize(fileName);
					ImGui::SetCursorPos(ImVec2(pos.x + (100.f - textSize.x) / 2, pos.y));

					ImGui::TextColored(editorColors.textColor, fileName);
				}
				else {

					ImVec2 textSize = ImGui::CalcTextSize((*files)[file->subfiles[i]->id].name.c_str());
					ImGui::SetCursorPos(ImVec2(pos.x + (100.f - textSize.x) / 2, pos.y));

					ImGui::TextColored(editorColors.textColor, (*files)[file->subfiles[i]->id].name.c_str());
				}
			}
			ImGui::EndGroup();
		}
		if ((i + 1) % maxElementsInRow != 0)
			ImGui::SameLine(0);
	}

	if (ImGui::ImageButton((ImTextureID)editorIcons.plusTextureID, size, uv0, uv1, frame_padding, bg_col, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)))
		ImGui::OpenPopup("add_item_popup");

	ImGui::SetNextWindowSize(ImVec2(210, 75));

	if (ImGui::BeginPopup("add_item_popup"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

		if (ImGui::Selectable("   New Folder"))
			editor->fileSystem.newFolder(file->id, "Folder");

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
		ImGui::Dummy(ImVec2(0, 1));

		if (ImGui::Selectable("   New Material"))
			editor->fileSystem.newMaterial(file->id, "Material");

		if (ImGui::Selectable("   New Physics Material"))
			editor->fileSystem.newPhysicMaterial(file->id, "PhysicsMaterial");

		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE_FILE"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int payload_n = *(const int*)payload->Data;
			editor->fileSystem.moveFile(payload_n, file->id);

			// include all the necessary end codes...
			return;
		}
		ImGui::EndDragDropTarget();
	}
}

//----- UTILITY FUNCTIONS -----

bool EditorGUI::mouseDistanceControl() {

	float dx = cursorPosWhenFirstClickedItem.x - ImGui::GetMousePos().x;
	float dy = cursorPosWhenFirstClickedItem.y - ImGui::GetMousePos().y;

	if (sqrt(dx * dx + dy * dy) < 5.0f)
		return true;

	return false;
}

//----- GET / SET FUNCTIONS -----

void EditorGUI::setEditor(Editor* editor) { this->editor = editor; }