#pragma once

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <stack>

#include <GLM/gtc/type_ptr.hpp>

#include <math.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imguizmo/imguizmo.h>


class Editor;

class EditorGUI {

private:

	Editor* editor;
	std::vector<FileNode>* files;
	//std::vector<Scene>* sceneList;

	/*
	* INDEX VARIABLES
	*/

	// File System
	int lastSelectedItemID = -1;
	int temp_lastClickedItemID = 1;
	int lastRightClickedItemID = -1;
	int renameItemID = -1;

	// Scene Graph
	int renameEntityID = -1;

	ImVec2 cursorPosWhenFirstClickedItem;
	ImVec4 textColor;
	ImVec4 textSelectedColor;
	ImVec4 textUnselectedColor;

	/*
	* FLAG VARIABLES
	*/

	// File System
	bool folderLineClicked = false;
	bool mouseLeftPressed = false;
	bool toggleClicked = false;
	bool entered = false; // sadece kontrol icin bir flag
	bool panelRightItemClicked = false; // tiklayip cektikten sonra
	bool panelRightItemTab = false; // tiklar tiklamaz
	bool subfolderCheckFlag = false;
	bool fileTreeClicked = false;

	// Scene Graph
	bool childrenCheckFlag = true;
	bool entityClicked = false;

	/*
	* UI VARIABLES
	*/
	float foldersPanelLeftPartWidth = 260.f;
	float itemSpacingRightPanel = 15.f;

	/*
	* TEMP VAR
	*/

	bool inspectorHovered = false;
	bool fileChangedInInspector = false;

	ImVec2 scenePos;
	ImVec2 sceneRegion;
	bool gizmoClicked = false;

public:

	/*
	* INDEX VARIABLES
	*/

	// File System
	int lastClickedItemID = 1;
	int lastSelectedEntityID = -1;

	/*
	* TEXTURE IDs
	*/

	ImGuizmo::OPERATION optype = ImGuizmo::OPERATION::TRANSLATE;

	unsigned int openFolderTextureID;
	unsigned int closedFolderTextureID;
	unsigned int plusTextureID;
	unsigned int greaterTextureID;
	unsigned int gameObjectTextureID;
	unsigned int transformTextureID;
	unsigned int meshrendererTextureID;
	unsigned int lightTextureID;
	unsigned int contextMenuTextureID;
	unsigned int eyeTextureID;
	unsigned int materialTextureID;
	unsigned int materialSmallTextureID;

	EditorGUI();

	void initImGui();

	void newFrameImGui();

	void renderImGui();

	void destroyImGui();

	void setTheme();

	void loadTextures();

	void updateStateMachine();

	void mainMenuBar();

	void createPanels();

	void createScenePanel();

	void setTransformOperation();

	void createConsolePanel();

	void createInspectorPanel();

	void addComponentButton();

	void showEntityName();

	void showTransformComponent();

	void showMeshRendererComponent(MeshRenderer& m_renderer);

	void showLightComponent();

	void showMaterialProperties(Material& material);

	void textureMenuPopup(Material& material, TextureType type, bool& flag);

	bool contextMenuPopup(ComponentType type);

	void createAppPanel();

	void createHierarchyPanel();

	void createSceneGraphRecursively(Transform* sceneGraph);

	void hiearchyCreateButton();

	void createFilesPanel();

	void showCurrentDirectoryText();

	void createFoldersRecursively(File * file);

	void createFilesPanelRightPart(ImVec2 area);

	bool mouseDistanceControl();

	void setEditor(Editor* editor);

	Editor* getEditor();

	void setFiles(std::vector<FileNode>* files);

	//std::map<int, FileNode>* getFiles();

	//void setSceneList(std::vector<Scene>* sceneList);

	//std::vector<Scene>* getSceneList();

};