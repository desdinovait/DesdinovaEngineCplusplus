#ifndef H_OPENPROC_H
#define H_OPENPROC_H

LRESULT CALLBACK OpenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	int wmId;
	int wmEvent;


	//MSS Parameters OPEN
	OPENFILENAME openParamsFX = {0}; 
	char openFX[BUFF_MAX] = {0};
	char openFXTitle[BUFF_MAX] = {0};
	char openFilterFX[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(openFilterFX,"Model FX File (*.fx)");
	int openindexFilterFX = (int)strlen(openFilterFX) + 1;
	openFilterFX[openindexFilterFX++] = '*';
	openFilterFX[openindexFilterFX++] = '.';
	openFilterFX[openindexFilterFX++] = 'f';
	openFilterFX[openindexFilterFX++] = 'x';

	//ESS Parameters OPEN
	OPENFILENAME openParamsESS = {0}; 
	char openESS[BUFF_MAX] = {0};
	char openESSTitle[BUFF_MAX] = {0};
	char openFilterESS[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(openFilterESS,"Entity ESS File (*.ess)");
	int openindexFilterESS = (int)strlen(openFilterESS) + 1;
	openFilterESS[openindexFilterESS++] = '*';
	openFilterESS[openindexFilterESS++] = '.';
	openFilterESS[openindexFilterESS++] = 'e';
	openFilterESS[openindexFilterESS++] = 's';
	openFilterESS[openindexFilterESS++] = 's';


	//PSY Parameters OPEN
	OPENFILENAME openParamsPSY = {0}; 
	char openPSY[BUFF_MAX] = {0};
	char openPSYTitle[BUFF_MAX] = {0};
	char openFilterPSY[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(openFilterPSY,"Particles System PSY File (*.psy)");
	int openindexFilterPSY = (int)strlen(openFilterPSY) + 1;
	openFilterPSY[openindexFilterPSY++] = '*';
	openFilterPSY[openindexFilterPSY++] = '.';
	openFilterPSY[openindexFilterPSY++] = 'p';
	openFilterPSY[openindexFilterPSY++] = 's';
	openFilterPSY[openindexFilterPSY++] = 'y';



	switch (message)
	{
		case WM_INITDIALOG:
		{


		}
		break;


		case WM_CLOSE:
		{
			EndDialog(hWnd, LOWORD(wParam));
			hWndOpen = NULL;
			return TRUE;
		}
		break;
			
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			
			switch (wmId)
			{
				//** MODEL **
				case BUT_OPEN_MODEL: 
				{
					ZeroMemory(&openParamsFX, sizeof(OPENFILENAME));
					openParamsFX.lStructSize = sizeof(OPENFILENAME);
					openParamsFX.hwndOwner = hWndMain; 
					openParamsFX.lpstrFilter = openFilterFX;	
					openParamsFX.lpstrFileTitle = openFXTitle;				
					openParamsFX.lpstrFile = openFX;				
					openParamsFX.nMaxFile = BUFF_MAX;
					openParamsFX.nMaxFileTitle = BUFF_MAX;
					openParamsFX.lpstrInitialDir = "Effects"; 
					openParamsFX.lpstrTitle = "Open Model from FX file...";
					openParamsFX.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
					
					if(GetOpenFileName(&openParamsFX))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(openParamsFX.lpstrFile))
						{
							char *openFilenameFX;
							openFilenameFX = (char *)malloc(MAX_PATH);
							memset(openFilenameFX, 0, sizeof(char) * MAX_PATH);
							openFilenameFX = DEUtility::NoRoot(openParamsFX.lpstrFile);

							//Manda il messaggio per la chiusura della sessione
							SendMessage(hWndMain,WM_COMMAND,MENU_FILE_CLOSE,0);

							//Se è stato chiuso tutto allora procede
							if (Scene0.currentCreationMode==0)
							{
								//Caricamento
								SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Loading..."); 

								char strLog[200];
								sprintf(strLog,"Model.Load[%s]",openFilenameFX);
								Scene0.Model.Create("AAA");						
								if (Scene0.Model.Load(openFilenameFX))
								{




									Scene0.Model.SetPhysicsScene(Scene0.GetScene());




									//Mostra l'avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load complete"); 

									//Aggiorna il ModelsManager con i nuovi modelli caricati (se visibile)
									//Utilizza l'inizializzazione della schermata del Models Manager
									SendMessage(hWndModelsManager, WM_INITDIALOG, 0, 0);
									//Chiude la schermata delle entità se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_ENTITIESMANAGER, MAKELONG(false, 0));
									Scene0.showEntitiesManager=false;
									SendMessage(hWndEntitiesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
									//Chiude la schermata del sistema particlellare se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_PARTICLESMANAGER, MAKELONG(false, 0));
									Scene0.showParticlesManager=false;
									SendMessage(hWndEntitiesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)

									//Flags apertura
									Scene0.opened = true;
									Scene0.currentCreationMode = 1;//Model

									//Chiude la schermata di apertura
									SendMessage(hWnd,WM_CLOSE,0,0);

									//Setta il titolo della schermata principale
									char currentMainTitle[MAX_PATH];
									char newMainTitle[MAX_PATH];
									memset(currentMainTitle, 0, sizeof(char) * MAX_PATH);
									memset(newMainTitle, 0, sizeof(char) * MAX_PATH);
									GetWindowText(hWndMain, currentMainTitle, GetWindowTextLength(hWndMain)+1);
									strcpy(newMainTitle, currentMainTitle);
									strcat(newMainTitle, " - [Model : ");
									strcat(newMainTitle, Scene0.Model.GetName());
									strcat(newMainTitle,"]");
									SetWindowText(hWndMain, newMainTitle);
								}
								else
								{
									MessageBox(NULL,"Error during loading model files","Loading Error",MB_OK);
									//Mostra il NON avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load failed"); 
								}
							}
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("Current file has got wrong path!\n")
												_T("All files must reside in root directory."));
							MessageBox(NULL, strMsg, "Loading Error", MB_ICONWARNING|MB_OK);
						}
					}
				}
				break;		

				//** ENTITY **
				case BUT_OPEN_ENTITY: 
				{
					ZeroMemory(&openParamsESS, sizeof(OPENFILENAME));
					openParamsESS.lStructSize = sizeof(OPENFILENAME);
					openParamsESS.hwndOwner = hWndMain; 
					openParamsESS.lpstrFilter = openFilterESS;	
					openParamsESS.lpstrFileTitle = openESSTitle;				
					openParamsESS.lpstrFile = openESS;				
					openParamsESS.nMaxFile = BUFF_MAX;
					openParamsESS.nMaxFileTitle = BUFF_MAX;
					openParamsESS.lpstrInitialDir = "Entities"; 
					openParamsESS.lpstrTitle = "Open Entity from ESS file...";
					openParamsESS.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
				
					if(GetOpenFileName(&openParamsESS))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(openParamsESS.lpstrFile))
						{
							char *openFilenameESS;
							openFilenameESS = (char *)malloc(MAX_PATH);
							memset(openFilenameESS, 0, sizeof(char) * MAX_PATH);
							openFilenameESS = DEUtility::NoRoot(openParamsESS.lpstrFile);
							
							//Manda il messaggio per la chiusura della sessione
							SendMessage(hWndMain,WM_COMMAND,MENU_FILE_CLOSE,0);

							//Se è stato chiuso tutto allora procede
							if (Scene0.currentCreationMode==0)
							{
								//Caricamento
								SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Loading..."); 

								//Carica il modello tramite il file ESS
								Scene0.Entity.Create("Entity");
								if (Scene0.Entity.Load(openFilenameESS))
								{
									//Mostra l'avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load complete"); 

									//Aggiorna il EntittiesManager con i nuovi modelli caricati (se visibile)
									//Utilizza l'inizializzazione della schermata del Entities Manager
									SendMessage(hWndEntitiesManager, WM_INITDIALOG, 0, 0);
									//Chiude la schermata dei modelli se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(false, 0));
									Scene0.showModelsManager=false;
									SendMessage(hWndModelsManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
									//Chiude la schermata del sistema particlellare se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_PARTICLESMANAGER, MAKELONG(false, 0));
									Scene0.showParticlesManager=false;
									SendMessage(hWndParticlesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)

									//Flags apertura
									Scene0.opened = true;
									Scene0.currentCreationMode = 2;//Entity

									//Chiude la schermata di apertura
									SendMessage(hWnd,WM_CLOSE,0,0);

									//Setta il titolo della schermata principale
									char currentMainTitle[MAX_PATH];
									char newMainTitle[MAX_PATH];
									memset(currentMainTitle, 0, sizeof(char) * MAX_PATH);
									memset(newMainTitle, 0, sizeof(char) * MAX_PATH);
									GetWindowText(hWndMain, currentMainTitle, GetWindowTextLength(hWndMain)+1);
									strcpy(newMainTitle, currentMainTitle);
									strcat(newMainTitle, " - [Entity : ");
									strcat(newMainTitle, Scene0.Entity.GetName());
									strcat(newMainTitle,"]");
									SetWindowText(hWndMain, newMainTitle);

								}
								else
								{
									MessageBox(NULL,"Error during loading ESS file","Loading Error",MB_OK);
									//Mostra il NON avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load failed"); 
								}
							}
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("Current file has got wrong path!\n")
												_T("All files must reside in root directory."));
							MessageBox(NULL, strMsg, "Loading Error", MB_ICONWARNING|MB_OK);
						}
					}
				}
				break;		

				//** PARTICLE SYSTEM **
				case BUT_OPEN_PAR: 
				{
					ZeroMemory(&openParamsPSY, sizeof(OPENFILENAME));
					openParamsPSY.lStructSize = sizeof(OPENFILENAME);
					openParamsPSY.hwndOwner = hWndMain; 
					openParamsPSY.lpstrFilter = openFilterPSY;	
					openParamsPSY.lpstrFileTitle = openPSYTitle;				
					openParamsPSY.lpstrFile = openPSY;				
					openParamsPSY.nMaxFile = BUFF_MAX;
					openParamsPSY.nMaxFileTitle = BUFF_MAX;
					openParamsPSY.lpstrInitialDir = "ParticlesSystems"; 
					openParamsPSY.lpstrTitle = "Open Particles System from PSY file...";
					openParamsPSY.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
					
					if(GetOpenFileName(&openParamsPSY))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(openParamsPSY.lpstrFile))
						{
							char *openFilenamePSY;
							openFilenamePSY = (char *)malloc(MAX_PATH);
							memset(openFilenamePSY, 0, sizeof(char) * MAX_PATH);
							openFilenamePSY = DEUtility::NoRoot(openParamsPSY.lpstrFile);

							//Manda il messaggio per la chiusura della sessione
							SendMessage(hWndMain,WM_COMMAND,MENU_FILE_CLOSE,0);

							//Se è stato chiuso tutto allora procede
							if (Scene0.currentCreationMode==0)
							{
								//Caricamento
								SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Loading..."); 

								//Carica il modello tramite il file PSY
								Scene0.ParticleSystems.Create("ParticleSystems");
								if (Scene0.ParticleSystems.Load(openFilenamePSY))
								{
									//Mostra l'avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load complete"); 

									//Aggiorna il EntittiesManager con i nuovi modelli caricati (se visibile)
									//Utilizza l'inizializzazione della schermata del Entities Manager
									SendMessage(hWndParticlesManager, WM_INITDIALOG, 0, 0);
									//Chiude la schermata dei modelli se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(false, 0));
									Scene0.showModelsManager=false;
									SendMessage(hWndModelsManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
									//Chiude la schermata delle entità se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_ENTITIESMANAGER, MAKELONG(false, 0));
									Scene0.showEntitiesManager=false;
									SendMessage(hWndEntitiesManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)

									//Flags apertura
									Scene0.opened = true;
									Scene0.currentCreationMode = 3;//Entity

									//Chiude la schermata di apertura
									SendMessage(hWnd,WM_CLOSE,0,0);

									//Setta il titolo della schermata principale
									char currentMainTitle[MAX_PATH];
									char newMainTitle[MAX_PATH];
									memset(currentMainTitle, 0, sizeof(char) * MAX_PATH);
									memset(newMainTitle, 0, sizeof(char) * MAX_PATH);
									GetWindowText(hWndMain, currentMainTitle, GetWindowTextLength(hWndMain)+1);
									strcpy(newMainTitle, currentMainTitle);
									strcat(newMainTitle, " - [Particles System : ");
									strcat(newMainTitle, Scene0.ParticleSystems.GetName());
									strcat(newMainTitle,"]");
									SetWindowText(hWndMain, newMainTitle);

								}
								else
								{
									MessageBox(NULL,"Error during loading PSY file","Loading Error",MB_OK);
									//Mostra il NON avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load failed"); 
								}
							}
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("Current file has got wrong path!\n")
												_T("All files must reside in root directory."));
							MessageBox(NULL, strMsg, "Loading Error", MB_ICONWARNING|MB_OK);
						}

					}
				}
				break;		

				//** CANCEL **
				case BUT_OPEN_CANCEL: 
				{
					SendMessage(hWnd,WM_CLOSE,0,0);
				}
				break;		
			}
			break;
		}
		break;

		



	}
    return FALSE;
}


#endif