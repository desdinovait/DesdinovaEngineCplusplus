#ifndef H_SHOWPROC_H
#define H_SHOWPROC_H

int CALLBACK ShowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

		//DSHOW
        case WM_GRAPHNOTIFY:
		{
            if (Video.HandleGraphEvent()==EC_COMPLETE)
				Engine.SetRendering(true);
		}

		case WM_CREATE:
		{
			//Video.SetFullScreen(true);
            //Video.OpenClip("Videos\\desdinova.avi");
		}
		break;

		
		case WM_CLOSE:
		{
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
			
		case WM_COMMAND:
		break;

	}
 
	//Video Nofify
    if (Video.pVW)
        Video.pVW->NotifyOwnerMessage((LONG_PTR) hWnd, message, wParam, lParam);

	return FALSE;
}

#endif