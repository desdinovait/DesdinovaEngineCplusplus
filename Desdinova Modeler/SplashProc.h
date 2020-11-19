#ifndef H_SPLASHPROC_H
#define H_SPLASHPROC_H


LRESULT CALLBACK SplashProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	   case WM_INITDIALOG:
				hWndSplash=hWnd;
				//Mostra la schermata per qualche secondo
				SetTimer(hWnd, 1, 3000, (TIMERPROC) NULL);
  			break;

	   case WM_CLOSE:
				hWndSplash = NULL;
				EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		break;
			
		case WM_TIMER:	 
			switch (wParam) 
			{ 
				case 1: 
				EndDialog(hWnd, LOWORD(wParam));
				return FALSE; 
			} 
		break;

		case WM_COMMAND:			
		break;

	}
    return FALSE;
}

#endif