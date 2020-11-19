#ifndef H_MAIN_H
#define H_MAIN_H

//** INCLUDES **
#include "AboutProc.h"
#include "SplashProc.h"
#include "CameraControlProc.h"
//#include "LightsManagerProc.h"
#include "ModelsManagerProc.h"
#include "EntitiesManagerProc.h"
#include "ParticlesManagerProc.h"
#include "ResourceProc.h"
#include "RenderZone1Proc.h"
#include "RenderZone2Proc.h"
#include "ClassProc.h"
#include "OpenProc.h"
#include "NewProc.h"
#include "SaveProc.h"

//** PROTOTIPI **
void CreateCommonControls(HWND windowHWND, HINSTANCE windowInstance);
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR     lpCmdLine,int       nCmdShow);
LRESULT WINAPI ClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif