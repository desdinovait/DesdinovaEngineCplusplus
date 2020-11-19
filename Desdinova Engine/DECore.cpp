#include "DEGeneralIncludes.h"
#include "DECore.h"

IDirect3DDevice9				*DECore::D3DDevice=NULL;
NxPhysicsSDK					*DECore::enginePhysX=NULL;
NxCookingInterface 				*DECore::enginePhysXCooking=NULL;
pDeviceFeatureValidationFunc	DECore::DeviceFeatureFunction;
LPD3DXEFFECT					DECore::engineEffect = NULL;


// A little function for finding stuff in a vector/list etc, with a comparison function. I use
// this find function instead of the std::find() function only when i have to find a type that
// does not have it's own overloaded == operator.
template<class inputIter, class itemType>
bool Find( const itemType& item, inputIter start, inputIter end, bool (*ComparisonFunc)(const itemType&, const itemType&) = NULL )
{
	for( inputIter i = start; i != end; i++ )
	{
		if( ComparisonFunc( (*i), item ) )
			return true;
	}
	return false;
}

//Oveload per l'operatore =
STRUCT_DE_ADAPTERFORMAT& STRUCT_DE_ADAPTERFORMAT::operator = ( const STRUCT_DE_ADAPTERFORMAT& rhs )
{
	UINT i;

	Format = rhs.Format;
	Windowed = rhs.Windowed;
	AdapterID = rhs.AdapterID;
	ID = rhs.ID;

	BackBuffersList.clear();
	for( i = 0; i < rhs.BackBuffersList.size(); i++ )
		BackBuffersList.push_back( rhs.BackBuffersList[i] );

	ConflictsList.clear();
	for( i = 0; i < rhs.ConflictsList.size(); i++ )
		ConflictsList.push_back( rhs.ConflictsList[i] );

	MultisamplesList.clear();
	for( i = 0; i < rhs.MultisamplesList.size(); i++ )
		MultisamplesList.push_back( rhs.MultisamplesList[i] );

	DepthStencilList.clear();
	for( i = 0; i < rhs.DepthStencilList.size(); i++ )
		DepthStencilList.push_back( rhs.DepthStencilList[i] );

	MultisamplesList.clear();
	for( i = 0; i < rhs.MultisamplesList.size(); i++ )
		MultisamplesList.push_back( rhs.MultisamplesList[i] );

	VertexProcessingList.clear();
	for( i = 0; i < rhs.VertexProcessingList.size(); i++ )
		VertexProcessingList.push_back( rhs.VertexProcessingList[i] );

	return *this;
}


/*******************************
/*            INIT             *
/*******************************/
DLL_API bool DECore::Init(char *logFile, char *logTitle, char *logSubtitle)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	//Inizializzazione
	D3DObject=NULL;
	D3DDevice=NULL;	
	//Creazione
	engineObjectCreated = false;
	engineWindowCreated = false;
	engineDeviceCreated = false;
	//Parametri da file
	engineFromFile = false;
	//Settaggi eseguiti
	engineSetSettings = false;
	//FrameRate
	engineFPSTimebase = 0;
	engineFPSTime = 0;
	engineFPSFrame = 0;
	engineFPS = 0.0f;
	engineFPSMillisec = 0.0f;
	engineFPSMax = 0;
	engineFPSMin = 1000;
	//Inizializza le impostazioni base
	engineVertexProcessingID=0;
	engineVertexProcessingType=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	engineMultisampleType=0;
	engineMultisampleQuality=0;
	engineSync=false;
	engineGammaValue = 128;
	enginehInstance=NULL;
	engineProc=NULL;
	engineClassName="DesdinovaBAKClass";
	engineWindowTitle="Desdinova Engine";
	engineShowInfo=false;
	engineShowFPS=false;
	//Parametri minimi di ricerca
	engineMinWidth=320;
	engineMinHeight=200;
	engineMinBPP=16;
	DeviceFeatureFunction = NULL;
	//Riempie il present parameters e vertex proc con valori standard
	engineAdapterID = 0;
	enginePresentParameters.Windowed					= true;
	enginePresentParameters.AutoDepthStencilFormat		= D3DFMT_D16;
	enginePresentParameters.BackBufferCount				= 1;
	enginePresentParameters.BackBufferFormat			= D3DFMT_R5G6B5;
	enginePresentParameters.BackBufferWidth				= 640;
	enginePresentParameters.BackBufferHeight			= 480;
	enginePresentParameters.EnableAutoDepthStencil		= true;
	enginePresentParameters.Flags						= 0;
	enginePresentParameters.FullScreen_RefreshRateInHz	= 0;
	enginePresentParameters.hDeviceWindow				= NULL; //Verrà riempito quando si crea la finestra
	enginePresentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE;
	enginePresentParameters.MultiSampleQuality			= 0;
	enginePresentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;
	enginePresentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	enginePresentParameters.FullScreen_RefreshRateInHz	= 0;
	engineVertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	engineMinVertexShaderMaj = 2;
	engineMinVertexShaderMin = 0;
	engineMinPixelShaderMaj = 2;
	engineMinPixelShaderMin = 0;

	//Inizializza il file di log usato in tutta l'applicazione
	DELog::LogStart(logFile, logTitle, logSubtitle);//"..::Desdinova Engine::.. Log File");

	//Scrive le informazioni sul sistema
	DELog::LogTitle("<br>Initialization Sequence");
	try
	{
		DELog::LogInfo("<br><b>Log Start Informations</b>");
		//Preleva le informazioni sull'ora e sulla data
		char systemTime[12];
		char systemDate[12];
		memset(systemTime, 0, 12);
		memset(systemDate, 0, 12);
		_strtime(systemTime);
		_strdate(systemDate);
		DELog::LogInfo("<li>Date : %s",systemDate);
		DELog::LogInfo("<li>Time : %s",systemTime);
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}


	try
	{
		//Analizza la CPU corrente
		engineCPUInfo.AnalyzeSystem();
		DELog::LogInfo("<li>CPU Family: %s",engineCPUInfo.GetCPUInfoStructure()->strProcessorFamily);
		DELog::LogInfo("<li>CPU Family ID: %d",engineCPUInfo.GetCPUInfoStructure()->iProcessorFamily);
		DELog::LogInfo("<li>CPU Speed: %.2fMHz",(float)engineCPUInfo.GetCPUInfoStructure()->uiSpeedHz / 1000000.0f);
		DELog::LogInfo("<li>CPU TypeID: %d",engineCPUInfo.GetCPUInfoStructure()->iProcessorType);
		DELog::LogInfo("<li>CPU Type: %s",engineCPUInfo.GetCPUInfoStructure()->strProcessorType);
		DELog::LogInfo("<li>CPU Serial: %s",engineCPUInfo.GetCPUInfoStructure()->strProcessorSerial);
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}

    
	try
	{
		//Preleva le informazioni sul sistema operativo
		OSVERSIONINFO systemVersion;
		ZeroMemory( &systemVersion, sizeof(OSVERSIONINFO) );
		systemVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&systemVersion);
		
		if ((systemVersion.dwMajorVersion == 4) && (systemVersion.dwMinorVersion==0))
			DELog::LogInfo("<li>OS    : Windows 95 - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 4) && (systemVersion.dwMinorVersion==10))
			DELog::LogInfo("<li>OS    : Windows 98 - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 4) && (systemVersion.dwMinorVersion==90))
			DELog::LogInfo("<li>OS    : Windows Me - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 3) && (systemVersion.dwMinorVersion==51))
			DELog::LogInfo("<li>OS    : Windows NT 3.51 - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 4) && (systemVersion.dwMinorVersion==0))
			DELog::LogInfo("<li>OS    : Windows 95 - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 5) && (systemVersion.dwMinorVersion==0))
			DELog::LogInfo("<li>OS    : Windows 2000 - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 5) && (systemVersion.dwMinorVersion==1))
			DELog::LogInfo("<li>OS    : Windows XP - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else if ((systemVersion.dwMajorVersion == 5) && (systemVersion.dwMinorVersion==2))
			DELog::LogInfo("<li>OS    : Windows Server 2003 - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
		else
			DELog::LogInfo("<li>OS    : Windows Generic OS - %d.%d(%s)", systemVersion.dwMajorVersion, systemVersion.dwMinorVersion, systemVersion.szCSDVersion);
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}
	

	try
	{
		DELog::LogInfo("<li>Total Memory : %.0f kB",(float)DEUtility::GetTotalMemory() / 1024.0f);
		DELog::LogInfo("<li>Available Memory : %.0f kB",(float)DEUtility::GetAvailableMemory() / 1024.0f);
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}

	try
	{
		//Preleva la dimensione del desktop
		HWND desktopHWND = GetDesktopWindow();
		GetWindowRect(desktopHWND, &engineDesktopRect);
		engineDesktopBPP = GetDeviceCaps(GetDC(NULL), BITSPIXEL);
		DELog::LogInfo("<li>Desktop Caps: %d x %d x %d", engineDesktopRect.right, engineDesktopRect.bottom, engineDesktopBPP);
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}

	try
	{
		//Preleva la directory corrente
		char bufferCurPath[MAX_PATH]="";
		sprintf(bufferCurPath, "<li>Current Path: %s",DEUtility::GetCurrentPath());
		DELog::LogInfo(bufferCurPath);
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}

	//Versione dell'engine grafico
	#ifdef _DEBUG
		DELog::LogInfo("<li>Desdinova Engine Version: %s - Debug",this->GetVersion());
	#else
		DELog::LogInfo("<li>Desdinova Engine Version: %s - Release",this->GetVersion());
	#endif 

	try
	{
		//Preleva le informazioni sulla versione delle DX
		HRESULT hr;
		DWORD dwDirectXVersion = 0;
		TCHAR strDirectXVersion[10];
		hr = DEUtility::GetDXVersion( &dwDirectXVersion, strDirectXVersion, 10 );
		if( SUCCEEDED(hr) )
		{
			if (strcmp(strDirectXVersion,"9.0c")!=0)
			//if( dwDirectXVersion <  0x00090000 )
			{
				DELog::LogError("<li>DirectX Version : %s - Incorrect version. Please install Microsoft DirectX 9.0c or higher.", strDirectXVersion);
				engineObjectCreated=false;
				return false;
			}
			else
			{
				DELog::LogInfo("<li>DirectX Version: %s",strDirectXVersion);
			}
		}
		else
		{
			DELog::LogError("<li>DirectX Version: Unknown");
			engineObjectCreated=false;
			return false;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
		engineObjectCreated=false;
		return false;
	}



	//Inizializzazione DirectX
	DELog::LogInfo("<br><b>Direct3D Initialization</b>");

	//Inizializza gli oggetti com
	CoInitialize(NULL);

	//Inizializza le DirectX e gli adapters
	try
	{
		D3DObject = Direct3DCreate9( D3D_SDK_VERSION );
		if (D3DObject != NULL)
		{
			DELog::LogInfo("<li>Direct3DCreate9(D3D_SDK_VERSION): The function completed successfully");
			engineObjectCreated=true;
		}
		else
		{
			DELog::LogError("<li>Direct3DCreate9(D3D_SDK_VERSION): Error during DirectX initialization.");
			engineObjectCreated=false;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>Direct3DCreate9(D3D_SDK_VERSION): Error during DirectX initialization.");
		engineObjectCreated=false;
	}

	return engineObjectCreated;
}



/*******************************
/*        SETTINGS FILE        *
/*******************************/
DLL_API bool DECore::ExistSettingsFile(char *fileName)
{
	try
	{
		HANDLE hFileFind=NULL;
		WIN32_FIND_DATA FindFileData;
		hFileFind = FindFirstFile(fileName, &FindFileData);
		if (hFileFind != INVALID_HANDLE_VALUE)	//File trovato
		{
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}


DLL_API STRUCT_DE_DEVICESETTINGS *DECore::LoadSettingsFile(char *fileName)
{
	bool loaded=false;
	STRUCT_DE_DEVICESETTINGS loadDeviceSettings;
	loadDeviceSettings.adapterID = 0;
	loadDeviceSettings.presentParameters = enginePresentParameters;
	loadDeviceSettings.vertexProcessing = DE_VERTEXPROCESSINGTYPE_SW;
	loadDeviceSettings.minVertexShaderMaj = 2;
	loadDeviceSettings.minVertexShaderMin = 0;
	loadDeviceSettings.minPixelShaderMaj = 2;
	loadDeviceSettings.minPixelShaderMin = 0;
	
	try
	{
		//Carica il file specificato
		HANDLE hFileFind;
		HANDLE hFileLoad;
		WIN32_FIND_DATA FindFileData;
		hFileFind = FindFirstFile(fileName, &FindFileData);
		if (hFileFind != INVALID_HANDLE_VALUE)	//File trovato
		{
			//Crea il file da aprire
			hFileLoad=CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hFileLoad!=INVALID_HANDLE_VALUE)
			{
				unsigned long rwBytes;			
				SetFilePointer(hFileLoad, 0, NULL, FILE_CURRENT);
				if (ReadFile(hFileLoad, &loadDeviceSettings, sizeof(STRUCT_DE_DEVICESETTINGS), &rwBytes, NULL))
				{
					//Chiude il file se aperto
					CloseHandle(hFileLoad);
					loaded = true;
				}
			}
		}
	}
	catch(...)
	{
		loaded=false;
	}

	if (loaded==false) DELog::LogWarning("<li>Could not read '%s' config file. Using default settings.", fileName);
	return &loadDeviceSettings;
}


DLL_API bool DECore::SetSettingsFromParams(STRUCT_DE_DEVICESETTINGS setDeviceSettings)
{
	engineSetSettings = false;
	try
	{
		//Setta i volori generali come quelli passati
		enginePresentParameters = setDeviceSettings.presentParameters;
		engineVertexProcessingType = setDeviceSettings.vertexProcessing;
		engineAdapterID = setDeviceSettings.adapterID;
		engineMinVertexShaderMaj = setDeviceSettings.minVertexShaderMaj;
		engineMinVertexShaderMin = setDeviceSettings.minVertexShaderMin;
		engineMinPixelShaderMaj = setDeviceSettings.minPixelShaderMaj;
		engineMinPixelShaderMin = setDeviceSettings.minPixelShaderMin;
		engineFromFile=false;
		engineSetSettings = true;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}


DLL_API bool DECore::SetSettingsFromFile(char *fileName)
{
	HANDLE hFileFind=NULL;
	HANDLE hFileLoad=NULL;
	engineSetSettings = false;

	try
	{
		STRUCT_DE_DEVICESETTINGS *loadDeviceSettings;
		loadDeviceSettings = LoadSettingsFile(fileName);
		engineAdapterID = loadDeviceSettings->adapterID;
		enginePresentParameters = loadDeviceSettings->presentParameters;
		engineVertexProcessingType = loadDeviceSettings->vertexProcessing;
		engineMinVertexShaderMaj = loadDeviceSettings->minVertexShaderMaj;
		engineMinVertexShaderMin = loadDeviceSettings->minVertexShaderMin;
		engineMinPixelShaderMaj = loadDeviceSettings->minPixelShaderMaj;
		engineMinPixelShaderMin = loadDeviceSettings->minPixelShaderMin;

		engineFromFile=true; //Da file
		//Qui si dovrebbe creare la lista che verrà mostrata direttamente
		//nel gioco
		//Crea la lista principale dell'adapter se si è in fullscreen oppure in finestra
		//engineAdapterID = BuildAdapterList(!enginePresentParameters.Windowed, enginePresentParameters.Windowed, false);
		//Seleziona il formato corrente dell'adapter passato
		//engineFormatID = SelectAdapterID(engineAdapterID);
		engineSetSettings = true;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>Could not read '%s' config file.", fileName);		
		return false;
	}
	return false;
}


DLL_API bool DECore::SaveSettingsFile(char *fileName, STRUCT_DE_DEVICESETTINGS deviceSettings)
{
	try
	{
		//Crea il file da salvare e ci salva il presentParameters settato dalla finestar di config
		HANDLE hFileSave;
		hFileSave=CreateFile(fileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
		if (hFileSave!=INVALID_HANDLE_VALUE)
		{
			unsigned long rwBytes;
			SetFilePointer(hFileSave, 0, NULL, FILE_CURRENT);
			//Scrive il present parameters
			if (WriteFile(hFileSave, &deviceSettings, sizeof(STRUCT_DE_DEVICESETTINGS), &rwBytes, NULL))
			{
				//Chiude il file se aperto
				CloseHandle(hFileSave);
				return true;
			}
		}	
	}
	catch(...)
	{
		return false;
	}
	return false;
}




/*******************************
/*          MAIN WINDOW        *
/*******************************/
DLL_API HWND DECore::CreateMainWindow(HINSTANCE hInstance, char *WindowIDResource, DLGPROC Proc, char *WindowTitle, bool centerWindow)
{
	HWND hWnd=NULL;
	
	if(engineObjectCreated)
	{	
		if (!engineSetSettings)
		{
			DELog::LogWarning("<li>No engine settings defined. Use window standard settings.");
		}

		//General settings
		enginehInstance=hInstance;
		engineProc=Proc;
		engineClassName="";
		engineWindowTitle=WindowTitle;

		//With resource
		try
		{
			DELog::LogInfo("<li>Window Creation Mode: From resource");
			hWnd=CreateDialog(enginehInstance, WindowIDResource, NULL, engineProc);
			if(hWnd)
			{	
				//Setta il titolo
				SetWindowText(hWnd, engineWindowTitle);

				//Setta gli hwnd
				engineHWND = hWnd;
				engineRenderHWND = hWnd;
				if (centerWindow)
				{
					HWND hwndOwner; 
					RECT rc, rcDlg, rcOwner; 
					if ((hwndOwner = GetParent(hWnd)) == NULL) 
						hwndOwner = GetDesktopWindow(); 
					GetWindowRect(hwndOwner, &rcOwner); 
					GetWindowRect(hWnd, &rcDlg); 
					CopyRect(&rc, &rcOwner); 
					OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
					OffsetRect(&rc, -rc.left, -rc.top); 
					OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
					SetWindowPos(hWnd, HWND_TOP,  rcOwner.left + (rc.right / 2),  rcOwner.top + (rc.bottom / 2),  0, 0, SWP_NOSIZE);
				}				
				engineWindowCreated = true;
			}
			else
			{
				DELog::LogError("<li>CreateDialog(...) : NULL");
				engineWindowCreated=false;
			}
		}
		catch(...)
		{
			DELog::LogError("<li>CreateDialog(...) : NULL");
			engineWindowCreated=false;
		}

		if (engineWindowCreated==false)	DELog::LogError("<li>Error during window creation.");
	}
	return hWnd;
}


DLL_API HWND DECore::CreateMainWindow(HINSTANCE hInstance, DLGPROC Proc, char *ClassName, char *WindowTitle, bool centerWindow)
{
	HWND hWnd=NULL;
	
	if(engineObjectCreated)
	{	
		if (!engineSetSettings)
		{
			DELog::LogWarning("<li>No engine settings defined. Use window standard settings.");
		}

		//General settings
		enginehInstance=hInstance;
		engineProc=Proc;
		engineClassName=ClassName;
		engineWindowTitle=WindowTitle;

		DELog::LogInfo("<li>Window Creation Mode: From class");

		WNDCLASS wc;
		DWORD    dwExStyle;
		DWORD    dwStyle;
		RECT     WindowRect;
		WindowRect.left   = (long)0;
		WindowRect.right  = (long)enginePresentParameters.BackBufferWidth;
		WindowRect.top    = (long)0;
		WindowRect.bottom = (long)enginePresentParameters.BackBufferHeight;

		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc   = (WNDPROC)Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = enginehInstance;
		wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = engineClassName;

		RegisterClass(&wc);

		if (enginePresentParameters.Windowed) 
		{
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle=WS_OVERLAPPEDWINDOW;
		}
		else
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth  = (DWORD)enginePresentParameters.BackBufferWidth;
			dmScreenSettings.dmPelsHeight = (DWORD)enginePresentParameters.BackBufferHeight;
			dmScreenSettings.dmBitsPerPel = (DWORD)DEUtility::GetBitsPerPixel(enginePresentParameters.BackBufferFormat);
			dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;		
			ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
			dwExStyle=WS_EX_APPWINDOW;
			dwStyle=WS_POPUP;
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);

		hWnd=CreateWindowEx(dwExStyle,							// Extended Style For The Window
							engineClassName,					// Class Name
							engineWindowTitle,					// Window Title
							dwStyle |							// Defined Window Style
							WS_CLIPSIBLINGS |					// Required Window Style
							WS_CLIPCHILDREN,					// Required Window Style
							0, 0,								// Window Position
							WindowRect.right-WindowRect.left,   // Calculate Window Width
							WindowRect.bottom-WindowRect.top,   // Calculate Window Height
							NULL,								// No Parent Window
							NULL,								// No Menu
							enginehInstance,					// Instance
							NULL);								// Dont Pass Anything To WM_CREATE
		
		if(hWnd!=NULL)
		{
			//Setta gli hwnd
			engineHWND = hWnd;
			engineRenderHWND = hWnd;
			if (centerWindow)
			{
				HWND hwndOwner; 
				RECT rc, rcDlg, rcOwner; 
				if ((hwndOwner = GetParent(hWnd)) == NULL) 
					hwndOwner = GetDesktopWindow(); 
				GetWindowRect(hwndOwner, &rcOwner); 
				GetWindowRect(hWnd, &rcDlg); 
				CopyRect(&rc, &rcOwner); 
				OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
				OffsetRect(&rc, -rc.left, -rc.top); 
				OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
				SetWindowPos(hWnd, HWND_TOP,  rcOwner.left + (rc.right / 2),  rcOwner.top + (rc.bottom / 2),  0, 0, SWP_NOSIZE);
			}
			engineWindowCreated = true;
		}
		else
		{
			DELog::LogError("<li>CreateWindowEx(...) : NULL");
			engineWindowCreated=false;
		}

		if (engineWindowCreated==false)	DELog::LogError("<li>Error during window creation.");
	}
	return hWnd;
}



/********************************
/*      INIT DIRECTX DEVICE     *
/********************************/
DLL_API bool DECore::CreateDevice()
{
	//Se l'oggetto DirectX e la finestra sono stati creati procede
	if ((engineObjectCreated)&&(engineWindowCreated))
	{
		HRESULT hr;
		try
		{
			if (engineFromFile)	DELog::LogInfo("<li>Settings Mode: From file");
			else				DELog::LogInfo("<li>Settings Mode: From parameters");

			//Valori preimpostati o calcolati a runtime...
			enginePresentParameters.hDeviceWindow = engineRenderHWND;
			//Solo se da file xkè se da parametri allora rimangono quelli specificati
			if (engineFromFile)
			{
				DWORD curFlags;
				if (enginePresentParameters.AutoDepthStencilFormat == D3DFMT_D16_LOCKABLE)	
					curFlags = D3DPRESENTFLAG_DEVICECLIP;	//Se il formato dello stencil è LOCKABLE allora non può essere DISCARD
				else																		
					curFlags = D3DPRESENTFLAG_DEVICECLIP | D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
				enginePresentParameters.Flags = curFlags;
				enginePresentParameters.BackBufferCount = 1;
				enginePresentParameters.EnableAutoDepthStencil = true;
				enginePresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			}

			if (enginePresentParameters.Windowed)
				DELog::LogInfo("<li>Used Windowed: TRUE");
			else
				DELog::LogInfo("<li>Used Windowed: FALSE");
			DELog::LogInfo("<li>Used BackBuffer Format: %s - %d Bpp", DEUtility::D3DFormatToString( enginePresentParameters.BackBufferFormat ), DEUtility::GetBitsPerPixel( enginePresentParameters.BackBufferFormat ));
			
			if ((enginePresentParameters.BackBufferWidth==0)&&(enginePresentParameters.BackBufferHeight==0))
				DELog::LogInfo("<li>Used BackBuffer Dimensions: AutoDetected");		
			else
				DELog::LogInfo("<li>Used BackBuffer Dimensions: %dx%d", enginePresentParameters.BackBufferWidth, enginePresentParameters.BackBufferHeight);		
			
			DELog::LogInfo("<li>Used Refresh Rate: %d", enginePresentParameters.FullScreen_RefreshRateInHz);
			DELog::LogInfo("<li>Used AutoDepthStencil Format: %s - %.0f Reserved Bpp", DEUtility::D3DFormatToString( enginePresentParameters.AutoDepthStencilFormat ), (float)DEUtility::GetStencilBits(enginePresentParameters.AutoDepthStencilFormat ));
			DELog::LogInfo("<li>Used Multisample Type and Quality: %s - %d", DEUtility::MultisampleTypeToString( enginePresentParameters.MultiSampleType ), enginePresentParameters.MultiSampleQuality);
			DELog::LogInfo("<li>Used Presentation Interval: %s", DEUtility::PresentIntervalToString(enginePresentParameters.PresentationInterval));
			
			//Preleva i caps
			D3DCAPS9 pCaps;
			D3DObject->GetDeviceCaps(engineAdapterID, D3DDEVTYPE_HAL, &pCaps);

			//Controlla il puredevice che deve essere anche hardware
			if (engineVertexProcessingType == D3DCREATE_PUREDEVICE)
			{
				engineVertexProcessingType = D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
			}


			//Controlla il vertproc HARDWARE scelto può essere eseguito
			if ((engineVertexProcessingType == D3DCREATE_HARDWARE_VERTEXPROCESSING) || (engineVertexProcessingType == (D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING)))
			{
				//Controllo T&L
				if ((pCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0)
				{
					DELog::LogError("<li>Requested Hardware T&L - Forced software vertex processing");
					engineVertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}
				else
				{
					DELog::LogInfo("<li>Hardware T&L supported");
				}
			}

			//Controllo su VS
			if (pCaps.VertexShaderVersion < D3DVS_VERSION(engineMinVertexShaderMaj, engineMinVertexShaderMin))	
			{
				DELog::LogError("<li>Vertex Shader version %d.%d - Requested Vertex Shader version %d.%d: Forced software vertex processing", (int)HIBYTE(pCaps.VertexShaderVersion), (int)LOBYTE(pCaps.VertexShaderVersion), engineMinVertexShaderMaj, engineMinVertexShaderMin);
				engineVertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}
			else
			{
				DELog::LogInfo("<li>Vertex Shader version %d.%d - Minimum Required: %d.%d", (int)HIBYTE(pCaps.VertexShaderVersion), (int)LOBYTE(pCaps.VertexShaderVersion), engineMinVertexShaderMaj, engineMinVertexShaderMin);		
			}

			//Controllo su PS
			if (pCaps.PixelShaderVersion < D3DPS_VERSION(engineMinPixelShaderMaj, engineMinPixelShaderMin))
			{
				DELog::LogError("<li>Pixel Shader version %d.%d - Requested Pixel Shader version %d.%d: Forced software vertex processing", (int)HIBYTE(pCaps.PixelShaderVersion), (int)LOBYTE(pCaps.PixelShaderVersion), engineMinPixelShaderMaj, engineMinPixelShaderMin);
				engineVertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}
			else
			{
				DELog::LogInfo("<li>Pixel Shader version %d.%d - Minimum Required: %d.%d", (int)HIBYTE(pCaps.PixelShaderVersion), (int)LOBYTE(pCaps.PixelShaderVersion), engineMinVertexShaderMaj, engineMinVertexShaderMin);
			}


			DELog::LogInfo("<li>Used Vertex Processing: %s", DEUtility::VertexProcessingToString(engineVertexProcessingType));


			//Tutto apposto...si può inizializzare il device con le impostazioni finali
			//Crea il device
			hr = D3DObject->CreateDevice(engineAdapterID, D3DDEVTYPE_HAL, engineRenderHWND, engineVertexProcessingType , &enginePresentParameters, &D3DDevice);
			if (hr==D3D_OK)
			{
				DELog::LogInfo("<li>D3DObject->CreateDevice(...): %s", DXGetErrorDescription9(hr));
					
				//Setta lo swap chain
				engineSwapChain.presentParameters = enginePresentParameters;
				engineSwapChain.hWnd = engineRenderHWND;
				engineSwapChain.isLost = false;
				engineSwapChain.isCreated = true;

				//Finalmente tutto è creato
				engineDeviceCreated = true;		
				
				//Crea i vertex buffer statici (bbox, bsphere...)
				if (DEVBManager::CreateStaticElements())	DELog::LogInfo("<li>DEVBManager::CreateStaticElements(...): The function completed successfully");		
				else										DELog::LogError("<li>DEVBManager::CreateStaticElements(...): Error during initialization");		

				D3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
				D3DDevice->SetDialogBoxMode(true);

				DELog::LogInfo("<li>Initial available textures memory = %.0fMb", (float)GetAvailableTextureMemory());


				//Crea lo shader principale
				char namePath[MAX_PATH];
				strcpy(namePath, DEUtility::GetCurrentPath());
				strcat(namePath, "Shaders\\Main.fx");
				ID3DXBuffer* pd3dErrors = NULL;
				hr = D3DXCreateEffectFromFile(DECore::D3DDevice, namePath, NULL,NULL,D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, NULL, &engineEffect, &pd3dErrors );
				if (hr==D3D_OK)
				{
					DELog::LogInfo("<li>D3DXCreateEffectFromFile( %s ): Created successfully", namePath);
				}
				else
				{
					DELog::LogError("<li>D3DXCreateEffectFromFile( %s ): %s", namePath, (const char*)pd3dErrors->GetBufferPointer());
					pd3dErrors->Release();
				}




			}
			else
			{
				DELog::LogError("<li>D3DObject->CreateDevice(...): %s", DXGetErrorDescription9(hr));
			}
		}
		catch(...)
		{
			DELog::LogError("<li>Unknown statement error");		
		}

		if (engineDeviceCreated)
		{
			DELog::LogInfo("<li>Created successfully");		
		}
		else
		{
			DELog::LogError("<li>NOT Created");		
		}

		//Inizializza il resto
		if (engineDeviceCreated)
		{
			//Inizializzazione le DirectInput
			engineInput.Init(engineHWND);
			engineInput.SetHWND(engineSwapChain.hWnd);
			if (engineSwapChain.presentParameters.Windowed == 1)	engineInput.SetWindowed(true);
			else													engineInput.SetWindowed(false);
			engineInput.SetCursorAttributes(false,1.5f);

			//Inizializza FMOD Sound System
			engineSound.Create(100);
			engineSound.Set3DSettings(1.0f, 1.0f,1.0f);
			engineSound.SetSpeakerMode(FMOD_SPEAKERMODE_QUAD);


			//Create the physX
			DELog::LogInfo("<br><b>PhysX Engine Initialization</b>");
			try
			{
				enginePhysX = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
				if (enginePhysX!=NULL)
				{
					//HW Detect
					engineUsePhysXHardware = false;
					if (enginePhysX->getHWVersion() != NX_HW_VERSION_NONE)
					{
						engineUsePhysXHardware = true;
						DELog::LogInfo("<li>PhysX Hardware Detected - Hardware mode");
					}
					else
					{
						engineUsePhysXHardware = false;
						DELog::LogWarning("<li>PhysX Hardware NOT Detected - Software mode");
					}
					DELog::LogInfo("<li>PhysX Engine Created");					

					//PhysX SDK Params
					enginePhysX->setParameter(NX_SKIN_WIDTH, 0.01);								//Default value for NxShapeDesc::skinWidth, see API reference for more info. (range: [0, inf) Unit: distance. See SkinWidth.
					enginePhysX->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, (0.15*0.15));	//The default linear velocity, squared, below which objects start going to sleep. (range: [0, inf))
					enginePhysX->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, (0.14*0.14));	//The default angular velocity, squared, below which objects start going to sleep. (range: [0, inf))
					enginePhysX->setParameter(NX_BOUNCE_THRESHOLD, -2);							//A contact with a relative velocity below this will not bounce. (range: (-inf, 0])
					enginePhysX->setParameter(NX_DYN_FRICT_SCALING, 1);							//This lets the user scale the magnitude of the dynamic friction applied to all objects. (range: [0, inf))
					enginePhysX->setParameter(NX_STA_FRICT_SCALING, 1);							//This lets the user scale the magnitude of the static friction applied to all objects. (range: [0, inf))
					enginePhysX->setParameter(NX_MAX_ANGULAR_VELOCITY, 7);						//See API Reference for NxBody::setMaxAngularVelocity() for details.
					enginePhysX->setParameter(NX_CONTINUOUS_CD, 0);								//Enable/disable continuous collision detection (0.0f to disable)
					enginePhysX->setParameter(NX_ADAPTIVE_FORCE, 1);							//Used to enable adaptive forces to accelerate convergence of the solver.
					enginePhysX->setParameter(NX_COLL_VETO_JOINTED, 1);							//Controls default filtering for jointed bodies (true = collision disabled).
					enginePhysX->setParameter(NX_TRIGGER_TRIGGER_CALLBACK, 1);					//Controls whether two touching triggers generate a callback or not.
					enginePhysX->setParameter(NX_CCD_EPSILON, 0.01);							//Distance epsilon for CCD algorithm.
					enginePhysX->setParameter(NX_SOLVER_CONVERGENCE_THRESHOLD, 0);				//Used to accelerate the solver.
					enginePhysX->setParameter(NX_BBOX_NOISE_LEVEL, 0.001);						//Used to accelerate HW Broad Phase.
					enginePhysX->setParameter(NX_IMPLICIT_SWEEP_CACHE_SIZE, 5.0);				//Used to set the sweep cache size.
					enginePhysX->setParameter(NX_DEFAULT_SLEEP_ENERGY, 0.005);					//The default sleep energy threshold. Objects with an energy below this threshold are allowed to go to sleep. Note: Only used when the NX_BF_ENERGY_SLEEP_TEST flag is set.
					enginePhysX->setParameter(NX_CONSTANT_FLUID_MAX_PACKETS, 924);				//Constant for the maximum number of packets per fluid. Used to compute the fluid packet buffer size in NxFluidPacketData. 
					enginePhysX->setParameter(NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP, 4096);	//Constant for the maximum number of new fluid particles per frame.


					//Cooking
					enginePhysXCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
					if (enginePhysXCooking!=NULL)
					{
						if (enginePhysXCooking->NxInitCooking())
							DELog::LogInfo("<li>PhysX Cooking Engine Created");
						else
							DELog::LogWarning("<li>PhysX Cooking Engine NOT Created");
					}
					else
					{
						DELog::LogWarning("<li>PhysX Cooking Engine NOT Created");
					}

					//VRD Debugging
					#ifdef _DEBUG
						if (enginePhysX->getFoundationSDK().getRemoteDebugger() && !enginePhysX->getFoundationSDK().getRemoteDebugger()->isConnected())
						{
							enginePhysX->getFoundationSDK().getRemoteDebugger()->connect("localhost", NX_DBG_DEFAULT_PORT, NX_DBG_EVENTMASK_EVERYTHING);
							DELog::LogWarning("<li>PhysX Engine Connected to Remote Debugger");
						}
					#endif
				}
				else					
				{
					DELog::LogError("<li>PhysX Engine NOT Created");
				}
			}
			catch(...)
			{				
				DELog::LogError("<li>NOT Created");		
			}


			//Inizializzazione console
			DELog::LogInfo("<br><b>Console System Initialization</b>");
			try
			{
				if (engineConsole.Create("Arial", 10, true, false, D3DCOLOR_XRGB(0, 255, 0), false, "engineConsole"))
				{
					char mConsoleMain[1024];
					char numPick[10]="";
					sprintf(numPick,"%s",this->GetVersion());
					strcpy(mConsoleMain, "__________________________________________________ _ _ _  _  _  _   _   _   _    _    _    _    _\n");
					strcat(mConsoleMain, "..:: Desdinova Engine ");
					strcat(mConsoleMain, numPick);
					strcat(mConsoleMain, " - Console System ::..\n");					
					engineConsole.SetCaption(mConsoleMain, 2);
					DELog::LogInfo("<li>Created successfully");		
				}
				else
				{
					DELog::LogError("<li>NOT Created");		
				}
			}
			catch(...)
			{				
				DELog::LogError("<li>NOT Created");		
			}


			//Inizializzazione Font
			engineFont.Create("Verdana", 10, false, false, "engineFont");


			//Log renderStaes Iniziali
			LogRenderStates(DE_LOGRENDERSTATES_ALL);


			DELog::LogTitle("<br><hr>Main Sequence");
		}
	}//engineObjectCreated e engineWindowCreated
	else
	{
		DELog::LogError("<li>Direct3D Object or Main Window NOT Created.");
		Release();
	}

	return engineDeviceCreated;
}



/*******************************
/*      POST-RENDER INFO       *
/*******************************/
DLL_API bool DECore::PostRender()
{	
	if (D3DDevice->BeginScene()==D3D_OK)
	{
		//Font info
		float infoX = 15;
		float infoYvar = 15;
		float infoY=5;
		//FPS
		if (engineShowFPS)
		{
			infoY = 5 + infoYvar;
			char bufferRate[40]="";
			sprintf(bufferRate, "FPS: %d (%.3fms)",(int)engineFPS, engineFPSMillisec);
			engineFont.BeginTextSprite();
			if (engineFPS >= 30)	engineFont.SetColor(D3DCOLOR_XRGB(255, 255, 255));
			else					engineFont.SetColor(D3DCOLOR_XRGB(255, 0, 0));
			engineFont.SetRectSprite(infoX, 5, 0, 0, DT_NOCLIP);
			engineFont.SetText(bufferRate);
			engineFont.Render();
			engineFont.EndTextSprite();
		}
		//Info
		if (engineShowInfo)
		{
			char bufferInfo[MAX_PATH]="";

			//Info sound
			float dsp, stream, update, total;
			engineSound.GetCPUUsage(&dsp, &stream, &update, &total);
			int a,b,c,d,e;
			engineSound.GetHardwareChannels(&a, &b, &c);
			engineSound.GetSoftwareChannels(&d);
			engineSound.GetChannelsPlaying(&e);


			engineFont.BeginTextSprite();
			engineFont.SetColor(D3DCOLOR_XRGB(255, 255, 255));

			#ifdef _DEBUG
				sprintf(bufferInfo, "Desdinova Engine version %s - DEBUG",this->GetVersion());
					engineFont.SetText(bufferInfo);
					engineFont.SetRectSprite(infoX, infoY + (infoYvar*0), 0, 0, DT_NOCLIP);
					engineFont.Render();
			#else
				sprintf(bufferInfo, "Desdinova Engine version %s - RELEASE",this->GetVersion());
					engineFont.SetText(bufferInfo);
					engineFont.SetRectSprite(infoX, infoY + (infoYvar*0), 0, 0, DT_NOCLIP);
					engineFont.Render();
			#endif 

			if (engineSwapChain.presentParameters.Windowed)
			{
				sprintf(bufferInfo, "%dx%d - Windowed", engineSwapChain.presentParameters.BackBufferWidth, engineSwapChain.presentParameters.BackBufferHeight );
					engineFont.SetText(bufferInfo);
					engineFont.SetRectSprite(infoX, infoY + (infoYvar*1), 0, 0, DT_NOCLIP);
					engineFont.Render();
			}
			else
			{
				sprintf(bufferInfo, "%dx%d - Fullscreen", engineSwapChain.presentParameters.BackBufferWidth, engineSwapChain.presentParameters.BackBufferHeight );		
					engineFont.SetText(bufferInfo);
					engineFont.SetRectSprite(infoX, infoY + (infoYvar*1), 0, 0, DT_NOCLIP);
					engineFont.Render();
			}
			sprintf(bufferInfo, "Current Path: %s",DEUtility::GetCurrentPath());
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*2), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Vertex Processing: %s", DEUtility::VertexProcessingToString(engineVertexProcessingType) );
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*3), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "BackBuffer Format: %s - %d Bpp", DEUtility::D3DFormatToString(engineSwapChain.presentParameters.BackBufferFormat),DEUtility::GetBitsPerPixel(engineSwapChain.presentParameters.BackBufferFormat));
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*4), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "DepthStencil Format: %s", DEUtility::D3DFormatToString(engineSwapChain.presentParameters.AutoDepthStencilFormat));
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*5), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Multisample: %s - %d", DEUtility::MultisampleTypeToString(engineSwapChain.presentParameters.MultiSampleType),engineSwapChain.presentParameters.MultiSampleQuality );
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*6), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Gamma Value: %.1f",(float)engineGammaValue);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*7), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*8), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem Total HW Channels: (2D/3D/Tot): %d/%d/%d",a,b,c);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*9), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem Total SW Channels: %d",d);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*10), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem Current Used Channels: %d",e);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*11), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem DSP CPU Time: %.2f%s",(float)dsp, "%");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*12), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem Stream CPU Time: %.2f%s",(float)stream, "%");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*13), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem Update CPU Time: %.2f%s",(float)update, "%");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*14), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "SoundSystem Total CPU Time: %.2f%s",(float)total, "%");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*15), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*16), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Total System Memory: %.0f kB",(float)DEUtility::GetTotalMemory()/1024.0f);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*17), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Process System Memory: %.0f kB",(float)DEUtility::GetCurrentProcessMemoryUsage()/1024.0f);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*18), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Available System Memory: %.0f kB",(float)DEUtility::GetAvailableMemory()/1024.0f);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*19), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Available Textures Memory: %.0f MB",(float)this->GetAvailableTextureMemory());
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*20), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "");			
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*21), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Loaded Textures(Un/Tot): %d/%d",(int)DETextureManager::GetLoadedTextures(),(int)DETextureManager::GetSearchedTextures());
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*22), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Loaded Materials(Un/Tot): %d/%d",(int)DEMaterialManager::GetLoadedMaterials(),(int)DEMaterialManager::GetSearchedMaterials());
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*23), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Loaded Meshes(Un/Tot): %d/%d",(int)DEMeshManager::GetLoadedMeshes(),(int)DEMeshManager::GetSearchedMeshes());
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*24), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*25), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "Mouse Position(X/Y): %d/%d",(int)engineInput.GetCursorPosition().x, (int)engineInput.GetCursorPosition().y);
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*26), 0, 0, DT_NOCLIP);
				engineFont.Render();
			sprintf(bufferInfo, "");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*27), 0, 0, DT_NOCLIP);
				engineFont.Render();

			if (engineUsePhysXHardware)	sprintf(bufferInfo, "PhysX Hardware Detected");
			else						sprintf(bufferInfo, "PhysX Hardware NOT Detected");
				engineFont.SetText(bufferInfo);
				engineFont.SetRectSprite(infoX, infoY + (infoYvar*28), 0, 0, DT_NOCLIP);
				engineFont.Render();

			engineFont.EndTextSprite();
		}

		//Console (internamente la disegna solo abilitata)
		engineConsole.SetPosition(D3DXVECTOR2(0, engineSwapChain.presentParameters.BackBufferHeight * 0.88f));
		engineConsole.Render();
		
		//Fine scena
		D3DDevice->EndScene();
	}
	return true;
}


/*******************************
/*        TEST e RESET         *
/*******************************/
DLL_API bool DECore::TestDevice()
{
	HRESULT hr;		
	if (D3DDevice!=NULL)
	{
			if ((engineSwapChain.isLost)&&(engineSwapChain.isCreated))
			{
				DELog::LogWarning("<br><b>Lost Device</b>");
				//CooperativeLevel
				hr = D3DDevice->TestCooperativeLevel();
				if(FAILED(hr))
				{
					DELog::LogInfo("<li>D3DDevice->TestCooperativeLevel(...): %s", DXGetErrorDescription9(hr));		
					//Il device è stato perso ma il reset NON è possibile allora prova successivamente
					if (hr == D3DERR_DEVICELOST)
					{
						Sleep(20);
						return true;
					}
					//Il device è stato perso ma il reste è possible allora lo esegue
					else if(hr == D3DERR_DEVICENOTRESET)
					{		
						//Potrebbe essere stato modificato il desktop allora ricalcola TUTTO
						//e ne setta le nuove proprietà (se potrebbe essere passati da 32 a 16bit)
						//unsigned int uiAdapter = BuildAdapterList(true, true, false);
						//unsigned int afID = SelectAdapterID(enginePresentParameters.Windowed, uiAdapter);
						//FillPresentParameters(uiAdapter, afID);
						//Resetta il device
						if(ResetDevice())
						{
							engineSwapChain.isLost = false;
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;			//Non è stato resettato il device
					}
				}
			}//*/
	}
	else
	{
		return false;
	}
	return true;//Tutto ok
}



DLL_API bool DECore::ResizeDevice(unsigned int newWidth, unsigned int newHeight)
{
	//Esegue solo se è stato creato il device
	//Questo controllo serve solitamente all'inizio di una applicazione fullscreen
	if (D3DDevice!=NULL)
	{
		if (engineSwapChain.isCreated==true)
		{
			//Setta la dimensione della schermata(per esempio se si fa il resize)
			if ((newWidth!=engineSwapChain.presentParameters.BackBufferWidth)||(newHeight!=engineSwapChain.presentParameters.BackBufferHeight))
			{
				//In finestra è possibile fare il resize
				//In fullscreen bisognerebbe vedere la compatibilità con la lista 
				//di risoluzioni accettabili
				if (engineSwapChain.presentParameters.Windowed)
				{
					DELog::LogInfo("<li>Old BackBuffer Dimensions: %dx%d", engineSwapChain.presentParameters.BackBufferWidth, engineSwapChain.presentParameters.BackBufferHeight);		
					//Setta il nuovo PresentParameters
					engineSwapChain.presentParameters.BackBufferWidth	= newWidth;
					engineSwapChain.presentParameters.BackBufferHeight	= newHeight;
					if ((newWidth==0)&&(newHeight==0))
						DELog::LogInfo("<li>New BackBuffer Dimensions: AutoDetected");		
					else
						DELog::LogInfo("<li>New BackBuffer Dimensions: %dx%d", engineSwapChain.presentParameters.BackBufferWidth, engineSwapChain.presentParameters.BackBufferHeight);		
					//Resetta il device
					if (ResetDevice())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}


DLL_API bool DECore::ResetDevice()
{
	HRESULT hr;
	if (D3DDevice!=NULL)
	{
		//Console
		engineConsole.Invalidate();
		//Font
		engineFont.Invalidate();

		//Rilascia tutte le scene associate
		for (unsigned int s=0; s<engineSwapChain.totalScenes; s++)
		{
			if (engineSwapChain.scene.at(s) != NULL)
			{
				if (engineSwapChain.scene.at(s)->Invalidate())	DELog::LogInfo("<li>Invalidate Scene %d: Completed successfully", (int)s);
				else											DELog::LogError("<li>Invalidate Scene %d: NOT Completed", (int)s);
			}
		}

		//Rilascia gli elementi statici
		DEVBManager::InvalidateStaticElements();
		DETextureManager::Invalidate();
		DEMeshManager::Invalidate();


		//Resetta il device
		engineSwapChain.presentParameters.hDeviceWindow = engineHWND;
		hr = D3DDevice->Reset( &engineSwapChain.presentParameters );
		if (hr==D3D_OK)
		{
			DELog::LogInfo("<li>D3DDevice->Reset(...): %s", DXGetErrorDescription9(hr));

			//Ricrea le scene annesse
			for (unsigned int s=0; s<engineSwapChain.scene.size(); s++)
			{
				if (engineSwapChain.scene.at(s) != NULL)		
				{
					engineSwapChain.scene.at(s)->SetWidth(engineSwapChain.presentParameters.BackBufferWidth);
					engineSwapChain.scene.at(s)->SetHeight(engineSwapChain.presentParameters.BackBufferHeight);
					if (engineSwapChain.scene.at(s)->Restore()) DELog::LogInfo("<li>Restore Scene %d: Completed successfully", (int)s);
					else										DELog::LogError("<li>Restore Scene %d: NOT Completed", (int)s);
				}
			}

			//Ricrea gli elementi statici
			DEVBManager::RestoreStaticElements();
			DETextureManager::Restore();
			DEMeshManager::Restore();

			//Console
			engineConsole.Restore();
			//Font
			engineFont.Restore();

			return true;
		}
		else if (hr==D3DERR_DRIVERINTERNALERROR)
		{
			DELog::LogError("<li>D3DDevice->Reset(...): Driver Internal Error");
			Release();
			return false;
		}
		else if (hr == D3DERR_DEVICELOST)
		{
			DELog::LogError("<li>D3DDevice->Reset(...): Device Lost");
			return false;
		}
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			DELog::LogError("<li>D3DDevice->Reset(...): Out of Video Memory");
			Release();
			return false;
		}
		else
		{
			DELog::LogError("<li>D3DDevice->Reset(...): %s", DXGetErrorDescription9(hr));
			Release();
			return false;
		}
	}
	return true;
}//*/





/*******************************
/*            SCENES           *
/*******************************/
DLL_API bool DECore::AddScene(DEScene *newCurrentScene)
{
	try
	{
		//Deve esserci un ID dello swapchain valido
		if (newCurrentScene!= NULL)
		{	
			//Imposta l'HWND fullscreen  e dimensioni di scena
			newCurrentScene->SetHWND(engineSwapChain.hWnd);
			if (engineSwapChain.presentParameters.Windowed == 1)	newCurrentScene->SetWindowed(true);
			else													newCurrentScene->SetWindowed(false);
			
			newCurrentScene->SetWidth(engineSwapChain.presentParameters.BackBufferWidth);
			newCurrentScene->SetHeight(engineSwapChain.presentParameters.BackBufferHeight);
			//Inserisce la scena
			engineSwapChain.scene.push_back(newCurrentScene);
			engineSwapChain.totalScenes = engineSwapChain.totalScenes + 1;
			engineSwapChain.isEmpty = false;
			engineCurrentSceneID = engineSwapChain.totalScenes - 1;
			return true;
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}


DLL_API bool DECore::SetCurrentScene(unsigned int sceneID, bool releaseCurrentScene)
{
	try
	{
		DELog::LogInfo("<br><b>Setting New Current Scene</b>");
		DELog::LogInfo("<li>New Scene: %d" , sceneID);
		DELog::LogInfo("<li>Current Scene: %d", engineSwapChain.currenteScene);
		if (releaseCurrentScene)	DELog::LogInfo("<li>Release Current: True");
		else						DELog::LogInfo("<li>New Scene: False");
		//Deve esserci un ID dello swapchain valido
		if ((sceneID>=0)&&(sceneID<engineSwapChain.totalScenes))
		{
			//Imposta delle variabili temporanee che verranno controllate nel loop
			//per vedere se una scena è stata caricata/rilasciato o meno
			engineCurrentSceneID = sceneID;
			engineCurrentSceneReleaseID = engineSwapChain.currenteScene;
			engineCurrentSceneRelease = releaseCurrentScene;
			return true;
		}
		return false;
	}
	catch(...)
	{
		return false;
	}
	return false;
}





/*******************************
/*        GETTING INFO         *
/*******************************/
DLL_API inline LPDIRECT3D9 DECore::GetD3DObject()
{
	if (D3DObject != NULL)	return D3DObject;
	else					return NULL;
}

DLL_API inline LPDIRECT3DDEVICE9 DECore::GetD3DDevice()
{
	if (D3DDevice != NULL)	return D3DDevice;
	else					return NULL;
}

DLL_API inline DEInput DECore::GetDInput()
{
	return engineInput;
}

DLL_API inline DEConsole DECore::GetConsole()
{
	return engineConsole;
}

DLL_API inline DESoundSystem DECore::GetSoundSystem()
{
	return engineSound;
}

DLL_API std::vector<STRUCT_DE_ADAPTER> DECore::GetAdapter()
{
	if (engineAdapters.size())
	{
		return engineAdapters;
	}
	else
	{
		std::vector<STRUCT_DE_ADAPTER> nullAdapter;
		return nullAdapter;
	}
}

DLL_API HWND DECore::GetHWND()
{
	return engineHWND;
}

DLL_API HWND DECore::GetRenderHWND()
{
	return engineRenderHWND;
}

DLL_API HINSTANCE DECore::GetInstance()
{
	return enginehInstance;
}

DLL_API LPCTSTR DECore::GetWindowTitle()
{
	return engineWindowTitle;
}

DLL_API unsigned int DECore::GetVertexProcessing()
{
	return engineVertexProcessingID;
}

DLL_API const char *DECore::GetVersion()
{
	return CURRENT_VERSION;
}

DLL_API	unsigned int DECore::GetWidth()
{
	return engineSwapChain.presentParameters.BackBufferWidth;
}

DLL_API	unsigned int DECore::GetHeight()
{
	return engineSwapChain.presentParameters.BackBufferHeight;
}

DLL_API	float DECore::GetAspectRatio()
{
	return (float)engineSwapChain.presentParameters.BackBufferWidth / (float)engineSwapChain.presentParameters.BackBufferHeight;
}

DLL_API	bool DECore::GetWindowed()
{
	if (engineSwapChain.presentParameters.Windowed==1)	return true;
	else												return false;
}

DLL_API unsigned int DECore::GetDesktopWidth()
{
	return engineDesktopRect.right;
}
DLL_API unsigned int DECore::GetDesktopHeight()
{
	return engineDesktopRect.bottom;
}
DLL_API unsigned int DECore::GetDesktopBPP()
{
	return engineDesktopBPP;
}

DLL_API unsigned int DECore::GetAvailableTextureMemory()
{
	return DECore::D3DDevice->GetAvailableTextureMem()/(1024*1024);
}

DLL_API float DECore::GetFPS()
{	
	return engineFPS;
}




/*******************************
/*          CREAZIONE          *
/*******************************/
DLL_API bool DECore::isObjectCreated()
{
	return engineObjectCreated;
}
DLL_API bool DECore::isDeviceCreated()
{
	return engineDeviceCreated;
}
DLL_API bool DECore::isWindowCreated()
{
	return engineWindowCreated;
}


/*******************************
/*           SHOW INFO         *
/*******************************/
DLL_API bool DECore::GetShowInfo()
{
	return engineShowInfo;
}
DLL_API void DECore::SetShowInfo(bool showInfo)
{
	engineShowInfo = showInfo;
}



/*******************************
/*           SHOW FPS          *
/*******************************/
DLL_API bool DECore::GetShowFPS()
{
	return engineShowFPS;
}
DLL_API void DECore::SetShowFPS(bool showFPS)
{
	engineShowFPS = showFPS;
}



/*******************************
/*            LOOP             *
/*******************************/
DLL_API bool DECore::Loop()
{
	//Mostra la finestra creata(va fatto solo prima del loop)
	ShowWindow(engineHWND, SW_SHOW);
	SetForegroundWindow(engineHWND);
	SetFocus(engineHWND);
	UpdateWindow(engineHWND);

	DELog::LogTitle("<br><hr>Loop Sequence");
	//Ciclo principale (controlla i messaggi x uscire altrimenti renderizza)
	MSG msg;
	engineDone = false;	
	unsigned short logErrorPhase = 0;
	HRESULT hr;

	while(!engineDone)
	{	
		//*******************
		//**    MESSAGGI   **
		//*******************
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				engineDone=true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{	
			//*******************
			//** UPDATE DINPUT **
			//*******************
			engineInput.Update(true,true,true);

			//*******************
			//** UPDATE FMOD   **
			//*******************
			engineSound.Update();

			//***************
			//** RENDERING **
			//***************	
			try
			{
				logErrorPhase = 0;
				//La scena corrente è quella impostata temporaneamente
				engineSwapChain.currenteScene = engineCurrentSceneID;

				//Lo swap chain non è vuoto(ci sono delle scene, o almeno una)
				if (engineSwapChain.isEmpty==false)
				{
					//******************
					//** PRESENTATION **
					//******************	
					logErrorPhase = 1;
					//Testa lo swapchain corrente(se necessario li ricrea tutti)
					if (TestDevice())
					{						
						//La scena è già stata caricata allora esegue normalmente
						if (engineSwapChain.scene.at(engineSwapChain.currenteScene)->isLoaded())
						{
							//***************************
							//** UPDATE MOTORE FISICO  **
							//***************************
							logErrorPhase = 2;
							engineSwapChain.scene.at(engineSwapChain.currenteScene)->PreparePhysics();
							engineSwapChain.scene.at(engineSwapChain.currenteScene)->UpdatePhysics();

							//****************
							//** INPUT BASE **
							//****************	
							logErrorPhase = 3;
							//Console
							if (engineConsole.GetEnable())
							{
								char key = engineInput.Pressed_KeyboardKey();
								if (key!=NULL)	engineConsole.CmdDefault(key);

								if(engineInput.Pressed_Keyboard(DIK_DOWN, false))
								{
									engineConsole.CmdHistoryDown();
								}
								if(engineInput.Pressed_Keyboard(DIK_UP, false))
								{
									engineConsole.CmdHistoryUp();
								}
								if(engineInput.Pressed_Keyboard(DIK_RETURN, false))
								{
									engineConsole.CmdReturn();
								}	
								if(engineInput.Pressed_Keyboard(DIK_BACK, false))
								{
									engineConsole.CmdBack();
								}
							}
							//Controlli generali
							if (engineInput.Pressed_Keyboard(DIK_PAUSE, false))
							{
								//Mette in pausa la scena corrente dello swap chain attivo
								engineSwapChain.scene.at(engineSwapChain.currenteScene)->TogglePause();
							}
							if(engineInput.Pressed_Keyboard(DIK_F6, false))
							{
								this->engineConsole.SwitchEnable();
							}
							if(engineInput.Pressed_Keyboard(DIK_F12, false))
							{
								this->SaveScreenshot(NULL);
							}
							if (engineInput.Pressed_Keyboard(DIK_ADD, false))	
							{
								this->SetGammaValue(this->GetGammaValue() + 10, false);
							}
							if (engineInput.Pressed_Keyboard(DIK_SUBTRACT, false))
							{
								this->SetGammaValue(this->GetGammaValue() - 10, false);
							}
							if(engineInput.Pressed_Keyboard(DIK_ESCAPE, false))
							{
								this->ExitLoop();
							}//*/


							//***************
							//** RENDERING **
							//***************	
							logErrorPhase = 4;
							//** Solo swap chan attivo **
							if (engineSwapChain.scene.at(engineSwapChain.currenteScene)->isLoaded())
							{
								logErrorPhase = 5;
								//HWND della scena corrente...serve per la camera e i video
								engineSwapChain.scene.at(engineSwapChain.currenteScene)->SetHWND(engineSwapChain.hWnd);
								
								//** Update degli input **
								engineInput.SetHWND(engineSwapChain.hWnd);
								//Esegue gli input solo se la console non è attiva
								if (engineConsole.GetEnable() == false)
								{
									//Preleva gli input solo da quello attivo
									engineSwapChain.scene.at(engineSwapChain.currenteScene)->UpdateInput();
								}

								//** Ascoltatore **
								//Update dell'ascoltatore in base alla camera corrente
								FMOD_VECTOR listenerpos;
								FMOD_VECTOR forward;
								FMOD_VECTOR up;
								FMOD_VECTOR vel;
								static FMOD_VECTOR lastpos;
								D3DXVECTOR3 camPos = engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetCurrentCamera()->GetPosition();
								D3DXVECTOR3 camView = engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetCurrentCamera()->GetView();
								D3DXVECTOR3 camUp = engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetCurrentCamera()->GetUpVector();
								listenerpos.x = camPos.x;
								listenerpos.y = camPos.y;
								listenerpos.z = camPos.z;
								forward.x = camView.x;
								forward.y = camView.y;
								forward.z = camView.z;
								up.x = camUp.x;
								up.y = camUp.y;
								up.z = camUp.z;
								lastpos = listenerpos;	//Salva la posizione corrente per dopo
								vel.x = (listenerpos.x - lastpos.x) * (1 / engineFPS);
								vel.y = (listenerpos.y - lastpos.y) * (1 / engineFPS);
								vel.z = (listenerpos.z - lastpos.z) * (1 / engineFPS);
								engineSound.Set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up);
							}
							//** Inizio rendering **
							//Preleva il backbuffer dallo swapchain corrente
							logErrorPhase = 6;
							engineCurrentBackBuffer = NULL;
							hr = D3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &engineCurrentBackBuffer );
							if (hr==D3D_OK)
							{
								//Setta il render target come quello appena prelevato
								logErrorPhase = 7;
								hr = D3DDevice->SetRenderTarget( 0, engineCurrentBackBuffer );
								if (hr==D3D_OK)
								{
									logErrorPhase = 8;

									//Posizione del mouse
									engineSwapChain.scene.at(engineSwapChain.currenteScene)->SetCursorPosition(engineInput.GetCursorPosition());

									//Renderizza la scena (internamente esegue l'update degli input, il 3d, il 2d e controlla il PostProcessing)
									engineSwapChain.scene.at(engineSwapChain.currenteScene)->Execute();

									//Render font, console, ecc (sempre fuori dal PostProcessing della scena)
									PostRender();

									//Present
									logErrorPhase = 9;
									hr = D3DDevice->Present(NULL, NULL, NULL, NULL );
									if (hr==D3D_OK)
									{
										engineSwapChain.isLost = false;
									}
									else if (hr == D3DERR_DEVICELOST)
									{
										engineSwapChain.isLost = true;
									}
								}
								//Il backbuffer è stato creato e usato quindi lo rilascia
								engineCurrentBackBuffer->Release();
							}

						}
						else	//Carica la scena
						{

							//***********************
							//**  INIT LOAD SCENE  **
							//***********************
							logErrorPhase = 10;
							engineCurrentBackBuffer = NULL;
							HRESULT hr = D3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &engineCurrentBackBuffer );
							if (hr==D3D_OK)
							{
								//Setta il render target come quello appena prelevato
								logErrorPhase = 11;
								hr = D3DDevice->SetRenderTarget( 0, engineCurrentBackBuffer );
								if (hr==D3D_OK)
								{
									logErrorPhase = 12;
									hr = D3DDevice->Clear(engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetClearProperties().Clear_RectsCount, engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetClearProperties().Clear_Rects, engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetClearProperties().Clear_ClearFlags, D3DCOLOR_XRGB(0,0,0),engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetClearProperties().Clear_Z, engineSwapChain.scene.at(engineSwapChain.currenteScene)->GetClearProperties().Clear_Stencil);
									if (hr ==D3D_OK)
									{
										//Inizio scena
										logErrorPhase = 13;
										if (D3DDevice->BeginScene()==D3D_OK)
										{
											//*************************
											//** RILASCIO PRECEDENTE **
											//*************************
											logErrorPhase = 14;
											if (engineCurrentSceneRelease)
											{
												if (engineSwapChain.scene.at(engineCurrentSceneReleaseID)->Release())
												{
													//Il loop del rilascia della scena è terminato dunque nessuna scena è da rilasciare
													engineCurrentSceneRelease = false;
													engineCurrentSceneReleaseID = engineSwapChain.currenteScene;
												}
											}
											else
											{
												//***********************
												//**       LOAD        **
												//***********************
												if (engineSwapChain.scene.at(engineSwapChain.currenteScene)->Load())
												{
													logErrorPhase = 15;
													//Report fine caricamento
													DEModelManager::LogReport();
													DEMaterialManager::LogReport();
													DETextureManager::LogReport();
													DEMeshManager::LogReport();
													DELog::LogInfo("<li>Load scene completed");
												}
											}
											//Fine scena
											D3DDevice->EndScene();
										}
									}
									logErrorPhase = 16;
									engineCurrentBackBuffer->Release();
									hr = D3DDevice->Present(NULL, NULL, NULL, NULL );
									if (hr==D3D_OK)
									{
										engineSwapChain.isLost = false;
									}
									else if (hr == D3DERR_DEVICELOST)
									{
										engineSwapChain.isLost = true;
									}
								}
								engineCurrentBackBuffer->Release();
							}


						}//Carica scena
					}//TestDevice()
				}//Not Empty if


				//***********************
				//** CALCOLO FRAMERATE **
				//***********************
				logErrorPhase = 17;
				engineFPSFrame++;
				engineFPSTime = GetTickCount();
				if( engineFPSTime - engineFPSTimebase >= 1000 )
				{
					engineFPS = engineFPSFrame * 1000.0f / (engineFPSTime - engineFPSTimebase);
 					engineFPSTimebase = engineFPSTime;
					engineFPSMillisec = 1000.0f / engineFPS;
					engineFPSFrame = 0;
					if (engineFPS >= 1)
					{
						if (engineFPS>=engineFPSMax) engineFPSMax = engineFPS;
						if (engineFPS<=engineFPSMin) engineFPSMin = engineFPS;
					}				
				}

			}
			catch(...)
			{
				//*********************
				//** ERRORE GENERALE **
				//*********************
				if (D3DDevice!=NULL)
				{
					//Schermata blu
					D3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER ,D3DCOLOR_XRGB(64,0,150),1.0f,0 );
					D3DDevice->BeginScene();
					D3DDevice->EndScene();
					D3DDevice->Present(NULL, NULL, NULL, 0);
				}
				//Messaggio
				MessageBox(NULL,"Desdinova Engine - Runtime Error.\n\rRefer to HTML log file for more details on error.\n\r\n\rPlease visit www.desdinova.altervista.org for questions or bugs report.","Desdinova Engine - Runtime Error",MB_OK | MB_ICONERROR);
				//Errore nel log
				DELog::LogError("<br><hr size=3 color=red>Auto-Shutdown (ErrorPhaseCode = %d) <hr size=3 color=red>", logErrorPhase);
				//Esce
				engineDone=true;
			}
		}
	}

	//E' uscito dal loop...rilascia tutto
	Release();
	return false;
}


DLL_API void DECore::ExitLoop()
{
	engineDone = true;
}


/*******************************
/*          RILASCIO           *
/*******************************/
DLL_API bool DECore::Release()
{
	try
	{
		DELog::LogTitle("<br><hr>Release Sequence");

		//Creazioni
		engineDone = true;
		engineObjectCreated = false;
		engineWindowCreated = false;
		engineDeviceCreated = false;

		//Rilascia le scene
		for (unsigned int s=0; s<engineSwapChain.scene.size(); s++)
		{
			if (engineSwapChain.scene.at(s) != NULL)		
			{
				engineSwapChain.scene.at(s)->Release();
			}
		}//*/

		//Release DirectInput
		engineInput.Release();
		
		//Release FMOD Sound System
		engineSound.Release();
		
		//PhysX release
		if (enginePhysX)			enginePhysX->release();
		if (enginePhysXCooking)		NxCloseCooking();

		//Release elementi aggiuntivi
		engineConsole.Release();

		//Rilascia gli adapters
		engineAdapters.clear();

		//Rilascio textures e effetti
		DEModelManager::Release();
		DEMaterialManager::Release();
		DETextureManager::Release();
		DEMeshManager::Release();
		//Report textures e effetti
		DEModelManager::LogReport();
		DEMaterialManager::LogReport();
		DETextureManager::LogReport();
		DEMeshManager::LogReport();
		//Report comandi console
		engineConsole.LogReport();
				
		//Deinizializza gli oggetti COM
		CoUninitialize();

		//Rilascia gli oggetti caricati
		SafeRelease(D3DDevice);
		SafeRelease(D3DObject);

		//Rilascia la finestra e la classe
		DELog::LogInfo("<br><b>Main Window Release</b>");
		if(engineHWND)
		{
			MSG msg;
			if (DestroyWindow(engineHWND))
			{
				DELog::LogInfo("<li>DestroyWindow(...): The function completed successfully");
				engineHWND=NULL;
			}
			else
			{
				DWORD dw = GetLastError(); 
				DELog::LogWarning("<li>DestroyWindow(...): Error Code %.0f", (float)dw);
			}

			while(PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
			{
				DispatchMessage(&msg);
			}	
		}
		//Unregistra la classe solo se è stata creata
		if(engineClassName!="")
		{
			if (UnregisterClass(engineClassName,enginehInstance))
			{
				DELog::LogInfo("<li>UnregisterClass(...): The function completed successfully");
				engineClassName=NULL;
			}
			else
			{
				DWORD dw = GetLastError(); 
				DELog::LogWarning("<li>UnregisterClass(...): Error Code %.0f", (float)dw);
			}
		}//*/

		//Statistiche
		DELog::LogInfo("<br><b>Statistics:</b>");
		DELog::LogInfo("<li>Maximum Framerate: %d fps", (int)engineFPSMax);
		DELog::LogInfo("<li>Minimum Framerate: %d fps", (int)engineFPSMin);
		int engineFPSAvarange = (int)((((int)engineFPSMax - (int)engineFPSMin) / 2.0f) + (int)engineFPSMin);
		DELog::LogInfo("<li>Avarange Framerate: %d fps", (int)engineFPSAvarange);

		//Informazioni di uscita
		DELog::LogInfo("<br><b>Log Exit Informations</b>");
		//Preleva le informazioni sull'ora e sulla data
		char systemTime[12];
		char systemDate[12];
		memset(systemTime, 0, 12);
		memset(systemDate, 0, 12);
		_strtime(systemTime);
		_strdate(systemDate);
		DELog::LogInfo("<li>Date: %s",systemDate);
		DELog::LogInfo("<li>Time: %s",systemTime);

		//Thanks
		DELog::LogInfo("<br><hr>");
		DELog::LogInfo("<b>Main Site:</b>");
		DELog::LogInfo("<li><a href='http://www.desdinova.altervista.org'>www.desdinova.altervista.org</a>");
		DELog::LogInfo("<br><b>Thanks To:</b>");
		DELog::LogInfo("<li>'Mondo' for 3D models and brainstorming");
		DELog::LogInfo("<li>'Macman' for engine tests and his efficiency");
		DELog::LogInfo("<li><a href='http://www.gamedev.net'>www.gamedev.net</a> for its news, papers and forum");
		DELog::LogInfo("<li><a href='http://www.gameprog.it'>www.gameprog.it</a> and its forum's users");
		DELog::LogInfo("<li><a href='http://www.devmaster.net'>www.devmaster.net</a> for its informations and engines list");
		//Copyrights
		DELog::LogInfo("<br><b>Copyrights:</b>");
		DELog::LogInfo("<li><a href='http://www.desdinova.altervista.org'>Desdinova Engine %s</a> - Copyright © 2002-2007 Ferla Daniele. All rights reserved.", this->GetVersion());
		DELog::LogInfo("<li><a href='http://www.microsoft.com/directx'>DirectX 9.0c</a> - Copyright © 2005 Microsoft Corporation. All rights reserved.");
		DELog::LogInfo("<li><a href='http://www.fmod.org'>FMOD Sound System</a> - Copyright © 1994-2005 Firelight Technologies Pty Ltd. All rights reserved.");
		DELog::LogInfo("<li><a href='http://www.ageia.org'>PhysX Engine</a> - Copyright © 2007 AGEIA Technologies Inc, Suite 118, 82 Pioneer Way, Mountain View, CA 94041 USA. All rights reserved.");
		
		//Ferma il logging
		DELog::LogStop();
		
		return true;
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
		//Ferma il logging
		DELog::LogStop();
	}
	return false;
}



/*******************************
/*       GAMMA SETTINGS        *
/*******************************/
DLL_API bool DECore::SetGammaValue(float gammaValue, bool calibrateGamma)
{
	//La correzione del gamma può essere effettuata solo in fullscreen
	//Effetti sul gamma tipo flash o simili possono non essere calibrati ma effetti
	//persistenti devo essere calibrati
	try
	{
		D3DGAMMARAMP r;
		if (gammaValue >= 255)	gammaValue = 255;
		if (gammaValue <= 0)	gammaValue = 0;
		float g_gamma = logf(0.5f)/logf((float)gammaValue/255.f);
		float correction_gamma = 1/g_gamma;
		for (UINT i = 0; i < 256; i++)
		{
			r.red[i] = r.green[i] = r.blue[i] = (int)(65535*pow(i/255.f, correction_gamma));
		}	

		//In finestra
		if (enginePresentParameters.Windowed)
		{
			return false;
		}
		else	//In Fullscreen
		{
			D3DCAPS9 pCaps;
			D3DDevice->GetDeviceCaps(&pCaps);
			if( pCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA )
			{
				if (calibrateGamma)
				{
					if( pCaps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA )
						D3DDevice->SetGammaRamp(0, D3DSGR_CALIBRATE, &r);
					else
						D3DDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &r);
				}
				else
				{
					//Non esegue la calibrazione
					D3DDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &r);
				}
				engineGammaValue = gammaValue;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	catch(...)
	{
		return false;
	}


	return false;
}
DLL_API float DECore::GetGammaValue()
{
	return engineGammaValue;
}
DLL_API D3DGAMMARAMP *DECore::GetGammaRamp()
{
	D3DGAMMARAMP *curRamp=NULL;
	D3DDevice->GetGammaRamp(0, curRamp);
	return curRamp;
}




/*******************************
/*          SCREENSHOT         *
/*******************************/
DLL_API void DECore::GetScreenshotFileName(string& FileName)
{
    // retrieve the filename of the EXE file
	string ModuleFileName;
    ModuleFileName.reserve(MAX_PATH);
    GetModuleFileName( NULL, const_cast<char*>(ModuleFileName.data()), MAX_PATH);
    // extract the path info from the filename
    FileName = ModuleFileName.substr(0, ModuleFileName.find_last_of(":\\"));
    // append the sub-folder path
	// If, for example, you want to save the screenshots to the sub-folder
	// "shots", set this to "shots\\".
    FileName.append("ScreenShots\\");
	//Create directory if not exist
	CreateDirectory(FileName.c_str(), NULL);

    // search for first unused filename
    char Buf[MAX_PATH];
    WIN32_FIND_DATA ffd;
    HANDLE h;
    for (int i = 0; i < 1000; i++)
    {
        // prepare search mask for FindFirstFile
        wsprintf(Buf, (FileName + "ScreenShot%03i.bmp").c_str(), i);
        // check whether this file already exists
        h = FindFirstFile(Buf, &ffd);
        // if the file exists, close the search handle and continue
        if (h != INVALID_HANDLE_VALUE)
        {   
			FindClose(h); 
		}
        // if the file does not exist, exit from the loop
        else
        {   
			break; 
		}
    }

    // set FileName to the first unused filename
    FileName = Buf;
}

DLL_API bool DECore::SaveScreenshot(const char* lpszFileName)
{
	HRESULT hr;
    LPDIRECT3DSURFACE9 lpSurface = NULL;    // copy of the front buffer
    LPDIRECT3D9 lpD3D = NULL;				// IDirect3D9 interface that created lpD3DDevice
    LPBYTE Bits = NULL;						// bitmap bits of the DIB
    FILE* f = NULL;							// BMP file handle
	string FileName;						// name of the BMP file

	DELog::LogInfo("<br><b>Save Screenshot</b>");
	try
	{
		// if a filename is given, use it
		if (lpszFileName)
		{
			FileName = lpszFileName; 
		}
		// if no filename is given, have a filename generated
		else
		{   
			GetScreenshotFileName(FileName);
		}
		DELog::LogInfo("<li>Filename: %s", FileName.c_str());

		char screenshotTime[12];
		memset(screenshotTime, 0, 12);
		_strtime(screenshotTime);
		DELog::LogInfo("<li>Time : %s",screenshotTime);


		// We need to know for which adapter the Direct3D D3DDevice was created,
		// so that we can query the right adapter's screen dimensions.
		// Therefore, query the D3DDevice's creation parameters
		D3DDEVICE_CREATION_PARAMETERS dcp;
		dcp.AdapterOrdinal = D3DADAPTER_DEFAULT;
		hr = D3DDevice->GetCreationParameters(&dcp);
		if (hr==D3D_OK)
		{
			D3DDISPLAYMODE dm;
			dm.Width = dm.Height = 0;

			// retrieve pointer to IDirect3D9 interface,
			// which provides the GetAdapterDisplayMode method
			hr = D3DDevice->GetDirect3D(&lpD3D);
			if (hr==D3D_OK)
			{
				// query the screen dimensions of the current adapter
				hr = lpD3D->GetAdapterDisplayMode(dcp.AdapterOrdinal, &dm);
				if (hr==D3D_OK)
				{				
					SafeRelease(lpD3D);
					
					// create a 32-bit ARGB system-memory surface that is going to receive
					// a copy of the front buffer's contents
					hr = D3DDevice->CreateOffscreenPlainSurface(dm.Width, dm.Height,D3DFMT_A8R8G8B8,D3DPOOL_SCRATCH,&lpSurface,NULL);

					if (hr==D3D_OK)
					{
						// have the GetFrontBuffer method copy the contents of the front
						// buffer to our system-memory surface						
						hr = D3DDevice->GetFrontBufferData(0, lpSurface);
						//hr = D3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &lpSurface);
						if (hr==D3D_OK)
						{
							RECT windowRect;
							GetWindowRect(engineSwapChain.hWnd, &windowRect);
							hr = D3DXSaveSurfaceToFile(FileName.c_str(),D3DXIFF_BMP,lpSurface,NULL,&windowRect);
							if (hr==D3D_OK)
							{
								DELog::LogInfo("<li>D3DXSaveSurfaceToFile(...): %s", DXGetErrorDescription9(hr));
								return true;
							}
							else
							{
								DELog::LogError("<li>D3DXSaveSurfaceToFile(...): %s", DXGetErrorDescription9(hr));
							}//*/
						}
						else
						{
							DELog::LogError("<li>D3DDevice->GetFrontBuffer(...): %s", DXGetErrorDescription9(hr));
						}
					}
					else
					{
						DELog::LogError("<li>D3DDevice->CreateImageSurface(...): %s", DXGetErrorDescription9(hr));
					}
				}
				else
				{
					DELog::LogError("<li>lpD3D->GetAdapterDisplayMode(...): %s", DXGetErrorDescription9(hr));
				}
			}
			else
			{
				DELog::LogError("<li>D3DDevice->GetDirect3D(...): %s", DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>D3DDevice->GetCreationParameters(...): %s", DXGetErrorDescription9(hr));
		}
	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");
	}

    return false;
}



/*******************************
/*       LOG RENDERSTATES      *
/*******************************/
void DECore::LogRenderStates(ENUM_DE_LOGRENDERSTATES Flags)
{
	DWORD State;

	if(D3DDevice == NULL)
	{
		return;
	}
	if (Flags)
	{
		DELog::LogInfo("<br><b>Current Render States Report:</b>");
	}
	
	if(Flags & DE_LOGRENDERSTATES_LIGHTING)
	{
		DELog::LogInfo("<li><u><b>Lighting Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_LIGHTING,&State);
		if(State == TRUE) DELog::LogInfo("<li>Lighting: True");
		else DELog::LogInfo("<li>Lighting: False");

		D3DDevice->GetRenderState(D3DRS_AMBIENT,&State);		
		//sprintf(str,"0x%8.8x",State);	
		DELog::LogInfo("<li>Ambient: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_NORMALIZENORMALS,&State);
		if(State == TRUE) DELog::LogInfo("<li>Normalize Normals: True");
		else DELog::LogInfo("<li>Normalize Normals: False");

		D3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,&State);
		if(State == D3DMCS_MATERIAL)	  DELog::LogInfo("<li>Diffuse Material Source: D3DMCS_MATERIAL");
		else if(State == D3DMCS_COLOR1)   DELog::LogInfo("<li>Diffuse Material Source: D3DMCS_COLOR1");
		else if(State == D3DMCS_COLOR2)   DELog::LogInfo("<li>Diffuse Material Source: D3DMCS_COLOR2");

		D3DDevice->GetRenderState(D3DRS_SPECULARMATERIALSOURCE,&State);
		if(State == D3DMCS_MATERIAL)	  DELog::LogInfo("<li>Specular Material Source: D3DMCS_MATERIAL");
		else if(State == D3DMCS_COLOR1)   DELog::LogInfo("<li>Specular Material Source: D3DMCS_COLOR1");
		else if(State == D3DMCS_COLOR2)   DELog::LogInfo("<li>Specular Material Source: D3DMCS_COLOR2");

		D3DDevice->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE,&State);
		if(State == D3DMCS_MATERIAL)	  DELog::LogInfo("<li>Ambient Material Source: D3DMCS_MATERIAL");
		else if(State == D3DMCS_COLOR1)   DELog::LogInfo("<li>Ambient Material Source: D3DMCS_COLOR1");
		else if(State == D3DMCS_COLOR2)   DELog::LogInfo("<li>Ambient Material Source: D3DMCS_COLOR2");

		D3DDevice->GetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,&State);
		if(State == D3DMCS_MATERIAL)	  DELog::LogInfo("<li>Emissive Material Source: D3DMCS_MATERIAL");
		else if(State == D3DMCS_COLOR1)   DELog::LogInfo("<li>Emissive Material Source: D3DMCS_COLOR1");
		else if(State == D3DMCS_COLOR2)   DELog::LogInfo("<li>Emissive Material Source: D3DMCS_COLOR2");

		D3DDevice->GetRenderState(D3DRS_SPECULARENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Specular Enable: True");
		else DELog::LogInfo("<li>Specular Enable: False");

		D3DDevice->GetRenderState(D3DRS_COLORVERTEX,&State);
		if(State == TRUE) DELog::LogInfo("<li>Color Vertex: True");
		else DELog::LogInfo("<li>Color Vertex: False");
	}

	if (Flags & DE_LOGRENDERSTATES_TEXTURE)
	{
		DELog::LogInfo("<li><b><u>Texture Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR,&State);
		DELog::LogInfo("<li>Texture Factor: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS,&State);
		if(State == TRUE) DELog::LogInfo("<li>Multi Sample Antialias: True");
		else DELog::LogInfo("<li>Multi Sample Antialias: False");

		D3DDevice->GetRenderState(D3DRS_MULTISAMPLEMASK,&State);
		DELog::LogInfo("<li>Multisample Mask: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_DITHERENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Dither Enable: True");
		else DELog::LogInfo("<li>Dither Enable: False");

	}

	if(Flags & DE_LOGRENDERSTATES_TEXTUREWRAP)
	{
		DELog::LogInfo("<li><b><u>Texture Wrap Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_WRAP0,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 0 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 0 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP1,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 1 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 1 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP2,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 2 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 2 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP3,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 3 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 3 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP4,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 4 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 4 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP5,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 5 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 5 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP6,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 6 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 6 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP7,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 7 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 7 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP8,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 8 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 8 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP9,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 9 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 9 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP10,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 10 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 10 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP11,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 11 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 11 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP12,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 12 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 12 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP13,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 13 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 13 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP14,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 14 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 14 Setting: NULL");

		D3DDevice->GetRenderState(D3DRS_WRAP15,&State);
		if(State == D3DWRAPCOORD_0)		 DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAPCOORD_0");
		else if(State == D3DWRAPCOORD_1) DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAPCOORD_1");
		else if(State == D3DWRAPCOORD_2) DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAPCOORD_2");
		else if(State == D3DWRAPCOORD_3) DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAPCOORD_3");
		else if(State == D3DWRAP_U)		 DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAP_U");
		else if(State == D3DWRAP_V)		 DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAP_V");
		else if(State == D3DWRAP_W)		 DELog::LogInfo("<li>Wrap 15 Setting: D3DWRAP_W");
		else if(State == NULL)           DELog::LogInfo("<li>Wrap 15 Setting: NULL");
	}

	if(Flags & DE_LOGRENDERSTATES_ALPHA)
	{
		DELog::LogInfo("<li><b><u>Alpha Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Alpha Blend Enable: True");
		else DELog::LogInfo("<li>Alpha Blend Enable: False");

		D3DDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Alpha Test Enable: True");
		else DELog::LogInfo("<li>Alpha Test Enable: False");

		D3DDevice->GetRenderState(D3DRS_ALPHAFUNC,&State);
		if(State == D3DCMP_NEVER)			DELog::LogInfo("<li>Alpha Function: D3DCMP_NEVER");
		else if (State == D3DCMP_LESS)      DELog::LogInfo("<li>Alpha Function: D3DCMP_LESS");
		else if (State == D3DCMP_EQUAL)     DELog::LogInfo("<li>Alpha Function: D3DCMP_EQUAL");
		else if (State == D3DCMP_LESSEQUAL) DELog::LogInfo("<li>Alpha Function: D3DCMP_LESSEQUAL");
		else if (State == D3DCMP_GREATER)   DELog::LogInfo("<li>Alpha Function: D3DCMP_GREATER");
		else if (State == D3DCMP_NOTEQUAL)  DELog::LogInfo("<li>Alpha Function: D3DCMP_NOTEQUAL");
		else if (State == D3DCMP_ALWAYS)    DELog::LogInfo("<li>Alpha Function: D3DCMP_ALWAYS");

		D3DDevice->GetRenderState(D3DRS_ALPHAREF,&State);
		DELog::LogInfo("<li>Alpha Ref: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_SRCBLEND,&State);
		if(State == D3DBLEND_ZERO)					DELog::LogInfo("<li>Source Blend Function: D3DBLEND_ZERO");
		else if (State == D3DBLEND_ONE)				DELog::LogInfo("<li>Source Blend Function: D3DBLEND_ONE");
		else if (State == D3DBLEND_SRCCOLOR)	    DELog::LogInfo("<li>Source Blend Function: D3DBLEND_SRCCOLOR");
		else if (State == D3DBLEND_INVSRCCOLOR)	    DELog::LogInfo("<li>Source Blend Function: D3DBLEND_INVSRCCOLOR");
		else if (State == D3DBLEND_SRCALPHA)        DELog::LogInfo("<li>Source Blend Function: D3DBLEND_SRCALPHA");
		else if (State == D3DBLEND_INVSRCALPHA)     DELog::LogInfo("<li>Source Blend Function: D3DBLEND_INVSRCALPHA");
		else if (State == D3DBLEND_DESTALPHA)       DELog::LogInfo("<li>Source Blend Function: D3DBLEND_DESTALPHA");
		else if (State == D3DBLEND_INVDESTALPHA)    DELog::LogInfo("<li>Source Blend Function: D3DBLEND_INVDESTALPHA");
		else if (State == D3DBLEND_DESTCOLOR)       DELog::LogInfo("<li>Source Blend Function: D3DBLEND_DESTCOLOR");
		else if (State == D3DBLEND_INVDESTCOLOR)    DELog::LogInfo("<li>Source Blend Function: D3DBLEND_INVDESTCOLOR");
		else if (State == D3DBLEND_SRCALPHASAT)     DELog::LogInfo("<li>Source Blend Function: D3DBLEND_SRCALPHASAT");
		else if (State == D3DBLEND_BOTHSRCALPHA)    DELog::LogInfo("<li>Source Blend Function: D3DBLEND_BOTHSRCALPHA");
		else if (State == D3DBLEND_BOTHINVSRCALPHA) DELog::LogInfo("<li>Source Blend Function: D3DBLEND_BOTHINVSRCALPHA");
		else if (State == D3DBLEND_FORCE_DWORD)     DELog::LogWarning("<li>Source Blend Function: D3DBLEND_FORCE_DWORD");


		D3DDevice->GetRenderState(D3DRS_DESTBLEND,&State);
		if(State == D3DBLEND_ZERO)					DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_ZERO");
		else if (State == D3DBLEND_ONE)				DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_ONE");
		else if (State == D3DBLEND_SRCCOLOR)	    DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_SRCCOLOR");
		else if (State == D3DBLEND_INVSRCCOLOR)	    DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_INVSRCCOLOR");
		else if (State == D3DBLEND_SRCALPHA)        DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_SRCALPHA");
		else if (State == D3DBLEND_INVSRCALPHA)     DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_INVSRCALPHA");
		else if (State == D3DBLEND_DESTALPHA)       DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_DESTALPHA");
		else if (State == D3DBLEND_INVDESTALPHA)    DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_INVDESTALPHA");
		else if (State == D3DBLEND_DESTCOLOR)       DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_DESTCOLOR");
		else if (State == D3DBLEND_INVDESTCOLOR)    DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_INVDESTCOLOR");
		else if (State == D3DBLEND_SRCALPHASAT)     DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_SRCALPHASAT");
		else if (State == D3DBLEND_BOTHSRCALPHA)    DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_BOTHSRCALPHA");
		else if (State == D3DBLEND_BOTHINVSRCALPHA) DELog::LogInfo("<li>Dest Blend Function: D3DBLEND_BOTHINVSRCALPHA");
		else if (State == D3DBLEND_FORCE_DWORD)     DELog::LogWarning("<li>Dest Blend Function: D3DBLEND_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_BLENDFACTOR,&State);
		DELog::LogInfo("<li>Blend Factor: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_BLENDOP,&State);
		if(State == D3DBLENDOP_ADD)					DELog::LogInfo("<li>Blend Operation: D3DBLENDOP_ADD");
		else if(State == D3DBLENDOP_SUBTRACT)		DELog::LogInfo("<li>Blend Operation: D3DBLENDOP_SUBTRACT");
		else if(State == D3DBLENDOP_REVSUBTRACT)	DELog::LogInfo("<li>Blend Operation: D3DBLENDOP_REVSUBTRACT");
		else if(State == D3DBLENDOP_MIN)			DELog::LogInfo("<li>Blend Operation: D3DBLENDOP_MIN");
		else if(State == D3DBLENDOP_MAX)			DELog::LogInfo("<li>Blend Operation: D3DBLENDOP_MAX");
		else if(State == D3DBLENDOP_FORCE_DWORD)	DELog::LogWarning("<li>Blend Operation: D3DBLENDOP_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Separate Alpha Blend Enable: True");
		else DELog::LogInfo("<li>Separate Alpha Blend Enable: False");

		D3DDevice->GetRenderState(D3DRS_SRCBLENDALPHA,&State);
		if(State == D3DBLEND_ZERO)					DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_ZERO");
		else if (State == D3DBLEND_ONE)				DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_ONE");
		else if (State == D3DBLEND_SRCCOLOR)	    DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_SRCCOLOR");
		else if (State == D3DBLEND_INVSRCCOLOR)	    DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_INVSRCCOLOR");
		else if (State == D3DBLEND_SRCALPHA)        DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_SRCALPHA");
		else if (State == D3DBLEND_INVSRCALPHA)     DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_INVSRCALPHA");
		else if (State == D3DBLEND_DESTALPHA)       DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_DESTALPHA");
		else if (State == D3DBLEND_INVDESTALPHA)    DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_INVDESTALPHA");
		else if (State == D3DBLEND_DESTCOLOR)       DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_DESTCOLOR");
		else if (State == D3DBLEND_INVDESTCOLOR)    DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_INVDESTCOLOR");
		else if (State == D3DBLEND_SRCALPHASAT)     DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_SRCALPHASAT");
		else if (State == D3DBLEND_BOTHSRCALPHA)    DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_BOTHSRCALPHA");
		else if (State == D3DBLEND_BOTHINVSRCALPHA) DELog::LogInfo("<li>Source Alpha Blend Function: D3DBLEND_BOTHINVSRCALPHA");
		else if (State == D3DBLEND_FORCE_DWORD)     DELog::LogWarning("<li>Source Alpha Blend Function: D3DBLEND_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_DESTBLENDALPHA,&State);
		if(State == D3DBLEND_ZERO)					DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_ZERO");
		else if (State == D3DBLEND_ONE)				DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_ONE");
		else if (State == D3DBLEND_SRCCOLOR)	    DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_SRCCOLOR");
		else if (State == D3DBLEND_INVSRCCOLOR)	    DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_INVSRCCOLOR");
		else if (State == D3DBLEND_SRCALPHA)        DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_SRCALPHA");
		else if (State == D3DBLEND_INVSRCALPHA)     DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_INVSRCALPHA");
		else if (State == D3DBLEND_DESTALPHA)       DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_DESTALPHA");
		else if (State == D3DBLEND_INVDESTALPHA)    DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_INVDESTALPHA");
		else if (State == D3DBLEND_DESTCOLOR)       DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_DESTCOLOR");
		else if (State == D3DBLEND_INVDESTCOLOR)    DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_INVDESTCOLOR");
		else if (State == D3DBLEND_SRCALPHASAT)     DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_SRCALPHASAT");
		else if (State == D3DBLEND_BOTHSRCALPHA)    DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_BOTHSRCALPHA");
		else if (State == D3DBLEND_BOTHINVSRCALPHA) DELog::LogInfo("<li>Destination Alpha Blend Function: D3DBLEND_BOTHINVSRCALPHA");
		else if (State == D3DBLEND_FORCE_DWORD)     DELog::LogWarning("<li>Destination Alpha Blend Function: D3DBLEND_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_BLENDOPALPHA,&State);
		if(State == D3DBLENDOP_ADD)					DELog::LogInfo("<li>Alpha Blend Operation(BLENDOPALPHA): D3DBLENDOP_ADD");
		else if(State == D3DBLENDOP_SUBTRACT)		DELog::LogInfo("<li>Alpha Blend Operation(BLENDOPALPHA): D3DBLENDOP_SUBTRACT");
		else if(State == D3DBLENDOP_REVSUBTRACT)	DELog::LogInfo("<li>Alpha Blend Operation(BLENDOPALPHA): D3DBLENDOP_REVSUBTRACT");
		else if(State == D3DBLENDOP_MIN)			DELog::LogInfo("<li>Alpha Blend Operation(BLENDOPALPHA): D3DBLENDOP_MIN");
		else if(State == D3DBLENDOP_MAX)			DELog::LogInfo("<li>Alpha Blend Operation(BLENDOPALPHA): D3DBLENDOP_MAX");
		else if(State == D3DBLENDOP_FORCE_DWORD)	DELog::LogWarning("<li>Alpha Blend Operation(BLENDOPALPHA): D3DBLENDOP_FORCE_DWORD");
	}
	if(Flags & DE_LOGRENDERSTATES_FOG)
	{
		DELog::LogInfo("<li><b><u>Fog Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_FOGENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Fog Enable: True");
		else DELog::LogInfo("<li>Fog Enable: False");

		D3DDevice->GetRenderState(D3DRS_RANGEFOGENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Range Fog Enable: True");
		else DELog::LogInfo("<li>Range Fog Enable: False");

		D3DDevice->GetRenderState(D3DRS_FOGCOLOR,&State);
		DELog::LogInfo("<li>Fog Color: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_FOGTABLEMODE,&State);
		if(State == D3DFOG_NONE)	  DELog::LogInfo("<li>Fog Table Mode: D3DFOG_NONE");
		else if(State == D3DFOG_EXP)  DELog::LogInfo("<li>Fog Table Mode: D3DFOG_EXP");
		else if(State == D3DFOG_EXP2) DELog::LogInfo("<li>Fog Table Mode: D3DFOG_EXP2");
		else if(State == D3DFOG_LINEAR) DELog::LogInfo("<li>Fog Table Mode: D3DFOG_LINEAR");

		D3DDevice->GetRenderState(D3DRS_FOGSTART,&State);
		DELog::LogInfo("<li>Fog Start Distance: %f", DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_FOGEND,&State);
		DELog::LogInfo("<li>Fog End Distance: %f", DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_FOGDENSITY,&State);
		DELog::LogInfo("<li>Fog Density: %f", DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_FOGVERTEXMODE,&State);
		if(State == D3DFOG_NONE)	  DELog::LogInfo("<li>Fog Vertex Mode: D3DFOG_NONE");
		else if(State == D3DFOG_EXP)  DELog::LogInfo("<li>Fog Vertex Mode: D3DFOG_EXP");
		else if(State == D3DFOG_EXP2) DELog::LogInfo("<li>Fog Vertex Mode: D3DFOG_EXP2");
		else if(State == D3DFOG_LINEAR) DELog::LogInfo("<li>Fog Vertex Mode: D3DFOG_LINEAR");

	}

	if(Flags & DE_LOGRENDERSTATES_POINT)
	{
		DELog::LogInfo("<li><b><u>Point List Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_POINTSPRITEENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Point Sprite Enable: True");
		else DELog::LogInfo("<li>Point Sprite Enable: False");

		D3DDevice->GetRenderState(D3DRS_POINTSCALEENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Point Sprite Scale Enable: True");
		else DELog::LogInfo("<li>Point Sprite Scale Enable: False");
		
		D3DDevice->GetRenderState(D3DRS_POINTSIZE,&State);
		DELog::LogInfo("<li>Point Size: %f", DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_POINTSIZE_MIN,&State);
		DELog::LogInfo("<li>Point Size Minimum: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_POINTSIZE_MAX,&State);
		DELog::LogInfo("<li>Point Size Maximum: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_POINTSCALE_A,&State);
		DELog::LogInfo("<li>Point Scale A: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_POINTSCALE_B,&State);
		DELog::LogInfo("<li>Point Scale B: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_POINTSCALE_C,&State);
		DELog::LogInfo("<li>Point Scale C: %f",DEUtility::DWtoF(State));
	}

	if(Flags & DE_LOGRENDERSTATES_COLOR)
	{
		DELog::LogInfo("<li><b><u>Color Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE,&State);
		DELog::LogInfo("<li>Color Write Enable: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE1,&State);
		DELog::LogInfo("<li>Color Write Enable 1: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE2,&State);
		DELog::LogInfo("<li>Color Write Enable 2: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE3,&State);
		DELog::LogInfo("<li>Color Write Enable 3: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_SRGBWRITEENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>sRGB Write Enable: True");
		else DELog::LogInfo("<li>sRGB Write Enable: False");
	}

	if(Flags & DE_LOGRENDERSTATES_CLIP)
	{
		DELog::LogInfo("<li><u><b>Clipping Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_CLIPPING,&State);
		if(State == TRUE) DELog::LogInfo("<li>Clipping: True");
		else DELog::LogInfo("<li>Clipping: False");

		D3DDevice->GetRenderState(D3DRS_CLIPPLANEENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Clip Plane Enable: True");
		else DELog::LogInfo("<li>Clip Plane Enable: False");

		D3DDevice->GetRenderState(D3DRS_CULLMODE,&State);
		if(State == D3DCULL_NONE)	   DELog::LogInfo("<li>Cull Mode: D3DCULL_NONE");
		else if (State == D3DCULL_CW)  DELog::LogInfo("<li>Cull Mode: D3DCULL_CW");
		else if (State == D3DCULL_CCW) DELog::LogInfo("<li>Cull Mode: D3DCULL_CCW");

		D3DDevice->GetRenderState(D3DRS_SCISSORTESTENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Scissor Test Enable: True");
		else DELog::LogInfo("<li>Scissor Test Enable: False");
	}


	if(Flags & DE_LOGRENDERSTATES_STENCIL)
	{
		DELog::LogInfo("<li><u><b>Stencil Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_STENCILENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Stencil Enable: True");
		else DELog::LogInfo("<li>Stencil Enable: False");

		D3DDevice->GetRenderState(D3DRS_STENCILFAIL,&State);
		if(State == D3DSTENCILOP_KEEP)			DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_KEEP");
		else if (State == D3DSTENCILOP_ZERO)	DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_ZERO");
		else if (State == D3DSTENCILOP_REPLACE) DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_REPLACE");
		else if (State == D3DSTENCILOP_INCRSAT) DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_INCRSAT");
		else if (State == D3DSTENCILOP_DECRSAT) DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_DECRSAT");
		else if (State == D3DSTENCILOP_INVERT)  DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_INVERT");
		else if (State == D3DSTENCILOP_INCR)    DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_INCR");
		else if (State == D3DSTENCILOP_DECR)    DELog::LogInfo("<li>Stencil Fail OP: D3DSTENCILOP_DECR");
		else if (State == D3DSTENCILOP_FORCE_DWORD)    DELog::LogWarning("<li>Stencil Fail OP: D3DSTENCILOP_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_STENCILZFAIL,&State);
		if(State == D3DSTENCILOP_KEEP)			DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_KEEP");
		else if (State == D3DSTENCILOP_ZERO)	DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_ZERO");
		else if (State == D3DSTENCILOP_REPLACE) DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_REPLACE");
		else if (State == D3DSTENCILOP_INCRSAT) DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_INCRSAT");
		else if (State == D3DSTENCILOP_DECRSAT) DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_DECRSAT");
		else if (State == D3DSTENCILOP_INVERT)  DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_INVERT");
		else if (State == D3DSTENCILOP_INCR)    DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_INCR");
		else if (State == D3DSTENCILOP_DECR)    DELog::LogInfo("<li>Stencil Z Fail OP: D3DSTENCILOP_DECR");
		else if (State == D3DSTENCILOP_FORCE_DWORD)    DELog::LogWarning("<li>Stencil Z Fail OP: D3DSTENCILOP_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_STENCILPASS,&State);
		if(State == D3DSTENCILOP_KEEP)			DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_KEEP");
		else if (State == D3DSTENCILOP_ZERO)	DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_ZERO");
		else if (State == D3DSTENCILOP_REPLACE) DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_REPLACE");
		else if (State == D3DSTENCILOP_INCRSAT) DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_INCRSAT");
		else if (State == D3DSTENCILOP_DECRSAT) DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_DECRSAT");
		else if (State == D3DSTENCILOP_INVERT)  DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_INVERT");
		else if (State == D3DSTENCILOP_INCR)    DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_INCR");
		else if (State == D3DSTENCILOP_DECR)    DELog::LogInfo("<li>Stencil Pass OP: D3DSTENCILOP_DECR");
		else if (State == D3DSTENCILOP_FORCE_DWORD)    DELog::LogWarning("<li>Stencil Pass OP: D3DSTENCILOP_FORCE_DWORD");


		D3DDevice->GetRenderState(D3DRS_STENCILFUNC,&State);
		if(State == D3DCMP_NEVER)			DELog::LogInfo("<li>Stencil Function: D3DCMP_NEVER");
		else if (State == D3DCMP_LESS)      DELog::LogInfo("<li>Stencil Function: D3DCMP_LESS");
		else if (State == D3DCMP_EQUAL)     DELog::LogInfo("<li>Stencil Function: D3DCMP_EQUAL");
		else if (State == D3DCMP_LESSEQUAL) DELog::LogInfo("<li>Stencil Function: D3DCMP_LESSEQUAL");
		else if (State == D3DCMP_GREATER)   DELog::LogInfo("<li>Stencil Function: D3DCMP_GREATER");
		else if (State == D3DCMP_NOTEQUAL)  DELog::LogInfo("<li>Stencil Function: D3DCMP_NOTEQUAL");
		else if (State == D3DCMP_ALWAYS)    DELog::LogInfo("<li>Stencil Function: D3DCMP_ALWAYS");

		D3DDevice->GetRenderState(D3DRS_STENCILREF,&State);
		DELog::LogInfo("<li>Stencil Ref: %d", State);

		D3DDevice->GetRenderState(D3DRS_STENCILMASK,&State);
		DELog::LogInfo("<li>Stencil Mask: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_STENCILWRITEMASK,&State);
		DELog::LogInfo("<li>Stencil Write Mask: 0x%8.8x", State);

		D3DDevice->GetRenderState(D3DRS_TWOSIDEDSTENCILMODE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Two Sided Stencil Mode: True");
		else DELog::LogInfo("<li>Two Sided Stencil Mode: False");

		D3DDevice->GetRenderState(D3DRS_CCW_STENCILFAIL,&State);
		if(State == D3DSTENCILOP_KEEP)			DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_KEEP");
		else if (State == D3DSTENCILOP_ZERO)	DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_ZERO");
		else if (State == D3DSTENCILOP_REPLACE) DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_REPLACE");
		else if (State == D3DSTENCILOP_INCRSAT) DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_INCRSAT");
		else if (State == D3DSTENCILOP_DECRSAT) DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_DECRSAT");
		else if (State == D3DSTENCILOP_INVERT)  DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_INVERT");
		else if (State == D3DSTENCILOP_INCR)    DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_INCR");
		else if (State == D3DSTENCILOP_DECR)    DELog::LogInfo("<li>Stencil Fail CCW OP: D3DSTENCILOP_DECR");
		else if (State == D3DSTENCILOP_FORCE_DWORD)    DELog::LogWarning("<li>Stencil Fail CCW OP: D3DSTENCILOP_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_CCW_STENCILZFAIL,&State);
		if(State == D3DSTENCILOP_KEEP)			DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_KEEP");
		else if (State == D3DSTENCILOP_ZERO)	DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_ZERO");
		else if (State == D3DSTENCILOP_REPLACE) DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_REPLACE");
		else if (State == D3DSTENCILOP_INCRSAT) DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_INCRSAT");
		else if (State == D3DSTENCILOP_DECRSAT) DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_DECRSAT");
		else if (State == D3DSTENCILOP_INVERT)  DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_INVERT");
		else if (State == D3DSTENCILOP_INCR)    DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_INCR");
		else if (State == D3DSTENCILOP_DECR)    DELog::LogInfo("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_DECR");
		else if (State == D3DSTENCILOP_FORCE_DWORD)    DELog::LogWarning("<li>Stencil Z Fail CCW OP: D3DSTENCILOP_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_CCW_STENCILPASS,&State);
		if(State == D3DSTENCILOP_KEEP)			DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_KEEP");
		else if (State == D3DSTENCILOP_ZERO)	DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_ZERO");
		else if (State == D3DSTENCILOP_REPLACE) DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_REPLACE");
		else if (State == D3DSTENCILOP_INCRSAT) DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_INCRSAT");
		else if (State == D3DSTENCILOP_DECRSAT) DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_DECRSAT");
		else if (State == D3DSTENCILOP_INVERT)  DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_INVERT");
		else if (State == D3DSTENCILOP_INCR)    DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_INCR");
		else if (State == D3DSTENCILOP_DECR)    DELog::LogInfo("<li>Stencil Pass CCW OP: D3DSTENCILOP_DECR");
		else if (State == D3DSTENCILOP_FORCE_DWORD)    DELog::LogWarning("<li>Stencil Pass CCW OP: D3DSTENCILOP_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_CCW_STENCILFUNC,&State);
		if(State == D3DCMP_NEVER)			DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_NEVER");
		else if (State == D3DCMP_LESS)      DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_LESS");
		else if (State == D3DCMP_EQUAL)     DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_EQUAL");
		else if (State == D3DCMP_LESSEQUAL) DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_LESSEQUAL");
		else if (State == D3DCMP_GREATER)   DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_GREATER");
		else if (State == D3DCMP_NOTEQUAL)  DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_NOTEQUAL");
		else if (State == D3DCMP_ALWAYS)    DELog::LogInfo("<li>Stencil CCW Function: D3DCMP_ALWAYS");
	}

	if(Flags & DE_LOGRENDERSTATES_ZCALC)
	{
		DELog::LogInfo("<li><u><b>Calculation Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_ZWRITEENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Z Write Enable: True");
		else DELog::LogInfo("<li>Z Write Enable: False");

		D3DDevice->GetRenderState(D3DRS_ZENABLE,&State);	
		if(State == D3DZB_TRUE)				DELog::LogInfo("<li>Z Enable: D3DZB_TRUE");
		else if (State == D3DZB_FALSE)		DELog::LogInfo("<li>Z Enable: D3DZB_FALSE");
		else if (State == D3DZB_USEW)		DELog::LogInfo("<li>Z Enable: D3DZB_USEW");

		D3DDevice->GetRenderState(D3DRS_ZFUNC,&State);
		if(State == D3DCMP_NEVER)			DELog::LogInfo("<li>Z Function: D3DCMP_NEVER");
		else if (State == D3DCMP_LESS)      DELog::LogInfo("<li>Z Function: D3DCMP_LESS");
		else if (State == D3DCMP_EQUAL)     DELog::LogInfo("<li>Z Function: D3DCMP_EQUAL");
		else if (State == D3DCMP_LESSEQUAL) DELog::LogInfo("<li>Z Function: D3DCMP_LESSEQUAL");
		else if (State == D3DCMP_GREATER)   DELog::LogInfo("<li>Z Function: D3DCMP_GREATER");
		else if (State == D3DCMP_NOTEQUAL)  DELog::LogInfo("<li>Z Function: D3DCMP_NOTEQUAL");
		else if (State == D3DCMP_ALWAYS)    DELog::LogInfo("<li>Z Function: D3DCMP_ALWAYS");

		D3DDevice->GetRenderState(D3DRS_DEPTHBIAS,&State);
		DELog::LogInfo("<li>Depth Bias: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,&State);
		DELog::LogInfo("<li>Slope Scale Depth Bias: %f",DEUtility::DWtoF(State));

	}

	if(Flags & DE_LOGRENDERSTATES_TESSELLATION)
	{
		DELog::LogInfo("<li><u><b>Tessellation Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION,&State);
		if(State == TRUE) DELog::LogInfo("<li>Enable Adaptive Tesselation: True");
		else DELog::LogInfo("<li>Enable Adaptive Tesselation: False");

		D3DDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL,&State);
		DELog::LogInfo("<li>Minimum Tesselation Level: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL,&State);
		DELog::LogInfo("<li>Maximum Tesselation Level: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_ADAPTIVETESS_X,&State);
		DELog::LogInfo("<li>AdaptiveTessX: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_ADAPTIVETESS_Y,&State);
		DELog::LogInfo("<li>AdaptiveTessY: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_ADAPTIVETESS_Z,&State);
		DELog::LogInfo("<li>AdaptiveTessZ: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_ADAPTIVETESS_W,&State);
		DELog::LogInfo("<li>AdaptiveTessW: %f",DEUtility::DWtoF(State));
	}


	if(Flags & DE_LOGRENDERSTATES_VERTEX)
	{
		DELog::LogInfo("<li><b><u>Vertex Settings</u></b>");
		D3DDevice->GetRenderState(D3DRS_VERTEXBLEND,&State);
		if(State == D3DVBF_DISABLE)	  DELog::LogInfo("<li>Vertex Blend: D3DVBF_DISABLE");
		else if(State == D3DVBF_1WEIGHTS)   DELog::LogInfo("<li>Vertex Blend: D3DVBF_1WEIGHTS");
		else if(State == D3DVBF_2WEIGHTS)   DELog::LogInfo("<li>Vertex Blend: D3DVBF_2WEIGHTS");
		else if(State == D3DVBF_3WEIGHTS)   DELog::LogInfo("<li>Vertex Blend: D3DVBF_3WEIGHTS");
		else if(State == D3DVBF_0WEIGHTS)   DELog::LogInfo("<li>Vertex Blend: D3DVBF_0WEIGHTS");
		else if(State == D3DVBF_TWEENING)   DELog::LogInfo("<li>Vertex Blend: D3DVBF_TWEENING");

		D3DDevice->GetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>Indexed Vertex Blend Enable: True");
		else DELog::LogInfo("<li>Indexed Vertex Blend Enable: False");

		D3DDevice->GetRenderState(D3DRS_TWEENFACTOR,&State);
		DELog::LogInfo("<li>Tween Factor: %f",DEUtility::DWtoF(State));

		D3DDevice->GetRenderState(D3DRS_FILLMODE,&State);
		if(State == D3DFILL_POINT)             DELog::LogInfo("<li>FillMode: D3DFILL_POINT");
		else if (State == D3DFILL_WIREFRAME)   DELog::LogInfo("<li>FillMode: D3DFILL_WIREFRAME");
		else if (State == D3DFILL_SOLID)       DELog::LogInfo("<li>FillMode: D3DFILL_SOLID");
		else if (State == D3DFILL_FORCE_DWORD) DELog::LogWarning("<li>FillMode: D3DFILL_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_SHADEMODE,&State);
		if(State == D3DSHADE_FLAT)				DELog::LogInfo("<li>Shademode: D3DSHADE_FLAT");
		else if (State == D3DSHADE_GOURAUD)     DELog::LogInfo("<li>Shademode: D3DSHADE_GOURAUD");
		else if (State == D3DSHADE_PHONG)       DELog::LogInfo("<li>Shademode: D3DSHADE_PHONG");
		else if (State == D3DSHADE_FORCE_DWORD) DELog::LogWarning("<li>Shademode: D3DSHADE_FORCE_DWORD");
	}



	if(Flags & DE_LOGRENDERSTATES_MISC)
	{
		DELog::LogInfo("<li><b><u>Miscellaneous Settings</u></b>");
    	D3DDevice->GetRenderState(D3DRS_LASTPIXEL,&State);
	    if(State == TRUE) DELog::LogInfo("<li>Last Pixel Writing: True");
		 else DELog::LogInfo("<li>Last Pixel Writing: False");

		D3DDevice->GetRenderState(D3DRS_LOCALVIEWER,&State);
		if(State == TRUE) DELog::LogInfo("<li>Local Viewer: True");
		 else DELog::LogInfo("<li>Local Viewer: False");

		D3DDevice->GetRenderState(D3DRS_PATCHEDGESTYLE,&State);
		if(State == D3DPATCHEDGE_DISCRETE)			DELog::LogInfo("<li>Patch Edge Style: D3DPATCHEDGE_DISCRETE");
		else if(State == D3DPATCHEDGE_CONTINUOUS)   DELog::LogInfo("<li>Patch Edge Style: D3DPATCHEDGE_CONTINUOUS");
		else if(State == D3DPATCHEDGE_FORCE_DWORD)  DELog::LogWarning("<li>Patch Edge Style: D3DPATCHEDGE_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_DEBUGMONITORTOKEN,&State);
		if(State == D3DDMT_ENABLE)					DELog::LogInfo("<li>Debug Monitor Token: D3DDMT_ENABLE");
		else if(State == D3DDMT_DISABLE)			DELog::LogInfo("<li>Debug Monitor Token: D3DDMT_DISABLE");
		else if(State == D3DDMT_FORCE_DWORD)		DELog::LogWarning("<li>Debug Monitor Token: D3DDMT_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_POSITIONDEGREE,&State);
		if(State == D3DDEGREE_LINEAR)				DELog::LogInfo("<li>Position Degree Type: D3DDEGREE_LINEAR");
		else if(State == D3DDEGREE_QUADRATIC)		DELog::LogInfo("<li>Position Degree Type: D3DDEGREE_QUADRATIC");
		else if(State == D3DDEGREE_CUBIC)			DELog::LogInfo("<li>Position Degree Type: D3DDEGREE_CUBIC");
		else if(State == D3DDEGREE_QUINTIC)			DELog::LogInfo("<li>Position Degree Type: D3DDEGREE_QUINTIC");
		else if(State == D3DCULL_FORCE_DWORD)	    DELog::LogWarning("<li>Position Degree Type: D3DCULL_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_NORMALDEGREE,&State);
		if(State == D3DDEGREE_LINEAR)				DELog::LogInfo("<li>Normal Degree Type: D3DDEGREE_LINEAR");
		else if(State == D3DDEGREE_QUADRATIC)		DELog::LogInfo("<li>Normal Degree Type: D3DDEGREE_QUADRATIC");
		else if(State == D3DDEGREE_CUBIC)			DELog::LogInfo("<li>Normal Degree Type: D3DDEGREE_CUBIC");
		else if(State == D3DDEGREE_QUINTIC)			DELog::LogInfo("<li>Normal Degree Type: D3DDEGREE_QUINTIC");
		else if(State == D3DCULL_FORCE_DWORD)	    DELog::LogWarning("<li>Normal Degree Type: D3DCULL_FORCE_DWORD");

		D3DDevice->GetRenderState(D3DRS_ANTIALIASEDLINEENABLE,&State);
		if(State == TRUE) DELog::LogInfo("<li>AntiAliased Line Enable: True");
		else DELog::LogInfo("<li>AntiAliased Line Enable: False");
	}
}

void DECore::LogTextureStageState(int Stage)
{
	DWORD State;
	if(D3DDevice == NULL) 
	{
		return;
	}
	if (Stage>7 || Stage <0) 
	{
		DELog::LogWarning("<br><b>Texture Stage %d States Report:</b> Invalid Stage", Stage);
	}
	else
	{
		DELog::LogInfo("<br><b>Texture Stage %d States Report:</b>", Stage);
	}

	D3DDevice->GetTextureStageState(Stage,D3DTSS_COLOROP,&State);
	if(State == D3DTOP_DISABLE) DELog::LogInfo("<li>Color Op: D3DTOP_DISABLE");
	else if (State == D3DTOP_SELECTARG1)				DELog::LogInfo("<li>Color Op: D3DTOP_SELECTARG1");
	else if (State == D3DTOP_SELECTARG2)				DELog::LogInfo("<li>Color Op: D3DTOP_SELECTARG2");
	else if (State == D3DTOP_MODULATE)					DELog::LogInfo("<li>Color Op: D3DTOP_MODULATE");
	else if (State == D3DTOP_MODULATE2X)				DELog::LogInfo("<li>Color Op: D3DTOP_MODULATE2X");
	else if (State == D3DTOP_MODULATE4X)				DELog::LogInfo("<li>Color Op: D3DTOP_MODULATE4X");
	else if (State == D3DTOP_ADD)						DELog::LogInfo("<li>Color Op: D3DTOP_ADD");
	else if (State == D3DTOP_ADDSIGNED)					DELog::LogInfo("<li>Color Op: D3DTOP_ADDSIGNED");
	else if (State == D3DTOP_ADDSIGNED2X)				DELog::LogInfo("<li>Color Op: D3DTOP_ADDSIGNED2X");
	else if (State == D3DTOP_ADDSMOOTH)					DELog::LogInfo("<li>Color Op: D3DTOP_ADDSMOOTH");
	else if (State == D3DTOP_BLENDDIFFUSEALPHA)			DELog::LogInfo("<li>Color Op: D3DTOP_BLENDDIFFUSEALPHA");
	else if (State == D3DTOP_BLENDTEXTUREALPHA)			DELog::LogInfo("<li>Color Op: D3DTOP_BLENDTEXTUREALPHA");
	else if (State == D3DTOP_BLENDTEXTUREALPHAPM)		DELog::LogInfo("<li>Color Op: D3DTOP_BLENDTEXTUREALPHAPM");
	else if (State == D3DTOP_BLENDCURRENTALPHA)			DELog::LogInfo("<li>Color Op: D3DTOP_BLENDCURRENTALPHA");
	else if (State == D3DTOP_PREMODULATE)				DELog::LogInfo("<li>Color Op: D3DTOP_PREMODULATE");
	else if (State == D3DTOP_MODULATEALPHA_ADDCOLOR)	DELog::LogInfo("<li>Color Op: D3DTOP_MODULATEALPHA_ADDCOLOR");
	else if (State == D3DTOP_MODULATECOLOR_ADDALPHA)	DELog::LogInfo("<li>Color Op: D3DTOP_MODULATECOLOR_ADDALPHA");
	else if (State == D3DTOP_MODULATEINVALPHA_ADDCOLOR) DELog::LogInfo("<li>Color Op: D3DTOP_MODULATEINVALPHA_ADDCOLOR");
	else if (State == D3DTOP_MODULATEINVCOLOR_ADDALPHA) DELog::LogInfo("<li>Color Op: D3DTOP_MODULATEINVCOLOR_ADDALPHA");
	else if (State == D3DTOP_BUMPENVMAP)				DELog::LogInfo("<li>Color Op: D3DTOP_BUMPENVMAP");
	else if (State == D3DTOP_BUMPENVMAPLUMINANCE)		DELog::LogInfo("<li>Color Op: D3DTOP_BUMPENVMAPLUMINANCE");
	else if (State == D3DTOP_DOTPRODUCT3)				DELog::LogInfo("<li>Color Op: D3DTOP_DOTPRODUCT3");
	else if (State == D3DTOP_MULTIPLYADD)				DELog::LogInfo("<li>Color Op: D3DTOP_MULTIPLYADD");
	else if (State == D3DTOP_LERP)						DELog::LogInfo("<li>Color Op: D3DTOP_LERP");
	else if (State == D3DTOP_FORCE_DWORD)				DELog::LogWarning("<li>Color Op: D3DTOP_FORCE_DWORD");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_COLORARG1,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Color Argument 1: D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Color Argument 1: D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Color Argument 1: D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Color Argument 1: D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Color Argument 1: D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Color Argument 1: D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_COLORARG1,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Color Argument 1: | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Color Argument 1: | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_COLORARG2,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Color Argument 2: D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Color Argument 2: D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Color Argument 2: D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Color Argument 2: D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Color Argument 2: D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Color Argument 2: D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_COLORARG2,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Color Argument 2: | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Color Argument 2: | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAOP,&State);
	if(State == D3DTOP_DISABLE) DELog::LogInfo("<li>Alpha Op: D3DTOP_DISABLE");
	else if (State == D3DTOP_SELECTARG1)				DELog::LogInfo("<li>Alpha Op: D3DTOP_SELECTARG1");
	else if (State == D3DTOP_SELECTARG2)				DELog::LogInfo("<li>Alpha Op: D3DTOP_SELECTARG2");
	else if (State == D3DTOP_MODULATE)					DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATE");
	else if (State == D3DTOP_MODULATE2X)				DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATE2X");
	else if (State == D3DTOP_MODULATE4X)				DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATE4X");
	else if (State == D3DTOP_ADD)						DELog::LogInfo("<li>Alpha Op: D3DTOP_ADD");
	else if (State == D3DTOP_ADDSIGNED)					DELog::LogInfo("<li>Alpha Op: D3DTOP_ADDSIGNED");
	else if (State == D3DTOP_ADDSIGNED2X)				DELog::LogInfo("<li>Alpha Op: D3DTOP_ADDSIGNED2X");
	else if (State == D3DTOP_ADDSMOOTH)					DELog::LogInfo("<li>Alpha Op: D3DTOP_ADDSMOOTH");
	else if (State == D3DTOP_BLENDDIFFUSEALPHA)			DELog::LogInfo("<li>Alpha Op: D3DTOP_BLENDDIFFUSEALPHA");
	else if (State == D3DTOP_BLENDTEXTUREALPHA)			DELog::LogInfo("<li>Alpha Op: D3DTOP_BLENDTEXTUREALPHA");
	else if (State == D3DTOP_BLENDTEXTUREALPHAPM)		DELog::LogInfo("<li>Alpha Op: D3DTOP_BLENDTEXTUREALPHAPM");
	else if (State == D3DTOP_BLENDCURRENTALPHA)			DELog::LogInfo("<li>Alpha Op: D3DTOP_BLENDCURRENTALPHA");
	else if (State == D3DTOP_PREMODULATE)				DELog::LogInfo("<li>Alpha Op: D3DTOP_PREMODULATE");
	else if (State == D3DTOP_MODULATEALPHA_ADDCOLOR)	DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATEALPHA_ADDCOLOR");
	else if (State == D3DTOP_MODULATECOLOR_ADDALPHA)	DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATECOLOR_ADDALPHA");
	else if (State == D3DTOP_MODULATEINVALPHA_ADDCOLOR) DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATEINVALPHA_ADDCOLOR");
	else if (State == D3DTOP_MODULATEINVCOLOR_ADDALPHA) DELog::LogInfo("<li>Alpha Op: D3DTOP_MODULATEINVCOLOR_ADDALPHA");
	else if (State == D3DTOP_BUMPENVMAP)				DELog::LogInfo("<li>Alpha Op: D3DTOP_BUMPENVMAP");
	else if (State == D3DTOP_BUMPENVMAPLUMINANCE)		DELog::LogInfo("<li>Alpha Op: D3DTOP_BUMPENVMAPLUMINANCE");
	else if (State == D3DTOP_DOTPRODUCT3)				DELog::LogInfo("<li>Alpha Op: D3DTOP_DOTPRODUCT3");
	else if (State == D3DTOP_MULTIPLYADD)				DELog::LogInfo("<li>Alpha Op: D3DTOP_MULTIPLYADD");
	else if (State == D3DTOP_LERP)						DELog::LogInfo("<li>Alpha Op: D3DTOP_LERP");
	else if (State == D3DTOP_FORCE_DWORD)				DELog::LogWarning("<li>Alpha Op: D3DTOP_FORCE_DWORD");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG1,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Alpha Argument 1: D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Alpha Argument 1: D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Alpha Argument 1: D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Alpha Argument 1: D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Alpha Argument 1: D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Alpha Argument 1: D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG1,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Alpha Argument 1: | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Alpha Argument 1: | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG2,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Alpha Argument 2: D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Alpha Argument 2: D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Alpha Argument 2: D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Alpha Argument 2: D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Alpha Argument 2: D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Alpha Argument 2: D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG2,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Alpha Argument 2: | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Alpha Argument 2: | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_BUMPENVMAT00,&State);
	DELog::LogInfo("<li>Bump Map Matrix [0][0]: %f",DEUtility::DWtoF(State));

	D3DDevice->GetTextureStageState(Stage,D3DTSS_BUMPENVMAT01,&State);
	DELog::LogInfo("<li>Bump Map Matrix [0][1]: %f",DEUtility::DWtoF(State));

	D3DDevice->GetTextureStageState(Stage,D3DTSS_BUMPENVMAT10,&State);
	DELog::LogInfo("<li>Bump Map Matrix [1][0]: %f",DEUtility::DWtoF(State));

	D3DDevice->GetTextureStageState(Stage,D3DTSS_BUMPENVMAT11,&State);
	DELog::LogInfo("<li>Bump Map Matrix [1][1]: %f",DEUtility::DWtoF(State));

	D3DDevice->GetTextureStageState(Stage,D3DTSS_TEXCOORDINDEX,&State);
	State = State & 0x0000000F;
	DELog::LogInfo("<li>Texture Coordinate Set Index: %d", DEUtility::DWtoF(State));
	D3DDevice->GetTextureStageState(Stage,D3DTSS_TEXCOORDINDEX,&State);
	if(State & 	D3DTSS_TCI_CAMERASPACENORMAL)					DELog::LogInfo("<li>| D3DTSS_TCI_CAMERASPACENORMAL");
	else if (State & D3DTSS_TCI_CAMERASPACEPOSITION)			DELog::LogInfo("<li>| D3DTSS_TCI_CAMERASPACEPOSITION");
	else if (State & D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR)	DELog::LogInfo("<li>| D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR");
	else if (State & D3DTSS_TCI_SPHEREMAP)						DELog::LogInfo("<li>| D3DTSS_TCI_SPHEREMAP");
	else DELog::LogInfo("<li>| D3DTSS_TCI_PASSTHRU");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_BUMPENVLSCALE,&State);
	DELog::LogInfo("<li>Bump Map Luminance Scale (D3DTSS_BUMPENVLSCALE): %f",DEUtility::DWtoF(State));

	D3DDevice->GetTextureStageState(Stage,D3DTSS_COLORARG0,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG2,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Color Argument 3 (D3DTSS_COLORARG0): | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG0,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG2,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Alpha Argument 3 (D3DTSS_ALPHAARG0): | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_RESULTARG,&State);
	State = State & 0x0000000F;
	if(State == D3DTA_CURRENT)			DELog::LogInfo("<li>Result Argument: D3DTA_CURRENT ");
	else if (State == D3DTA_DIFFUSE)	DELog::LogInfo("<li>Result Argument: D3DTA_DIFFUSE ");
	else if (State == D3DTA_SPECULAR)	DELog::LogInfo("<li>Result Argument: D3DTA_SPECULAR ");
	else if (State == D3DTA_TEMP)		DELog::LogInfo("<li>Result Argument: D3DTA_TEMP ");
	else if (State == D3DTA_TEXTURE)    DELog::LogInfo("<li>Result Argument: D3DTA_TEXTURE ");
	else if (State == D3DTA_TFACTOR)    DELog::LogInfo("<li>Result Argument: D3DTA_TFACTOR ");
	D3DDevice->GetTextureStageState(Stage,D3DTSS_ALPHAARG2,&State);
	if(State & D3DTA_ALPHAREPLICATE) DELog::LogInfo("<li>Result Argument: | D3DTA_ALPHAREPLICATE ");
	if(State & D3DTA_COMPLEMENT)     DELog::LogInfo("<li>Result Argument: | D3DTA_COMPLEMENT");

	D3DDevice->GetTextureStageState(Stage,D3DTSS_CONSTANT,&State);
	DELog::LogInfo("<li>Per Stage Constant Color: 0x%8.8x", State);

}



/*******************************
/*    CREAZIONE LISTE VIDEO    *
/*******************************/
DLL_API void DECore::BuildBackBufferList( STRUCT_DE_ADAPTERFORMAT* pFormat )
{
	pFormat->BackBuffersList.clear();

	// A list of valid back buffer formats
	const D3DFORMAT bbArray[] = 
	{ 
        D3DFMT_A2R10G10B10,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_A1R5G5B5,
		D3DFMT_X1R5G5B5,
		D3DFMT_R5G6B5, 

    };
    const UINT uiNumElements = sizeof(bbArray) / sizeof(bbArray[0]);

	// Loop through the back buffers and see if they can be used with
	// this adapter format.
	for( UINT ui = 0; ui < uiNumElements; ui++ )
	{
		D3DFORMAT fmt = bbArray[ui];
		// This function chacks if the back buffer is valid
		if( SUCCEEDED( D3DObject->CheckDeviceType( pFormat->AdapterID,
												D3DDEVTYPE_HAL,
												pFormat->Format,
												fmt,
												pFormat->Windowed ) ) )
		{
			// If its valid, then make sure it isnt already in the vector
			// then add it in.
			if( std::find( pFormat->BackBuffersList.begin(), pFormat->BackBuffersList.end(), fmt ) 
				== pFormat->BackBuffersList.end() ) 
			{
				pFormat->BackBuffersList.push_back( fmt );
			}
		}
	}
}

DLL_API void DECore::DetermineAdapterBehaviour( STRUCT_DE_ADAPTERFORMAT* pFormat, D3DCAPS9* pCaps, bool supportMixedVP)
{
	// With all these checks we first check to see if the check also passes
	// the feature function test. We pass in the adapter format, the vertex
	// processing type, they caps structure, and the d3d interface. The
	// feature validation function does it's own stuff and returns a true
	// or false depending ont he user. If it passes then we're good, otherwise just
	// dont add it to the vector. pretty simple...

	// Check if hardware transform and lighting is supported. If it is
	// then further check stuff inside.
	if( pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
    {
		// Check if this adapter canbe used as a pure device. 
        if( pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE )
        {
			if ( DeviceFeatureFunction == NULL || 
				 DeviceFeatureFunction( pCaps, pFormat->Format, DE_VERTEXPROCESSINGTYPE_PUREHW, D3DObject ) )
            {
				if( std::find( pFormat->VertexProcessingList.begin(), pFormat->VertexProcessingList.end(), DE_VERTEXPROCESSINGTYPE_PUREHW ) 
					== pFormat->VertexProcessingList.end() )
				{
					pFormat->VertexProcessingList.push_back( DE_VERTEXPROCESSINGTYPE_PUREHW );
				}
            }
        }

        if( DeviceFeatureFunction == NULL || 
			DeviceFeatureFunction( pCaps, pFormat->Format, DE_VERTEXPROCESSINGTYPE_HW, D3DObject ) )
        {
			if( std::find( pFormat->VertexProcessingList.begin(), pFormat->VertexProcessingList.end(), DE_VERTEXPROCESSINGTYPE_HW ) 
				== pFormat->VertexProcessingList.end() )
				pFormat->VertexProcessingList.push_back( DE_VERTEXPROCESSINGTYPE_HW );
        }

		// Check if this application requires mixed vertex processing.
        if( supportMixedVP == true && ( 
			DeviceFeatureFunction == NULL || 
			DeviceFeatureFunction( pCaps, pFormat->Format, DE_VERTEXPROCESSINGTYPE_MIXED, D3DObject ) ) )
        {
			if( std::find( pFormat->VertexProcessingList.begin(), pFormat->VertexProcessingList.end(), DE_VERTEXPROCESSINGTYPE_MIXED ) 
				== pFormat->VertexProcessingList.end() )
				pFormat->VertexProcessingList.push_back( DE_VERTEXPROCESSINGTYPE_MIXED );
        }
    }
	
	// If nothing else then we'll have to use software vertex processing. But
	// if the function fails here, then we'll need a REF device.
	if( DeviceFeatureFunction == NULL || 
		DeviceFeatureFunction( pCaps, pFormat->Format, DE_VERTEXPROCESSINGTYPE_SW, D3DObject ) )
    {
		if( std::find( pFormat->VertexProcessingList.begin(), pFormat->VertexProcessingList.end(), DE_VERTEXPROCESSINGTYPE_SW ) 
				== pFormat->VertexProcessingList.end() )
			pFormat->VertexProcessingList.push_back( DE_VERTEXPROCESSINGTYPE_SW );
    }
}

DLL_API void DECore::BuildPresentationList( STRUCT_DE_ADAPTER* pAdapter )
{
	pAdapter->PresentIntervalsList.clear();

	//D3DPRESENT_INTERVAL_DEFAULT esiste sempre
	pAdapter->PresentIntervalsList.push_back( D3DPRESENT_INTERVAL_DEFAULT );

	//Lista dei presentation intervals
	const UINT piArray[] = 
	{ 
        D3DPRESENT_INTERVAL_IMMEDIATE,
        D3DPRESENT_INTERVAL_ONE,
        D3DPRESENT_INTERVAL_TWO,
        D3DPRESENT_INTERVAL_THREE,
        D3DPRESENT_INTERVAL_FOUR,
    };
    const UINT uiNumElements = sizeof(piArray) / sizeof(piArray[0]);

	//Aggiunge alla lista i PI se possibili
    for( UINT i = 0; i < uiNumElements; i++ )
    {
		if( pAdapter->Caps.PresentationIntervals & piArray[i] )
        {
			pAdapter->PresentIntervalsList.push_back( piArray[i] );
        }
    }
	//NB:In finestra i presentationintervals 2, 3, e 4 non sono consentiti
}


// Find function to find conflicts
bool FindConflict( const STRUCT_DE_MSFCONFLICT& l, const STRUCT_DE_MSFCONFLICT& r )
{
	return ( l.Format == r.Format && l.Type == r.Type );
}
DLL_API void DECore::BuildConflictList( STRUCT_DE_ADAPTERFORMAT* pFormat )
{
	// This will store the found conflict
	STRUCT_DE_MSFCONFLICT conflict;
	pFormat->ConflictsList.clear();

	// Go through all the multisample types
	for( UINT i = 0; i < pFormat->MultisamplesList.size(); i++ )
    {
		D3DMULTISAMPLE_TYPE Type = pFormat->MultisamplesList[i].Type;

		// Go through all the depth stencil formats
		UINT j=0;
		for( j = 0; j < pFormat->DepthStencilList.size(); j++ )
        {
			D3DFORMAT format = pFormat->DepthStencilList[j];
			// Check if the ms type and the ds format fail
			if( FAILED( D3DObject->CheckDeviceMultiSampleType( pFormat->AdapterID, 
				                                            D3DDEVTYPE_HAL,
                                                            format, 
															pFormat->Windowed, 
															Type, 
															NULL ) ) )
            {
				// Add the conflict to the vector if it isnt already there.
				conflict.Format = format;
				conflict.Type = Type;
				if( Find( conflict, 
					pFormat->ConflictsList.begin(), 
					pFormat->ConflictsList.end(),
					FindConflict ) == false )
				{
					pFormat->ConflictsList.push_back( conflict );
				}
            }
        }

		// Now go through all the backbuffer formats we found and make
		// sure they are compatible with the multisample type
		for( j = 0; j < pFormat->BackBuffersList.size(); j++ )
        {
			D3DFORMAT format = pFormat->BackBuffersList[j];
			// Check if the ms type and the format fail
			if( FAILED( D3DObject->CheckDeviceMultiSampleType( pFormat->AdapterID, 
				                                            D3DDEVTYPE_HAL,
                                                            format, 
															pFormat->Windowed, 
															Type, 
															NULL ) ) )
            {
				// Add the conflict to the vector if it isnt already there.
				conflict.Format = format;
				conflict.Type = Type;
				if( Find( conflict, 
					pFormat->ConflictsList.begin(), 
					pFormat->ConflictsList.end(),
					FindConflict ) == false )
				{
					pFormat->ConflictsList.push_back( conflict );
				}
            }
        }
    }
}

// The comparison function used by Find(...) to find a multisample
bool FindMS( const STRUCT_DE_MULTISAMPLE& l, const STRUCT_DE_MULTISAMPLE& r )
{
	return ( l.Type == r.Type );
}

DLL_API void DECore::BuildMultisampleList( STRUCT_DE_ADAPTERFORMAT* pFormat )
{
	STRUCT_DE_MULTISAMPLE ms;
	pFormat->MultisamplesList.clear();

	// List of multisample types
    const D3DMULTISAMPLE_TYPE TypeArray[] = 
	{ 
        D3DMULTISAMPLE_NONE,
        D3DMULTISAMPLE_NONMASKABLE,
        D3DMULTISAMPLE_2_SAMPLES,
        D3DMULTISAMPLE_3_SAMPLES,
        D3DMULTISAMPLE_4_SAMPLES,
        D3DMULTISAMPLE_5_SAMPLES,
        D3DMULTISAMPLE_6_SAMPLES,
        D3DMULTISAMPLE_7_SAMPLES,
        D3DMULTISAMPLE_8_SAMPLES,
        D3DMULTISAMPLE_9_SAMPLES,
        D3DMULTISAMPLE_10_SAMPLES,
        D3DMULTISAMPLE_11_SAMPLES,
        D3DMULTISAMPLE_12_SAMPLES,
        D3DMULTISAMPLE_13_SAMPLES,
        D3DMULTISAMPLE_14_SAMPLES,
        D3DMULTISAMPLE_15_SAMPLES,
        D3DMULTISAMPLE_16_SAMPLES,
    };

    const UINT uiNumElements = sizeof(TypeArray) / sizeof(TypeArray[0]);

	// Loop through all the multisamples
    for( UINT i = 0; i < uiNumElements; i++ )
    {
		ms.Type = TypeArray[i];

		// Check if the multisample type is supported
		if( SUCCEEDED( D3DObject->CheckDeviceMultiSampleType( pFormat->AdapterID, 
														   D3DDEVTYPE_HAL, 
														   pFormat->Format, 
														   pFormat->Windowed, 
														   ms.Type, 
														   &ms.QualityLevels ) ) )
        {
			STRUCT_DE_MULTISAMPLE s;
			s.Type = ms.Type;
			// If the multisample hasnt already been added then add it.
			if( Find( s, pFormat->MultisamplesList.begin(),pFormat->MultisamplesList.end(), FindMS ) == false )
				pFormat->MultisamplesList.push_back( ms );
        }
    }
}

DLL_API void DECore::BuildDepthStencilList( STRUCT_DE_ADAPTERFORMAT* pFormat )
{
	pFormat->DepthStencilList.clear();

	// A list of depth stencil formats
    const D3DFORMAT dsFormatArray[] = 
    {
		D3DFMT_D16_LOCKABLE,
        D3DFMT_D16,
        D3DFMT_D15S1,
        D3DFMT_D24X8,
        D3DFMT_D24S8,
        D3DFMT_D24X4S4,
        D3DFMT_D32,
    };

    const UINT uiNumElements = sizeof(dsFormatArray) / sizeof(dsFormatArray[0]);

	// Loop through all the depth stencil formats
    for( UINT i = 0; i < uiNumElements; i++ )
    {
		// Verify that the depth stencil format exists
		if( SUCCEEDED( D3DObject->CheckDeviceFormat( pFormat->AdapterID, 
												  D3DDEVTYPE_HAL, 
												  pFormat->Format, 
											      D3DUSAGE_DEPTHSTENCIL, 
												  D3DRTYPE_SURFACE, 
												  dsFormatArray[i] ) ) )
        {
			// If it exists, verify its compatibitlity with the adapter format
			if( SUCCEEDED( D3DObject->CheckDepthStencilMatch( pFormat->AdapterID, 
														   D3DDEVTYPE_HAL, 
														   pFormat->Format, 
														   pFormat->Format, 
														   dsFormatArray[i] ) ) )
            {
				// If it's not already there add it
				if( std::find( pFormat->DepthStencilList.begin(), pFormat->DepthStencilList.end(), dsFormatArray[i] ) 
					== pFormat->DepthStencilList.end() )
				{
					pFormat->DepthStencilList.push_back( dsFormatArray[i] );
				}
            }
        }
    }
}


// Comparison function for finding a width and height in an stl container
bool FindWidthHeight( const STRUCT_DE_DISPLAYINFO& left, const STRUCT_DE_DISPLAYINFO& right )
{
	if( left.Height == right.Height &&
		left.Width == right.Width )
		return true;

	return false;
}

// Comparison function for finding a UINT in an stl container
bool FindRefreshRate( const UINT& rr1, const UINT& rr2 )
{
	return ( rr1 == rr2 );
}

DLL_API void DECore::BuildWidthHeightList( STRUCT_DE_ADAPTER* pAdapter)
{
	D3DDISPLAYMODE dm;	// Used to get Display Mode infor from D3D
	UINT ui;			// Used for looping

	pAdapter->DisplayModesList.clear();

	// This is an array of the valid adapter formats
	const D3DFORMAT fmtArray[] = 
	{
		D3DFMT_X1R5G5B5,
		D3DFMT_R5G6B5,
		D3DFMT_X8R8G8B8,
		D3DFMT_A2R10G10B10,
	};
		
	const UINT uiNumArrayElements = sizeof(fmtArray) / sizeof(fmtArray[0]);

	// Loop through all the valid adapter formats, and add the ones that are
	// available on this adapter.
	for( ui = 0; ui < uiNumArrayElements; ui++ )
	{		
		// Check if there are any available display modes using this format
		UINT uiNumModes = D3DObject->GetAdapterModeCount( pAdapter->ID, fmtArray[ui] );

		// If the bpp of this format is less then what we wanted
		// then continue with next loop
		if( DEUtility::GetBitsPerPixel( fmtArray[ui] ) < engineMinBPP )
			continue;

		STRUCT_DE_DISPLAYMODE mode;
		mode.Format = fmtArray[ui];

		// Loop through allthe modes
		if( uiNumModes )
		{
			pAdapter->DisplayModesList.push_back( mode );
		}

	}

	// Now loop through all the ones we found and add their widths and heights
	for( ui = 0; ui < pAdapter->DisplayModesList.size(); ui++ )
	{
		// Go through all the modes on the adapter using this adapter format
		UINT uiNumModes = D3DObject->GetAdapterModeCount( pAdapter->ID, pAdapter->DisplayModesList[ui].Format );
		for( UINT i = 0; i < uiNumModes; i++ )
		{
			// Get the width and height
			D3DObject->EnumAdapterModes( pAdapter->ID, pAdapter->DisplayModesList[ui].Format, i, &dm );

			// Make sure our min/max width height values are being obeyed
			if( dm.Width < engineMinWidth || dm.Height < engineMinHeight )
				continue;

			// Store the info
			STRUCT_DE_DISPLAYINFO wh;
			wh.Height = dm.Height;
			wh.Width = dm.Width;

			// Make sure that the w/h is not already in there, and add it to the vector
			if( Find( wh, 
				      pAdapter->DisplayModesList[ui].WidthHeightList.begin(), 
					  pAdapter->DisplayModesList[ui].WidthHeightList.end(), 
					  FindWidthHeight ) == false )
			{
				pAdapter->DisplayModesList[ui].WidthHeightList.push_back( wh );
			}
		}
	}
	
	// Go through all the display modes 

	for( ui = 0; ui < pAdapter->DisplayModesList.size(); ui++ )
	{
		// Go through all the width/heights, and add the supported refresh rates.
		for( UINT uiWH = 0; uiWH < pAdapter->DisplayModesList[ui].WidthHeightList.size(); uiWH++ )
		{
			// Go through all the modes on the adapter using this adapter format
			UINT uiNumModes = D3DObject->GetAdapterModeCount( pAdapter->ID, pAdapter->DisplayModesList[ui].Format );
			for( UINT i = 0; i < uiNumModes; i++ )
			{
				D3DObject->EnumAdapterModes( pAdapter->ID, pAdapter->DisplayModesList[ui].Format, i, &dm );

				if( dm.Height == pAdapter->DisplayModesList[ui].WidthHeightList[uiWH].Height &&
					dm.Width == pAdapter->DisplayModesList[ui].WidthHeightList[uiWH].Width )
				{
					// Make sure the refresh rate is not already there
					if( Find( dm.RefreshRate, 
							pAdapter->DisplayModesList[ui].WidthHeightList[uiWH].RefreshRateList.begin(),
							pAdapter->DisplayModesList[ui].WidthHeightList[uiWH].RefreshRateList.end(),
							FindRefreshRate ) == false )
					{
						pAdapter->DisplayModesList[ui].WidthHeightList[uiWH].RefreshRateList.push_back( dm.RefreshRate );
					}
				}
			}
		}
	}
}


DLL_API unsigned int DECore::SelectAdapterID(bool startWindowed, unsigned int uiAdapterID)
{
	HRESULT hr;

	//ID del formato ottimale(esegue il controllo quando è in finestra per farlo
	//coincidere con quello del desktop corrente)
	UINT afIDFind = 0;

	//A questo punto prendiamo il display corrente con l'adapter appena selezionato
	D3DDISPLAYMODE dm;
	if( FAILED( ( hr = D3DObject->GetAdapterDisplayMode( uiAdapterID, &dm ) ) ) )
		return hr;

	//Controlliamo il mode's format per vedere se supporta HAL
	for( UINT uiAF = 0; uiAF < engineAdapters[uiAdapterID].FormatsList.size(); uiAF++ )
	{
		//Trova il formato dell'adapter
		if( engineAdapters[uiAdapterID].FormatsList[uiAF].Format == dm.Format && startWindowed == engineAdapters[uiAdapterID].FormatsList[uiAF].Windowed )
		{
			//Se questo formato non supporta HAL cerca il primo che lo supporta e lo imposta
			if( engineAdapters[uiAdapterID].FormatsList[uiAF].DeviceType == D3DDEVTYPE_HAL )
			{
				afIDFind = engineAdapters[uiAdapterID].FormatsList[uiAF].ID;
			}
			else
			{
				for( UINT uii = 0; uii < engineAdapters[uiAdapterID].FormatsList.size(); uii++ )
				{
					if( engineAdapters[uiAdapterID].FormatsList[uii].DeviceType == D3DDEVTYPE_HAL && engineAdapters[uiAdapterID].FormatsList[uii].Windowed == startWindowed )
					{
						afIDFind = engineAdapters[uiAdapterID].FormatsList[uii].ID;
						break;
					}
				}
			}
			break;
		}
	}
	return afIDFind;
}

DLL_API int DECore::BuildAdapterList(bool supportFullscreen, bool supportWindowed, bool bBreakOnREF)
{
	HRESULT hr;
	engineAdapters.clear();

	//Attraversa tutti gli adapter esistenti
	UINT uiAdapter = 0;
	for(uiAdapter = 0; uiAdapter < D3DObject->GetAdapterCount(); uiAdapter++ )
	{
		//Crea l'adapter corrente
		STRUCT_DE_ADAPTER adapter;

		//Riempie la struttura dell'adapter corrente, e inizializza D3DCAPS
		if( FAILED( ( hr = D3DObject->GetAdapterIdentifier( uiAdapter, 0, &adapter.Info ) ) ) )
			return -1;
		if( FAILED( ( hr = D3DObject->GetDeviceCaps( uiAdapter, D3DDEVTYPE_HAL, &adapter.Caps ) ) ) )
			return -1;

		// Salva l'ID dell'adapter corrente. 0 solitamente è D3DADAPTER_DEFAULT. 
		adapter.ID = uiAdapter;

		//Crea le liste principali
		BuildPresentationList( &adapter );
		BuildWidthHeightList( &adapter );

		//Struttura usata per ogni formato da controllare
		const bool WindowedArray[2] = 
		{ 
			false, 
			true,
		};

		//Si è deciso di non supportare ne la finestra ne il fullscreen
		//Bene...come facciamo? Infatti...non facciamo
		if( supportWindowed == false && supportFullscreen == false )
			return -1;
	
		//Array di possibili formati del dispay dell'adapter
		const D3DFORMAT fmtArray[] = 
		{
			D3DFMT_X1R5G5B5,
			D3DFMT_R5G6B5,
			D3DFMT_X8R8G8B8,
			D3DFMT_A2R10G10B10,
		};	
		const UINT uiNumArrayElements = sizeof(fmtArray) / sizeof(fmtArray[0]);

		//Questo valore tiene il conto di quanti formati sono validi.
		//Questo perchè potremmo avere per esempio 4 formati ma solo 1 o 2 sono
		//formati validi.
		UINT uiAFLoops = 0;
		//Passiamo tutti i formati dell'adapter
		for( UINT uiFmt = 0; uiFmt < uiNumArrayElements; uiFmt++ )
		{
			//Se il BPP del formato che stiamo testando e minore del BPP
			//minimo che abbiamo dichiarato allora continua
			if( DEUtility::GetBitsPerPixel( fmtArray[uiFmt] ) < engineMinBPP )
				continue;
			//Controlla se esistono dei modi per il formato dell'adapter corrente
			//Se non esiste vuol dire che non è supportato allora continua
			UINT uiFmtModeCount = D3DObject->GetAdapterModeCount( uiAdapter, fmtArray[uiFmt] );
			if( uiFmtModeCount == 0 )
				continue;
			//Crea il formato dell'adapter
			STRUCT_DE_ADAPTERFORMAT af;
			//Per ogni formato dell'adapter verifichiamo che esista la modalità in finestra e
			//in fullscreen
			bool Windowed;
			for( UINT w = 0; w < 2; w++ )
			{
				// Set the value of Windowed. on loop 0 it will be false
				// and on loop 1 it will be true
				Windowed = WindowedArray[w];
				//Vogliamo il fullscreen ma se non lo supportiamo passa oltre
				if ( Windowed == false && supportFullscreen == false )
					continue;
				//Vogliamo la finestra ma se non la supportiamo passa oltre
				if ( Windowed == true && supportWindowed == false )
					continue;
				//Salva le informazioni del formato dell'adapter corrente
				af.Format = fmtArray[uiFmt];
				af.AdapterID = uiAdapter;
				af.Windowed = Windowed;
				af.ID = uiAFLoops;
				//Con il formato trovato creiamo le varie liste...
				BuildBackBufferList( &af );
				BuildMultisampleList( &af );
				BuildDepthStencilList( &af );
				BuildConflictList( &af );
				DetermineAdapterBehaviour( &af, &adapter.Caps, true );
				//Se viene trovato almeno un vertex processing allora usiamo HAL altrimenti
				//usiamo il REF
				af.DeviceType = D3DDEVTYPE_REF;
				if( af.VertexProcessingList.size() )
					af.DeviceType = D3DDEVTYPE_HAL;
				//Aggiungiamo l'adapter format alla lista
				if( (bBreakOnREF == true && af.DeviceType == D3DDEVTYPE_HAL )|| bBreakOnREF == false )
				{
					adapter.FormatsList.push_back( af );	//Aggiunge il formato
					uiAFLoops++;							//Aggiunge uno al conto
				}
			}
		}
		//Passiamo attraverso i formati dell'adapter che abbiamo trovato
		adapter.DeviceType = D3DDEVTYPE_REF;
		for( UINT uiFmtCount = 0; uiFmtCount < adapter.FormatsList.size(); uiFmtCount++ )
		{
			if( adapter.FormatsList[uiFmtCount].DeviceType == D3DDEVTYPE_HAL )
			{
				adapter.DeviceType = D3DDEVTYPE_HAL;
				break;
			}
		}
		//Aggiungiamo l'adapter format alla lista
		if( (bBreakOnREF == true && adapter.DeviceType == D3DDEVTYPE_HAL )|| bBreakOnREF == false )
		{
			engineAdapters.push_back( adapter );
		}
	}
	//Verifichiamo che un adapter è stato trovato
	if( !engineAdapters.size() )return -1;
	//Questo vettore contiene gli ID degli adapters che utilizanno HAL
	std::vector<UINT> vHALOnAdapter;
	for( UINT x = 0; x < engineAdapters.size(); x++ )
	{
		//Se supporta HAL allora lo aggiunge
		if( engineAdapters[x].DeviceType == D3DDEVTYPE_HAL )
			vHALOnAdapter.push_back( x );
	}

	//Se nessun adapter supporta HAL allora allora usiamo quello di default
	//Altrimenti usiamo quello (il primo) che lo supporta
	if( vHALOnAdapter.size() == 0 ) 
	{
		uiAdapter = D3DADAPTER_DEFAULT;
	}
	else
	{
		uiAdapter = vHALOnAdapter[0];
	}
	return uiAdapter;
}