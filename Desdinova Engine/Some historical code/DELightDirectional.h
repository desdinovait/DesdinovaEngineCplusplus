#ifndef H_LIGHTDIRECTIONAL_H
#define H_LIGHTDIRECTIONAL_H

/*
[CLASS] 
[
    [NOME] 
    DELightDirectional
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle luci DirectX.
	E' possibile configurare tutte le proprietà riguardanti le luci.
]
*/
class DELightDirectional
{
	private:
		//** Variabili
		D3DLIGHT9				d3dLight;
		unsigned int			maxLights;
		int						index;
		unsigned short			priority;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		bool					isEnable;
		bool					isDinamicEnable;
		bool					isCreated;

	public:
		//** Funzioni **
		DLL_API					DELightDirectional();
		DLL_API bool			Create(LPCSTR logTitle="DELightDirectional");
		DLL_API bool			Restore();
		DLL_API bool			Invalidate();
		DLL_API bool			Release();

		DLL_API void			SetPriority(unsigned short newPriority);
		DLL_API void			SetDirection(float dirX, float dirY, float dirZ);
		DLL_API void			SetDiffuse(float diffuseR, float diffuseG, float diffuseB, float diffuseA);
		DLL_API void			SetAmbient(float ambientR, float ambientG, float ambientB, float ambientA);
		DLL_API void			SetSpecular(float specularR, float specularG, float specularB, float specularA);
		DLL_API void			SetDinamicEnable(bool newDinamicEnable);
		DLL_API bool			GetDinamicEnable();
		DLL_API D3DXMATRIX		CalculateOccupationMatrix();
		DLL_API void			SetEnable(bool enable);
		DLL_API bool			GetEnable();
		DLL_API bool			GetCreated();
		DLL_API unsigned short	GetPriority();
		DLL_API int				GetIndex();

		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();

};


#endif