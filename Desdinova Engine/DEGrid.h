#ifndef H_GRID_H
#define H_GRID_H

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
class DEGrid
{
	private:
		//** Stutture e altre definizioni **
		#define CUSTOMVERTEX_GRID_D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
		struct CUSTOMVERTEX_GRID
		{
			D3DXVECTOR3 pos;
			DWORD color;
		};

		//** Variabili **
		DWORD					vertexProcessingUsage;
		LPDIRECT3DVERTEXBUFFER9 VertexBuffer;
		D3DXMATRIX				finalMatrix;
		CUSTOMVERTEX_GRID		*Vertices;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		int						totalVertices;
		bool					isCreated;

		//** Funzioni **
		DLL_API void			SetDefaultRenderStates();


	public:
		//** Funzioni **
		DLL_API					DEGrid();
		DLL_API bool			Create(int sides, float sideLenght, D3DCOLOR color, D3DCOLOR centerColor, LPCSTR logTitle="DEGrid");
		DLL_API void			CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation);
		DLL_API void			CalculateMatrix(D3DXMATRIX newMatrix);
		DLL_API void			Render(); 
		DLL_API bool			GetCreated();
		DLL_API bool			Invalidate();
		DLL_API bool			Restore();
		DLL_API bool			Release();
		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();
};



#endif