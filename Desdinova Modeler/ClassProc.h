#ifndef H_CLASSPROC_H
#define H_CLASSPROC_H


LRESULT WINAPI ClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	hWndMain=hWnd;
    switch(msg)
    {
        case WM_SIZE:
		{
			if( wParam != SIZE_MINIMIZED )
			{
				Engine.ResizeSwapChain(LOWORD(lParam), HIWORD(lParam) );
			}          
		}
		break;

        case WM_CLOSE:
		{
			Engine.Quit();
		}
        break;        

		/*
        case WM_MOVE:
		{
		}
		break;
		case WM_PAINT:
		{
		}
		break;
        case WM_ACTIVATE:
 		{
		}
        break;		
		case WM_RBUTTONUP:
		{
		}
		break;
		case WM_LBUTTONDOWN:
		{
		}
		break;
 		case WM_LBUTTONUP:
		{
		}
		break;*/

	}

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif
