#include "DEGeneralIncludes.h"

CUSTOMVERTEX_BOUNDINGBOX	*DEVBManager::pVerticesBoundingBox={NULL};
LPDIRECT3DVERTEXBUFFER9		DEVBManager::VertexBufferBoundingBox={NULL};		
CUSTOMVERTEX_BOUNDINGSPHERE	*DEVBManager::pVerticesBoundingSphere={NULL};
LPDIRECT3DVERTEXBUFFER9		DEVBManager::VertexBufferBoundingSphere={NULL};		
CUSTOMVERTEX_AXES			*DEVBManager::pVerticesAxes={NULL};
LPDIRECT3DVERTEXBUFFER9		DEVBManager::VertexBufferAxes={NULL};		
int							DEVBManager::dimensionSphere=0;
bool						DEVBManager::isCreatedBoundingBox=false;
bool						DEVBManager::isCreatedBoundingSphere=false;
bool						DEVBManager::isCreatedAxes=false;
DWORD						DEVBManager::vertexProcessingUsage;

DLL_API bool DEVBManager::CreateStaticElements()
{
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
	//Crea gli elementi
	CreateBoundingBox();
	CreateBoundingSphere();
	CreateAxes();
	return true;
}

DLL_API void DEVBManager::ReleaseStaticElements()
{
	ReleaseBoundingBox();
	ReleaseBoundingSphere();
	ReleaseAxes();
}

DLL_API bool DEVBManager::InvalidateStaticElements()
{
	//ReleaseStaticElements();
	return true;
}

DLL_API bool DEVBManager::RestoreStaticElements()
{
	//CreateStaticElements();
	return true;
}

DLL_API bool DEVBManager::CreateBoundingBox()
{
	HRESULT hr;
	isCreatedBoundingBox=false;
	if (DECore::D3DDevice!=NULL)
	{
		//Crea il VertexBuffer
		hr = DECore::D3DDevice->CreateVertexBuffer(36 * sizeof(CUSTOMVERTEX_BOUNDINGBOX), vertexProcessingUsage, CUSTOMVERTEX_BOUNDINGBOX_D3DFVF,D3DPOOL_MANAGED, &VertexBufferBoundingBox, NULL);
		if (hr==D3D_OK)
		{
			//Blocca il VertexBuffer
			hr = VertexBufferBoundingBox->Lock(0, 36 * sizeof(CUSTOMVERTEX_BOUNDINGBOX), (void**)&pVerticesBoundingBox, 0);
			if (hr==D3D_OK)
			{
				for (int i=0; i<=35; i++)
				{
					pVerticesBoundingBox[i].colour = D3DCOLOR_ARGB(255, 255, 255, 255);
				}

				//Front
				pVerticesBoundingBox[0].x = -1.0f;
				pVerticesBoundingBox[0].y = -1.0f;
				pVerticesBoundingBox[0].z = 1.0f;
				pVerticesBoundingBox[1].x = 1.0f;
				pVerticesBoundingBox[1].y = -1.0f;
				pVerticesBoundingBox[1].z = 1.0f; 
				pVerticesBoundingBox[2].x = -1.0f;
				pVerticesBoundingBox[2].y = 1.0f;
				pVerticesBoundingBox[2].z = 1.0f;

				pVerticesBoundingBox[3].x = 1.0f;
				pVerticesBoundingBox[3].y = -1.0f;
				pVerticesBoundingBox[3].z = 1.0f; 
				pVerticesBoundingBox[4].x = 1.0f;
				pVerticesBoundingBox[4].y = 1.0f;
				pVerticesBoundingBox[4].z = 1.0f;
				pVerticesBoundingBox[5].x = -1.0f;
				pVerticesBoundingBox[5].y = 1.0f;
				pVerticesBoundingBox[5].z = 1.0f;

				//Back
				pVerticesBoundingBox[6].x = -1.0f;
				pVerticesBoundingBox[6].y = -1.0f;
				pVerticesBoundingBox[6].z = -1.0f;
				pVerticesBoundingBox[7].x = -1.0f;
				pVerticesBoundingBox[7].y = 1.0f;
				pVerticesBoundingBox[7].z = -1.0f;
				pVerticesBoundingBox[8].x = 1.0f;
				pVerticesBoundingBox[8].y = -1.0f;
				pVerticesBoundingBox[8].z = -1.0f; 

				pVerticesBoundingBox[9].x = 1.0f;
				pVerticesBoundingBox[9].y = -1.0f;
				pVerticesBoundingBox[9].z = -1.0f; 
				pVerticesBoundingBox[10].x = -1.0f;
				pVerticesBoundingBox[10].y = 1.0f;
				pVerticesBoundingBox[10].z = -1.0f;
				pVerticesBoundingBox[11].x = 1.0f;
				pVerticesBoundingBox[11].y = 1.0f;
				pVerticesBoundingBox[11].z = -1.0f;

				//Left
				pVerticesBoundingBox[12].x = -1.0f;
				pVerticesBoundingBox[12].y = -1.0f;
				pVerticesBoundingBox[12].z = -1.0f;
				pVerticesBoundingBox[13].x = -1.0f;
				pVerticesBoundingBox[13].y = -1.0f;
				pVerticesBoundingBox[13].z = 1.0f; 
				pVerticesBoundingBox[14].x = -1.0f;
				pVerticesBoundingBox[14].y = 1.0f;
				pVerticesBoundingBox[14].z = -1.0f;

				pVerticesBoundingBox[15].x = -1.0f;
				pVerticesBoundingBox[15].y = -1.0f;
				pVerticesBoundingBox[15].z = 1.0f; 
				pVerticesBoundingBox[16].x = -1.0f;
				pVerticesBoundingBox[16].y = 1.0f;
				pVerticesBoundingBox[16].z = 1.0f;
				pVerticesBoundingBox[17].x = -1.0f;
				pVerticesBoundingBox[17].y = 1.0f;
				pVerticesBoundingBox[17].z = -1.0f;

				//Right
				pVerticesBoundingBox[18].x = 1.0f;
				pVerticesBoundingBox[18].y = -1.0f;
				pVerticesBoundingBox[18].z = -1.0f;
				pVerticesBoundingBox[19].x = 1.0f;
				pVerticesBoundingBox[19].y = 1.0f;
				pVerticesBoundingBox[19].z = -1.0f;
				pVerticesBoundingBox[20].x = 1.0f;
				pVerticesBoundingBox[20].y = -1.0f;
				pVerticesBoundingBox[20].z = 1.0f; 

				pVerticesBoundingBox[21].x = 1.0f;
				pVerticesBoundingBox[21].y = -1.0f;
				pVerticesBoundingBox[21].z = 1.0f; 
				pVerticesBoundingBox[22].x = 1.0f;
				pVerticesBoundingBox[22].y = 1.0f;
				pVerticesBoundingBox[22].z = -1.0f;
				pVerticesBoundingBox[23].x = 1.0f;
				pVerticesBoundingBox[23].y = 1.0f;
				pVerticesBoundingBox[23].z = 1.0f;

				//Up
				pVerticesBoundingBox[24].x = 1.0f;
				pVerticesBoundingBox[24].y = 1.0f;
				pVerticesBoundingBox[24].z = 1.0f;
				pVerticesBoundingBox[25].x = 1.0f;
				pVerticesBoundingBox[25].y = 1.0f;
				pVerticesBoundingBox[25].z = -1.0f;
				pVerticesBoundingBox[26].x = -1.0f;
				pVerticesBoundingBox[26].y = 1.0f;
				pVerticesBoundingBox[26].z = 1.0f; 

				pVerticesBoundingBox[27].x = 1.0f;
				pVerticesBoundingBox[27].y = 1.0f;
				pVerticesBoundingBox[27].z = -1.0f; 
				pVerticesBoundingBox[28].x = -1.0f;
				pVerticesBoundingBox[28].y = 1.0f;
				pVerticesBoundingBox[28].z = -1.0f;
				pVerticesBoundingBox[29].x = -1.0f;
				pVerticesBoundingBox[29].y = 1.0f;
				pVerticesBoundingBox[29].z = 1.0f;

				//Down
				pVerticesBoundingBox[30].x = 1.0f;
				pVerticesBoundingBox[30].y = -1.0f;
				pVerticesBoundingBox[30].z = 1.0f;
				pVerticesBoundingBox[31].x = -1.0f;
				pVerticesBoundingBox[31].y = -1.0f;
				pVerticesBoundingBox[31].z = 1.0f; 
				pVerticesBoundingBox[32].x = 1.0f;
				pVerticesBoundingBox[32].y = -1.0f;
				pVerticesBoundingBox[32].z = -1.0f;

				pVerticesBoundingBox[33].x = 1.0f;
				pVerticesBoundingBox[33].y = -1.0f;
				pVerticesBoundingBox[33].z = -1.0f; 
				pVerticesBoundingBox[34].x = -1.0f;
				pVerticesBoundingBox[34].y = -1.0f;
				pVerticesBoundingBox[34].z = 1.0f;
				pVerticesBoundingBox[35].x = -1.0f;
				pVerticesBoundingBox[35].y = -1.0f;
				pVerticesBoundingBox[35].z = -1.0f;

				//Slocca il VertexBuffer
				hr = VertexBufferBoundingBox->Unlock();
				if (hr == D3D_OK)
				{
					isCreatedBoundingBox = true;
				}
				else
				{
					DELog::LogError("<li>Static BoundingBox : VertexBuffer->Unlock(...): %s", DXGetErrorDescription9(hr));		
				}
			}
			else
			{
				DELog::LogError("<li>Static BoundingBox : VertexBuffer->Lock(...): %s", DXGetErrorDescription9(hr));
			}
		}
		else
		{
 			DELog::LogError("<li>Static BoundingBox : DECore::D3DDevice->CreateVertexBuffer(...): %s", DXGetErrorDescription9(hr));		
		}
	}
	else
	{
		DELog::LogError("<li>Static BoundingBox : DECore::D3DDevice: NULL");
	}

	return isCreatedBoundingBox;

}

DLL_API LPDIRECT3DVERTEXBUFFER9 DEVBManager::GetBoundingBoxVB()
{
	return VertexBufferBoundingBox;
}

DLL_API unsigned int DEVBManager::GetBoundingBoxSize()
{
	return sizeof(CUSTOMVERTEX_BOUNDINGBOX);
}

DLL_API DWORD DEVBManager::GetBoundingBoxFVF()
{
	return CUSTOMVERTEX_BOUNDINGBOX_D3DFVF;
}

DLL_API CUSTOMVERTEX_BOUNDINGBOX DEVBManager::GetBoundingBoxVertices(unsigned int vertexID)
{
	return pVerticesBoundingBox[vertexID];
}

DLL_API int DEVBManager::GetBoundingBoxDimension()
{
	return 36;
}

DLL_API bool DEVBManager::GetBoundingBoxGetCreated()
{
	return isCreatedBoundingBox;
}

DLL_API void DEVBManager::ReleaseBoundingBox()
{
	SafeRelease(VertexBufferBoundingBox);
}



DLL_API bool DEVBManager::CreateBoundingSphere(int Segments, int Rings)
{
	HRESULT hr;
	float BallRadius = 1.0f;
	// Calcola il numero dei vertici 	
	dimensionSphere = 2 * Rings * ( Segments + 1 ) ;	

	if (DECore::D3DDevice!=NULL)
	{	
		//Crea il VertexBuffer
		hr = DECore::D3DDevice->CreateVertexBuffer( dimensionSphere * sizeof (CUSTOMVERTEX_BOUNDINGSPHERE ), vertexProcessingUsage, CUSTOMVERTEX_BOUNDINGSPHERE_D3DFVF, D3DPOOL_MANAGED, &VertexBufferBoundingSphere, NULL );
		if (hr==D3D_OK)
		{
			//Blocca il VertexBuffer
			hr = VertexBufferBoundingSphere->Lock( 0, 0, (void**) & pVerticesBoundingSphere , 0 );
			if (hr==D3D_OK)
			{
				// Costanti per i calcoli	
				FLOAT fDeltaRingAngle = ( D3DX_PI / Rings );	
				FLOAT fDeltaSegAngle = ( 2.0f * D3DX_PI / Segments );	
				
				// Genera gli anelli della sfera	
				for(int ring = 0; ring < Rings; ring++ )	
				{
					FLOAT r0 = sinf ( (ring+0) * fDeltaRingAngle );	
					FLOAT r1 = sinf ( (ring+1) * fDeltaRingAngle );	
					FLOAT y0 = cosf ( (ring+0) * fDeltaRingAngle );	
					FLOAT y1 = cosf ( (ring+1) * fDeltaRingAngle );		
					
					// genera gli spicchi della sfera		
					for(int seg = 0; seg < ( Segments + 1 ) ; seg++ )		
					{			
						FLOAT x0 = r0 * sinf( seg * fDeltaSegAngle );			
						FLOAT z0 = r0 * cosf( seg * fDeltaSegAngle );			
						FLOAT x1 = r1 * sinf( seg * fDeltaSegAngle );			
						FLOAT z1 = r1 * cosf( seg * fDeltaSegAngle );			
						
						// Aggiunge due vertici per volta al VertexBuffer			
						pVerticesBoundingSphere->x = x0;
						pVerticesBoundingSphere->y = y0;
						pVerticesBoundingSphere->z = z0;
						pVerticesBoundingSphere->colour = D3DCOLOR_ARGB(255, 255, 255, 255);
						pVerticesBoundingSphere ++;

						pVerticesBoundingSphere->x = x1;
						pVerticesBoundingSphere->y = y1;
						pVerticesBoundingSphere->z = z1;
						pVerticesBoundingSphere->colour = D3DCOLOR_ARGB(255, 255, 255, 255);
						pVerticesBoundingSphere ++ ;		
					}	
				}
				
				// Sblocca il VertexBuffer	
				hr = VertexBufferBoundingSphere->Unlock();
				if (hr==D3D_OK)
				{
					isCreatedBoundingSphere=true;
				}
				else
				{
					DELog::LogError("<li>Static BoundingSphere : VertexBuffer->Unlock(...): %s", DXGetErrorDescription9(hr));
				}
			}
			else
			{
				DELog::LogError("<li>Static BoundingSphere : VertexBuffer->Lock(...): %s", DXGetErrorDescription9(hr));		
			}
		}
		else
		{
 			DELog::LogError("<li>Static BoundingSphere : DECore::D3DDevice->CreateVertexBuffer(...): %s", DXGetErrorDescription9(hr));
		}
	}
	else
	{
		DELog::LogError("<li>Static BoundingSphere : DECore::D3DDevice: NULL");
	}

	return isCreatedBoundingSphere;

}

DLL_API LPDIRECT3DVERTEXBUFFER9 DEVBManager::GetBoundingSphereVB()
{
	return VertexBufferBoundingSphere;
}

DLL_API unsigned int DEVBManager::GetBoundingSphereSize()
{
	return sizeof(CUSTOMVERTEX_BOUNDINGSPHERE);
}

DLL_API DWORD DEVBManager::GetBoundingSphereFVF()
{
	return CUSTOMVERTEX_BOUNDINGSPHERE_D3DFVF;
}

DLL_API CUSTOMVERTEX_BOUNDINGSPHERE DEVBManager::GetBoundingSphereVertices(unsigned int vertexID)
{
	return pVerticesBoundingSphere[vertexID];
}

DLL_API int DEVBManager::GetBoundingSphereDimension()
{
	return dimensionSphere;
}

DLL_API bool DEVBManager::GetBoundingSphereGetCreated()
{
	return isCreatedBoundingSphere;
}

DLL_API void DEVBManager::ReleaseBoundingSphere()
{
	SafeRelease(VertexBufferBoundingSphere);
}







DLL_API bool DEVBManager::CreateAxes()
{
	HRESULT hr;
	isCreatedAxes=false;
	if (DECore::D3DDevice!=NULL)
	{
		//Crea il VertexBuffer
		hr = DECore::D3DDevice->CreateVertexBuffer( 6 * sizeof(CUSTOMVERTEX_AXES), vertexProcessingUsage, CUSTOMVERTEX_AXES_D3DFVF, D3DPOOL_MANAGED, &VertexBufferAxes, NULL );
		if (hr == D3D_OK)
		{	
			//Blocca il VertexBuffer
			hr = VertexBufferAxes->Lock( 0, 6 * sizeof(CUSTOMVERTEX_AXES), (void**)&pVerticesAxes, 0 );
			if (hr == D3D_OK)
			{				
				pVerticesAxes[0].pos = D3DXVECTOR3(0.0,    0.0, 0.0);
				pVerticesAxes[1].pos = D3DXVECTOR3(0.0,  1.0, 0.0);
				pVerticesAxes[2].pos = D3DXVECTOR3(0.0,    0.0, 0.0);
				pVerticesAxes[3].pos = D3DXVECTOR3(0.0,  0.0, 1.0);
				pVerticesAxes[4].pos = D3DXVECTOR3(0.0,    0.0, 0.0);
				pVerticesAxes[5].pos = D3DXVECTOR3(1.0,  0.0, 0.0);

				pVerticesAxes[0].color = 0xff00ff00;
				pVerticesAxes[1].color = 0xff00ff00;
				pVerticesAxes[2].color = 0xff0000ff;
				pVerticesAxes[3].color = 0xff0000ff;
				pVerticesAxes[4].color = 0xffff0000;
				pVerticesAxes[5].color = 0xffff0000;

				//Sblocca il VertexBuffer
				hr = VertexBufferAxes->Unlock();
				if (hr==D3D_OK)
				{
					isCreatedAxes=true;
				}
				else
				{
					DELog::LogError("<li>Static Axes : VertexBuffer->Unlock(...): %s", DXGetErrorDescription9(hr));
				}
			}
			else
			{
				DELog::LogError("<li>Static Axes : VertexBuffer->Lock(...): %s", DXGetErrorDescription9(hr));		
			}
		}
		else
		{
 			DELog::LogError("<li>Static Axes : DECore::D3DDevice->CreateVertexBuffer(...): %s", DXGetErrorDescription9(hr));
		}
	}
	else
	{
		DELog::LogError("<li>Static Axes : DECore::D3DDevice: NULL");
	}

	return isCreatedAxes;
}


DLL_API LPDIRECT3DVERTEXBUFFER9 DEVBManager::GetAxesVB()
{
	return VertexBufferAxes;
}

DLL_API unsigned int DEVBManager::GetAxesSize()
{
	return sizeof(CUSTOMVERTEX_AXES);
}

DLL_API DWORD DEVBManager::GetAxesFVF()
{
	return CUSTOMVERTEX_AXES_D3DFVF;
}

DLL_API bool DEVBManager::GetAxesGetCreated()
{
	return isCreatedAxes;
}

DLL_API void DEVBManager::ReleaseAxes()
{
	SafeRelease(VertexBufferAxes);
}