#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <stack>

class Editor;

class EditorGUI {

private:

	Editor* editor;

public:

	/*
	* TEXTURE IDs
	*/
	unsigned int openFolderTextureID;
	unsigned int closedFolderTextureID;
	unsigned int plusTextureID;
	unsigned int greaterTextureID;

	/*
	* INDEX VARIABLES
	*/
	int indexForFolder;
	int lastClickedItemID = 1;
	int lastSelectedItemID = -1;
	int temp_lastClickedItemID = 1;
	int lastRightClickedItemID = -1;
	int renameItemID = -1;

	ImVec2 cursorPosWhenFirstClickedItem;
	ImVec4 textColor;

	/*
	* FLAG VARIABLES
	*/
	bool folderLineClicked = false;
	bool mouseLeftPressed = false;
	bool toggleClicked = false;
	bool entered = false; // sadece kontrol icin bir flag
	bool panelRightItemClicked = false; // tiklayip cektikten sonra
	bool panelRightItemTab = false; // tiklar tiklamaz
	bool subfolderCheckFlag = false;
	bool fileTreeClicked = false;
	/*
	* UI VARIABLES
	*/
	float foldersPanelLeftPartWidth = 260.f;

	EditorGUI();

	void initImGui();

	void newFrameImGui();

	void setTheme();

	void renderImGui();

	void destroyImGui();

	void createPanels();

	void ShowExampleAppDockSpace();

	void createScenePanel();

	void createConsolePanel();

	void createInspectorPanel();

	void createHierarchyPanel();

	void createFoldersRecursively(File * file);

	void createFilesPanelRightPart(ImVec2 area);

	void createFilesPanel();

	void showCurrentDirectoryText();

	void setEditor(Editor* editor);

	Editor* getEditor();

	bool mouseDistanceControl();

};