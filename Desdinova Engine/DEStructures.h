#ifndef H_STRUCTURES_H
#define H_STRUCTURES_H

//************************
//** STRUTTURE GENERALI **
//************************

//Swap chains
typedef struct STRUCT_DE_SWAPCHAIN
{
	D3DPRESENT_PARAMETERS			presentParameters;
	HWND							hWnd;
	bool							isLost;
	bool							isCreated;
	unsigned int					currenteScene;
	unsigned int					totalScenes;
	vector<DEScene*>				scene;
	bool							isEmpty;
}STRUCT_DE_SWAPCHAIN;


//Texture
typedef struct STRUCT_DE_TEXTURE
{
	//Texture
	LPDIRECT3DTEXTURE9	Texture_Pointer;
	D3DXIMAGE_INFO		Texture_Info;
	char				Texture_FileName[MAX_PATH];
	unsigned int		Texture_Pos;
	//Proprietà
	DWORD				Texture_KeyColour;
	bool				Texture_MipMaps;
}STRUCT_DE_TEXTURE;


//Model Information
typedef struct STRUCT_DE_MODELINFO
{
	//Componenti principali del modello
	int							Info_ArrayID;
	int							Info_MaterialID;
	int							Info_MeshID;
	int							Info_ModelID;
	int							Info_TextureID[3];
	//Matrici bounding
	D3DXMATRIX					Info_BoundingBoxMatrix;
	D3DXMATRIX					Info_BoundingSphereMatrix;
	D3DXMATRIX					Info_AxesMatrix;
	//Matrici
	D3DXMATRIX					Info_FinalMatrix;
	D3DXMATRIX					Info_FinalMatrixNoScale;
	//Posizione
	D3DXVECTOR3					Info_Position;
	D3DXVECTOR3					Info_Rotation;
	D3DXVECTOR3					Info_Scale;
	//Dimensioni finali scalate o modificate
	D3DXVECTOR3					Info_CenterCoord;
	D3DXVECTOR3					Info_CenterCoord2D;
	D3DXVECTOR3					Info_CenterCoord3D;
	D3DXVECTOR3					Info_MinCoord;
	D3DXVECTOR3					Info_MaxCoord;
	float						Info_Radius;
	float						Info_Width;
	float						Info_Height;
	float						Info_Depth;
	float						Info_Radius_Scaled;
	float						Info_Width_Scaled;
	float						Info_Height_Scaled;
	float						Info_Depth_Scaled;
	//Correnti
	bool						Info_IsCreated;
	bool						Info_IsFar;
	bool						Info_IsPicked;
	bool						Info_IsCulled;
	bool						Info_IsTrascured;
	bool						Info_CellIsVisible;
	bool						Info_CellIsNear;
	//Altro
	float						Info_DistanceFromCamera;
	unsigned int				Info_CurrentLod;
	//Logs
	LPCSTR						Info_LogTitle;
	LPCSTR						Info_Tag;

}STRUCT_DE_MODELINFO;


//Model properties
typedef struct STRUCT_DE_MODELPROPERTIES
{
	unsigned int				Model_Pos;
	char						Model_Filename[MAX_PATH];
	//Nome
	char						Model_Name[MAX_PATH];
	//Files
	char						Model_File_Mesh[MAX_PATH];
	char						Model_File_Material[MAX_PATH];
	char						Model_File_Shader[MAX_PATH];
	//Textures
	char						Model_Texture_Filename0[MAX_PATH];
	char						Model_Texture_Filename1[MAX_PATH];
	char						Model_Texture_Filename2[MAX_PATH];
	DWORD						Model_Texture_ColorKey0;
	DWORD						Model_Texture_ColorKey1;
	DWORD						Model_Texture_ColorKey2;
	bool						Model_Texture_MipMaps0;
	bool						Model_Texture_MipMaps1;
	bool						Model_Texture_MipMaps2;
	float						Model_Texture_BlendFactor;
	//Proprietà
	bool						Model_IsScreenAligned;
	bool						Model_IsPickable;
	bool						Model_IsCullable;
	bool						Model_IsTrascurable;
	//Debug
	int							Model_Debug_FillMode;			//0=Solid, 1=Wireframe, 2=Point 
	int							Model_Debug_BoundingType;		//0=No bounding, 1=Box, 2=Sphere
	int							Model_Debug_ShowAxes;			//0=No axes, 1=Show axes
	//Info
	char						Model_Info_Author[MAX_PATH];
	char						Model_Info_Date[MAX_PATH];
	char						Model_Info_Revision[MAX_PATH];
	char						Model_Info_Comment[MAX_PATH];
	char						Model_Info_Other[MAX_PATH];
	//Physics
	float						Model_Physics_Mass;
	int							Model_Physics_Class;
	bool						Model_Physics_CCD;
	float						Model_Physics_CCDScale;
	float						Model_Physics_CCDMotionThreshold;
	bool						Model_Physics_Kinematic;
	//Other
	D3DXVECTOR3					Model_Scale;

}STRUCT_DE_MODELPROPERTIES;


//Materiale
typedef struct STRUCT_DE_MATERIAL
{
	//Info Materiale
	char				Material_Filename[MAX_PATH];
	unsigned int		Material_Pos;
	//Materiale 
	char				Material_Name[MAX_PATH];
	float				Material_Density;			//See table of densities
	float				Material_Restitution;		//Not set to 1.0 exactly
	float				Material_StaticFriction;	//Can be higher than 1.
	float				Material_DynamicFriction;	//Should not be higher than 1.
	bool				Material_IsAnisotropic;
	float				Material_DynamicFrictionV;	//If anisotropic is True
	float				Material_StaticFrictionV;	//If anisotropic is True
	D3DXVECTOR3			Material_DirOfAnisotropy;	//If anisotropic is True
	bool				Material_UseDirectionalLights;
	bool				Material_UsePointLights;	
	bool				Material_IsSpecular;
	bool				Material_IsEnvironmental;
	bool				Material_IsTransparent;
	bool				Material_IsBumpmapped;
	D3DXCOLOR			Material_Ambient;
	D3DXCOLOR			Material_Diffuse;
	float				Material_Shininess;
	float				Material_Alpha;

}STRUCT_DE_MATERIAL;


//Modello
typedef struct STRUCT_DE_MESH
{
	//Modello
	LPD3DXPMESH			Mesh_Progressive;
	LPD3DXMESH			Mesh_Point;
	char				Mesh_Filename[MAX_PATH];
	unsigned int		Mesh_Pos;
	//General Value
	unsigned int		Mesh_Parts;
	//Bounding
	D3DXVECTOR3			Mesh_minCoord;
	D3DXVECTOR3			Mesh_maxCoord;
	D3DXVECTOR3			Mesh_centerCoord;
	D3DXVECTOR3			Mesh_pivot;
	float				Mesh_radius;	
	float				Mesh_height;
	float				Mesh_width;
	float				Mesh_depth;
}STRUCT_DE_MESH;


//Sistema particellare
typedef struct STRUCT_DE_PARTICLES
{
	char			Particle_TextureFile[MAX_PATH];
	float			Particle_Max;
	float			Particle_Release;
	float			Particle_Interval;
	float			Particle_Life;
	float			Particle_Size;
	float			Particle_VelocityVar;
	bool			Particle_Resistence;
	D3DXCOLOR		Particle_Color;
	D3DXVECTOR3		Particle_Position;
	D3DXVECTOR3		Particle_Velocity;
	D3DXVECTOR3		Particle_Gravity;
	D3DXVECTOR3		Particle_Wind;	
	ENUM_DE_PSTYPE	Particle_Type;
	D3DXVECTOR3		Particle_Distance;
	bool			Particle_RelativeY;
}STRUCT_DE_PARTICLES;


//Visualization
typedef struct STRUCT_DE_GENERAL
{
	D3DXCOLOR			General_AmbientColor;
	D3DXVECTOR3			General_Wind;
	//...
}STRUCT_DE_GENERAL;

//Fov
typedef struct STRUCT_DE_FOV
{
	D3DXVECTOR4			Fov_LeftPlane;
	D3DXVECTOR4			Fov_RightPlane;
	D3DXVECTOR4			Fov_TopPlane;
	D3DXVECTOR4			Fov_BottomPlane;
	float				Fov_FrontClip;
	float				Fov_BackClip;
}STRUCT_DE_FOV;

//Clear
typedef struct STRUCT_DE_CLEAR
{
	DWORD				Clear_RectsCount;
	D3DRECT*			Clear_Rects;
	DWORD				Clear_ClearFlags;
	D3DCOLOR			Clear_EnvironmentColor;
	float				Clear_Z;
	DWORD				Clear_Stencil;
}STRUCT_DE_CLEAR;

//Perspective
typedef struct STRUCT_DE_PERSPECTIVE
{
	float				Perspective_Angle;
	float				Perspective_Near;
	float				Perspective_Far;
}STRUCT_DE_PERSPECTIVE;

//Nebbia
typedef struct STRUCT_DE_FOG
{
	D3DXCOLOR			Fog_Color;
    float				Fog_Start;
	float				Fog_Range;
	bool				Fog_Enable;
}STRUCT_DE_FOG;

//Luce
typedef struct STRUCT_DE_LIGHT
{
	ENUM_DE_LIGHTTYPE	Light_Type;
	D3DXCOLOR			Light_Color;
	D3DXVECTOR3			Light_Position;
	D3DXVECTOR3			Light_Direction;
	float				Light_Falloff;
	float				Light_Range;
	bool				Light_Enable;
	bool				Light_DinamicEnable;
}STRUCT_DE_LIGHT;

//Proprietà visualizzazione di debug
typedef struct STRUCT_DE_DEBUG
{
	short				Debug_FillMode;				//-1=No overwrite, 0=Solid, 2=Wireframe, 1=Point 
	short				Debug_UseLighting;			//-1=No overwrite, 0=No lighting, 1=Force lighting
	short				Debug_UseFrustrumCulling;	//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
	short				Debug_UseLOD;				//-1=No overwrite, 0=No lods, 1=Force lods
	short				Debug_BoundingType;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
	short				Debug_ShowAxes;				//-1=No overwrite, 0=No axes, 1=Show axes
	
	bool				Debug_ShowLights;			//0=No lights, 1=Show lights
	bool				Debug_ShowSounds;			//0=No sounds, 1=Show sounds
	bool				Debug_ShowCenterAxes;		//0=No axes, 1=Show axes
	bool				Debug_ShowSpatialGrid;		//0=No grid, 1=Show grid
	bool				Debug_ShowGroundGrid;		//0=No grid, 1=Show grid
	bool				Debug_ShowTriggers;			//0=No triggers, 1=Show triggers
	bool				Debug_ShowInfo;				//0=No info, 1=Show info
}STRUCT_DE_DEBUG;



//Griglie
typedef struct STRUCT_DE_GRIDS
{
	unsigned short		GridsProperties_SpatialGrid_DimensionCell;
	unsigned short		GridsProperties_SpatialGrid_PerSideCells;
	unsigned short		GridsProperties_GroundGrid_DimensionCell;
	unsigned short		GridsProperties_GroundGrid_PerSideCells;
}STRUCT_DE_GRIDS;


//Proprietà scena
typedef bool (* PFUNCSCENE_BOOL)();
typedef struct STRUCT_DE_SCENE
{
	//Nome
	LPCSTR						Scene_Name;
	//Puntatori a funzione
	PFUNCSCENE_BOOL				Scene_FuncPointer_Load;
	PFUNCSCENE_BOOL				Scene_FuncPointer_Input;
	PFUNCSCENE_BOOL				Scene_FuncPointer_Render3D;
	PFUNCSCENE_BOOL				Scene_FuncPointer_Render2D;
	PFUNCSCENE_BOOL				Scene_FuncPointer_Invalidate;
	PFUNCSCENE_BOOL				Scene_FuncPointer_Restore;
	PFUNCSCENE_BOOL				Scene_FuncPointer_Release;
	//Tipo di scena fisica
	NxSimulationType			Scene_Physics_Type;
	//Post Process
	LPCSTR						Scene_PostProcess_Filename;
	//16/9
	LPCSTR						Scene_CineEffect_Filename;
	//Puntatore
	LPCSTR						Scene_Cursor_Filename[50];
}STRUCT_DE_SCENE;


typedef struct STRUCT_DE_SCENE_PHYSICSPROPERTIES
{
	bool				Scene_PhysicsProperties_Ground;
	D3DXVECTOR3			Scene_PhysicsProperties_GroundNormal;
	float				Scene_PhysicsProperties_GroundY;

	float				Scene_PhysicsProperties_StepSize;
	int					Scene_PhysicsProperties_NumIterations;
	D3DXVECTOR3			Scene_PhysicsProperties_Gravity;

	float				Scene_PhysicsProperties_DefaultMaterial_Restitution;
	float				Scene_PhysicsProperties_DefaultMaterial_StaticFriction;
	float				Scene_PhysicsProperties_DefaultMaterial_DynamicFriction ;
	float				Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV;
	float				Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV;
	D3DXVECTOR3			Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy;
	float				Scene_PhysicsProperties_DefaultMaterial_Flags;

}STRUCT_DE_SCENE_PHYSICSPROPERTIES;



typedef struct STRUCT_DE_SCENE_RUNTIMEINFO
{
	unsigned int	Scene_RuntimeInfo_TotalCameras;
	unsigned int	Scene_RuntimeInfo_TotalModels;
	unsigned int	Scene_RuntimeInfo_TotalPanels;
	unsigned int	Scene_RuntimeInfo_TotalParticles;
	unsigned int	Scene_RuntimeInfo_TotalSoundGroup;
	unsigned int	Scene_RuntimeInfo_TotalSounds;
	unsigned int	Scene_RuntimeInfo_TotalFlares;
	unsigned int	Scene_RuntimeInfo_TotalSky;
	unsigned int	Scene_RuntimeInfo_TotalFonts;

	unsigned int	Scene_RuntimeInfo_TotalLightsDirectional;
	unsigned int	Scene_RuntimeInfo_TotalLightsOmni;
	unsigned int	Scene_RuntimeInfo_TotalLightsSpot;
	unsigned int	Scene_RuntimeInfo_TotalLights;
	unsigned int	Scene_RuntimeInfo_TotalLightsActive;

	unsigned int	Scene_RuntimeInfo_TotalVertices;
	unsigned int	Scene_RuntimeInfo_TotalFaces;
	unsigned int	Scene_RuntimeInfo_TotalVerticesVisible;
	unsigned int	Scene_RuntimeInfo_TotalFacesVisible;
	unsigned int	Scene_RuntimeInfo_TotalVerticesPassVisible;
	unsigned int	Scene_RuntimeInfo_TotalFacesPassVisible;
	unsigned int	Scene_RuntimeInfo_CurrentCameraID;
	short			Scene_RuntimeInfo_CurrentCameraGridPosition;
	D3DXVECTOR3		Scene_RuntimeInfo_CurrentCameraPosition;
	D3DXVECTOR3		Scene_RuntimeInfo_CurrentCameraView;

	unsigned short	Scene_RuntimeInfo_CurrentSpatialCellsVisible;
	unsigned short	Scene_RuntimeInfo_TotalSpatialCells;

}STRUCT_DE_SCENE_RUNTIMEINFO;



//***************
//**  CONSOLE  **
//***************
typedef struct STRUCT_DE_CONSOLEFUNCTION
{
	vector<ENUM_DE_CONSOLEPARAM>	params;
    void*							proc;
}STRUCT_DE_CONSOLEFUNCTION;





//****************************************
//** STRUTTURE PER LE DEFINIZIONI VIDEO **
//****************************************

//Multisample
typedef struct STRUCT_DE_MULTISAMPLE
{
	D3DMULTISAMPLE_TYPE			Type;					//Tipo di multisample
	DWORD						QualityLevels;			//Livelli di qualità
}STRUCT_DE_MULTISAMPLE;

//Formati incompatibili con il multisample
typedef struct STRUCT_DE_MSFCONFLICT
{
    D3DFORMAT					Format;					//Formato
    D3DMULTISAMPLE_TYPE			Type;					//Multisample
}STRUCT_DE_MSFCONFLICT;

//Dimensioni e refrash rate
typedef struct STRUCT_DE_DISPLAYINFO
{
	UINT						Width;					//Larghezza
	UINT						Height;					//Altezza
	vector<UINT>				RefreshRateList;		//Lista di refrash rate supportati
}STRUCT_DE_DISPLAYINFO;

//Display mode
typedef struct STRUCT_DE_DISPLAYMODE
{
	D3DFORMAT							Format;					//Formato
	vector<STRUCT_DE_DISPLAYINFO>		WidthHeightList;		//Lista di dimensioni e refresh rate
}STRUCT_DE_DISPLAYMODE;

//Formato dell'adapter
typedef struct STRUCT_DE_ADAPTERFORMAT
{
	UINT								ID;						//ID del formato
	UINT								AdapterID;				//Adapter del formato
	bool								Windowed;				//Finestra o meno
	D3DFORMAT							Format;					//Formato
	D3DDEVTYPE							DeviceType;				//HAL o REF

	vector<D3DFORMAT>					BackBuffersList;		//Formato del backbuffer
	vector<STRUCT_DE_MSFCONFLICT>		ConflictsList;			//Conflitti con il multisample
    vector<STRUCT_DE_MULTISAMPLE>		MultisamplesList;		//Tipi di multisample
    vector<D3DFORMAT>					DepthStencilList;		//Formati del depth/stencil
	vector<DWORD>						VertexProcessingList;	//Tipo di vertex processing
	
	STRUCT_DE_ADAPTERFORMAT& operator = ( const STRUCT_DE_ADAPTERFORMAT& rhs );
}STRUCT_DE_ADAPTERFORMAT;

//Adapter
typedef struct STRUCT_DE_ADAPTER
{
    UINT								ID;						//ID dell'adapter
	D3DCAPS9							Caps;					//Caps dell'adapter
	D3DDEVTYPE							DeviceType;				//HAL / REF
	D3DADAPTER_IDENTIFIER9				Info;					//Informazioni sull'adapter
	vector<STRUCT_DE_ADAPTERFORMAT>		FormatsList;			//Lista di formati
	vector<STRUCT_DE_DISPLAYMODE>		DisplayModesList;		//Lista di display modes
	vector<UINT>						PresentIntervalsList;	//Lista di presentation intervals
}STRUCT_DE_ADAPTER;

//Device
typedef struct STRUCT_DE_DEVICESETTINGS
{
	unsigned int			adapterID;
	D3DPRESENT_PARAMETERS	presentParameters;
	DWORD					vertexProcessing;
	int						minVertexShaderMaj;
	int						minVertexShaderMin;
	int						minPixelShaderMaj;
	int						minPixelShaderMin;

}STRUCT_DE_DEVICESETTINGS;

#endif