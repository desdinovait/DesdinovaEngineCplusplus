#ifndef H_LIGHTOMNI_H
#define H_LIGHTOMNI_H

/*
[CLASS] 
[
    [NOME] 
    DELightOmni
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle luci DirectX.
	E' possibile configurare tutte le proprietÓ riguardanti le luci.
]
*/
class DELightOmni
{
	private:
		//** Variabili
		D3DLIGHT9				d3dLight;
		DEBillboard				lightBill;
		unsigned int			maxLights;
		int						index;
		unsigned short			priority;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		bool					isFar;
		bool					isEnable;
		bool					isDinamicEnable;
		bool					isCulled;
		bool					isCreated;

	public:
		//** Funzioni **
		DLL_API					DELightOmni();
		DLL_API bool			Create(LPCSTR textureName, LPCSTR logTitle="DELightOmni");
		DLL_API bool			Restore();
		DLL_API bool			Invalidate();
		DLL_API bool			Release();

		DLL_API void			Render(D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties);
		DLL_API void			SetPriority(unsigned short newPriority);
		DLL_API void			SetPosition(float posX, float posY, float posZ);
		DLL_API void			SetDiffuse(float diffuseR, float diffuseG, float diffuseB, float diffuseA);
		DLL_API void			SetAmbient(float ambientR, float ambientG, float ambientB, float ambientA);
		DLL_API void			SetSpecular(float specularR, float specularG, float specularB, float specularA);
		DLL_API void			SetAttenuation(float attenuation0, float attenuation1, float attenuation2);
		DLL_API void			SetRange(float range);
		DLL_API void			SetEnable(bool enable);
		DLL_API void			SetDinamicEnable(bool newDinamicEnable);
		DLL_API bool			GetDinamicEnable();
		DLL_API bool			GetEnable();
		DLL_API D3DLIGHT9		GetLightProperties();
		DLL_API bool			GetCulled();
		DLL_API bool			GetFar();
		DLL_API D3DXMATRIX		CalculateOccupationMatrix();
		DLL_API bool			CheckDistance(D3DXVECTOR3 centerCamera);
		DLL_API bool			CheckPosition(D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties);
		DLL_API bool			GetCreated();
		DLL_API unsigned short	GetPriority();
		DLL_API int				GetIndex();

		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();

};


#endif