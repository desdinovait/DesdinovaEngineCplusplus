#include "DEGeneralIncludes.h"
#include "DESoundBackground.h"

DLL_API DESoundBackground::DESoundBackground()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DESoundBackground::Create(WCHAR *pwsFileName, LPCSTR logTitle)
{
	HRESULT hr;
	mainLogTitle = logTitle;
	isCreated = false;

	try
	{
		hr = CoCreateInstance(CLSID_FilterGraph, NULL,CLSCTX_INPROC, IID_IGraphBuilder, (void**)&m_pGraph);
		if (hr==S_OK)
		{
			hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMediaControl);
			if (hr == S_OK)
			{	
				hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&m_pMediaPosition);
				if (hr == S_OK)
				{
					hr = m_pGraph->QueryInterface(IID_IBasicAudio, (void **)&m_pBasicAudio);
					if (hr == S_OK)
					{
						hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEventEx);
						if (hr == S_OK)
						{
							hr = m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&m_pMediaSeeking);
							if (hr == S_OK)
							{
								hr = m_pGraph->RenderFile(pwsFileName, NULL);
								if (hr == S_OK)
								{
									isCreated=true;
								}
							}
							else
							{
								DELog::LogError("<li>%s : m_pGraph->QueryInterface(...IID_IMediaSeeking...): %s", logTitle, DXGetErrorDescription9(hr));		
							}
						}
						else
						{
							DELog::LogError("<li>%s : m_pGraph->QueryInterface(...IID_IMediaEventEx...): %s", logTitle, DXGetErrorDescription9(hr));		
						}
					}
					else
					{
						DELog::LogError("<li>%s : m_pGraph->QueryInterface(...IID_IBasicAudio...): %s", logTitle, DXGetErrorDescription9(hr));		
					}
				}
				else
				{
					DELog::LogError("<li>%s : m_pGraph->QueryInterface(...IID_IMediaPosition...): %s", logTitle, DXGetErrorDescription9(hr));		
				}
			}
			else
			{
				DELog::LogError("<li>%s : m_pGraph->QueryInterface(...IID_IMediaControl...): %s", logTitle, DXGetErrorDescription9(hr));		
			}
		}
		else
		{
			DELog::LogError("<li>%s : CoCreateInstance(...CLSID_FilterGraph...): %s", logTitle, DXGetErrorDescription9(hr));		
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}

DLL_API void DESoundBackground::Play()
{
	m_pMediaPosition->put_CurrentPosition(0);
	m_pMediaControl->Run();
}

DLL_API void DESoundBackground::Stop()
{
	m_pMediaControl->Stop();
	m_pMediaPosition->put_CurrentPosition(0);
}

DLL_API void DESoundBackground::Pause(bool pause)
{
	if (pause)
	{
		m_pMediaControl->Pause();
		inPause=true;
	}
	else
	{
		m_pMediaControl->Run();
		inPause=false;
	}
}

DLL_API void DESoundBackground::TogglePause()
{
	if (inPause)
	{
		m_pMediaControl->Run();
		inPause=false;
	}
	else
	{
		m_pMediaControl->Pause();
		inPause=true;
	}
}

DLL_API void DESoundBackground::SetVolume(long newVolume)
{
	//Specifies the volume, as a number from –10,000 to 0, inclusive.  
	//Full volume is 0, and –10,000 is silence. 
	//Multiply the desired decibel level by 100. For example, –10,000 = –100 dB
	if (newVolume >= 0) newVolume=0;
	if (newVolume <= -10000) newVolume=-10000;
	m_pBasicAudio->put_Volume(newVolume);
}

DLL_API void DESoundBackground::SetVolumeLevel(unsigned int newVolume)
{
	if (newVolume<=0)	newVolume=0;
	if (newVolume>=10)	newVolume=10;
	long volume = (10-newVolume) * 1000;
	m_pBasicAudio->put_Volume(-volume);
}

DLL_API bool DESoundBackground::IsPlaying()
{
	REFTIME refPosition;
	REFTIME refDuration;
	m_pMediaPosition->get_CurrentPosition(&refPosition);
	m_pMediaPosition->get_Duration(&refDuration);
	
	if(refPosition < refDuration)	return true;
	else							return false;

	return false;
}


DLL_API long DESoundBackground::Update(bool loop)
{
    HRESULT hr=S_OK;
    long evCodeSound=0;
	long evParam1=0;
	long evParam2=0;
	
	if (isCreated)
	{
		if (!m_pMediaEventEx)return 1;

		//Processa gli eventi
		while(SUCCEEDED(m_pMediaEventEx->GetEvent(&evCodeSound, (LONG_PTR *) &evParam1,(LONG_PTR *) &evParam2, 0)))
		{
			//Libera la memoria del callback fino a che non lo usiamo ancora
			hr = m_pMediaEventEx->FreeEventParams(evCodeSound, evParam1, evParam2);

			//Siamo arrivati alla fine del clip
			if(EC_COMPLETE == evCodeSound)
			{
				//Si vuole eseguire il loop
				if (loop)
				{
					LONGLONG pos=0;
					//Setta la posizione del clip all'inizio = 0
					hr = m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
					if (FAILED(hr))
					{
						if (FAILED(hr = m_pMediaControl->Run()))
						{
							break;
						}
					}
				}
				else
				{
					//Fermiamo il clip una volta arrivato alla fine
					m_pMediaControl->Stop();
				}
			}
		}
	}

	return evCodeSound;
}

DLL_API bool DESoundBackground::GetCreated()
{
	return isCreated;
}

DLL_API bool DESoundBackground::Release()
{	
	try
	{
		m_pMediaControl->Stop();
		m_pMediaPosition->put_CurrentPosition(0);

		SafeRelease(m_pMediaControl);
		SafeRelease(m_pMediaPosition);
		SafeRelease(m_pBasicAudio);
		SafeRelease(m_pGraph);
		SafeRelease(m_pMediaEventEx);
		SafeRelease(m_pMediaSeeking);
		isCreated=false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

DLL_API bool DESoundBackground::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DESoundBackground::GetTag()
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