#include "General.h"
#include "Main.h"
#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"

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
bool Console_screenshot()
{
	return Engine.SaveScreenshot(NULL);
}
bool Console_setGamma(float value)
{
	return Engine.SetGammaValue(value, false);
}
bool Console_exitApp()
{
	PostQuitMessage(0);
	return true;
}
bool Console_setTextColor(DWORD value)
{
	Engine.GetConsole().SetTextColor(0xff000000 | value);
	return true;
}
bool Console_closeConsole()
{
	Engine.GetConsole().SetEnable(false);
	return true;
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Inizializza il motore grafico
	Engine.Init("Desdinova.htm", "..:: Desdinova Engine::..", "Main Application LogFile");

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
		setDeviceSettings.adapterID = 0;
		setDeviceSettings.vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		setDeviceSettings.presentParameters.Windowed					= true;
		setDeviceSettings.presentParameters.AutoDepthStencilFormat		= D3DFMT_D16;
		setDeviceSettings.presentParameters.BackBufferCount				= 1;
		setDeviceSettings.presentParameters.BackBufferFormat			= D3DFMT_X8R8G8B8; //D3DFMT_UNKNOWN o D3DFMT_X8R8G8B8
		setDeviceSettings.presentParameters.BackBufferWidth				= 800;
		setDeviceSettings.presentParameters.BackBufferHeight			= 600;
		setDeviceSettings.presentParameters.EnableAutoDepthStencil		= true;
		setDeviceSettings.presentParameters.Flags						= 0;
		setDeviceSettings.presentParameters.hDeviceWindow				= NULL; //Verrà riempito quando si crea la finestra
		setDeviceSettings.presentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE;//D3DMULTISAMPLE_6_SAMPLES;
		setDeviceSettings.presentParameters.MultiSampleQuality			= 0;
		setDeviceSettings.presentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
		setDeviceSettings.presentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;
		setDeviceSettings.presentParameters.FullScreen_RefreshRateInHz	= 0;
		setDeviceSettings.minVertexShaderMaj = 2;
		setDeviceSettings.minVertexShaderMin = 0;
		setDeviceSettings.minPixelShaderMaj = 2;
		setDeviceSettings.minPixelShaderMin = 0;
		Engine.SetSettingsFromParams(setDeviceSettings);
	}
	
	//Crea la finestra base del motore grafico
	Engine.CreateMainWindow(hInstance, (DLGPROC)ClassProc, "DesdinovaClass", "Desdinova Engine");

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
		Engine.GetConsole().Add(Console_setGamma,			"cmd_gamma",			DE_CONSOLEPARAM_FLOAT);
		Engine.GetConsole().Add(Console_setTextColor,		"cmd_consolecolor",		DE_CONSOLEPARAM_DWORD);
		Engine.GetConsole().Add(Console_showInfo,			"cmd_info",				DE_CONSOLEPARAM_BOOL);
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
		Scene1_Properties.Scene_PostProcess_Filename = "PostProcessEffects.fx";
		//16/9
		Scene1_Properties.Scene_CineEffect_Filename = "Textures\\Panels\\Cine.bmp";
		//Puntatore
		Scene1_Properties.Scene_Cursor_Filename[0] = "Textures\\Panels\\Cursor1.bmp";
		Scene1_Properties.Scene_Cursor_Filename[1] = "Textures\\Panels\\Cursor2.bmp";
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
		Scene1_Debug.Debug_ShowCenterAxes = true;
		Scene1_Debug.Debug_ShowInfo = true;
		Scene1_Debug.Debug_ShowGroundGrid= true;
		Scene1_Debug.Debug_ShowSpatialGrid = false;
		Scene1_Debug.Debug_ShowLights = false;
		Scene1_Debug.Debug_ShowTriggers = true;
		Scene1_Debug.Debug_FillMode = -1;				//-1=No overwrite, 0=Solid, 2=Wireframe, 1=Point 
		Scene1_Debug.Debug_UseLighting = -1;			//-1=No overwrite, 0=No lighting, 1=Force lighting
		Scene1_Debug.Debug_BoundingType = -1;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
		Scene1_Debug.Debug_ShowAxes = -1;				//-1=No overwrite, 0=No axes, 1=Show axes
		Scene1_Debug.Debug_UseFrustrumCulling = -1;		//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
		Scene1_Debug.Debug_UseLOD = -1;					//-1=No overwrite, 0=No lods, 1=Force lods
		//Prospettiva
		Scene1_Perspective.Perspective_Angle = D3DX_PI/4;
		Scene1_Perspective.Perspective_Near = 1.0f;
		Scene1_Perspective.Perspective_Far = 1000.0f;
		//Fog
		Scene1_Fog.Fog_Enable = false;
		Scene1_Fog.Fog_Type = D3DRS_FOGVERTEXMODE;
		Scene1_Fog.Fog_Mode = D3DFOG_LINEAR;
		Scene1_Fog.Fog_Start = 1.0f;
		Scene1_Fog.Fog_End = 100.0f;
		Scene1_Fog.Fog_Color = D3DCOLOR_XRGB(150,55,55);
		Scene1_Fog.Fog_UseRange = false;
		//Griglie
		Scene1_Grids.GridsProperties_SpatialGrid_DimensionCell = 100;
		Scene1_Grids.GridsProperties_SpatialGrid_PerSideCells = 10;
		Scene1_Grids.GridsProperties_GroundGrid_DimensionCell = 1;
		Scene1_Grids.GridsProperties_GroundGrid_PerSideCells = 10;
		//Proprietà fisiche
		Scene1_Physics.Scene_PhysicsProperties_Ground = false;
		Scene1_Physics.Scene_PhysicsProperties_GroundNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Scene1_Physics.Scene_PhysicsProperties_GroundY = 0.0f;
		Scene1_Physics.Scene_PhysicsProperties_StepSize = 1.0f/60.0f;
		Scene1_Physics.Scene_PhysicsProperties_NumIterations = 8;
		Scene1_Physics.Scene_PhysicsProperties_Gravity = D3DXVECTOR3(0.0f, -9.8f, 0.0f);
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_Restitution = 0.5f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFriction = 0.5f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFriction = 0.5f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV = 0.8f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV = 1.0f;
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy = D3DXVECTOR3(0.0f,0.0f,1.0f);
		Scene1_Physics.Scene_PhysicsProperties_DefaultMaterial_Flags = NX_MF_ANISOTROPIC;

		//Setta le proprietà
		Scene1.SetClearProperties(Scene1_Clear);
		Scene1.SetFogProperties(Scene1_Fog);
		Scene1.SetDebugProperties(Scene1_Debug);
		Scene1.SetPerspectiveProperties(Scene1_Perspective);
		Scene1.SetGridsProperties(Scene1_Grids);
		Scene1.SetPhysicsProperties(Scene1_Physics);
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene1);		



		//***************
		//*** Scena 2 ***
		//***************
		//Nome
		Scene2_Properties.Scene_Name = "Scene2";
		//Funzioni utente
		Scene2_Properties.Scene_FuncPointer_Load = Scene2_Load;
		Scene2_Properties.Scene_FuncPointer_Input = Scene2_Input;
		Scene2_Properties.Scene_FuncPointer_Render3D = Scene2_Render3D;
		Scene2_Properties.Scene_FuncPointer_Render2D = Scene2_Render2D;
		Scene2_Properties.Scene_FuncPointer_Invalidate = Scene2_Invalidate;
		Scene2_Properties.Scene_FuncPointer_Restore = Scene2_Restore;
		Scene2_Properties.Scene_FuncPointer_Release = Scene2_Release;
		//Tipo scena fisica
		Scene2_Properties.Scene_Physics_Type = NX_SIMULATION_HW;
		//PostProcess
		Scene2_Properties.Scene_PostProcess_Filename = "PostProcessEffects.fx";
		//16/9
		Scene2_Properties.Scene_CineEffect_Filename = "Textures\\Panels\\Cine.bmp";
		//Puntatore
		Scene2_Properties.Scene_Cursor_Filename[0] = "Textures\\Panels\\Cursor1.bmp";
		Scene2_Properties.Scene_Cursor_Filename[1] = "Textures\\Panels\\Cursor2.bmp";
		//Crea la scene
		Scene2.Create(Scene2_Properties, "Scene2");
		//Clear
		Scene2_Clear.Clear_RectsCount = 0;
		Scene2_Clear.Clear_Rects = NULL;
		Scene2_Clear.Clear_ClearFlags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
		Scene2_Clear.Clear_EnvironmentColor = D3DCOLOR_XRGB(0,0,0);
		Scene2_Clear.Clear_Z = 1.0f;
		Scene2_Clear.Clear_Stencil = 0;
		//Debug
		Scene2_Debug.Debug_ShowCenterAxes = false;
		Scene2_Debug.Debug_ShowInfo = false;
		Scene2_Debug.Debug_ShowGroundGrid= false;
		Scene2_Debug.Debug_ShowSpatialGrid = false;
		Scene2_Debug.Debug_ShowLights = false;
		Scene2_Debug.Debug_ShowTriggers = false;
		Scene2_Debug.Debug_FillMode = -1;				//-1=No overwrite, 0=Solid, 2=Wireframe, 1=Point 
		Scene2_Debug.Debug_UseLighting = -1;			//-1=No overwrite, 0=No lighting, 1=Force lighting
		Scene2_Debug.Debug_BoundingType = -1;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
		Scene2_Debug.Debug_ShowAxes = -1;				//-1=No overwrite, 0=No axes, 1=Show axes
		Scene2_Debug.Debug_UseFrustrumCulling = -1;		//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
		Scene2_Debug.Debug_UseLOD = -1;					//-1=No overwrite, 0=No lods, 1=Force lods
		//Prospettiva
		Scene2_Perspective.Perspective_Angle = D3DX_PI/4;
		Scene2_Perspective.Perspective_Near = 1.0f;
		Scene2_Perspective.Perspective_Far = 3000.0f;
		//Fog
		Scene2_Fog.Fog_Enable = false;
		Scene2_Fog.Fog_Type = D3DRS_FOGVERTEXMODE;
		Scene2_Fog.Fog_Mode = D3DFOG_LINEAR;
		Scene2_Fog.Fog_Start = 100.0f;
		Scene2_Fog.Fog_End = Scene2_Perspective.Perspective_Far * 3;
		Scene2_Fog.Fog_Color = D3DCOLOR_XRGB(150,55,55);
		Scene2_Fog.Fog_UseRange = false;
		//Griglie
		Scene2_Grids.GridsProperties_SpatialGrid_DimensionCell = 100;
		Scene2_Grids.GridsProperties_SpatialGrid_PerSideCells = 10;
		Scene2_Grids.GridsProperties_GroundGrid_DimensionCell = 1;
		Scene2_Grids.GridsProperties_GroundGrid_PerSideCells = 100;
		//Proprietà fisiche
		Scene2_Physics.Scene_PhysicsProperties_Ground = true;
		Scene2_Physics.Scene_PhysicsProperties_GroundNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Scene2_Physics.Scene_PhysicsProperties_GroundY = 0.0f;
		Scene2_Physics.Scene_PhysicsProperties_StepSize = 1.0f/60.0f;
		Scene2_Physics.Scene_PhysicsProperties_NumIterations = 8;
		Scene2_Physics.Scene_PhysicsProperties_Gravity = D3DXVECTOR3(0.0f, -9.8f, 0.0f);
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_Restitution = 0.5f;
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFriction = 0.5f;
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFriction = 0.5f;
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV = 0.8f;
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV = 1.0f;
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy = D3DXVECTOR3(0.0f,0.0f,1.0f);
		Scene2_Physics.Scene_PhysicsProperties_DefaultMaterial_Flags = NX_MF_ANISOTROPIC;
		//Setta le proprietà
		Scene2.SetClearProperties(Scene2_Clear);
		Scene2.SetFogProperties(Scene2_Fog);
		Scene2.SetDebugProperties(Scene2_Debug);
		Scene2.SetPerspectiveProperties(Scene2_Perspective);
		Scene2.SetGridsProperties(Scene2_Grids);
		Scene2.SetPhysicsProperties(Scene2_Physics);
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene2);		


		//***************
		//*** Scena 3 ***
		//***************
		//Nome
		Scene3_Properties.Scene_Name = "Scene3";
		//Funzioni utente
		Scene3_Properties.Scene_FuncPointer_Load = Scene3_Load;
		Scene3_Properties.Scene_FuncPointer_Input = Scene3_Input;
		Scene3_Properties.Scene_FuncPointer_Render3D = Scene3_Render3D;
		Scene3_Properties.Scene_FuncPointer_Render2D = Scene3_Render2D;
		Scene3_Properties.Scene_FuncPointer_Invalidate = Scene3_Invalidate;
		Scene3_Properties.Scene_FuncPointer_Restore = Scene3_Restore;
		Scene3_Properties.Scene_FuncPointer_Release = Scene3_Release;
		//Tipo scena fisica
		Scene3_Properties.Scene_Physics_Type = NX_SIMULATION_HW;
		//PostProcess
		Scene3_Properties.Scene_PostProcess_Filename = "PostProcessEffects.fx";
		//16/9
		Scene3_Properties.Scene_CineEffect_Filename = "Textures\\Panels\\Cine.bmp";
		//Puntatore
		Scene3_Properties.Scene_Cursor_Filename[0] = "Textures\\Panels\\Cursor1.bmp";
		Scene3_Properties.Scene_Cursor_Filename[1] = "Textures\\Panels\\Cursor2.bmp";
		//Crea la scene
		Scene3.Create(Scene3_Properties, "Scene3");
		//Clear
		Scene3_Clear.Clear_RectsCount = 0;
		Scene3_Clear.Clear_Rects = NULL;
		Scene3_Clear.Clear_ClearFlags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
		Scene3_Clear.Clear_EnvironmentColor = D3DCOLOR_XRGB(161,161,161);
		Scene3_Clear.Clear_Z = 1.0f;
		Scene3_Clear.Clear_Stencil = 0;
		//Debug
		Scene3_Debug.Debug_ShowCenterAxes = true;
		Scene3_Debug.Debug_ShowInfo = false;
		Scene3_Debug.Debug_ShowGroundGrid = true;
		Scene3_Debug.Debug_ShowSpatialGrid = false;
		Scene3_Debug.Debug_ShowLights = false;
		Scene3_Debug.Debug_ShowTriggers = false;
		Scene3_Debug.Debug_FillMode = -1;				//-1=No overwrite, 0=Solid, 1=Wireframe, 2=Point 
		Scene3_Debug.Debug_UseLighting = -1;			//-1=No overwrite, 0=No lighting, 1=Force lighting
		Scene3_Debug.Debug_BoundingType = -1;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
		Scene3_Debug.Debug_ShowAxes = -1;				//-1=No overwrite, 0=No axes, 1=Show axes
		Scene3_Debug.Debug_UseFrustrumCulling = -1;		//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
		Scene3_Debug.Debug_UseLOD = -1;					//-1=No overwrite, 0=No lods, 1=Force lods
		//Prospettiva
		Scene3_Perspective.Perspective_Angle = D3DX_PI/4;
		Scene3_Perspective.Perspective_Near = 1.0f;
		Scene3_Perspective.Perspective_Far = 3000.0f;
		//Fog
		Scene3_Fog.Fog_Enable = false;
		Scene3_Fog.Fog_Type = D3DRS_FOGVERTEXMODE;
		Scene3_Fog.Fog_Mode = D3DFOG_LINEAR;
		Scene3_Fog.Fog_Start = 1.0f;
		Scene3_Fog.Fog_End = Scene3_Perspective.Perspective_Far * 3;
		Scene3_Fog.Fog_Color = D3DCOLOR_XRGB(0,255,0);
		Scene3_Fog.Fog_UseRange = false;
		//Griglie
		Scene3_Grids.GridsProperties_SpatialGrid_DimensionCell = 1000;
		Scene3_Grids.GridsProperties_SpatialGrid_PerSideCells = 10;
		Scene3_Grids.GridsProperties_GroundGrid_DimensionCell = 1;
		Scene3_Grids.GridsProperties_GroundGrid_PerSideCells = 40;
		//Proprietà fisiche
		Scene3_Physics.Scene_PhysicsProperties_Ground = false;
		Scene3_Physics.Scene_PhysicsProperties_GroundNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Scene3_Physics.Scene_PhysicsProperties_GroundY = 0.0f;
		Scene3_Physics.Scene_PhysicsProperties_StepSize = 1.0f/60.0f;
		Scene3_Physics.Scene_PhysicsProperties_NumIterations = 8;
		Scene3_Physics.Scene_PhysicsProperties_Gravity = D3DXVECTOR3(0.0f, -9.8f, 0.0f);
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_Restitution = 0.25f;
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFriction = 1.0f;
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFriction = 1.0f;
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV = 0.8f;
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV = 1.0f;
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy = D3DXVECTOR3(0.0f,0.0f,1.0f);
		Scene3_Physics.Scene_PhysicsProperties_DefaultMaterial_Flags = NX_MF_ANISOTROPIC;
		//Setta le proprietà
		Scene3.SetClearProperties(Scene3_Clear);
		Scene3.SetFogProperties(Scene3_Fog);
		Scene3.SetDebugProperties(Scene3_Debug);
		Scene3.SetPerspectiveProperties(Scene3_Perspective);
		Scene3.SetGridsProperties(Scene3_Grids);
		Scene3.SetPhysicsProperties(Scene3_Physics);
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene3);



		//Setta la scena iniziale
		Engine.SetCurrentScene(2, false);



		//Loop generale dell'engine
		if (Engine.Loop()==false)
		{
			//Terminato il loop rilascia il motore
			Engine.Release();
		}
	}
	else
	{
		MessageBox(NULL,"Error during initialization. Refer to log file for more details.","Desdinova Engine",MB_OK);
	}

	return 0;
}


LRESULT WINAPI ClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {

		case WM_INITDIALOG:
		{
			ShowWindow(hWnd,SW_SHOW);
		}
		break;

        case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{
				Engine.ResizeDevice(LOWORD(lParam), HIWORD(lParam));
			}          
		}
		break;

        case WM_DESTROY:
		{
           // return TRUE;
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


LRESULT CALLBACK RenderZone1Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	int wmEvent;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			ShowWindow(hWnd,SW_SHOW);
		}
		break;

        case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{				
				Engine.ResizeDevice(LOWORD(lParam), HIWORD(lParam));
			}
		}
		break;

        case WM_DESTROY:
		{
            return 0;
		}
        break;

		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			return TRUE;
		}
		break;

        case WM_SETFOCUS:
		{
			//Engine.SetActiveSwapChain(1);
		}
        break;
	
		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
					break;
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					break;
				return TRUE;						// Prevent From Happening

				default:
				   return DefWindowProc(hWnd,uMsg,wParam,lParam);

			}
		}
		break;
 
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
		}
		break;

	}
	return FALSE;
}
