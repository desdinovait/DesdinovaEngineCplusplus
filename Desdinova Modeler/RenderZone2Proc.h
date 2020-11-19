#ifndef H_RENDERZONE2PROC_H
#define H_RENDERZONE2PROC_H



LRESULT CALLBACK RenderZone2Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	int wmEvent;

	switch (uMsg)									// Check For Windows Messages
	{

		case WM_INITDIALOG:							// Watch For Window Activate Message
		{
			RECT windowRectMain;
			RECT windowRect1;
			GetWindowRect( Engine.GetHWND(),&windowRectMain);
			GetWindowRect( hWndRenderZone1,&windowRect1);

			SetWindowPos(hWnd,HWND_TOP,windowRectMain.right,windowRectMain.top+windowRect1.bottom,NULL,NULL,SWP_NOSIZE);
			ShowWindow(hWnd,SW_SHOW);
		}
		break;

        case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{				
				//Ridimensiona il device
				Engine.ResizeSwapChain(0, 0, 2);				
			}
		}
		break;


        case WM_DESTROY:
		{
			SendMessage(hWnd,WM_ACTIVATE,0,0);
 			Engine.RemoveSwapChain(2);
            return 0;
		}
        break;


        case WM_ACTIVATE:
		{
			Engine.SetActiveSwapChain(2);
			Scene2.Camera.SetHWND(Engine.GetActiveSwapChain().hWnd);
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
			DestroyWindow(hWnd);
			return TRUE;							// Jump Back
		}
		break;
 

		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
		}
		break;

	}
	return FALSE;
}


#endif