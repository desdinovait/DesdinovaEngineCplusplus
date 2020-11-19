#include "DEGeneralIncludes.h"
#include "DESound.h"

LPDIRECTSOUND8		DESoundManager::DSDevice=NULL;

DLL_API void DESoundManager::Release()
{
	objectCreated = false;
	listenerCreated = false;
	soundDeviceEnum = false;
    SafeRelease(DSDevice);
}


DLL_API INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName, void* pContext )
{
    // Set aside static storage space for 20 audio drivers
    static GUID  AudioDriverGUIDs[20];
    static DWORD dwAudioDriverIndex = 0;
    GUID* pTemp  = NULL;

    if( pGUID )
    {
        if( dwAudioDriverIndex >= 20 )
            return TRUE;

        pTemp = &AudioDriverGUIDs[dwAudioDriverIndex++];
        memcpy( pTemp, pGUID, sizeof(GUID) );
    }

    HWND soundDevicesHWND = (HWND)pContext;

    // Add the string to the combo box
    SendMessage( soundDevicesHWND, CB_ADDSTRING,  0, (LPARAM) (LPCTSTR) strDesc );

    // Get the index of the string in the combo box
    INT nIndex = (INT)SendMessage( soundDevicesHWND, CB_FINDSTRING, 0, (LPARAM) (LPCTSTR) strDesc );

    // Set the item data to a pointer to the static guid stored in AudioDriverGUIDs
    SendMessage( soundDevicesHWND, CB_SETITEMDATA, nIndex, (LPARAM) pTemp );

    return TRUE;
}

DLL_API STRUCT_DE_SOUNDDEVICE *DESoundManager::EnumerateDevices()
{
	/** PER ENUMERARE **
	STRUCT_DE_SOUNDDEVICE *devices;
	devices = Sound.EnumerateDevices();
	unsigned int tot = Sound.GetEnumeratedDevices();
	//In devices ci sarà devices[0], devices[1]...device[tot]
	Sound.Init(Engine.GetHWND(), DSSCL_PRIORITY, 1);
	*/

	HRESULT hr=NULL;
    soundDevicesHWND = CreateWindow("COMBOBOX", "", CBS_DROPDOWN, 0,0,CW_USEDEFAULT,CW_USEDEFAULT, NULL, NULL, NULL, NULL); 
 	if( FAILED( hr = DirectSoundEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback, (VOID*)soundDevicesHWND ) ) )
	{
		soundDeviceEnum=false;
		return 0;
	}

	//Devices totali trovati
	soundDevicesCounter = (unsigned int)SendMessage(soundDevicesHWND, CB_GETCOUNT, 0, 0);
	
	char buffer[MAX_PATH];
	for (unsigned int i=0; i<soundDevicesCounter;i++)
	{
		memset(buffer, 0, sizeof(char) * MAX_PATH);
		GUID* pSoundGUID = (GUID*) SendMessage( soundDevicesHWND, CB_GETITEMDATA, i, 0 );
		SendMessage(soundDevicesHWND, CB_GETLBTEXT, i, (LONG) (LPSTR) buffer);
		soundDevices[i].deviceGUID = pSoundGUID;
		strcpy(soundDevices[i].deviceDesc,buffer);
		soundDevices[i].deviceID = i;
	}
	soundDeviceEnum=true;
	DestroyWindow(soundDevicesHWND);

	return soundDevices;
}

DLL_API unsigned int DESoundManager::GetEnumeratedDevices()
{
	return soundDevicesCounter;
}

DLL_API bool DESoundManager::Init( HWND hWnd, DWORD dwCoopLevel, unsigned int deviceID )
{
    HRESULT hr=NULL;
	DSDevice = NULL;
	objectCreated = false;
	listenerCreated = false;

	DELog::LogInfo("<br><b>DirectSound Initialization</b>");
	try
	{
		//E' stata fatta l'enumerazione
		if (soundDeviceEnum)
		{
			//Scrive sul log i devices trovati
			for (unsigned int i=0; i<soundDevicesCounter;i++)
			{
				char bufferL[MAX_PATH];
				memset(bufferL, 0, sizeof(char) * MAX_PATH);
				strcpy(bufferL, soundDevices[i].deviceDesc);
				DELog::LogInfo("<li>DirectSoundEnumerate(%d): %s", soundDevices[i].deviceID, bufferL);		
			}
			//Inizializzzazione in base al guid selezionato da ID
			if (deviceID >=soundDevicesCounter) deviceID=0;
			char buffer[MAX_PATH];
			memset(buffer, 0, sizeof(char) * MAX_PATH);
			strcpy(buffer, soundDevices[deviceID].deviceDesc);
			hr = DirectSoundCreate8( soundDevices[deviceID].deviceGUID, &DSDevice, NULL );
			DELog::LogInfo("<li>DirectSoundCreate8(%s): %s", buffer, DXGetErrorDescription9(hr) );		
		}
		else
		{
			//Inizializzazione standard senze guid(usa quello di default)
			hr = DirectSoundCreate8( NULL, &DSDevice, NULL );
			DELog::LogInfo("<li>DirectSoundCreate8( NULL ): %s", DXGetErrorDescription9(hr));		
		}
		if (hr == DS_OK)
		{
			hr = DSDevice->SetCooperativeLevel( hWnd, dwCoopLevel );
			if (hr == DS_OK)
			{
				DELog::LogInfo("<li>SetCooperativeLevel(...): %s", DXGetErrorDescription9(hr));		
				objectCreated = true;
				
				//Retrive listener for 3D Sound
				listenerCreated = GetListenerInterface(&m_pDSListener);
			}
		}

	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}

	if (objectCreated)	DELog::LogInfo("<li>Created successfully");	
	else				DELog::LogError("<li>NOT Created");

	return objectCreated;
}




//-----------------------------------------------------------------------------
// Name: DESoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       !WARNING! - Setting the primary buffer format and then using this
//                   same dsound object for DirectMusic messes up DirectMusic! 
//                   This method should not be used by most applications.
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
DLL_API bool DESoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate )
{
	if (objectCreated)
	{
		HRESULT hr;
		LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

		if( DSDevice == NULL )
			return CO_E_NOTINITIALIZED;

		// Get the primary buffer 
		DSBUFFERDESC dsbd;
		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes = 0;
		dsbd.lpwfxFormat   = NULL;
	       
		if( FAILED( hr = DSDevice->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )  return false;

		WAVEFORMATEX wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
		wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
		wfx.nChannels       = (WORD) dwPrimaryChannels; 
		wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
		wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
		wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
		wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

		if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) ) return false;

		SafeRelease( pDSBPrimary );

		return true;
	}
	else
	{
		return false;
	}
}


DLL_API bool DESoundManager::GetListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
	if (objectCreated)
	{
		HRESULT             hr;
		DSBUFFERDESC        dsbdesc;
		LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

		if( ppDSListener == NULL )		return false;
		if( DSDevice == NULL )				return false;

		*ppDSListener = NULL;

		// Obtain primary buffer, asking it for 3D control
		ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
		if( FAILED( hr = DSDevice->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
			return false;

		if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, (VOID**)ppDSListener ) ) )
		{
			SafeRelease( pDSBPrimary );
			return false;
		}

		SetListenerImmediateApply(true);

		// Release the primary buffer, since it is not need anymore
		SafeRelease( pDSBPrimary );

		return true;
	}
	else
	{
		return false;
	}
}

DLL_API LPDIRECTSOUND8 DESoundManager::GetDirectSound()
{ 
	return DSDevice; 
}

DLL_API bool DESoundManager::GetCreated()
{
	return objectCreated;
}



DLL_API bool DESoundManager::ListenerCommitDeferredSettings()
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->CommitDeferredSettings();
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}

DLL_API bool DESoundManager::SetListenerImmediateApply(bool immediateApply)
{
	if (listenerCreated)
	{
		if (immediateApply)	listenerApplyMode=DS3D_IMMEDIATE;
		else				listenerApplyMode=DS3D_DEFERRED;
		return true;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerPosition(D3DXVECTOR3 position)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetPosition(position.x, position.y, position.z, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerOrientation(D3DXVECTOR3 frontOrientation, D3DXVECTOR3 topOrientation)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetOrientation(frontOrientation.x, frontOrientation.y, frontOrientation.z, topOrientation.x, topOrientation.y, topOrientation.z, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerVelocity(D3DXVECTOR3 velocity)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetVelocity(velocity.x, velocity.y, velocity.z, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerDistanceFactor(float distanceFactor)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetDistanceFactor(distanceFactor, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerDopplerFactor(float dopplerFactor)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetDopplerFactor(dopplerFactor, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerRolloffFactor(float rolloffFactor)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetRolloffFactor(rolloffFactor, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}
DLL_API bool DESoundManager::SetListenerParameters(LPCDS3DLISTENER listenerParams)
{
	if (listenerCreated)
	{
		HRESULT hr=NULL;
		hr = m_pDSListener->SetAllParameters(listenerParams, listenerApplyMode);
		if (hr==DS_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}



DLL_API bool DESoundManager::GetListenerImmediateApply()
{
	if (listenerApplyMode==DS3D_IMMEDIATE)	return true;
	else									return false;
}
DLL_API D3DXVECTOR3 DESoundManager::GetListenerPosition()
{
	D3DXVECTOR3 position;
	position.x=0.0f; position.y=0.0f; position.z=0.0f; 
	if (listenerCreated)
	{
		m_pDSListener->GetPosition(&position);
	}
	return position;
}
DLL_API D3DXVECTOR3 DESoundManager::GetListenerOrientation(unsigned int mode)
{
	D3DXVECTOR3 frontOrientation;
	D3DXVECTOR3 topOrientation;
	frontOrientation.x=0.0f; frontOrientation.y=0.0f; frontOrientation.z=0.0f; 
	topOrientation.x=0.0f; topOrientation.y=0.0f; topOrientation.z=0.0f; 
	if (listenerCreated)
	{
		m_pDSListener->GetOrientation(&frontOrientation, &topOrientation);
	}
	if (mode==0)	return frontOrientation;
	else			return topOrientation;
}
DLL_API D3DXVECTOR3 DESoundManager::GetListenerVelocity()
{
	D3DXVECTOR3 velocity;
	velocity.x=0.0f; velocity.y=0.0f; velocity.z=0.0f; 
	if (listenerCreated)
	{
		m_pDSListener->GetVelocity(&velocity);
	}
	return velocity;
}
DLL_API float DESoundManager::GetListenerDistanceFactor()
{
	float distanceFactor=0.0f;
	if (listenerCreated)
	{
		m_pDSListener->GetDistanceFactor(&distanceFactor);
	}
	return distanceFactor;
}
DLL_API float DESoundManager::GetListenerDopplerFactor()
{
	float dopplerFactor=0.0f;
	if (listenerCreated)
	{
		m_pDSListener->GetDopplerFactor(&dopplerFactor);
	}
	return dopplerFactor;
}
DLL_API float DESoundManager::GetListenerRolloffFactor()
{
	float rolloffFactor=0.0f;
	if (listenerCreated)
	{
		m_pDSListener->GetRolloffFactor(&rolloffFactor);
	}
	return rolloffFactor;
}
DLL_API LPDS3DLISTENER DESoundManager::GetListenerParameters()
{
	LPDS3DLISTENER listenerParams=NULL;
	if (listenerCreated)
	{
		m_pDSListener->GetAllParameters(listenerParams);
	}
	return listenerParams;
}





























DLL_API DESound::DESound()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DESound::Create(LPTSTR strWaveFileName,  DWORD dwCreationFlags,  GUID guid3DAlgorithm, DWORD dwNumBuffers, LPCSTR logTitle )
{
	HRESULT hr=NULL;
	LPDIRECTSOUNDBUFFER* apDSBuffer = NULL;
	DWORD dwDSBufferSize = NULL;
	DEWaveFile* pWaveFile;
	//strcpy((char *)mainLogTitle, (char *)logTitle);
	//strcpy((char *)mainTag, "");
	//isCreated = false;

	//File corrente
	char currentFile[MAX_PATH];
	memset(currentFile, 0, sizeof(char) * MAX_PATH);
	strcpy(currentFile, strWaveFileName);
	//Directory corrente
	char currentRoot[MAX_PATH];
	memset(currentRoot, 0, sizeof(char) * MAX_PATH);
	strcpy(currentRoot, DEUtility::GetCurrentPath());
	//File corrente nella directory corrente
	char currentFileInRoot[MAX_PATH];
	memset(currentFileInRoot, 0, sizeof(char) * MAX_PATH);
	strcpy(currentFileInRoot, currentRoot);
	strcat(currentFileInRoot, currentFile);

	try
	{
		if (DESoundManager::DSDevice!=NULL)
		{
			if( strWaveFileName != NULL || dwNumBuffers >= 1 )
			{
				//Crea il buffer
				apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
				if( apDSBuffer != NULL )
				{
					//crea il file wave
					pWaveFile = new DEWaveFile();
					if( pWaveFile != NULL )
					{
						//Apre il file
						hr = pWaveFile->Open( currentFileInRoot, NULL, WAVEFILE_READ );
						if (hr==DS_OK)
						{
							//Controllo 3D
							if (dwCreationFlags == DSBCAPS_CTRL3D)
							{
								WAVEFORMATEX* pwfx = pWaveFile->GetFormat();
								if( pwfx == NULL )
								{	
									//Invalid wave file format
									goto LFail;
								}
								if( pwfx->nChannels > 1 )	
								{	
									//Wave file must be mono for 3D control.
									goto LFail;
								}
								if( pwfx->wFormatTag != WAVE_FORMAT_PCM )	
								{	
									//Wave file must be PCM for 3D control.
									goto LFail;
								}
							}

							if( pWaveFile->GetSize() == 0 )
							{
								// Wave is blank, so don't create it.
								goto LFail;
							}

							// Make the DirectSound buffer the same size as the wav file
							dwDSBufferSize = pWaveFile->GetSize();

							// Create the direct sound buffer, and only request the flags needed
							// since each requires some overhead and limits if the buffer can 
							// be hardware accelerated
							DSBUFFERDESC dsbd;
							ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
							dsbd.dwSize          = sizeof(DSBUFFERDESC);
							dsbd.dwFlags         = dwCreationFlags;
							dsbd.dwBufferBytes   = dwDSBufferSize;
							dsbd.guid3DAlgorithm = guid3DAlgorithm;
							dsbd.lpwfxFormat     = pWaveFile->GetFormat();
			
							// DirectSound is only guarenteed to play PCM data.  Other
							// formats may or may not work depending the sound card driver.
							hr = DESoundManager::DSDevice->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );
							if( FAILED(hr) )
							{
								// DSERR_BUFFERTOOSMALL will be returned if the buffer is
								// less than DSBSIZE_FX_MIN and the buffer is created
								// with DSBCAPS_CTRLFX.
								goto LFail;
							}

							// Default to use DuplicateSoundBuffer() when created extra buffers since always 
							// create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
							// DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
							if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
							{
								for(DWORD i=1; i<dwNumBuffers; i++ )
								{
									if( FAILED( hr = DESoundManager::DSDevice->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
									{
										goto LFail;
									}
								}
							}
							else
							{
								for(DWORD i=1; i<dwNumBuffers; i++ )
								{
									hr = DESoundManager::DSDevice->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
									if( FAILED(hr) )
									{
										goto LFail;
									}
								}
							}
			
							//Crea il buffer
							m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
							if( NULL != m_apDSBuffer )
							{
								for( DWORD i=0; i<dwNumBuffers; i++ )
									m_apDSBuffer[i] = apDSBuffer[i];
							
								m_dwDSBufferSize = dwDSBufferSize;
								m_dwNumBuffers   = dwNumBuffers;
								m_pWaveFile      = pWaveFile;
								m_dwCreationFlags = dwCreationFlags;
							    
								FillBufferWithSound( m_apDSBuffer[0], FALSE );

								if (dwCreationFlags == DSBCAPS_CTRL3D)
								{
									Get3DBufferInterface( 0, &g_pDS3DBuffer );
									g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
									g_pDS3DBuffer->GetAllParameters( &g_dsBufferParams);
								}
							}
							//cancella il buffe rtemporaneo
							SafeDeleteArray( apDSBuffer );
							isCreated = true;
						}
					}
				}
			}
		}
		else
		{
			DELog::LogError("<li>%s : DESoundManager::DSDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		
	goto LExit;

	LFail:
	SafeDelete( pWaveFile );
	SafeDeleteArray( apDSBuffer );
	isCreated = false;
	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	LExit:
	return isCreated;

}

DLL_API bool DESound::GetCreated()
{
	return isCreated;
}


//-----------------------------------------------------------------------------
// Name: DESound::~DESound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
DLL_API bool DESound::Release()
{
	try
	{
		for( DWORD i=0; i<m_dwNumBuffers; i++ )
		{
			SafeRelease( m_apDSBuffer[i] ); 
		}
		SafeDeleteArray( m_apDSBuffer ); 
		SafeDelete( m_pWaveFile );
		isCreated = false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);	
		return false;
	}
	return false;
}


DLL_API bool DESound::SetPosition(D3DXVECTOR3 position)
{
	HRESULT hr=NULL;
 	g_dsBufferParams.vPosition = position;
	hr = g_pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
	if (hr==DS_OK)	return true;
	else			return false;
}

DLL_API bool DESound::SetVelocity(D3DXVECTOR3 velocity)
{
	HRESULT hr=NULL;
 	g_dsBufferParams.vVelocity = velocity;
	hr = g_pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
	if (hr==DS_OK)	return true;
	else			return false;
}

DLL_API bool DESound::SetDistances(float minDistance, float maxDistance)
{
	HRESULT hr=NULL;
 	g_dsBufferParams.flMinDistance = minDistance;
 	g_dsBufferParams.flMaxDistance = maxDistance;
	hr = g_pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
	if (hr==DS_OK)	return true;
	else			return false;
}

DLL_API bool DESound::SetCones(DWORD insideConeAngle, DWORD outsideConeAngle, D3DXVECTOR3 coneOrientation, long outsideConeVolume)
{
	HRESULT hr=NULL;
 	g_dsBufferParams.dwInsideConeAngle = insideConeAngle;
 	g_dsBufferParams.dwOutsideConeAngle = outsideConeAngle;
 	g_dsBufferParams.vConeOrientation = coneOrientation;
 	g_dsBufferParams.lConeOutsideVolume = outsideConeVolume;
	hr = g_pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
	if (hr==DS_OK)	return true;
	else			return false;
}

DLL_API bool DESound::SetMode(DWORD mode)
{
	HRESULT hr=NULL;
 	g_dsBufferParams.dwMode = mode;
	hr = g_pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
	if (hr==DS_OK)	return true;
	else			return false;
}

DLL_API bool DESound::SetAllParameters(DS3DBUFFER bufferParams)
{
	HRESULT hr=NULL;
 	g_dsBufferParams = bufferParams;
	hr = g_pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
	if (hr==DS_OK)	return true;
	else			return false;
}

//-----------------------------------------------------------------------------
// Name: DESound::FillBufferWithSound()
// Desc: Fills a DirectSound buffer with a sound file 
//-----------------------------------------------------------------------------
DLL_API bool DESound::FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger )
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    if( pDSB == NULL ) return false;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if( FAILED( hr = RestoreBuffer( pDSB, NULL ) ) ) return false;

    // Lock the buffer down
    if( FAILED( hr = pDSB->Lock( 0, m_dwDSBufferSize, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0L ) ) )
        return false;

    // Reset the wave file to the beginning 
    m_pWaveFile->ResetFile();

    if( FAILED( hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer, dwDSLockedBufferSize, &dwWavDataRead ) ) )           
        return false;

    if( dwWavDataRead == 0 )
    {
        // Wav is blank, so just fill with silence
        FillMemory( (BYTE*) pDSLockedBuffer, dwDSLockedBufferSize,  (BYTE)(m_pWaveFile->GetFormat()->wBitsPerSample == 8 ? 128 : 0 ) );
    }
    else if( dwWavDataRead < dwDSLockedBufferSize )
    {
        // If the wav file was smaller than the DirectSound buffer, 
        // we need to fill the remainder of the buffer with data 
        if( bRepeatWavIfBufferLarger )
        {       
            // Reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full 
                // for very short files
                if( FAILED( hr = m_pWaveFile->ResetFile() ) )return false;

                hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar, dwDSLockedBufferSize - dwReadSoFar, &dwWavDataRead );
                if( FAILED(hr) )return false;

                dwReadSoFar += dwWavDataRead;
            } 
        }
        else
        {
            // Don't repeat the wav file, just fill in silence 
            FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, dwDSLockedBufferSize - dwWavDataRead, (BYTE)(m_pWaveFile->GetFormat()->wBitsPerSample == 8 ? 128 : 0 ) );
        }
    }

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    return true;
}

//-----------------------------------------------------------------------------
// Name: DESound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also NULL if the information is not needed.
//-----------------------------------------------------------------------------
DLL_API HRESULT DESound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
{
    HRESULT hr;

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return DXTRACE_ERR( TEXT("GetStatus"), hr );

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( ( hr = pDSB->Restore() ) == DSERR_BUFFERLOST );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}


//-----------------------------------------------------------------------------
// Name: DESound::GetLenght()
// Desc: Return lenght in milliseconds
//-----------------------------------------------------------------------------
DLL_API DWORD DESound::GetLenght()
{
	return m_pWaveFile->GetLength();
}


//-----------------------------------------------------------------------------
// Name: DESound::GetFreeBuffer()
// Desc: Finding the first buffer that is not playing and return a pointer to 
//       it, or if all are playing return a pointer to a randomly selected buffer.
//-----------------------------------------------------------------------------
DLL_API LPDIRECTSOUNDBUFFER DESound::GetFreeBuffer()
{
    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
                break;
        }
    }

    if( i != m_dwNumBuffers )
        return m_apDSBuffer[ i ];
    else
        return m_apDSBuffer[ rand() % m_dwNumBuffers ];
}

//-----------------------------------------------------------------------------
// Name: DESound::GetBuffer()
// Desc: 
//-----------------------------------------------------------------------------
DLL_API LPDIRECTSOUNDBUFFER DESound::GetBuffer( DWORD dwIndex )
{
    if( m_apDSBuffer == NULL )
        return NULL;
    if( dwIndex >= m_dwNumBuffers )
        return NULL;

    return m_apDSBuffer[dwIndex];
}

//-----------------------------------------------------------------------------
// Name: DESound::Get3DBufferInterface()
// Desc: 
//-----------------------------------------------------------------------------
DLL_API HRESULT DESound::Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;
    if( dwIndex >= m_dwNumBuffers )
        return E_INVALIDARG;

    *ppDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)ppDS3DBuffer );
}

//-----------------------------------------------------------------------------
// Name: DESound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
DLL_API bool DESound::Play( DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan )
{
    HRESULT hr;
    BOOL bRestored;
	if (isCreated)
	{
		LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();
		if( pDSB == NULL ) return false;

		// Restore the buffer if it was lost
		if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
			return false;

		if( bRestored )
		{
			// The buffer was restored, so we need to fill it with new data
			if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
				return false;
		}

		if( m_dwCreationFlags & DSBCAPS_CTRLVOLUME )
		{
			pDSB->SetVolume( lVolume );
		}

		if( (lFrequency != -1) && (m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY) )
		{
			pDSB->SetFrequency( lFrequency );
		}
	    
		if( m_dwCreationFlags & DSBCAPS_CTRLPAN )
		{
			pDSB->SetPan( lPan );
		}
	    
		hr = pDSB->Play( 0, dwPriority, dwFlags );
		if (hr=DS_OK)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name: DESound::Play3D()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
DLL_API bool DESound::Play3D( DWORD dwPriority, DWORD dwFlags, LONG lFrequency )
{	
	HRESULT hr;
    BOOL    bRestored;
    DWORD   dwBaseFrequency;

	if (isCreated)
	{
		LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();
		if( pDSB == NULL )return false;

		// Restore the buffer if it was lost
		if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
			return false;

		if( bRestored )
		{
			// The buffer was restored, so we need to fill it with new data
			if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
				return false;
		}

		if( m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY )
		{
			pDSB->GetFrequency( &dwBaseFrequency );
			pDSB->SetFrequency( dwBaseFrequency + lFrequency );
		}

		// QI for the 3D buffer 
		LPDIRECTSOUND3DBUFFER pDS3DBuffer;
		hr = pDSB->QueryInterface( IID_IDirectSound3DBuffer, (VOID**) &pDS3DBuffer );
		if( SUCCEEDED( hr ) )
		{
			hr = pDS3DBuffer->SetAllParameters( &g_dsBufferParams, DS3D_IMMEDIATE );
			if( SUCCEEDED( hr ) )
			{
				hr = pDSB->Play( 0, dwPriority, dwFlags );
			}
			pDS3DBuffer->Release();
		}
	}
	else
	{
		return false;
	}
    return false;
}

//-----------------------------------------------------------------------------
// Name: DESound::Stop()
// Desc: Stops the sound from playing
//-----------------------------------------------------------------------------
DLL_API bool DESound::Stop()
{
	if (isCreated)
	{
		for( DWORD i=0; i<m_dwNumBuffers; i++ )
		{
			if (FAILED( m_apDSBuffer[i]->Stop()))
				return false;
		}
		return true;
	}
    return false;
}

//-----------------------------------------------------------------------------
// Name: DESound::Reset()
// Desc: Reset all of the sound buffers
//-----------------------------------------------------------------------------
DLL_API bool DESound::Reset()
{
 	if (isCreated)
	{
		for( DWORD i=0; i<m_dwNumBuffers; i++ )
		{
			if (FAILED(m_apDSBuffer[i]->SetCurrentPosition( 0 )))
				return false;
		}
		return true;
	}
    return false;
}

//-----------------------------------------------------------------------------
// Name: DESound::IsSoundPlaying()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
DLL_API bool DESound::IsSoundPlaying()
{
    bool bIsPlaying = false;

    if( m_apDSBuffer == NULL )
        return false; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
        }
    }

    return bIsPlaying;
}


DLL_API bool DESound::Invalidate()
{
	try
	{
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESound::Restore()
{
	try
	{
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}



DLL_API bool DESound::SetTag(LPCSTR newTag)
{
	try
	{
		mainTag = newTag;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API LPCSTR DESound::GetTag()
{
	LPCSTR	nullTag="NoTag";
	try
	{
		if (mainTag!=NULL)	return mainTag;
		else				return nullTag;
	}
	catch(...)
	{
		return nullTag;
	}
	return nullTag;
}





























//-----------------------------------------------------------------------------
// Name: DEWaveFile::~DEWaveFile()
// Desc: Destructs the class
//-----------------------------------------------------------------------------
DLL_API void DEWaveFile::Release()
{
    Close();

    if( !m_bIsReadingFromMemory )
        SafeDeleteArray( m_pwfx );
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::Open()
// Desc: Opens a wave file for reading
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
    HRESULT hr;

    m_pwfx    = NULL;
    m_hmmio   = NULL;
    m_pResourceBuffer = NULL;
    m_dwSize  = 0;
    m_dwFlags = dwFlags;
    m_bIsReadingFromMemory = FALSE;

    if( m_dwFlags == WAVEFILE_READ )
    {
        if( strFileName == NULL )
            return E_INVALIDARG;
        SafeDeleteArray( m_pwfx );

        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

        if( NULL == m_hmmio )
        {
            HRSRC   hResInfo;
            HGLOBAL hResData;
            DWORD   dwSize;
            VOID*   pvRes;

            // Loading it as a file failed, so try it as a resource
            if( NULL == ( hResInfo = FindResource( NULL, strFileName, TEXT("WAVE") ) ) )
            {
                if( NULL == ( hResInfo = FindResource( NULL, strFileName, TEXT("WAV") ) ) )
                    return DXTRACE_ERR( TEXT("FindResource"), E_FAIL );
            }

            if( NULL == ( hResData = LoadResource( NULL, hResInfo ) ) )
                return DXTRACE_ERR( TEXT("LoadResource"), E_FAIL );

            if( 0 == ( dwSize = SizeofResource( NULL, hResInfo ) ) ) 
                return DXTRACE_ERR( TEXT("SizeofResource"), E_FAIL );

            if( NULL == ( pvRes = LockResource( hResData ) ) )
                return DXTRACE_ERR( TEXT("LockResource"), E_FAIL );

            m_pResourceBuffer = new CHAR[ dwSize ];
            memcpy( m_pResourceBuffer, pvRes, dwSize );

            MMIOINFO mmioInfo;
            ZeroMemory( &mmioInfo, sizeof(mmioInfo) );
            mmioInfo.fccIOProc = FOURCC_MEM;
            mmioInfo.cchBuffer = dwSize;
            mmioInfo.pchBuffer = (CHAR*) m_pResourceBuffer;

            m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
        }

        if( FAILED( hr = ReadMMIO() ) )
        {
            // ReadMMIO will fail if its an not a wave file
            mmioClose( m_hmmio, 0 );
            return DXTRACE_ERR( TEXT("ReadMMIO"), hr );
        }

        if( FAILED( hr = ResetFile() ) )
            return DXTRACE_ERR( TEXT("ResetFile"), hr );

        // After the reset, the size of the wav file is m_ck.cksize so store it now
        m_dwSize = m_ck.cksize;
    }
    else
    {
        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF  | 
                                                  MMIO_READWRITE | 
                                                  MMIO_CREATE );
        if( NULL == m_hmmio )
            return DXTRACE_ERR( TEXT("mmioOpen"), E_FAIL );

        if( FAILED( hr = WriteMMIO( pwfx ) ) )
        {
            mmioClose( m_hmmio, 0 );
            return DXTRACE_ERR( TEXT("WriteMMIO"), hr );
        }
                        
        if( FAILED( hr = ResetFile() ) )
            return DXTRACE_ERR( TEXT("ResetFile"), hr );
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::OpenFromMemory()
// Desc: copy data to DEWaveFile member variable from memory
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
    m_pwfx       = pwfx;
    m_ulDataSize = ulDataSize;
    m_pbData     = pbData;
    m_pbDataCur  = m_pbData;
    m_bIsReadingFromMemory = TRUE;
    
    if( dwFlags != WAVEFILE_READ )
        return E_NOTIMPL;       
    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx. 
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::ReadMMIO()
{
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

    // Check to make sure this is a valid wave file
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        return DXTRACE_ERR( TEXT("mmioFOURCC"), E_FAIL ); 

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
       if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
           return DXTRACE_ERR( TEXT("sizeof(PCMWAVEFORMAT)"), E_FAIL );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
            return DXTRACE_ERR( TEXT("m_pwfx"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
            return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
            return DXTRACE_ERR( TEXT("new"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            SafeDelete( m_pwfx );
            return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        SafeDelete( m_pwfx );
        return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::GetSize()
// Desc: Retuns the size of the read access wave file 
//-----------------------------------------------------------------------------
DLL_API DWORD DEWaveFile::GetSize()
{
    return m_dwSize;
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::ResetFile()
// Desc: Resets the internal m_ck pointer so reading starts from the 
//       beginning of the file again 
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::ResetFile()
{
    if( m_bIsReadingFromMemory )
    {
        m_pbDataCur = m_pbData;
    }
    else 
    {
        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( m_dwFlags == WAVEFILE_READ )
        {
            // Seek to the data
            if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
                            SEEK_SET ) )
                return DXTRACE_ERR( TEXT("mmioSeek"), E_FAIL );

            // Search the input file for the 'data' chunk.
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
              return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
        }
        else
        {
            // Create the 'data' chunk that holds the waveform samples.  
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            m_ck.cksize = 0;

            if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) ) 
                return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );

            if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
                return DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
        }
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DEWaveFile::Read()
// Desc: Reads section of data from a wave file into pBuffer and returns 
//       how much read in pdwSizeRead, reading not more than dwSizeToRead.
//       This uses m_ck to determine where to start reading from.  So 
//       subsequent calls will be continue where the last left off unless 
//       Reset() is called.
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
    if( m_bIsReadingFromMemory )
    {
        if( m_pbDataCur == NULL )
            return CO_E_NOTINITIALIZED;
        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( (BYTE*)(m_pbDataCur + dwSizeToRead) > 
            (BYTE*)(m_pbData + m_ulDataSize) )
        {
            dwSizeToRead = m_ulDataSize - (DWORD)(m_pbDataCur - m_pbData);
        }
        
        CopyMemory( pBuffer, m_pbDataCur, dwSizeToRead );
        
        if( pdwSizeRead != NULL )
            *pdwSizeRead = dwSizeToRead;

        return S_OK;
    }
    else 
    {
        MMIOINFO mmioinfoIn; // current status of m_hmmio

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;
        if( pBuffer == NULL || pdwSizeRead == NULL )
            return E_INVALIDARG;

        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
            return DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
                
        UINT cbDataIn = dwSizeToRead;
        if( cbDataIn > m_ck.cksize ) 
            cbDataIn = m_ck.cksize;       

        m_ck.cksize -= cbDataIn;
    
        for( DWORD cT = 0; cT < cbDataIn; cT++ )
        {
            // Copy the bytes from the io to the buffer.
            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
            {
                if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
                    return DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );

                if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                    return DXTRACE_ERR( TEXT("mmioinfoIn.pchNext"), E_FAIL );
            }

            // Actual copy.
            *((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
            mmioinfoIn.pchNext++;
        }

        if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
            return DXTRACE_ERR( TEXT("mmioSetInfo"), E_FAIL );

        if( pdwSizeRead != NULL )
            *pdwSizeRead = cbDataIn;

        return S_OK;
    }
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::Close()
// Desc: Closes the wave file 
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::Close()
{
    if( m_dwFlags == WAVEFILE_READ )
    {
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
        SafeDeleteArray( m_pResourceBuffer );
    }
    else
    {
        m_mmioinfoOut.dwFlags |= MMIO_DIRTY;

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( 0 != mmioSetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
            return DXTRACE_ERR( TEXT("mmioSetInfo"), E_FAIL );
    
        // Ascend the output file out of the 'data' chunk -- this will cause
        // the chunk size of the 'data' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
            return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
        // Do this here instead...
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
        
        mmioSeek( m_hmmio, 0, SEEK_SET );

        if( 0 != (INT)mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) )
            return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
    
        m_ck.ckid = mmioFOURCC('f', 'a', 'c', 't');

        if( 0 == mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) ) 
        {
            DWORD dwSamples = 0;
            mmioWrite( m_hmmio, (HPSTR)&dwSamples, sizeof(DWORD) );
            mmioAscend( m_hmmio, &m_ck, 0 ); 
        }
    
        // Ascend the output file out of the 'RIFF' chunk -- this will cause
        // the chunk size of the 'RIFF' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DEWaveFile::WriteMMIO()
// Desc: Support function for reading from a multimedia I/O stream
//       pwfxDest is the WAVEFORMATEX for this new wave file.  
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_ck.  
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::WriteMMIO( WAVEFORMATEX *pwfxDest )
{
    DWORD    dwFactChunk; // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
    MMCKINFO ckOut1;
    
    dwFactChunk = (DWORD)-1;

    // Create the output file RIFF chunk of form type 'WAVE'.
    m_ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');       
    m_ckRiff.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &m_ckRiff, MMIO_CREATERIFF ) )
        return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
    // We are now descended into the 'RIFF' chunk we just created.
    // Now create the 'fmt ' chunk. Since we know the size of this chunk,
    // specify it in the MMCKINFO structure so MMIO doesn't have to seek
    // back and set the chunk size after ascending from the chunk.
    m_ck.ckid = mmioFOURCC('f', 'm', 't', ' ');
    m_ck.cksize = sizeof(PCMWAVEFORMAT);   

    if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
        return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
    // Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type. 
    if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
    {
        if( mmioWrite( m_hmmio, (HPSTR) pwfxDest, 
                       sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
            return DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    }   
    else 
    {
        // Write the variable length size.
        if( (UINT)mmioWrite( m_hmmio, (HPSTR) pwfxDest, 
                             sizeof(*pwfxDest) + pwfxDest->cbSize ) != 
                             ( sizeof(*pwfxDest) + pwfxDest->cbSize ) )
            return DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    }  
    
    // Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
    if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
        return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
    // Now create the fact chunk, not required for PCM but nice to have.  This is filled
    // in when the close routine is called.
    ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
    ckOut1.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &ckOut1, 0 ) )
        return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
    if( mmioWrite( m_hmmio, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != 
                    sizeof(dwFactChunk) )
         return DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    
    // Now ascend out of the fact chunk...
    if( 0 != mmioAscend( m_hmmio, &ckOut1, 0 ) )
        return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
       
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::Write()
// Desc: Writes data to the open wave file
//-----------------------------------------------------------------------------
DLL_API HRESULT DEWaveFile::Write( UINT nSizeToWrite, BYTE* pbSrcData, UINT* pnSizeWrote )
{
    UINT cT;

    if( m_bIsReadingFromMemory )
        return E_NOTIMPL;
    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pnSizeWrote == NULL || pbSrcData == NULL )
        return E_INVALIDARG;

    *pnSizeWrote = 0;
    
    for( cT = 0; cT < nSizeToWrite; cT++ )
    {       
        if( m_mmioinfoOut.pchNext == m_mmioinfoOut.pchEndWrite )
        {
            m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
            if( 0 != mmioAdvance( m_hmmio, &m_mmioinfoOut, MMIO_WRITE ) )
                return DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );
        }

        *((BYTE*)m_mmioinfoOut.pchNext) = *((BYTE*)pbSrcData+cT);
        (BYTE*)m_mmioinfoOut.pchNext++;

        (*pnSizeWrote)++;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DEWaveFile::GetFormat()
// Desc: Retrive wave format
//-----------------------------------------------------------------------------
DLL_API WAVEFORMATEX* DEWaveFile::GetFormat()
{ 
	return m_pwfx; 
};

//-----------------------------------------------------------------------------
// Name: DEWaveFile::GetLength()
// Desc: Retrive wave lenght, in milliseconds
//-----------------------------------------------------------------------------
DLL_API DWORD DEWaveFile::GetLength()
{
	DWORD dwLen = 0;
	DWORD dwSize;
	WAVEFORMATEX* wfx;

	wfx = GetFormat();
	dwSize = GetSize();
	dwLen = (DWORD) (1000 * dwSize / wfx->nAvgBytesPerSec);
	return dwLen;
}
