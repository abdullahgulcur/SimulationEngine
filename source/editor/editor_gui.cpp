#include "editor.hpp"
#include "editor_gui.hpp"

EditorGUI::EditorGUI() {
	
	textColor = ImVec4(1, 1, 1, 1);
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

	Texture texture;
	openFolderTextureID = texture.loadDDS("resource/icons/folder_open.DDS");
	closedFolderTextureID = texture.loadDDS("resource/icons/folder_closed.DDS");
	plusTextureID = texture.loadDDS("resource/icons/plus.DDS");
	greaterTextureID = texture.loadDDS("resource/icons/greater.DDS");
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
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 7;
	style.ChildRounding = 4;
	style.FrameRounding = 4;
	style.PopupRounding = 2;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;
	style.IndentSpacing = 20;
}

void EditorGUI::newFrameImGui() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//scenePanelSize = ImGui::GetContentRegionAvail();

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

void EditorGUI::createPanels() {

	EditorGUI::ShowExampleAppDockSpace();

	// general controls...
}

void EditorGUI::ShowExampleAppDockSpace()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);//
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	bool p_open = true;

	ImGui::Begin("MyDockSpace", &p_open, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	EditorGUI::createInspectorPanel();
	EditorGUI::createHierarchyPanel();
	EditorGUI::createFilesPanel();
	EditorGUI::createScenePanel();
	EditorGUI::createConsolePanel();

	int mouse_button_left = ImGuiPopupFlags_MouseButtonLeft;
	int mouse_button_right = ImGuiPopupFlags_MouseButtonRight;

	if (ImGui::IsMouseDoubleClicked(mouse_button_left)) {

		entered = true;

		if (EditorGUI::mouseDistanceControl()) {

			if (editor->fileSystem.files[temp_lastClickedItemID].type == FileType::folder) {

				lastClickedItemID = temp_lastClickedItemID;
				subfolderCheckFlag = false;
			}
			else {

				ShellExecute(NULL, L"open", std::filesystem::absolute(editor->fileSystem.files[temp_lastClickedItemID].path).c_str(), NULL, NULL, SW_RESTORE);
			}
		}
	}

	if (ImGui::IsMouseReleased(mouse_button_left)) {

		if (entered) {

			mouseLeftPressed = false;
			entered = false;
			panelRightItemClicked = false;
			ImGui::End();
			return;
		}

		if (EditorGUI::mouseDistanceControl() && !toggleClicked && !panelRightItemClicked) {
			lastClickedItemID = temp_lastClickedItemID;
		}
		folderLineClicked = false;
		ImGui::ResetMouseDragDelta();
		mouseLeftPressed = false;
		toggleClicked = false;
		panelRightItemClicked = false;
		fileTreeClicked = false;
	}

	if (ImGui::IsMouseReleased(mouse_button_right)) {

		if(!panelRightItemClicked)
			lastSelectedItemID = -1;
	}

	if (ImGui::IsMouseClicked(mouse_button_left)) {

		mouseLeftPressed = true;
		
		if (!panelRightItemTab)
			lastSelectedItemID = -1;

		panelRightItemTab = false;
	}

	ImGui::End();
}

void EditorGUI::createScenePanel() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Scene");

	ImVec2 temp = ImGui::GetContentRegionAvail();
	ImVec2 windowSize(temp.x, temp.y);

	//EditorCamera am = editor->getEditorCamera();

	editor->getEditorCamera().setAspectRatio(temp.x, temp.y);

	//editor->getEditorCamera().setAspectRatio(1920, 1080); //(temp.x, temp.y);

	//static bool resizeFlag = false;

	//if (!resizeFlag) {
	//	scenePanelSize = temp;
	//	resizeFlag = true;
	//}

	//if (this->scenePanelSize.x != temp.x || this->scenePanelSize.y != temp.y) {

	//	resizeFlag = false;
	//}

	ImTextureID textureId = (ImTextureID)(editor->getWindow().textureColorbuffer);
	ImGui::Image(textureId, windowSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorGUI::createConsolePanel() {

	ImGui::Begin("Console");

	ImGui::End();
}

void EditorGUI::createInspectorPanel() {

	ImGui::Begin("Inspector");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}

void EditorGUI::createHierarchyPanel() {

	ImGui::Begin("Hierarchy");

	ImGui::End();	
}

void EditorGUI::createFoldersRecursively(File* file) {

	for (int i = 0; i < file->subfiles.size(); i++) {

		if (editor->fileSystem.files[file->subfiles[i]->id].type != FileType::folder)
			continue;

		ImGui::Indent(ImGui::GetStyle().IndentSpacing);

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGuiTreeNodeFlags node_flags = base_flags;

		bool hasSubFolder = editor->fileSystem.hasSubFolder(file->subfiles[i]);
		if (!hasSubFolder)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		else
			node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (editor->fileSystem.subfolderCheck(editor->fileSystem.files[lastClickedItemID].addr, file->subfiles[i])) {

			if (!subfolderCheckFlag)
				ImGui::SetNextItemOpen(true);
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)file->subfiles[i]->id, node_flags, ""); // , "%d ", file->subfiles[i]->id

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
			ImGui::Text(editor->fileSystem.files[file->subfiles[i]->id].name.c_str());
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
		ImVec4 bg_col = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

		if (nodeOpen && hasSubFolder)
			ImGui::Image((ImTextureID)openFolderTextureID, size, uv0, uv1, tint_col, border_col);
		else
			ImGui::Image((ImTextureID)closedFolderTextureID, size, uv0, uv1, tint_col, border_col);

		ImGui::SameLine();

		if(lastClickedItemID == file->subfiles[i]->id)
			ImGui::TextColored(ImVec4(0.2f, 0.72f, 0.95, 1), editor->fileSystem.files[file->subfiles[i]->id].name.c_str());
		else
			ImGui::Text(editor->fileSystem.files[file->subfiles[i]->id].name.c_str());

		indexForFolder++;

		if (nodeOpen && editor->fileSystem.files[file->subfiles[i]->id].type == FileType::folder)
			EditorGUI::createFoldersRecursively(file->subfiles[i]);

		ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
	}
}

void EditorGUI::createFilesPanelRightPart(ImVec2 area) {

	int maxElementsInRow = (int)(area.x / 75);

	if (lastClickedItemID == -1 || maxElementsInRow == 0)
		return;

	File* file = NULL;

	if (editor->fileSystem.files[lastClickedItemID].type == FileType::folder)
		file = editor->fileSystem.files[lastClickedItemID].addr;
	else
		file = editor->fileSystem.files[lastClickedItemID].addr->parent;

	int frame_padding = 1;
	ImVec2 size = ImVec2(64.0f, 64.0f);
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 bg_col = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	for (int i = 0; i < file->subfiles.size(); i++) {

		ImGui::BeginGroup();
		{
			ImGui::PushID(i);

			if(lastSelectedItemID == file->subfiles[i]->id)
				tint_col = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
			else
				tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			
			if (ImGui::ImageButton((ImTextureID)editor->fileSystem.files[file->subfiles[i]->id].textureID, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
				
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

					if (editor->fileSystem.files[lastSelectedItemID].type == FileType::folder)
						lastClickedItemID = lastSelectedItemID;
					else
						ShellExecute(NULL, L"open", std::filesystem::absolute(editor->fileSystem.files[lastSelectedItemID].path).c_str(), NULL, NULL, SW_RESTORE);

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

				if (ImGui::Selectable("   Rename")) {

					renameItemID = file->subfiles[i]->id;

					// include all the necessary end codes...
					//ImGui::PopStyleColor();
					//ImGui::EndPopup();
					//ImGui::PopID();
					//ImGui::EndGroup();
					//return;
				}

				p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 192, p.y + 1), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
				ImGui::Dummy(ImVec2(0, 1));

				if (ImGui::Selectable("   Show in Explorer")) {

					ShellExecute(NULL, L"open", std::filesystem::absolute(editor->fileSystem.files[editor->fileSystem.files[lastSelectedItemID].addr->parent->id].path).c_str(), NULL, NULL, SW_RESTORE);

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
				ImGui::Text(editor->fileSystem.files[file->subfiles[i]->id].name.c_str());
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
				textColor = ImVec4(0.2f, 0.72f, 0.95, 1);
			else
				textColor = ImVec4(1, 1, 1, 1);

			if (renameItemID == file->subfiles[i]->id) {

				char temp[3];
				temp[0] = '#';
				temp[1] = '#';
				temp[2] = '\0';
				char str0[128] = "";
				strcat(str0, (char*)editor->fileSystem.files[file->subfiles[i]->id].name.c_str());
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
				ImGui::SetKeyboardFocusHere(0);
				if (ImGui::InputText(temp, str0, IM_ARRAYSIZE(str0), input_text_flags)) {

					if(strlen(str0) != 0) 
						editor->fileSystem.rename(file->subfiles[i]->id, str0);
					renameItemID = -1;
				}
			}
			else {

				ImGui::Indent(5);
				int len = editor->fileSystem.files[file->subfiles[i]->id].name.length();

				if (len > 9) {

					std::string name = "";
					name = editor->fileSystem.files[file->subfiles[i]->id].name.substr(0, 7) + "..";
					ImGui::TextColored(textColor, name.c_str());
				}
				else
					ImGui::TextColored(textColor, editor->fileSystem.files[file->subfiles[i]->id].name.c_str());

				ImGui::Unindent(5);

			}
			ImGui::EndGroup();
		}
		if ((i + 1) % maxElementsInRow != 0)
			ImGui::SameLine();
	}

	tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	if (ImGui::ImageButton((ImTextureID)plusTextureID, size, uv0, uv1, frame_padding, bg_col, tint_col)) 
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

void EditorGUI::showCurrentDirectoryText() {

	if (lastClickedItemID == -1)
		return;

	std::stack<File*>fileStack;
	File* iter = editor->fileSystem.files[lastClickedItemID].addr;

	if (editor->fileSystem.files[iter->id].type != FileType::folder)
		iter = iter->parent;

	while (iter->parent != NULL) {

		fileStack.push(editor->fileSystem.files[iter->id].addr);
		iter = iter->parent;
	}
	while (fileStack.size() > 1) {

		File* popped = fileStack.top();
		fileStack.pop();

		if(temp_lastClickedItemID == popped->id && mouseLeftPressed && !fileTreeClicked)
			textColor = ImVec4(0.2f, 0.72f, 0.95, 1);
		else
			textColor = ImVec4(1, 1, 1, 1);

		ImGui::TextColored(textColor, editor->fileSystem.files[popped->id].name.c_str());
		if (ImGui::IsItemClicked()) {
			cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
			temp_lastClickedItemID = popped->id;
		}
		ImGui::SameLine(0,0);

		ImVec2 uv_min = ImVec2(0.0f, 0.0f);
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImGui::Image((ImTextureID)greaterTextureID, ImVec2(16.f, 16.f), uv_min, uv_max, tint_col, border_col);

		ImGui::SameLine(0,0);
	}
	File* popped = fileStack.top();
	fileStack.pop();

	ImGui::Text(editor->fileSystem.files[popped->id].name.c_str());
	if (ImGui::IsItemClicked()) {
		cursorPosWhenFirstClickedItem = ImGui::GetMousePos();
		temp_lastClickedItemID = popped->id;
	}

	if (editor->fileSystem.files[lastSelectedItemID].type != FileType::folder) {

		ImGui::SameLine();
		std::string temp = " | " + editor->fileSystem.files[lastSelectedItemID].name + editor->fileSystem.files[lastSelectedItemID].extension;
		ImGui::Text(temp.c_str());
	}
}

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
	int maxElementsInRow = (int)(scrolling_child_size_r.x / 75);

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

void EditorGUI::setEditor(Editor* editor) {
	this->editor = editor; 
	EditorGUI::initImGui();
}

Editor* EditorGUI::getEditor() { return editor; }

bool EditorGUI::mouseDistanceControl() {

	float dx = cursorPosWhenFirstClickedItem.x - ImGui::GetMousePos().x;
	float dy = cursorPosWhenFirstClickedItem.y - ImGui::GetMousePos().y;

	if (sqrt(dx * dx + dy * dy) < 5.0f)
		return true;
	
	return false;
}