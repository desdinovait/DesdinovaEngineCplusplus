#ifndef H_SKY_H
#define H_SKY_H

//** Stutture e altre definizioni **
#define CUSTOMVERTEX_SKYCYLINDER_D3DFVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
struct CUSTOMVERTEX_SKYCYLINDER
{
	float		x;
	float		y;
	float		z;
	float		nx;
	float		ny;
	float		nz;
	float		tu;
	float		tv;
};

#define CUSTOMVERTEX_SKYBOX_D3DFVF (D3DFVF_XYZ|D3DFVF_TEX1)
struct CUSTOMVERTEX_SKYBOX
{
	float x, y, z;
	float tu, tv;
};

#define CUSTOMVERTEX_SKYSPHERE_D3DFVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
struct CUSTOMVERTEX_SKYSPHERE
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	float		tu;
	float		tv;

};



/*
[CLASS] 
[
    [NOME] 
    DESky
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione dello Sky
]
*/
class DESky
{
	private:
		//** Variabili **
		LPDIRECT3DVERTEXBUFFER9		VertexBuffer;
		DWORD						vertexProcessingUsage;
		CUSTOMVERTEX_SKYCYLINDER	*pVertices_Cylinder;
		CUSTOMVERTEX_SKYBOX			*pVertices_Box;
		CUSTOMVERTEX_SKYSPHERE		*pVertices_Sphere;
		D3DXMATRIX					finalMatrix;
		D3DXMATRIX					positionMatrix;
		D3DXMATRIX					rotationMatrix;
		D3DXMATRIX					scaleMatrix;
		D3DXVECTOR3					position;
		D3DXVECTOR3					rotation;
		D3DXVECTOR3					scale;
		bool						useTransparent;
		D3DXCOLOR					transparentColour;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		int							textureID[6];
		//Only cylinder
		DWORD						m_dwNumOfVertices;	
		DWORD						m_dwNumOfSidePolygons;
		DWORD						m_dwNumOfEndPolygons;
		DWORD						m_dwNumOfSegments;
		//Only sphere
		int							Segments;
		int							Rings;
		int							BallVBSize;

		ENUM_DE_SKYTYPE				skyType;
		bool						isCreated;
		D3DXMATRIX					worldMatrix;

	public:

		//** Funzioni **
		//Gestione
		DLL_API						DESky();
		DLL_API bool				Create(const char *szTextureFilePath_Top, const char *szTextureFilePath_Bottom, const char *szTextureFilePath_Side,float m_rHeight, float m_rRadius, int m_nSegments, LPCSTR logTitle="DESky_Cylinder");
		DLL_API bool				Create(const char *szTextureFilePath, int sphereSegments, int sphereRings, LPCSTR logTitle="DESky_Sphere");
		DLL_API bool				Create(const char *szTextureFilePath_Front, const char *szTextureFilePath_Back, const char *szTextureFilePath_Left, const char *szTextureFilePath_Right, const char *szTextureFilePath_Top, const char *szTextureFilePath_Bottom, LPCSTR logTitle="DESky_Box");
		DLL_API bool				Invalidate();
		DLL_API bool				Restore();
		DLL_API bool				Release();
		//Settaggi
		DLL_API void				SetPosition(D3DXVECTOR3 newPosition);
		DLL_API void				SetRotation(D3DXVECTOR3 newRotation);
		DLL_API void				SetScale(D3DXVECTOR3 newScale);
		DLL_API	void				SetTrasparency(bool isTransparent, float transparentValue);
		DLL_API	void				SetColor(D3DXCOLOR newColor);
		DLL_API	void				SetWorldMatrix(D3DXMATRIX newMatrix);
		//Settaggi correnti
		DLL_API D3DXVECTOR3			GetPosition();
		DLL_API D3DXVECTOR3			GetRotation();
		DLL_API D3DXVECTOR3			GetScale();
		DLL_API ENUM_DE_SKYTYPE		GetType();
		DLL_API	float				GetTrasparency();
		DLL_API bool				GetCreated();
		//Render
		DLL_API void				Render();
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();
};

#endif 
