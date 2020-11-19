#ifndef H_SCENE_H
#define H_SCENE_H

/*
[CLASS] 
[
    [NOME] 
    DEScene
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle varie scene(sequenze) all'interno
	della applicazione. Si può considerare scena, per esempio, la sequenza iniziale,
	i menu iniziali e i vari livelli di gioco.
]
*/
class DEScene
{	
	private:

		//** Variabili **
		STRUCT_DE_SCENE				sceneProperties;
		STRUCT_DE_SCENE_RUNTIMEINFO	sceneRuntimeinfo;
		STRUCT_DE_CLEAR				SceneClear;
		STRUCT_DE_DEBUG				SceneDebug;
		STRUCT_DE_FOG				SceneFog;
		STRUCT_DE_PERSPECTIVE		ScenePerspective;
		STRUCT_DE_GRIDS				SceneGrids;
		STRUCT_DE_GENERAL			SceneGeneral;
		D3DXMATRIX					matrixProj;
		D3DXMATRIX					matrixOrtho;	
		D3DXMATRIX					matrixIdentity;
		bool						bCreated;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		STRUCT_DE_FOV				sFOV;
		//Pannelli 16/9
		DEPanel						sceneCinematicPanel;
		bool						sceneUseCinematic;
		//PostProcess
		char						scenePostProcessTechnique[MAX_PATH];
		bool						scenePostProcessEnable;
		bool						scenePostProcessInclude2D;
		//Puntatore
		DEPanel						sceneCursor;
		bool						sceneCursorEnable;
		D3DXVECTOR2					sceneCursorPosition;
		int							sceneCursorTextureID;
		vector<int>					sceneCursorTextureList;
		//Physics
		STRUCT_DE_SCENE_PHYSICSPROPERTIES scenePhysicsProperties;
		NxScene						*scenePhysX;
		NxActor						*scenePhysXGround;
		vector<NxActor*>			scenePhysXTriggerActor;
		NxCompartment*				sceneCompartment;
		DEBoundingBox				triggerBoundingBox;
		DEBoundingSphere			triggerBoundingSphere;
		unsigned int				totalTriggers;
		bool						preparedPhysics;
		//Times
		float						Time;
		float						TimeLast;
		//Info
		unsigned int				totalVerticies;
		unsigned int				totalVerticiesCurrent;
		unsigned int				totalFaces;
		unsigned int				totalFacesCurrent;
		unsigned int				totalVerticiesPassCurrent;
		unsigned int				totalFacesPassCurrent;
		//Bounding
		DEBoundingBox				modelBoundingBox;
		DEBoundingSphere			modelBoundingSphere;
		DEAxes						modelAxes;
		DEBoundingSphere			lightBoundingSphere;
		DEBoundingBox				lightBoundingBox;
		DEBoundingSphere			soundBoundingSphere;
		//Griglia
		short						spatialGridPosition;
		DESpatialGrid				spatialGrid;
		//Assi
		DEAxes						mainAxes;
		//Griglia
		DEGrid						mainGrid;
		//Font
		DEFont						mainFont;
		//Camere
		vector<DECamera*>			camerasList;
		unsigned int				totalCameras;
		unsigned int				currentCameraID;
		//Luci
		vector<DELight*>			lightList;
		unsigned int				totalLight;
		unsigned int				totalActiveLights;
		//Modelli
		vector<DEModel*>			modelsList;
		unsigned int				totalModels;
		//Pannelli
		vector<DEPanel*>			panelsList;
		unsigned int				totalPanels;
		//Sistemi particelari
		vector<DEParticleSystem*>	particlesList;
		unsigned int				totalParticles;
		//Suoni
		vector<DESound*>			soundsList;
		unsigned int				totalSounds;
		//Gruppi
		vector<DESoundChannelGroup*>soundsGroupList;
		unsigned int				totalSoundsGroup;
		//Flares
		vector<DELensFlare*>		flaresList;
		unsigned int				totalFlares;
		//SkyBox
		vector<DESky*>				skyList;
		unsigned int				totalSky;
		//Font
		vector<DEFont*>				fontsList;
		unsigned int				totalFonts;
		//Pausa
		bool						isPaused;
		//HWND
		HWND						mainHWND;
		bool						mainWindowed;
		unsigned int				mainWidth;
		unsigned int				mainHeight;

		//Post processing
		#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)
		struct Vertex
		{
			float x, y, z;
			float tu, tv;
		};
		LPD3DXEFFECT				g_pEffect;
		IDirect3DTexture9*			g_pRenderTargetTexture;
		IDirect3DTexture9*			g_pNormalTexture;
		IDirect3DSurface9*			g_pRenderTargetSurface;
		IDirect3DSurface9*			g_pFrameBufferSurface;
		LPDIRECT3DVERTEXBUFFER9		g_pVertexBuffer;
		LPDIRECT3DVERTEXBUFFER9		g_pFullScreenVertexBuffer;
		bool						postProcessIsCreated;

		//Default Render States Blocks
		IDirect3DStateBlock9*		sceneDefaultPixelstateBlock;
		IDirect3DStateBlock9*		sceneDefaultVertexstateBlock;


	public:	
		//** Variabili **
		bool								bLoaded;
		//** Funzioni **
		DLL_API								DEScene();
		DLL_API bool						Create(STRUCT_DE_SCENE newSceneProperties, LPCSTR logTitle);
		DLL_API bool						CreateDefaultRenderstatesBlocks();
		DLL_API bool						CreatePostProcessing();
		DLL_API void						PreparePostProcessing();
		DLL_API void						ExecutePostProcessing();
		//Post processing
		DLL_API void						SetPostProcessingTechnique(LPCTSTR techniqueName);
		DLL_API void						SetPostProcessingEnable(bool enable);
		DLL_API void						SetPostProcessingInclude2D(bool include);
		DLL_API LPCTSTR						GetPostProcessingTechnique();
		DLL_API bool						GetPostProcessingEnable();
		DLL_API bool						GetPostProcessingInclude2D();
		DLL_API LPD3DXEFFECT				GetPostProcessing();
		//16/9
		DLL_API void						SetCinematicEffect(bool useCinematic);
		DLL_API bool						GetCinematicEffect();
		//Physics
		DLL_API void						SetPhysicsProperties(STRUCT_DE_SCENE_PHYSICSPROPERTIES newProperties);
		DLL_API STRUCT_DE_SCENE_PHYSICSPROPERTIES GetPhysicsProperties();

		//Puntatore mouse
		DLL_API void						SetCursorEnable(bool enable);
		DLL_API bool						GetCursorEnable();
		DLL_API void						SetCursorPosition(D3DXVECTOR2 position);
		DLL_API D3DXVECTOR2					GetCursorPosition();
		DLL_API void						SetCursorTextureID(unsigned int textureID);
		DLL_API unsigned int				GetCursorTextureID();
		//Camera
		DLL_API bool						SetCurrentCamera(unsigned int currentCameraID);
		DLL_API unsigned int				GetCurrentCameraID();
		DLL_API DECamera*					GetCurrentCamera();
		//Inserimento
		DLL_API unsigned int				AddCamera(DECamera *newCamera);
		DLL_API unsigned int				AddLight(DELight *newLight);
		DLL_API bool						AddModel(DEModel *newModel);
		DLL_API bool						RemoveModel(DEModel *removeModel);
		DLL_API unsigned int				AddPanel(DEPanel *newPanel);
		DLL_API unsigned int				AddParticlesSystem(DEParticleSystem *newParticlesSystem);
		DLL_API unsigned int				AddSound(DESound *newSound);
		DLL_API unsigned int				AddSoundGroup(DESoundChannelGroup *newSoundGroup);
		DLL_API unsigned int				AddLensFlare(DELensFlare *newLensFlare);
		DLL_API unsigned int				AddSky(DESky *newSky);
		DLL_API unsigned int				AddFont(DEFont *newFont);
		//Prepare
		DLL_API void						PreparePhysics();
		DLL_API	NxSceneStats				GetPhysXStats() ;
		DLL_API bool						SetGroupCollisionFlag(unsigned int group1, unsigned int group2, bool collide);
		DLL_API bool						GetGroupCollisionFlag(unsigned int group1, unsigned int group2);
		//Update
		DLL_API void						UpdatePhysics();
		DLL_API bool						UpdateInput();
		DLL_API void						UpdateView();
		//Load
		DLL_API bool						Load();
		DLL_API bool						Execute();
		DLL_API bool						Execute3D();
		DLL_API bool						Execute2D();
		DLL_API bool						Execute2DInfo();
		DLL_API bool						Release();
		DLL_API bool						Restore();
		DLL_API bool						Invalidate();
		//Creazione
		DLL_API bool						GetCreated();
		DLL_API bool						isLoaded();
		//Info
		DLL_API STRUCT_DE_FOV				GetFOV();
		DLL_API LPCSTR						GetName();
		//Pausa
		DLL_API void						SetPause(bool pause);
		DLL_API void						TogglePause();
		DLL_API bool						GetPause();
		//HWND
		DLL_API void						SetHWND(HWND associtedHWND);
		DLL_API HWND						GetHWND();
		//Windowed
		DLL_API void						SetWindowed(bool windowed);
		DLL_API bool						GetWindowed();
		//Dimensioni
		DLL_API void						SetWidth(unsigned int width);
		DLL_API unsigned int				GetWidth();
		DLL_API void						SetHeight(unsigned int height);
		DLL_API unsigned int				GetHeight();
		//Proprietà principali
		DLL_API STRUCT_DE_SCENE				GetSceneProperties();
		DLL_API STRUCT_DE_SCENE_RUNTIMEINFO GetRuntimeInfo();
		//Proprietà
		DLL_API STRUCT_DE_DEBUG				GetDebugProperties();
		DLL_API bool						SetDebugProperties(STRUCT_DE_DEBUG);
		DLL_API STRUCT_DE_GENERAL			GetGeneralProperties();
		DLL_API bool						SetGeneralProperties(STRUCT_DE_GENERAL);
		DLL_API STRUCT_DE_CLEAR				GetClearProperties();
		DLL_API bool						SetClearProperties(STRUCT_DE_CLEAR);
		DLL_API STRUCT_DE_FOG				GetFogProperties();
		DLL_API bool						SetFogProperties(STRUCT_DE_FOG);
		DLL_API STRUCT_DE_GRIDS				GetGridsProperties();
		DLL_API bool						SetGridsProperties(STRUCT_DE_GRIDS);
		DLL_API STRUCT_DE_PERSPECTIVE		GetPerspectiveProperties();
		DLL_API bool						SetPerspectiveProperties(STRUCT_DE_PERSPECTIVE);
		//Matrici
		DLL_API D3DXMATRIX					GetIdentityMatrix();
		DLL_API void						SetIdentityMatrix(D3DXMATRIX newMatrix);
		DLL_API D3DXMATRIX					GetOrthoMatrix();
		DLL_API void						SetOrthoMatrix(D3DXMATRIX newMatrix);
		DLL_API D3DXMATRIX					GetProjectionMatrix();
		DLL_API void						SetProjectionMatrix(D3DXMATRIX newMatrix);
		//Tag
		DLL_API bool						SetTag(LPCSTR newTag);
		DLL_API LPCSTR						GetTag();
		//Trigger
		DLL_API void						MoveTrigger(unsigned int triggerID, D3DXVECTOR3 position);
		DLL_API unsigned int				AddTrigger_Box(D3DXVECTOR3 pos, D3DXVECTOR3 dim);
		DLL_API unsigned int				AddTrigger_Sphere(D3DXVECTOR3 pos, float radius);
};


#endif