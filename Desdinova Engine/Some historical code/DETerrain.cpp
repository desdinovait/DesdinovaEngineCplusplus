#include "DEGeneralIncludes.h"
#include "DETerrain.h"

DLL_API void DETerrain::Release()
{
	SafeRelease(Texture);
	SafeRelease(IndexBuffer);
	SafeRelease(VertexBuffer);
}


DLL_API void DETerrain::Init(WORD wRows, WORD wCols, float rTileSize, WORD wMaxHeight)
{
	VertexBuffer = NULL;
	IndexBuffer = NULL;
	Texture = NULL;

	//Set a default size and position
	m_wRows = wRows;
	m_wCols = wCols;
	
	//Set the tile size for the terrain
	m_rTileSize = rTileSize;

	//Set the max height for any vertex
	m_wMaxHeight = wMaxHeight;

	//Setup counts for this object
	m_dwNumOfVertices = (m_wCols + 1) * (m_wRows + 1);
	m_dwNumOfPolygons = m_wRows * m_wCols * 2;
	m_dwNumOfIndices  = m_dwNumOfPolygons * 3;

	//Set material default values (R, G, B, A)
	D3DCOLORVALUE rgbaDiffuse  = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaAmbient  = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaSpecular = {0.0, 0.0, 0.0, 0.0,};
	D3DCOLORVALUE rgbaEmissive = {0.0, 0.0, 0.0, 0.0,};
	
	SetMaterial(rgbaDiffuse, rgbaAmbient, rgbaSpecular, rgbaEmissive, 0);

	//Initialize Vertex Buffer
    if(SUCCEEDED(CreateVertexBuffer()))
	{
		if(CreateIndexBuffer())
		{
			UpdateVertices();
		}
	}
}

DLL_API DWORD DETerrain::Render()
{
	DECore::D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(TERRAIN_CUSTOMVERTEX));
	DECore::D3DDevice->SetFVF(TERRAIN_D3DFVF_CUSTOMVERTEX);
	
	if(Texture != NULL)
	{
		//A texture has been set. We want our texture to be shaded based
		//on the current light levels, so used D3DTOP_MODULATE.
		DECore::D3DDevice->SetTexture(0, Texture);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}
	else
	{
		//No texture has been set
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	//Select the material to use
	DECore::D3DDevice->SetMaterial(&m_matMaterial);

	//Select index buffer
	DECore::D3DDevice->SetIndices(IndexBuffer);

	//Render polygons from index buffer
	DECore::D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumOfVertices, 0, m_dwNumOfPolygons);

	//Return the number of polygons rendered
	return m_dwNumOfPolygons;
}


DLL_API HRESULT DETerrain::CreateVertexBuffer()
{
    //Create the vertex buffer from our DECore::D3DDevice.
    if(FAILED(DECore::D3DDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(TERRAIN_CUSTOMVERTEX),
                                               0, TERRAIN_D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_MANAGED, &VertexBuffer, NULL)))
    {
        return E_FAIL;
    }

    return S_OK;
}


DLL_API bool DETerrain::CreateIndexBuffer()
{
	VOID* pBufferIndices;

	//Create the index buffer from our DECore::D3DDevice
	if(FAILED(DECore::D3DDevice->CreateIndexBuffer(m_dwNumOfIndices * sizeof(WORD), 
											  0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
											  &IndexBuffer, NULL)))
	{
	    return false;
	}
	
	
	//Set values for the index buffer
	WORD* pIndices = new WORD[m_dwNumOfIndices];	//Array holds the indices
	
	WORD a = m_wCols + 1;
	WORD b = 0;
	WORD c = a + 1;

	WORD x, z, i = 0;
	for(z = 0; z < m_wRows; z++)
	{
		for(x = 0; x < m_wCols; x++)
		{
			pIndices[i]	= a;
			pIndices[i + 1]	= b;
			pIndices[i + 2]	= c;
			pIndices[i + 3]	= b + 1;
			pIndices[i + 4]	= c;
			pIndices[i + 5]	= b;

			a++;
			b++;
			c++;
			i += 6;
		}

		a = c;
		b++;
		c++;
	}

	//Get a pointer to the index buffer indices and lock the index buffer    
	IndexBuffer->Lock(0, m_dwNumOfIndices * sizeof(WORD), (void**)&pBufferIndices, 0);

	//Copy our stored indices values into the index buffer
	memcpy(pBufferIndices, pIndices, m_dwNumOfIndices * sizeof(WORD));
	
	//Unlock the index buffer
	IndexBuffer->Unlock();

	//Clean up
	delete pIndices;
	pIndices = NULL;

	return true;
}


DLL_API D3DXVECTOR3 DETerrain::GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3)
{
	D3DXVECTOR3 vNormal;
	D3DXVECTOR3 v1;
	D3DXVECTOR3 v2;

	D3DXVec3Subtract(&v1, vVertex2, vVertex1);
	D3DXVec3Subtract(&v2, vVertex3, vVertex1);

	D3DXVec3Cross(&vNormal, &v1, &v2);

	D3DXVec3Normalize(&vNormal, &vNormal);

	return vNormal;
}


DLL_API bool DETerrain::UpdateVertices()
{
	DWORD i = 0;
	VOID* pVertices;
	WORD* pBufferIndices;
	D3DXVECTOR3 vNormal;
	DWORD dwVertex1;
	DWORD dwVertex2;
	DWORD dwVertex3;

	WORD* pNumOfSharedPolygons = new WORD[m_dwNumOfVertices];							//Array holds how many times this vertex is shared
	D3DVECTOR* pSumVertexNormal = new D3DVECTOR[m_dwNumOfVertices];						//Array holds sum of all face normals for shared vertex
	TERRAIN_CUSTOMVERTEX* pcvVertices = new TERRAIN_CUSTOMVERTEX[m_dwNumOfVertices];	//Array holds the veritces																				

	float x, z;

	//Centre terrain around the origin
	float zStart	= (float)(0.0 - (m_wRows/2.0));
	float zEnd		= (float)(m_wRows/2.0);
	float xStart	= (float)(0.0 - (m_wCols/2.0));
	float xEnd		= (float)(m_wCols/2.0);

	//Initialise the random number generator
	srand(timeGetTime());  

	//Clear memory and setup vertices for terrain
	for(z = zStart; z <= zEnd; z++)
	{
		for(x = xStart; x <= xEnd; x++)
		{
			pNumOfSharedPolygons[i] = 0;
			pSumVertexNormal[i] = D3DXVECTOR3(0,0,0);

			pcvVertices[i].x = x * m_rTileSize;
			
			//Make sure that the edges are all the same level
			if((z == zStart) || (z == zEnd) || (x == xStart) || (x == xEnd))
			{
				pcvVertices[i].y = 0.0;
			}
			else
			{
				//Set a random height for y
				pcvVertices[i].y = (float)(rand() % m_wMaxHeight);
			}
			

			pcvVertices[i].z = -z * m_rTileSize;	
			pcvVertices[i].nx = 0.0;
			pcvVertices[i].ny = 0.0;
			pcvVertices[i].nz = 0.0;
			
			if(((int)z % 2) == 0)
			{
				if(((int)x % 2) == 0)
				{
					pcvVertices[i].tu = 0.0;
					pcvVertices[i].tv = 0.0;
				}
				else
				{
					pcvVertices[i].tu = 1.0;
					pcvVertices[i].tv = 0.0;
				}
			}
			else
			{
				if(((int)x % 2) == 0)
				{
					pcvVertices[i].tu = 0.0;
					pcvVertices[i].tv = 1.0;
				}
				else
				{
					pcvVertices[i].tu = 1.0;
					pcvVertices[i].tv = 1.0;
				}
			}

			i++;
		}
	}


	//Get a pointer to the index buffer indices and lock the index buffer    
	IndexBuffer->Lock(0, m_dwNumOfIndices * sizeof(WORD), (void**)&pBufferIndices, D3DLOCK_READONLY);

	//For each triangle, count the number of times each vertex is used and
	//add together the normals of faces that share a vertex
	for(i = 0; i < m_dwNumOfIndices; i += 3)
	{
		dwVertex1 = pBufferIndices[i];
		dwVertex2 = pBufferIndices[i + 1];
		dwVertex3 = pBufferIndices[i + 2];

		vNormal = GetTriangeNormal(&D3DXVECTOR3(pcvVertices[dwVertex1].x, pcvVertices[dwVertex1].y, pcvVertices[dwVertex1].z), 
								   &D3DXVECTOR3(pcvVertices[dwVertex2].x, pcvVertices[dwVertex2].y, pcvVertices[dwVertex2].z), 
								   &D3DXVECTOR3(pcvVertices[dwVertex3].x, pcvVertices[dwVertex3].y, pcvVertices[dwVertex3].z));

		
		pNumOfSharedPolygons[dwVertex1]++;
		pNumOfSharedPolygons[dwVertex2]++;
		pNumOfSharedPolygons[dwVertex3]++;

		pSumVertexNormal[dwVertex1].x += vNormal.x;
		pSumVertexNormal[dwVertex1].y += vNormal.y;
		pSumVertexNormal[dwVertex1].z += vNormal.z;
		
		pSumVertexNormal[dwVertex2].x += vNormal.x;
		pSumVertexNormal[dwVertex2].y += vNormal.y;
		pSumVertexNormal[dwVertex2].z += vNormal.z;

		pSumVertexNormal[dwVertex3].x += vNormal.x;
		pSumVertexNormal[dwVertex3].y += vNormal.y;
		pSumVertexNormal[dwVertex3].z += vNormal.z;
	}


	//Unlock the index buffer
	IndexBuffer->Unlock();

	//For each vertex, calculate the average normal
	//CHAR DEBUG[255];
	for(i = 0; i < m_dwNumOfVertices; i++)
	{
		vNormal.x = pSumVertexNormal[i].x / pNumOfSharedPolygons[i];
		vNormal.y = pSumVertexNormal[i].y / pNumOfSharedPolygons[i];
		vNormal.z = pSumVertexNormal[i].z / pNumOfSharedPolygons[i];

		D3DXVec3Normalize(&vNormal, &vNormal);

		pcvVertices[i].nx = vNormal.x;
		pcvVertices[i].ny = vNormal.y;
		pcvVertices[i].nz = vNormal.z;

		//sprintf(DEBUG, "Vertex Data %d: x = %f, y = %f, z = %f, nx = %f, ny = %f, nz = %f, tu = %f, tv = %f\n", i, pcvVertices[i].x, pcvVertices[i].y, pcvVertices[i].z, pcvVertices[i].nx, pcvVertices[i].ny, pcvVertices[i].nz, pcvVertices[i].tu, pcvVertices[i].tv);
		//OutputDebugString(DEBUG);
	}


	//Get a pointer to the vertex buffer vertices and lock the vertex buffer
    if(FAILED(VertexBuffer->Lock(0, m_dwNumOfVertices * sizeof(TERRAIN_CUSTOMVERTEX), (void**)&pVertices, 0)))
    {
        OutputDebugString("Lock Vertex Buffer Failed!\n");
		return false;
    }

    //Copy our stored vertices values into the vertex buffer
    memcpy(pVertices, pcvVertices, m_dwNumOfVertices * sizeof(TERRAIN_CUSTOMVERTEX));

    //Unlock the vertex buffer
    VertexBuffer->Unlock();
	

	//Clean up
	delete pNumOfSharedPolygons;
	delete pSumVertexNormal;
	delete pcvVertices;

	pNumOfSharedPolygons = NULL;
	pSumVertexNormal = NULL;
	pcvVertices = NULL;

	return true;
}


DLL_API bool DETerrain::SetSize(WORD wRows, WORD wCols, float rTileSize, WORD wMaxHeight)
{
	//Set size
	m_wRows = wRows;
	m_wCols = wCols;
	
	//Set the tile size for the terrain
	m_rTileSize = rTileSize;

	//Set the max height for any vertex
	m_wMaxHeight = wMaxHeight;

	//Setup counts for this object
	m_dwNumOfVertices = (m_wCols + 1)  * (m_wRows + 1);	
	m_dwNumOfPolygons = m_wRows * m_wCols * 2;
	m_dwNumOfIndices  = m_dwNumOfPolygons * 3;

	UpdateVertices();

	return true;
}


DLL_API bool DETerrain::SetTexture(const char *szTextureFilePath)
{
	if(FAILED(D3DXCreateTextureFromFile(DECore::D3DDevice, szTextureFilePath, &Texture)))
	{
		OutputDebugString("Failed to load texture.");
		return false;
	}

	return true;
}


DLL_API bool DETerrain::SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower)
{
	//Set the RGBA for diffuse light reflected from this material. 
	m_matMaterial.Diffuse = rgbaDiffuse; 

	//Set the RGBA for ambient light reflected from this material. 
	m_matMaterial.Ambient = rgbaAmbient; 

	//Set the color and sharpness of specular highlights for the material. 
	m_matMaterial.Specular = rgbaSpecular; 
	m_matMaterial.Power = rPower;

	//Set the RGBA for light emitted from this material. 
	m_matMaterial.Emissive = rgbaEmissive;

	return true;
}