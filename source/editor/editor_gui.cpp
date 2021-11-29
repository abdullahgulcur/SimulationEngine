#include "editor.hpp"
#include "editor_gui.hpp"

EditorGUI::EditorGUI() {}

void EditorGUI::init(Editor* editor) {

	this->editor = editor;
	EditorGUI::setFiles(&editor->fileSystem.files);
	EditorGUI::initImGui();
}

void EditorGUI::initImGui() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
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
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
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
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
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

	textSelectedColor = ImVec4(0.2f, 0.72f, 0.95f, 1.f);
	textUnselectedColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	textColor = textUnselectedColor;
}

void EditorGUI::loadTextures() {

	openFolderTextureID = TextureNS::loadDDS("resource/icons/folder_open.DDS");
	closedFolderTextureID = TextureNS::loadDDS("resource/icons/folder_closed.DDS");
	plusTextureID = TextureNS::loadDDS("resource/icons/plus.DDS");
	greaterTextureID = TextureNS::loadDDS("resource/icons/greater.DDS");
	gameObjectTextureID = TextureNS::loadDDS("resource/icons/gameobject.DDS");
	transformTextureID = TextureNS::loadDDS("resource/icons/transform.DDS");
	meshrendererTextureID = TextureNS::loadDDS("resource/icons/meshrenderer.DDS");
	lightTextureID = TextureNS::loadDDS("resource/icons/light.DDS");
	contextMenuTextureID = TextureNS::loadDDS("resource/icons/contextMenu.DDS");
	eyeTextureID = TextureNS::loadDDS("resource/icons/eye.DDS");
	materialTextureID = TextureNS::loadDDS("resource/icons/material.DDS");
	materialSmallTextureID = TextureNS::loadDDS("resource/icons/materialSmall.DDS");
	rigidbodyTextureID = TextureNS::loadDDS("resource/icons/rigidbody.DDS");
	meshColliderTextureID = TextureNS::loadDDS("resource/icons/meshcollider.DDS");
}

void EditorGUI::handleInputs() {

	if (Input::mouseDoubleClicked(0)) {

		entered = true;

		if (EditorGUI::mouseDistanceControl()) {

			if ((*files)[temp_lastClickedItemID].type == FileType::folder) {

				lastClickedItemID = temp_lastClickedItemID;
				subfolderCheckFlag = false;
			}
			else
				ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[temp_lastClickedItemID].path).c_str(), NULL, NULL, SW_RESTORE);
		}
	}

	if (Input::mouseReleased(0)) {

		if (materialChanged) {

			if (lastSelectedEntityID != -1) {
				
				MeshRenderer& m_renderer = editor->scene.meshRendererComponents[editor->scene.entities[lastSelectedEntityID].m_rendererComponentIndex];
				editor->fileSystem.writeMaterialFile(editor->fileSystem.files[m_renderer.mat->fileAddr->id].path, *m_renderer.mat);

				m_renderer.mat->deleteProgram();
				m_renderer.mat->compileShaders(editor->fileSystem.getVertShaderPath(m_renderer.mat->vertShaderFileAddr),
					editor->fileSystem.getFragShaderPath(m_renderer.mat->fragShaderFileAddr),
					editor->scene.dirLightCount, editor->scene.pointLightCount);
			}
			else {
				
				MaterialNS::MaterialFile& mat = editor->fileSystem.getMaterial(lastSelectedItemID);
				editor->fileSystem.writeMaterialFile(editor->fileSystem.files[mat.fileAddr->id].path, mat);

				mat.deleteProgram();
				mat.compileShaders(editor->fileSystem.getVertShaderPath(mat.vertShaderFileAddr), editor->fileSystem.getFragShaderPath(mat.fragShaderFileAddr),
					editor->scene.dirLightCount, editor->scene.pointLightCount);
			}

			materialChanged = false;
		}

		if (entered) {

			mouseLeftPressed = false;
			entered = false;
			panelRightItemClicked = false;
			return;
		}

		if (EditorGUI::mouseDistanceControl() && !toggleClicked && !panelRightItemClicked)
			lastClickedItemID = temp_lastClickedItemID;

		folderLineClicked = false;
		ImGui::ResetMouseDragDelta();
		mouseLeftPressed = false;
		toggleClicked = false;
		panelRightItemClicked = false;
		fileTreeClicked = false;

		entityClicked = false;
	}

	if (Input::mouseReleased(1)) {

		if (!panelRightItemClicked)
			lastSelectedItemID = -1;
	}

	if (Input::mouseClicked(0)) {

		mouseLeftPressed = true;

		if (!panelRightItemTab && !inspectorHovered)
			lastSelectedItemID = -1;

		panelRightItemTab = false;

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

			if (lastSelectedItemID != -1)
				lastSelectedItemID = editor->fileSystem.duplicateFile(lastSelectedItemID);
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

		if (lastSelectedItemID != -1) {

			editor->fileSystem.deleteFileCompletely(lastSelectedItemID);
			lastSelectedItemID = -1;
		}
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_KeyPadEnter))) {

		if (lastSelectedItemID != -1 && renameItemID == -1) {

			if ((*files)[lastSelectedItemID].type == FileType::folder)
				lastClickedItemID = lastSelectedItemID;
			else
				ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[lastSelectedItemID].path).c_str(), NULL, NULL, SW_RESTORE);
		}

		if (renameItemID != -1)
			renameItemID = -1;
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
	ImVec2 windowSize(sceneRegion.x, sceneRegion.y);

	ImTextureID textureId = (ImTextureID)(editor->window.textureColorbuffer);
	ImGui::Image(textureId, windowSize, ImVec2(0, 1), ImVec2(1, 0));

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

			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChildTransforms(optype);
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

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::PopStyleVar();

	ImGui::Indent(6);

	if (lastSelectedEntityID != -1) {

		EditorGUI::showEntityName();

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 5));

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.23f, 0.23f, 0.23f, 1.f));

		EditorGUI::showTransformComponent();

		if (editor->scene.entities[lastSelectedEntityID].m_rendererComponentIndex != -1) {

			MeshRenderer& m_renderer = editor->scene.meshRendererComponents[editor->scene.entities[lastSelectedEntityID].m_rendererComponentIndex];
			MaterialFile& material = *m_renderer.mat;
			EditorGUI::showMeshRendererComponent(m_renderer);
			EditorGUI::showMaterialProperties(material);
		}

		if (editor->scene.entities[lastSelectedEntityID].lightComponentIndex != -1)
			EditorGUI::showLightComponent();

		if (editor->scene.entities[lastSelectedEntityID].rigidbodyComponentIndex != -1)
			EditorGUI::showRigidbodyComponent();

		if (editor->scene.entities[lastSelectedEntityID].meshColliderComponentIndex != -1)
			EditorGUI::showMeshColliderComponent();

		ImGui::PopStyleColor(); // for the separator

		EditorGUI::addComponentButton();
	}

	if (lastSelectedItemID != -1 && editor->fileSystem.files[lastSelectedItemID].type == FileType::material) {

		MaterialFile& material = editor->fileSystem.getMaterial(lastSelectedItemID);
		EditorGUI::showMaterialProperties(material);
	}

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

		if (ImGui::Selectable("   Light")) {

			editor->scene.entities[lastSelectedEntityID].addLightComponent(editor->scene.lightComponents, &editor->scene);
		}
		ImGui::Separator();

		if (ImGui::Selectable("   Mesh Collider")) {

			editor->scene.entities[lastSelectedEntityID].addMeshColliderComponent(editor->scene.meshColliderComponents);
		}
		ImGui::Separator();

		if (ImGui::Selectable("   Mesh Renderer")) {

			MeshFile* mesh = &editor->fileSystem.meshes["Null"];
			MaterialFile* mat = &editor->fileSystem.materials["Default"];
			editor->scene.entities[lastSelectedEntityID].addMeshRendererComponent(mesh, mat, editor->scene.meshRendererComponents);
		}
		ImGui::Separator();

		if (ImGui::Selectable("   Rigidbody")) {

			editor->scene.entities[lastSelectedEntityID].addRigidbodyComponent(editor->scene.rigidbodyComponents);
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

	ImGui::Image((ImTextureID)gameObjectTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();

	static bool active = true;
	ImGui::Checkbox("##8", &active);
	ImGui::SameLine();

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

void EditorGUI::showTransformComponent() {

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
	ImGui::Image((ImTextureID)transformTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text(" Transform");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if(ImGui::ImageButton((ImTextureID)contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	EditorGUI::contextMenuPopup(ComponentType::Transform);

	if (treeNodeOpen) {

		ImGui::Unindent(8);

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Position  X");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float pos_x = editor->scene.entities[lastSelectedEntityID].transform->localPosition.x;
		if (ImGui::DragFloat("##0", &pos_x, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localPosition.x = pos_x;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float pos_y = editor->scene.entities[lastSelectedEntityID].transform->localPosition.y;
		if (ImGui::DragFloat("##1", &pos_y, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localPosition.y = pos_y;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float pos_z = editor->scene.entities[lastSelectedEntityID].transform->localPosition.z;
		if (ImGui::DragFloat("##2", &pos_z, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localPosition.z = pos_z;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();

		ImGui::Text("Rotation  X");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float rot_x = editor->scene.entities[lastSelectedEntityID].transform->localRotation.x;
		if (ImGui::DragFloat("##3", &rot_x, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localRotation.x = rot_x;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float rot_y = editor->scene.entities[lastSelectedEntityID].transform->localRotation.y;
		if (ImGui::DragFloat("##4", &rot_y, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localRotation.y = rot_y;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float rot_z = editor->scene.entities[lastSelectedEntityID].transform->localRotation.z;
		if (ImGui::DragFloat("##5", &rot_z, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localRotation.z = rot_z;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();

		ImGui::Text("Scale     X");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float scale_x = editor->scene.entities[lastSelectedEntityID].transform->localScale.x;
		if (ImGui::DragFloat("##6", &scale_x, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localScale.x = scale_x;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float scale_y = editor->scene.entities[lastSelectedEntityID].transform->localScale.y;
		if (ImGui::DragFloat("##7", &scale_y, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localScale.y = scale_y;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushItemWidth((width - 140) / 3);

		float scale_z = editor->scene.entities[lastSelectedEntityID].transform->localScale.z;
		if (ImGui::DragFloat("##8", &scale_z, 0.1f, 0.0f, 0.0f, "%.2f")) {
			editor->scene.entities[lastSelectedEntityID].transform->localScale.z = scale_z;
			editor->scene.entities[lastSelectedEntityID].transform->updateSelfAndChild();
		}

		ImGui::PopStyleColor();

		ImGui::TreePop();

		ImGui::Indent(8);
	}

	ImGui::Separator();
}

void EditorGUI::showMeshRendererComponent(MeshRenderer& m_renderer) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	bool treeNodeOpen = ImGui::TreeNode("##1");

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)meshrendererTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Mesh Renderer");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::MeshRenderer)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		//const char* meshName = editor->fileSystem.meshNames[m_renderer.VAO].c_str();
		unsigned int vao = m_renderer.VAO;

		const char* matPath;
		if (m_renderer.mat->fileAddr == NULL)
			matPath = "Default";
		else
			matPath = editor->fileSystem.files[m_renderer.mat->fileAddr->id].path.c_str();

		int size_meshes = editor->fileSystem.meshes.size();
		int size_mats = editor->fileSystem.materials.size();
		const char** meshNames = new const char* [size_meshes];
		unsigned int* meshVAOs = new unsigned int[size_meshes];
		unsigned int* meshIndiceSizes = new unsigned int [size_meshes];
		const char** matPaths = new const char* [size_mats];
		const char** matNames = new const char* [size_mats];

		int meshIndex = -1;
		int matIndex = -1;

		int i = 0;
		for (auto& it : editor->fileSystem.meshes) {

			if (it.second.fileAddr == NULL)
				meshNames[i] = "Null";
			else
				meshNames[i] = editor->fileSystem.files[it.second.fileAddr->id].name.c_str();

			//meshNames[i] = editor->fileSystem.files[it.second.fileAddr->id].name.c_str();
			meshVAOs[i] = it.second.VAO;
			meshIndiceSizes[i] = it.second.indiceSize;
			if (it.second.VAO == vao)
				meshIndex = i;
			i++;
		}

		ImGui::Text("Mesh");
		ImGui::SameLine(95);
		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
		ImGui::SetNextItemWidth(width - 130);

		if (ImGui::Combo("##0", &meshIndex, meshNames, size_meshes)) {
			m_renderer.VAO = meshVAOs[meshIndex];
			m_renderer.indiceSize = meshIndiceSizes[meshIndex];
		}

		i = 0;
		for (auto& it : editor->fileSystem.materials) {

			if (it.second.fileAddr == NULL) {
				matPaths[i] = "Default";
				matNames[i] = "Default";
			}
			else {
				matPaths[i] = editor->fileSystem.files[it.second.fileAddr->id].path.c_str();
				matNames[i] = editor->fileSystem.files[it.second.fileAddr->id].name.c_str();
			}

			if (strcmp(matPath, matPaths[i]) == 0)
				matIndex = i;
			i++;
		}

		ImGui::Text("Material");
		ImGui::SameLine(95);
		ImGui::PushItemWidth(80);
		ImGui::SetNextItemWidth(width - 130);

		if (ImGui::Combo("##1", &matIndex, matNames, size_mats)) {

			m_renderer.mat = &editor->fileSystem.materials[matPaths[matIndex]];
		}
		delete[] meshNames;
		delete[] meshVAOs;
		delete[] meshIndiceSizes;
		delete[] matPaths;
		delete[] matNames;

		ImGui::PopStyleColor();
		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showLightComponent() {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	bool treeNodeOpen = ImGui::TreeNode("##2");

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)lightTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Light");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::Light)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Type");

		ImGui::SameLine(95);

		ImGui::PushItemWidth(80);

		int item = editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].type == LightType::DirectionalLight ? 0 : 1;
		const char* items[] = { "Directional", "Point" };

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));

		ImGui::SetNextItemWidth(width - 180);
		if (ImGui::Combo("##0", &item, items, IM_ARRAYSIZE(items))) {

			if (editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].type == LightType::DirectionalLight) {

				if (item == 1) {

					editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].type = LightType::PointLight;
					editor->scene.dirLightCount--;
					editor->scene.pointLightCount++;
					editor->scene.recompileAllMaterials();
				}
			}
			else {

				if (item == 0) {

					editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].type = LightType::DirectionalLight;
					editor->scene.dirLightCount++;
					editor->scene.pointLightCount--;
					editor->scene.recompileAllMaterials();
				}
			}
		}

		ImGui::PopStyleColor();

		ImGui::Text("Power");

		ImGui::SameLine(95);

		float power = editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].power;
		if (ImGui::DragFloat("##1", &power, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].power = power;

		ImGui::Text("Color");

		ImGui::SameLine(95);

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));

		ImVec4 color = ImVec4(editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].color.x,
			editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].color.y,
			editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].color.z, 1.f);

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
			editor->scene.lightComponents[editor->scene.entities[lastSelectedEntityID].lightComponentIndex].color = glm::vec3(color.x, color.y, color.z);

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showMaterialProperties(MaterialFile& material) {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	if(material.fileAddr == NULL)
		ImGui::SetNextItemOpen(false);

	bool treeNodeOpen = ImGui::TreeNode("##3");

	int frame_padding = 2;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(28);
	ImGui::Image((ImTextureID)materialSmallTextureID, ImVec2(16.0f, 16.0f), uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Material");

	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.18f, 0.18f, 1.f));

	if (treeNodeOpen) {

		std::vector<FileNode>& files = editor->fileSystem.files;

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		{
			std::vector<ShaderFile>& vertShaderFiles = editor->fileSystem.vertShaderFiles;
			ImGui::Text("Vert Shader");

			int indiceSize = vertShaderFiles.size() + 1;
			const char** shaderNames = new const char* [indiceSize];
			shaderNames[0] = "Default\0";

			const char* vertShaderPath = editor->fileSystem.getVertShaderPath(material.vertShaderFileAddr);

			int index = 0;
			int i = 1;

			for (auto& shaderFile : vertShaderFiles) {

				shaderNames[i] = files[shaderFile.fileAddr->id].name.c_str();
				i++;
			}

			if (indiceSize == 1 || strcmp(vertShaderPath, "source/shader/Default.vert") == 0)
				index = 0;
			else {

				i = 1;
				for (auto& shaderFile :vertShaderFiles) {

					if (strcmp(vertShaderPath, files[shaderFile.fileAddr->id].path.c_str()) == 0) {
						index = i;
						break;
					}
					i++;
				}
			}

			ImGui::SameLine(115); ImGui::SetNextItemWidth(width - 120);

			if (ImGui::Combo("##0", &index, shaderNames, indiceSize)) {
				if (index == 0)
					material.vertShaderFileAddr = NULL;
				else
					material.vertShaderFileAddr = vertShaderFiles[index - 1].fileAddr;
				materialChanged = true;
			}

			delete[] shaderNames;
		}

		{
			std::vector<ShaderFile>& fragShaderFiles = editor->fileSystem.fragShaderFiles;
			ImGui::Text("Frag Shader");

			int indiceSize = fragShaderFiles.size() + 1;
			const char** shaderNames = new const char* [indiceSize];
			shaderNames[0] = "Default";

			const char* fragShaderPath = editor->fileSystem.getFragShaderPath(material.fragShaderFileAddr);;

			int index = 0;
			int i = 1;

			for (auto& shaderFile : fragShaderFiles) {

				shaderNames[i] = files[shaderFile.fileAddr->id].name.c_str();
				i++;
			}

			if (indiceSize == 1 || strcmp(fragShaderPath, "source/shader/Default.frag") == 0)
				index = 0;
			else {

				i = 1;
				for (auto& shaderFile : fragShaderFiles) {

					if (strcmp(fragShaderPath, files[shaderFile.fileAddr->id].path.c_str()) == 0) {
						index = i;
						break;
					}
					i++;
				}
			}

			ImGui::SameLine(115); ImGui::SetNextItemWidth(width - 120);

			bool matShaderSourceChange = false;
			if (ImGui::Combo("##1", &index, shaderNames, indiceSize)) {
				if (index == 0)
					material.fragShaderFileAddr = NULL;
				else
					material.fragShaderFileAddr = fragShaderFiles[index - 1].fileAddr;
				materialChanged = true;
				matShaderSourceChange = true;
			}

			delete[] shaderNames;

			if (index != 0) {

				int sampler2DCount = fragShaderFiles[index - 1].sampler2DNames.size();
				int floatCount = fragShaderFiles[index - 1].floatNames.size();

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
					ImGui::Text(fragShaderFiles[index - 1].sampler2DNames[i].c_str());
				}

				for (int i = 0; i < floatCount; i++) {

					ImGui::Text(fragShaderFiles[index - 1].floatNames[i].c_str());

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

void EditorGUI::showRigidbodyComponent() {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	bool treeNodeOpen = ImGui::TreeNode("##4");

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)rigidbodyTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Rigidbody");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::Rigidbody)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Mass");

		ImGui::SameLine(110);

		float mass = editor->scene.rigidbodyComponents[editor->scene.entities[lastSelectedEntityID].rigidbodyComponentIndex].mass;
		if (ImGui::DragFloat("##0", &mass, 0.1f, 0.0f, 0.0f, "%.2f"))
			editor->scene.rigidbodyComponents[editor->scene.entities[lastSelectedEntityID].rigidbodyComponentIndex].mass = mass;

		ImGui::Text("Use Gravity");

		ImGui::SameLine(110);

		bool& useGravity = editor->scene.rigidbodyComponents[editor->scene.entities[lastSelectedEntityID].rigidbodyComponentIndex].useGravity;

		ImGui::Checkbox("##1", &useGravity);

		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorGUI::showMeshColliderComponent() {

	float width = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemOpen(true);

	bool treeNodeOpen = ImGui::TreeNode("##5");

	int frame_padding = 1;
	ImVec2 size = ImVec2(16.0f, 16.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 bg_col = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

	ImGui::SameLine(25);
	ImGui::Image((ImTextureID)meshColliderTextureID, size, uv0, uv1, tint_col, border_col);
	ImGui::SameLine();
	ImGui::Text("  Mesh Collider");

	ImGui::SameLine();
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(width - 20, pos.y));

	if (ImGui::ImageButton((ImTextureID)contextMenuTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col))
		ImGui::OpenPopup("context_menu_popup");

	if (EditorGUI::contextMenuPopup(ComponentType::MeshCollider)) {

		ImGui::TreePop();
		return;
	}

	if (treeNodeOpen) {

		pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 3));

		ImGui::Text("Convex");

		ImGui::SameLine(110);

		bool& convex = editor->scene.meshColliderComponents[editor->scene.entities[lastSelectedEntityID].meshColliderComponentIndex].convex;

		ImGui::Checkbox("##0", &convex);

		ImGui::Text("Trigger");

		ImGui::SameLine(110);

		bool& trigger = editor->scene.meshColliderComponents[editor->scene.entities[lastSelectedEntityID].meshColliderComponentIndex].trigger;

		ImGui::Checkbox("##1", &trigger);

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
					strcpy(name, (*files)[it.second.fileAddr->id].name.c_str());
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

bool EditorGUI::contextMenuPopup(ComponentType type) {

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

			if(type != ComponentType::Transform)
				editor->scene.entities[lastSelectedEntityID].removeComponent(type, &editor->scene);

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
	ImGui::Image((ImTextureID)eyeTextureID, imgsize, uv0, uv1, tint_col, border_col);
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
				editor->scene.moveEntity(payload_n, editor->scene.rootTransform->id);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Unindent(15);
		EditorGUI::createSceneGraphRecursively(editor->scene.rootTransform);
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
				toggleClicked = true;
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

		ImGui::Image((ImTextureID)gameObjectTextureID, size, uv0, uv1, tint_col, border_col);

		ImGui::SameLine();

		if (lastSelectedItemID == transform->children[i]->id)
			textColor = textSelectedColor;
		else
			textColor = textUnselectedColor;

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
				ImGui::TextColored(textSelectedColor, editor->scene.entities[transform->children[i]->id].name);
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

			lastSelectedEntityID = editor->scene.newEntity(editor->scene.rootTransform->id, "Entity")->id;
			// include all the necessary end codes...
			ImGui::PopStyleColor();
			ImGui::EndPopup();
			return;
		}

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
		ImGui::Dummy(ImVec2(0, 1));

		if (ImGui::Selectable("   Sun")) {

			lastSelectedEntityID = editor->scene.newLight(editor->scene.rootTransform->id, "Sun", LightType::DirectionalLight);
			// include all the necessary end codes...
			ImGui::PopStyleColor();
			ImGui::EndPopup();
			return;
		}

		if (ImGui::Selectable("   Point Light")) {

			lastSelectedEntityID = editor->scene.newLight(editor->scene.rootTransform->id, "Point_Light", LightType::PointLight);
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
		folderLineClicked = true;

	if (size.x > 400) {
		if (folderLineClicked) {

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

		if (temp_lastClickedItemID == popped->id && mouseLeftPressed && !fileTreeClicked)
			textColor = textSelectedColor;
		else
			textColor = textUnselectedColor;

		ImGui::TextColored(textColor, (*files)[popped->id].name.c_str());
		if (ImGui::IsItemClicked()) {
			cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
			temp_lastClickedItemID = popped->id;
		}
		ImGui::SameLine(0, 0);

		ImVec2 uv_min = ImVec2(0.0f, 0.0f);
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImGui::Image((ImTextureID)greaterTextureID, ImVec2(16.f, 16.f), uv_min, uv_max, tint_col, border_col);

		ImGui::SameLine(0, 0);
	}
	File* popped = fileStack.top();
	fileStack.pop();

	ImGui::Text((*files)[popped->id].name.c_str());
	if (ImGui::IsItemClicked()) {
		cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
		temp_lastClickedItemID = popped->id;
	}

	if (lastSelectedItemID != -1 && (*files)[lastSelectedItemID].type != FileType::folder) {

		ImGui::SameLine();
		char line[] = " | \0";
		char fileName[32];
		strcpy(fileName, line);
		strcat(fileName, (*files)[lastSelectedItemID].name.c_str());
		strcat(fileName, (*files)[lastSelectedItemID].extension.c_str());
		ImGui::Text(fileName);
	}
}

void EditorGUI::createFoldersRecursively(File* file) {

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

			if (!subfolderCheckFlag)
				ImGui::SetNextItemOpen(true);
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)file->subfiles[i]->id, node_flags, ""); //, " %d", file->subfiles[i]->id

		if (ImGui::IsItemClicked()) {

			fileTreeClicked = true;
			subfolderCheckFlag = true;
			cursorPosWhenFirstClickedItem = ImGui::GetMousePos();

			if (!ImGui::IsItemToggledOpen())
				temp_lastClickedItemID = file->subfiles[i]->id;
			else
				toggleClicked = true;
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
			ImGui::Image((ImTextureID)openFolderTextureID, size, uv0, uv1, tint_col, border_col);
		else
			ImGui::Image((ImTextureID)closedFolderTextureID, size, uv0, uv1, tint_col, border_col);

		ImGui::SameLine();

		if (lastClickedItemID == file->subfiles[i]->id)
			ImGui::TextColored(textSelectedColor, (*files)[file->subfiles[i]->id].name.c_str());
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

			if (lastSelectedItemID == file->subfiles[i]->id)
				tint_col = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
			else
				tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImVec2 pos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(pos.x + (100.f - 64.f) / 2, pos.y));

			if (ImGui::ImageButton((ImTextureID)(*files)[file->subfiles[i]->id].textureID, size, uv0, uv1, frame_padding, bg_col, tint_col)) {

				cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
				temp_lastClickedItemID = file->subfiles[i]->id;
				panelRightItemClicked = true;
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

				lastSelectedItemID = file->subfiles[i]->id;
				panelRightItemTab = true;
			}

			ImGui::SetNextWindowSize(ImVec2(210, 120));
			if (ImGui::BeginPopupContextItem("context_menu_item_popup"))
			{
				cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
				lastSelectedItemID = file->subfiles[i]->id;
				panelRightItemClicked = true;

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

				if (ImGui::Selectable("   Open")) {

					if ((*files)[lastSelectedItemID].type == FileType::folder)
						lastClickedItemID = lastSelectedItemID;
					else
						ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[lastSelectedItemID].path).c_str(), NULL, NULL, SW_RESTORE);

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

					editor->fileSystem.deleteFileCompletely(lastSelectedItemID);
					lastSelectedItemID = -1;

					// include all the necessary end codes...
					ImGui::PopStyleColor();
					ImGui::EndPopup();
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}

				if (ImGui::Selectable("   Duplicate")) {

					editor->fileSystem.duplicateFile(lastSelectedItemID);

					// include all the necessary end codes...
					ImGui::PopStyleColor();
					ImGui::EndPopup();
					ImGui::PopID();
					ImGui::EndGroup();
					return;
				}

				if (ImGui::Selectable("   Rename"))
					renameItemID = file->subfiles[i]->id;

				p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
				ImGui::Dummy(ImVec2(0, 1));

				if (ImGui::Selectable("   Show in Explorer")) {

					ShellExecute(NULL, L"open", std::filesystem::absolute((*files)[(*files)[lastSelectedItemID].addr->parent->id].path).c_str(), NULL, NULL, SW_RESTORE);

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

			if (lastSelectedItemID == file->subfiles[i]->id)
				textColor = textSelectedColor;
			else
				textColor = textUnselectedColor;

			pos = ImGui::GetCursorPos();

			if (renameItemID == file->subfiles[i]->id) {

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
					//renameItemID = -1;
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

					ImGui::TextColored(textColor, fileName);
				}
				else {

					ImVec2 textSize = ImGui::CalcTextSize((*files)[file->subfiles[i]->id].name.c_str());
					ImGui::SetCursorPos(ImVec2(pos.x + (100.f - textSize.x) / 2, pos.y));

					ImGui::TextColored(textColor, (*files)[file->subfiles[i]->id].name.c_str());
				}
			}
			ImGui::EndGroup();
		}
		if ((i + 1) % maxElementsInRow != 0)
			ImGui::SameLine(0);
	}

	if (ImGui::ImageButton((ImTextureID)plusTextureID, size, uv0, uv1, frame_padding, bg_col, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)))
		ImGui::OpenPopup("add_item_popup");

	ImGui::SetNextWindowSize(ImVec2(210, 55));

	if (ImGui::BeginPopup("add_item_popup"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 2.0f));

		if (ImGui::Selectable("   New Folder")) {

			editor->fileSystem.newFolder(file->id, "Folder");
		}

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
		ImGui::Dummy(ImVec2(0, 1));

		if (ImGui::Selectable("   New Material")) {

			editor->fileSystem.newMaterial(file->id, "Material");
		}

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

Editor* EditorGUI::getEditor() { return editor; }

void EditorGUI::setFiles(std::vector<FileNode>* files) { this->files = files; }