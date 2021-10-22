#include "editor.hpp"
#include "editor_gui.hpp"

EditorGUI::EditorGUI() {
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

	ImGui_ImplGlfw_InitForOpenGL(editor->getWindow().getGLFWwindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	EditorGUI::loadTextures();
}

void EditorGUI::newFrameImGui() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EditorGUI::renderImGui() {

	ImGui::Render();

	int display_w, display_h;
	glfwGetFramebufferSize(editor->getWindow().getGLFWwindow(), &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);

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
	//colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.8f, 0.8f, 0.8f, 1.f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
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
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
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

	Texture texture;
	openFolderTextureID = texture.loadDDS("resource/icons/folder_open.DDS");
	closedFolderTextureID = texture.loadDDS("resource/icons/folder_closed.DDS");
	plusTextureID = texture.loadDDS("resource/icons/plus.DDS");
	greaterTextureID = texture.loadDDS("resource/icons/greater.DDS");
}

void EditorGUI::updateStateMachine() {

	if (ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft)) {

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

	if (ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonLeft)) {

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
	}

	if (ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonRight)) {

		if (!panelRightItemClicked)
			lastSelectedItemID = -1;
	}

	if (ImGui::IsMouseClicked(ImGuiPopupFlags_MouseButtonLeft)) {

		mouseLeftPressed = true;

		if (!panelRightItemTab)
			lastSelectedItemID = -1;

		panelRightItemTab = false;
	}
}

void EditorGUI::createPanels() {

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

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

	EditorGUI::createInspectorPanel();
	EditorGUI::createHierarchyPanel();
	EditorGUI::createFilesPanel();
	EditorGUI::createScenePanel();
	EditorGUI::createConsolePanel();

	EditorGUI::updateStateMachine();

	ImGui::End();
}

//----- SCENE VIEWPORT -----

void EditorGUI::createScenePanel() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Scene");

	ImVec2 region = ImGui::GetContentRegionAvail();
	ImVec2 windowSize(region.x, region.y);

	editor->getEditorCamera().setAspectRatio(region.x, region.y);

	ImTextureID textureId = (ImTextureID)(editor->getWindow().textureColorbuffer);
	ImGui::Image(textureId, windowSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
	ImGui::PopStyleVar();
}

//----- CONSOLE MENU -----

void EditorGUI::createConsolePanel() {

	ImGui::Begin("Console");

	ImGui::End();
}

//----- INSPECTOR MENU -----

void EditorGUI::createInspectorPanel() {

	ImGui::Begin("Inspector");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Mouse x: %.1f y: %.1f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

	ImGui::End();
}

//----- HIERARCHY MENU -----

void EditorGUI::createHierarchyPanel() {

	ImGui::Begin("Hierarchy");

	ImGui::End();	
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
		indexForFolder = 1;
		EditorGUI::createFoldersRecursively(editor->fileSystem.file);
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

	if ((*files)[lastSelectedItemID].type != FileType::folder) {

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

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)file->subfiles[i]->id, node_flags, "");

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

		indexForFolder++;

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

			if (renameItemID == file->subfiles[i]->id) {

				char temp[3];
				temp[0] = '#';
				temp[1] = '#';
				temp[2] = '\0';
				char str0[32] = "";
				strcat(str0, (char*)(*files)[file->subfiles[i]->id].name.c_str());
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
				ImGui::SetKeyboardFocusHere(0);
				if (ImGui::InputText(temp, str0, IM_ARRAYSIZE(str0), input_text_flags)) {

					if (strlen(str0) != 0)
						editor->fileSystem.rename(file->subfiles[i]->id, str0);
					renameItemID = -1;
				}
			}
			else {

				//ImGui::Indent(5);
				int len = (*files)[file->subfiles[i]->id].name.length();
				ImVec2 pos = ImGui::GetCursorPos();

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

				//ImGui::Unindent(5);

			}
			ImGui::EndGroup();
		}
		if ((i + 1) % maxElementsInRow != 0)
			ImGui::SameLine(0);
	}

	if (ImGui::ImageButton((ImTextureID)plusTextureID, size, uv0, uv1, frame_padding, bg_col, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)))
		editor->fileSystem.newFolder(file->id, "NewFolder");

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

void EditorGUI::setFiles(std::map<int, FileNode>* files) { this->files = files; }

std::map<int, FileNode>* EditorGUI::getFiles() { return files; }


