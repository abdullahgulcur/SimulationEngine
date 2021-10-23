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
	//std::vector<Scene>* sceneList;

	/*
	* INDEX VARIABLES
	*/
	int lastSelectedItemID = -1;
	int temp_lastClickedItemID = 1;
	int lastRightClickedItemID = -1;
	int renameItemID = -1;

	int indexForEntity;
	int lastSelectedEntityID = -1;
	int temp_lastClickedEntityID = 1;
	int lastRightClickedEntityID = -1;
	int renameEntityID = -1;

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

	bool entityClicked = false;
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
	int lastClickedEntityID = 1;

	/*
	* TEXTURE IDs
	*/
	unsigned int openFolderTextureID;
	unsigned int closedFolderTextureID;
	unsigned int plusTextureID;
	unsigned int greaterTextureID;
	unsigned int gameObjectTextureID;

	EditorGUI();

	void initImGui();

	void newFrameImGui();

	void renderImGui();

	void destroyImGui();

	void setTheme();

	void loadTextures();

	void updateStateMachine();

	void createPanels();

	void createScenePanel();

	void createConsolePanel();

	void createInspectorPanel();

	void createHierarchyPanel();

	void createSceneGraphRecursively(Entity* sceneGraph);

	void hiearchyCreateButton();

	void createFilesPanel();

	void showCurrentDirectoryText();

	void createFoldersRecursively(File * file);

	void createFilesPanelRightPart(ImVec2 area);

	bool mouseDistanceControl();

	void setEditor(Editor* editor);

	Editor* getEditor();

	void setFiles(std::map<int, FileNode>* files);

	std::map<int, FileNode>* getFiles();

	//void setSceneList(std::vector<Scene>* sceneList);

	//std::vector<Scene>* getSceneList();

};