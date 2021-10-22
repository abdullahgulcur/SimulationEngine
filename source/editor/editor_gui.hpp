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
	std::map<int, FileNode>* files;

	/*
	* INDEX VARIABLES
	*/
	int indexForFolder;
	int lastSelectedItemID = -1;
	int temp_lastClickedItemID = 1;
	int lastRightClickedItemID = -1;
	int renameItemID = -1;

	ImVec2 cursorPosWhenFirstClickedItem;
	ImVec4 textColor;
	ImVec4 textSelectedColor;
	ImVec4 textUnselectedColor;

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
	float itemSpacingRightPanel = 15.f;

	/*
	* TEMP VAR
	*/
	ImVec2 mousePos;

public:

	/*
	* INDEX VARIABLES
	*/
	int lastClickedItemID = 1;

	/*
	* TEXTURE IDs
	*/
	unsigned int openFolderTextureID;
	unsigned int closedFolderTextureID;
	unsigned int plusTextureID;
	unsigned int greaterTextureID;

	EditorGUI();

	void initImGui();

	void loadTextures();

	void newFrameImGui();

	void setTheme();

	void renderImGui();

	void destroyImGui();

	void createPanels();

	void updateStateMachine();

	void createScenePanel();

	void createConsolePanel();

	void createInspectorPanel();

	void createHierarchyPanel();

	void createFoldersRecursively(File * file);

	void createFilesPanelRightPart(ImVec2 area);

	void createFilesPanel();

	void showCurrentDirectoryText();

	void setEditor(Editor* editor);

	void setFiles(std::map<int, FileNode>* files);

	std::map<int, FileNode>* getFiles();

	Editor* getEditor();

	bool mouseDistanceControl();

};