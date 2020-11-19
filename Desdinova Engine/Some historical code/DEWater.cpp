#include "DEGeneralIncludes.h"
#include "DEWater.h"

// Helper function for determining if a float is less than or equal to
// another float
DLL_API inline bool ToleranceLE(float a, float b, float tolerance = 0.001f)
{
	return ((a - tolerance) <= b);
}


DLL_API DEWater::DEWater()
{
	verts = NULL;
	indices = NULL;
	vertCount = 0;
	indexCount = 0;
	
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;

	vertexBuf = NULL;
	indexBuf = NULL;

	kWaterTesselation = 32;
	kWaveTableSize = 36;
	kWaterForce = 1.0f / 25.0f;
	kWaterWid = 50.0f;
	kWaterHgt = 50.0f;
}


DLL_API bool DEWater::Create(const char *szTextureFilePath, D3DCOLOR dwKeyColour, float width, float height, int tessellation, int tableSize, float force, LPCSTR logTitle)
{
	HRESULT hr;
	isCreated=false;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{
			D3DXMatrixIdentity(&finalMatrix);

			kWaterTesselation = tessellation;
			kWaveTableSize = tableSize;
			kWaterForce = force;
			kWaterWid = width;
			kWaterHgt = height;

			if(kWaterWid < 0.0f || kWaterHgt < 0.0f || kWaterTesselation <= 0)
			return false;

			//Riempie la wave table
			float angInc = 360.0f / (float)kWaveTableSize;
			float angle = 0.0f;
			
			waveTable = (float *)malloc(kWaveTableSize*sizeof(float));
			memset(waveTable, 0, sizeof(waveTable));	

			//Riempie la tabella con i valori del seno 
			for(int i = 0; i < kWaveTableSize; ++i)
			{
				waveTable[i] = sin(D3DXToRadian(angle));
				angle += angInc;
			}

			//** TASSELLAZIONE **
			// Creates a tesselated quad of (width x height) dimensions
			// Number of triangles in quad is (tessAmt * tessAmt * 2)
				
			// Calculate vertex count.  We know that we have (tesselation * tesselation * 2) triangles
			// with 3 vertices per triangle.  So each row of vertices will contain "number of rows + 1"
			// vertices and each column of vertices well contain "number of columns + 1" vertices.  This
			// Makes the total number of vertices the following equation
			vertCount = (kWaterTesselation + 1) * (kWaterTesselation + 1);
			
			// Calculate index count.  We know that we'll have (tesselation * tesselation * 2) triangles
			// and we have 3 vertices per triangle, thus the below equation
			indexCount = kWaterTesselation * kWaterTesselation * 6;
			
			// Allocate space for vertices and indices
			verts = new CUSTOMVERTEX_WATER[vertCount];
			indices = new WORD[indexCount];
			
			if(verts != NULL && indices != NULL)
			{			
				// Get the sub divided width and height of each triangle
				float subDivideWid = kWaterWid * (1.0f / (float)kWaterTesselation);
				float subDivideHgt = kWaterHgt * (1.0f / (float)kWaterTesselation);
				
				//Water is always centered at (0,0,0)
				D3DXVECTOR3	kWaterCen;
				kWaterCen.x = 0.0f;
				kWaterCen.y = 0.0f;
				kWaterCen.z = 0.0f;

				// Calculate the starting (x,y) and ending (x,y) of the quad 
				float startX = kWaterCen.x - (kWaterWid / 2.0f);
				float startY = kWaterCen.z - (kWaterHgt / 2.0f);
				float endX = kWaterCen.x + (kWaterWid / 2.0f);
				float endY = kWaterCen.z + (kWaterHgt / 2.0f);
				
				float uCoord = 0.0f; // Holds the current trinagle's U coordinate
				float vCoord = 1.0f; // Holds the current triangle's V coordinate
				float uvInc = 1.0f / (float)kWaterTesselation; // Calculate the amount to add to each UV coordinate
				
				int i = 0; // Index for all of for...loops
				
				// Create all the vertices
				for(float y = startY; ToleranceLE(y, endY); y += subDivideHgt)
				{
					for(float x = startX; ToleranceLE(x, endX); x += subDivideWid, ++i)
					{
						// Set the value for the current vert
						verts[i].x = x;
						verts[i].y = kWaterCen.y;
						verts[i].z = y;
						verts[i].u = uCoord;
						verts[i].v = vCoord;
						verts[i].color = D3DCOLOR_ARGB(128, 255, 255, 255);
						
						uCoord += uvInc;			
					}
					
					// Set UV for next row of vertices
					uCoord = 0.0f;
					vCoord -= uvInc;
				}
				
				//vertCount = i; // Set vertex count
				assert(vertCount == i);
				
				int indexValue = 0;
				int nextRow = 0;
				
				// Look at this sample 2 x 2 quad to understand the pattern used to fill in the indices
				/*
					6----7----8		Index count =  24
					|\ 	 |\   |		Tesselation = 2	
					| \  | \  | 	
					|  \ |  \ |		Pattern is:	0, 3, 1
					|   \|   \|					1, 3, 4
					3----4----5					1, 4, 2
					|\ 	 |\   |					2, 4, 5
					| \  | \  | 				3, 6, 4
					|  \ |  \ |					4, 6, 7
					|   \|   \|					4, 7, 5,
					0----1----2					5, 7, 8
				*/
				
				// Create all the indices
				for(i = 0; i < indexCount; i += 6)
				{
					indices[i] = indexValue;
					indices[i + 1] = indexValue + (kWaterTesselation + 1);
					indices[i + 2] = indices[i] + 1;
					
					indexValue++;
					
					indices[i + 3] = indexValue;
					indices[i + 4] = indexValue + kWaterTesselation;
					indices[i + 5] = indices[i + 4] + 1;
					
					if(++nextRow == kWaterTesselation)
					{
						indexValue++;
						nextRow = 0;
					}
				}	
				//**************************************************************


				//Guarda se è stato settato il software processing
				if (DECore::D3DDevice->GetSoftwareVertexProcessing())
				{
					//In Software
					vertexProcessingUsage = D3DUSAGE_WRITEONLY | D3DUSAGE_SOFTWAREPROCESSING;
				}
				else
				{
					//In hardware
					vertexProcessingUsage = D3DUSAGE_WRITEONLY;
				}

				// Get the size of our vertex list
				vertListSize = sizeof(CUSTOMVERTEX_WATER) * vertCount;
				// Create the vertex buffer
				hr = DECore::D3DDevice->CreateVertexBuffer(vertListSize, vertexProcessingUsage| D3DUSAGE_DYNAMIC, CUSTOMVERTEX_WATER_D3DFVF, D3DPOOL_DEFAULT, &vertexBuf, NULL);										
				if(hr == D3D_OK)
				{
					// Calculate the size of the index list
					indexListSize = sizeof(WORD) * indexCount;
					// Create the index buffer
					hr = DECore::D3DDevice->CreateIndexBuffer(indexListSize, vertexProcessingUsage| D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuf, NULL);
					if(hr == D3D_OK)
					{
						//Crea la texture
						textureID = DETextureManager::CreateTexture(szTextureFilePath, dwKeyColour, false);

						//Creazione corretta
						isCreated = true;
					}
				}
			}
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);	

	return isCreated;
}



DLL_API void DEWater::CalculateMatrix(D3DXMATRIX newMatrix)
{
	//Setta la matrice
	finalMatrix = newMatrix;
}

DLL_API void DEWater::CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation)
{
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMATRIX scaleMatrix;
	D3DXMATRIX posMatrix;
	D3DXMATRIX rotMatrix;
	D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&posMatrix, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&rotMatrix, D3DXToRadian(rotation.y), D3DXToRadian(rotation.x), D3DXToRadian(rotation.z));
	//Setta la matrice
	finalMatrix = scaleMatrix * rotMatrix * posMatrix;
}


DLL_API void DEWater::Render()
{
	SetDefaultRenderStates();

	//** PROCESS **
	//Processa i movimenti dell'acqua
	static int startIndex = 0;
	int waveIndex = startIndex;
	int vertIndex = 0;
	// Loop through all rows of triangles
	for(int i = 0; i <= kWaterTesselation; ++i)
	{
		// Loop through all the columns of triangles
		for(int j = 0; j <= kWaterTesselation; ++j, ++vertIndex)
		{
			// Perturb each vertex
			verts[vertIndex].y += waveTable[waveIndex] * kWaterForce;	
			if(++waveIndex == kWaveTableSize)
				waveIndex = 0;
		}	
	}
	// Keep cycling thorough the waveTable
	if(++startIndex == kWaveTableSize)	startIndex = 0;

	//** NEW BUFFERS **
	//Vertex Buffer
	VOID *vertsVoid = NULL;
	HRESULT hr = vertexBuf->Lock(0, 0, (void**)&vertsVoid, 0);	
	if(hr == D3D_OK)
	{
		memcpy(vertsVoid, verts, vertListSize);
		vertexBuf->Unlock();
		//Index Buffer
		VOID* indicesVoid = NULL; 
		hr = indexBuf->Lock(0, 0, (void**)&indicesVoid, 0);
		if(hr != D3D_OK)
		{
			vertexBuf->Release();
			indexBuf->Release();
			return;
		}
		memcpy(indicesVoid, indices, indexListSize);
		indexBuf->Unlock();
	}
	else
	{
		vertexBuf->Release();
		return;
	}

	//** RENDER **
	DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );
	DECore::D3DDevice->SetStreamSource(0, vertexBuf, 0, sizeof(CUSTOMVERTEX_WATER));
	DECore::D3DDevice->SetIndices(indexBuf);
	DECore::D3DDevice->SetFVF(CUSTOMVERTEX_WATER_D3DFVF);
	DECore::D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,vertCount,0,indexCount / 3);
}


// Free the memory
DLL_API bool DEWater::Release()
{
	free(waveTable);
	vertexBuf->Release();
	indexBuf->Release();
	if(verts)
	{
		delete[] verts;
		verts = NULL;
	}
	if(indices)
	{
		delete[] indices;
		indices = NULL;
	}
	return true;
}
	



DLL_API bool DEWater::Invalidate()
{
	if (isCreated)
	{
		try
		{
			SafeRelease(vertexBuf);
			SafeRelease(indexBuf);
			return true;
		}
		catch(...)
		{
			return false;
		}
	}
	return false;
}

DLL_API bool DEWater::Restore()
{
	HRESULT hr;
	if (isCreated)
	{
		isCreated = false;
		// Create the vertex buffer
		hr = DECore::D3DDevice->CreateVertexBuffer(vertListSize, vertexProcessingUsage| D3DUSAGE_DYNAMIC, CUSTOMVERTEX_WATER_D3DFVF, D3DPOOL_DEFAULT, &vertexBuf, NULL);										
		if(hr == D3D_OK)
		{
			// Calculate the size of the index list
			indexListSize = sizeof(WORD) * indexCount;
			// Create the index buffer
			hr = DECore::D3DDevice->CreateIndexBuffer(indexListSize, vertexProcessingUsage| D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuf, NULL);
			if(hr == D3D_OK)
			{
				//Creazione corretta
				isCreated = true;
			}
		}
	}
	return false;
}






DLL_API void DEWater::SetDefaultRenderStates()
{
	DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	
	DECore::D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );			
		//DECore::D3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(96,96,96));
	
	DECore::D3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE);
	
	DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);
		DECore::D3DDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL  );
	
	DECore::D3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE);
	DECore::D3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);
	
	DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
 		DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	
	DECore::D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE );
		//DECore::D3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x00000000 );
		//DECore::D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,	D3DCMP_ALWAYS );

	DECore::D3DDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE);
			//DECore::D3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			
	DECore::D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE );
			//DECore::D3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
			
	DECore::D3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	DECore::D3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	DECore::D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		/*DECore::D3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGCOLOR, 0);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGSTART, (DWORD)0.0);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGEND,   (DWORD)1.0);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGDENSITY, (DWORD)1.0);*/

	DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);

	DECore::D3DDevice->SetTexture(0, DETextureManager::GetTexture(textureID).Texture_Pointer);
		DECore::D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		DECore::D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		DECore::D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	DECore::D3DDevice->SetTexture(1, NULL);
	DECore::D3DDevice->SetTexture(2, NULL);
	DECore::D3DDevice->SetTexture(3, NULL);
	DECore::D3DDevice->SetTexture(4, NULL);
	DECore::D3DDevice->SetTexture(5, NULL);
	DECore::D3DDevice->SetTexture(6, NULL);
	DECore::D3DDevice->SetTexture(7, NULL);
}


DLL_API inline bool DEWater::GetCreated()
{
	return isCreated;
}

DLL_API bool DEWater::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEWater::GetTag()
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
		