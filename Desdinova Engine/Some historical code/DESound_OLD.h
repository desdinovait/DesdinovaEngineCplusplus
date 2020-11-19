#ifndef H_SOUND_H
#define H_SOUND_H

#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

/*
[CLASS] 
[
    [NOME] 
    DEWaveFile
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei files wave.
]
*/
class DEWaveFile
{
	private:
		//** Variabili **
		WAVEFORMATEX*			m_pwfx;
		HMMIO					m_hmmio;
		MMCKINFO				m_ck;
		MMCKINFO				m_ckRiff;
		DWORD					m_dwSize;
		MMIOINFO				m_mmioinfoOut;
		DWORD					m_dwFlags;
		BOOL					m_bIsReadingFromMemory;
		BYTE*					m_pbData;
		BYTE*					m_pbDataCur;
		ULONG					m_ulDataSize;
		CHAR*					m_pResourceBuffer;

		//** Funzioni **
		DLL_API HRESULT			ReadMMIO();
		DLL_API HRESULT			WriteMMIO( WAVEFORMATEX *pwfxDest );

	public:
		//** Funzioni **
		DLL_API HRESULT			Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
		DLL_API HRESULT			OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
		DLL_API HRESULT			Close();
		DLL_API void			Release();
		DLL_API HRESULT			ResetFile();

		DLL_API HRESULT			Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
		DLL_API HRESULT			Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

		DLL_API DWORD			GetSize();
		DLL_API WAVEFORMATEX*	GetFormat();
		DLL_API DWORD			GetLength();

};


/*
[CLASS] 
[
    [NOME] 
    DESound
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione delle istanze dei singoli suoni.
]
*/
class DESound
{
	private:
		//** Variabili **
		LPDIRECTSOUNDBUFFER*			m_apDSBuffer;
		DWORD							m_dwDSBufferSize;
		DEWaveFile*						m_pWaveFile;
		DWORD							m_dwNumBuffers;
		DWORD							m_dwCreationFlags;
		LPDIRECTSOUND3DBUFFER			g_pDS3DBuffer;
		DS3DBUFFER						g_dsBufferParams;
		LPCSTR							mainLogTitle;
		LPCSTR							mainTag;
		bool							isCreated;

		//** Funzioni **
		DLL_API HRESULT					RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

	public:
		//** Funzioni **
		DLL_API							DESound();
		DLL_API bool					Create(LPTSTR strWaveFileName,  DWORD dwCreationFlags,  GUID guid3DAlgorithm, DWORD dwNumBuffers, LPCSTR logTitle="DESound" );
		DLL_API bool					Release();
		DLL_API bool					FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
		//Bufferd
		DLL_API DWORD					GetLenght();
		DLL_API LPDIRECTSOUNDBUFFER		GetFreeBuffer();
		DLL_API LPDIRECTSOUNDBUFFER		GetBuffer( DWORD dwIndex );
		DLL_API HRESULT					Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
		//Esecuzione
		DLL_API bool					Play( DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lVolume = 0, LONG lFrequency = -1, LONG lPan = 0 );
		DLL_API bool					Play3D( DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lFrequency = 0 );
		DLL_API bool					Stop();
		DLL_API bool					Reset();
		DLL_API bool					IsSoundPlaying();
		DLL_API bool					GetCreated();
		//Parametri
		DLL_API bool					SetPosition(D3DXVECTOR3 position);
		DLL_API bool					SetVelocity(D3DXVECTOR3 velocity);
		DLL_API bool					SetDistances(float minDistance, float maxDistance);
		DLL_API bool					SetCones(DWORD insideConeAngle, DWORD outsideConeAngle, D3DXVECTOR3 coneOrientation, long outsideConeVolume);
		DLL_API bool					SetMode(DWORD mode);
		DLL_API bool					SetAllParameters(DS3DBUFFER bufferParams);

		DLL_API bool					Invalidate();
		DLL_API bool					Restore();
		//Tag
		DLL_API bool					SetTag(LPCSTR newTag);
		DLL_API LPCSTR					GetTag();

};


/*
[CLASS] 
[
    [NOME] 
    DESoundManager
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione dell'oggetto audio utilizzato generalmente 
	per tutte le istanze dei suoni.
]
*/


class DESoundManager
{
	private:
		//** Variabili **
		LPDIRECTSOUND3DLISTENER		m_pDSListener;
		DS3DLISTENER				g_dsListenerParams;
		DWORD						listenerApplyMode;
		bool						objectCreated;
		bool						listenerCreated;
		STRUCT_DE_SOUNDDEVICE		soundDevices[20];
		HWND						soundDevicesHWND;
		bool						soundDeviceEnum;
		unsigned int				soundDevicesCounter;

	public:
		//** Variabili **
		static LPDIRECTSOUND8		DSDevice;
		
		//** Funzioni **
		DLL_API STRUCT_DE_SOUNDDEVICE *EnumerateDevices();
		DLL_API unsigned int		GetEnumeratedDevices();
		DLL_API bool				Init( HWND hWnd, DWORD dwCoopLevel, unsigned int deviceID=0 );
		DLL_API void				Release();

		DLL_API bool				SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
		DLL_API LPDIRECTSOUND8		GetDirectSound();
		DLL_API	bool				GetCreated();
		//Deferred
		DLL_API bool				ListenerCommitDeferredSettings();
		//Listener
		DLL_API bool				SetListenerImmediateApply(bool immediateApply);
		DLL_API bool				SetListenerPosition(D3DXVECTOR3 position);
		DLL_API bool				SetListenerOrientation(D3DXVECTOR3 frontOrientation, D3DXVECTOR3 topOrientation);
		DLL_API bool				SetListenerVelocity(D3DXVECTOR3 velocity);
		DLL_API bool				SetListenerDistanceFactor(float distanceFactor);
		DLL_API bool				SetListenerDopplerFactor(float dopplerFactor);
		DLL_API bool 				SetListenerRolloffFactor(float rolloffFactor);
		DLL_API bool				SetListenerParameters(LPCDS3DLISTENER listenerParams);
		DLL_API bool				GetListenerImmediateApply();
		DLL_API D3DXVECTOR3			GetListenerPosition();
		DLL_API D3DXVECTOR3			GetListenerOrientation(unsigned int mode);
		DLL_API D3DXVECTOR3 		GetListenerVelocity();
		DLL_API float				GetListenerDistanceFactor();
		DLL_API float				GetListenerDopplerFactor();
		DLL_API float				GetListenerRolloffFactor();
		DLL_API LPDS3DLISTENER		GetListenerParameters();
		DLL_API bool				GetListenerInterface(LPDIRECTSOUND3DLISTENER* ppDSListener);

};

#endif