#ifndef H_CONSOLE_H
#define H_CONSOLE_H

/*
[CLASS] 
[
    [NOME] 
    DEConsole
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione e la gestione della console.
]
*/
class DEConsoleManager
{		
	public:
		//** Variabili **
		typedef std::map<int, STRUCT_DE_CONSOLEFUNCTION>			Map_t;
		typedef std::map<int, STRUCT_DE_CONSOLEFUNCTION>::iterator	MapIter_t;
		//** Funzioni **
		DLL_API void			AddFunction( const char* fnName, STRUCT_DE_CONSOLEFUNCTION& fi );
		DLL_API std::string		CallFunction( const char* cmd, int size = 0 );

	private:		
		//** Variabili **
		Map_t mFunctionMap;	
		//** Funzioni **
		DLL_API bool			ParseBool( const char* str, bool& b );
		DLL_API bool			ParseChar( const char* str, char& c );
		DLL_API bool			ParseInt( const char* str, int& res );
		DLL_API bool			ParseFloat( const char* str, float& f );
		DLL_API bool			ParseDword( const char* str, DWORD& d );
		DLL_API DWORD			Call_cdecl( const void* args, size_t sz, DWORD func );


};



class DEConsole
{		
	private:
 		//** Variabili **
		bool						mbConsoleOn;
		DEConsoleManager			*mpConsole;
		char						mConsoleBuff[1024];
		char						mConsoleInfo[1024];
		char						mConsoleCmd[128];
		char						mConsoleMain[1024];
		int							mConsoleIdx;
		int							mConsoleTextHeight;
		int							mConsoleCaptionLines;
		int							mConsoleCmdIdx;
		DWORD						mConsoleTextColor;
		unsigned int				mConsoleTime;
		DEFont						mConsoleFont;
		bool						mConsoleOn;
		char						mainConsoleCmd[200][128];
		int							mainConsoleCmdIdx[200];
		unsigned int				currentConsoleHistory;
		unsigned int				totalConsoleHistory;
		D3DXVECTOR2					position;
		float						cursorFlicker;
		bool						isCreated;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;

	public:
		//** Funzioni **
		//Gestione
		DLL_API						DEConsole();
		DLL_API bool				Create(LPSTR fontFace="Arial", int fontHeight=14, bool fontBold=false, bool fontItalic=false, DWORD fontColor=D3DCOLOR_XRGB(0, 255, 0), bool initialOn=false, LPCSTR logTitle="DEConsole");
		DLL_API bool				Invalidate();
		DLL_API bool				Restore();	
		DLL_API bool				Release();
		//Aggiunta funzioni
		DLL_API bool				Add(void* prc, const char* name, ENUM_DE_CONSOLEPARAM p1=DE_CONSOLEPARAM_NULL, ENUM_DE_CONSOLEPARAM p2=DE_CONSOLEPARAM_NULL, ENUM_DE_CONSOLEPARAM p3=DE_CONSOLEPARAM_NULL, ENUM_DE_CONSOLEPARAM p4=DE_CONSOLEPARAM_NULL, ENUM_DE_CONSOLEPARAM p5=DE_CONSOLEPARAM_NULL, ENUM_DE_CONSOLEPARAM p6=DE_CONSOLEPARAM_NULL, ENUM_DE_CONSOLEPARAM p7=DE_CONSOLEPARAM_NULL );
		//Comandi
		DLL_API void				CmdHistoryUp();
		DLL_API void				CmdHistoryDown();
		DLL_API void				CmdBack();
		DLL_API void				CmdReturn();
		DLL_API void				CmdDefault(int wParamValue);
		DLL_API void				CmdDefault(char wParamValue);

		//Settaggi
		DLL_API inline void			SetTextColor(DWORD textColor);
		DLL_API inline void			SetEnable(bool enableConsole);
		DLL_API inline void			SetCaption(const char *maincaption, int occupedLines=1);
		DLL_API inline void			SetPosition(D3DXVECTOR2 newPosition);
		DLL_API inline void			SetCursorFlick(float newCursorFlicker);
		DLL_API inline void			SwitchEnable();
		//Settaggi correnti
		DLL_API inline DWORD		GetTextColor();
		DLL_API inline bool			GetEnable();
		DLL_API inline D3DXVECTOR2	GetPosition();
		DLL_API inline float		GetCursorFlicker();
		DLL_API inline bool			GetCreated();
		//Render
		DLL_API void				Render();
		//Log comandi
		DLL_API void				LogReport();
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();
};

#endif

/*Console
case WM_CHAR:
{
	switch( wParam )
	{
		case VK_TAB:
		{
			Engine.engineConsole.CmdHistoryDown();
		}
		break;

		case VK_RETURN:
		{
			Engine.engineConsole.CmdReturn();
		}
		break;

		case VK_BACK:
		{
			Engine.engineConsole.CmdBack();
		}
		break;

		default:
			Engine.engineConsole.CmdDefault(wParam);
	}
}
break;*/