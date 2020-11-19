#include "DEGeneralIncludes.h"

DLL_API bool DESpatialGrid::Create(unsigned short dimensionCell, unsigned short perSideCells, LPCSTR logTitle)
{
	mainLogTitle = logTitle;

	cellDimension = dimensionCell;
	cellDimensionHalf = cellDimension /2;
	
	//Il massimo è 40 perchè 40*40*40=64000 perchè lo unsigned short arriva ad un massimo di 65535
	if (perSideCells <=2 )perSideCells=2;
	if (perSideCells >=40 )perSideCells=40;

	cellsPerSide = perSideCells;
	cellsPerSideHalf = perSideCells /2;
	
	gridDimension = dimensionCell * cellsPerSide;
	gridDimensionHalf = gridDimension /2;
	
	short counter=0;
	for(unsigned short y=0; y<cellsPerSide; y++)
	{
		for (unsigned short z=0; z<cellsPerSide; z++)
		{
			for (unsigned short x=0; x<cellsPerSide; x++)
			{
				grid[x][y][z]=counter;
				gridFrustrum[x][y][z]=true;
				counter=counter+1;
			}
		}
	}
	cellsTotal = counter;

	//Crea il box usato per disegnare TUTTI i box
	gridBox.Create("DESpatialGrid");

	//Teorema di pitagora per cercare il raggio della sfera circoscritta al cubo
	//Se usassimo il raggio inscritto(cellDimensionHalf) il calcolo del check del frustrum
	//risulterebbe errato
	float r = pow((double)cellDimensionHalf, (double)2);
	cellRadius = sqrt( r + r );

	return true;
}

DLL_API short DESpatialGrid::GetCellID(D3DXVECTOR3 position, bool calculateNeightbourhood)
{
	short posX = (position.x / cellDimension) + cellsPerSideHalf;
	short posY = (position.y / cellDimension) + cellsPerSideHalf;
	short posZ = (position.z / cellDimension) + cellsPerSideHalf;
	short final = grid[posX][posY][posZ];
	if (final<=0)		final=0;
	if (final>=1000000)	final=1000000;
	if (calculateNeightbourhood)
	{
		short posXMin = posX-1;
		short posXMax = posX+1;
		short posYMin = posY-1;
		short posYMax = posY+1;
		short posZMin = posZ-1;
		short posZMax = posZ+1;

		if (posXMin<0)posXMin=0;
		if (posYMin<0)posYMin=0;
		if (posZMin<0)posZMin=0;
		if (posXMax>cellsPerSide)posXMax=cellsPerSide;
		if (posYMax>cellsPerSide)posYMax=cellsPerSide;
		if (posZMax>cellsPerSide)posZMax=cellsPerSide;

		//Piano X-Z e Y=0
		cellsNeightbourhood[0] = grid[posX][posY][posZ];
		cellsNeightbourhood[1] = grid[posXMin][posY][posZ];
		cellsNeightbourhood[2] = grid[posXMax][posY][posZ];
		cellsNeightbourhood[3] = grid[posX][posY][posZMax];
		cellsNeightbourhood[4] = grid[posX][posY][posZMin];
		cellsNeightbourhood[5] = grid[posXMin][posY][posZMax];
		cellsNeightbourhood[6] = grid[posXMin][posY][posZMin];
		cellsNeightbourhood[7] = grid[posXMax][posY][posZMax];
		cellsNeightbourhood[8] = grid[posXMax][posY][posZMin];

		//Piano X-Z e Y=+1
		cellsNeightbourhood[9] = grid[posX][posYMax][posZ];
		cellsNeightbourhood[10] = grid[posXMin][posYMax][posZ];
		cellsNeightbourhood[11] = grid[posXMax][posYMax][posZ];
		cellsNeightbourhood[12] = grid[posX][posYMax][posZMax];
		cellsNeightbourhood[13] = grid[posX][posYMax][posZMin];
		cellsNeightbourhood[14] = grid[posXMin][posYMax][posZMax];
		cellsNeightbourhood[15] = grid[posXMin][posYMax][posZMin];
		cellsNeightbourhood[16] = grid[posXMax][posYMax][posZMax];
		cellsNeightbourhood[17] = grid[posXMax][posYMax][posZMin];
	
		//Piano X-Z e Y=-1
		cellsNeightbourhood[18] = grid[posX][posYMin][posZ];
		cellsNeightbourhood[19] = grid[posXMin][posYMin][posZ];
		cellsNeightbourhood[20] = grid[posXMax][posYMin][posZ];
		cellsNeightbourhood[21] = grid[posX][posYMin][posZMax];
		cellsNeightbourhood[22] = grid[posX][posYMin][posZMin];
		cellsNeightbourhood[23] = grid[posXMin][posYMin][posZMax];
		cellsNeightbourhood[24] = grid[posXMin][posYMin][posZMin];
		cellsNeightbourhood[25] = grid[posXMax][posYMin][posZMax];
		cellsNeightbourhood[26] = grid[posXMax][posYMin][posZMin];
	
		/*Piano X-Z e Y=0
		gridNeight[0] = grid[posX][posY][posZ];	
		gridNeight[1] = grid[posX-1][posY][posZ];
		gridNeight[2] = grid[posX+1][posY][posZ];
		gridNeight[3] = grid[posX][posY][posZ+1];
		gridNeight[4] = grid[posX][posY][posZ-1];
		gridNeight[5] = grid[posX-1][posY][posZ+1];
		gridNeight[6] = grid[posX-1][posY][posZ-1];
		gridNeight[7] = grid[posX+1][posY][posZ+1];
		gridNeight[8] = grid[posX+1][posY][posZ-1];

		//Piano X-Z e Y=+1
		gridNeight[9] = grid[posX][posY+1][posZ];
		gridNeight[10] = grid[posX-1][posY+1][posZ];
		gridNeight[11] = grid[posX+1][posY+1][posZ];
		gridNeight[12] = grid[posX][posY+1][posZ+1];
		gridNeight[13] = grid[posX][posY+1][posZ-1];
		gridNeight[14] = grid[posX-1][posY+1][posZ+1];
		gridNeight[15] = grid[posX-1][posY+1][posZ-1];
		gridNeight[16] = grid[posX+1][posY+1][posZ+1];
		gridNeight[17] = grid[posX+1][posY+1][posZ-1];
		
		//Piano X-Z e Y=-1
		gridNeight[18] = grid[posX][posY-1][posZ];
		gridNeight[19] = grid[posX-1][posY-1][posZ];
		gridNeight[20] = grid[posX+1][posY-1][posZ];
		gridNeight[21] = grid[posX][posY-1][posZ+1];
		gridNeight[22] = grid[posX][posY-1][posZ-1];
		gridNeight[23] = grid[posX-1][posY-1][posZ+1];
		gridNeight[24] = grid[posX-1][posY-1][posZ-1];
		gridNeight[25] = grid[posX+1][posY-1][posZ+1];
		gridNeight[26] = grid[posX+1][posY-1][posZ-1];*/
	}
	return final;
}




DLL_API bool DESpatialGrid::GetCellInFrustrum(D3DXVECTOR3 position)
{
	short posX = (position.x / cellDimension) + cellsPerSideHalf;
	short posY = (position.y / cellDimension) + cellsPerSideHalf;
	short posZ = (position.z / cellDimension) + cellsPerSideHalf;
	return gridFrustrum[posX][posY][posZ];
}

DLL_API bool DESpatialGrid::GetCellNeightbourhood(D3DXVECTOR3 position)
{
	short posX = (position.x / cellDimension) + cellsPerSideHalf;
	short posY = (position.y / cellDimension) + cellsPerSideHalf;
	short posZ = (position.z / cellDimension) + cellsPerSideHalf;
	short final = grid[posX][posY][posZ];
	for (int i=0; i<26; i++)
	{
		if (cellsNeightbourhood[i] == final)
		{
			return true;
		}
	}
	return false;
}


DLL_API unsigned short DESpatialGrid::GetCellsInFrustrum()
{
	return cellsInFrustrum;
}
DLL_API unsigned short DESpatialGrid::GetGridDimension()
{
	return gridDimension;
}
DLL_API unsigned short DESpatialGrid::GetCellDimension()
{
	return cellDimension;
}
DLL_API unsigned short DESpatialGrid::GetTotalCells()
{
	return cellsTotal;
}
DLL_API unsigned short *DESpatialGrid::GetCellsNeightbourhood()
{
	return cellsNeightbourhood;
}
DLL_API void DESpatialGrid::Update(D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties)
{
	//Controlla le celle nel frustrum
	D3DXMATRIX viewTransform;
	DECore::D3DDevice->GetTransform(D3DTS_VIEW,& viewTransform);
	cellsInFrustrum = 0;
	//Il ciclo è così definito perchè prima si scrive sulla z, poi sulla z e infine sulla y
	for(unsigned short y=0; y<cellsPerSide; y++)
	{
		for (unsigned short z=0; z<cellsPerSide; z++)
		{
			for (unsigned short x=0; x<cellsPerSide; x++)
			{
				short posX = (x*cellDimension) + cellDimensionHalf - gridDimensionHalf ;
				short posY = (y*cellDimension) + cellDimensionHalf - gridDimensionHalf ;
				short posZ = (z*cellDimension) + cellDimensionHalf - gridDimensionHalf ;
				D3DXVECTOR3 cellPosition = D3DXVECTOR3(posX, posY, posZ);	
				if (DEUtility::FOVCheck(cellPosition, cellRadius, viewTransform, fovProperties, &viewPos))	
				{
					gridFrustrum[x][y][z]=true;
					cellsInFrustrum = cellsInFrustrum + 1;
				}
				else
				{
					gridFrustrum[x][y][z]=false;
				}
			}
		}
	}//*/
}


DLL_API void DESpatialGrid::Render(bool onlyNeightbourhood)
{
	//Disegna la griglia
	D3DXMATRIX identityMatrix;
	D3DXMatrixIdentity(&identityMatrix);
	//Il ciclo è così definito perchè prima si scrive sulla z, poi sulla z e infine sulla y
	for(unsigned short y=0; y<cellsPerSide; y++)
	{
		for (unsigned short z=0; z<cellsPerSide; z++)
		{
			for (unsigned short x=0; x<cellsPerSide; x++)
			{
				//Se è nel frustrum lo disegna
				if (gridFrustrum[x][y][z])
				{
					short posX = (x*cellDimension) + cellDimensionHalf - gridDimensionHalf ;
					short posY = (y*cellDimension) + cellDimensionHalf - gridDimensionHalf ;
					short posZ = (z*cellDimension) + cellDimensionHalf - gridDimensionHalf ;
					D3DXVECTOR3 cellPosition = D3DXVECTOR3(posX, posY, posZ);	
					gridBox.CalculateMatrix(cellPosition, D3DXVECTOR3(cellDimensionHalf, cellDimensionHalf, cellDimensionHalf), D3DXVECTOR3(0,0,0));
					for (unsigned short i=0; i<27; i++)
					{
						if (grid[x][y][z] == cellsNeightbourhood[i])
						{
							//gridBox.SetColor(D3DXCOLOR(255,0,0,0));
							gridBox.CalculateMatrix(cellPosition, D3DXVECTOR3(cellDimensionHalf*0.999f, cellDimensionHalf*0.999f, cellDimensionHalf*0.999f), D3DXVECTOR3(0,0,0));
							if (onlyNeightbourhood)	gridBox.Render();		
							goto exits;
						}
						else
						{
							//gridBox.SetColor(D3DXCOLOR(255,255,255,0));
						}
					}
					exits:
					if (onlyNeightbourhood==false)	gridBox.Render();		
				}
			}
		}
	}
}


DLL_API bool DESpatialGrid::Invalidate()
{
	try
	{
		gridBox.Invalidate();
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESpatialGrid::Restore()
{
	try
	{
		gridBox.Restore();
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESpatialGrid::Release()
{
	try
	{
		gridBox.Release();
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}


DLL_API bool DESpatialGrid::SetTag(LPCSTR newTag)
{
	try
	{
		mainTag = newTag;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API LPCSTR DESpatialGrid::GetTag()
{
	LPCSTR	nullTag="NoTag";
	try
	{
		if (mainTag!=NULL)	return mainTag;
		else				return nullTag;
	}
	catch(...)
	{
		return nullTag;
	}
	return nullTag;
}
