#ifndef H_LIGHT_H
#define H_LIGHT_H

/*
[CLASS] 
[
    [NOME] 
    DELight
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle luci DirectX.
	E' possibile configurare tutte le proprietà riguardanti le luci.
]
*/
class DELight
{
	private:
		//** Variabili
		STRUCT_DE_LIGHT			light;
		DEBillboard				lightBill;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		bool					isFar;
		bool					isCulled;
		bool					isCreated;

	public:
		//** Funzioni **
		DLL_API					DELight();
		DLL_API bool			Create(LPCSTR textureName, LPCSTR logTitle="DELight");
		DLL_API bool			Restore();
		DLL_API bool			Invalidate();
		DLL_API bool			Release();

		DLL_API void			Render(D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties);
		DLL_API void			SetPriority(unsigned short newPriority);

		DLL_API void			SetType(ENUM_DE_LIGHTTYPE type);
		DLL_API void			SetDirection(D3DXVECTOR3 dir);
		DLL_API void			SetPosition(D3DXVECTOR3 pos);
		DLL_API void			SetDiffuse(D3DXCOLOR color);
		DLL_API void			SetFalloff(float falloff);
		DLL_API void			SetRange(float range);
		DLL_API void			SetEnable(bool enable);
		DLL_API void			SetDinamicEnable(bool newDinamicEnable);
		
		
		DLL_API bool			GetDinamicEnable();
		DLL_API bool			GetEnable();
		DLL_API STRUCT_DE_LIGHT	GetLightProperties();
		DLL_API bool			GetCulled();
		DLL_API bool			GetFar();
		DLL_API D3DXMATRIX		CalculateOccupationMatrix();
		DLL_API bool			CheckDistance(D3DXVECTOR3 centerCamera);
		DLL_API bool			CheckPosition(D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties);
		DLL_API bool			GetCreated();

		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();

};


#endif