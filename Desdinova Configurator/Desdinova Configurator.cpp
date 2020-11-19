#include "Desdinova Configurator.h"

//WinMain principale
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR     lpCmdLine,int       nCmdShow)
{


	//Inizializza il motore senza file di log (verrà creato dall'applicazione)
	Engine.Init("","","");
	//Crea la lista principale dell'adapter in finestra (default)
	engineAdapterID = Engine.BuildAdapterList(false, true, false);
	//Seleziona il formato corrente dell'adapter passato
	engineFormatID = Engine.SelectAdapterID(true,engineAdapterID);
	//Crea la schermata dei settaggi
	settingsHWND=CreateDialog(hInstance, (LPCTSTR)DIALOG_SETTINGS, NULL, (DLGPROC)SettingsProc);
	if(settingsHWND==NULL)
	{	
		char currentTitle[MAX_PATH];
		memset(currentTitle, 0, sizeof(char) * MAX_PATH);
		strcpy(currentTitle, settingsTitle);
		strcat(currentTitle, " - Error");
		MessageBox(NULL, "Error during creation settings window. Try to re-execute this application.", currentTitle, MB_OK|MB_ICONWARNING);
	}
	else
	{
		//Ciclo principale dei messaggi
		MSG msg;
		while(1)
		{		
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				if (msg.message==WM_QUIT)
				{				
					break;//Esce dal ciclo
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}

	Engine.Release();

	return 0;
}

void FillSettingsControls(HWND hWnd, unsigned int currentAdapter, unsigned int	currentFormat, unsigned int	currentMultisample, unsigned int currentResolution)
{
	//Risoluzione
	SendMessage(GetDlgItem(hWnd,SETTINGS_RESOLUTION), CB_RESETCONTENT, 0, 0);
	for( UINT x = 0; x < Engine.engineAdapters[currentAdapter].DisplayModesList[currentFormat].WidthHeightList.size(); x++ )
	{
		char currentDisp[MAX_PATH];
		char currentDispX[MAX_PATH];
		char currentDispY[MAX_PATH];
		memset(currentDisp, 0, sizeof(char) * MAX_PATH);
		memset(currentDispX, 0, sizeof(char) * MAX_PATH);
		memset(currentDispY, 0, sizeof(char) * MAX_PATH);
		itoa(Engine.engineAdapters[currentAdapter].DisplayModesList[currentFormat].WidthHeightList[x].Width, currentDispX, 10);
		itoa(Engine.engineAdapters[currentAdapter].DisplayModesList[currentFormat].WidthHeightList[x].Height, currentDispY, 10);
		strcat(currentDisp, currentDispX);
		strcat(currentDisp, "x");
		strcat(currentDisp, currentDispY);
		SendMessage(GetDlgItem(hWnd,SETTINGS_RESOLUTION), CB_ADDSTRING , 0, (LPARAM)currentDisp); 
	}

	//Refresh
	SendMessage(GetDlgItem(hWnd,SETTINGS_REFRESH), CB_RESETCONTENT, 0, 0);
	for( UINT x = 0; x < Engine.engineAdapters[currentAdapter].DisplayModesList[currentFormat].WidthHeightList[engineResolutionID].RefreshRateList.size(); x++ )
	{
		char currentRef[MAX_PATH];
		memset(currentRef, 0, sizeof(char) * MAX_PATH);
		itoa(Engine.engineAdapters[currentAdapter].DisplayModesList[currentFormat].WidthHeightList[engineResolutionID].RefreshRateList[x], currentRef, 10);
		SendMessage(GetDlgItem(hWnd,SETTINGS_REFRESH), CB_ADDSTRING , 0, (LPARAM)currentRef); 
	}

	//BackBuffer
	SendMessage(GetDlgItem(hWnd,SETTINGS_BACKBUFFER), CB_RESETCONTENT, 0, 0);
	UINT ff=Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].BackBuffersList.size();
	for( UINT x = 0; x < ff; x++ )
	{
		char currentForm[MAX_PATH];
		char currentBit[MAX_PATH];
		memset(currentForm, 0, sizeof(char) * MAX_PATH);
		strcpy(currentForm, DEUtility::D3DFormatToString( Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].BackBuffersList[x] ));
		strcat(currentForm, " - ");
		itoa(DEUtility::GetBitsPerPixel(Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].BackBuffersList[x]), currentBit, 10);
		strcat(currentForm, currentBit);
		strcat(currentForm, " Bits");
		SendMessage(GetDlgItem(hWnd,SETTINGS_BACKBUFFER), CB_ADDSTRING , 0, (LPARAM)currentForm); 
	}

	//Depthbuffer
	SendMessage(GetDlgItem(hWnd,SETTINGS_DEPTHBUFFER), CB_RESETCONTENT, 0, 0);
	for( UINT x = 0; x < Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].DepthStencilList.size(); x++ )
		SendMessage(GetDlgItem(hWnd,SETTINGS_DEPTHBUFFER), CB_ADDSTRING , 0, (LPARAM)DEUtility::D3DFormatToString( Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].DepthStencilList[x])); 	
	
	//Multisample
	SendMessage(GetDlgItem(hWnd,SETTINGS_MULTISAMPLETYPE), CB_RESETCONTENT, 0, 0);
	for( UINT x = 0; x < Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].MultisamplesList.size(); x++ )
		SendMessage(GetDlgItem(hWnd,SETTINGS_MULTISAMPLETYPE), CB_ADDSTRING , 0, (LPARAM)DEUtility::MultisampleTypeToString( Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].MultisamplesList[x].Type)); 

	//Vertex processing
	SendMessage(GetDlgItem(hWnd,SETTINGS_VERTEXPROCESSING), CB_RESETCONTENT, 0, 0);
	for( UINT x = 0; x < Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].VertexProcessingList.size(); x++ )
		SendMessage(GetDlgItem(hWnd,SETTINGS_VERTEXPROCESSING), CB_ADDSTRING , 0, (LPARAM)DEUtility::VertexProcessingToString( Engine.engineAdapters[currentAdapter].FormatsList[currentFormat].VertexProcessingList[x] )); 			
	//*/

	//Vertex shader
	SendMessage(GetDlgItem(hWnd,SETTINGS_VS), CB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hWnd,SETTINGS_VS), CB_ADDSTRING , 0, (LPARAM)"1.0"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_VS), CB_ADDSTRING , 0, (LPARAM)"1.1"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_VS), CB_ADDSTRING , 0,	(LPARAM)"1.2"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_VS), CB_ADDSTRING , 0, (LPARAM)"2.0"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_VS), CB_ADDSTRING , 0, (LPARAM)"3.0"); 			

	//Pixel shader
	SendMessage(GetDlgItem(hWnd,SETTINGS_PS), CB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hWnd,SETTINGS_PS), CB_ADDSTRING , 0, (LPARAM)"1.0"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_PS), CB_ADDSTRING , 0, (LPARAM)"1.1"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_PS), CB_ADDSTRING , 0, (LPARAM)"1.2"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_PS), CB_ADDSTRING , 0, (LPARAM)"2.0"); 			
	SendMessage(GetDlgItem(hWnd,SETTINGS_PS), CB_ADDSTRING , 0, (LPARAM)"3.0");
}


LRESULT CALLBACK SettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId; 
	int wmEvent; 

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			//Inizializza i common control utili per il tooltip
			INITCOMMONCONTROLSEX icex;
			TOOLINFO    ti;
			icex.dwSize = sizeof(icex);
			icex.dwICC  = ICC_BAR_CLASSES;
			if(!InitCommonControlsEx(&icex))
			return NULL;
			//Crea il tooltip
			tipHWND = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, GetModuleHandle(NULL), NULL );
			//Settaggi tooltip			
			SendMessage(tipHWND, TTM_SETMAXTIPWIDTH, 0, 275);
			SendMessage(tipHWND, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELONG(7500,0));
			SendMessage(tipHWND, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELONG(1500,0));
			SendMessage(tipHWND, TTM_SETDELAYTIME, TTDT_RESHOW, MAKELONG(1000,0));
			SetWindowPos(tipHWND, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			
			//Imposta le stringe in base alla posizione dei vari controlli
			RECT rect;
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_SUBCLASS|TTF_TRANSPARENT;
			ti.hinst = GetModuleHandle(NULL);
			ti.uId = 0;

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_ADAPTER);
			GetClientRect(ti.hwnd,&rect);
			ti.lpszText = "List of adapters connected to the monitors.";
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_WINDOWED);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage0[ ] =
			"Application runs in windowed mode.\r\n"
			"You can resize window dimension to desidered resolution.";
			ti.lpszText = szLongMessage0;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_FULLSCREEN);
			GetClientRect(ti.hwnd,&rect);
			ti.lpszText = "Application runs in fullscreen mode.";
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT);
			GetClientRect(ti.hwnd,&rect);
			ti.lpszText = "Resolution color bit depth.";
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_RESOLUTION);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage1[ ] =
			"Screen resolution of the application.\r\n"
			"For windowed applications is the dimension of the window, in fullscreen mode is the render screen resolution.";
			ti.lpszText = szLongMessage1;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_REFRESH);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage2[ ] =
			"Monitor refresh rate.\r\n"
			"This parameters is only for fullscreen mode.";
			ti.lpszText = szLongMessage2;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_BACKBUFFER);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage3[ ] =
			"Format of the back buffer.\r\n"
			"For windowed applications, the back buffer format does not need to match the adapter format if the hardware supports color conversion. The set of possible back buffer formats is constrained, but the runtime will allow any valid back buffer format to be presented to any desktop format.\r\n"
			"Usually this parameter should be the same of the adapter format.";
			ti.lpszText = szLongMessage3;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_DEPTHBUFFER);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage4[ ] =
			"Format of the depth/stencil buffer.\r\n"
			"A depth buffer, often called a z-buffer or a w-buffer, is a property of the device that stores depth information to be used. "
			"Typical applications for entertainment or visual simulations with exterior scenes often require far-plane/near-plane ratios of anywhere between 1000 to 10000. At a ratio of 1000, 98% of the range is spent on the 1st 2% of the depth range, and the distribution becomes worse with higher ratios. This can cause hidden surface artifacts in distant objects, especially when using 16-bit depth buffers, the most commonly supported bit-depth. "
			"However, in most cases this parameters should be D3DFMT_D16.";
			ti.lpszText = szLongMessage4;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_MULTISAMPLETYPE);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage5[ ] =
			"Technique of antialiasing.\r\n"
			"D3DMULTISAMPLE_NONE : No antialising.\r\n"
			"D3DMULTISAMPLE_NONMASKABLE : Non maskable type(include next types).\r\n"
			"D3DMULTISAMPLE_2_SAMPLES\\3_SAMPLES\\4_SAMPLES... : Tecniques applied.\r\n";
			ti.lpszText = szLongMessage5;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_MULTISAMPLEQUALITY);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage6[ ] =
			"Quality level of antialiasing.\r\n"
			"0 is the first level of quality.";
			ti.lpszText = szLongMessage6;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_VERTEXPROCESSING);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage7[ ] =
			"Vertex processing mode.\r\n"
			"With HAL adapters this parameters should be HARDWARE to enable hardware acceleration.";
			ti.lpszText = szLongMessage7;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_INTERVAL);
			GetClientRect(ti.hwnd,&rect);
			char szLongMessage8[ ] =
			"Vertical retrace syncronization.\r\n"
			"D3DPRESENT_INTERVAL_DEFAULT : This is usually equivalent to D3DPRESENT_INTERVAL_ONE.\r\n"
			"D3DPRESENT_INTERVAL_IMMEDIATE : Updates the window client area immediately and might do so more than once during the adapter refresh period.\r\n"
			"D3DPRESENT_INTERVAL_ONE\\TWO\\THREE... : Wait for the vertical retrace period.\r\n";
			ti.lpszText = szLongMessage8;
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			ti.hwnd = GetDlgItem(hWnd,SETTINGS_LINK);
			GetClientRect(ti.hwnd,&rect);
			ti.lpszText = "Latest version and more info on Desdinova Engine website. Click to check it out!";
			ti.rect = rect;	
			SendMessage(tipHWND, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);


			//Setta il titolo della finestra
			SetWindowText(hWnd,settingsTitle);

			try
			{
				//Adapter
				SendMessage(GetDlgItem(hWnd,SETTINGS_ADAPTER), CB_RESETCONTENT, 0, 0);
				for( UINT x = 0; x < Engine.engineAdapters.size(); x++ )
				{
					char currentAdap[MAX_PATH];
					memset(currentAdap, 0, sizeof(char) * MAX_PATH);
					strcpy(currentAdap, Engine.engineAdapters[x].Info.Description);
					SendMessage(GetDlgItem(hWnd,SETTINGS_ADAPTER), CB_ADDSTRING , 0, (LPARAM)currentAdap); 
				}
				//Formati
				SendMessage(GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_RESETCONTENT, 0, 0);
				for( UINT x = 0; x < Engine.engineAdapters[engineAdapterID].FormatsList.size(); x++ )
				{
					char currentForm[MAX_PATH];
					char currentBit[MAX_PATH];
					memset(currentForm, 0, sizeof(char) * MAX_PATH);
					strcpy(currentForm, DEUtility::D3DFormatToString( Engine.engineAdapters[engineAdapterID].FormatsList[x].Format ));
					strcat(currentForm, " - ");
					itoa(DEUtility::GetBitsPerPixel(Engine.engineAdapters[engineAdapterID].FormatsList[x].Format), currentBit, 10);
					strcat(currentForm, currentBit);
					strcat(currentForm, " Bits");
					SendMessage(GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_ADDSTRING , 0, (LPARAM)currentForm); 
				}
				//Present
				SendMessage(GetDlgItem(hWnd,SETTINGS_INTERVAL), CB_RESETCONTENT, 0, 0);
				for( UINT ui = 0; ui < Engine.engineAdapters[engineAdapterID].PresentIntervalsList.size(); ui++ )
					SendMessage(GetDlgItem(hWnd,SETTINGS_INTERVAL), CB_ADDSTRING , 0, (LPARAM)DEUtility::PresentIntervalToString( Engine.engineAdapters[engineAdapterID].PresentIntervalsList[ui] )); 

				FillSettingsControls(hWnd, engineAdapterID, engineFormatID, engineMultisampleType, engineResolutionID);

				SendMessage(GetDlgItem(hWnd,SETTINGS_WINDOWED), BM_SETCHECK, BST_CHECKED, 0);
				SendMessage (GetDlgItem(hWnd,SETTINGS_ADAPTER), CB_SETCURSEL, engineAdapterID, 0);
				SendMessage (GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_SETCURSEL, engineFormatID, 0);
				SendMessage (GetDlgItem(hWnd,SETTINGS_RESOLUTION), CB_SETCURSEL, Engine.engineAdapters[engineAdapterID].DisplayModesList[engineFormatID].WidthHeightList.size()-1, 0);

				EnableWindow(GetDlgItem(hWnd,SETTINGS_REFRESH),false);

				//Mostra la schermata
				ShowWindow(hWnd,SW_SHOW);
			}
			catch(...)
			{
				char currentTitle[MAX_PATH];
				memset(currentTitle, 0, sizeof(char) * MAX_PATH);
				strcpy(currentTitle, settingsTitle);
				strcat(currentTitle, " - Error");
				MessageBox(NULL, "Error during window parameters initialization. Check hardware capabilities and try again.", currentTitle, MB_OK|MB_ICONWARNING);
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}
		break;

		case WM_CLOSE:
		case WM_DESTROY: 
		{	 
			PostQuitMessage(0); 
			return 0;
		}
		break;

		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				//** OK **
				case SETTINGS_OK: 
				{
					//Salva e esce
					if (SaveSettingsFile(settingsFilename))
					{
						SendMessage(hWnd, WM_CLOSE, 0, 0);
					}
				}
				break;

				//** CANCEL **
				case SETTINGS_CANCEL: 
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
				break;

				//** ADAPTER **
				case SETTINGS_ADAPTER: 
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
							engineAdapterID = SendMessage(GetDlgItem(hWnd,SETTINGS_ADAPTER), CB_GETCURSEL, 0, 0);
							FillSettingsControls(hWnd, engineAdapterID, engineFormatID, engineMultisampleType, engineResolutionID);
							SendMessage (GetDlgItem(hWnd,SETTINGS_ADAPTER), CB_SETCURSEL, engineAdapterID, 0);
						}
						break;
					}
				}
				break;
			
				//** WINDOWED **
				case SETTINGS_WINDOWED: 
				{
					Engine.BuildAdapterList(false, true, false);
					engineFormatID = Engine.SelectAdapterID(true,engineAdapterID);
					FillSettingsControls(hWnd, engineAdapterID, engineFormatID, engineMultisampleType, engineResolutionID);
					EnableWindow(GetDlgItem(hWnd,SETTINGS_REFRESH),false);
					SendMessage (GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_SETCURSEL, engineFormatID, 0);
				}
				break;

				//** FULLSCREEN **
				case SETTINGS_FULLSCREEN: 
				{
					Engine.BuildAdapterList(true, false, false);
					engineFormatID = Engine.SelectAdapterID(false,engineAdapterID);
					FillSettingsControls(hWnd, engineAdapterID, engineFormatID, engineMultisampleType, engineResolutionID);
					EnableWindow(GetDlgItem(hWnd,SETTINGS_REFRESH),true);
					SendMessage (GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_SETCURSEL, engineFormatID, 0);
				}
				break;

				//** ADAPTER FORMAT **
				case SETTINGS_ADAPTERFORMAT: 
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
							engineFormatID = SendMessage(GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_GETCURSEL, 0, 0);
							FillSettingsControls(hWnd, engineAdapterID, engineFormatID, engineMultisampleType, engineResolutionID);
							SendMessage (GetDlgItem(hWnd,SETTINGS_ADAPTERFORMAT), CB_SETCURSEL, engineFormatID, 0);
						}
						break;
					}
				}
				break;

				//** RESOLUTION **
				case SETTINGS_RESOLUTION: 
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
							engineResolutionID = SendMessage(GetDlgItem(hWnd,SETTINGS_RESOLUTION), CB_GETCURSEL, 0, 0);
							FillSettingsControls(hWnd, engineAdapterID, engineFormatID, engineMultisampleType, engineResolutionID);
							SendMessage (GetDlgItem(hWnd,SETTINGS_RESOLUTION), CB_SETCURSEL, engineResolutionID, 0);
						}
						break;
					}
				}
				break;

				//** MULTISAMPLE **
				case SETTINGS_MULTISAMPLETYPE: 
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
							engineMultisampleType = SendMessage(GetDlgItem(hWnd,SETTINGS_MULTISAMPLETYPE), CB_GETCURSEL, 0, 0);
							//Multisample quality
							SendMessage(GetDlgItem(hWnd,SETTINGS_MULTISAMPLEQUALITY), CB_RESETCONTENT, 0, 0);
							for ( UINT y=0; y<Engine.engineAdapters[engineAdapterID].FormatsList[engineFormatID].MultisamplesList[engineMultisampleType].QualityLevels; y++)
							{
								char currentQual[MAX_PATH];
								memset(currentQual, 0, sizeof(char) * MAX_PATH);
								itoa(y, currentQual, 10);
								SendMessage(GetDlgItem(hWnd,SETTINGS_MULTISAMPLEQUALITY), CB_ADDSTRING , 0, (LPARAM)currentQual ); 		
							}
							SendMessage (GetDlgItem(hWnd,SETTINGS_MULTISAMPLETYPE), CB_SETCURSEL, engineMultisampleType, 0);
						}
						break;
					}
				}
				break;

				//** LINK **
				case SETTINGS_LINK: 
				{
					switch (HIWORD(wParam))
					{
						case STN_CLICKED:
						{
							HlinkSimpleNavigateToString(L"http://www.desdinova.altervista.org",NULL,  NULL, NULL, NULL, NULL, 0, 0);
						}
						break;
					}
				}
				break;

				default:
				   return DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
		}
		break;
	}

	return FALSE;
}


bool SaveSettingsFile(char *fileName)
{
	//Titolo x errore
	char currentTitle[MAX_PATH];
	memset(currentTitle, 0, sizeof(char) * MAX_PATH);
	strcpy(currentTitle, settingsTitle);
	strcat(currentTitle, " - Missing parameter");
	//Controllo sull'inserimento dei parametri, se mancante mostra un avviso
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select an Adapter Name.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_RESOLUTION),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Resolution.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select an Adapter Format.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_FULLSCREEN),BM_GETCHECK ,0,0)==BST_CHECKED)
	{
		if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_REFRESH),CB_GETCURSEL,0,0)==CB_ERR)
		{
			MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Refresh Rate.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
		}
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_BACKBUFFER),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Back Buffer Format.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_DEPTHBUFFER),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select Depth/Stencil Buffer Format.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_MULTISAMPLETYPE),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Multisample Type.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_MULTISAMPLEQUALITY),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Multisample Quality.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_VERTEXPROCESSING),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Vertex Processing.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_INTERVAL),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Presentation Interval.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_VS),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Minimum Vertex Shader.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}
	if (SendMessage(GetDlgItem(settingsHWND,SETTINGS_PS),CB_GETCURSEL,0,0)==CB_ERR)
	{
		MessageBox(NULL,"Cannot save config file bacause missing parameter. Select a Minimum Presentation Interval.",currentTitle,MB_OK|MB_ICONWARNING);	return false;
	}

	STRUCT_DE_DEVICESETTINGS	saveDeviceSettings;
	saveDeviceSettings.adapterID			= engineAdapterID;

	if ( SendMessage (GetDlgItem(settingsHWND,SETTINGS_WINDOWED), BM_GETSTATE, 0, 0)==BST_CHECKED)	saveDeviceSettings.presentParameters.Windowed = true;
	else																							saveDeviceSettings.presentParameters.Windowed = false;
	saveDeviceSettings.presentParameters.BackBufferFormat			= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].FormatsList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].BackBuffersList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_BACKBUFFER),CB_GETCURSEL,0,0)];
	saveDeviceSettings.presentParameters.BackBufferWidth			= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].DisplayModesList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].WidthHeightList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_RESOLUTION),CB_GETCURSEL,0,0)].Width;
	saveDeviceSettings.presentParameters.BackBufferHeight			= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].DisplayModesList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].WidthHeightList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_RESOLUTION),CB_GETCURSEL,0,0)].Height;			
	saveDeviceSettings.presentParameters.AutoDepthStencilFormat		= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].FormatsList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].DepthStencilList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_DEPTHBUFFER),CB_GETCURSEL,0,0)];
	saveDeviceSettings.presentParameters.MultiSampleType			= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].FormatsList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].MultisamplesList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_MULTISAMPLETYPE),CB_GETCURSEL,0,0)].Type;
	saveDeviceSettings.presentParameters.MultiSampleQuality			= SendMessage(GetDlgItem(settingsHWND,SETTINGS_MULTISAMPLEQUALITY),CB_GETCURSEL,0,0);
	saveDeviceSettings.presentParameters.PresentationInterval		= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].PresentIntervalsList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_INTERVAL),CB_GETCURSEL,0,0)];
	if (saveDeviceSettings.presentParameters.Windowed)				saveDeviceSettings.presentParameters.FullScreen_RefreshRateInHz	= 0;
	else															saveDeviceSettings.presentParameters.FullScreen_RefreshRateInHz	= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].DisplayModesList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].WidthHeightList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_RESOLUTION),CB_GETCURSEL,0,0)].RefreshRateList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_REFRESH),CB_GETCURSEL,0,0)];
	saveDeviceSettings.vertexProcessing								= Engine.engineAdapters[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTER),CB_GETCURSEL,0,0)].FormatsList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_ADAPTERFORMAT),CB_GETCURSEL,0,0)].VertexProcessingList[SendMessage(GetDlgItem(settingsHWND,SETTINGS_VERTEXPROCESSING),CB_GETCURSEL,0,0)];
	//VS
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_VS),CB_GETCURSEL,0,0)==0)
	{
		saveDeviceSettings.minVertexShaderMaj = 1;
		saveDeviceSettings.minVertexShaderMin = 0;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_VS),CB_GETCURSEL,0,0)==1)
	{
		saveDeviceSettings.minVertexShaderMaj = 1;
		saveDeviceSettings.minVertexShaderMin = 1;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_VS),CB_GETCURSEL,0,0)==2)
	{
		saveDeviceSettings.minVertexShaderMaj = 1;
		saveDeviceSettings.minVertexShaderMin = 2;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_VS),CB_GETCURSEL,0,0)==3)
	{
		saveDeviceSettings.minVertexShaderMaj = 2;
		saveDeviceSettings.minVertexShaderMin = 0;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_VS),CB_GETCURSEL,0,0)==4)
	{
		saveDeviceSettings.minVertexShaderMaj = 3;
		saveDeviceSettings.minVertexShaderMin = 0;
	}
	//PS
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_PS),CB_GETCURSEL,0,0)==0)
	{
		saveDeviceSettings.minPixelShaderMaj = 1;
		saveDeviceSettings.minPixelShaderMin = 0;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_PS),CB_GETCURSEL,0,0)==1)
	{
		saveDeviceSettings.minPixelShaderMaj = 1;
		saveDeviceSettings.minPixelShaderMin = 1;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_PS),CB_GETCURSEL,0,0)==2)
	{
		saveDeviceSettings.minPixelShaderMaj = 1;
		saveDeviceSettings.minPixelShaderMin = 2;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_PS),CB_GETCURSEL,0,0)==3)
	{
		saveDeviceSettings.minPixelShaderMaj = 2;
		saveDeviceSettings.minPixelShaderMin = 0;
	}
	if (SendMessage(GetDlgItem(settingsHWND, SETTINGS_PS),CB_GETCURSEL,0,0)==4)
	{
		saveDeviceSettings.minPixelShaderMaj = 3;
		saveDeviceSettings.minPixelShaderMin = 0;
	}


	//Crea il file da salvare e ci salva il presentParameters settato dalla finestra di config
	if (Engine.SaveSettingsFile(fileName, saveDeviceSettings))
	{
		//Attesa per l'effettivo salvataggio del file
		Sleep(500);
		//Avviso
		char currentTitle[MAX_PATH];
		memset(currentTitle, 0, sizeof(char) * MAX_PATH);
		strcpy(currentTitle, settingsTitle);
		strcat(currentTitle, " - Information");
		MessageBox(NULL, "Config file created successfully. Now you can run application with these new settings.", currentTitle, MB_OK|MB_ICONINFORMATION);
		return true;
	}
	else
	{
		//Errore
		char currentTitle[MAX_PATH];
		memset(currentTitle, 0, sizeof(char) * MAX_PATH);
		strcpy(currentTitle, settingsTitle);
		strcat(currentTitle, " - Error");
		MessageBox(NULL, "Error during saving settings file. Check file location and attributes then try again.", currentTitle, MB_OK|MB_ICONWARNING);
		return false;
	}

	return false;
}