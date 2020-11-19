#include "DEGeneralIncludes.h"
#include "DEVideo.h"

DLL_API BOOL DEVideo::IsWindowsMediaFile(WCHAR *lpszFile)
{
    USES_CONVERSION;
    TCHAR szFilename[MAX_PATH];

    // Copy the file name to a local string and convert to lowercase
    _tcsncpy(szFilename, W2T(lpszFile), NUMELMS(szFilename));
    szFilename[MAX_PATH-1] = 0;
    _tcslwr(szFilename);

    if (_tcsstr(szFilename, TEXT(".asf")) ||
        _tcsstr(szFilename, TEXT(".wma")) ||
        _tcsstr(szFilename, TEXT(".wmv")))
        return TRUE;
    else
        return FALSE;
}

DLL_API HRESULT DEVideo::GetUnconnectedPin( IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;

    if (!ppPin)
        return E_POINTER;
    *ppPin = 0;

    // Get a pin enumerator
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
        return hr;

    // Look for the first unconnected pin
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;

        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;

            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }

    // Release the enumerator
    pEnum->Release();

    // Did not find a matching pin
    return E_FAIL;
}

DLL_API HRESULT DEVideo::InitializeWindowlessVMR(IBaseFilter **ppVmr)
{
    IBaseFilter* pVmr = NULL;

    if (!ppVmr)
        return E_POINTER;
    *ppVmr = NULL;

    // Create the VMR and add it to the filter graph.
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);
    if (SUCCEEDED(hr)) 
    {
        hr = pGB->AddFilter(pVmr, L"Video Mixing Renderer");
        if (SUCCEEDED(hr)) 
        {
            // Set the rendering mode and number of streams
            CComPtr <IVMRFilterConfig> pConfig;
            pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
            if( SUCCEEDED(hr))
            {
                pConfig->SetRenderingMode(VMRMode_Windowless);
            }

            hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
            if( SUCCEEDED(hr)) 
            {
                pWC->SetVideoClippingWindow(ghAppParent);
                pWC->SetBorderColor(RGB(0,0,0));
            }
        }

        // Don't release the pVmr interface because we are copying it into
        // the caller's ppVmr9 pointer
        *ppVmr = pVmr;
    }

    return hr;
}


DLL_API HRESULT DEVideo::RenderFileToVMR9(IGraphBuilder *pGB, WCHAR *wFileName, IBaseFilter *pRenderer, BOOL bRenderAudio)
{
    HRESULT hr=S_OK;
    CComPtr <IPin> pOutputPin;
    CComPtr <IBaseFilter> pSource;
    CComPtr <IBaseFilter> pAudioRenderer;
    CComPtr <IFilterGraph2> pFG;

    // Add a file source filter for this media file
    if (!IsWindowsMediaFile(wFileName))
    {
        // Add the source filter to the graph
        if (FAILED(hr = pGB->AddSourceFilter(wFileName, L"SOURCE", &pSource)))
        {
            /*USES_CONVERSION;
            TCHAR szMsg[MAX_PATH + 128];

            wsprintf(szMsg, TEXT("Failed to add the source filter to the graph!  hr=0x%x\r\n\r\n")
                     TEXT("Filename: %s\0"), hr, W2T(wFileName));
            MessageBox(NULL, szMsg, TEXT("Failed to render file to VMR9"), MB_OK | MB_ICONERROR);
			*/
            return hr;
        }

        // Get the interface for the first unconnected output pin
        GetUnconnectedPin(pSource, PINDIR_OUTPUT, &pOutputPin);
    }
    else
    {
        /*MessageBox(NULL, TEXT("Windows Media files (ASF,WMA,WMV) are not supported by this application.\r\n\r\n")
                   TEXT("For a full example of Windows Media support using the\r\n")
                   TEXT("DirectShow WM ASF Reader filter and implementing a key provider\r\n")
                   TEXT("for Windows Media content, refer to the following SDK samples:\r\n\r\n")
                   TEXT("\t- ASFCopy\t- AudioBox\r\n\t- Jukebox  \t- PlayWndASF\r\n\r\n")
                   TEXT("Each of the listed samples provides the necessary extra code\r\n")
                   TEXT("and links with the required Windows Media libraries.\0"),
                   TEXT("Windows Media files are not supported"), MB_OK);*/
        return E_FAIL;
    }

    // Render audio if requested (defaults to TRUE)
    if (bRenderAudio)
    {
        // Because we will be rendering with the RENDERTOEXISTINGRENDERERS flag,
        // we need to create an audio renderer and add it to the graph.  
        // Create an instance of the DirectSound renderer (for each media file).
        //
        // Note that if the system has no sound card (or if the card is disabled),
        // then creating the DirectShow renderer will fail.  In that case,
        // handle the failure quietly.
        if (SUCCEEDED(CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pAudioRenderer)))
        {
            // The audio renderer was successfully created, so add it to the graph
            pGB->AddFilter(pAudioRenderer, L"Audio Renderer");
        }
    }

    // Get an IFilterGraph2 interface to assist in building the
    // multifile graph with the non-default VMR9 renderer
    pGB->QueryInterface(IID_IFilterGraph2, (void **)&pFG);

    // Render the output pin, using the VMR9 as the specified renderer.  This is 
    // necessary in case the GraphBuilder needs to insert a Color Space convertor,
    // or if multiple filters insist on using multiple allocators.
    // The audio renderer will also be used, if the media file contains audio.
    pFG->RenderEx(pOutputPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);

    // If this media file does not contain an audio stream, then the 
    // audio renderer that we created will be unconnected.  If left in the 
    // graph, it could interfere with rate changes and timing.
    // Therefore, if the audio renderer is unconnected, remove it from the graph.
    if (pAudioRenderer != NULL)
    {
        IPin *pUnconnectedPin=0;

        // Is the audio renderer's input pin connected?
        HRESULT hrPin = GetUnconnectedPin(pAudioRenderer, PINDIR_INPUT, &pUnconnectedPin);

        // If there is an unconnected pin, then remove the unused filter
        if (SUCCEEDED(hrPin) && (pUnconnectedPin != NULL))
        {
            // Release the returned IPin interface
            pUnconnectedPin->Release();

            // Remove the audio renderer from the graph
            hrPin = pGB->RemoveFilter(pAudioRenderer);
        }
    }

    return hr;
}

DLL_API bool DEVideo::Create(HWND parentHWND, char *szFile, LPCSTR logTitle)
{
    USES_CONVERSION;
    WCHAR wFile[MAX_PATH];
    HRESULT hr=NULL;
	
	//Inizializzazione varibili
	mainLogTitle = logTitle;
	isCreated = false;
	ghAppParent = parentHWND;
	ghInst = 0;
	g_bAudioOnly = FALSE, 
	g_lVolume = 0; //0 significa volume massimo
	g_dwGraphRegister = 0;
	g_psCurrent = Video_Initialize;
	g_PlaybackRate = 1.0;
	hDrain = 0;
	//Oggetti COM
	pGB = NULL;
	pMC = NULL;
	pME = NULL;
	pWC = NULL;
	pBA = NULL;
	pMS = NULL;
	pMP = NULL;
	pFS = NULL;

	try
	{
		// Convert filename to wide character string
		lstrcpy(g_szFileName, szFile);
		wcscpy(wFile, T2W(szFile));

		// Get the interface for DirectShow's GraphBuilder
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGB);

		if(SUCCEEDED(hr))
		{
			CComPtr <IBaseFilter> pVmr;

			// Create the Video Mixing Renderer and add it to the graph
	        InitializeWindowlessVMR(&pVmr);

			// Render the file programmatically to use the VMR9 as renderer.
			// We pass a pointer to the VMR9 so that it will be used as the 
			// video renderer.  Pass TRUE to create an audio renderer also.
			hr = RenderFileToVMR9(pGB, wFile, pVmr, TRUE);
			if(SUCCEEDED(hr))
			{
				// QueryInterface for DirectShow interfaces
				pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
				pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME);
				pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS);
				pGB->QueryInterface(IID_IMediaPosition, (void **)&pMP);

				// Query for audio interfaces, which may not be relevant for video-only files
				pGB->QueryInterface(IID_IBasicAudio, (void **)&pBA);

				//Definisce se è un file di solo video o meno
				if (!pWC)
				{
					g_bAudioOnly = TRUE;
				}
				else
				{
					// Clear the global flag
					g_bAudioOnly = FALSE;
					
					LONG lWidth=0, lHeight=0;
					hr = pWC->GetNativeVideoSize(&lWidth, &lHeight, 0, 0);
					if (hr == E_NOINTERFACE)
					{
						DELog::LogWarning("<li>%s : pWC->get_Visible(...) - %s", logTitle, DXGetErrorDescription9(hr));
						g_bAudioOnly = TRUE;
					}

					// If this is an audio-only clip, width and height will be 0.
					if ((lWidth == 0) && (lHeight == 0)) g_bAudioOnly = TRUE;//*/

				}
				
				//C'è il video
				if (!g_bAudioOnly)
				{
					// Some video renderers support stepping media frame by frame with the
					// IVideoFrameStep interface.  See the interface documentation for more
					// details on frame stepping.
					IVideoFrameStep *pFSTest = NULL;
					hr = pGB->QueryInterface(__uuidof(IVideoFrameStep), (PVOID *)&pFSTest);
					if (!FAILED(hr))
					{
						//Controlla che si possa utilizaare lo step
						hr = pFSTest->CanStep(0L, NULL);
						if (hr == S_OK)
						{
							pFS = pFSTest;
						}
						else
						{
							DELog::LogWarning("<li>%s : pGB->QueryInterface(...) - %s", logTitle, DXGetErrorDescription9(hr));
							pFSTest->Release();
							pFS = NULL;
						}
					}
					else
					{
						pFSTest = NULL;
						pFS = NULL;
					}
					
					//Posizione video
       				RECT client;
					GetClientRect(ghAppParent, &client);
					pWC->SetVideoPosition(NULL, &client);
				}
				else
				{
					//No video
					isCreated = true;
				}

				//Alla fine della creazione è come se s fosse in STOP
				g_psCurrent = Video_Stopped;
				isCreated = true;

				#ifdef REGISTER_FILTERGRAPH
				hr = AddGraphToRot(pGB, &g_dwGraphRegister);
				if (FAILED(hr))
				{
					//Failed to register filter graph with ROT
					DELog::LogWarning("<li>%s : AddGraphToRot(...) - %s", logTitle, DXGetErrorDescription9(hr));
					g_dwGraphRegister = 0;
				}
				#endif
			}
			else
			{
				DELog::LogError("<li>%s : pGB->RenderFile(%s) - %s", logTitle, g_szFileName, DXGetErrorDescription9(hr));
				isCreated = false;
			}
		}
		else
		{
			Release();
			isCreated = false;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error",logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created",logTitle);		

	return isCreated;
}


DLL_API bool DEVideo::GetVideoSize(LONG *lWidth, LONG *lHeight)
{
	if ((isCreated)&&(!g_bAudioOnly)&&(pWC))
	{
        HRESULT hr = pWC->GetNativeVideoSize(lWidth, lHeight, 0, 0);
        if (hr == E_NOINTERFACE)
        {
            // If this video is encoded with an unsupported codec,
            return false;
        }
        // If this is an audio-only clip, width and height will be 0.
        if ((lWidth == 0) && (lHeight == 0)) return false;

		return true;
	}
	return false;
}

DLL_API bool DEVideo::SetVideoWindow(RECT rect)
{
    HRESULT hr;
	if ((isCreated)&&(!g_bAudioOnly)&&(pWC))
	{
		// Track the movement of the container window and resize as needed
		hr = pWC->SetVideoPosition(NULL, &rect);
		if (!FAILED(hr))
		{	
			return true;
		}
	}
	return false;
}


DLL_API bool DEVideo::Play()
{
	if (isCreated)
	{
		HRESULT hr = pMC->Run();
		if (FAILED(hr))
		{
			Release();
			return false;
		}
		else
		{
 			g_psCurrent=Video_Running;
			return true;
		}
	}
	return false;
}


DLL_API bool DEVideo::Pause()
{
	if (isCreated)
	{
		if (!pMC) return false;

		// Toggle play/pause behavior
		if((g_psCurrent == Video_Paused) || (g_psCurrent == Video_Stopped))
		{
			if (SUCCEEDED(pMC->Run())) g_psCurrent = Video_Running;
		}
		else
		{
			if (SUCCEEDED(pMC->Pause())) g_psCurrent = Video_Paused;
		}
		return true;
	}
	return false;
}


DLL_API bool DEVideo::Stop()
{
	if (isCreated)
	{
		HRESULT hr;
		if ((!pMC) || (!pMS)) return false;
		// Stop and reset postion to beginning
		if((g_psCurrent == Video_Paused) || (g_psCurrent == Video_Running))
		{
			LONGLONG pos = 0;
			hr = pMC->Stop();
			g_psCurrent = Video_Stopped;

			// Seek to the beginning
			hr = pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning , NULL, AM_SEEKING_NoPositioning);

			// Display the first frame to indicate the reset condition
			hr = pMC->Pause();
		}
		return true;
	}
	return false;
}


DLL_API void DEVideo::Release()
{
    HRESULT hr;

    // Stop media playback
    if(pMC) hr = pMC->Stop();

    // Disable event callbacks
    if (pME) hr = pME->SetNotifyWindow((OAHWND)NULL, 0, 0);

	#ifdef REGISTER_FILTERGRAPH
    if (g_dwGraphRegister)
    {
        RemoveGraphFromRot(g_dwGraphRegister);
        g_dwGraphRegister = 0;
    }
	#endif

    //Rilascio interfaccie
    SafeRelease(pME);
    SafeRelease(pMS);
    SafeRelease(pMP);
    SafeRelease(pMC);
    SafeRelease(pBA);
    SafeRelease(pWC);
    SafeRelease(pFS);
    SafeRelease(pGB);
    SafeRelease(pWC);
	
    // Clear file name
    g_szFileName[0] = L'\0';

    // Reset the player window
    RECT rect;
    GetClientRect(ghAppParent, &rect);
    InvalidateRect(ghAppParent, &rect, TRUE);

	// Clear global flags
    g_psCurrent = Video_Stopped;
    g_bAudioOnly = TRUE;
	isCreated=false;

}

DLL_API HRESULT DEVideo::ToggleMute(void)
{
	HRESULT hr=S_OK;
	if (isCreated)
	{
		if ((!pGB) || (!pBA))
			return S_OK;

		//Volume corrente
		hr = pBA->get_Volume(&g_lVolume);
		if (hr == E_NOTIMPL)
		{
			//Video senza suono
			return S_OK;
		}
		else if (FAILED(hr))
		{
			//Errore lettura volume
			return hr;
		}

		//0=volume massimo; -10000L=volume al minimo
		if (g_lVolume == 0)
			g_lVolume = -10000L;
		else
			g_lVolume = 0;

		//Nuovo volume
		pBA->put_Volume(g_lVolume);
	}
    return hr;
}


DLL_API HRESULT DEVideo::ModifyRate(double dRateAdjust)
{
	HRESULT hr=S_OK;

	if (isCreated) 
	{
		double dRate;
		// If the IMediaPosition interface exists, use it to set rate
		if ((pMP) && (dRateAdjust != 0))
		{
			if ((hr = pMP->get_Rate(&dRate)) == S_OK)
			{
				// Add current rate to adjustment value
				double dNewRate = dRate + dRateAdjust;
				hr = pMP->put_Rate(dNewRate);

				// Save global rate
				if (SUCCEEDED(hr))
				{
					g_PlaybackRate = dNewRate;
				}
			}
		}
	}
    return hr;
}


DLL_API HRESULT DEVideo::SetRate(double dRate)
{
    HRESULT hr=S_OK;

	if (isCreated)
	{
		// If the IMediaPosition interface exists, use it to set rate
		if (pMP)
		{
			hr = pMP->put_Rate(dRate);

			// Save global rate
			if (SUCCEEDED(hr))
			{
				g_PlaybackRate = dRate;
			}
		}
	}
    return hr;
}


DLL_API bool DEVideo::Update(bool loop, bool autoRelease)
{
	try
	{
		if (isCreated)
		{
			LONG evCode, evParam1, evParam2;
			HRESULT hr=S_OK;

			// Make sure that we don't access the media event interface
			// after it has already been released.
			if (!pME)return false;

			//Update per il ridimensionamento della schermata
			if ((!g_bAudioOnly)&&(pWC))
			{
				RECT client;
				RECT video;
				GetClientRect(ghAppParent, &client);
				hr = pWC->GetVideoPosition(NULL, &video);
				if (SUCCEEDED(hr))
				{
					//Setta la nuova dimensione solo se è diversa da quella attuale
					if ((video.bottom!=client.bottom)||(video.left!=client.left)||(video.right!=client.right)||(video.top!=client.top))
					{
						hr = pWC->SetVideoPosition(NULL, &client);
					}
				}
			}

			// Process all queued events
			try
			{
				while(SUCCEEDED(pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,(LONG_PTR *) &evParam2, 0)))
				{
					// Free memory associated with callback, since we're not using it
					hr = pME->FreeEventParams(evCode, evParam1, evParam2);

					// If this is the end of the clip, reset to beginning
					if(EC_COMPLETE == evCode)
					{
						if (loop)
						{
							LONGLONG pos=0;
							// Reset to first frame of movie
							hr = pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
							if (FAILED(hr))
							{
								// Some custom filters (like the Windows CE MIDI filter)
								// may not implement seeking interfaces (IMediaSeeking)
								// to allow seeking to the start.  In that case, just stop
								// and restart for the same effect.  This should not be
								// necessary in most cases.
								if (FAILED(hr = pMC->Stop()))	break;
								if (FAILED(hr = pMC->Run()))	break;
							}
						}
						else
						{
							this->Stop();
							if (autoRelease)Release();
							return false;
						}
					}

					if (EC_USERABORT == evCode)
					{
						this->Stop();
						if (autoRelease)Release();
						return false;
					}

					Sleep(100);
					return true;
				}
				return true;
			}
			catch(...)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
    return false;
}

DLL_API void DEVideo::OnDisplayChange(HWND hwnd) 
{
    if (pWC) pWC->DisplayModeChanged();
}

DLL_API void DEVideo::OnPaint(HWND hwnd) 
{
    HRESULT hr;
    PAINTSTRUCT ps; 
    HDC         hdc; 
    RECT        rcClient; 

    GetClientRect(hwnd, &rcClient); 
    hdc = BeginPaint(hwnd, &ps); 

    if(pWC && !g_bAudioOnly) 
    { 
        // When using VMR Windowless mode, you must explicitly tell the
        // renderer when to repaint the video in response to WM_PAINT
        // messages.  This is most important when the video is stopped
        // or paused, since the VMR won't be automatically updating the
        // window as the video plays.
        hr = pWC->RepaintVideo(hwnd, hdc);  
    } 
    else  // No video image. Just paint the whole client area. 
    { 
        FillRect(hdc, &rcClient, (HBRUSH)(COLOR_BTNFACE + 1)); 
    } 

    EndPaint(hwnd, &ps); 
} 


DLL_API void DEVideo::GetScreenshotFileName(string& FileName)
{
    // retrieve the filename of the EXE file
	string ModuleFileName;
    ModuleFileName.reserve(MAX_PATH);
    GetModuleFileName( NULL, const_cast<char*>(ModuleFileName.data()), MAX_PATH);
    // extract the path info from the filename
    FileName = ModuleFileName.substr(0, ModuleFileName.find_last_of(":\\"));
    // append the sub-folder path
	// If, for example, you want to save the screenshots to the sub-folder
	// "shots", set this to "shots\\".
    FileName.append("ScreenShots\\");
	//Create directory if not exist
	CreateDirectory(FileName.c_str(), NULL);

    // search for first unused filename
    char Buf[MAX_PATH];
    WIN32_FIND_DATA ffd;
    HANDLE h;
    for (int i = 0; i < 1000; i++)
    {
        // prepare search mask for FindFirstFile
        wsprintf(Buf, (FileName + "ScreenShot%03i.bmp").c_str(), i);
        // check whether this file already exists
        h = FindFirstFile(Buf, &ffd);
        // if the file exists, close the search handle and continue
        if (h != INVALID_HANDLE_VALUE)
        {   
			FindClose(h); 
		}
        // if the file does not exist, exit from the loop
        else
        {   
			break; 
		}
    }

    // set FileName to the first unused filename
    FileName = Buf;
}

DLL_API bool DEVideo::CaptureImage(LPCTSTR lpszFileName)
{
	HRESULT hr=NULL;
	string FileName;
    BYTE* lpCurrImage = NULL;

    if(pWC && !g_bAudioOnly)
    {
		DELog::LogInfo("<br><b>Save Screenshot from Video</b>");
		// if a filename is given, use it
		if (lpszFileName)
		{
			FileName = lpszFileName; 
		}
		// if no filename is given, have a filename generated
		else
		{   
			GetScreenshotFileName(FileName);
		}
		DELog::LogInfo("<li>Filename: %s", FileName.c_str());
		
		char screenshotTime[12];
		memset(screenshotTime, 0, 12);
		_strtime(screenshotTime);
		DELog::LogInfo("<li>Time : %s",screenshotTime);

        // Read the current video frame into a byte buffer.  The information
        // will be returned in a packed Windows DIB and will be allocated
        // by the VMR.
        if(SUCCEEDED(hr = pWC->GetCurrentImage(&lpCurrImage)))
        {
            BITMAPFILEHEADER    hdr;
            DWORD               dwSize, dwWritten;
            LPBITMAPINFOHEADER  pdib = (LPBITMAPINFOHEADER) lpCurrImage;

            // Create a new file to store the bitmap data
            HANDLE hFile = CreateFile(FileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
            if (hFile == INVALID_HANDLE_VALUE)	return FALSE;

            // Initialize the bitmap header
            dwSize = DibSize(pdib);
            hdr.bfType          = BFT_BITMAP;
            hdr.bfSize          = dwSize + sizeof(BITMAPFILEHEADER);
            hdr.bfReserved1     = 0;
            hdr.bfReserved2     = 0;
            hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) + pdib->biSize + DibPaletteSize(pdib);

            // Write the bitmap header and bitmap bits to the file
            WriteFile(hFile, (LPCVOID) &hdr, sizeof(BITMAPFILEHEADER), &dwWritten, 0);
            WriteFile(hFile, (LPCVOID) pdib, dwSize, &dwWritten, 0);

            // Close the file
            CloseHandle(hFile);

            // The app must free the image data returned from GetCurrentImage()
            CoTaskMemFree(lpCurrImage);

			DELog::LogInfo("<li>pWC->GetCurrentImage(...) - %s", DXGetErrorDescription9(hr));
            return true;
        }
        else
        {
            return false;
        }
    }
	DELog::LogError("<li>pWC->GetCurrentImage(...) - %s", DXGetErrorDescription9(hr));
	
    return false;
}

DLL_API bool DEVideo::StepToNextFrame()
{
	if (isCreated)
	{
		HRESULT hr=NULL;
		// If the Frame Stepping interface exists, use it to step one frame
		if (pFS)
		{
			// The graph must be paused for frame stepping to work
			if (g_psCurrent != State_Paused) Pause();
			hr = pFS->Step(1, NULL);
			if (!FAILED(hr))	return true;
		}
	}
    return false;
}


DLL_API bool DEVideo::StepToFrame(int frameNumber)
{
	if (isCreated)
	{
		HRESULT hr=NULL;
		if (pFS)
		{
			// The renderer may not support frame stepping for more than one
			// frame at a time, so check for support.  S_OK indicates that the
			// renderer can step nFramesToStep successfully.
			if ((hr = pFS->CanStep(frameNumber, NULL)) == S_OK)
			{
				if (g_psCurrent != State_Paused) Pause();
				hr = pFS->Step(frameNumber, NULL);
				if (!FAILED(hr))	return true;
			}
		}
	}
    return false;
}


DLL_API inline bool DEVideo::GetCreated()
{
	return isCreated;
}

DLL_API bool DEVideo::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEVideo::GetTag()
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






#ifdef REGISTER_FILTERGRAPH

DLL_API HRESULT DEVideo::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }

    WCHAR wsz[128];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
              GetCurrentProcessId());

    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
        pMoniker->Release();
    }

    pROT->Release();
    return hr;
}

DLL_API void DEVideo::RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}


#endif


