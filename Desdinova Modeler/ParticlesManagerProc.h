#ifndef H_PARTICLESMANAGERPROC_H
#define H_PARTICLESMANAGERPROC_H


LRESULT CALLBACK ParticlesManagerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	int wmId;
	int wmEvent;
	
	//MSS Child Parameters
	OPENFILENAME openParamsChild = {0}; 
	char file_nameChild[BUFF_MAX] = {0};
	char file_nameChildTitle[BUFF_MAX] = {0};
	char dialogFilterChild[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(dialogFilterChild,"Child Files");
	int indexFilterChild = (int)strlen(dialogFilterChild) + 1;
	dialogFilterChild[indexFilterChild++] = '*';
	dialogFilterChild[indexFilterChild++] = '.';
	dialogFilterChild[indexFilterChild++] = 'm';
	dialogFilterChild[indexFilterChild++] = 's';
	dialogFilterChild[indexFilterChild++] = 's';

	//IMAGE Parameters New
	OPENFILENAME newParamsIMAGE = {0}; 
	char newIMAGE[BUFF_MAX] = {0};
	char newIMAGETitle[BUFF_MAX] = {0};
	char newFilterIMAGE[BUFF_MAX] = {0};

	switch (message)
	{
		case WM_INITDIALOG:
		{
			hWndParticlesManager=hWnd;
			RECT windowRectMain;
			GetWindowRect( Engine.GetHWND(),&windowRectMain);
			SetWindowPos(hWnd,HWND_TOP,windowRectMain.right,windowRectMain.top,NULL,NULL,SWP_NOSIZE);
			ShowWindow(hWnd,SW_SHOW);

			SetDlgItemText(hWnd, LAB_PAR_TEXTURE, Scene0.ParticleSystems.GetTextureName());

			char bufferInit[10]="";
			D3DXVECTOR3 currentPos = Scene0.ParticleSystems.GetPosition();
			D3DXVECTOR3 currentVel = Scene0.ParticleSystems.GetVelocity();
			D3DXVECTOR3 currentGrav = Scene0.ParticleSystems.GetGravity();
			D3DXVECTOR3 currentWind = Scene0.ParticleSystems.GetWind();
			D3DXVECTOR3 currentDistance = Scene0.ParticleSystems.GetDistance();
			D3DXCOLOR	currentColor = Scene0.ParticleSystems.GetColor();
			sprintf(bufferInit,"%.4f",currentPos.x);
			SetDlgItemText(hWnd, TXT_PAR_POSX, bufferInit);
			sprintf(bufferInit,"%.4f",currentPos.y);
			SetDlgItemText(hWnd, TXT_PAR_POSY, bufferInit);
			sprintf(bufferInit,"%.4f",currentPos.z);
			SetDlgItemText(hWnd, TXT_PAR_POSZ, bufferInit);
			sprintf(bufferInit,"%.4f",currentVel.x);
			SetDlgItemText(hWnd, TXT_PAR_VELX, bufferInit);
			sprintf(bufferInit,"%.4f",currentVel.y);
			SetDlgItemText(hWnd, TXT_PAR_VELY, bufferInit);
			sprintf(bufferInit,"%.4f",currentVel.z);
			SetDlgItemText(hWnd, TXT_PAR_VELZ, bufferInit);
			sprintf(bufferInit,"%.4f",currentGrav.x);
			SetDlgItemText(hWnd, TXT_PAR_GRAVX, bufferInit);
			sprintf(bufferInit,"%.4f",currentGrav.y);
			SetDlgItemText(hWnd, TXT_PAR_GRAVY, bufferInit);
			sprintf(bufferInit,"%.4f",currentGrav.z);
			SetDlgItemText(hWnd, TXT_PAR_GRAVZ, bufferInit);
			sprintf(bufferInit,"%.4f",currentWind.x);
			SetDlgItemText(hWnd, TXT_PAR_WINDX, bufferInit);
			sprintf(bufferInit,"%.4f",currentWind.y);
			SetDlgItemText(hWnd, TXT_PAR_WINDY, bufferInit);
			sprintf(bufferInit,"%.4f",currentWind.z);
			SetDlgItemText(hWnd, TXT_PAR_WINDZ, bufferInit);
			sprintf(bufferInit,"%.3f",currentColor.r);
			SetDlgItemText(hWnd, TXT_PAR_R, bufferInit);
			sprintf(bufferInit,"%.4f",currentDistance.x);
			SetDlgItemText(hWnd, TXT_PAR_DISTANCEX, bufferInit);
			sprintf(bufferInit,"%.4f",currentDistance.y);
			SetDlgItemText(hWnd, TXT_PAR_DISTANCEY, bufferInit);
			sprintf(bufferInit,"%.4f",currentDistance.z);
			SetDlgItemText(hWnd, TXT_PAR_DISTANCEZ, bufferInit);
			sprintf(bufferInit,"%.3f",currentColor.g);
			SetDlgItemText(hWnd, TXT_PAR_G, bufferInit);
			sprintf(bufferInit,"%.3f",currentColor.b);
			SetDlgItemText(hWnd, TXT_PAR_B, bufferInit);
			sprintf(bufferInit,"%.3f",currentColor.a);
			SetDlgItemText(hWnd, TXT_PAR_A, bufferInit);


			sprintf(bufferInit,"%.0f",(float)Scene0.ParticleSystems.GetMaxParticles());
			SetDlgItemText(hWnd, TXT_PAR_MAX, bufferInit);
			sprintf(bufferInit,"%.0f",(float)Scene0.ParticleSystems.GetNumToRelease());
			SetDlgItemText(hWnd, TXT_PAR_RELEASE, bufferInit);
			sprintf(bufferInit,"%.4f",(float)Scene0.ParticleSystems.GetReleaseInterval());
			SetDlgItemText(hWnd, TXT_PAR_INTERVAL, bufferInit);
			sprintf(bufferInit,"%.4f",(float)Scene0.ParticleSystems.GetLifeCycle());
			SetDlgItemText(hWnd, TXT_PAR_LIFE, bufferInit);
			sprintf(bufferInit,"%.4f",(float)Scene0.ParticleSystems.GetSize());
			SetDlgItemText(hWnd, TXT_PAR_SIZE, bufferInit);
			sprintf(bufferInit,"%.4f",(float)Scene0.ParticleSystems.GetVelocityVar());
			SetDlgItemText(hWnd, TXT_PAR_VELOCITY, bufferInit);
			if (Scene0.ParticleSystems.GetAirResistence())
			{
				CheckDlgButton(hWnd,CHK_PAR_RESISTENCE, BST_CHECKED);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDX),true);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDY),true);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDZ),true);
			}
			else
			{
				CheckDlgButton(hWnd,CHK_PAR_RESISTENCE, BST_UNCHECKED);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDX),false);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDY),false);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDZ),false);
			}

			if (Scene0.ParticleSystems.GetRelativeY())
			{
				CheckDlgButton(hWnd,CHK_PAR_RELATIVEY, BST_CHECKED);
			}
			else
			{
				CheckDlgButton(hWnd,CHK_PAR_RELATIVEY, BST_UNCHECKED);
			}

			HWND hWndComboType=GetDlgItem(hWnd,CB_PAR_TYPE);
			SendMessage(hWndComboType, CB_RESETCONTENT, 0, 0);
			SendMessage(hWndComboType, CB_ADDSTRING, 0, (LPARAM)"Point"); 
			SendMessage(hWndComboType, CB_ADDSTRING, 0, (LPARAM)"Cube"); 
			SendMessage(hWndComboType, CB_ADDSTRING, 0, (LPARAM)"Circle"); 
			
			ENUM_DE_PSTYPE currentType = Scene0.ParticleSystems.GetType();
			if (currentType == DE_PSTYPE_POINT)
			{
				SendMessage(hWndComboType, CB_SETCURSEL,0,0);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEX),false);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEY),false);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEZ),false);
				EnableWindow(GetDlgItem(hWnd,CHK_PAR_RELATIVEY),false);
			}
			if (currentType == DE_PSTYPE_CUBE)
			{
				SendMessage(hWndComboType, CB_SETCURSEL,1,0);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEX),true);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEY),true);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEZ),true);
				EnableWindow(GetDlgItem(hWnd,CHK_PAR_RELATIVEY),true);
			}
			if (currentType == DE_PSTYPE_CIRCLE)
			{
				SendMessage(hWndComboType, CB_SETCURSEL,2,0);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEX),true);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEY),true);
				EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEZ),true);
				EnableWindow(GetDlgItem(hWnd,CHK_PAR_RELATIVEY),true);
			}

		}
		break;


		case WM_CLOSE:
		{
			// If we close the dialog box
		    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_PARTICLESMANAGER, MAKELONG(false, 0));
			Scene0.showParticlesManager=false;
			hWndParticlesManager=NULL;
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
			
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				//** CURRENT TYPE **
				case CB_PAR_TYPE: 
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:

							if (SendMessage(GetDlgItem(hWnd,CB_PAR_TYPE), CB_GETCURSEL,0,0)==0)
							{
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEX),false);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEY),false);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEZ),false);
								EnableWindow(GetDlgItem(hWnd,CHK_PAR_RELATIVEY),false);
							}
							else
							{
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEX),true);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEY),true);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_DISTANCEZ),true);
								EnableWindow(GetDlgItem(hWnd,CHK_PAR_RELATIVEY),true);
							}
						break;
					}break;
				}break;


				//** CURRENT TYPE **
				case CHK_PAR_RESISTENCE: 
				{
					switch (HIWORD(wParam))
					{
						case BN_CLICKED:

							if (IsDlgButtonChecked(hWnd, CHK_PAR_RESISTENCE)==BST_UNCHECKED)
							{
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDX),false);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDY),false);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDZ),false);
							}
							else
							{
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDX),true);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDY),true);
								EnableWindow(GetDlgItem(hWnd,TXT_PAR_WINDZ),true);
							}
						break;
					}break;
				}break;

				case BUT_PARTICLES_LOAD:
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
					newParamsIMAGE.lpstrTitle = "Select new Particles System image file...";
					newParamsIMAGE.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
					
					if(GetOpenFileName(&newParamsIMAGE))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(newParamsIMAGE.lpstrFile))
						{
							char *openFilenameParticles;
							openFilenameParticles = (char *)malloc(MAX_PATH);
							memset(openFilenameParticles, 0, sizeof(char) * MAX_PATH);
							openFilenameParticles = DEUtility::NoRoot(newParamsIMAGE.lpstrFile);
						
							Scene0.ParticleSystems.SetTexture(openFilenameParticles);

							SetDlgItemText(hWnd, LAB_PAR_TEXTURE, Scene0.ParticleSystems.GetTextureName());

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



				case BUT_PARTICLES_COLOR:
				{
					CHOOSECOLOR  colorParams = {0};
					static COLORREF acrCustClr[16]; //array of custom colors 
					static DWORD rgbCurrent;        //initial color selection
					
					ZeroMemory(&colorParams, sizeof(CHOOSECOLOR));
					colorParams.lStructSize = sizeof(CHOOSECOLOR);
					colorParams.hwndOwner = hWnd;
					colorParams.lpCustColors = (LPDWORD) acrCustClr;
					colorParams.rgbResult = rgbCurrent;
					colorParams.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
					if (ChooseColor(&colorParams))
					{
						float colorR = GetRValue(colorParams.rgbResult);
						float colorG = GetGValue(colorParams.rgbResult);
						float colorB = GetBValue(colorParams.rgbResult);
						
						char bufferColor[10]="";
						sprintf(bufferColor,"%.3f",colorR/255);
						SetDlgItemText(hWnd, TXT_PAR_R, bufferColor);
						sprintf(bufferColor,"%.3f",colorG/255);
						SetDlgItemText(hWnd, TXT_PAR_G, bufferColor);
						sprintf(bufferColor,"%.3f",colorB/255);
						SetDlgItemText(hWnd, TXT_PAR_B, bufferColor);
					}
				}
				break;




				case BUT_PARTICLES_SET:
				{									
					if(IsDlgButtonChecked(hWnd, CHK_PAR_RESISTENCE))	Scene0.ParticleSystems.SetAirResistence(true);
					else												Scene0.ParticleSystems.SetAirResistence(false);

					//Properties		
					LPSTR ParticlesProp={0};
					ParticlesProp=(LPSTR)malloc(10);
					GetDlgItemText(hWnd, TXT_PAR_MAX, ParticlesProp, 10);
					Scene0.ParticleSystems.SetMaxParticles((float)(atof(ParticlesProp)));
					GetDlgItemText(hWnd, TXT_PAR_RELEASE, ParticlesProp, 10);
					Scene0.ParticleSystems.SetNumToRelease((float)(atof(ParticlesProp)));
					GetDlgItemText(hWnd, TXT_PAR_INTERVAL, ParticlesProp, 10);
					Scene0.ParticleSystems.SetReleaseInterval((float)(atof(ParticlesProp)));
					GetDlgItemText(hWnd, TXT_PAR_LIFE, ParticlesProp, 10);
					Scene0.ParticleSystems.SetLifeCycle((float)(atof(ParticlesProp)));
					GetDlgItemText(hWnd, TXT_PAR_SIZE, ParticlesProp, 10);
					Scene0.ParticleSystems.SetSize((float)(atof(ParticlesProp)));
					GetDlgItemText(hWnd, TXT_PAR_VELOCITY, ParticlesProp, 10);
					Scene0.ParticleSystems.SetVelocityVar((float)(atof(ParticlesProp)));

					D3DXVECTOR3 currentPos;
					D3DXVECTOR3 currentVel;
					D3DXVECTOR3 currentGrav;
					D3DXVECTOR3 currentWind;
					D3DXCOLOR	currentColor;
					D3DXVECTOR3	currentDistance;
					GetDlgItemText(hWnd, TXT_PAR_POSX, ParticlesProp, 10);
					currentPos.x = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_POSY, ParticlesProp, 10);
					currentPos.y = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_POSZ, ParticlesProp, 10);
					currentPos.z = (float)(atof(ParticlesProp));
					Scene0.ParticleSystems.SetPosition(currentPos);

					GetDlgItemText(hWnd, TXT_PAR_VELX, ParticlesProp, 10);
					currentVel.x = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_VELY, ParticlesProp, 10);
					currentVel.y = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_VELZ, ParticlesProp, 10);
					currentVel.z = (float)(atof(ParticlesProp));
					Scene0.ParticleSystems.SetVelocity(currentVel);

					GetDlgItemText(hWnd, TXT_PAR_GRAVX, ParticlesProp, 10);
					currentGrav.x = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_GRAVY, ParticlesProp, 10);
					currentGrav.y = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_GRAVZ, ParticlesProp, 10);
					currentGrav.z = (float)(atof(ParticlesProp));
					Scene0.ParticleSystems.SetGravity(currentGrav);

					GetDlgItemText(hWnd, TXT_PAR_WINDX, ParticlesProp, 10);
					currentWind.x = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_WINDY, ParticlesProp, 10);
					currentWind.y = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_WINDZ, ParticlesProp, 10);
					currentWind.z = (float)(atof(ParticlesProp));
					Scene0.ParticleSystems.SetWind(currentWind);

					GetDlgItemText(hWnd, TXT_PAR_R, ParticlesProp, 10);
					currentColor.r = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_G, ParticlesProp, 10);
					currentColor.g = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_B, ParticlesProp, 10);
					currentColor.b = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_A, ParticlesProp, 10);
					currentColor.a = (float)(atof(ParticlesProp));
					Scene0.ParticleSystems.SetColor(currentColor);

					HWND hWndComboType=GetDlgItem(hWnd,CB_PAR_TYPE);
					if (SendMessage(hWndComboType, CB_GETCURSEL,0,0) == 0) Scene0.ParticleSystems.SetType(DE_PSTYPE_POINT);
					if (SendMessage(hWndComboType, CB_GETCURSEL,0,0) == 1) Scene0.ParticleSystems.SetType(DE_PSTYPE_CUBE);
					if (SendMessage(hWndComboType, CB_GETCURSEL,0,0) == 2) Scene0.ParticleSystems.SetType(DE_PSTYPE_CIRCLE);

					GetDlgItemText(hWnd, TXT_PAR_DISTANCEX, ParticlesProp, 10);
					currentDistance.x = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_DISTANCEY, ParticlesProp, 10);
					currentDistance.y = (float)(atof(ParticlesProp));
					GetDlgItemText(hWnd, TXT_PAR_DISTANCEZ, ParticlesProp, 10);
					currentDistance.z = (float)(atof(ParticlesProp));
					Scene0.ParticleSystems.SetDistance(currentDistance);

					if(IsDlgButtonChecked(hWnd, CHK_PAR_RELATIVEY))	Scene0.ParticleSystems.SetRelativeY(true);
					else											Scene0.ParticleSystems.SetRelativeY(false);

					free(ParticlesProp);

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