#include "General.h"
#include "Main.h"

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
	Engine.engineConsole.SetTextColor(0xff000000 | value);
	return true;
}
bool Console_closeConsole()
{
	Engine.engineConsole.SetEnable(false);
	return true;
}


void CreateCommonControls(HWND windowHWND, HINSTANCE windowInstance)
{
	//**** TOOLBAR ****
    InitCommonControls();

	hWndToolbar = CreateToolbarEx(windowHWND, WS_CHILD | TBSTYLE_TOOLTIPS | CCS_ADJUSTABLE | WS_VISIBLE | TBSTYLE_FLAT , 1, 16, windowInstance, IDB_TOOLBAR2, 0,0, 25, 25, 25, 25, sizeof(TBBUTTON));

	const DWORD cButtons = 24;
    TBBUTTON Buttons[cButtons];

    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

	//File
    Buttons[0].iBitmap = 14; 
    Buttons[0].idCommand = MENU_FILE_NEW; 
    Buttons[0].fsState = TBSTATE_ENABLED; 
    Buttons[0].fsStyle = TBSTYLE_BUTTON; 
    Buttons[0].dwData = 0; 
    Buttons[0].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "New..."); 

    Buttons[1].iBitmap = 6; 
    Buttons[1].idCommand = MENU_FILE_OPEN; 
    Buttons[1].fsState = TBSTATE_ENABLED; 
    Buttons[1].fsStyle = TBSTYLE_BUTTON; 
    Buttons[1].dwData = 0; 
    Buttons[1].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Open..."); 

	Buttons[2].iBitmap = 12; 
    Buttons[2].idCommand = MENU_FILE_CLOSE; 
    Buttons[2].fsState = TBSTATE_ENABLED; 
    Buttons[2].fsStyle = TBSTYLE_BUTTON; 
    Buttons[2].dwData = 0; 
    Buttons[2].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Close..."); 

	Buttons[3].iBitmap = 7; 
    Buttons[3].idCommand = MENU_FILE_SAVE; 
    Buttons[3].fsState = TBSTATE_ENABLED; 
    Buttons[3].fsStyle = TBSTYLE_BUTTON; 
    Buttons[3].dwData = 0; 
    Buttons[3].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Save"); 
    
	Buttons[4].iBitmap = STD_HELP; 
    Buttons[4].idCommand = 0; 
    Buttons[4].fsState = TBSTATE_ENABLED; 
    Buttons[4].fsStyle = TBSTYLE_SEP; 
    Buttons[4].dwData = 0; 
    Buttons[4].iString = 0; 

	//Edit
    Buttons[5].iBitmap = 21; 
    Buttons[5].idCommand = MENU_EDIT_MODELSMANAGER; 
    Buttons[5].fsState = TBSTATE_ENABLED; 
    Buttons[5].fsStyle = TBSTYLE_BUTTON; 
    Buttons[5].dwData = 0; 
    Buttons[5].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Models Manager...");
    	
    Buttons[6].iBitmap = 22; 
    Buttons[6].idCommand = MENU_EDIT_ENTITIESMANAGER; 
    Buttons[6].fsState = TBSTATE_ENABLED; 
    Buttons[6].fsStyle = TBSTYLE_BUTTON; 
    Buttons[6].dwData = 0; 
    Buttons[6].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Entities Manager...");

	Buttons[7].iBitmap = 20; 
    Buttons[7].idCommand = MENU_EDIT_PARTICLESMANAGER; 
    Buttons[7].fsState = TBSTATE_ENABLED; 
    Buttons[7].fsStyle = TBSTYLE_BUTTON; 
    Buttons[7].dwData = 0; 
    Buttons[7].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Particles System Manager..."); 
	    
	Buttons[8].iBitmap = 2; 
    Buttons[8].idCommand = MENU_EDIT_CAMERACONTROL; 
    Buttons[8].fsState = TBSTATE_ENABLED; 
    Buttons[8].fsStyle = TBSTYLE_BUTTON; 
    Buttons[8].dwData = 0; 
    Buttons[8].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Camera Control...");

	Buttons[9].iBitmap = STD_HELP; 
    Buttons[9].idCommand = 0; 
    Buttons[9].fsState = TBSTATE_ENABLED; 
    Buttons[9].fsStyle = TBSTYLE_SEP; 
    Buttons[9].dwData = 0; 
    Buttons[9].iString = 0; 

	//View
    Buttons[10].iBitmap = 8; 
    Buttons[10].idCommand = MENU_VIEW_FRUSTRUMCULLING; 
    Buttons[10].fsState = TBSTATE_ENABLED; 
    Buttons[10].fsStyle = TBSTYLE_BUTTON; 
    Buttons[10].dwData = 0; 
    Buttons[10].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Frustrum culling"); 

    Buttons[11].iBitmap = 16; 
    Buttons[11].idCommand = MENU_VIEW_LOD; 
    Buttons[11].fsState = TBSTATE_ENABLED; 
    Buttons[11].fsStyle = TBSTYLE_BUTTON; 
    Buttons[11].dwData = 0; 
    Buttons[11].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Levels of Detail"); 

	Buttons[12].iBitmap = 5; 
    Buttons[12].idCommand = MENU_VIEW_WIREFRAME; 
    Buttons[12].fsState = TBSTATE_ENABLED; 
    Buttons[12].fsStyle = TBSTYLE_BUTTON; 
    Buttons[12].dwData = 0; 
    Buttons[12].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Wireframe"); 

    Buttons[13].iBitmap = 3; 
    Buttons[13].idCommand = MENU_VIEW_LIGHTING; 
    Buttons[13].fsState = TBSTATE_ENABLED; 
    Buttons[13].fsStyle = TBSTYLE_BUTTON; 
    Buttons[13].dwData = 0; 
    Buttons[13].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Lighting");

	Buttons[14].iBitmap = STD_HELP; 
    Buttons[14].idCommand = 0; 
    Buttons[14].fsState = TBSTATE_ENABLED; 
    Buttons[14].fsStyle = TBSTYLE_SEP; 
    Buttons[14].dwData = 0; 
    Buttons[14].iString = 0; 

	Buttons[15].iBitmap = 17; 
    Buttons[15].idCommand = MENU_BOUNDING_NONE; 
    Buttons[15].fsState = TBSTATE_ENABLED; 
    Buttons[15].fsStyle = TBSTYLE_BUTTON; 
    Buttons[15].dwData = 0; 
    Buttons[15].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Bounding None");

	Buttons[16].iBitmap = 18; 
    Buttons[16].idCommand = MENU_BOUNDING_BOX; 
    Buttons[16].fsState = TBSTATE_ENABLED; 
    Buttons[16].fsStyle = TBSTYLE_BUTTON; 
    Buttons[16].dwData = 0; 
    Buttons[16].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Bounding Box");

	Buttons[17].iBitmap = 19; 
    Buttons[17].idCommand = MENU_BOUNDING_SPHERE; 
    Buttons[17].fsState = TBSTATE_ENABLED; 
    Buttons[17].fsStyle = TBSTYLE_BUTTON; 
    Buttons[17].dwData = 0; 
    Buttons[17].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Bounding Sphere");

	Buttons[18].iBitmap = STD_HELP; 
    Buttons[18].idCommand = 0; 
    Buttons[18].fsState = TBSTATE_ENABLED; 
    Buttons[18].fsStyle = TBSTYLE_SEP; 
    Buttons[18].dwData = 0; 
    Buttons[18].iString = 0; 

	Buttons[19].iBitmap = 10; 
    Buttons[19].idCommand = MENU_VIEW_AXES; 
    Buttons[19].fsState = TBSTATE_ENABLED; 
    Buttons[19].fsStyle = TBSTYLE_BUTTON; 
    Buttons[19].dwData = 0; 
    Buttons[19].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Axes");

	Buttons[20].iBitmap = 15; 
    Buttons[20].idCommand = MENU_VIEW_GRID; 
    Buttons[20].fsState = TBSTATE_ENABLED; 
    Buttons[20].fsStyle = TBSTYLE_BUTTON; 
    Buttons[20].dwData = 0; 
    Buttons[20].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Grid");

	Buttons[21].iBitmap = 9; 
    Buttons[21].idCommand = MENU_VIEW_INFO; 
    Buttons[21].fsState = TBSTATE_ENABLED; 
    Buttons[21].fsStyle = TBSTYLE_BUTTON; 
    Buttons[21].dwData = 0; 
    Buttons[21].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Informations");

	Buttons[22].iBitmap = STD_HELP; 
    Buttons[22].idCommand = 0; 
    Buttons[22].fsState = TBSTATE_ENABLED; 
    Buttons[22].fsStyle = TBSTYLE_SEP; 
    Buttons[22].dwData = 0; 
    Buttons[22].iString = 0; 

	//Utilities
    Buttons[23].iBitmap = 11; 
    Buttons[23].idCommand = MENU_UTILITIES_CAPTURE; 
    Buttons[23].fsState = TBSTATE_ENABLED; 
    Buttons[23].fsStyle = TBSTYLE_BUTTON; 
    Buttons[23].dwData = 0;
    Buttons[23].iString = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) "Capture screenshot");
	
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)cButtons, (LPARAM) (LPTBBUTTON) &Buttons);
	SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);				//Mettere a 0 per vedere i tooltip dei pulsanti
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);					//Permette il ridimensionamento dei pulsanti(vedere successivo...)
	SendMessage(hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(25,25));	//Setta la dimensione dei pulsanti per diminuire la loro dimensione modificata dalle scritte(provare a togliere x vedere)

	//Check toolbar items
	if ( Scene0.renderProperties.useBounding == 0)		SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_NONE, MAKELONG(true, 0));
	if ( Scene0.renderProperties.useBounding == 1)		SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_BOX, MAKELONG(true, 0));
	if ( Scene0.renderProperties.useBounding == 2)		SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_SPHERE, MAKELONG(true, 0));
	if ( Scene0.renderProperties.useFrustrumCulling )	SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_FRUSTRUMCULLING, MAKELONG(true, 0));
	else												SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_FRUSTRUMCULLING, MAKELONG(false, 0));
	if ( Scene0.renderProperties.useLOD )				SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LOD, MAKELONG(true, 0));
	else												SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LOD, MAKELONG(false, 0));
	if ( Scene0.renderProperties.useWireframe )			SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_WIREFRAME, MAKELONG(true, 0));
	else												SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_WIREFRAME, MAKELONG(false, 0));
	if ( Scene0.renderProperties.useLighting )			SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LIGHTING, MAKELONG(true, 0));
	else												SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LIGHTING, MAKELONG(false, 0));
	if ( Scene0.showAxes )				SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_AXES, MAKELONG(true, 0));
	else								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_AXES, MAKELONG(false, 0));
	if ( Scene0.showGrid )				SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_GRID, MAKELONG(true, 0));
	else								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_GRID, MAKELONG(false, 0));
	if ( Scene0.showInfo )				SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_INFO, MAKELONG(true, 0));
	else								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_INFO, MAKELONG(false, 0));




	//**** STATUSBAR ****
    RECT rcClient;
    HLOCAL hloc;
    LPINT lpParts;
    int i, nWidth;
	int nParts = 2;
	hWndStatusbar = CreateWindowEx( 0, STATUSCLASSNAME, (LPCTSTR) NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,	windowHWND, (HMENU) 0, windowInstance, NULL);
    GetClientRect(windowHWND, &rcClient);
	//Allocca memoria in base alle parti da creare
	hloc = LocalAlloc(LHND, sizeof(int) * nParts);
    lpParts = (LPINT)LocalLock(hloc);
	//calcola la lunghezza delle singole parti in modo proporzionale 
	nWidth = rcClient.right / nParts;
    for (i = 0; i < nParts; i++) 
	{ 
       lpParts[i] = nWidth * (i+1);
    }
    SendMessage(hWndStatusbar, SB_SETPARTS, (WPARAM) nParts, (LPARAM)lpParts);
    //Libera la memoria
    LocalUnlock(hloc);
    LocalFree(hloc);//*/

}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR     lpCmdLine,int       nCmdShow)
{
	//Splash screen
	//DialogBox(hInstance, (LPCTSTR)DIALOG_SPLASH, NULL, (DLGPROC)SplashProc);

	//Inizializza il motore grafico
	Engine.Init("DesdinovaModeler.htm");
	
	//Setta il device
	STRUCT_DE_DEVICESETTINGS	setDeviceSettings;
	setDeviceSettings.adapterID = 0;
	setDeviceSettings.vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	setDeviceSettings.presentParameters.Windowed					= true;
	setDeviceSettings.presentParameters.AutoDepthStencilFormat		= D3DFMT_D16;
	setDeviceSettings.presentParameters.BackBufferCount				= 1;
	setDeviceSettings.presentParameters.BackBufferFormat			= D3DFMT_UNKNOWN; //D3DFMT_UNKNOWN o D3DFMT_X8R8G8B8
	setDeviceSettings.presentParameters.BackBufferWidth				= 0;	//Prende quella dell'HWND
	setDeviceSettings.presentParameters.BackBufferHeight			= 0;	//Prende quella dell'HWND
	setDeviceSettings.presentParameters.EnableAutoDepthStencil		= true;
	setDeviceSettings.presentParameters.Flags						= 0;
	setDeviceSettings.presentParameters.hDeviceWindow				= NULL; //Verrà riempito quando si crea la finestra
	setDeviceSettings.presentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE;
	setDeviceSettings.presentParameters.MultiSampleQuality			= 0;
	setDeviceSettings.presentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	setDeviceSettings.presentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	setDeviceSettings.presentParameters.FullScreen_RefreshRateInHz	= 0;
	Engine.SetSettingsFromParams(setDeviceSettings);	

	//Crea la finestra tramite resource
	Engine.SetHWND(Engine.CreateMainWindow(hInstance, (LPCTSTR)DIALOG_MAIN, (DLGPROC)ResourceProc, "Desdinova Modeler v1.03"));
	Engine.SetRenderHWND(GetDlgItem(Engine.GetHWND(), RENDER_ZONE));
	
	//Wndows Common Controls
	CreateCommonControls(Engine.GetHWND(), Engine.GetInstance());
	
	//Inizializzazione Direct3D
	if (Engine.Create())
	{
		//Mostra gli fps
		Engine.SetShowFPS(true);
		Engine.SetShowInfo(false);

		//Console base
		Engine.engineConsole.Add(Console_screenshot,		"cmd_screenshot");
		Engine.engineConsole.Add(Console_exitApp,			"cmd_exit");
		Engine.engineConsole.Add(Console_closeConsole,		"cmd_close");
		Engine.engineConsole.Add(Console_setGamma,			"cmd_gamma",			DE_CONSOLEPARAM_FLOAT);
		Engine.engineConsole.Add(Console_setTextColor,		"cmd_consolecolor",		DE_CONSOLEPARAM_DWORD);
		Engine.engineConsole.Add(Console_showInfo,			"cmd_info",				DE_CONSOLEPARAM_BOOL);
		Engine.engineConsole.Add(Console_showFPS,			"cmd_fps",				DE_CONSOLEPARAM_BOOL);

		/**** Crea le zone di rendering aggiuntive ****
		hWndRenderZone1=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_RENDERZONE1, NULL, (DLGPROC)RenderZone1Proc);
		hWndRenderZone2=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_RENDERZONE2, NULL, (DLGPROC)RenderZone2Proc);
		Engine.AddSwapChain(hWndRenderZone1,0,0);
		Engine.AddSwapChain(hWndRenderZone2,0,0);
		//**********************************************/

		//Crea le scene dell'applicazione	
		//*** Scena 1 ***
		STRUCT_DE_SCENEPROPERTIES Scene1_Properties;
		Scene1_Properties.Scene_SwapChainID = 1;
		Scene1_Properties.Scene_Angle = D3DX_PI/4;
		Scene1_Properties.Scene_Name = "Main Scene"
		Scene1_Properties.Scene_Near = 1.0f;
		Scene1_Properties.Scene_Far = 3000.0f;
		Scene1_Properties.Scene_AspectRatio = Engine.GetSwapChainAspectRatio(0);
		Scene1_Properties.Scene_PhysicsDescription.gravity = NxVec3(0.0f, -9.8f, 0.0f);
		Scene1_Properties.Scene_PhysicsDescription.groundPlane = false;
		Scene1_Properties.Scene_PhysicsDescription.collisionDetection = true;
		Scene1_Properties.Scene_PhysicsDescription.broadPhase = NX_BROADPHASE_COHERENT;
		Scene1_Properties.Scene_PhysicsDescription.groundPlane = true;
		//Crea la scene
		Scene1.Create(Scene1_Properties, "Scene1");
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene1);
		//Crea le camere
		Scene1_Camera1.Create(D3DXVECTOR3(-25,25,-50), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 5.0f, "Scene1_Camera1");
		Scene1_Camera2.Create(D3DXVECTOR3(0,0,-100), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 1.0f, "Scene1_Camera2");
		//Aggiunge le camere alla scena
		Scene1.AddCamera(&Scene1_Camera);
		Scene1.AddCamera(&Scene2_Camera);
		//Imposta la camera corrente
		Scene1.SetCurrentCamera(1);

		/*** Scena 2 ***
		STRUCT_DE_SCENEPROPERTIES Scene2_Properties;
		Scene2_Properties.Scene_SwapChainID = 1;
		Scene2_Properties.Scene_Angle = D3DX_PI/4;
		Scene2_Properties.Scene_Name = "Secondary Scene"
		Scene2_Properties.Scene_Near = 1.0f;
		Scene2_Properties.Scene_Far = 3000.0f;
		Scene2_Properties.Scene_AspectRatio = Engine.GetSwapChainAspectRatio(1);
		Scene2_Properties.Scene_PhysicsDescription.gravity = NxVec3(0.0f, 0.0f, 0.0f);
		Scene2_Properties.Scene_PhysicsDescription.groundPlane = false;
		Scene2_Properties.Scene_PhysicsDescription.collisionDetection = true;
		Scene2_Properties.Scene_PhysicsDescription.broadPhase = NX_BROADPHASE_COHERENT;
		Scene2_Properties.Scene_PhysicsDescription.groundPlane = false;
		//Crea la scene
		Scene2.Create(Scene2_Properties, "Scene2");	
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene2);
		//Crea la camera
		Scene2_Camera.Create(D3DXVECTOR3(0,0,250), D3DXVECTOR3(0,-70,0), D3DXVECTOR3(0,1,0), 0, 1.0f, "Scene2_Camera");
		//Aggiunge la camera alla scena
		Scene2.AddCamera(&Scene2_Camera);
		//Imposta la camera corrente
		Scene2.SetCurrentCamera(1);
		
		//*** Scena 3 ***
		STRUCT_DE_SCENEPROPERTIES Scene3_Properties;
		Scene3_Properties.Scene_SwapChainID = 1;
		Scene3_Properties.Scene_Angle = D3DX_PI/4;
		Scene3_Properties.Scene_Name = "Last Scene"
		Scene3_Properties.Scene_Near = 1.0f;
		Scene3_Properties.Scene_Far = 3000.0f;
		Scene3_Properties.Scene_AspectRatio = Engine.GetSwapChainAspectRatio(2);
		Scene3_Properties.Scene_PhysicsDescription.gravity = NxVec3(0.0f, 0.0f, 0.0f);
		Scene3_Properties.Scene_PhysicsDescription.groundPlane = false;
		Scene3_Properties.Scene_PhysicsDescription.collisionDetection = true;
		Scene3_Properties.Scene_PhysicsDescription.broadPhase = NX_BROADPHASE_COHERENT;
		Scene3_Properties.Scene_PhysicsDescription.groundPlane = false;
		//Crea la scene
		Scene3.Create(Scene3_Properties, "Scene3");
		//Aggiunge la scena al motore
		Engine.AddScene(&Scene3);
		//Crea la camera
		Scene3_Camera.Create(D3DXVECTOR3(0,0,250), D3DXVECTOR3(0,-70,0), D3DXVECTOR3(0,1,0), 0, 1.0f, "Scene3_Camera");
		//Aggiunge la camera alla scena
		Scene3.AddCamera(Scene3_Camera);
		//Imposta la camera corrente
		Scene3.SetCurrentCamera(1);//*/



		//Loop generale dell'engine
		if (Engine.Loop()==false)
		{
			//Terminato il loop rilascia gli oggetti principali
			Engine.Release();
		}		
	}
	else
	{
		MessageBox(NULL,"Error during initialization. Refer to log file for more details.","Desdinova Engine",MB_OK);
	}

	return 0;
}