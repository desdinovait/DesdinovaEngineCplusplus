#ifndef H_MAIN_H
#define H_MAIN_H

//** FUNZIONI WINDOWS **
bool Console_showFPS(bool value);
bool Console_showInfo(bool value);
bool Console_screenshot();
bool Console_closeConsole();
bool Console_exitApp();

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT WINAPI ClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif