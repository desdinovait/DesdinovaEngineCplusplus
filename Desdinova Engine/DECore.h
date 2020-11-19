#ifndef H_CORE_H
#define H_CORE_H

#include "DEInput.h"
#include "DESoundSystem.h"

typedef bool (*pDeviceFeatureValidationFunc)( D3DCAPS9* pCaps, D3DFORMAT adapterFormat, DWORD dwBehavior, IDirect3D9* pD3D );

/*
[CLASS] 
[
    [NOME] 
    DECore
    
    [DESCRIZIONE]   
    Classe generale del motore grafico(Desdinova Engine) contenente tutte le funzioni
	necessarie per creare l'ambiente Direct3D e la gestione delle scene che
	comporranno l'applicazione.
]
*/
class DECore
{
	private:
		//** Strutture e altre definizioni **
		#define CURRENT_VERSION	"0.8.4"

		//** Variabili **
		//Generale
		DECpuInfo							engineCPUInfo;
		//Oggetti DirectX
		IDirect3D9*							D3DObject;
		D3DPRESENT_PARAMETERS				enginePresentParameters;
		DEInput								engineInput;
		DEConsole							engineConsole;
		DESoundSystem						engineSound;
		//Swapchain
		unsigned int						engineCurrentSceneID;
		unsigned int						engineCurrentSceneReleaseID;
		bool								engineCurrentSceneRelease;
		STRUCT_DE_SWAPCHAIN					engineSwapChain;
		LPDIRECT3DSURFACE9					engineCurrentBackBuffer;
		//Finestra
		HINSTANCE							enginehInstance;
		LPCTSTR								engineWindowTitle;
		LPCTSTR								engineClassName;
		DLGPROC								engineProc;
		HWND								engineHWND;					//Finestra principale
		HWND								engineRenderHWND;			//Finestra di rendering effettiva(diversa se si renderizza in un controllo)
		bool								engineFromFile;
		bool								engineSetSettings;

		unsigned int						engineAdapterID;
		unsigned int						engineFormatID;
		
		D3DFORMAT							engineBackbufferFormat;
		unsigned int						engineVertexProcessingID;	//0=Software, 1=Hardware, 2=Mixed, 3=PureHardware
		DWORD								engineVertexProcessingType;
		unsigned int						engineMultisampleType;		//0=None, 1=NonMaskable, 2=Type2 3=Type3 ... 16=Type16	
		unsigned int						engineMultisampleQuality;
		bool								engineSync;
	
		int									engineMinVertexShaderMaj;
		int									engineMinVertexShaderMin;
		int									engineMinPixelShaderMaj;
		int									engineMinPixelShaderMin;

		bool								engineUsePhysXHardware;

		unsigned int						engineMinWidth;
		unsigned int						engineMinHeight;
		unsigned int						engineMinBPP;
		RECT								engineDesktopRect;
		unsigned int						engineDesktopBPP;

		float								engineGammaValue;
		//Creazione
		bool								engineObjectCreated;
		bool								engineWindowCreated;
		bool								engineDeviceCreated;
		//Framerate
		long								engineFPSTimebase;
		long								engineFPSTime; 
		long								engineFPSFrame;
		float								engineFPS;
		float								engineFPSMillisec;
		float								engineFPSMax;
		float								engineFPSMin;
		//Font
		DEFont								engineFont;
		//Info
		bool								engineShowInfo;
		bool								engineShowFPS;
		//Funzioni
		static pDeviceFeatureValidationFunc	DeviceFeatureFunction;
		//Controllo uscita
		bool								engineDone;	


		//** Funzioni **
		//Post rendering(info)
		DLL_API bool						PostRender();
		//Screenshot
		DLL_API void						GetScreenshotFileName(string& FileName);
		//Log States
		DLL_API void						LogRenderStates(ENUM_DE_LOGRENDERSTATES Flags);
		DLL_API void						LogTextureStageState(int Stage);
		//Liste
		DLL_API void						DetermineAdapterBehaviour( STRUCT_DE_ADAPTERFORMAT* pFormat, D3DCAPS9* pCaps, bool supportMixedVP=true);
		DLL_API void						BuildPresentationList( STRUCT_DE_ADAPTER* pAdapter );
		DLL_API void						BuildWidthHeightList( STRUCT_DE_ADAPTER* pAdapter );
		DLL_API void						BuildBackBufferList( STRUCT_DE_ADAPTERFORMAT* pFormat );
		DLL_API void						BuildConflictList( STRUCT_DE_ADAPTERFORMAT* pFormat );
		DLL_API void						BuildMultisampleList( STRUCT_DE_ADAPTERFORMAT* pFormat );
		DLL_API void						BuildDepthStencilList( STRUCT_DE_ADAPTERFORMAT* pFormat );

	public:		

		vector<STRUCT_DE_ADAPTER>			engineAdapters;

		//** Funzioni **
		//Variabili statiche
		static IDirect3DDevice9*			D3DDevice;
		static NxPhysicsSDK*				enginePhysX;
		static NxCookingInterface*			enginePhysXCooking;
		static LPD3DXEFFECT					engineEffect;

		//Creazione
		DLL_API bool						Init(char *logFile, char *logTitle, char *logSubtitle);
		DLL_API HWND						CreateMainWindow(HINSTANCE hInstance, DLGPROC Proc, char *ClassName, char *WindowTitle, bool centerWindow=true);
		DLL_API HWND						CreateMainWindow(HINSTANCE hInstance, char *WindowIDResource, DLGPROC Proc, char *WindowTitle, bool centerWindow=true);
		DLL_API bool						Release();
		//Loop generale
		DLL_API bool						Loop();
		DLL_API void						ExitLoop();
		//Scene
		DLL_API bool						AddScene(DEScene *newCurrentScene);
		DLL_API bool						SetCurrentScene(unsigned int sceneID, bool releaseCurrentScene=false);		
		//Riconvalida
		DLL_API bool						CreateDevice();
		DLL_API bool						TestDevice();
		DLL_API bool						ResetDevice();
		DLL_API bool						ResizeDevice(unsigned int newWidth, unsigned int newHeight);
		//Creazione
		DLL_API bool						isObjectCreated();
		DLL_API bool						isDeviceCreated();
		DLL_API bool						isWindowCreated();
		//Setta proprietà
		DLL_API bool						ExistSettingsFile(char *fileName);
		DLL_API bool						SaveSettingsFile(char *fileName, STRUCT_DE_DEVICESETTINGS deviceSettings);
		DLL_API STRUCT_DE_DEVICESETTINGS*	LoadSettingsFile(char *fileName);
		DLL_API bool						SetSettingsFromFile(char *filename);
		DLL_API bool						SetSettingsFromParams(STRUCT_DE_DEVICESETTINGS setDeviceSettings);
		//Gamma ramp
		DLL_API bool						SetGammaValue(float gammaValue, bool calibrateGamma);
		DLL_API	float						GetGammaValue();
		DLL_API D3DGAMMARAMP*				GetGammaRamp();
		//Info
		DLL_API inline LPDIRECT3DDEVICE9	GetD3DDevice();
		DLL_API inline LPDIRECT3D9			GetD3DObject();
		DLL_API inline DEInput				GetDInput();
		DLL_API inline DEConsole			GetConsole();
		DLL_API inline DESoundSystem		GetSoundSystem();
		DLL_API vector<STRUCT_DE_ADAPTER>	GetAdapter();
		DLL_API HWND						GetHWND();
		DLL_API HWND						GetRenderHWND();
		DLL_API HINSTANCE					GetInstance();
		DLL_API LPCTSTR						GetWindowTitle();
		DLL_API unsigned int				GetVertexProcessing();
		DLL_API const char*					GetVersion();
		DLL_API unsigned int				GetDesktopWidth();
		DLL_API unsigned int				GetDesktopHeight();
		DLL_API unsigned int				GetDesktopBPP();
		DLL_API unsigned int				GetAvailableTextureMemory();
		DLL_API float						GetFPS();
		DLL_API	unsigned int				GetWidth();
		DLL_API	unsigned int				GetHeight();
		DLL_API	float						GetAspectRatio();
		DLL_API	bool						GetWindowed();
		//Info e FPS
		DLL_API void						SetShowInfo(bool showInfo);
		DLL_API bool						GetShowInfo();
		DLL_API void						SetShowFPS(bool showFPS);
		DLL_API bool						GetShowFPS();
		//Screenshot
		DLL_API bool						SaveScreenshot(const char* lpszFileName);
		//Adapter
		DLL_API int							BuildAdapterList(bool supportFullscreen=true, bool supportWindowed=true, bool bBreakOnREF=false);
		DLL_API unsigned int				SelectAdapterID(bool startWindowed, unsigned int uiAdapterID);
};

#endif