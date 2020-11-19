#ifndef H_ABOUTPROC_H
#define H_ABOUTPROC_H

int CALLBACK AboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

		case WM_INITDIALOG:
		{
			hWndAbout=hWnd;
			SetWindowText(GetDlgItem(hWnd,LAB_ABOUT_TITLE),Engine.GetWindowTitle());
		}
		break;

		
		case WM_CLOSE:
		{
			hWndAbout=NULL;
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
			

		case WM_COMMAND:
			if (LOWORD(wParam) == OK_BUT) 
			{
				EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
		break;

	}

    return FALSE;
}

#endif