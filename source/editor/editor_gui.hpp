#pragma once

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <stack>

#include <GLM/gtc/type_ptr.hpp>

#include <math.hpp>
#include <input.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imguizmo/imguizmo.h>

#include <gamecamera.hpp>
#include "debugrenderer.hpp"

using namespace Material;

class Editor;

struct EditorIcons {

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
	unsigned int rigidbodyTextureID;
	unsigned int colliderTextureID;
	unsigned int startTextureID;
	unsigned int stopTextureID;
	unsigned int pauseTextureID;
	unsigned int pmatSmallTextureID;
};

struct EditorColors {

	ImVec4 textColor;
	ImVec4 textSelectedColor;
	ImVec4 textUnselectedColor;
};

struct FileSystemControlVars {

	int lastSelectedItemID = -1;
	int temp_lastClickedItemID = 1;
	int lastRightClickedItemID = -1;
	int renameItemID = -1;

	bool folderLineClicked = false;
	bool mouseLeftPressed = false;
	bool toggleClicked = false;
	bool entered = false; // sadece kontrol icin bir flag
	bool panelRightItemClicked = false; // tiklayip cektikten sonra
	bool panelRightItemTab = false; // tiklar tiklamaz
	bool subfolderCheckFlag = false;
	bool fileTreeClicked = false;
};

class EditorGUI {

private:

	Editor* editor;

	EditorIcons editorIcons;
	EditorColors editorColors;
	FileSystemControlVars fileSystemControlVars;
	/*
	* INDEX VARIABLES
	*/

	// File System
	

	// Scene Graph
	Entity* renameEntity = NULL;

	ImVec2 cursorPosWhenFirstClickedItem;

	/*
	* FLAG VARIABLES
	*/

	// File System
	

	// Scene Graph
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
	bool materialChanged = false;
	bool physicMaterialChanged = false;
	bool firstCycle = true;
	
	bool gizmoClicked = false;

public:

	std::string statusMessage = "Ready";

	ImVec2 scenePos;
	ImVec2 sceneRegion;

	/*
	* INDEX VARIABLES
	*/

	// File System
	int lastClickedItemID = 1;
	//int lastSelectedEntityID = -1;
	Entity* lastSelectedEntity = NULL;

	/*
	* TEXTURE IDs
	*/

	ImGuizmo::OPERATION optype = ImGuizmo::OPERATION::TRANSLATE;

	BoxColliderRenderer* bcr = NULL;
	SphereColliderRenderer* scr = NULL;
	//CameraRenderer* camRenderer = NULL;

	// terrain component vars
	int orderForTerrainTexture;
	unsigned int albedoTemp;

	ImVec2 gameCameraRegion;

	EditorGUI(Editor* editor);

	void init();

	void initImGui();

	void newFrameImGui();

	void update();

	void renderImGui();

	void destroyImGui();

	void setTheme();

	void loadTextures();

	void handleInputs();

	void mainMenuBar();

	void secondaryMenuBar();

	void createPanels();

	void createScenePanel();

	void setTransformOperation();

	void createConsolePanel();

	void createGamePanel();

	void createInspectorPanel();

	void addComponentButton();

	void showEntityName();

	void showGameCameraComponent(GameCamera* camComp, int index);

	void showTransformComponent(int index);

	void showMeshRendererComponent(MeshRenderer* meshRendererComp, int index);

	void showTerrainGeneratorComponent(TerrainGenerator* terrainComp, int index);

	template <typename T>
	T dragUnitValueAssign(T min, T max, T value);

	void showLightComponent(int index);

	void showRigidbodyComponent(int index);

	template<class T>
	void showColliderPhysicMaterial(T* colliderComp, float windowWidth, int frameIndex);

	void showMeshColliderComponent(MeshCollider* meshColliderComp, int index, int meshColliderIndex);

	void showBoxColliderComponent(BoxCollider* boxColliderComp, int index, int boxColliderIndex);

	void showCapsuleColliderComponent(CapsuleCollider* capsuleColliderComp, int index, int capsuleColliderIndex);

	void showSphereColliderComponent(SphereCollider* sphereColliderComp, int index, int sphereColliderIndex);

	void showMaterialProperties(MaterialFile& material, int index);

	void showPhysicMaterialProperties(PhysicMaterialFile& mat);

	void textureMenuPopup(MaterialFile& material, int index, bool& flag);

	void textureMenuPopupForTerrain(TerrainGenerator* terrain, bool& flag, int& order);

	//void heightMapPopup(TerrainGenerator* terrainComp);

	bool contextMenuPopup(ComponentType type, int index);

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

};