#ifndef H_GENERAL_H
#define H_GENERAL_H

//** LIBS **
#pragma comment(lib, "comctl32.lib")					//Per i controlli windows
#pragma comment(lib, "Urlmon.lib")						//Web Link

//** INCLUDES **
//Desdinova Engine Dinamic Link Library
#include "D:\\Desdinova Project\\Desdinova\\Desdinova Engine\\Desdinova Engine.h"
#pragma comment(lib, "D:\\Desdinova Project\\Desdinova\\Desdinova Engine\\Desdinova Engine.lib")
//Windows
#include <windows.h>
#include <commctrl.h>
#include <Urlmon.h>
//Risorse
#include "Resource.h"

#include "Scene0.h"

//** DEFINES **
#define BUFF_MAX 256

//** INSTANZE DI CLASSE **
//Engine
DECore		Engine;
//Scene
DEScene1	Scene1;
//DEScene	Scene2;
//DEScene	Scene3;
//Camere
DECamera	Scene1_Camera1;
DECamera	Scene1_Camera2;
DECamera	Scene2_Camera;
DECamera	Scene3_Camera;		


//Handle finestre
HWND hWndRenderZone1=NULL;
HWND hWndRenderZone2=NULL;
HWND hWndSplash=NULL;
HWND hWndMain=NULL;
HWND hWndAbout=NULL;
HWND hWndEntitiesManager=NULL;
HWND hWndModelsManager=NULL;
HWND hWndParticlesManager=NULL;
HWND hWndLightsManager=NULL;
HWND hWndCameraControl=NULL;
HWND hWndNew=NULL;
HWND hWndOpen=NULL;
HWND hWndSave=NULL;

//Controlli
HWND hWndToolbar;
HWND hWndTooltip;
HWND hWndStatusbar; 
HWND hWndTab;
HWND hWndTabDisplay;
DLGTEMPLATE *apRes[5]; 
HWND hWndTree;


//** PROROTIPI	**
LRESULT CALLBACK InfoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OpenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OpenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SaveProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif