#ifndef H_LIGHTSMANAGERPROC_H
#define H_LIGHTSMANAGERPROC_H

LRESULT CALLBACK LightsManagerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	int wmEvent;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			hWndLightsManager=hWnd;

			float currentPosX = Light[currentLight].d3dLight.Position.x;
			float currentPosY = Light[currentLight].d3dLight.Position.y;
			float currentPosZ = Light[currentLight].d3dLight.Position.z;
			float currentDiffuseR = Light[currentLight].d3dLight.Diffuse.r;
			float currentDiffuseG = Light[currentLight].d3dLight.Diffuse.g;
			float currentDiffuseB = Light[currentLight].d3dLight.Diffuse.b;
			float currentDiffuseA = Light[currentLight].d3dLight.Diffuse.a;
			float currentAmbientR = Light[currentLight].d3dLight.Ambient.r;
			float currentAmbientG = Light[currentLight].d3dLight.Ambient.g;
			float currentAmbientB = Light[currentLight].d3dLight.Ambient.b;
			float currentAmbientA = Light[currentLight].d3dLight.Ambient.a;
			float currentSpecularR = Light[currentLight].d3dLight.Specular.r;
			float currentSpecularG = Light[currentLight].d3dLight.Specular.g;
			float currentSpecularB = Light[currentLight].d3dLight.Specular.b;
			float currentSpecularA = Light[currentLight].d3dLight.Specular.a;
			float currentDirectionX = Light[currentLight].d3dLight.Direction.x;
			float currentDirectionY = Light[currentLight].d3dLight.Direction.y;
			float currentDirectionZ = Light[currentLight].d3dLight.Direction.z;
			float currentAttenuation0 = Light[currentLight].d3dLight.Attenuation0;
			float currentAttenuation1 = Light[currentLight].d3dLight.Attenuation1;
			float currentAttenuation2 = Light[currentLight].d3dLight.Attenuation2;
			float currentRange = Light[currentLight].d3dLight.Range;
			float currentFallOff = Light[currentLight].d3dLight.Falloff;
			float currentPhi = Light[currentLight].d3dLight.Phi;
			float currentTheta = Light[currentLight].d3dLight.Theta;
			bool  currentShowRange = Light[currentLight].showRange;
			bool  currentShowPosition = Light[currentLight].showPosition;

			char bufferParameters[50]="";
			sprintf(bufferParameters,"%.2f",currentPosX);
			SetDlgItemText(hWnd, TXT_LIGHT_POS_X, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentPosY);
			SetDlgItemText(hWnd, TXT_LIGHT_POS_Y, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentPosZ);
			SetDlgItemText(hWnd, TXT_LIGHT_POS_Z, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDiffuseR);
			SetDlgItemText(hWnd, TXT_LIGHT_DIFFUSE_R, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDiffuseG);
			SetDlgItemText(hWnd, TXT_LIGHT_DIFFUSE_G, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDiffuseB);
			SetDlgItemText(hWnd, TXT_LIGHT_DIFFUSE_B, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDiffuseA);
			SetDlgItemText(hWnd, TXT_LIGHT_DIFFUSE_A, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAmbientR);
			SetDlgItemText(hWnd, TXT_LIGHT_AMBIENT_R, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAmbientG);
			SetDlgItemText(hWnd, TXT_LIGHT_AMBIENT_G, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAmbientB);
			SetDlgItemText(hWnd, TXT_LIGHT_AMBIENT_B, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAmbientA);
			SetDlgItemText(hWnd, TXT_LIGHT_AMBIENT_A, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentSpecularR);
			SetDlgItemText(hWnd, TXT_LIGHT_SPECULAR_R, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentSpecularG);
			SetDlgItemText(hWnd, TXT_LIGHT_SPECULAR_G, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentSpecularB);
			SetDlgItemText(hWnd, TXT_LIGHT_SPECULAR_B, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentSpecularA);
			SetDlgItemText(hWnd, TXT_LIGHT_SPECULAR_A, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDirectionX);
			SetDlgItemText(hWnd, TXT_LIGHT_DIR_X, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDirectionY);
			SetDlgItemText(hWnd, TXT_LIGHT_DIR_Y, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentDirectionZ);
			SetDlgItemText(hWnd, TXT_LIGHT_DIR_Z, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAttenuation0);
			SetDlgItemText(hWnd, TXT_LIGHT_ATT_0, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAttenuation1);
			SetDlgItemText(hWnd, TXT_LIGHT_ATT_1, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentAttenuation2);
			SetDlgItemText(hWnd, TXT_LIGHT_ATT_2, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentRange);
			SetDlgItemText(hWnd, TXT_LIGHT_RANGE, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentFallOff);
			SetDlgItemText(hWnd, TXT_LIGHT_FALLOFF, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentPhi);
			SetDlgItemText(hWnd, TXT_LIGHT_PHI, bufferParameters);
			sprintf(bufferParameters,"%.2f",currentTheta);
			SetDlgItemText(hWnd, TXT_LIGHT_THETA, bufferParameters);
			if (currentShowRange)	CheckDlgButton(hWnd,CHK_LIGHT_RANGE, BST_CHECKED);
			else					CheckDlgButton(hWnd,CHK_LIGHT_RANGE, BST_UNCHECKED);
			if (currentShowPosition)	CheckDlgButton(hWnd,CHK_LIGHT_POS, BST_CHECKED);
			else						CheckDlgButton(hWnd,CHK_LIGHT_POS, BST_UNCHECKED);

		}
		break;

		case WM_CLOSE:										// If we close the dialog box
		    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_LIGHTSMANAGER, MAKELONG(false, 0));
			showLightsManager=false;
			hWndLightsManager=NULL;
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		break;
			
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			

			switch (wmId)
			{
				//Settaggio
				case BUT_LIGHT_SET:
				{
					//Position
					LPSTR LightPositionX={0};
					LPSTR LightPositionY={0};
					LPSTR LightPositionZ={0};
					LightPositionX=(LPSTR)malloc(5);
					LightPositionY=(LPSTR)malloc(5);
					LightPositionZ=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_POS_X,LightPositionX,5);
					GetDlgItemText(hWnd,TXT_LIGHT_POS_Y,LightPositionY,5);
					GetDlgItemText(hWnd,TXT_LIGHT_POS_Z,LightPositionZ,5);
					Light[currentLight].SetPosition((float)(atof(LightPositionX)),(float)(atof(LightPositionY)),(float)(atof(LightPositionZ)) );
					free(LightPositionX);
					free(LightPositionY);
					free(LightPositionZ);

					//Diffuse
					LPSTR LightDiffuseR={0};
					LPSTR LightDiffuseG={0};
					LPSTR LightDiffuseB={0};
					LPSTR LightDiffuseA={0};
					LightDiffuseR=(LPSTR)malloc(5);
					LightDiffuseG=(LPSTR)malloc(5);
					LightDiffuseB=(LPSTR)malloc(5);
					LightDiffuseA=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIFFUSE_R,LightDiffuseR,5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIFFUSE_G,LightDiffuseG,5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIFFUSE_B,LightDiffuseB,5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIFFUSE_A,LightDiffuseA,5);
					Light[currentLight].SetDiffuse((float)(atof(LightDiffuseR)),(float)(atof(LightDiffuseG)),(float)(atof(LightDiffuseB)),(float)(atof(LightDiffuseA)) );
					free(LightDiffuseR);
					free(LightDiffuseG);
					free(LightDiffuseB);
					free(LightDiffuseA);

					//Ambient
					LPSTR LightAmbientR={0};
					LPSTR LightAmbientG={0};
					LPSTR LightAmbientB={0};
					LPSTR LightAmbientA={0};
					LightAmbientR=(LPSTR)malloc(5);
					LightAmbientG=(LPSTR)malloc(5);
					LightAmbientB=(LPSTR)malloc(5);
					LightAmbientA=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_AMBIENT_R,LightAmbientR,5);
					GetDlgItemText(hWnd,TXT_LIGHT_AMBIENT_G,LightAmbientG,5);
					GetDlgItemText(hWnd,TXT_LIGHT_AMBIENT_B,LightAmbientB,5);
					GetDlgItemText(hWnd,TXT_LIGHT_AMBIENT_A,LightAmbientA,5);
					Light[currentLight].SetAmbient((float)(atof(LightAmbientR)),(float)(atof(LightAmbientG)),(float)(atof(LightAmbientB)),(float)(atof(LightAmbientA)) );
					free(LightAmbientR);
					free(LightAmbientG);
					free(LightAmbientB);
					free(LightAmbientA);

					//Specular
					LPSTR LightSpecularR={0};
					LPSTR LightSpecularG={0};
					LPSTR LightSpecularB={0};
					LPSTR LightSpecularA={0};
					LightSpecularR=(LPSTR)malloc(5);
					LightSpecularG=(LPSTR)malloc(5);
					LightSpecularB=(LPSTR)malloc(5);
					LightSpecularA=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_SPECULAR_R,LightSpecularR,5);
					GetDlgItemText(hWnd,TXT_LIGHT_SPECULAR_G,LightSpecularG,5);
					GetDlgItemText(hWnd,TXT_LIGHT_SPECULAR_B,LightSpecularB,5);
					GetDlgItemText(hWnd,TXT_LIGHT_SPECULAR_A,LightSpecularA,5);
					Light[currentLight].SetSpecular((float)(atof(LightSpecularR)),(float)(atof(LightSpecularG)),(float)(atof(LightSpecularB)),(float)(atof(LightSpecularA)) );
					free(LightSpecularR);
					free(LightSpecularG);
					free(LightSpecularB);
					free(LightSpecularA);

					//Direction
					LPSTR LightDirectionX={0};
					LPSTR LightDirectionY={0};
					LPSTR LightDirectionZ={0};
					LightDirectionX=(LPSTR)malloc(5);
					LightDirectionY=(LPSTR)malloc(5);
					LightDirectionZ=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIR_X,LightDirectionX,5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIR_Y,LightDirectionY,5);
					GetDlgItemText(hWnd,TXT_LIGHT_DIR_Z,LightDirectionZ,5);
					Light[currentLight].SetDirection((float)(atof(LightDirectionX)),(float)(atof(LightDirectionY)),(float)(atof(LightDirectionZ)) );
					free(LightDirectionX);
					free(LightDirectionY);
					free(LightDirectionZ);

					//Attenuation
					LPSTR LightAttenuation0={0};
					LPSTR LightAttenuation1={0};
					LPSTR LightAttenuation2={0};
					LightAttenuation0=(LPSTR)malloc(5);
					LightAttenuation1=(LPSTR)malloc(5);
					LightAttenuation2=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_ATT_0,LightAttenuation0,5);
					GetDlgItemText(hWnd,TXT_LIGHT_ATT_1,LightAttenuation1,5);
					GetDlgItemText(hWnd,TXT_LIGHT_ATT_2,LightAttenuation2,5);
					Light[currentLight].SetAttenuation((float)(atof(LightAttenuation0)),(float)(atof(LightAttenuation1)),(float)(atof(LightAttenuation2)) );
					free(LightAttenuation0);
					free(LightAttenuation1);
					free(LightAttenuation2);

					//Range
					LPSTR LightRange={0};
					LightRange=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_RANGE,LightRange,5);
					Light[currentLight].SetRange((float)(atof(LightRange)));
					free(LightRange);

					//FallOff
					LPSTR LightFalloff={0};
					LightFalloff=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_FALLOFF,LightFalloff,5);
					Light[currentLight].SetFalloff((float)(atof(LightFalloff)));
					free(LightFalloff);

					//Phi
					LPSTR LightPhi={0};
					LightPhi=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_PHI,LightPhi,5);
					Light[currentLight].SetPhi((float)(atof(LightPhi)));
					free(LightPhi);

					//Theta
					LPSTR LightTheta={0};
					LightTheta=(LPSTR)malloc(5);
					GetDlgItemText(hWnd,TXT_LIGHT_THETA,LightTheta,5);
					Light[currentLight].SetTheta((float)(atof(LightTheta)));
					free(LightTheta);

				}
				break;

				//Range
				case CHK_LIGHT_RANGE:
				{
					if(IsDlgButtonChecked(hWnd, CHK_LIGHT_RANGE))
					{
						Light[currentLight].showRange = true;
						CheckDlgButton(hWnd,CHK_LIGHT_RANGE, BST_CHECKED);
					}
					else
					{
						Light[currentLight].showRange = false;
						CheckDlgButton(hWnd,CHK_LIGHT_RANGE, BST_UNCHECKED);
					}
				}
				break;
				//Position
				case CHK_LIGHT_POS:
				{
					if(IsDlgButtonChecked(hWnd, CHK_LIGHT_POS))
					{
						Light[currentLight].showPosition = true;
						CheckDlgButton(hWnd,CHK_LIGHT_POS, BST_CHECKED);
					}
					else
					{
						Light[currentLight].showPosition = false;
						CheckDlgButton(hWnd,CHK_LIGHT_POS, BST_UNCHECKED);
					}
				}
				break;

				default:
				  return DefWindowProc(hWnd,message,wParam,lParam);
			}
		}
		break;

	}
    return FALSE;
}

#endif