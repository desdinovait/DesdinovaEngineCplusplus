#ifndef H_BOUNDINGSPHERE_H
#define H_BOUNDINGSPHERE_H

/*
[CLASS] 
[
    [NOME] 
    DEBoundingSphere
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione di una sfera in wireframe.
	Questa classe è usata principalmente della classe dei modelli(CModel).
]
*/
class DEBoundingSphere
{
	private:
		//** Variabili **
		bool						isCreated;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		float						color[4];
		D3DXMATRIX					finalMatrix;

	public:
		//** Funzioni **
		DLL_API						DEBoundingSphere();
		DLL_API bool				Create(LPCSTR logTitle="DEBoundingSphere");
		DLL_API	bool				Invalidate();
		DLL_API	bool				Restore();
		DLL_API	bool				Release();
		
		DLL_API int					Render();
		DLL_API void				SetColor(float r, float g, float b);
		DLL_API void				CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation);
		DLL_API void				CalculateMatrix(D3DXMATRIX newMatrix);
		DLL_API inline D3DXMATRIX	GetFinalMatrix();
		DLL_API inline bool			GetCreated();
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();

};

#endif 
