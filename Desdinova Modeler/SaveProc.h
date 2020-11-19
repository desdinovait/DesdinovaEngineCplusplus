#ifndef H_SAVEPROC_H
#define H_SAVEPROC_H


LRESULT CALLBACK SaveProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	int wmId;
	int wmEvent;


	switch (message)
	{

		case WM_INITDIALOG:
		{
			if (Scene0.currentCreationMode==1)
			{
				SetWindowText(hWnd,"Save Model");
				SetDlgItemText(hWnd, TXT_SAVE_NAME, Scene0.Model.GetName());
			}
			if (Scene0.currentCreationMode==2)
			{
				SetWindowText(hWnd,"Save Entity");
				SetDlgItemText(hWnd, TXT_SAVE_NAME, Scene0.Entity.GetName());
			}
			if (Scene0.currentCreationMode==3)
			{
				SetWindowText(hWnd,"Save Particles System");
				SetDlgItemText(hWnd, TXT_SAVE_NAME, Scene0.ParticleSystems.GetName());
			}
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
				//** MODEL **
				case BUT_SAVE_SAVE: 
				{
					//Viene salvato il modello
					if (Scene0.currentCreationMode==1)
					{
						TCHAR strMsg[512];
						_tcscpy( strMsg, _T("Modify .fx file to Save."));
						MessageBox(hWnd,strMsg,"Models Manager",MB_ICONWARNING|MB_OK);
					}//*/

					//Viene salvata l'entity
					if (Scene0.currentCreationMode==2)
					{
						char	saveNameEntity[MAX_PATH];
						strcpy(saveNameEntity, "Entities\\");
						char bufferCurrentNameEntity[MAX_PATH];
						GetDlgItemText(hWnd,TXT_SAVE_NAME,bufferCurrentNameEntity,MAX_PATH);
						strcat(saveNameEntity, bufferCurrentNameEntity);
						strcat(saveNameEntity, ".ess");
						char	saveNameEntityComplete[MAX_PATH];

						strcpy(saveNameEntityComplete, DEUtility::GetCurrentPath());
						strcat(saveNameEntityComplete, saveNameEntity);

						WIN32_FIND_DATA FindFileData;
						HANDLE hFind = FindFirstFile(saveNameEntityComplete, &FindFileData);
						if (hFind == INVALID_HANDLE_VALUE)
						{
							//Salta qui se si vuole sovrascrivere il file
							writeFileEntity:

							//Messaggio di creazione script
							SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Creating script files"); 	

							//Scrive il file dell'entità (.ESS)
							if (Scene0.Entity.Save(saveNameEntity))
							{
								TCHAR strMsg[512];
								_tcscpy( strMsg, _T("Current entity saved."));
								MessageBox(hWnd,strMsg,"Entities Manager",MB_ICONWARNING|MB_OK);
							}
							else
							{
								TCHAR strMsg[512];
								_tcscpy( strMsg, _T("Error during saving entity file.\n")
													_T("Please check filename, extension or location."));
								MessageBox(hWnd,strMsg,"Entities Manager Error",MB_ICONWARNING|MB_OK);
							}//*/
						
							//Messaggio di fine creazione script
							SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Save complete"); 	

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
							strncpy(newMainTitle, currentMainTitle, result1-2 );
							strcat(newMainTitle, " - [Entity : ");
							strcat(newMainTitle, Scene0.Entity.GetName());
							strcat(newMainTitle,"]");
							SetWindowText(hWndMain, newMainTitle);
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("Current file already exist.      \n")
											_T("Do you want to replace it?      "));
							if (MessageBox(hWnd,strMsg,"Save Entity",MB_ICONEXCLAMATION|MB_OKCANCEL)==IDOK)
							{
								goto writeFileEntity;
							}
						}
					}



					//Viene salvato il sistema particellare
					if (Scene0.currentCreationMode==3)
					{
						char	saveNameParticle[MAX_PATH];
						strcpy(saveNameParticle, "ParticlesSystems\\");
						char bufferCurrentNameParticle[MAX_PATH];
						GetDlgItemText(hWnd,TXT_SAVE_NAME,bufferCurrentNameParticle,MAX_PATH);
						strcat(saveNameParticle, bufferCurrentNameParticle);
						strcat(saveNameParticle, ".psy");
						char	saveNameParticleComplete[MAX_PATH];

						strcpy(saveNameParticleComplete, DEUtility::GetCurrentPath());
						strcat(saveNameParticleComplete, saveNameParticle);

						WIN32_FIND_DATA FindFileData;
						HANDLE hFind = FindFirstFile(saveNameParticleComplete, &FindFileData);
						if (hFind == INVALID_HANDLE_VALUE)
						{
							//Salta qui se si vuole sovrascrivere il file
							writeFileParticle:

							//Messaggio di creazione script
							SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Creating script files"); 	

							//Scrive il file del sistema particlellare in base al nome della texture (.PSY)
							if (Scene0.ParticleSystems.Save(saveNameParticle))
							{
								TCHAR strMsg[512];
								_tcscpy( strMsg, _T("Current particles system saved."));
								MessageBox(hWnd,strMsg,"Particles System Manager",MB_ICONWARNING|MB_OK);
							}
							else
							{
								TCHAR strMsg[512];
								_tcscpy( strMsg, _T("Error during saving particles system file.\n")
													_T("Please check filename, extension or location."));
								MessageBox(hWnd,strMsg,"Particles System Manager Error",MB_ICONWARNING|MB_OK);
							}//*/
						
							//Messaggio di fine creazione script
							SendMessage(hWndStatusbar, SB_SETTEXT , (WPARAM) 0, (LPARAM)"Save complete"); 						

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
							strncpy(newMainTitle, currentMainTitle, result1-2 );
							strcat(newMainTitle, " - [Particles System : ");
							strcat(newMainTitle, Scene0.ParticleSystems.GetName());
							strcat(newMainTitle,"]");
							SetWindowText(hWndMain, newMainTitle);
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("Current file already exist.      \n")
											_T("Do you want to replace it?      "));
							if (MessageBox(hWnd,strMsg,"Save Particles System",MB_ICONEXCLAMATION|MB_OKCANCEL)==IDOK)
							{
								goto writeFileParticle;
							}
						}
					}

					//Chiude la schermata
					SendMessage(hWnd,WM_CLOSE,0,0);
				}
				break;		

				//** CANCEL **
				case BUT_SAVE_CANCEL: 
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