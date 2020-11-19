#ifndef H_WATER_H
#define H_WATER_H

//** Stutture e altre definizioni **
#define CUSTOMVERTEX_WATER_D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
struct CUSTOMVERTEX_WATER
{
	float x, y, z;
	DWORD color;
	float u, v;
};		



/*
[CLASS] 
[
    [NOME] 
    DEWater
    
    [DESCRIZIONE]   
    Classe utilizzata per creare un piano di acqua in movimento
]
*/
class DEWater
{
	private:
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		bool					isCreated;
		D3DXMATRIX				finalMatrix;
		DWORD					vertexProcessingUsage;
		IDirect3DVertexBuffer9	*vertexBuf;
		IDirect3DIndexBuffer9	*indexBuf;
		int						vertListSize;
		int						indexListSize;
		int						textureID;
		CUSTOMVERTEX_WATER		*verts;
		WORD					*indices;
		int						vertCount;
		int						indexCount;
		float					*waveTable;		
	
		int						kWaterTesselation;	//Number of rows X cols of tessellated quads in water
		int						kWaveTableSize;		//Number of entries in the wave table
		float					kWaterForce;		//Controls the "choppiness" of the water (Range is 0.0f - 1.0f)
		float					kWaterWid;			//Width of water (along X-axis)
		float					kWaterHgt;			//Height of water (along Z-axis)

		DLL_API	void			SetDefaultRenderStates();

	public:	
		DLL_API					DEWater();
		DLL_API bool			Create(const char *szTextureFilePath, D3DCOLOR dwKeyColour, float width=50.0f, float height=50.0f, int tessellation=32, int tableSize=26, float force=0.04f, LPCSTR logTitle="DEWater");
		DLL_API	bool			Invalidate();
		DLL_API	bool			Restore();
		DLL_API	bool			Release();
		//Render
		DLL_API void			Render();
		//Settaggi correnti
		DLL_API void			CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation);
		DLL_API void			CalculateMatrix(D3DXMATRIX newMatrix);
		DLL_API inline D3DXMATRIX GetFinalMatrix();
		DLL_API	inline bool		GetCreated();
		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();	
};
#endif
