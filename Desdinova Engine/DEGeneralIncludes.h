#ifndef H_GENERALINCLUDES_H
#define H_GENERALINCLUDES_H

//************************
//** INCLUDES e DEFINES **
//************************

//** Dllexport **
#ifndef DLL_API
#define DLL_API __declspec(dllexport) 
#endif


//** Libraries **
#ifdef _DEBUG
	#pragma comment(lib,"d3dx9d.lib")
#else
	#pragma comment(lib,"d3dx9.lib")
#endif
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NxExtensions.lib")
#pragma comment(lib, "NxCooking.lib")
#pragma comment(lib, "NxCharacter.lib")


//** Deprecated Warnings **
#pragma warning(disable : 4995)	
#pragma warning(disable : 4305)	
#pragma warning(disable : 4786)
#pragma warning(disable : 4006)	

#if (_MSC_VER >= 1400)//VC8+
	#pragma warning(disable : 4996)//Deprecation warnings
	 //Or just turn off warnings about the newly deprecated CRT functions.
	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE
	#endif
	#ifndef _CRT_NONSTDC_NO_DEPRECATE
		#define _CRT_NONSTDC_NO_DEPRECATE
	#endif
#endif


//** Pre-Defines **
#define DIRECTINPUT_VERSION 0x0800


//** Includes **
//Windows
#include <fstream>
#include <cassert>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <hash_map>
#include <cmath>
#include <limits>
#define NOMINMAX
#include <windows.h>
#include <winuser.h>
#include <cguid.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <string>
#include <cstdarg>
#include <strstream>
#include <atlbase.h>
#include <objbase.h>
#include <search.h>
#include "psapi.h"
#include "assert.h"

//DirectX
#ifdef _DEBUG
	#define D3D_DEBUG_INFO
#endif
#include <D3DX9.h>
#include <Dxerr9.h>
#include <dxdiag.h>
#include <dinput.h>

//** Additionals SDK or LIBS **
//NovodeX physics engine 
//1. Specify the “SDKs\Foundation\include” and “SDKs\Physics\include” as directories in which include files are located.
//2. Identify the NxFoundation.lib file in the
//“SDKs\Foundation\lib\win32\Release” and the NxPhysics.lib file in
//“SDKs\Physics\lib\win32\Release” as libraries to the linker.
//
//When you have created an executable, you should make sure NxFoundation.dll and
//NxPhysics.dll is available somewhere in the windows DLL search path (see the MSDN
//documentation entry for “LoadLibrary” for details.). The easiest way to achieve this is to
//have the compiler write the produced executable into the /BIN/win32 directory.
//
//The punt #define NOMINMAX before <windows.h> definition
//#undef random
#include "NxPhysics.h"
#include "NxCooking.h"
#include "NxStream.h"

//FMOD SoundSystem
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"


//** Other inclusions **
//Namespaces
using namespace std;
//Resource
#include "resource.h"
#include "DEClasses.h"
#include "DEEnumerations.h"
#include "DEStructures.h"


//** General Defines **
#define SafeRelease(pInterface) if(pInterface != NULL) {pInterface->Release(); pInterface=NULL;}
#define SafeDelete(pObject) if(pObject != NULL) {delete pObject; pObject=NULL;}
#define SafeDeleteArray(pObject) if(pObject != NULL) {delete[] pObject; pObject=NULL;}


//** Includes **
#include "DECpuInfo.h"
#include "DEAxes.h"
#include "DEBillboard.h"
#include "DEBoundingBox.h"
#include "DEBoundingSphere.h"
#include "DESpatialGrid.h"
#include "DECamera.h"
#include "DEConsole.h"
#include "DEPhysX.h"
#include "DECore.h"
#include "DEModel.h"
#include "DEModelManager.h"
#include "DEMaterialManager.h"
#include "DEEntity.h"
#include "DEFont.h"
#include "DEGrid.h"
#include "DEInput.h"
#include "DEInterpolations.h"
#include "DELensFlare.h"
#include "DELight.h"
#include "DELog.h"
#include "DEModel.h"
#include "DEMeshManager.h"
#include "DEPanel.h"
#include "DEParticleSystem.h"
#include "DEProperty.h"
#include "DEScene.h"
#include "DESky.h"

#include "DESoundDSP.h"
#include "DESoundSystem.h"
#include "DESound.h"
#include "DESoundChannelGroup.h"

#include "DETextureManager.h"
#include "DEUtility.h"
#include "DEVBManager.h"


#endif