#include "DEGeneralIncludes.h"

int DEUtility::currentPoint=0;
float DEUtility::currentTime=0;

//Conversioni
DLL_API DWORD DEUtility::FtoDW( FLOAT f )
{ 
	return *((DWORD*)&f); 
}
DLL_API FLOAT DEUtility::DWtoF( DWORD d ) 
{ 
	return *((FLOAT*)&d); 
}


DLL_API long DEUtility::GetTotalMemory()
{ 
	try
	{
		MEMORYSTATUS	mMemoryStatus;
		memset( &mMemoryStatus,  0, sizeof(MEMORYSTATUS) );
		mMemoryStatus.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus( &mMemoryStatus ); 
		return (long)mMemoryStatus.dwTotalPhys;
	}
	catch(...)
	{
		return 0;
	}

	return 0;

}

DLL_API long DEUtility::GetAvailableMemory()
{ 
	try
	{
		MEMORYSTATUS	mMemoryStatus;
		memset( &mMemoryStatus,  0, sizeof(MEMORYSTATUS) );
		mMemoryStatus.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus( &mMemoryStatus ); 
		return (long)mMemoryStatus.dwAvailPhys;
	}
	catch(...)
	{
		return 0;
	}

	return 0;

}

DLL_API long DEUtility::GetMemoryUsage()
{ 
	try
	{
		MEMORYSTATUS	mMemoryStatus;
		memset( &mMemoryStatus,  0, sizeof(MEMORYSTATUS));
		mMemoryStatus.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus( &mMemoryStatus ); 
		return (long)mMemoryStatus.dwMemoryLoad;
	}
	catch(...)
	{
		return 0;
	}

	return 0;
}

DLL_API long DEUtility::GetCurrentProcessMemoryUsage()
{ 
	try
	{
		long returnedMemory = 0;
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;
		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, GetCurrentProcessId() );
		if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)))
		{
			returnedMemory = pmc.WorkingSetSize;
		}
		CloseHandle( hProcess );
		return (long)returnedMemory;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}

DLL_API long DEUtility::GetProcessMemoryUsage(DWORD processID)
{ 
	try
	{
		long returnedMemory = 0;
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;
		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, processID );
		if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)))
		{
			returnedMemory = pmc.WorkingSetSize;
		}
		CloseHandle( hProcess );
		return (long)returnedMemory;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}

DLL_API  char *DEUtility::GetCurrentPath()
{
	
	char *moduleName;
	moduleName=(char *)malloc(MAX_PATH);
	GetModuleFileName(NULL, moduleName, MAX_PATH);
	
	char	strModulePath[MAX_PATH];
	memset(strModulePath, 0, sizeof(char) * MAX_PATH);
	char *pdest1;
	int result1=0;
	pdest1 = strrchr( moduleName, '\\' );
	result1 = pdest1 - moduleName + 1;
	strncpy( (char *)strModulePath, (char *)moduleName, result1-1 );	
	strcat(strModulePath, "\\");
	free(moduleName);

	return strModulePath;
}



DLL_API  bool DEUtility::InRoot(const char *completeFilename)
{
	char modulePath[MAX_PATH];
	char currentPath[MAX_PATH];
	memset(modulePath, 0, sizeof(char) * MAX_PATH);
	memset(currentPath, 0, sizeof(char) * MAX_PATH);
	strcpy(modulePath, GetCurrentPath());
	int modulePath_Lenght = (int)strlen(modulePath);
	
	
	char *pdest1;
	int result1=0;
	pdest1 = strrchr( (char *)completeFilename, '\\' );
	result1 = pdest1 - completeFilename + 1;
	strncpy(currentPath, completeFilename, result1-1 );	
	strcat(currentPath, "\\");
	int currentPath_Lenght = (int)strlen(currentPath);

	//Il path è più corto quindi è sicuramente diverso
	if (currentPath_Lenght < modulePath_Lenght)
	{
		return false;
	}
	else
	{
		//Resetta la variabile
		memset(currentPath, 0, sizeof(char) * MAX_PATH);
		//Copia la stessa dimensione per vedere se sono uguali
		strncpy(currentPath, completeFilename, modulePath_Lenght);
		//Trasforma le due stringhe in modo da renderle simili
		strcpy(modulePath, strupr(modulePath));
		strcpy(currentPath, strupr(currentPath));

		if (strcmp(currentPath, modulePath)==0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}



DLL_API  char *DEUtility::NoRoot(const char *completeFilename)
{

	char *modulePath;
	char *soloPath;
	modulePath = (char *)malloc(MAX_PATH);
	soloPath = (char *)malloc(MAX_PATH);
	memset(modulePath, 0, sizeof(char) * MAX_PATH);
	memset(soloPath, 0, sizeof(char) * MAX_PATH);
	strcpy(modulePath, GetCurrentPath());
	int modulePath_Lenght = (int)strlen(modulePath);
	int currentFile_Lenght = (int)strlen(completeFilename);

	char *reversed;
	char *reversed2;
	reversed = (char *)malloc(MAX_PATH);
	reversed2 = (char *)malloc(MAX_PATH);
	memset(reversed, 0, sizeof(char) * MAX_PATH);
	memset(reversed2, 0, sizeof(char) * MAX_PATH);
	reversed = _strrev( _strdup( completeFilename ) );

	strncpy(reversed2,reversed, currentFile_Lenght - modulePath_Lenght);

	soloPath = _strrev( _strdup( reversed2 ) );

	free(modulePath);
	free(reversed);
	free(reversed2);

	return soloPath;

}


//-----------------------------------------------------------------------------
// Name: ConvertAnsiStringToWideCch()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
DLL_API HRESULT DEUtility::ConvertAnsiStringToWideCch( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

    int nResult = MultiByteToWideChar( CP_ACP, 0, strSource, -1, 
                                       wstrDestination, cchDestChar );
    wstrDestination[cchDestChar-1] = 0;
    
    if( nResult == 0 )
        return E_FAIL;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ConvertStringToWideCch()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
DLL_API HRESULT DEUtility::ConvertStringToWideCch( WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar )
{
    if( wstrDestination==NULL || tstrSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

#ifdef _UNICODE
    wcsncpy( wstrDestination, tstrSource, cchDestChar );
    wstrDestination[cchDestChar-1] = L'\0';
    return S_OK;
#else
    return DEUtility::ConvertAnsiStringToWideCch( wstrDestination, tstrSource, cchDestChar );
#endif    
}


DLL_API int DEUtility::HashString( const char* str )
{
    int ret = 0;
    while( *str )
    {
        ret = 3 * ret + *str;
        str++;
    }
    return ret;
}

DLL_API int DEUtility::HashString( const std::string& s )
{
    register int ret = 0;
    for( unsigned int i = 0; i < s.size(); ++i )
    {
        ret = 3 * ret + s[i];
	}
    return ret;
}

DLL_API void DEUtility::TokenizeString( const char* str, std::vector<std::string>& tokens, const char* delimiters, bool bIgnoreQuotes )
{
	static char tempBuffer[256];
	tokens.clear();
	const char* delims = delimiters;
	bool bInsideQuotes = false;
	bool bJustCamOut = false;
	std::string quotedString = "";
	if( str )
	{
		std::strcpy( tempBuffer, str );

		char* str = strtok( tempBuffer, delims );

		while( str )
		{
			if( !bInsideQuotes && !bIgnoreQuotes )
			{
				if( *str == '\"' )
					bInsideQuotes = true;
			}

			if( !bInsideQuotes )
			{
				if( bJustCamOut )
				{
					quotedString[quotedString.size() - 1] = '\0';
					char* qlessbuff = &quotedString[1];
					tokens.push_back( qlessbuff );
					quotedString = "";
					bJustCamOut = false;
				}
				
				tokens.push_back( str );
			}
			else
			{
				quotedString += str;				
			}

			if( bInsideQuotes )
			{
				if( quotedString[quotedString.size() - 1] == '\"' )
				{
					bJustCamOut = true;
					bInsideQuotes = false;
				}
				else
					quotedString += " ";
			}

			str = strtok( NULL, delims );
		}
	}			
}



DLL_API D3DXMATRIX DEUtility::GetCubeMapViewMatrix(DWORD dwFace, D3DXVECTOR3 Position)
{
	D3DXVECTOR3 LookDir;
	D3DXVECTOR3 UpDir;

	switch( dwFace )
	{
	case D3DCUBEMAP_FACE_POSITIVE_X:
		LookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) + Position;
		UpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_X:
		LookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f ) + Position;
		UpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		break;
	case D3DCUBEMAP_FACE_POSITIVE_Y:
		LookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) + Position;
		UpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_Y:
		LookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f ) + Position;
		UpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		break;
	case D3DCUBEMAP_FACE_POSITIVE_Z:
		LookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) + Position;
		UpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_Z:
		LookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f ) + Position;
		UpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		break;
	}

	// Set the view transform for this cubemap surface
	D3DXMATRIX NewView;
	D3DXMatrixLookAtLH( &NewView, &Position, &LookDir, &UpDir );
	return NewView;
}


//This function calculates the planes for describing the view frustum using 3 points.
//Because we are in view space the cameras position is at 0,0,0
//We use the back clip position and the viewing angle to work out a point on the edge 
//of the frustum.
//The view angle is the angle between the top and bottom of the view frustum in radians.
DLL_API STRUCT_DE_FOV DEUtility::SetupFOVClipPlanes( float Angle , float FrontClip , float BackClip, float AspectRatio )
{
	STRUCT_DE_FOV finalFOV;
	D3DXVECTOR3	P0 , P1 , P2;

	//Calculate left plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = -BackClip * ( (float)tan( Angle * 0.5f) * AspectRatio);
	P1.y = -BackClip * ( (float)tan( Angle * 0.5f) );
	P1.z = BackClip;
	P2.x = P1.x;
	P2.y = -P1.y;
	P2.z = P1.z;
	DEUtility::PlaneFromPoints( &P0, &P1 , &P2 , &finalFOV.Fov_LeftPlane );

	//Calculate right plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = BackClip * ( (float)tan( Angle * 0.5f ) * AspectRatio);
	P1.y = BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = P1.x;
	P2.y = -P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &finalFOV.Fov_RightPlane );

	//Calculate top plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = -BackClip * ( (float)tan( Angle * 0.5f ) * AspectRatio);
	P1.y = BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = -P1.x;
	P2.y = P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &finalFOV.Fov_TopPlane );

	//Calculate bottom plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = BackClip * ( (float)tan( Angle * 0.5f ) * AspectRatio);
	P1.y = -BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = -P1.x;
	P2.y = P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &finalFOV.Fov_BottomPlane );


	finalFOV.Fov_BackClip = BackClip;
	finalFOV.Fov_FrontClip = FrontClip;

	return finalFOV;

}



//Funzioni ToString(...), ritornano in versione char l'elemento passato come valore const)
//Utili per scrivere nel file di debug
DLL_API char* DEUtility::VertexProcessingToString( const DWORD& vp )
{
	switch( vp )
	{
	case DE_VERTEXPROCESSINGTYPE_SW:		return "D3DCREATE_SOFTWARE_VERTEXPROCESSING";      	                        
	case DE_VERTEXPROCESSINGTYPE_HW:		return "D3DCREATE_HARDWARE_VERTEXPROCESSING";       
	case DE_VERTEXPROCESSINGTYPE_PUREHW:	return "D3DCREATE_PUREDEVICE"; 
	case DE_VERTEXPROCESSINGTYPE_MIXED:		return "D3DCREATE_MIXED_VERTEXPROCESSING"; 
	default:		return "UNKNOWN";
	}
}

DLL_API char* DEUtility::DeviceTypeToString( const D3DDEVTYPE& dt )
{
	switch( dt )
	{
	case D3DDEVTYPE_HAL:	return "Hardware Abstraction Layer (HAL)";      	                        
	case D3DDEVTYPE_REF:	return "Reference Rasterizer (REF)";       
	case D3DDEVTYPE_SW:		return "Software Device"; 
	default:				return "UNKNOWN";
	}
}

DLL_API char* DEUtility::D3DFormatToString( const D3DFORMAT& fmt )
{
	switch( fmt ) 
	{
	case D3DFMT_UNKNOWN:      return "D3DFMT_UNKNOWN";      
	                        
	case D3DFMT_R8G8B8:       return "D3DFMT_R8G8B8";       
	case D3DFMT_A8R8G8B8:     return "D3DFMT_A8R8G8B8";     
	case D3DFMT_X8R8G8B8:     return "D3DFMT_X8R8G8B8";     
	case D3DFMT_R5G6B5:       return "D3DFMT_R5G6B5";       
	case D3DFMT_X1R5G5B5:     return "D3DFMT_X1R5G5B5";     
	case D3DFMT_A1R5G5B5:     return "D3DFMT_A1R5G5B5";     
	case D3DFMT_A4R4G4B4:     return "D3DFMT_A4R4G4B4";     
	case D3DFMT_R3G3B2:       return "D3DFMT_R3G3B2";       
	case D3DFMT_A8:           return "D3DFMT_A8";           
	case D3DFMT_A8R3G3B2:     return "D3DFMT_A8R3G3B2";     
	case D3DFMT_X4R4G4B4:     return "D3DFMT_X4R4G4B4";     
	                        
	case D3DFMT_A8P8:         return "D3DFMT_A8P8";         
	case D3DFMT_P8:           return "D3DFMT_P8";           
	                        
	case D3DFMT_L8:           return "D3DFMT_L8";           
	case D3DFMT_A8L8:         return "D3DFMT_A8L8";         
	case D3DFMT_A4L4:         return "D3DFMT_A4L4";         
	                        
	case D3DFMT_V8U8:         return "D3DFMT_V8U8";         
	case D3DFMT_L6V5U5:       return "D3DFMT_L6V5U5";       
	case D3DFMT_X8L8V8U8:     return "D3DFMT_X8L8V8U8";     
	case D3DFMT_Q8W8V8U8:     return "D3DFMT_Q8W8V8U8";     
	case D3DFMT_V16U16:       return "D3DFMT_V16U16";       
	                                                            
	case D3DFMT_UYVY:         return "D3DFMT_UYVY";         
	case D3DFMT_YUY2:         return "D3DFMT_YUY2";         
	case D3DFMT_DXT1:         return "D3DFMT_DXT1";         
	case D3DFMT_DXT2:         return "D3DFMT_DXT2";         
	case D3DFMT_DXT3:         return "D3DFMT_DXT3";         
	case D3DFMT_DXT4:         return "D3DFMT_DXT4";         
	case D3DFMT_DXT5:         return "D3DFMT_DXT5";         

	case D3DFMT_D16_LOCKABLE: return "D3DFMT_D16_LOCKABLE"; 
	case D3DFMT_D32:          return "D3DFMT_D32";          
	case D3DFMT_D15S1:        return "D3DFMT_D15S1";        
	case D3DFMT_D24S8:        return "D3DFMT_D24S8";        
	case D3DFMT_D16:          return "D3DFMT_D16";          
	case D3DFMT_D24X8:        return "D3DFMT_D24X8";        
	case D3DFMT_D24X4S4:      return "D3DFMT_D24X4S4";      

	case D3DFMT_VERTEXDATA:   return "D3DFMT_VERTEXDATA"; 
	case D3DFMT_INDEX16:      return "D3DFMT_INDEX16"; 
	case D3DFMT_INDEX32:      return "D3DFMT_INDEX32"; 
	default:                  return "D3DFMT_UNKNOWN"; 
	}
}

DLL_API char* DEUtility::MultisampleTypeToString( const D3DMULTISAMPLE_TYPE& ms )
{
	switch ( ms )
	{
	case D3DMULTISAMPLE_NONE:			return "D3DMULTISAMPLE_NONE";
	case D3DMULTISAMPLE_NONMASKABLE:	return "D3DMULTISAMPLE_NONMASKABLE";
	case D3DMULTISAMPLE_2_SAMPLES:		return "D3DMULTISAMPLE_02_SAMPLES";
	case D3DMULTISAMPLE_3_SAMPLES:		return "D3DMULTISAMPLE_03_SAMPLES";
	case D3DMULTISAMPLE_4_SAMPLES:		return "D3DMULTISAMPLE_04_SAMPLES";
	case D3DMULTISAMPLE_5_SAMPLES:		return "D3DMULTISAMPLE_05_SAMPLES";
	case D3DMULTISAMPLE_6_SAMPLES:		return "D3DMULTISAMPLE_06_SAMPLES";
	case D3DMULTISAMPLE_7_SAMPLES:		return "D3DMULTISAMPLE_07_SAMPLES";
	case D3DMULTISAMPLE_8_SAMPLES:		return "D3DMULTISAMPLE_08_SAMPLES";
	case D3DMULTISAMPLE_9_SAMPLES:		return "D3DMULTISAMPLE_09_SAMPLES";
	case D3DMULTISAMPLE_10_SAMPLES:		return "D3DMULTISAMPLE_10_SAMPLES";
	case D3DMULTISAMPLE_11_SAMPLES:		return "D3DMULTISAMPLE_11_SAMPLES";
	case D3DMULTISAMPLE_12_SAMPLES:		return "D3DMULTISAMPLE_12_SAMPLES";
	case D3DMULTISAMPLE_13_SAMPLES:		return "D3DMULTISAMPLE_13_SAMPLES";
	case D3DMULTISAMPLE_14_SAMPLES:		return "D3DMULTISAMPLE_14_SAMPLES";
	case D3DMULTISAMPLE_15_SAMPLES:		return "D3DMULTISAMPLE_15_SAMPLES";
	case D3DMULTISAMPLE_16_SAMPLES:		return "D3DMULTISAMPLE_16_SAMPLES";
	default:							return "D3DMULTISAMPLE_UNKNOWN";
	}
}

DLL_API char* DEUtility::PresentIntervalToString( const UINT& pi )
{
	switch( pi )
	{
	case D3DPRESENT_INTERVAL_IMMEDIATE:	return "D3DPRESENT_INTERVAL_IMMEDIATE";
	case D3DPRESENT_INTERVAL_DEFAULT:	return "D3DPRESENT_INTERVAL_DEFAULT";
	case D3DPRESENT_INTERVAL_ONE:		return "D3DPRESENT_INTERVAL_ONE";
	case D3DPRESENT_INTERVAL_TWO:		return "D3DPRESENT_INTERVAL_TWO";
	case D3DPRESENT_INTERVAL_THREE:		return "D3DPRESENT_INTERVAL_THREE";
	case D3DPRESENT_INTERVAL_FOUR:		return "D3DPRESENT_INTERVAL_FOUR";
	default:							return "D3DPRESENT_INTERVAL_UNKNOWN";
	}
}


//Ritorna i BPP del formato desiderato
DLL_API UINT DEUtility::GetBitsPerPixel( const D3DFORMAT& fmt )
{
    switch( fmt ) 
	{
    //32 bit modes
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A2R10G10B10:
		return 32;

    //24 bit modes
    case D3DFMT_R8G8B8:
		return 24;

    //16 bit modes
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
		return 16;

    default:
		return 0;
	}
}

//Ritorna i depth bits del formato desiderato
DLL_API UINT DEUtility::GetDepthBits( const D3DFORMAT& fmt )
{
    switch( fmt )
    {
    case D3DFMT_D16:         
    case D3DFMT_D16_LOCKABLE:
        return 16;

    case D3DFMT_D15S1:
        return 15;

    case D3DFMT_D24X8:
    case D3DFMT_D24S8:
    case D3DFMT_D24X4S4:
        return 24;

    case D3DFMT_D32:
        return 32;

    default:
        return 0;
    }
}

//Ritorna gli stencil bits riservati del formato desiderato
DLL_API UINT DEUtility::GetStencilBits( const D3DFORMAT& fmt)
{
    switch( fmt )
    {
    case D3DFMT_D24S8:   
        return 8;

    case D3DFMT_D15S1:     
        return 1;

    case D3DFMT_D24X4S4: 
        return 4;

    case D3DFMT_D32:      
    case D3DFMT_D24X8:  
    case D3DFMT_D16:                    
    case D3DFMT_D16_LOCKABLE:
        return 0;

    default:
        return 0;
    }
}



DLL_API  float DEUtility::GetRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

DLL_API  bool DEUtility::GetRandomBool()
{
	float fMin = 0;
	float fMax = 1;
    float fRandNum = (float)rand () / RAND_MAX;
    float result =  fMin + (fMax - fMin) * fRandNum;
	if (result <= 0.5f) return true;
	else				return false;

}

DLL_API D3DXVECTOR3 DEUtility::GetRandomVector(float fMin, float fMax)
{
	D3DXVECTOR3 vVector;

    // Pick a random Z between fMin e fMax
	vVector.z = GetRandomMinMax( fMin, fMax );
    
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = GetRandomMinMax( -D3DX_PI, D3DX_PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

	return vVector;
}



DLL_API void DEUtility::PlaneFromPoints(D3DXVECTOR3 *pP0 , D3DXVECTOR3 *pP1 , D3DXVECTOR3 *pP2 , D3DXVECTOR4 *pPlane)
{
	D3DXVECTOR3 V0,V1,V2;

	V0.x = pP1->x - pP0->x;
	V0.y = pP1->y - pP0->y;
	V0.z = pP1->z - pP0->z;
	V1.x = pP2->x - pP0->x;
	V1.y = pP2->y - pP0->y;
	V1.z = pP2->z - pP0->z;
	D3DXVec3Cross( &V2, &V0, &V1);
	D3DXVec3Normalize( &V2, &V2);
	pPlane->x = V2.x;
	pPlane->y = V2.y;
	pPlane->z = V2.z;
	pPlane->w = -( V2.x * pP0->x + V2.y * pP0->y + V2.z * pP0->z );
}

//Questa funzione lavora con le curve bezier
//La curva ha un punto iniziale P1,uno finale P4, e due intermedi P2,P3 che creano la 
//caratteristica curva bazier.E' possibile definire i due punti intermedi uguali a quello
//iniziale o finale i modo da creare una semplice linea.
//Il valore di T deve essere tra 0 e 1 ed segna la posizione nel tempo sulla curva:
//più il valore di t si avvicina a 1 e più siamo vicini al punto finale(interpolazione)
//E' possibile anche non mettere il limite a t e quindi il punto generato dalla funzione
//sarà comunque tendente ad infinito seguendo il vettore linea della curva(cioè il cuntinuo esatto all'infinito del punto finale della curva)
DLL_API D3DXVECTOR3 DEUtility::PointOnBezierCurve4(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4, float t)
{
	float var1, var2, var3;
    D3DXVECTOR3 vPoint;

	var1 = 1 - t;	
    var2 = var1 * var1 * var1;
    var3 = t * t * t;

    vPoint.x = var2*p1.x + 3*t*var1*var1*p2.x + 3*t*t*var1*p3.x + var3*p4.x;
    vPoint.y = var2*p1.y + 3*t*var1*var1*p2.y + 3*t*t*var1*p3.y + var3*p4.y;
    vPoint.z = var2*p1.z + 3*t*var1*var1*p2.z + 3*t*t*var1*p3.z + var3*p4.z;

    return(vPoint);				
}

DLL_API D3DXVECTOR3 DEUtility::PointOnBezierCurve3(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, float t)
{
   double mum1,mum12,mu2;
   D3DXVECTOR3 vPoint;

   mu2 = t * t;
   mum1 = 1 - t;
   mum12 = mum1 * mum1;
   vPoint.x = p1.x * mum12 + 2 * p2.x * mum1 * t + p3.x * mu2;
   vPoint.y = p1.y * mum12 + 2 * p2.y * mum1 * t + p3.y * mu2;
   vPoint.z = p1.z * mum12 + 2 * p2.z * mum1 * t + p3.z * mu2;

   return(vPoint);
}

DLL_API D3DXVECTOR3 DEUtility::PointOnBezierCurveN(D3DXVECTOR3 *p, int n, double t)
{
   int k,kn,nn,nkn;
   double blend,muk,munk;
   D3DXVECTOR3 vPoint;
   vPoint.x = 0.0;
   vPoint.y = 0.0;
   vPoint.z = 0.0;
	  
   n = n-1;

   muk = 1;
   munk = pow(1-t,(double)n);

   for (k=0;k<=n;k++) 
   {
      nn = n;
      kn = k;
      nkn = n - k;
      blend = muk * munk;
      muk *= t;
      munk /= (1-t);
      while (nn >= 1) 
	  {
         blend *= nn;
         nn--;
         if (kn > 1) 
		 {
            blend /= (double)kn;
            kn--;
         }
         if (nkn > 1)
		 {
            blend /= (double)nkn;
            nkn--;
         }
      }
      vPoint.x += p[k].x * blend;
      vPoint.y += p[k].y * blend;
      vPoint.z += p[k].z * blend;
   }

   return(vPoint);
}


/*Interpolazione lineare
DLL_API D3DXVECTOR3 DEUtility::PointOnLine(D3DXVECTOR3 *P, int totalPoints, float steps)
{
	D3DXVECTOR3 result;

	currentTime = currentTime + 0.1f;
	if (currentTime==steps)
	{
		currentPoint = currentPoint + 1;
		if (currentPoint >= totalPoints-1)
		{
			currentPoint=0;
		}
		currentTime=1.0f;
	}

	float t = (float)currentTime / (float)steps;

	result.x = P[currentPoint].x + (currentTime/(P[currentPoint+1].x - P[currentPoint].x));
	result.y = P[currentPoint].y + (currentTime/(P[currentPoint+1].y - P[currentPoint].y));
	result.z = P[currentPoint].z + (currentTime/(P[currentPoint+1].z - P[currentPoint].z));

	return result;
}*/


//Interpolazione lineare
DLL_API D3DXVECTOR3 DEUtility::PointOnLine(D3DXVECTOR4 *P, int totalPoints, float velocity)
{
	D3DXVECTOR3 result;

	currentTime = currentTime + velocity;
	if (currentTime == P[currentPoint+1].w )
	{
		currentTime = 0;
		currentPoint=currentPoint+1;
		if (currentPoint>= totalPoints-1)
		{
			currentPoint=0;
		}
	}
	
	/*result.x = P[currentPoint].x + ((currentTime * P[currentPoint+1].x ) / (P[currentPoint+1].w - P[currentPoint].w));
	result.y = P[currentPoint].y + ((currentTime * P[currentPoint+1].y ) / (P[currentPoint+1].w - P[currentPoint].w));
	result.z = P[currentPoint].z + ((currentTime * P[currentPoint+1].z ) / (P[currentPoint+1].w - P[currentPoint].w));*/


	float t = (float)currentTime / (float)(P[currentPoint+1].w - P[currentPoint].w);
	
	result.x = P[currentPoint].x + (currentTime/(P[currentPoint+1].x - P[currentPoint].x));
	result.y = P[currentPoint].y + (currentTime/(P[currentPoint+1].y - P[currentPoint].y));
	result.z = P[currentPoint].z + (currentTime/(P[currentPoint+1].z - P[currentPoint].z));

		

	return result;
}


DLL_API  bool DEUtility::RayIntersectsSphere2D( D3DXVECTOR3 orig, D3DXVECTOR3 dir, D3DXVECTOR3 center, float radius )
{
	D3DXVECTOR3 vOToC = center - orig;
	
	float fHypotenuseSq = D3DXVec3LengthSq( &vOToC ),
		  fProjection   = D3DXVec3Dot( &vOToC,&dir );
	
	// If the projection's negative, then 't' is going to be negative ( if there's a 't' )
	if( fProjection < 0 )
	{
		return false;
	}
	
	//
	// fDistanceSq is (square) the shortest distance between the sphere center & the ray
	//
	float fDistanceSq = D3DXVec3LengthSq( &vOToC ) - fProjection * fProjection;
	
	// If it's greater than (square) the radius, then there's no intersection
	if(fDistanceSq > radius * radius)
	{
		return false;
	}
		
	// Calculate 't', the parameter
	float t = fProjection - (float)sqrt( radius * radius - fDistanceSq );
	
	// If it's less than 0, then we have a straight line-sphere intersection not a
	// ray-sphere intersection
	if( t < 0 )
	{
		return false;
	}
		
	return true;
}

//Given a ray origin (orig) and direction (dir), and three vertices of
//of a triangle, this function returns TRUE and the interpolated texture
//coordinates if the ray intersects the triangle
DLL_API  bool DEUtility::RayIntersectsTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return TRUE;
}


DLL_API  bool DEUtility::PointInSphere(D3DXVECTOR3 &point, D3DXVECTOR3 &center,float centerRadius)
{
	D3DXVECTOR3 resultVector;
	resultVector=center-point;
	return D3DXVec3LengthSq(&resultVector) < centerRadius*centerRadius;
}


DLL_API  bool DEUtility::SphereInSphere(D3DXVECTOR3 &point, float pointRadius, D3DXVECTOR3 &center,float centerRadius)
{
	D3DXVECTOR3 resultVector;
	resultVector=center - point;
	float centDist=centerRadius + pointRadius;
	return D3DXVec3LengthSq(&resultVector) < centDist*centDist;
}

DLL_API  bool DEUtility::PointInAABB(D3DXVECTOR3 &point, D3DXVECTOR3 &boundMin, D3DXVECTOR3 &boundMax)
{
	if( (point.x > boundMin.x)&&
		(point.y > boundMin.y)&&
		(point.z > boundMin.z)&&
		(point.x < boundMax.x)&&
		(point.y < boundMax.y)&&
		(point.z < boundMax.z))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}



DLL_API bool DEUtility::FOVCheck(D3DXVECTOR3 position, float sphereRadius, D3DXMATRIX viewTransform, STRUCT_DE_FOV fovProperties, D3DXVECTOR3 *pViewPos)
{

	float	Dist;

	//Transform Z into view space
	pViewPos->z = viewTransform._13 * position.x +
		viewTransform._23 * position.y +
		viewTransform._33 * position.z + viewTransform._43;

	//Behind front clip plane?
	if( ( pViewPos->z + sphereRadius ) < fovProperties.Fov_FrontClip )
		return FALSE;

	//Beyond the back clip plane?
	if( ( pViewPos->z - sphereRadius ) > fovProperties.Fov_BackClip )
		return FALSE;

	//Transform X into view space
	pViewPos->x = viewTransform._11 * position.x +
		viewTransform._21 * position.y +
		viewTransform._31 * position.z + viewTransform._41;

	//Test against left clip plane
	Dist = ( pViewPos->x * fovProperties.Fov_LeftPlane.x ) + ( pViewPos->z * fovProperties.Fov_LeftPlane.z );
	if( Dist > sphereRadius)
		return FALSE;

	//Test against right clip plane
	Dist = ( pViewPos->x * fovProperties.Fov_RightPlane.x ) + ( pViewPos->z * fovProperties.Fov_RightPlane.z );
	if( Dist > sphereRadius)
		return FALSE;

	//Transform Y into view space
	pViewPos->y = viewTransform._12 * position.x +
		viewTransform._22 * position.y +
		viewTransform._32 * position.z + viewTransform._42;

	//Test against top clip plane
	Dist = ( pViewPos->y * fovProperties.Fov_TopPlane.y ) + ( pViewPos->z * fovProperties.Fov_TopPlane.z );
	if( Dist > sphereRadius )
		return FALSE;

	//Test against bottom plane
	Dist = ( pViewPos->y * fovProperties.Fov_BottomPlane.y ) + ( pViewPos->z * fovProperties.Fov_BottomPlane.z);
	if( Dist > sphereRadius )
		return FALSE;//*/

	//Mesh is inside the field of view
	return TRUE;

}



DLL_API DWORD DEUtility::VectorToRGB(D3DXVECTOR3* NormalVector)
{
	DWORD dwR = (DWORD)(127 * NormalVector->x + 128);
	DWORD dwG = (DWORD)(127 * NormalVector->y + 128);
	DWORD dwB = (DWORD)(127 * NormalVector->z + 128);

	return (DWORD)(0xff000000 + (dwR << 16) + (dwG << 8) + dwB);
}



//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version.
// Arguments: 
//      pdwDirectXVersion - This can be NULL.  If non-NULL, the return value is:
//              0x00000000 = No DirectX installed
//              0x00010000 = DirectX 1.0 installed
//              0x00020000 = DirectX 2.0 installed
//              0x00030000 = DirectX 3.0 installed
//              0x00030001 = DirectX 3.0a installed
//              0x00050000 = DirectX 5.0 installed
//              0x00060000 = DirectX 6.0 installed
//              0x00060100 = DirectX 6.1 installed
//              0x00060101 = DirectX 6.1a installed
//              0x00070000 = DirectX 7.0 installed
//              0x00070001 = DirectX 7.0a installed
//              0x00080000 = DirectX 8.0 installed
//              0x00080100 = DirectX 8.1 installed
//              0x00080101 = DirectX 8.1a installed
//              0x00080102 = DirectX 8.1b installed
//              0x00080200 = DirectX 8.2 installed
//              0x00090000 = DirectX 9.0 installed
//      strDirectXVersion - Destination string to receive a string name of the DirectX Version.  Can be NULL.
//      cchDirectXVersion - Size of destination buffer in characters.  Length should be at least 10 chars.
// Returns: S_OK if the function succeeds.  
//          E_FAIL if the DirectX version info couldn't be determined.
//
// Please note that this code is intended as a general guideline. Your
// app will probably be able to simply query for functionality (via
// QueryInterface) for one or two components.
//
// Also please ensure your app will run on future releases of DirectX.
// For example:
//     "if( dwDirectXVersion != 0x00080100 ) return false;" is VERY BAD. 
//     "if( dwDirectXVersion < 0x00080100 ) return false;" is MUCH BETTER.
//-----------------------------------------------------------------------------
DLL_API HRESULT DEUtility::GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion )
{
    bool bGotDirectXVersion = false;  

    // Init values to unknown
    if( pdwDirectXVersion )
        *pdwDirectXVersion = 0;
    if( strDirectXVersion && cchDirectXVersion > 0 )
        strDirectXVersion[0] = 0;

    DWORD dwDirectXVersionMajor = 0;
    DWORD dwDirectXVersionMinor = 0;
    TCHAR cDirectXVersionLetter = ' ';

    // First, try to use dxdiag's COM interface to get the DirectX version.  
    // The only downside is this will only work on DX9 or later.
    if( SUCCEEDED( GetDirectXVersionViaDxDiag( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter ) ) )
        bGotDirectXVersion = true;
  
    if( !bGotDirectXVersion )
    {
        // Getting the DirectX version info from DxDiag failed, 
        // so most likely we are on DX8.x or earlier
        if( SUCCEEDED( GetDirectXVersionViaFileVersions( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter ) ) )
            bGotDirectXVersion = true;
    }

    // If both techniques failed, then return E_FAIL
    if( !bGotDirectXVersion )
        return E_FAIL;
        
    // Set the output values to what we got and return       
    cDirectXVersionLetter = (char)tolower(cDirectXVersionLetter);
    
    if( pdwDirectXVersion )
    {
        // If pdwDirectXVersion is non-NULL, then set it to something 
        // like 0x00080102 which would represent DX8.1b
        DWORD dwDirectXVersion = dwDirectXVersionMajor;
        dwDirectXVersion <<= 8;
        dwDirectXVersion += dwDirectXVersionMinor;
        dwDirectXVersion <<= 8;
        if( cDirectXVersionLetter >= 'a' && cDirectXVersionLetter <= 'z' )
            dwDirectXVersion += (cDirectXVersionLetter - 'a') + 1;
        
        *pdwDirectXVersion = dwDirectXVersion;
    }
    
    if( strDirectXVersion && cchDirectXVersion > 0 )
    {
        // If strDirectXVersion is non-NULL, then set it to something 
        // like "8.1b" which would represent DX8.1b
        if( cDirectXVersionLetter == ' ' )
            _sntprintf( strDirectXVersion, cchDirectXVersion, TEXT("%d.%d"), dwDirectXVersionMajor, dwDirectXVersionMinor );
        else
            _sntprintf( strDirectXVersion, cchDirectXVersion, TEXT("%d.%d%c"), dwDirectXVersionMajor, dwDirectXVersionMinor, cDirectXVersionLetter );
        strDirectXVersion[cchDirectXVersion-1] = 0;
    }
    
   return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaDxDiag()
// Desc: Tries to get the DirectX version from DxDiag's COM interface
//-----------------------------------------------------------------------------
DLL_API HRESULT DEUtility::GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor,  TCHAR* pcDirectXVersionLetter )
{
    HRESULT hr;
    bool bCleanupCOM = false;

    bool bSuccessGettingMajor = false;
    bool bSuccessGettingMinor = false;
    bool bSuccessGettingLetter = false;
    
    //Init COM. COM may fail if its already been inited with a different 
    //concurrency model.  And if it fails you shouldn't release it.
    hr = CoInitialize(NULL);
	if(hr==false)
	{
		CoUninitialize();
		CoInitialize(NULL);
	}

    bCleanupCOM = SUCCEEDED(hr);

    // Get an IDxDiagProvider
    bool bGotDirectXVersion = false;
    IDxDiagProvider* pDxDiagProvider = NULL;
    hr = CoCreateInstance( CLSID_DxDiagProvider,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IDxDiagProvider,
                           (LPVOID*) &pDxDiagProvider );
    if( SUCCEEDED(hr) )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
        dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
        dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks        = false;
        dxDiagInitParam.pReserved               = NULL;

        // Init the m_pDxDiagProvider
        hr = pDxDiagProvider->Initialize( &dxDiagInitParam ); 
        if( SUCCEEDED(hr) )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
            if( SUCCEEDED(hr) ) 
            {
                // Get the object called DxDiag_SystemInfo
                hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo );
                if( SUCCEEDED(hr) )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "dwDirectXVersionMajor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMajor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMajor )
                            *pdwDirectXVersionMajor = var.ulVal; 
                        bSuccessGettingMajor = true;
                    }
                    VariantClear( &var );

                    // Get the "dwDirectXVersionMinor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMinor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMinor )
                            *pdwDirectXVersionMinor = var.ulVal; 
                        bSuccessGettingMinor = true;
                    }
                    VariantClear( &var );

                    // Get the "szDirectXVersionLetter" property
                    hr = pDxDiagSystemInfo->GetProp( L"szDirectXVersionLetter", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL )
                    {
#ifdef UNICODE
                        *pcDirectXVersionLetter = var.bstrVal[0]; 
#else
                        char strDestination[10];
                        WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, strDestination, 10*sizeof(CHAR), NULL, NULL );
                        if( pcDirectXVersionLetter )
                            *pcDirectXVersionLetter = strDestination[0]; 
#endif
                        bSuccessGettingLetter = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( bSuccessGettingMajor && bSuccessGettingMinor && bSuccessGettingLetter )
                        bGotDirectXVersion = true;

                    pDxDiagSystemInfo->Release();
                }

                pDxDiagRoot->Release();
            }
        }

        pDxDiagProvider->Release();
    }

    if( bCleanupCOM )
        CoUninitialize();
        
    if( bGotDirectXVersion )
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: GetDirectXVerionViaFileVersions()
// Desc: Tries to get the DirectX version by looking at DirectX file versions
//-----------------------------------------------------------------------------
DLL_API HRESULT DEUtility::GetDirectXVersionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter )
{       
    ULARGE_INTEGER llFileVersion;  
    TCHAR szPath[512];
    TCHAR szFile[512];
    BOOL bFound = false;

    if( GetSystemDirectory( szPath, MAX_PATH ) != 0 )
    {
        szPath[MAX_PATH-1]=0;
            
        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 2, 0, 95 ) ) >= 0 ) // Win9x version
            {                    
                // flle is >= DX1.0 version, so we must be at least DX1.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 1;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 3, 0, 1096 ) ) >= 0 ) // Win9x version
            {                    
                // flle is is >= DX2.0 version, so we must DX2.0 or DX2.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 2;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 68 ) ) >= 0 ) // Win9x version
            {                    
                // flle is is >= DX3.0 version, so we must be at least DX3.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }
        
        // Switch off the d3drg8x.dll version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\d3drg8x.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 70 ) ) >= 0 ) // Win9x version
            {                    
                // d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b  (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }       

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 5, 0, 155 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX5.0 version, so we must be DX5.0 or DX5.2 (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 5;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 318 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX6.0 version, so we must be at least DX6.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 436 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX6.1 version, so we must be at least DX6.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the dplayx.dll version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\dplayx.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 3, 518 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 700 ) ) >= 0 ) // Win9x version
            {                    
                // TODO: find win2k version
                
                // ddraw.dll is the DX7.0 version, so we must be at least DX7.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the dinput version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\dinput.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 716 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX7.0 version, so we must be at least DX7.0a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 0, 400 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2258, 400 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // ddraw.dll is the DX8.0 version, so we must be at least DX8.0 or DX8.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\d3d8.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 1, 881 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2600, 881 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // d3d8.dll is the DX8.1 version, so we must be at least DX8.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 1, 901 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2600, 901 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // d3d8.dll is the DX8.1a version, so we must be at least DX8.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\mpg2splt.ax"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 6, 3, 1, 885 ) ) >= 0 ) // Win9x/Win2k/WinXP version
            {                    
                // quartz.dll is the DX8.1b version, so we must be at least DX8.1b
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('b');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\dpnet.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 9, 0, 134 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 2, 3677, 134 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // dpnet.dll is the DX8.2 version, so we must be at least DX8.2
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 2;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\d3d9.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            // File exists, but be at least DX9
            if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 9;
            if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
            if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
            bFound = true;
        }
    }

    if( !bFound )
    {
        // No DirectX installed
        if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 0;
        if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
        if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFileVersion()
// Desc: Returns ULARGE_INTEGER with a file version of a file, or a failure code.
//-----------------------------------------------------------------------------
DLL_API HRESULT DEUtility::GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion )
{   
    if( szPath == NULL || pllFileVersion == NULL )
        return E_INVALIDARG;

    DWORD dwHandle;
    UINT  cb;
    cb = GetFileVersionInfoSize( szPath, &dwHandle );
    if (cb > 0)
    {
        BYTE* pFileVersionBuffer = new BYTE[cb];
        if( pFileVersionBuffer == NULL )
            return E_OUTOFMEMORY;

        if (GetFileVersionInfo( szPath, 0, cb, pFileVersionBuffer))
        {
            VS_FIXEDFILEINFO* pVersion = NULL;
            if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) && 
                pVersion != NULL) 
            {
                pllFileVersion->HighPart = pVersion->dwFileVersionMS;
                pllFileVersion->LowPart  = pVersion->dwFileVersionLS;
                delete[] pFileVersionBuffer;
                return S_OK;
            }
        }

        delete[] pFileVersionBuffer;
    }

    return E_FAIL;
}


//-----------------------------------------------------------------------------
// Name: MakeInt64()
// Desc: Returns a ULARGE_INTEGER where a<<48|b<<32|c<<16|d<<0
//-----------------------------------------------------------------------------
DLL_API ULARGE_INTEGER DEUtility::MakeInt64( WORD a, WORD b, WORD c, WORD d )
{
    ULARGE_INTEGER ull;
    ull.HighPart = MAKELONG(b,a);
    ull.LowPart = MAKELONG(d,c);
    return ull;
}


//-----------------------------------------------------------------------------
// Name: CompareLargeInts()
// Desc: Returns 1 if ullParam1 > ullParam2
//       Returns 0 if ullParam1 = ullParam2
//       Returns -1 if ullParam1 < ullParam2
//-----------------------------------------------------------------------------
DLL_API int DEUtility::CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 )
{
    if( ullParam1.HighPart > ullParam2.HighPart )
        return 1;
    if( ullParam1.HighPart < ullParam2.HighPart )
        return -1;

    if( ullParam1.LowPart > ullParam2.LowPart )
        return 1;
    if( ullParam1.LowPart < ullParam2.LowPart )
        return -1;

    return 0;
}



// This calculates a vector between 2 points and returns the result
DLL_API D3DXVECTOR3 DEUtility::Vector(D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2)
{
	D3DXVECTOR3 vVector;							// The variable to hold the resultant vector

	vVector.x = vPoint1.x - vPoint2.x;			// Subtract point1 and point2 x's
	vVector.y = vPoint1.y - vPoint2.y;			// Subtract point1 and point2 y's
	vVector.z = vPoint1.z - vPoint2.z;			// Subtract point1 and point2 z's

	return vVector;								// Return the resultant vector
}

// This adds 2 vectors together and returns the result
DLL_API D3DXVECTOR3 DEUtility::AddVector(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2)
{
	D3DXVECTOR3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector2.x + vVector1.x;		// Add Vector1 and Vector2 x's
	vResult.y = vVector2.y + vVector1.y;		// Add Vector1 and Vector2 y's
	vResult.z = vVector2.z + vVector1.z;		// Add Vector1 and Vector2 z's

	return vResult;								// Return the resultant vector
}

// This divides a vector by a single number (scalar) and returns the result
DLL_API D3DXVECTOR3 DEUtility::DivideVectorByScaler(D3DXVECTOR3 vVector1, float Scaler)
{
	D3DXVECTOR3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler

	return vResult;								// Return the resultant vector
}


// This returns the normal of a vector
DLL_API D3DXVECTOR3 DEUtility::Normalize(D3DXVECTOR3 vNormal)
{
	double Magnitude;							// This holds the magitude			

	Magnitude = Mag(vNormal);					// Get the magnitude

	vNormal.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
	vNormal.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
	vNormal.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude

	return vNormal;								// Return the normal
}


DLL_API D3DXVECTOR3 DEUtility::GetTriangleNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3)
{
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;

    D3DXVec3Subtract(&v1, vVertex2, vVertex1);
    D3DXVec3Subtract(&v2, vVertex3, vVertex1);

    D3DXVec3Cross(&vNormal, &v1, &v2);

    D3DXVec3Normalize(&vNormal, &vNormal);

    return vNormal;
}


