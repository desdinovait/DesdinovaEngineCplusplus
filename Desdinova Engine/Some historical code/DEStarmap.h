#ifndef H_STARMAP_H
#define H_STARMAP_H

/*
[CLASS] 
[
    [NOME] 
    DEStarmap
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione della mappa stellare
]
*/
class DEStarmap
{	
	private:
		//** Variabili **
		LPCSTR				mainLogTitle;
		LPCSTR				mainTag;
		bool				isCreated;
		STRUCT_DE_FOV		mainFovProperties;
		D3DXVECTOR3			mainViewPos;

		DEBillboard			Planet;


	public :

		//** Funzioni **
		DLL_API				DEStarmap();
		DLL_API bool		Create(STRUCT_DE_FOV fovProperties, LPCSTR logTitle="DEStarmap");
		DLL_API bool		Invalidate();
		DLL_API bool		Restore();		
		DLL_API bool		Release();
		
		DLL_API void		CalculatePosition(D3DXVECTOR3 viewPos);

		DLL_API void		Render2D();
		DLL_API void		Render3D();

		//Tag
		DLL_API bool		SetTag(LPCSTR newTag);
		DLL_API LPCSTR		GetTag();

};

#endif