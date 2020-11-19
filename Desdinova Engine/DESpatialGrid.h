#ifndef H_SPATIALGRID_H
#define H_SPATIALGRID_H

/*
[CLASS] 
[
    [NOME] 
    DESpatialGrid
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dello spazio(collisioni, frustrum, ecc)
]
*/
class DESpatialGrid
{
	private:
		//Griglia
		unsigned short			grid[40][40][40];
		bool					gridFrustrum[40][40][40];
		DEBoundingBox			gridBox;
		unsigned short			cellsNeightbourhood[27];
		unsigned short			cellsPerSide;
		unsigned short			cellsPerSideHalf;
		unsigned short			cellDimension;
		unsigned short			cellDimensionHalf;
		unsigned short			gridDimension;
		unsigned short			gridDimensionHalf;
		unsigned short			cellsInFrustrum;
		float					cellRadius;
		unsigned short			cellsTotal;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;

	public:
		DLL_API bool			Create(unsigned short dimensionCell=100, unsigned short perSideCells=4, LPCSTR logTitle="DESpatialgrid");
		DLL_API void			Update(D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties);
		DLL_API void			Render(bool onlyNeightbourhood=false);
		DLL_API bool			GetCellInFrustrum(D3DXVECTOR3 position);
		DLL_API short			GetCellID(D3DXVECTOR3 position, bool calculateNeightbourhood);
		DLL_API unsigned short	GetCellsInFrustrum();
		DLL_API unsigned short	GetGridDimension();
		DLL_API unsigned short	GetCellDimension();
		DLL_API unsigned short	GetTotalCells();
		DLL_API bool			GetCellNeightbourhood(D3DXVECTOR3 position);
		DLL_API unsigned short	*GetCellsNeightbourhood();
		DLL_API bool			Invalidate();
		DLL_API bool			Restore();
		DLL_API bool			Release();
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();

};

#endif