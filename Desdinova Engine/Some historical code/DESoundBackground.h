#ifndef H_SOUNDBACKGROUND_H
#define H_SOUNDBACKGROUND_H

/*
[CLASS] 
[
    [NOME] 
    DESoundBackground
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione degli oggetti suono di sottofondo (.MP3)
]
*/
class DESoundBackground
{
	private:
		//** Variabili **
		IGraphBuilder	*m_pGraph;
		IMediaControl	*m_pMediaControl;
		IMediaPosition	*m_pMediaPosition;
		IBasicAudio		*m_pBasicAudio;
		IMediaEventEx	*m_pMediaEventEx;
		IMediaSeeking	*m_pMediaSeeking;
		bool			inPause;
		bool			isCreated;
		LPCSTR			mainLogTitle;
		LPCSTR			mainTag;

	public:
		//** Funzioni **
		DLL_API			DESoundBackground();
		DLL_API bool	Create(WCHAR *pwsFileName, LPCSTR logTitle);
		DLL_API bool	Release();
		DLL_API long	Update(bool loop=true);
		
		DLL_API void	Play();
		DLL_API void	Stop();
		DLL_API void	Pause(bool pause);
		DLL_API void	TogglePause();
		DLL_API void	SetVolume(long newVolume);
		DLL_API void	SetVolumeLevel(unsigned int newVolume);
		DLL_API bool	IsPlaying();
		DLL_API bool	GetCreated();
		//Tag
		DLL_API bool	SetTag(LPCSTR newTag);
		DLL_API LPCSTR	GetTag();

};

#endif
