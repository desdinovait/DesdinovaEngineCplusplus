#include "DEGeneralIncludes.h"

DLL_API DESky::DESky()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMatrixIdentity(&positionMatrix);
	D3DXMatrixIdentity(&rotationMatrix);
	D3DXMatrixIdentity(&scaleMatrix);
}



DLL_API bool DESky::Create(const char *szTextureFilePath_Top, const char *szTextureFilePath_Bottom,  const char *szTextureFilePath_Side,  float m_rHeight,  float m_rRadius, int m_nSegments, LPCSTR logTitle)
{
	HRESULT hr;
	VertexBuffer = NULL;
	pVertices_Box = NULL;
	pVertices_Sphere = NULL;
	pVertices_Cylinder = NULL;
	isCreated = false;	
	mainLogTitle = logTitle;
	skyType = DE_SKYTYPE_CYLINDER;
	this->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	try
	{
		// Calcola il numero dei vertici 	
		m_dwNumOfVertices = ((m_nSegments + 1) * 4) + 2;	
		m_dwNumOfSidePolygons = m_nSegments * 2;
		m_dwNumOfEndPolygons = m_nSegments;
		m_dwNumOfSegments = m_nSegments;

		if (DECore::D3DDevice!=NULL)
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

			//Crea il VertexBuffer
			hr = DECore::D3DDevice->CreateVertexBuffer( m_dwNumOfVertices * sizeof (CUSTOMVERTEX_SKYCYLINDER ), vertexProcessingUsage, CUSTOMVERTEX_SKYCYLINDER_D3DFVF, D3DPOOL_MANAGED, &VertexBuffer, NULL );
			if (hr==D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = VertexBuffer->Lock( 0, 0, (void**) & pVertices_Cylinder , 0 );
				if (hr==D3D_OK)
				{		
					WORD wVertexIndex = 0;
					int nCurrentSegment;

					float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
					float rSegmentLength = 1.0f / (float)m_nSegments;

					//Create the sides triangle strip
					for(nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++)
					{
						float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
						float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);

						pVertices_Cylinder->x = x0;
						pVertices_Cylinder->y = 0.0f + (m_rHeight / 2.0f);
						pVertices_Cylinder->z = z0;
						pVertices_Cylinder->nx = x0;
						pVertices_Cylinder->ny = 0.0f;
						pVertices_Cylinder->nz = z0;
						pVertices_Cylinder->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
						pVertices_Cylinder->tv = 0.0f;
						pVertices_Cylinder++;

						pVertices_Cylinder->x = x0;
						pVertices_Cylinder->y = 0.0f - (m_rHeight / 2.0f);
						pVertices_Cylinder->z = z0;
						pVertices_Cylinder->nx = x0;
						pVertices_Cylinder->ny = 0.0f;
						pVertices_Cylinder->nz = z0;
						pVertices_Cylinder->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
						pVertices_Cylinder->tv = 1.0f;
						pVertices_Cylinder++;
					}
					
					//Create the top triangle fan: Center
					pVertices_Cylinder->x = 0.0f;
					pVertices_Cylinder->y = 0.0f + (m_rHeight / 2.0f);
					pVertices_Cylinder->z = 0.0f;
					pVertices_Cylinder->nx = 0.0f;
					pVertices_Cylinder->ny = 1.0f;
					pVertices_Cylinder->nz = 0.0f;
					pVertices_Cylinder->tu = 0.5f;
					pVertices_Cylinder->tv = 0.5f;
					pVertices_Cylinder++;

					//Create the top triangle fan: Edges
					for(nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++)
					{
						float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
						float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);
					
						pVertices_Cylinder->x = x0;
						pVertices_Cylinder->y = 0.0f + (m_rHeight / 2.0f);
						pVertices_Cylinder->z = z0;
						pVertices_Cylinder->nx = 0.0f;
						pVertices_Cylinder->ny = 1.0f;
						pVertices_Cylinder->nz = 0.0f;

						float tu0 = (0.5f * sinf(nCurrentSegment * rDeltaSegAngle)) + 0.5f;
						float tv0 = (0.5f * cosf(nCurrentSegment * rDeltaSegAngle)) + 0.5f;

						pVertices_Cylinder->tu = tu0;
						pVertices_Cylinder->tv = tv0;
						pVertices_Cylinder++;
					}

					//Create the bottom triangle fan: Center
					pVertices_Cylinder->x = 0.0f;
					pVertices_Cylinder->y = 0.0f - (m_rHeight / 2.0f);
					pVertices_Cylinder->z = 0.0f;
					pVertices_Cylinder->nx = 0.0f;
					pVertices_Cylinder->ny = -1.0f;
					pVertices_Cylinder->nz = 0.0f;
					pVertices_Cylinder->tu = 0.5f;
					pVertices_Cylinder->tv = 0.5f;
					pVertices_Cylinder++;

					//Create the bottom triangle fan: Edges
					for(nCurrentSegment = m_nSegments; nCurrentSegment >= 0; nCurrentSegment--)
					{
						float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
						float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);
					
						pVertices_Cylinder->x = x0;
						pVertices_Cylinder->y = 0.0f - (m_rHeight / 2.0f);
						pVertices_Cylinder->z = z0;
						pVertices_Cylinder->nx = 0.0f;
						pVertices_Cylinder->ny = -1.0f;
						pVertices_Cylinder->nz = 0.0f;


						float tu0 = (0.5f * sinf(nCurrentSegment * rDeltaSegAngle)) + 0.5f;
						float tv0 = (0.5f * cosf(nCurrentSegment * rDeltaSegAngle)) + 0.5f;

						pVertices_Cylinder->tu = tu0;
						pVertices_Cylinder->tv = tv0;
						pVertices_Cylinder++;
					}


					// Sblocca il VertexBuffer	
					hr = VertexBuffer->Unlock();
					if (hr==D3D_OK)
					{
						isCreated=true;

						//Crea la texture 0
						textureID[0] = DETextureManager::CreateTexture(szTextureFilePath_Top, NULL, false);
						if (textureID[0] < 0)
						{
							DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Top);
						}

						//Crea la texture 1
						textureID[1] = DETextureManager::CreateTexture(szTextureFilePath_Bottom, NULL, false);
						if (textureID[1] < 0)
						{
							DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Bottom);
						}

						//Crea la texture 2
						textureID[2] = DETextureManager::CreateTexture(szTextureFilePath_Side, NULL, false);
						if (textureID[2] < 0)
						{
							DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Side);
						}

					}
					else
					{
						DELog::LogError("<li>%s : VertexBuffer->Unlock(...): %s", logTitle, DXGetErrorDescription9(hr));		
					}
				}
				else
				{
					DELog::LogError("<li>%s : VertexBuffer->Lock(...): %s", logTitle, DXGetErrorDescription9(hr));		
				}
			}
			else
			{
 				DELog::LogError("<li>%s : DECore::D3DDevice->CreateVertexBuffer(...): %s", logTitle, DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>%s : DECore::D3DDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}




DLL_API bool DESky::Create(const char *szTextureFilePath, int sphereSegments, int sphereRings, LPCSTR logTitle)
{
	HRESULT hr;
	VertexBuffer = NULL;
	pVertices_Box = NULL;
	pVertices_Sphere = NULL;
	pVertices_Cylinder = NULL;
	isCreated = false;	
	mainLogTitle = logTitle;
	Segments = sphereSegments;
	Rings = sphereRings;
	skyType = DE_SKYTYPE_SPHERE;
	
	try
	{
		if (DECore::D3DDevice!=NULL)
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

			//Raggio
			float BallRadius = 1.0f;
			// Calcola il numero dei vertici 	
			BallVBSize = 2 * Rings * ( Segments + 1 ) ;	

			//Crea il VertexBuffer
			hr = DECore::D3DDevice->CreateVertexBuffer( BallVBSize * sizeof (CUSTOMVERTEX_SKYSPHERE ), vertexProcessingUsage, CUSTOMVERTEX_SKYSPHERE_D3DFVF, D3DPOOL_MANAGED, &VertexBuffer, NULL );
			if (hr==D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = VertexBuffer->Lock( 0, 0, (void**) & pVertices_Sphere , 0 );
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
							pVertices_Sphere->Position = D3DXVECTOR3(x0, y0, z0);
							pVertices_Sphere->Normal = pVertices_Sphere->Position;
							pVertices_Sphere->tu = -((FLOAT)seg) / Segments;			
							pVertices_Sphere->tv = (ring+0) / (FLOAT) Rings;
							pVertices_Sphere ++;

							pVertices_Sphere->Position = D3DXVECTOR3(x1, y1, z1);
							pVertices_Sphere->Normal = pVertices_Sphere->Position;
							pVertices_Sphere->tu = -((FLOAT)seg)/ Segments;			
							pVertices_Sphere->tv = (ring+1)/(FLOAT) Rings;
							pVertices_Sphere ++ ;		
						}	
					}
			
					// Sblocca il VertexBuffer	
					hr = VertexBuffer->Unlock();
					if (hr==D3D_OK)
					{
						isCreated=true;		

						//Crea la texture
						textureID[0] = DETextureManager::CreateTexture(szTextureFilePath, NULL, false);
						if (textureID < 0)
						{
							DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath);
						}		
					}
					else
					{
						DELog::LogError("<li>%s : VertexBuffer->Unlock(...): %s", logTitle, DXGetErrorDescription9(hr));		
					}
				}
				else
				{
					DELog::LogError("<li>%s : VertexBuffer->Lock(...): %s", logTitle, DXGetErrorDescription9(hr));		
				}
			}
			else
			{
 				DELog::LogError("<li>%s : DECore::D3DDevice->CreateVertexBuffer(...): %s", logTitle, DXGetErrorDescription9(hr));
			}		
		}
		else
		{
			DELog::LogError("<li>%s : DECore::D3DDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}




DLL_API bool DESky::Create(const char *szTextureFilePath_Front, const char *szTextureFilePath_Back, const char *szTextureFilePath_Left, const char *szTextureFilePath_Right, const char *szTextureFilePath_Top, const char *szTextureFilePath_Bottom, LPCSTR logTitle)
{
	HRESULT hr;
	VertexBuffer = NULL;
	pVertices_Box = NULL;
	pVertices_Sphere = NULL;
	pVertices_Cylinder = NULL;
	isCreated = false;
   	mainLogTitle = logTitle;
	skyType = DE_SKYTYPE_BOX;

	try
	{
		if (DECore::D3DDevice!=NULL)
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

			//Crea il VertexBuffer
			hr = DECore::D3DDevice->CreateVertexBuffer(36 * sizeof(CUSTOMVERTEX_SKYBOX), vertexProcessingUsage, CUSTOMVERTEX_SKYBOX_D3DFVF,D3DPOOL_MANAGED, &VertexBuffer, NULL);
			if (hr==D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = VertexBuffer->Lock(0, 36 * sizeof(CUSTOMVERTEX_SKYBOX), (void**)&pVertices_Box, 0);
				if (hr==D3D_OK)
				{
					//Front
					pVertices_Box[0].x = -1.0f;
					pVertices_Box[0].y = -1.0f;
					pVertices_Box[0].z = 1.0f;
					pVertices_Box[1].x = 1.0f;
					pVertices_Box[1].y = -1.0f;
					pVertices_Box[1].z = 1.0f; 
					pVertices_Box[2].x = -1.0f;
					pVertices_Box[2].y = 1.0f;
					pVertices_Box[2].z = 1.0f;

					pVertices_Box[0].tu = 0.0f;
					pVertices_Box[0].tv = 1.0f;
					pVertices_Box[1].tu = 1.0f;
					pVertices_Box[1].tv = 1.0f;
					pVertices_Box[2].tu = 0.0f;
					pVertices_Box[2].tv = 0.0f;

					pVertices_Box[3].x = 1.0f;
					pVertices_Box[3].y = -1.0f;
					pVertices_Box[3].z = 1.0f; 
					pVertices_Box[4].x = 1.0f;
					pVertices_Box[4].y = 1.0f;
					pVertices_Box[4].z = 1.0f;
					pVertices_Box[5].x = -1.0f;
					pVertices_Box[5].y = 1.0f;
					pVertices_Box[5].z = 1.0f;

					pVertices_Box[3].tu = 1.0f;
					pVertices_Box[3].tv = 1.0f;
					pVertices_Box[4].tu = 1.0f;
					pVertices_Box[4].tv = 0.0f;
					pVertices_Box[5].tu = 0.0f;
					pVertices_Box[5].tv = 0.0f;

					//Back		
					pVertices_Box[6].x = -1.0f;
					pVertices_Box[6].y = -1.0f;
					pVertices_Box[6].z = -1.0f;
					pVertices_Box[7].x = -1.0f;
					pVertices_Box[7].y = 1.0f;
					pVertices_Box[7].z = -1.0f;					
					pVertices_Box[8].x = 1.0f;
					pVertices_Box[8].y = -1.0f;
					pVertices_Box[8].z = -1.0f; 


					pVertices_Box[6].tu = 1.0f;
					pVertices_Box[6].tv = 1.0f;
					pVertices_Box[7].tu = 1.0f;
					pVertices_Box[7].tv = 0.0f;
					pVertices_Box[8].tu = 0.0f;
					pVertices_Box[8].tv = 1.0f;

					pVertices_Box[9].x = 1.0f;
					pVertices_Box[9].y = -1.0f;
					pVertices_Box[9].z = -1.0f; 
					pVertices_Box[10].x = -1.0f;
					pVertices_Box[10].y = 1.0f;
					pVertices_Box[10].z = -1.0f;
					pVertices_Box[11].x = 1.0f;
					pVertices_Box[11].y = 1.0f;
					pVertices_Box[11].z = -1.0f;

					pVertices_Box[9].tu = 0.0f;
					pVertices_Box[9].tv = 1.0f;
					pVertices_Box[10].tu = 1.0f;
					pVertices_Box[10].tv = 0.0f;
					pVertices_Box[11].tu = 0.0f;
					pVertices_Box[11].tv = 0.0f;


					//Left
					pVertices_Box[12].x = -1.0f;
					pVertices_Box[12].y = -1.0f;
					pVertices_Box[12].z = -1.0f;
					pVertices_Box[13].x = -1.0f;
					pVertices_Box[13].y = -1.0f;
					pVertices_Box[13].z = 1.0f; 
					pVertices_Box[14].x = -1.0f;
					pVertices_Box[14].y = 1.0f;
					pVertices_Box[14].z = -1.0f;

					pVertices_Box[12].tu = 0.0f;
					pVertices_Box[12].tv = 1.0f;
					pVertices_Box[13].tu = 1.0f;
					pVertices_Box[13].tv = 1.0f;
					pVertices_Box[14].tu = 0.0f;
					pVertices_Box[14].tv = 0.0f;

					pVertices_Box[15].x = -1.0f;
					pVertices_Box[15].y = -1.0f;
					pVertices_Box[15].z = 1.0f; 
					pVertices_Box[16].x = -1.0f;
					pVertices_Box[16].y = 1.0f;
					pVertices_Box[16].z = 1.0f;
					pVertices_Box[17].x = -1.0f;
					pVertices_Box[17].y = 1.0f;
					pVertices_Box[17].z = -1.0f;
					
					pVertices_Box[15].tu = 1.0f;
					pVertices_Box[15].tv = 1.0f;
					pVertices_Box[16].tu = 1.0f;
					pVertices_Box[16].tv = 0.0f;
					pVertices_Box[17].tu = 0.0f;
					pVertices_Box[17].tv = 0.0f;

					//Right
					pVertices_Box[18].x = 1.0f;
					pVertices_Box[18].y = -1.0f;
					pVertices_Box[18].z = -1.0f;
					pVertices_Box[19].x = 1.0f;
					pVertices_Box[19].y = 1.0f;
					pVertices_Box[19].z = -1.0f;					
					pVertices_Box[20].x = 1.0f;
					pVertices_Box[20].y = -1.0f;
					pVertices_Box[20].z = 1.0f; 


					pVertices_Box[18].tu = 1.0f;
					pVertices_Box[18].tv = 1.0f;
					pVertices_Box[19].tu = 1.0f;
					pVertices_Box[19].tv = 0.0f;
					pVertices_Box[20].tu = 0.0f;
					pVertices_Box[20].tv = 1.0f;

					pVertices_Box[21].x = 1.0f;
					pVertices_Box[21].y = -1.0f;
					pVertices_Box[21].z = 1.0f; 
					pVertices_Box[22].x = 1.0f;
					pVertices_Box[22].y = 1.0f;
					pVertices_Box[22].z = -1.0f;
					pVertices_Box[23].x = 1.0f;
					pVertices_Box[23].y = 1.0f;
					pVertices_Box[23].z = 1.0f;
					
					pVertices_Box[21].tu = 0.0f;
					pVertices_Box[21].tv = 1.0f;
					pVertices_Box[22].tu = 1.0f;
					pVertices_Box[22].tv = 0.0f;
					pVertices_Box[23].tu = 0.0f;
					pVertices_Box[23].tv = 0.0f;


					//Up
					pVertices_Box[24].x = 1.0f;
					pVertices_Box[24].y = 1.0f;
					pVertices_Box[24].z = 1.0f;
					pVertices_Box[25].x = 1.0f;
					pVertices_Box[25].y = 1.0f;
					pVertices_Box[25].z = -1.0f;
					pVertices_Box[26].x = -1.0f;
					pVertices_Box[26].y = 1.0f;
					pVertices_Box[26].z = 1.0f; 

					pVertices_Box[24].tu = 1.0f;
					pVertices_Box[24].tv = 1.0f;
					pVertices_Box[25].tu = 1.0f;
					pVertices_Box[25].tv = 0.0f;
					pVertices_Box[26].tu = 0.0f;
					pVertices_Box[26].tv = 1.0f;

					pVertices_Box[27].x = 1.0f;
					pVertices_Box[27].y = 1.0f;
					pVertices_Box[27].z = -1.0f; 
					pVertices_Box[28].x = -1.0f;
					pVertices_Box[28].y = 1.0f;
					pVertices_Box[28].z = -1.0f;
					pVertices_Box[29].x = -1.0f;
					pVertices_Box[29].y = 1.0f;
					pVertices_Box[29].z = 1.0f;

					pVertices_Box[27].tu = 1.0f;
					pVertices_Box[27].tv = 0.0f;
					pVertices_Box[28].tu = 0.0f;
					pVertices_Box[28].tv = 0.0f;
					pVertices_Box[29].tu = 0.0f;
					pVertices_Box[29].tv = 1.0f;

					//Down
					pVertices_Box[30].x = 1.0f;
					pVertices_Box[30].y = -1.0f;
					pVertices_Box[30].z = 1.0f;
					pVertices_Box[31].x = -1.0f;
					pVertices_Box[31].y = -1.0f;
					pVertices_Box[31].z = 1.0f; 
					pVertices_Box[32].x = 1.0f;
					pVertices_Box[32].y = -1.0f;
					pVertices_Box[32].z = -1.0f;

					pVertices_Box[30].tu = 1.0f;
					pVertices_Box[30].tv = 0.0f;
					pVertices_Box[31].tu = 0.0f;
					pVertices_Box[31].tv = 0.0f;					
					pVertices_Box[32].tu = 1.0f;
					pVertices_Box[32].tv = 1.0f;


					pVertices_Box[33].x = 1.0f;
					pVertices_Box[33].y = -1.0f;
					pVertices_Box[33].z = -1.0f; 
					pVertices_Box[34].x = -1.0f;
					pVertices_Box[34].y = -1.0f;
					pVertices_Box[34].z = 1.0f;
					pVertices_Box[35].x = -1.0f;
					pVertices_Box[35].y = -1.0f;
					pVertices_Box[35].z = -1.0f;

					pVertices_Box[33].tu = 1.0f;
					pVertices_Box[33].tv = 1.0f;
					pVertices_Box[34].tu = 0.0f;
					pVertices_Box[34].tv = 0.0f;
					pVertices_Box[35].tu = 0.0f;
					pVertices_Box[35].tv = 1.0f;

					//Slocca il VertexBuffer
					hr = VertexBuffer->Unlock();
					if (hr==D3D_OK)
					{
						isCreated = true;

						//Crea le texture
						textureID[0] = DETextureManager::CreateTexture(szTextureFilePath_Front, NULL, false);
						if (textureID[0] < 0)	DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Front);

						textureID[1] = DETextureManager::CreateTexture(szTextureFilePath_Back, NULL, false);
						if (textureID[1] < 0)	DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Back);

						textureID[2] = DETextureManager::CreateTexture(szTextureFilePath_Left, NULL, false);
						if (textureID[2] < 0)	DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Left);

						textureID[3] = DETextureManager::CreateTexture(szTextureFilePath_Right, NULL, false);
						if (textureID[3] < 0)	DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Right);

						textureID[4] = DETextureManager::CreateTexture(szTextureFilePath_Top, NULL, false);
						if (textureID[4] < 0)	DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Top);

						textureID[5] = DETextureManager::CreateTexture(szTextureFilePath_Bottom, NULL, false);
						if (textureID[5] < 0)	DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", logTitle, szTextureFilePath_Bottom);
					}
					else
					{
						DELog::LogError("<li>%s : VertexBuffer->Unlock(...): %s", logTitle, DXGetErrorDescription9(hr));		
					}
				}
				else
				{
					DELog::LogError("<li>%s : VertexBuffer->Lock(...): %s", logTitle, DXGetErrorDescription9(hr));		
				}
			}
			else
			{
 				DELog::LogError("<li>%s : DECore::D3DDevice->CreateVertexBuffer(...): %s", logTitle, DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>%s : DECore::D3DDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}









DLL_API void DESky::SetPosition(D3DXVECTOR3 newPosition)
{
	position = newPosition;
	D3DXMatrixTranslation(&positionMatrix, position.x, position.y, position.z);
}
DLL_API void DESky::SetRotation(D3DXVECTOR3 newRotation)
{
	rotation = newRotation;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, rotation.y, rotation.x, position.z);
}
DLL_API void DESky::SetScale(D3DXVECTOR3 newScale)
{
	scale = newScale;
	D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
}
DLL_API	void DESky::SetTrasparency(bool isTransparent, float transparentValue)
{
	//Colore di trasparenza x TextureFactor
	useTransparent = isTransparent;
	if (useTransparent)
	{
		D3DXCOLOR fade= D3DCOLOR_ARGB(255,255,255,255);
		D3DXCOLOR diff= D3DCOLOR_ARGB(0,0,0,0);
		D3DXColorLerp(&transparentColour, &fade, &diff, transparentValue);
	}
}

DLL_API	void DESky::SetColor(D3DXCOLOR newColor)
{
	float color[4] = { newColor.r, newColor.g, newColor.b, newColor.a };
	DECore::engineEffect->SetFloatArray("shadedColor", color, 4);
}

DLL_API D3DXVECTOR3	DESky::GetPosition()
{ 
	return position;
}
DLL_API D3DXVECTOR3	DESky::GetRotation()
{
	return rotation;
}
DLL_API D3DXVECTOR3	DESky::GetScale()
{
	return scale;
}
DLL_API ENUM_DE_SKYTYPE DESky::GetType()
{
	return skyType;
}

DLL_API	void DESky::SetWorldMatrix(D3DXMATRIX newMatrix)
{
	worldMatrix = newMatrix;
}

DLL_API void DESky::Render()
{
	if (isCreated)
	{

		//Setta la matrice finale
		//finalMatrix = rotationMatrix * scaleMatrix * positionMatrix;
		D3DXMatrixMultiply(&finalMatrix, &scaleMatrix, &positionMatrix);
		D3DXMatrixMultiply(&finalMatrix, &rotationMatrix, &finalMatrix);
		//DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );

		D3DXMATRIX m_matView;
		D3DXMATRIX m_matProj;
		DECore::D3DDevice->GetTransform(D3DTS_PROJECTION, &m_matProj);
		DECore::D3DDevice->GetTransform(D3DTS_VIEW, &m_matView);
		D3DXMATRIX worldViewProj = finalMatrix * m_matView * m_matProj;
		DECore::engineEffect->SetMatrix("matWorldViewProj", &worldViewProj);

		DECore::engineEffect->SetTechnique("Shaded");

		DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

		/*if (useTransparent)
		{
 				DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

			DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR,transparentColour);
			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		}*/

		
		//*** SKYCYLINDER ***
		UINT uPasses;
		if (skyType == DE_SKYTYPE_CYLINDER )
		{
			DECore::D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(CUSTOMVERTEX_SKYCYLINDER));
			DECore::D3DDevice->SetFVF(CUSTOMVERTEX_SKYCYLINDER_D3DFVF);

			//Alto
			DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[0]).Texture_Pointer);
			DECore::engineEffect->Begin( &uPasses, 0 ); 
			for( UINT uPass = 0; uPass < uPasses; ++uPass )
			{
				DECore::engineEffect->BeginPass( uPass );
				DECore::engineEffect->CommitChanges();			
				DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, ((m_dwNumOfSegments + 1) * 2), m_dwNumOfEndPolygons);
				DECore::engineEffect->EndPass();
			}
			DECore::engineEffect->End();

			//Basso
			DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[1]).Texture_Pointer);
			DECore::engineEffect->Begin( &uPasses, 0 ); 
			for( UINT uPass = 0; uPass < uPasses; ++uPass )
			{
				DECore::engineEffect->BeginPass( uPass );
				DECore::engineEffect->CommitChanges();			
				DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, ((m_dwNumOfSegments + 1) * 3) + 1, m_dwNumOfEndPolygons);
				DECore::engineEffect->EndPass();
			}
			DECore::engineEffect->End();		
		
			//Lato
			DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[2]).Texture_Pointer);
			DECore::engineEffect->Begin( &uPasses, 0 ); 
			for( UINT uPass = 0; uPass < uPasses; ++uPass )
			{
				DECore::engineEffect->BeginPass( uPass );
				DECore::engineEffect->CommitChanges();			
				DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_dwNumOfSidePolygons);
				DECore::engineEffect->EndPass();
			}
			DECore::engineEffect->End();	
		}


		//*** SKYSPHERE ***
		if (skyType == DE_SKYTYPE_SPHERE )
		{
			DECore::D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(CUSTOMVERTEX_SKYSPHERE));
			DECore::D3DDevice->SetFVF(CUSTOMVERTEX_SKYSPHERE_D3DFVF);

			DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[0]).Texture_Pointer);
			DECore::engineEffect->Begin( &uPasses, 0 ); 
			for( UINT uPass = 0; uPass < uPasses; ++uPass )
			{
				DECore::engineEffect->BeginPass( uPass );
				DECore::engineEffect->CommitChanges();			
				DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, BallVBSize - 2);
				DECore::engineEffect->EndPass();
			}
			DECore::engineEffect->End();	
		}



		//*** SKYBOX ***
		if (skyType == DE_SKYTYPE_BOX )
		{
			DECore::D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(CUSTOMVERTEX_SKYBOX));
			DECore::D3DDevice->SetFVF(CUSTOMVERTEX_SKYBOX_D3DFVF);
			for (int t=0; t<6; t++)
			{
				if (t==0)	DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[0]).Texture_Pointer);
				if (t==1)	DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[1]).Texture_Pointer);
				if (t==2)	DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[2]).Texture_Pointer);
				if (t==3)	DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[3]).Texture_Pointer);
				if (t==4)	DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[4]).Texture_Pointer);
				if (t==5)	DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(textureID[5]).Texture_Pointer);
				DECore::engineEffect->Begin( &uPasses, 0 ); 
				for( UINT uPass = 0; uPass < uPasses; ++uPass )
				{
					DECore::engineEffect->BeginPass( uPass );
					DECore::engineEffect->CommitChanges();
					DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6*t, 2);
					DECore::engineEffect->EndPass();
				}
				DECore::engineEffect->End();
			}
		}



	}
}

DLL_API bool DESky::GetCreated()
{
	return isCreated;
}

DLL_API bool DESky::Invalidate()
{
	try
	{
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESky::Restore()
{
	try
	{
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESky::Release()
{
	try
	{
		DETextureManager::ReleaseTexture(textureID[0]);
		DETextureManager::ReleaseTexture(textureID[1]);
		DETextureManager::ReleaseTexture(textureID[2]);
		DETextureManager::ReleaseTexture(textureID[4]);
		DETextureManager::ReleaseTexture(textureID[5]);
		DETextureManager::ReleaseTexture(textureID[6]);
		SafeRelease(VertexBuffer);
		isCreated = false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);
		return false;
	}	
	return false;
}

DLL_API bool DESky::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DESky::GetTag()
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