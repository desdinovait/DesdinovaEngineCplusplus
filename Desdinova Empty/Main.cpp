#include "General.h"

bool Console_showFPS(bool value)
{
	Engine.SetShowFPS(value);
	return true;
}
bool Console_showInfo(bool value)
{
	Engine.SetShowInfo(value);
	return true;
}
bool Console_showInfoScene(bool value)
{
	Scene1_Debug.Debug_ShowInfo = value;
	Scene1.SetDebugProperties(Scene1_Debug);
	return true;
}	

bool Console_screenshot()
{
	return Engine.SaveScreenshot(NULL);
}
bool Console_closeConsole()
{
	Engine.GetConsole().SetEnable(false);
	return true;
}
bool Console_exitApp()
{
	PostQuitMessage(0);
	return true;
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Inizializza il motore grafico
	Engine.Init("Desdinova Empty.htm", "..:: Desdinova Engine::..", "Main Application LogFile");

	//Se esiste il file di config allora lo carica altrimenti setta dei parametri predefiniti
	if (Engine.ExistSettingsFile("Desdinova.cfg"))
	{
		//Setta il motore grafico tramite file
		Engine.SetSettingsFromFile("Desdinova.cfg");
	}
	else
	{
		//Setta il motore grafico tramite parametri forzati
		STRUCT_DE_DEVICESETTINGS	setDeviceSettings;
		setDeviceSettings.adapterID				= 0;
		setDeviceSettings.vertexProcessing		= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		setDeviceSettings.minVertexShaderMaj	= 2;
		setDeviceSettings.minVertexShaderMin	= 0;
		setDeviceSettings.minPixelShaderMaj		= 2;
		setDeviceSettings.minPixelShaderMin		= 0;
		#ifdef _DEBUG
			setDeviceSettings.presentParameters.Windowed					= true;
			setDeviceSettings.presentParameters.BackBufferWidth				= 800;
			setDeviceSettings.presentParameters.BackBufferHeight			= 600;
		#else
			setDeviceSettings.presentParameters.Windowed					= false;
			setDeviceSettings.presentParameters.BackBufferWidth				= 1024;
			setDeviceSettings.presentParameters.BackBufferHeight			= 768;
		#endif
		setDeviceSettings.presentParameters.AutoDepthStencilFormat		= D3DFMT_D16;
		setDeviceSettings.presentParameters.BackBufferCount				= 1;
		setDeviceSettings.presentParameters.BackBufferFormat			= D3DFMT_X8R8G8B8; //D3DFMT_UNKNOWN o D3DFMT_X8R8G8B8
		setDeviceSettings.presentParameters.EnableAutoDepthStencil		= true;
		setDeviceSettings.presentParameters.Flags						= 0;
		setDeviceSettings.presentParameters.hDeviceWindow				= NULL; //Verrà riempito quando si crea la finestra
		setDeviceSettings.presentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE;//D3DMULTISAMPLE_6_SAMPLES;
		setDeviceSettings.presentParameters.MultiSampleQuality			= 0;
		setDeviceSettings.presentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
		setDeviceSettings.presentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;
		setDeviceSettings.presentParameters.FullScreen_RefreshRateInHz	= 0;
		Engine.SetSettingsFromParams(setDeviceSettings);
	}
	
	//Crea la finestra base del motore grafico
	Engine.CreateMainWindow(hInstance, (DLGPROC)ClassProc, "DesdinovaClass", "DesdinovaEngine");

	//Inizializzazione Direct3D
	if (Engine.CreateDevice())
	{	
		//Mostra gli fps
		Engine.SetShowFPS(true);
		Engine.SetShowInfo(false);

		//Console base
		Engine.GetConsole().Add(Console_screenshot,			"cmd_screenshot");
		Engine.GetConsole().Add(Console_exitApp,			"cmd_exit");
		Engine.GetConsole().Add(Console_closeConsole,		"cmd_close");
		Engine.GetConsole().Add(Console_showInfo,			"cmd_info",				DE_CONSOLEPARAM_BOOL);
		Engine.GetConsole().Add(Console_showInfoScene,		"cmd_info_scene",		DE_CONSOLEPARAM_BOOL);
		Engine.GetConsole().Add(Console_showFPS,			"cmd_fps",				DE_CONSOLEPARAM_BOOL);

		//Crea le scene dell'applicazione	
		//***************
		//*** Scena 1 ***
		//***************
		//Nome
		Scene1_Properties.Scene_Name = "Scene1";
		//Funzioni utente
		Scene1_Properties.Scene_FuncPointer_Load = Scene1_Load;
		Scene1_Properties.Scene_FuncPointer_Input = Scene1_Input;
		Scene1_Properties.Scene_FuncPointer_Render3D = Scene1_Render3D;
		Scene1_Properties.Scene_FuncPointer_Render2D = Scene1_Render2D;
		Scene1_Properties.Scene_FuncPointer_Invalidate = Scene1_Invalidate;
		Scene1_Properties.Scene_FuncPointer_Restore = Scene1_Restore;
		Scene1_Properties.Scene_FuncPointer_Release = Scene1_Release;
		//Tipo scena fisica
		Scene1_Properties.Scene_Physics_Type = NX_SIMULATION_HW;
		//PostProcess
		Scene1_Properties.Scene_PostProcess_Filename = "Shaders\\PostProcessEffects.fx";
		//16/9
		Scene1_Properties.Scene_CineEffect_Filename = "Textures\\Panels\\Cine.bmp";
		//Puntatore
		Scene1_Properties.Scene_Cursor_Filename[0] = "Textures\\Panels\\Cursor1.dds";
		Scene1_Properties.Scene_Cursor_Filename[1] = "Textures\\Panels\\Cursor2.dds";
		//Crea la scene
		Scene1.Create(Scene1_Properties, "Scene1");
		//Clear
		Scene1_Clear.Clear_RectsCount = 0;
		Scene1_Clear.Clear_Rects = NULL;
		Scene1_Clear.Clear_ClearFlags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
		Scene1_Clear.Clear_EnvironmentColor = D3DCOLOR_XRGB(161,161,161);
		Scene1_Clear.Clear_Z = 1.0f;
		Scene1_Clear.Clear_Stencil = 0;
		//Debug
		#ifdef _DEBUG
			Scene1_Debug.Debug_ShowCenterAxes = true;
			Scene1_Debug.Debug_ShowInfo = true;
			Scene1_Debug.Debug_ShowGroundGrid= true;
			Scene1_Debug.Debug_ShowSpatialGrid = false;
			Scene1_Debug.Debug_ShowLights = true;
			Scene1_Debug.Debug_ShowSounds = true;
			Scene1_Debug.Debug_ShowTriggers = true;
			Scene1_Debug.Debug_FillMode = -1;				//-1=No overwrite, 0=Solid, 2=Wireframe, 1=Point 
			Scene1_Debug.Debug_UseLighting = -1;			//-1=No overwrite, 0=No lighting, 1=Force lighting
			Scene1_Debug.Debug_BoundingType = -1;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
			Scene1_Debug.Debug_ShowAxes = -1;				//-1=No overwrite, 0=No axes, 1=Show axes
			Scene1_Debug.Debug_UseFrustrumCulling = -1;		//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
			Scene1_Debug.Debug_UseLOD = -1;					//-1=No overwrite, 0=No lods, 1=Force lods
		#else
			Scene1_Debug.Debug_ShowCenterAxes = false;
			Scene1_Debug.Debug_ShowInfo = true;
			Scene1_Debug.Debug_ShowGroundGrid= false;
			Scene1_Debug.Debug_ShowSpatialGrid = false;
			Scene1_Debug.Debug_ShowLights = false;
			Scene1_Debug.Debug_ShowSounds = false;
			Scene1_Debug.Debug_ShowTriggers = false;
			Scene1_Debug.Debug_FillMode = 0;				//-1=No overwrite, 0=Solid, 2=Wireframe, 1=Point 
			Scene1_Debug.Debug_UseLighting = -1;			//-1=No overwrite, 0=No lighting, 1=Force lighting
			Scene1_Debug.Debug_BoundingType = 0;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
			Scene1_Debug.Debug_ShowAxes = 0;				//-1=No overwrite, 0=No axes, 1=Show axes
			Scene1_Debug.Debug_UseFrustrumCulling = -1;		//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
			Scene1_Debug.Debug_UseLOD = -1;					//-1=No overwrite, 0=No lods, 1=Force lods
		#endif
		//Prospettiva
		Scene1_Perspective.Perspective_Angle = D3DX_PI/4;
		Scene1_Perspective.Perspective_Near = 1.0f;
		Scene1_Perspective.Perspective_Far = 10000.0f;
		//General
		Scene1_General.General_AmbientColor = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
		Scene1_General.General_Wind = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//Fog
		Scene1_Fog.Fog_Color = D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f);
		Scene1_Fog.Fog_Start = 65.0f;
		Scene1_Fog.Fog_Range = 550.0f;
		Scene1_Fog.Fog_Enable = false;
		//Griglie
		Scene1_Grids.GridsProperties_SpatialGrid_DimensionCell = 100;
		Scene1_Grids.GridsProperties_SpatialGrid_PerSideCells = 10;
		Scene1_Grids.GridsProperties_GroundGrid_DimensionCell = 1;
		Scene1_Grids.GridsProperties_GroundGrid_PerSideCells = 50;
		//Proprietà fisiche
		Scene1_Physics.Scene_PhysicsProperties_Ground = true;
		Scene1_Physics.Scene_PhysicsProperties_GroundNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Scene1_Physics.Scene_PhysicsProperties_GroundY = 0.0f;
		Scene1_Physics.Scene_PhysicsProperties_StepSize = 1.0f/60.0f;
		Scene1_Physics.Scene_PhysicsProperties_NumIterations = 8;
		Scene1_Physics.Scene_PhysicsProperties_Gravity = D3DXVECTOR3(0.0f, -9.0f, 0.0f);
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_Restitution = 0.5f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFriction = 0.5f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFriction = 0.5f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_Flags = 0;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV = 0.8f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV = 1.0f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy = D3DXVECTOR3(0.0f,0.0f,1.0f);

		//Setta le proprietà
		Scene1.SetGeneralProperties(Scene1_General);
		Scene1.SetClearProperties(Scene1_Clear);
		Scene1.SetFogProperties(Scene1_Fog);
		Scene1.SetDebugProperties(Scene1_Debug);
		Scene1.SetPerspectiveProperties(Scene1_Perspective);
		Scene1.SetGridsProperties(Scene1_Grids);
		Scene1.SetPhysicsProperties(Scene1_Physics);
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene1);		


		//Setta la scena iniziale
		Engine.SetCurrentScene(0, false);


		//Loop generale dell'engine
		if (Engine.Loop()==false)
		{
			//Terminato il loop...rilasciare le ultime cose
			//...
		}
	}
	else
	{
		MessageBox(NULL, _T("Error during initialization. Refer to log file for more details."), _T("Desdinova Engine"), MB_OK);
	}

	return 0;
}


LRESULT WINAPI ClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {

		case WM_INITDIALOG:
		{
			return TRUE;
		}
		break;

		case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{
				Engine.ResizeDevice(LOWORD(lParam), HIWORD(lParam));
			}          
			return TRUE;
		}
		break;

		case WM_DESTROY:
		{
			Engine.ExitLoop();
			return TRUE;
		}
		break;

		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			return TRUE;
		}
		break;
	}

    return DefWindowProc(hWnd, msg, wParam, lParam);
}