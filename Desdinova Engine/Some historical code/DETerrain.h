#ifndef H_TERRAIN_H
#define H_TERRAIN_H

/*
[CLASS] 
[
    [NOME] 
    DETerrain
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione di terreno
]
*/


//Define a FVF for our terrain
#define TERRAIN_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)


class DETerrain  
{
	private:
		//** Stutture e altre definizioni **
		struct TERRAIN_CUSTOMVERTEX
		{
			FLOAT x, y, z;		//Position of vertex in 3D space
			FLOAT nx, ny, nz;	//Lighting Normal
			FLOAT tu, tv;		//Texture coordinates
		};

		//** Variabili **
		LPDIRECT3DVERTEXBUFFER9 VertexBuffer;
		LPDIRECT3DTEXTURE9		Texture;
		D3DMATERIAL9			m_matMaterial;
		LPDIRECT3DINDEXBUFFER9	IndexBuffer;			
		WORD					m_wRows;
		WORD					m_wCols;
		WORD					m_wMaxHeight;
		float					m_rTileSize;
		DWORD					m_dwNumOfVertices;	
		DWORD					m_dwNumOfIndices;
		DWORD					m_dwNumOfPolygons;

		//** Funzioni **
		DLL_API bool			CreateIndexBuffer();
		DLL_API D3DXVECTOR3		GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3);
		DLL_API bool			UpdateVertices();
		DLL_API HRESULT			CreateVertexBuffer();

	public:
		//** Funzioni **
		DLL_API void			Init(WORD wRows, WORD wCols, float rTileSize, WORD wMaxHeight);
		DLL_API bool			SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower);
		DLL_API bool			SetTexture(const char* szTextureFilePath);
		DLL_API bool			SetSize(WORD wRows, WORD wCols, float rTileSize, WORD wMaxHeight);
		DLL_API DWORD			Render();
		DLL_API void			Release();

};

#endif
