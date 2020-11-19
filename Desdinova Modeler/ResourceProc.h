#ifndef H_RESOURCEPROC_H
#define H_RESOURCEPROC_H



LRESULT CALLBACK ResourceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	int wmId;
	int wmEvent;
	HMENU mainMenu;
	mainMenu=GetMenu(hWnd);
	
	//ESS Parameters SAVE
	OPENFILENAME saveParamsESS = {0}; 
	char saveESS[BUFF_MAX] = {0};
	char saveESSTitle[BUFF_MAX] = {0};
	char saveFilterESS[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(saveFilterESS,"Entity ESS File (*.ess)");
	int saveindexFilterESS = (int)strlen(saveFilterESS) + 1;
	saveFilterESS[saveindexFilterESS++] = '*';
	saveFilterESS[saveindexFilterESS++] = '.';
	saveFilterESS[saveindexFilterESS++] = 'e';
	saveFilterESS[saveindexFilterESS++] = 's';
	saveFilterESS[saveindexFilterESS++] = 's';

	//MSS Parameters SAVE
	OPENFILENAME saveParamsMSS = {0}; 
	char saveMSS[BUFF_MAX] = {0};
	char saveMSSTitle[BUFF_MAX] = {0};
	char saveFilterMSS[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(saveFilterMSS,"Entity MSS File (*.mss)");
	int saveindexFilterMSS = (int)strlen(saveFilterMSS) + 1;
	saveFilterMSS[saveindexFilterMSS++] = '*';
	saveFilterMSS[saveindexFilterMSS++] = '.';
	saveFilterMSS[saveindexFilterMSS++] = 'm';
	saveFilterMSS[saveindexFilterMSS++] = 's';
	saveFilterMSS[saveindexFilterMSS++] = 's';

	switch (uMsg)									// Check For Windows Messages
	{

		case WM_INITDIALOG:							// Watch For Window Activate Message
		{
			hWndMain=hWnd;
		}
		break;

        case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{
				//Ridimensiona la toolbar
				SendMessage(hWndToolbar, WM_SIZE, 0, 0 );

				//Ridimensiona l'area di rendering in base alla finestra padre e alla status bar
				RECT rect1;
				RECT rect2;
				RECT rect3;
				RECT rect4;
				GetWindowRect(hWndMain, &rect1);
				GetWindowRect(GetDlgItem(hWndMain,RENDER_ZONE), &rect2);
				GetWindowRect(hWndStatusbar, &rect3);
				SetWindowPos(GetDlgItem(hWndMain,RENDER_ZONE), HWND_TOP, 0, 0, rect1.right-rect1.left-7, (rect1.bottom-rect1.top)-(rect2.top - rect1.top)-(rect3.bottom-rect3.top)-4, SWP_NOMOVE); 
				
				//Ridimensiona la status bar proporzionando nuovamente le dimensioni delle parti
				RECT rcClient;
				HLOCAL hloc;
				LPINT lpParts;
				int i, nWidth;
				int nParts = SendMessage(hWndStatusbar, SB_GETPARTS, 0 , 0);
				GetClientRect(hWndMain, &rcClient);
				hloc = LocalAlloc(LHND, sizeof(int) * nParts);
				lpParts = (LPINT)LocalLock(hloc);
				nWidth = rcClient.right / nParts;
				for (i = 0; i < nParts; i++) 
				{ 
					lpParts[i] = nWidth * (i+1);
				}
				SendMessage(hWndStatusbar, SB_SETPARTS, (WPARAM) nParts, (LPARAM)lpParts);
				SendMessage(hWndStatusbar, WM_SIZE, 0, 0 );
				LocalUnlock(hloc);
				LocalFree(hloc);//*/
					
				//Ridimensiona il device
				Engine.ResizeSwapChain(0, 0, 0);				
				//GetWindowRect(GetDlgItem(hWndMain,RENDER_ZONE), &rect2);
				//Engine.ResizeDevice(0, rect2.right-rect2.left-4, rect2.bottom-rect2.top-4 );

			}
		}
		break;


        case WM_DESTROY:
		{
            PostQuitMessage(0);
            return 0;
		}
        break;


        case WM_ACTIVATE:
		{
			Engine.SetActiveSwapChain(0);
			Scene0.Camera.SetHWND(Engine.GetActiveSwapChain().hWnd);
		}
        break;

		case WM_PAINT:
		{
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
		break;										// Exit


		case WM_CLOSE:
		{
			Engine.RemoveSwapChain(0);
		}
		break;
 

		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{


				//* * * * * * * * * * * 
				//* * * MENU FILE * * * 
				//* * * * * * * * * * * 


				case MENU_FILE_NEW:
				{
					EnableWindow(hWnd,false);
					HWND hWndNew=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_NEW, hWnd, (DLGPROC)NewProc);
					ShowWindow(hWndNew,SW_SHOW);
				}
				break;

				case MENU_FILE_OPEN:
				{
					EnableWindow(hWnd,false);
					HWND hWndOpen=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_OPEN, hWnd, (DLGPROC)OpenProc);
					ShowWindow(hWndOpen,SW_SHOW);
				}
				break;

				case MENU_FILE_CLOSE:
				{
					if (Scene0.opened)	//Se è aperto un modello
					{
						HRESULT hResult;
						hResult=MessageBox(hWnd,"Are you sure? Unsaved changes will be lost!","Close",MB_OKCANCEL|MB_ICONQUESTION);
						if (hResult==IDOK)
						{
							//Chiude la schermata delle entità se aperta
							SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_ENTITIESMANAGER, MAKELONG(false, 0));
							Scene0.showEntitiesManager=false;
							SendMessage(hWndEntitiesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
							//Chiude la schermata dei modelli se aperta
							SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(false, 0));
							Scene0.showModelsManager=false;
							SendMessage(hWndModelsManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
							//Chiude la schermata del sistema particellare se aperta
							SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_PARTICLESMANAGER, MAKELONG(false, 0));
							Scene0.showParticlesManager=false;
							SendMessage(hWndParticlesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)

							//Rilascia cosa è stato aperto
							if (Scene0.currentCreationMode==1)	//Modello
							{
								Scene0.Model.Release();
							}
							if (Scene0.currentCreationMode==2)	//Entità
							{
								Scene0.Entity.Release();
							}
							if (Scene0.currentCreationMode==3)	//Sistema Particellare
							{
								Scene0.ParticleSystems.Release();
							}

							//Nulla aperto
							Scene0.opened = false;

							//Setta il tipo di modalità (nessuno)
							Scene0.currentCreationMode = 0;

							//Entità
							Scene0.currentCHILDID = -1;	//Solo padre

							//Setta il titolo della schermata principale
							char currentMainTitle[MAX_PATH];
							char newMainTitle[MAX_PATH];
							memset(currentMainTitle, 0, sizeof(char) * MAX_PATH);
							memset(newMainTitle, 0, sizeof(char) * MAX_PATH);
							GetWindowText(hWndMain, currentMainTitle, GetWindowTextLength(hWndMain));
							char *pdest1;
							int result1=0;
							pdest1 = strchr( currentMainTitle, '-' );
							result1 = pdest1 - currentMainTitle + 1;
							strncpy(newMainTitle, currentMainTitle, result1-1 );
							SetWindowText(hWndMain, newMainTitle);

						}
					}
				}
				break;

				case MENU_FILE_SAVE:
				{
					if (Scene0.opened)
					{
						EnableWindow(hWnd,false);
						HWND hWndSave=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_SAVE, hWnd, (DLGPROC)SaveProc);
						ShowWindow(hWndSave,SW_SHOW);
					}
				}
				break;



				//Uscita
				case MENU_FILE_EXIT:
				{					
					//Se è aperto un modello
					if (Scene0.opened)
					{
						SendMessage(hWnd,WM_COMMAND,MENU_FILE_CLOSE,0);//Chiude la finestra(vedere WM_CLOSE)
					}
					SendMessage(hWnd,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
				}
				break;






				//* * * * * * * * * * * 
				//* * * MENU EDIT * * * 
				//* * * * * * * * * * * 
				//Model Manager
				case MENU_EDIT_MODELSMANAGER:
				{
					if (Scene0.opened)
					{
						if (Scene0.currentCreationMode==1)
						{
							if (!Scene0.showModelsManager)
							{
								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(true, 0));
								HWND hWndModelsManager=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_MODELSMANAGER,hWnd,(DLGPROC)ModelsManagerProc);
								ShowWindow(hWndModelsManager,SW_SHOW);
								Scene0.showModelsManager=true;
							}
							else
							{
								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(false, 0));
								Scene0.showModelsManager=false;
								SendMessage(hWndModelsManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
							}
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("No current model.\n")
											_T("Please create or load model and try again."));
							MessageBox(hWnd,strMsg,"Models Manager Error",MB_ICONWARNING|MB_OK);
						}
					}
					else
					{
						TCHAR strMsg[512];
						_tcscpy( strMsg, _T("No current model.\n")
										 _T("Please create or load model and try again."));
						MessageBox(hWnd,strMsg,"Models Manager Error",MB_ICONWARNING|MB_OK);
					}
				}
				break;
				
				//Entity Manager
				case MENU_EDIT_ENTITIESMANAGER:
				{
					if (Scene0.opened)
					{
						if (Scene0.currentCreationMode==2)
						{
							if (!Scene0.showEntitiesManager)
							{
								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_ENTITIESMANAGER, MAKELONG(true, 0));
								HWND hWndEntitiesManager=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_ENTITIESMANAGER,hWnd,(DLGPROC)EntitiesManagerProc);
								ShowWindow(hWndEntitiesManager,SW_SHOW);
								Scene0.showEntitiesManager=true;
							}
							else
							{
								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_ENTITIESMANAGER, MAKELONG(false, 0));
								Scene0.showEntitiesManager=false;
								SendMessage(hWndEntitiesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
							}
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("No current entity.\n")
											_T("Please create or load entity and try again."));
							MessageBox(hWnd,strMsg,"Entities Manager Error",MB_ICONWARNING|MB_OK);
						}
					}
					else
					{
						TCHAR strMsg[512];
						_tcscpy( strMsg, _T("No current entity.\n")
										 _T("Please create or load entity and try again."));
						MessageBox(hWnd,strMsg,"Entities Manager Error",MB_ICONWARNING|MB_OK);
					}
				}
				break;



				//Partciles system Manager
				case MENU_EDIT_PARTICLESMANAGER:
				{
					if (Scene0.opened)
					{
						if (Scene0.currentCreationMode==3)
						{
							if (!Scene0.showParticlesManager)
							{
								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_PARTICLESMANAGER, MAKELONG(true, 0));
								HWND hWndParticlesManager=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_PARTICLESMANAGER,hWnd,(DLGPROC)ParticlesManagerProc);
								ShowWindow(hWndParticlesManager,SW_SHOW);
								Scene0.showParticlesManager=true;
							}
							else
							{
								SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_PARTICLESMANAGER, MAKELONG(false, 0));
								Scene0.showParticlesManager=false;
								SendMessage(hWndParticlesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
							}
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("No current particles system.\n")
											_T("Please create or load particles system and try again."));
							MessageBox(hWnd,strMsg,"Particles System Manager Error",MB_ICONWARNING|MB_OK);
						}

					}
					else
					{
						TCHAR strMsg[512];
						_tcscpy( strMsg, _T("No current particles system.\n")
										_T("Please create or load particles system and try again."));
						MessageBox(hWnd,strMsg,"Particles System Manager Error",MB_ICONWARNING|MB_OK);
					}

				}
				break;


				/*Lights Manager
				case MENU_EDIT_LIGHTSMANAGER:
				{
					if (!showLightsManager)
					{
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_LIGHTSMANAGER, MAKELONG(true, 0));
						HWND hWndLightsManager=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_LIGHTSMANAGER,hWnd,(DLGPROC)LightsManagerProc);
						ShowWindow(hWndLightsManager,SW_SHOW);
						showLightsManager=true;
					}
					else
					{
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_LIGHTSMANAGER, MAKELONG(false, 0));
						showLightsManager=false;
						SendMessage(hWndLightsManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
					}
				}
				break;*/

				//Camera Control
				case MENU_EDIT_CAMERACONTROL:
				{
					if (!Scene0.showCameraControl)
					{
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_CAMERACONTROL, MAKELONG(true, 0));
						HWND hWndCameraControl=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_CAMERACONTROL,hWnd,(DLGPROC)CameraControlProc);
						ShowWindow(hWndCameraControl,SW_SHOW);	
						Scene0.showCameraControl=true;
					}
					else
					{
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_CAMERACONTROL, MAKELONG(false, 0));
						Scene0.showCameraControl=false;
						SendMessage(hWndCameraControl,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
					}
				}
				break;





				//* * * * * * * * * * * 
				//* * * MENU VIEW * * * 
				//* * * * * * * * * * * 
				//Frustrum Culling
				case MENU_VIEW_FRUSTRUMCULLING:
				{
					if (!Scene0.renderProperties.useFrustrumCulling)
				    { 
						CheckMenuItem(mainMenu,MENU_VIEW_FRUSTRUMCULLING, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_FRUSTRUMCULLING, MAKELONG(true, 0));
						Scene0.renderProperties.useFrustrumCulling=true;
					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_FRUSTRUMCULLING, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_FRUSTRUMCULLING, MAKELONG(false, 0));
						Scene0.renderProperties.useFrustrumCulling=false;
					}
				}
				break;


				//LOD
				case MENU_VIEW_LOD:
				{
					if (!Scene0.renderProperties.useLOD)
				    { 
						CheckMenuItem(mainMenu,MENU_VIEW_LOD, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LOD, MAKELONG(true, 0));
						Scene0.renderProperties.useLOD=true;
					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_LOD, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LOD, MAKELONG(false, 0));
						Scene0.renderProperties.useLOD=false;
					}
				}
				break;


				//Wireframe
				case MENU_VIEW_WIREFRAME:
				{
					if (!Scene0.renderProperties.useWireframe)
				    { 
						CheckMenuItem(mainMenu,MENU_VIEW_WIREFRAME, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_WIREFRAME, MAKELONG(true, 0));
						Scene0.renderProperties.useWireframe=true;
					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_WIREFRAME, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_WIREFRAME, MAKELONG(false, 0));
						Scene0.renderProperties.useWireframe=false;
					}
				}
				break;


				//Use Lighting
				case MENU_VIEW_LIGHTING:
				{
					if (!Scene0.renderProperties.useLighting)
				    { 
						CheckMenuItem(mainMenu,MENU_VIEW_LIGHTING, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LIGHTING, MAKELONG(true, 0));
						Scene0.renderProperties.useLighting=true;
					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_LIGHTING, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_LIGHTING, MAKELONG(false, 0));
						Scene0.renderProperties.useLighting=false;
					}
				}
				break;


				//Bounding None
				case MENU_BOUNDING_NONE:
				{
					CheckMenuItem(mainMenu,MENU_BOUNDING_NONE, MF_CHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_NONE, MAKELONG(true, 0));
					CheckMenuItem(mainMenu,MENU_BOUNDING_BOX, MF_UNCHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_BOX, MAKELONG(false, 0));
					CheckMenuItem(mainMenu,MENU_BOUNDING_SPHERE, MF_UNCHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_SPHERE, MAKELONG(false, 0));
					Scene0.renderProperties.useBounding=0;
				}
				break;
				//Bounding Box
				case MENU_BOUNDING_BOX:
				{
					CheckMenuItem(mainMenu,MENU_BOUNDING_NONE, MF_UNCHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_NONE, MAKELONG(false, 0));
					CheckMenuItem(mainMenu,MENU_BOUNDING_BOX, MF_CHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_BOX, MAKELONG(true, 0));
					CheckMenuItem(mainMenu,MENU_BOUNDING_SPHERE, MF_UNCHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_SPHERE, MAKELONG(false, 0));
					Scene0.renderProperties.useBounding=1;
				}
				break;
				//Bounding Sphere
				case MENU_BOUNDING_SPHERE:
				{
					CheckMenuItem(mainMenu,MENU_BOUNDING_NONE, MF_UNCHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_NONE, MAKELONG(false, 0));
					CheckMenuItem(mainMenu,MENU_BOUNDING_BOX, MF_UNCHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_BOX, MAKELONG(false, 0));
					CheckMenuItem(mainMenu,MENU_BOUNDING_SPHERE, MF_CHECKED);
					SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_BOUNDING_SPHERE, MAKELONG(true, 0));
					Scene0.renderProperties.useBounding=2;
				}
				break;

				//Axes
				case MENU_VIEW_AXES:
				{
					if (!Scene0.showAxes)
					{ 
						CheckMenuItem(mainMenu,MENU_VIEW_AXES, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_AXES, MAKELONG(true, 0));
						Scene0.showAxes=true;

					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_AXES, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_AXES, MAKELONG(false, 0));
						Scene0.showAxes=false;
					}
				}
				break;


				//Grid
				case MENU_VIEW_GRID:
				{
					if (!Scene0.showGrid)
					{ 
						CheckMenuItem(mainMenu,MENU_VIEW_GRID, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_GRID, MAKELONG(true, 0));
						Scene0.showGrid=true;

					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_GRID, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_GRID, MAKELONG(false, 0));
						Scene0.showGrid=false;
					}
				}
				break;


				//Informations
				case MENU_VIEW_INFO:
				{
					if (!Scene0.showInfo)
				    { 
						CheckMenuItem(mainMenu,MENU_VIEW_INFO, MF_CHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_INFO, MAKELONG(true, 0));
						Scene0.showInfo=true;
					}
					else
					{
						CheckMenuItem(mainMenu,MENU_VIEW_INFO, MF_UNCHECKED);
					    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_VIEW_INFO, MAKELONG(false, 0));
						Scene0.showInfo=false;
					}
				}
				break;




				//* * * * * * * * * * * * * *
				//* * *  MENU UTILITIES * * * 
				//* * * * * * * * * * * * * *
				//Capture screenshot
				case MENU_UTILITIES_CAPTURE:
				{
					if (Engine.SaveScreenshot(NULL,0))
					{
						MessageBox(hWnd,"Screenshot saved successfully.", "Capture Screenshot", MB_OK|MB_ICONINFORMATION);
					}
					else
					{
						MessageBox(hWnd,"Error during save screenshot.", "Capture Screenshot", MB_OK|MB_ICONWARNING);
					}
				}
				break;



				//* * * * * * * * * * * *
				//* * *  MENU ABOUT * * * 
				//* * * * * * * * * * * *
				//Help
				case MENU_HELP_HELP:
				{
					//Esegue il file della guida in linea di windows
					ShellExecute(hWnd, "open", "hh.exe", "Desdinova Modeler.chm", NULL, SW_SHOWMAXIMIZED  );
				}
				break;		
				//URL Pagina web
				case MENU_HELP_SITE:
				{
					HlinkSimpleNavigateToString(L"http://www.desdinova.altervista.org",NULL,  NULL, NULL, NULL, NULL, 0, 0);
				}
				break;				
				//About
				case MENU_HELP_ABOUT:
				{
					EnableWindow(hWnd,false);
					HWND hWndAbout=CreateDialog(Engine.GetInstance(), (LPCTSTR)DIALOG_ABOUT,hWnd,(DLGPROC)AboutProc);
					ShowWindow(hWndAbout,SW_SHOW);
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


#endif