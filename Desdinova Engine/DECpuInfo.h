#ifndef H_CPUINFO_H
#define H_CPUINFO_H

enum ECPUVendor
{
	CPUV_Cyrix = 1,
	CPUV_AMD,
	CPUV_Intel,
	CPUV_Other,
	CPUV_NumCPUV
};

// I dont go below 486 processors because the cpuid instruction is not supported on processors older 
// then a 486. Besides I doubt anyone playing games on a PC will have anything lower then a pentium.
enum ECPUFamily
{
	CPUF_Intel486 = 1,
	CPUF_IntelP1,			// First pentium 1
	CPUF_IntelPMMX,			// Pentium one with MMX supported
	CPUF_IntelPPro,			// Pentium pro > Pentium 1
	CPUF_IntelP2,
	CPUF_IntelP3,
	CPUF_IntelP4,
	CPUF_IntelP6Model,		// If the processor is above a Pentium Pro but not a P4.

	CPUF_AMDUnknown,
	CPUF_AMD486,
	CPUF_AMD5x86,
	CPUF_AMDK5,
	CPUF_AMDK6,
	CPUF_AMDK7,
	CPUF_AMDK7Plus,

	CPUF_Other
};

enum ECPUType
{
	CPUT_StandardOEM = 1,
	CPUT_Overdrive,
	CPUT_Dual,
	CPUT_Reserved,
	CPUT_Other,
	CPUT_NumCPUT
};


// These values can be passed into the GetCPUCaps function in the SystemInfo class
enum ECPUCaps
{
	CPUC_FPU = 1,		// Floating point unit
	CPUC_VME,			// Virtual 8086 mode enhancements
	CPUC_DE,			// Debugging extentions
	CPUC_PSE,			// Page size extentions
	CPUC_TSC,			// Time stamp counter
	CPUC_MSR,			// Model specific registers
	CPUC_PAE,			// Physical address extention
	CPUC_MCE,			// Machine check exception
	CPUC_CX8,			// COMPXCHG8B Instruction
	CPUC_APIC,			// Advanced programmable interrupt controller
	CPUC_SEP,			// Fast system call
	CPUC_MTRR,			// Memory type range register
	CPUC_PGE,			// PTE global flag
	CPUC_MCA,			// Machine check architecture
	CPUC_CMOV,			// Conditional move and compare instruction
	CPUC_FGPAT,			// FG page attribute table
	CPUC_PSE36,			// 36 bit page size extentions
	CPUC_PN,			// Processor serial number
	CPUC_CLFSH,			// CFLUSH instruction
	CPUC_DS,			// Debug store
	CPUC_ACPI,			// ACPI thermal monitor and clock control
	CPUC_EMMX,			// Extended multimedia extentions
	CPUC_MMX,			// Multimedia extentions
	CPUC_FXSR,			// Fast streaming SIMD exntentions save and restore
	CPUC_SSE,			// Streaming SIMD ententions
	CPUC_SSE2,			// Streaming SIMD2 extentions	
	CPUC_SS,			// Self snoop
	CPUC_HT,			// Hyper threading
	CPUC_TM,			// Thermal monitor
	CPUC_IA64,			// Intel 64 bit architecture
	CPUC_3DNOW,			// 3DNOW extentions
	CPUC_E3DNOW,		// Extended 3DNOW extentions
	CPUC_AA64,			// AMD 64 bit architecture
	CPUC_APICID,		// The APIC ID number
	CPUC_CLFLUSHSize,	// CFLUSH instruction flush size
	CPUC_HTSiblings,	// Number of Hyper threading sibblings
	CPUC_NumCPUC, // 37       - 1
};

class DECpuInfo
{
	public:

		// Used internally to store the CPU's features.
		struct SProcessorCapabilities
		{
			bool bFloatingPointUnit;
			bool bVirtual8086ModeEnhancements;
			bool bDebuggingExtensions;
			bool bPageSizeExtensions;
			bool bTimeStampCounter;
			bool bModelSpecificRegisters;
			bool bPhysicalAddressExtension;
			bool bMachineCheckException;
			bool bCOMPXCHG8BInstruction;
			bool bAdvancedProgrammableInterruptController;
			bool bSEPFastSystemCall;
			bool bMemoryTypeRangeRegisters;
			bool bPTEGlobalFlag;
			bool bMachineCheckArchitecture;
			bool bConditionalMoveAndCompareInstructions;
			bool bFGPageAttributeTable;
			bool b36bitPageSizeExtension;
			bool bProcessorSerialNumber;
			bool bCFLUSHInstruction;
			bool bDebugStore;
			bool bACPIThermalMonitorAndClockControl;
			bool bMultimediaExtensionsExt;
			bool bMultimediaExtensions;
			bool bFastStreamingSIMDExtensionsSaveRestore;
			bool bStreamingSIMDExtensions;
			bool bStreamingSIMD2Extensions;
			bool bSelfSnoop;
			bool bHyperThreading;
			bool bThermalMonitor;
			bool bIntel64BitArchitecture;
			bool b3DNOWInstructionExtensions;
			bool b3DNOWInstructionExtensionsExt;
			bool bAMD64BitArchitecture;
			unsigned long uiAPIC_ID;
			unsigned long uiCLFLUSHInstructionCacheLineSize;
			unsigned long uiHyperThreadingSiblings;
		};


		// Information about the processor on the system. 
		struct SProcessorInformation
		{
			unsigned int	uiSpeedHz;
			int				iProcessorVendor;
			int				iProcessorFamily;
			int				iProcessorType;
			unsigned long	dwMaxSupportedCPUID;
			unsigned long	dwMaxSupportedCPUIDExt;

			char			strProcessorName[32];
			char			strProcessorType[32];
			char			strProcessorFamily[64];
			char			strProcessorSerial[64];
		};

		//** Funzioni **
		DLL_API DECpuInfo();
		DLL_API ~DECpuInfo();
		DLL_API bool						SetRequiredCPUTypes( int iFirst, ... );
		DLL_API bool						SetRequiredCPUVendors( int iFirst, ... );
		DLL_API void						SetMinimumCPUSpeedMHz( float fSpeed ) {mfMinCPUSpeedMHz = fSpeed;}
		DLL_API void						SetMinCPUFamily( ECPUFamily minIntel, ECPUFamily minAMD ) {miMinAMDCPUFamily = minAMD;miMinIntelCPUFamily = minIntel;}
		DLL_API void						SetMinRequiredRAM( int iRAM ){miMinRAM = iRAM;}
		DLL_API bool						AnalyzeSystem();
		DLL_API bool						CPUIDAvailable();
		DLL_API unsigned int				CPUSpeedHz( unsigned int time = 1000 );
		DLL_API int							CPUVendor( char* strCPUName );
		DLL_API int							CPUFamily( char* strFamily );
		DLL_API int							CPUType( char* strType );
		DLL_API bool						RDTSCAvailable();
		DLL_API unsigned long				MaxCPUIDInstruction();
		DLL_API unsigned long				MaxCPUIDInstructionExt();
		DLL_API int							ProcessorSerialNum( char* strSerial );
		DLL_API bool						FillCPUCapabilities( SProcessorCapabilities* pc );
		DLL_API bool						FillCPUInformation( SProcessorInformation* pi );
		DLL_API unsigned long				GetCPUCaps( ECPUCaps cpuc );
		DLL_API SProcessorInformation*		GetCPUInfoStructure()  { return &mProcessorInfo; }
		DLL_API	SProcessorCapabilities*		GetCPUCapsStructure()  { return &mProcessorCaps; }
		DLL_API void						CPUInfoToString( char* strOut, int iSize );

	private:		

		SProcessorCapabilities				mProcessorCaps;
		SProcessorInformation				mProcessorInfo;
		MEMORYSTATUSEX						mMemoryStatus;
		char								mstrOSName[128];
		int									miOSID;

		bool								mRequiredCPUVs[CPUV_NumCPUV];
		bool								mRequiredCPUTs[CPUT_NumCPUT];
		int									miMinIntelCPUFamily;
		int									miMinAMDCPUFamily;
		int									miMinRAM;
		float								mfMinCPUSpeedMHz;
};

#endif