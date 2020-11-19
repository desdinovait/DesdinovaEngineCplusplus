#ifndef H_MODELSMANAGERPROC_H
#define H_MODELSMANAGERPROC_H

LRESULT CALLBACK ModelsManagerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	int wmEvent;

	switch (message)
	{
		case WM_INITDIALOG:
		{	
			hWndModelsManager=hWnd;
			RECT windowRectMain;
			GetWindowRect( Engine.GetHWND(),&windowRectMain);
			SetWindowPos(hWnd,HWND_TOP,windowRectMain.right,windowRectMain.top,NULL,NULL,SWP_NOSIZE);
			ShowWindow(hWnd,SW_SHOW);

			/*Lod Level
			char bufferLOD[50]="";
			HWND hWndComboLOD=GetDlgItem(hWnd,CB_LODID);
			SendMessage(hWndComboLOD, CB_RESETCONTENT, 0, 0);
			for(int l = 0; l < Model.totalLods; l++)
			{			 
				sprintf(bufferLOD,"Level %.0f",(float)l);
				SendMessage(hWndComboLOD, CB_ADDSTRING, 0, (DWORD)bufferLOD); 
			}
			SendMessage(hWndComboLOD,CB_SETCURSEL,0,0);*/
			
			/*char bufferInfos_Init[50]="";
			sprintf(bufferInfos_Init,"%.0f",(float)Model.Object[0].totalVertices/3);
			SetDlgItemText(hWnd, LAB_FACES, bufferInfos_Init);
			sprintf(bufferInfos_Init,"%.0f",(float)Model.Object[0].totalVertices);
			SetDlgItemText(hWnd, LAB_VERTICES, bufferInfos_Init);
			sprintf(bufferInfos_Init,"%.0f",(float)Model.distanceLods);
			SetDlgItemText(hWnd,TXT_LODDISTANCE,bufferInfos_Init);*/

			/*Dimensioni
			char bufferDimensions[50]="";
			sprintf(bufferDimensions,"%.3f - %.3f",(float)Model.width,(float)Model.width * (float)Model.scaleValue.x);
			SetDlgItemText(hWnd, LAB_WIDTH, bufferDimensions);
			sprintf(bufferDimensions,"%.3f - %.3f",(float)Model.height,(float)Model.height * (float)Model.scaleValue.y);
			SetDlgItemText(hWnd, LAB_HEIGHT, bufferDimensions);
			sprintf(bufferDimensions,"%.3f - %.3f",(float)Model.depth,(float)Model.depth * (float)Model.scaleValue.z);
			SetDlgItemText(hWnd, LAB_DEPTH, bufferDimensions);
			sprintf(bufferDimensions,"%.3f - %.3f",(float)Model.radius,(float)Model.radiusMax);
			SetDlgItemText(hWnd, LAB_RADIUS, bufferDimensions);*/
			
		}
		break;


		case WM_CLOSE:
		{
			// If we close the dialog box
		    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_MODELSMANAGER, MAKELONG(false, 0));
			Scene0.showModelsManager=false;
			hWndModelsManager=NULL;
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
	
				//** CURRENT LOD **
				case CB_LODID: 
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
							/*Il controllo NON deve avere la proprietà Sort=true perckè altrimenti l'ordine degli ID risulta diverso						
							int currentLODID = (int)SendDlgItemMessage(hWnd, CB_LODID, CB_GETCURSEL, 0, 0);
							char bufferInfos[50]="";
							sprintf(bufferInfos,"%.0f",(float)Model.Object[currentLODID].totalVertices/3);
							SetDlgItemText(hWnd, LAB_FACES, bufferInfos);
							sprintf(bufferInfos,"%.0f",(float)Model.Object[currentLODID].totalVertices);
							SetDlgItemText(hWnd, LAB_VERTICES, bufferInfos);*/
						break;
					}
					break;
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