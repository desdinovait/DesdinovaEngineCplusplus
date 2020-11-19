#ifndef H_INPUT_H
#define H_INPUT_H

#define KEYDOWN(name,key) (name[key] & 0x80)
/*
[CLASS] 
[
    [NOME] 
    DEInput
    
    [DESCRIZIONE]   
    Classe utilizzata per gestione degli input generati dall'utente tramite mouse e
	tastiera. E' possibile creare anche un puntatore del mouse personalizzato
	il quale può anche obbedire ad un fattore di sensibilità.
]
*/
class DEInput
{
	public:
		static LPDIRECTINPUT8			DirectInputObject;
		static LPDIRECTINPUTDEVICE8		KeyboardDevice;
		static LPDIRECTINPUTDEVICE8		MouseDevice;
		static LPDIRECTINPUTDEVICE8		JoyDevice;
	
	private:
		//** Varibili **
		bool						isCreated;
		bool						keyboardCreated;
		bool						mouseCreated;
		bool						joyCreated;
		HRESULT						hr;
		HWND						mainHWND;
		bool						mainWindowed;
		char						m_KeyBuffer[256];
		char						m_KeyBufferPrev[256];
		char						m_KeyBufferLoop[256];
		DIMOUSESTATE2				m_MouseState;
		bool						mouseTemp[8];
		DIJOYSTATE2					m_JoyState;
		bool						m_bInverted;
		float						m_fSensitivity;
		float						m_fCursorX;
		float						m_fCursorY;
		DWORD						m_dwAxes;
		DWORD						m_dwButtons;
		DWORD						m_dwScreenWidth;
		DWORD						m_dwScreenHeight;

	public:
		//** Funzioni **
		//Inizializzazione
		DLL_API bool				Init(HWND setHWND);
		DLL_API bool				Update(bool useKeyboard=true, bool useMouse=true, bool useJoy=true);
		DLL_API void				Release();
		//Pressione tasti
		DLL_API bool				Pressed_Keyboard(int Key, bool loop=true);
		DLL_API char				Pressed_KeyboardKey();
		DLL_API bool				Pressed_MouseDown(ENUM_DE_MOUSEBUTTON Button);
		DLL_API bool				Pressed_MouseUp(ENUM_DE_MOUSEBUTTON Button);
		DLL_API LONG				Wheel_Mouse();
		DLL_API bool				Pressed_Joy(ENUM_DE_JOYBUTTON Button);
		//Pannello di controllo
		DLL_API bool				RunControlPanel();
		DLL_API bool				RunControlPanel_Keyboard();
		DLL_API bool				RunControlPanel_Mouse();
		DLL_API bool				RunControlPanel_Joy();
		//Proprietà
		DLL_API LPDIDEVCAPS			GetCapabilities_Keyboard();
		DLL_API LPDIDEVCAPS			GetCapabilities_Mouse();
		DLL_API LPDIDEVCAPS			GetCapabilities_Joy();
		//Device Info
		DLL_API LPDIDEVICEINSTANCE	GetDeviceInfo_Keyboard();
		DLL_API LPDIDEVICEINSTANCE	GetDeviceInfo_Mouse();
		DLL_API LPDIDEVICEINSTANCE	GetDeviceInfo_Joy();
		//Stati
		DLL_API DIMOUSESTATE2		GetState_Mouse();
		DLL_API DIJOYSTATE2			GetState_Joy();
		//Cursore
		DLL_API void				SetCursorAttributes(bool bInvert,float fNewSensitivity);
		DLL_API void				SetCursorPosition(float,float);
		DLL_API void				SetCursorClip(HWND clipHWND);
		DLL_API void				ShowWindowCursor(bool showCursor);
		DLL_API D3DXVECTOR2			GetCursorPosition();

		DLL_API void				SetWindowed(bool inWindow);
		DLL_API bool				GetWindowed();
		DLL_API void				SetHWND(HWND cursorHWND);
		DLL_API HWND				GetHWND();

		//Posizioni
		DLL_API float				GetRelativeX_Mouse(void);
		DLL_API float				GetRelativeY_Mouse(void);
		DLL_API float				GetRelativeZ_Mouse(void);
		DLL_API float				GetAbsoluteX_Mouse(void);
		DLL_API float				GetAbsoluteY_Mouse(void);
		//Creazione
		DLL_API bool				GetCreated();
		DLL_API bool				isCreated_Mouse();
		DLL_API bool				isCreated_Keyboard();
		DLL_API bool				isCreated_Joy();

};

#endif