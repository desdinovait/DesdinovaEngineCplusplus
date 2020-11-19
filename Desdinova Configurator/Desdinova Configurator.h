//Libs
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Urlmon.lib")

//Desdinova Engine Dinamic Link Library
#include "D:\\Desdinova Project\\Desdinova\\Desdinova Engine\\Desdinova Engine.h"
#pragma comment(lib, "D:\\Desdinova Project\\Desdinova\\Desdinova Engine\\Desdinova Engine.lib")

//Includes
#include <windows.h>
#include <commctrl.h>
#include <Urlmon.h>
#include "resource.h"

//Instanza di classe del motore grafico
DECore					Engine;

//Variabili
HWND					settingsHWND;
HWND					tipHWND;
unsigned int			engineAdapterID;
unsigned int			engineFormatID;
unsigned int			engineMultisampleType;		//0=None, 1=NonMaskable, 2=Type2 3=Type3 ... 16=Type16	
unsigned int			engineMultisampleQuality;
unsigned int			engineResolutionID;
int						engineMinVertexShaderMaj;
int						engineMinVertexShaderMin;
int						engineMinPixelShaderMaj;
int						engineMinPixelShaderMin;

//Impostazioni
char*					settingsFilename="Desdinova.cfg";
char*					settingsStartname="Desdinova.exe";
char*					settingsTitle="Desdinova Engine Configurator 1.2";

//Prototipi funzioni
int APIENTRY			WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR     lpCmdLine,int       nCmdShow);
LRESULT CALLBACK		SettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void					FillSettingsControls(HWND hWnd, unsigned int currentAdapter, unsigned int	currentFormat, unsigned int	currentMultisample, unsigned int currentResolution);
bool					SaveSettingsFile(char *fileName);
