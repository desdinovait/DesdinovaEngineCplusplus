#ifndef H_BILLBOARD_H
#define H_BILLBOARD_H

/*
[CLASS] 
[
    [NOME] 
    DEBillboard
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle superfici billboard (cioè superfici 
	sempre rivolte verso la telecamera).
]
*/
class DEBillboard
{
	private:
		//** Stutture e altre definizioni **
		#define CUSTOMVERTEX_BILLBOARD_D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
		struct CUSTOMVERTEX_BILLBOARD
		{
			D3DXVECTOR3 pos;
			DWORD colour;
			float u, v;
		};

		//** Variabili **
		CUSTOMVERTEX_BILLBOARD		*pVertices;
		DWORD						vertexProcessingUsage;
		LPDIRECT3DVERTEXBUFFER9		VertexBuffer;
		int							textureID;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		D3DXCOLOR					color;
		float						captureDistance;
		bool						isCreated;
		D3DXMATRIX					worldMatrix;
		bool						visible;
		bool						cull;		
		D3DXVECTOR3					position;
		D3DXVECTOR2					scale;
		ENUM_DE_BBROTATION			rotationType;
		float						rotationVelocity;
		float						rotationAngleCurrent;		

		//** Funzioni **
		DLL_API bool				UpdateVertices();
		DLL_API bool				CreateVertexBuffer();
		DLL_API void				SetDefaultRenderStates();

	public:	
		//** Funzioni **
		DLL_API						DEBillboard();
		DLL_API bool				Create(const char *szTextureFilePath, DWORD initColor, LPCSTR logTitle="DEBillboard");
		DLL_API	bool				Invalidate();
		DLL_API	bool				Restore();
		DLL_API	bool				Release();

		DLL_API void				Render(bool useFadingNear, bool useFadingFar, float initDistance, float nearDistance, float centerDistance,  float farDistance, D3DXVECTOR3 centerCamera, bool useFrustrumCulling, STRUCT_DE_FOV fovProperties);
		DLL_API inline void			SetColor(DWORD newColor);
		DLL_API inline void			SetPosition(D3DXVECTOR3 newPosition);
		DLL_API inline void			SetScale(D3DXVECTOR2 newScale);
		DLL_API inline void			SetScaleEqual(float newScale);
		DLL_API inline void			SetRotation(ENUM_DE_BBROTATION rotateType, float newVelocity);
		DLL_API	inline void			SetWorldMatrix(D3DXMATRIX newMatrix);

		DLL_API inline bool			GetCreated();
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();

};

#endif 
