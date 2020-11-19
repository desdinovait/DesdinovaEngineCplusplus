#ifndef H_AXES_H
#define H_AXES_H

#include "DEFont.h"

/*
[CLASS] 
[
    [NOME] 
    DEAxes
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione di assi cartesiani orientati (X,Y,Z).
	Solitamente utilizzati nella fase di debug o in editor.
]
*/
class DEAxes
{
	private:
		//** Variabili **
		bool					initialized;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		D3DXMATRIX				finalMatrix;
		bool					isCreated;
		DEFont					fontAxes;
		D3DXVECTOR3				fontAxes2DposX;
		D3DXVECTOR3				fontAxes2DposY;
		D3DXVECTOR3				fontAxes2DposZ;
		//** Funzioni **
		DLL_API void			SetDefaultRenderStates();

	public:
		//** Funzioni **
		//Gestione
		DLL_API					DEAxes();
		DLL_API	bool			Create(LPCSTR logTitle="DEAxes");
		DLL_API	bool			Invalidate();
		DLL_API	bool			Restore();
		DLL_API	bool			Release();
		//Render
		DLL_API void			Render();
		//Settaggi
		DLL_API void			CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation);
		DLL_API void			CalculateMatrix(D3DXMATRIX newMatrix);
		//Settaggi correnti
		DLL_API	inline bool		GetCreated();
		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();
};


#endif