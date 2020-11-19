#ifndef H_VIDEO_H
#define H_VIDEO_H

//Macros
#define REGISTER_FILTERGRAPH		//To enable registration, define REGISTER_FILTERGRAPH.
#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)
#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))
#define BFT_BITMAP 0x4d42   /* 'BM' */


class DEVideo
{
	public:
		//** Strutture e altre definizioni **
		enum PLAYSTATE {Video_Stopped, Video_Paused, Video_Running, Video_Initialize};

		//** Variabili **
		HWND				ghAppParent;
		HINSTANCE			ghInst;
		DLGPROC				ghProc;
		TCHAR				g_szFileName[MAX_PATH];
		BOOL				g_bAudioOnly;
		LONG				g_lVolume;
		DWORD				g_dwGraphRegister;
		PLAYSTATE			g_psCurrent;
		double				g_PlaybackRate;
		HWND				hDrain;
		bool				isCreated;
		LPCSTR				mainLogTitle;
		LPCSTR				mainTag;

		// DirectShow interfaces
		IGraphBuilder		*pGB;
		IMediaControl		*pMC;
		IMediaEventEx		*pME;
		IBasicAudio			*pBA;
		IMediaSeeking		*pMS;
		IMediaPosition		*pMP;
		IVideoFrameStep		*pFS;
		IVMRWindowlessControl *pWC;

		//** Funzioni **
		//Graphs
		DLL_API HRESULT		AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
		DLL_API void		RemoveGraphFromRot(DWORD pdwRegister);
		DLL_API HRESULT		InitializeWindowlessVMR(IBaseFilter **ppVmr);
		DLL_API HRESULT		RenderFileToVMR9(IGraphBuilder *pGB, WCHAR *wFileName, IBaseFilter *pRenderer, BOOL bRenderAudio=TRUE);
		DLL_API BOOL		IsWindowsMediaFile(WCHAR *lpszFile);
		DLL_API HRESULT		GetUnconnectedPin( IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);

	public:
		//** Funzioni **
		//Uso
		DLL_API bool		Create(HWND parentHWND, char *szFile, LPCSTR logTitle="DEVideo");
		DLL_API bool		Pause(void);
		DLL_API bool		Stop(void);
		DLL_API bool		Play();
		DLL_API bool		Update(bool loop, bool autoRelease=true);
		DLL_API void		OnPaint(HWND hwnd);			//case WM_PAINT
		DLL_API void		OnDisplayChange(HWND hwnd);	//case WM_DISPLAYCHANGE
		DLL_API void		Release();
		//Step(se possibile)
		DLL_API bool		StepToFrame(int frameNumber);
		DLL_API bool		StepToNextFrame();
		//Rating
		DLL_API HRESULT		ModifyRate(double dRateAdjust);
		DLL_API HRESULT		SetRate(double dRate);
		//Toggle
		DLL_API bool		SetVideoWindow(RECT rect);
		DLL_API HRESULT		ToggleMute(void);
		DLL_API inline bool GetCreated();
		DLL_API bool		GetVideoSize(LONG *lWidth, LONG *lHeight);
		DLL_API bool		CaptureImage(LPCTSTR lpszFileName);
		DLL_API void		GetScreenshotFileName(string& FileName);
		//Tag
		DLL_API bool		SetTag(LPCSTR newTag);
		DLL_API LPCSTR		GetTag();
};

/*Esempio di inizializzazione
//Avvio del clip
DEVideo Video;
Video.Create(mainHWND, "Videos\\desdinova.avi", true);
Video.Play();

//Chiudi
Video.CloseClip();

//Pausa
Video.PauseClip();

//Ferma
Video.StopClip();

//Mette e toglie audio
Video.ToggleMute();

//Avanzamento singolo frame
Video.StepOneFrame();

//Riduce la velocità del 25%
Video.ModifyRate(-0.25);

//Aumenta la velocità del 25%
Video.ModifyRate(0.25);

//Velocità normale
Video.SetRate(1.0);

//Riduce la velocità della metà
Video.SetRate(0.5);

//Aumenta la velocità del doppia
Video.SetRate(2.0);
*/

#endif