#ifndef H_MAIN_H
#define H_MAIN_H

//Funzioni console base
DWORD consoleColor;
bool Console_setTextColor(DWORD value);
bool Console_setGamma(float value);
bool Console_closeConsole();
bool Console_exitApp();
bool Console_screenshot();
bool Console_showInfo(bool value);
bool Console_showFPS(bool value);
bool Console_showCine(bool value);
bool Console_setScene(float value);

//Win
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT WINAPI ClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RenderZone1Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
