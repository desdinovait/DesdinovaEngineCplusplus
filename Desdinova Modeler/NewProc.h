#ifndef H_NEWPROC_H
#define H_NEWPROC_H


LRESULT CALLBACK NewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	int wmId;
	int wmEvent;


	//Texture Parameters New
	OPENFILENAME newParamsTEXTURE = {0}; 
	char newTEXTURE[BUFF_MAX] = {0};
	char newTEXTURETitle[BUFF_MAX] = {0};
	char newFilterTEXTURE[BUFF_MAX] = {0};

	//FX Parameters New
	OPENFILENAME newParamsFX = {0}; 
	char newFX[BUFF_MAX] = {0};
	char newFXTitle[BUFF_MAX] = {0};
	char newFilterFX[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(newFilterFX,"Model FX File (*.fx)");
	int newindexFilterFX = (int)strlen(newFilterFX) + 1;
	newFilterFX[newindexFilterFX++] = '*';
	newFilterFX[newindexFilterFX++] = '.';
	newFilterFX[newindexFilterFX++] = 'f';
	newFilterFX[newindexFilterFX++] = 'x';

	//IMAGE Parameters New
	OPENFILENAME newParamsIMAGE = {0}; 
	char newIMAGE[BUFF_MAX] = {0};
	char newIMAGETitle[BUFF_MAX] = {0};
	char newFilterIMAGE[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(newFilterIMAGE,"Model IMAGE File (*.IMAGE)");
	int newindexFilterIMAGE = (int)strlen(newFilterIMAGE) + 1;
	newFilterIMAGE[newindexFilterIMAGE++] = '*';
	newFilterIMAGE[newindexFilterIMAGE++] = '.';
	newFilterIMAGE[newindexFilterIMAGE++] = 'm';
	newFilterIMAGE[newindexFilterIMAGE++] = 's';
	newFilterIMAGE[newindexFilterIMAGE++] = 's';

	switch (message)
	{

		case WM_INITDIALOG:
		{
		}
		break;


		case WM_CLOSE:
		{
			EndDialog(hWnd, LOWORD(wParam));
			hWndNew = NULL;
			return TRUE;
		}
		break;
			

		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{

				//** ENTITY **
				case BUT_NEW_ENTITY: 
				{
					ZeroMemory(&newParamsFX, sizeof(OPENFILENAME));
					newParamsFX.lStructSize = sizeof(OPENFILENAME);
					newParamsFX.hwndOwner = hWndMain; 
					newParamsFX.lpstrFilter = newFilterFX;	
					newParamsFX.lpstrFileTitle = newFXTitle;				
					newParamsFX.lpstrFile = newFX;				
					newParamsFX.nMaxFile = BUFF_MAX;
					newParamsFX.nMaxFileTitle = BUFF_MAX;
					newParamsFX.lpstrInitialDir = "Effects"; 
					newParamsFX.lpstrTitle = "Create new Entity from FX parent file...";
					newParamsFX.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
					
					if(GetOpenFileName(&newParamsFX))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(newParamsFX.lpstrFile))
						{
							char *openFilenameEntity;
							openFilenameEntity = (char *)malloc(MAX_PATH);
							memset(openFilenameEntity, 0, sizeof(char) * MAX_PATH);
							openFilenameEntity = DEUtility::NoRoot(newParamsFX.lpstrFile);

							//Manda il messaggio per la chiusura della sessione
							SendMessage(hWndMain,WM_COMMAND,MENU_FILE_CLOSE,0);

							//Se è stato chiuso tutto allora procede
							if (Scene0.currentCreationMode==0)
							{
								//Caricamento
								SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Loading..."); 

								//Carica il modello tramite il file FX
								Scene0.Entity.Create("Entity");
								if (Scene0.Entity.AddParent(openFilenameEntity))
								{
									//Mostra l'avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load complete"); 

									//Aggiorna il ModelsManager con i nuovi modelli caricati (se visibile)
									//Utilizza l'inizializzazione della schermata del Models Manager
									SendMessage(hWndEntitiesManager, WM_INITDIALOG, 0, 0);
									//Chiude la schermata dei modelli se aperta
									SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(false, 0));
									Scene0.showModelsManager=false;
									SendMessage(hWndModelsManager,WM_CLOSE,0,0);//Chiude la finestra(vedere WM_CLOSE)
									//Chiude la schermata del sistema particellare se aperta
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
									MessageBox(NULL,"Error during loading FX file","Loading Error",MB_OK);
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
				case BUT_NEW_PAR: 
				{

					ZeroMemory(&newParamsIMAGE, sizeof(OPENFILENAME));
					newParamsIMAGE.lStructSize = sizeof(OPENFILENAME);
					newParamsIMAGE.hwndOwner = hWndMain; 
					newParamsIMAGE.lpstrFilter = "All image files\0*.bmp;*.jpg;*.dds;*.dib;*.png;*.tga\0Windows or OS/2 Bitmap (*.bmp)\0*.bmp\0JPEG Compliant (*.jpg)\0*.jpg\0DirectDraw sufrace (*.dds)\0*.dds\0Windows or OS/2 DIB (*.dib)\0*.dib\0Portable Network Graphics (*.png)\0*.png\0Truevision Targa (*.tga)\0*.tga\0";	
					newParamsIMAGE.lpstrFileTitle = newIMAGETitle;				
					newParamsIMAGE.lpstrFile = newIMAGE;				
					newParamsIMAGE.nMaxFile = BUFF_MAX;
					newParamsIMAGE.nMaxFileTitle = BUFF_MAX;
					newParamsIMAGE.lpstrInitialDir = "Textures"; 
					newParamsIMAGE.lpstrTitle = "Create new Particles System from image file...";
					newParamsIMAGE.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
					
					if (GetOpenFileName(&newParamsIMAGE))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(newParamsIMAGE.lpstrFile))
						{
							char *openFilenameParticles;
							openFilenameParticles = (char *)malloc(MAX_PATH);
							memset(openFilenameParticles, 0, sizeof(char) * MAX_PATH);
							openFilenameParticles = DEUtility::NoRoot(newParamsIMAGE.lpstrFile);

							//Manda il messaggio per la chiusura della sessione
							SendMessage(hWndMain,WM_COMMAND,MENU_FILE_CLOSE,0);

							//Se è stato chiuso tutto allora procede
							if (Scene0.currentCreationMode==0)
							{
								//Caricamento
								SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Loading..."); 

								Scene0.ParticleSystems.Create("ParticleSystems");
								if(Scene0.ParticleSystems.New(openFilenameParticles))
								{							
									//Mostra l'avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load complete"); 

									//Aggiorna il ModelsManager con i nuovi modelli caricati (se visibile)
									//Utilizza l'inizializzazione della schermata del Models Manager
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
									Scene0.currentCreationMode = 3;//Particles

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
									MessageBox(NULL,"Error during creating particles system...","Loading Error",MB_OK);
									//Mostra il NON avvenuto caricamento
									SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Load failed"); 
								}//*/
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
				case BUT_NEW_CANCEL: 
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