#ifndef H_RENDERZONE1PROC_H
#define H_RENDERZONE1PROC_H



LRESULT CALLBACK RenderZone1Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	int wmEvent;

	switch (uMsg)									// Check For Windows Messages
	{

		case WM_INITDIALOG:							// Watch For Window Activate Message
		{
			RECT windowRectMain;
			GetWindowRect( Engine.GetHWND(),&windowRectMain);
			SetWindowPos(hWnd,HWND_TOP,windowRectMain.right,windowRectMain.top,NULL,NULL,SWP_NOSIZE);
			ShowWindow(hWnd,SW_SHOW);
		}
		break;


        case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{				
				//Ridimensiona il device
				Engine.ResizeSwapChain(0, 0, 1);				
			}
		}
		break;


        case WM_DESTROY:
		{
			Engine.RemoveSwapChain(1);
            return 0;
		}
        break;

        case WM_ACTIVATE:
		{
			Engine.SetActiveSwapChain(1);
			Scene1.Camera.SetHWND(Engine.GetActiveSwapChain().hWnd);
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