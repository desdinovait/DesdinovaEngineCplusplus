#include "DEGeneralIncludes.h"

using std::endl;

// Just returns the tick count on the processor.
__int64 GetACPUTick()
{
	__asm rdtsc;
}

// Checks if a bit in an object is set (1) or not set (0)
template<class type>
inline bool IsBitSet( type t, unsigned int bit )
{
	if( ( t >> bit ) & 1 ) 
		return true;

	return false;
}


DLL_API DECpuInfo::DECpuInfo()
{
	memset( &mProcessorCaps, 0, sizeof(SProcessorCapabilities) );
	memset( &mProcessorInfo, 0, sizeof(SProcessorInformation) );
	memset( &mMemoryStatus,  0, sizeof(MEMORYSTATUSEX) );
	memset( mRequiredCPUVs,  0, CPUV_NumCPUV );
	memset( mRequiredCPUTs,  0, CPUT_NumCPUT );
	mfMinCPUSpeedMHz =       0;
	miMinAMDCPUFamily =		 0;
	miMinIntelCPUFamily =	 0;
	miMinRAM =			     0;
	miOSID =				 0;

	SetMinCPUFamily( CPUF_Intel486, CPUF_AMD486 );
	SetMinimumCPUSpeedMHz( 0 );
	SetMinRequiredRAM( 0 );
	SetRequiredCPUTypes( 0 );
	SetRequiredCPUVendors( 0 );
}


DLL_API bool DECpuInfo::SetRequiredCPUTypes( int iFirst, ... )
{
	if( iFirst == 0 )
	{
		memset( mRequiredCPUTs, true, CPUT_NumCPUT );
		return true;
	}

	int i = iFirst, count = 0;
	va_list vl;

	va_start( vl, iFirst );

	while( i != 0 )
	{
		if( count++ >= CPUT_NumCPUT || i >= CPUT_NumCPUT )
			return false;

		mRequiredCPUTs[i - 1] = true;
		i = va_arg( vl, int );
	}

	va_end( vl );

	return true;
}

DLL_API bool DECpuInfo::SetRequiredCPUVendors( int iFirst, ... )
{
	if( iFirst == 0 )
	{
		memset( mRequiredCPUVs, true, CPUV_NumCPUV );
		return true;
	}

	int i = iFirst, count = 0;
	va_list vl;

	va_start( vl, iFirst );

	while( i != 0 )
	{
		if( count++ >= CPUV_NumCPUV || i >= CPUV_NumCPUV )
			return false;

		mRequiredCPUVs[i - 1] = true;
		i = va_arg( vl, int );
	}

	va_end( vl );

	return true;
}


DLL_API DECpuInfo::~DECpuInfo()
{
}

DLL_API bool DECpuInfo::AnalyzeSystem()
{
	int i;
	bool bSucceeded = true;


	FillCPUInformation( &mProcessorInfo );
	FillCPUCapabilities( &mProcessorCaps );


	// Make sure memry requirementrs are met
	if( ( mMemoryStatus.ullTotalPhys / 1000000 ) < miMinRAM )
		bSucceeded = false;



	// The the vendor is intel or AMD, make sure their requirements are met
	if( mProcessorInfo.iProcessorVendor == CPUV_Intel )
	{
		if( mProcessorInfo.iProcessorFamily < miMinIntelCPUFamily )
			bSucceeded = false;
	}
	else if( mProcessorInfo.iProcessorVendor == CPUV_AMD )
	{
		if( mProcessorInfo.iProcessorFamily < miMinAMDCPUFamily )
			bSucceeded = false;
	}



	// Next check if the CPU speed is as required
	if( ( mProcessorInfo.uiSpeedHz / 1000000.0f ) < mfMinCPUSpeedMHz )
		bSucceeded = false;



	// Now check that the processor type matches what we support.
	bool bFoundCPUType = false;
	for( i = 0; i < CPUT_NumCPUT; i++ )
	{
		if( mRequiredCPUTs[i] )
		{
			if( mProcessorInfo.iProcessorType == i + 1 )
			{	
				bFoundCPUType = true;
				break;
			}
		}
	}

	if( bFoundCPUType == false )
		bSucceeded = false;



	// Now check the the processor is either an intel, amd, cyrix, etc...
	bool bFoundCPUVendor = false;
	for( i = 0; i < CPUV_NumCPUV; i++ )
	{
		if( mRequiredCPUVs[i] )
		{
			if( mProcessorInfo.iProcessorVendor == i + 1 )
			{	
				bFoundCPUVendor = true;
				break;
			}
		}
	}

	if( bFoundCPUVendor == false )
		bSucceeded = false;





	// If something didnt match then bSucceeded will be false.
	return bSucceeded;
}


DLL_API bool DECpuInfo::CPUIDAvailable()
{
	enum { AVAILABLE, NOT_AVAILABLE };

	int iCpuid = NOT_AVAILABLE;
	_asm 
	{
		pushfd					// Get original EFLAGS
		pop		eax
		mov 	ecx, eax
		xor     eax, 200000h	// Flip ID bit in EFLAGS
		push    eax				// Save new EFLAGS value on stack
		popfd					// Replace current EFLAGS value
		pushfd					// Get new EFLAGS
		pop     eax				// Store new EFLAGS in EAX
		xor     eax, ecx		// Can not toggle ID bit,
		jz     supported		// Processor = 80486
		
		mov iCpuid, AVAILABLE

	supported:
	}

	return ( iCpuid == AVAILABLE ? true : false );
}


DLL_API int DECpuInfo::CPUVendor( char* strCPUName )
{
	if( !CPUIDAvailable() )
		return 0;

	long name[12] = {0};
	ECPUVendor processor;

	__asm
	{
		push	ebx	// ELP

		xor     eax, 0

		cpuid					// Get and save vendor ID

		mov     dword ptr name, ebx
		mov     dword ptr name[+4], edx
		mov     dword ptr name[+8], ecx

		pop		ebx
	}

	// strncpy( strCPUName, (const char*)name, 12 );
	strcpy( strCPUName, (const char*)name );

	// Check for string name in cpuName
	if( strstr( strCPUName, "Intel" ) )
	{
		processor = CPUV_Intel;
	}
	else if( strstr( strCPUName, "AMD" ) )
	{
		processor = CPUV_AMD;
	}
	else if( strstr( strCPUName, "Cyrix" ) )
	{
		processor = CPUV_Cyrix;
	}
	else
	{
		processor = CPUV_Other;
	}

	return processor;
}


DLL_API int DECpuInfo::CPUType( char* strType )
{
	if( !CPUIDAvailable() )
		return 0;

	// get cpuid info at level 1
	unsigned long dwEAX;

	int iType = 0;

	// Get the eax register for CPU iFamily info
	__asm
	{
		mov eax, 1
		cpuid
		mov dwEAX, eax
	}

	iType = ( dwEAX >> 12 ) & 0x3;

	switch( iType )
	{
	case 0:
		strcpy( strType, "Standard OEM" );
		iType = CPUT_StandardOEM;
		break;
	case 1:
		strcpy( strType, "Overdrive" );
		iType = CPUT_Overdrive;
		break;
	case 2:
		strcpy( strType, "Dual" );
		iType = CPUT_Dual;
		break;
	case 3:
		strcpy( strType, "Reserved" );
		iType = CPUT_Reserved;
		break;
	default:
		strcpy( strType, "Unknown" );
		iType = 0;
	}
	

	return iType;
}


DLL_API unsigned int DECpuInfo::CPUSpeedHz( unsigned int time )
{
	if( !RDTSCAvailable() )
		return 0;

	__int64 timeStart, timeStop;
	__int64 startTick, endTick;

	__int64 overhead = GetACPUTick() - GetACPUTick();

	timeStart = timeGetTime();
	while( timeGetTime() == timeStart ) timeStart = timeGetTime();	

	while(1)
	{
		timeStop = timeGetTime();
		if ( (timeStop - timeStart) > 1 )	
		{
			startTick = GetACPUTick();
			break;
		}
	}

	timeStart = timeStop;
	while(1)
	{
		timeStop = timeGetTime();
		if ( (timeStop - timeStart) > time )	
		{
			endTick = GetACPUTick();
			break;
		}
	}

	return (unsigned int)( ( endTick - startTick ) + ( overhead ) );
}


DLL_API int DECpuInfo::CPUFamily( char* strFamily )
{
	if( !CPUIDAvailable() )
		return 0;

	// get cpuid info at level 1
	unsigned long dwEAX;

	int iFamily = 0, iModel;
	char tmp[36];
	int vendor = CPUVendor( tmp );

	// Get the eax register for CPU iFamily info
	__asm
	{
		mov eax, 1
		cpuid
		mov dwEAX, eax
	}

	iFamily = (dwEAX >> 8) & 0xF;		// This is 0xF (15) only for intel p4 family
	iModel  = (dwEAX >> 4) & 0xF;

	if( vendor == CPUV_Intel )
	{
		switch( iFamily )
		{		
			if( iFamily >= 7 && iFamily < 15 )
			{
				iFamily = CPUF_IntelP6Model;
				strcpy( strFamily, "Intel P6 family (Not PIV and Higher then PPro" );
				break;
			}

			// Start at 486 becuase if it's below 486 there is no cpuid instruction
			case 4:
				iFamily = CPUF_Intel486;
				strcpy( strFamily, "Intel 486" );
				break;
			case 5:		
				switch( iModel )
				{
				case 4:
				case 8:		// 0.25 µm
					iFamily = CPUF_IntelPMMX;
					strcpy( strFamily, "Intel Pentium (MMX)" );
					break;
				default:
					iFamily = CPUF_IntelP1;
					strcpy( strFamily, "Intel Pentium" );
				}
				break;
			case 6:		
				switch( iModel )
				{
				case 0:		// Pentium pro (P6 A-Step)
				case 1:		// Pentium pro
					iFamily = CPUF_IntelPPro;
					strcpy( strFamily, "Intel Pentium Pro" );
					break;

				case 2:		// 66 MHz FSB
				case 5:		// Xeon/Celeron (0.25 µm)
				case 6:		// Internal L2 cache
					iFamily = CPUF_IntelP2;
					strcpy( strFamily, "Intel Pentium II" );
					break;

				case 7:		// Xeon external L2 cache
				case 8:		// Xeon/Celeron with 256 KB on-die L2 cache
				case 10:	// Xeon/Celeron with 1 or 2 MB on-die L2 cache
				case 11:	// Xeon/Celeron with Tualatin core, on-die cache
					iFamily = CPUF_IntelP3;
					strcpy( strFamily, "Intel Pentium III" );
					break;

				default:
					iFamily = CPUF_IntelPPro;
					strcpy( strFamily, "Intel Pentium Pro (Unknown)" );
				}
				break;
			case 15:
				switch( iModel )
				{
				case 0:		// Willamette (A-Step)
				case 1:		// Willamette 
				case 2:		// Northwood 

					/*char cpuString[48];
					__asm
					{
						mov eax, 0x80000002
						cpuid

						mov dword ptr cpuString, eax
						mov dword ptr cpuString[+4], ebx
						mov dword ptr cpuString[+8], ecx
						mov dword ptr cpuString[+12], edx
						
						mov eax, 0x80000003
						cpuid

						mov dword ptr cpuString[+16], eax
						mov dword ptr cpuString[+20], ebx
						mov dword ptr cpuString[+24], ecx
						mov dword ptr cpuString[+28], edx
						
						mov eax, 0x80000004
						cpuid

						mov dword ptr cpuString[+32], eax
						mov dword ptr cpuString[+36], ebx
						mov dword ptr cpuString[+40], ecx
						mov dword ptr cpuString[+44], edx
					}*/

					iFamily = CPUF_IntelP4;
					strcpy( strFamily, "Intel Pentium IV" );
					break;

				default:
					iFamily = CPUF_IntelP4;
					strcpy( strFamily, "Intel Pentium IV (Unknown)" );
					break;
				}
				break;
			default:
				return 0;
		}
	}
	else if( vendor == CPUV_AMD )
	{
		switch (iFamily)
		{
			if( iFamily >= 7 )
			{
				iFamily = CPUF_AMDK7Plus;
				strcpy( strFamily, "AMD K7+" );
				break;
			}

			case 4:
				switch( iModel )
				{
				case 14: 
				case 15:		// Write-back enhanced
					iFamily = CPUF_AMD5x86;
					strcpy( strFamily, "AMD 5x86" );
					break;

				case 3:			// DX2
				case 7:			// Write-back enhanced DX2
				case 8:			// DX4
				case 9:			// Write-back enhanced DX4
					iFamily = CPUF_AMD486;
					strcpy( strFamily, "AMD 486" );
					break;

				default:
					iFamily = CPUF_AMDUnknown;
					strcpy( strFamily, "AMD Unknown" );

				}
				break;

			case 5:		
				switch( iModel )
				{
				case 0:		// SSA 5 (75, 90 and 100 Mhz)
				case 1:		// 5k86 (PR 120 and 133 MHz)
				case 2:		// 5k86 (PR 166 MHz)
				case 3:		// K5 5k86 (PR 200 MHz)
					iFamily = CPUF_AMDK5;
					strcpy( strFamily, "AMD K5" );
					break;

				case 6:		
				case 7:		// (0.25 µm)
				case 8:		// K6-2
				case 9:		// K6-III
				case 14:	// K6-2+ / K6-III+
					iFamily = CPUF_AMDK6;
					strcpy( strFamily, "AMD K6" );
					break;

				default:
					iFamily = CPUF_AMDUnknown;
					strcpy( strFamily, "AMD Unknown" );
				}
				break;
			case 6:		
				iFamily = CPUF_AMDK7;
				strcpy( strFamily, "AMD K7" );
				break;
			default:
				return 0;
		}
	}
	else if( vendor == (int)CPUV_Cyrix )
	{
		iFamily = CPUF_Other;
		strcpy( strFamily, "Cyrix" );
	}
	else
	{
		iFamily = CPUF_Other;
		strcpy( strFamily, "Unknown" );
	}

	return iFamily;
}


DLL_API bool DECpuInfo::RDTSCAvailable()
{
	if( !CPUIDAvailable() )
		return false;

	unsigned long dwFeat;

	__asm
	{
		mov eax, 1
		cpuid
		mov dwFeat, edx
	}

	// If bit 4 is set in edx then rdtsc is available
	return IsBitSet( dwFeat, 4 );
}


DLL_API unsigned long DECpuInfo::MaxCPUIDInstruction()
{
	if( !CPUIDAvailable() )
		return -1;

	unsigned long dwEAX;

	__asm
	{
		mov eax, 0
		cpuid
		mov dwEAX, eax
	}

	return  dwEAX & 0xFFFF;
}


DLL_API unsigned long DECpuInfo::MaxCPUIDInstructionExt()
{
	if( !CPUIDAvailable() )
		return -1;

	unsigned long dwEAX;

	__asm
	{
		mov eax, 0x80000000
		cpuid
		mov dwEAX, eax
	}
	
	return dwEAX;
}


DLL_API int DECpuInfo::ProcessorSerialNum( char* strSerial )
{
	if( !CPUIDAvailable() )
		return 0;
	
	if( MaxCPUIDInstruction() >= 3 )
	{
		unsigned long p1, p2, p3;

		__asm
		{
			mov eax, 1
			cpuid
			mov p1, eax
			mov eax, 3
			cpuid
			mov p2, ecx
			mov p3, edx
		}

		std::strstream buff;

		// Make the data a string
		buff<< ( p1 >> 16)<< "-"
			<< ( p1 & 0xFFFF )<< "-"
			<< ( p3 >> 16 )<< "-"
			<< ( p3 & 0xFFFF )<< "-"
			<< ( p2 >> 16 )<< "-"
			<< ( p2 & 0xFFFF );

		strcpy( strSerial, buff.str() );

		return 1;
	}
	else
	{
		strcpy( strSerial, "Serial number not available" );
        
		return 0;
	}
}


DLL_API bool DECpuInfo::FillCPUInformation( SProcessorInformation* pi )
{
	bool bSucceeded = true;

	if( ( pi->dwMaxSupportedCPUID = MaxCPUIDInstruction() ) == -1 )
		bSucceeded = false;

	if(	( pi->dwMaxSupportedCPUIDExt = MaxCPUIDInstructionExt() ) == -1 ) 
		bSucceeded = false;

	if( !( pi->iProcessorFamily = CPUFamily( pi->strProcessorFamily ) ) )
		bSucceeded = false;

	if( !( pi->iProcessorType = CPUType( pi->strProcessorType ) ) )
		bSucceeded = false;

	if( !( pi->iProcessorVendor = CPUVendor( pi->strProcessorName ) ) )
		bSucceeded = false;

	if( !( ProcessorSerialNum( pi->strProcessorSerial ) ) )
		bSucceeded = false;

	if( !( pi->uiSpeedHz = CPUSpeedHz( 1000 ) ) )
		bSucceeded = false;		

	return bSucceeded;
}



DLL_API bool DECpuInfo::FillCPUCapabilities( SProcessorCapabilities *pc )
{
	if( !CPUIDAvailable() )
		return false;

	unsigned long dwEDX, dwEBX;

	// Call cpuid with standard feature set.
	__asm
	{
		mov eax, 1
		cpuid
		mov dwEDX, edx
		mov dwEBX, ebx
	}

	// Then we mask some bits
	pc->bFloatingPointUnit							= IsBitSet( dwEDX, 0 );
	pc->bVirtual8086ModeEnhancements				= IsBitSet( dwEDX, 1 );
	pc->bDebuggingExtensions						= IsBitSet( dwEDX, 2 );
	pc->bPageSizeExtensions							= IsBitSet( dwEDX, 3 );
	pc->bTimeStampCounter							= IsBitSet( dwEDX, 4 );
	pc->bModelSpecificRegisters						= IsBitSet( dwEDX, 5 );
	pc->bPhysicalAddressExtension					= IsBitSet( dwEDX, 6 );
	pc->bMachineCheckArchitecture					= IsBitSet( dwEDX, 7 );
	pc->bCOMPXCHG8BInstruction						= IsBitSet( dwEDX, 8 );
	pc->bAdvancedProgrammableInterruptController	= IsBitSet( dwEDX, 9 );
	pc->bSEPFastSystemCall							= IsBitSet( dwEDX, 11 );
	pc->bMemoryTypeRangeRegisters					= IsBitSet( dwEDX, 12 );
	pc->bPTEGlobalFlag								= IsBitSet( dwEDX, 13 );
	pc->bMachineCheckArchitecture					= IsBitSet( dwEDX, 14 );
	pc->bConditionalMoveAndCompareInstructions		= IsBitSet( dwEDX, 15 );
	pc->bFGPageAttributeTable						= IsBitSet( dwEDX, 16 );
	pc->b36bitPageSizeExtension						= IsBitSet( dwEDX, 17 );
	pc->bProcessorSerialNumber						= IsBitSet( dwEDX, 18 );
	pc->bCFLUSHInstruction							= IsBitSet( dwEDX, 19 );
	pc->bDebugStore									= IsBitSet( dwEDX, 21 );
	pc->bACPIThermalMonitorAndClockControl			= IsBitSet( dwEDX, 22 );
	pc->bMultimediaExtensions						= IsBitSet( dwEDX, 23 );
	pc->bFastStreamingSIMDExtensionsSaveRestore		= IsBitSet( dwEDX, 24 );
	pc->bStreamingSIMDExtensions					= IsBitSet( dwEDX, 25 );
	pc->bStreamingSIMD2Extensions					= IsBitSet( dwEDX, 26 );
	pc->bSelfSnoop									= IsBitSet( dwEDX, 27 );
	pc->bHyperThreading								= IsBitSet( dwEDX, 28 );
	pc->bThermalMonitor								= IsBitSet( dwEDX, 29 );
	pc->bIntel64BitArchitecture						= IsBitSet( dwEDX, 30 );

	pc->uiAPIC_ID									= ( dwEBX >> 24 ) & 0xFF;
	pc->uiCLFLUSHInstructionCacheLineSize			= ( dwEBX >> 8  ) & 0xFF;
	pc->uiHyperThreadingSiblings					= ( dwEBX >> 16 ) & 0xFF;


	if( MaxCPUIDInstructionExt() >= 0x80000001 )
	{
		char tmp[36];
		int vendor = CPUVendor( tmp );
		unsigned long dwEDX;
		__asm
		{
			mov eax, 0x80000001
			cpuid
			mov dwEDX, edx
		}

		if( vendor == CPUV_AMD )
		{
			pc->bMultimediaExtensionsExt			= IsBitSet( dwEDX, 22 );
			pc->bAMD64BitArchitecture				= IsBitSet( dwEDX, 29 );
			pc->b3DNOWInstructionExtensionsExt		= IsBitSet( dwEDX, 30 );
			pc->b3DNOWInstructionExtensions			= IsBitSet( dwEDX, 31 );
		}
		else
		{
			pc->bMultimediaExtensionsExt			= false;
			pc->bAMD64BitArchitecture				= false;
			pc->b3DNOWInstructionExtensionsExt		= false;
			pc->b3DNOWInstructionExtensions			= false;
		}
	}		

	return true;
}

DLL_API unsigned long DECpuInfo::GetCPUCaps( ECPUCaps cpuc )
{
	__int64 iAddress = (__int64)&mProcessorCaps;
	int border =  CPUC_APICID;
	
	if( cpuc >= border && cpuc < CPUC_NumCPUC )
	{
		int iBytesMore = sizeof(long) * ( cpuc - border  ) - 1;
		iAddress += ( border - 1 ) + iBytesMore + sizeof(long);
		return ( *(unsigned long*)iAddress );
	}
	else
	{
		iAddress += ( cpuc - 1 );
		bool bit = *(bool*)iAddress;			
		return ( bit ? 1 : 0 );
	}

	return -1;
}


DLL_API void DECpuInfo::CPUInfoToString( char* strOut, int iSize )
{
	std::strstream buff;

	buff<< "\t*****************************"<< endl
		<< "\t*   Processor Information   *"<< endl
		<< "\t*****************************\n\n";

	buff<< "CPUID: \t\t\t"<< ( CPUIDAvailable() ? "Supported" : "Not supported" )<< endl;
	buff<< "Max CPUID Standard: \t"<< MaxCPUIDInstruction()<< endl;
	buff<< "Max CPUID Extended: \t"<< MaxCPUIDInstructionExt() - 0x80000001 << endl;
	buff<< "CPU vender ID: \t\t"<< mProcessorInfo.iProcessorVendor<< endl;
	buff<< "CPU vender name: \t"<< mProcessorInfo.strProcessorName<< endl;	
	buff<< "CPU Speed: \t\t"<< mProcessorInfo.uiSpeedHz / 1000000.0 << "MHz"<< endl;		
	buff<< "CPU family ID: \t\t"<< mProcessorInfo.iProcessorFamily<< endl;
	buff<< "CPU family: \t\t"<< mProcessorInfo.strProcessorFamily<< endl;
	buff<< "CPU type ID: \t\t"<< mProcessorInfo.iProcessorType<< endl;
	buff<< "CPU type: \t\t"<< mProcessorInfo.strProcessorType<< endl;
	buff<< "CPU serial: \t\t"<< mProcessorInfo.strProcessorSerial<< endl;


	buff<< "\n\tCPU Features:-\n";
	buff<< "\n\t36 byte page extensions "<< ( mProcessorCaps.b36bitPageSizeExtension ? "detected" : "not detected" );
	buff<< "\n\t3DNOW instructions "<< ( mProcessorCaps.b3DNOWInstructionExtensions ? "detected" : "not detected" );
	buff<< "\n\tExtended 3DNOW instructions "<< ( mProcessorCaps.b3DNOWInstructionExtensionsExt ? "detected" : "not detected" );
	buff<< "\n\tThermal monitor and clock control "<< ( mProcessorCaps.bACPIThermalMonitorAndClockControl ? "detected" : "not detected" );
	buff<< "\n\tAdvanced programmable interrupt controller "<< ( mProcessorCaps.bAdvancedProgrammableInterruptController ? "detected" : "not detected" );
	buff<< "\n\tAMD 64 bit architecture "<< ( mProcessorCaps.bAMD64BitArchitecture ? "detected" : "not detected" );
	buff<< "\n\tCFLUSH instruction "<< ( mProcessorCaps.bCFLUSHInstruction ? "detected" : "not detected" );
	buff<< "\n\tCOMPXCHG8B instructions "<< ( mProcessorCaps.bCOMPXCHG8BInstruction ? "detected" : "not detected" );
	buff<< "\n\tCMOV instruction "<< ( mProcessorCaps.bConditionalMoveAndCompareInstructions ? "detected" : "not detected" );
	buff<< "\n\tDebugging extentions "<< ( mProcessorCaps.bDebuggingExtensions ? "detected" : "not detected" );
	buff<< "\n\tDebug store "<< ( mProcessorCaps.bDebugStore ? "detected" : "not detected" );
	buff<< "\n\tFXSR instruction "<< ( mProcessorCaps.bFastStreamingSIMDExtensionsSaveRestore ? "detected" : "not detected" );
	buff<< "\n\tPage attribute table "<< ( mProcessorCaps.bFGPageAttributeTable ? "detected" : "not detected" );
	buff<< "\n\tFPU "<< ( mProcessorCaps.bFloatingPointUnit ? "detected" : "not detected" );
	buff<< "\n\tHyper threading "<< ( mProcessorCaps.bHyperThreading ? "detected" : "not detected" );
	buff<< "\n\tIntel 64 bit architecture "<< ( mProcessorCaps.bIntel64BitArchitecture ? "detected" : "not detected" );
	buff<< "\n\tMachine check architecture "<< ( mProcessorCaps.bMachineCheckArchitecture ? "detected" : "not detected" );
	buff<< "\n\tMachine check exception "<< ( mProcessorCaps.bMachineCheckException ? "detected" : "not detected" );
	buff<< "\n\tMemory type range resistors "<< ( mProcessorCaps.bMemoryTypeRangeRegisters ? "detected" : "not detected" );
	buff<< "\n\tModel specific registers "<< ( mProcessorCaps.bModelSpecificRegisters ? "detected" : "not detected" );
	buff<< "\n\tMMX "<< ( mProcessorCaps.bMultimediaExtensions ? "detected" : "not detected" );
	buff<< "\n\tEMMX "<< ( mProcessorCaps.bMultimediaExtensionsExt ? "detected" : "not detected" );
	buff<< "\n\tPage size extentions "<< ( mProcessorCaps.bPageSizeExtensions ? "detected" : "not detected" );
	buff<< "\n\tPhysical address extention "<< ( mProcessorCaps.bPhysicalAddressExtension ? "detected" : "not detected" );
	buff<< "\n\tProcessor serial number "<< ( mProcessorCaps.bProcessorSerialNumber ? "detected" : "not detected" );
	buff<< "\n\tGlobal flag "<< ( mProcessorCaps.bPTEGlobalFlag ? "detected" : "not detected" );
	buff<< "\n\tSelf snoop "<< ( mProcessorCaps.bSelfSnoop ? "detected" : "not detected" );
	buff<< "\n\tFast system call "<< ( mProcessorCaps.bSEPFastSystemCall ? "detected" : "not detected" );
	buff<< "\n\tSSE2 "<< ( mProcessorCaps.bStreamingSIMD2Extensions ? "detected" : "not detected" );
	buff<< "\n\tSSE "<< ( mProcessorCaps.bStreamingSIMDExtensions ? "detected" : "not detected" );
	buff<< "\n\tThermal monitor "<< ( mProcessorCaps.bThermalMonitor ? "detected" : "not detected" );
	buff<< "\n\tRDTSC instruction "<< ( mProcessorCaps.bTimeStampCounter ? "detected" : "not detected" );
	buff<< "\n\tVitual 8086 mode "<< ( mProcessorCaps.bVirtual8086ModeEnhancements ? "detected" : "not detected" );
	buff<< "\n\tAPIC ID: "<< mProcessorCaps.uiAPIC_ID;
	buff<< "\n\tCFLUSH cache size: "<< mProcessorCaps.uiCLFLUSHInstructionCacheLineSize;
	buff<< "\n\tHyper threading siblings "<< mProcessorCaps.uiHyperThreadingSiblings;
	//==============================================================================================


	buff<<"\n\n\n";
}
