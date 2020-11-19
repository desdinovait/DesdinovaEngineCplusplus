#ifndef H_FONT_H
#define H_FONT_H

#include "DEPanel.h"

/*
[CLASS] 
[
    [NOME] 
    DEFont
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei font DirectX.
	E' possibile definire il carattere e tutte le proprietà Windows legato ad esso.
]
*/
class DEFont
{
	private:
		//** Variabili **
		LPD3DXFONT		m_pFont;
	    ID3DXSprite*	m_pD3DXSprite;
		DEPanel			panelBitmap;
		LPCSTR			mainLogTitle;
		LPCSTR			mainTag;

		LPSTR			mainText;
		RECT			mainRect;
		DWORD			mainFormat;
		D3DCOLOR		mainColour;
		float			mainScale;
		float			mainInner;
		int				mainMode;

		LPSTR			lettersString;
		bool			isCreated;
		bool			isSprite;
		bool			isBitmap;
		int				textureID;
		int				letterDimX;
		int				letterDimY;
		float			timeCounter;
		bool			allocCounter;
		char			*currentText;

		//** Funzioni **
		DLL_API int		CalculateBitmapFrame(char offsetChar);


	public:
		//** Funzioni **
		DLL_API			DEFont();
		DLL_API bool	Create(LPSTR face, int height, bool bold, bool italic, LPCSTR logTitle="DEFont");
		DLL_API bool	Create(LPSTR faceFile, DWORD fontKeyColor, unsigned int totalRows=8, unsigned int lettersPerRow=10, LPCSTR logTitle="DEFont");
		DLL_API void	DrawBitmap(char *pText, float x, float y, float scale, float interLetter, int mode, bool useTrasparent, float trasparentValue);
		//Generazione animazine stringa
		DLL_API char	*GenerateTypedText(LPSTR pText, float velocity, bool restart, int &strlenCurrent);
		//Begin/End
		DLL_API bool	BeginTextSprite();
		DLL_API bool	EndTextSprite();
		//Render
		DLL_API void	Render();
		DLL_API void	SetBitmapLettersString(LPSTR newLettersString);
		DLL_API void	SetText(LPSTR pText);
		DLL_API void	SetColor(D3DCOLOR newColor);
		DLL_API void	SetRectSprite(float x, float y, float width, float height, DWORD format);
		DLL_API void	SetRectBitmap(float x, float y, float scale, float interLetter, int mode);
		//Rilasci
		DLL_API bool	GetCreated();
		DLL_API bool	Invalidate();
		DLL_API bool	Restore();
		DLL_API bool	Release();
		//Tag
		DLL_API bool	SetTag(LPCSTR newTag);
		DLL_API LPCSTR	GetTag();
};

#endif