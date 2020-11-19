#ifndef H_CAMERACONTROLPROC_H
#define H_CAMERACONTROLPROC_H

LRESULT CALLBACK CameraControlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	int wmId;
	int wmEvent;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			hWndCameraControl=hWnd;
			//Mostra la velocità corrente della camera
			char bufferCam[50]="";
			sprintf(bufferCam,"%.3f",Scene0.Camera.GetSpeed());
			SetDlgItemText(hWnd, TXT_CAM_SPEED, bufferCam);
			sprintf(bufferCam,"%.1f",Scene0.Camera.GetPosition().x);
			SetDlgItemText(hWnd, LAB_CAM_POSX, bufferCam);
			sprintf(bufferCam,"%.1f",Scene0.Camera.GetPosition().y);
			SetDlgItemText(hWnd, LAB_CAM_POSY, bufferCam);
			sprintf(bufferCam,"%.1f",Scene0.Camera.GetPosition().z);
			SetDlgItemText(hWnd, LAB_CAM_POSZ, bufferCam);
			sprintf(bufferCam,"%.1f",Scene0.Camera.GetView().x);
			SetDlgItemText(hWnd, LAB_CAM_VIEWX, bufferCam);
			sprintf(bufferCam,"%.1f",Scene0.Camera.GetView().y);
			SetDlgItemText(hWnd, LAB_CAM_VIEWY, bufferCam);
			sprintf(bufferCam,"%.1f",Scene0.Camera.GetView().z);
			SetDlgItemText(hWnd, LAB_CAM_VIEWZ, bufferCam);


		}
		break;

		case WM_CLOSE:										// If we close the dialog box
		    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_CAMERACONTROL, MAKELONG(false, 0));
			Scene0.showCameraControl=false;
			hWndCameraControl=NULL;
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		break;
			
		case WM_COMMAND:

			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				//Setta la velocità della camera
				case BUT_CAM_SETSPEED:
				{
					LPSTR strSpeed={0};
					strSpeed=(LPSTR)malloc(10);
					GetDlgItemText(hWnd,TXT_CAM_SPEED,strSpeed,10);
					//Velocità
					Scene0.Camera.SetSpeed((float)atof(strSpeed));
					free(strSpeed);
				}
				break;
			}


		break;

	}
    return FALSE;
}

#endif