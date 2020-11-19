#ifndef H_LOCKER_H
#define H_LOCKER_H

/*
[CLASS] 
[
    [NOME] 
    DELocker
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione dell'effetto di lens flare :-)
]
*/
class DELocker
{	
	private:
		//** Strutture e altre definizioni **

		//** Variabili **
		DEPanel				Locker;
		D3DVIEWPORT9		currentViewport;
		char				*mainLogTitle;
		int					screenWidth;
		int					screenHeight;
		float				middleScreenWidth;
		float				middleScreenHeight;
		D3DXVECTOR3			pos2D;
		D3DXVECTOR3			pos3D;
		float				currentScale;
		float				currentAlpha;
		bool				isCreated;
		bool				cull;

	public :
		//** Funzioni **
		DLL_API bool		Create(char *logTitle="DELocker");
		DLL_API void		CalculatePosition(D3DXVECTOR3 newPosition, D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties);
		DLL_API bool		FOVCheck(D3DXVECTOR3 *pViewPos, STRUCT_DE_FOV fovProperties);
		DLL_API void		Render();
		DLL_API bool		GetCreated();
		DLL_API void		Release();
};

#endif