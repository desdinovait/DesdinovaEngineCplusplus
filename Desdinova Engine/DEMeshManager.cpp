#include "DEGeneralIncludes.h"

vector<STRUCT_DE_MESH>		DEMeshManager::MeshList;
int							DEMeshManager::totalMeshes=0;

DLL_API void DEMeshManager::ReleaseMesh(int meshID)
{
	if ((meshID>=0)&&(meshID<totalMeshes))
	{
		MeshList[meshID].Mesh_Pos = MeshList[meshID].Mesh_Pos - 1;
		//Rilascia completamente
		if (MeshList[meshID].Mesh_Pos == 0)
		{
			//Rimuove il Mesh
			MeshList[meshID] = GetNullMesh();
		}
	}
}

DLL_API bool DEMeshManager::ReleaseAll()
{
	try
	{
		//Rilascia tutto
		for ( int t=0; t<totalMeshes; t++)
		{
			MeshList[t] = GetNullMesh();
		}
		totalMeshes=0;
		return true;
	}
	catch(...)
	{
		return false;
	}

	return false;
}

DLL_API bool DEMeshManager::Release()
{
	DELog::LogInfo("<br><b>Release Meshes Manager:</b>");
	//Rilascia tutte le texture
    if (ReleaseAll())
	{
		//Cancella tutta la memoria occupata
		MeshList.clear();
		DELog::LogInfo("<li>The function completed successfully");
		return true;
	}
	else
	{
		DELog::LogError("<li>Unknown statement error");		
		return false;
	}

	return false;
}

DLL_API int DEMeshManager::CreateMesh(const char *szMeshFilePath, bool createTangentBinormal)
{
	int currentMesh=-1;
	try
	{
		//Passa tutti i nomi di textures già presenti
		for ( int t=0; t< totalMeshes; t++)
		{
			//La stringa non è vuota
			int result1 = strncmp( (const char *)(strdup(szMeshFilePath)), "" , strlen(szMeshFilePath) );
			if (result1 != 0)
			{
				//Confronta le stringhe
				//Viene utilizzata una copia lowercase delle 2 stringhe in modo da renderle uguali per
				//il confronto che fallirebbe se una delle due stringhe avesse una lettera minusco o maiuscola anche
				//se uguale all'altra.
				int result2 = strncmp( (const char *)strlwr(strdup(szMeshFilePath)), (const char *)strlwr(strdup(MeshList[t].Mesh_Filename)) , strlen(szMeshFilePath) );
				//Le stringhe sono uguali
				if ((result2 == 0))
				{
					//La texture esiste già per cui fa riferimento a quella trovata(t)
					MeshList[t].Mesh_Pos = MeshList[t].Mesh_Pos + 1;
					currentMesh = t;
					return currentMesh;
				}
			}
			else
			{
				return -1;
			}
		}
		//Aggiunge la nuova texture all'array
		currentMesh = AddMesh(szMeshFilePath, createTangentBinormal);
		return currentMesh;
	}
	catch(...)
	{
		return -1;
	}

	return -1;
}


DLL_API int DEMeshManager::AddMesh(const char *szMeshFilePath, bool createTangentBinormal)
{
	try
	{
		HRESULT hr;
		STRUCT_DE_MESH		newMesh;
		LPD3DXBUFFER		pAdjacencyBuffer = NULL;
		LPD3DXBUFFER		pMtrlBuffer = NULL;
		LPD3DXMESH			pMesh = NULL;
		LPD3DXMESH			pTempMesh = NULL;
		LPD3DXPMESH			pMeshProg = NULL;
		DWORD				totalParts;

		DWORD vertexProcessingUsage;
		//Guarda se è stato settato il software processing
		if (DECore::D3DDevice->GetSoftwareVertexProcessing())
		{
			//In Software
			vertexProcessingUsage = D3DXMESH_WRITEONLY | D3DXMESH_SOFTWAREPROCESSING;
		}
		else
		{
			//In hardware
			vertexProcessingUsage = D3DXMESH_WRITEONLY;
		}


		//Path corrente + nome del file fx perchè le DX vogliono tutto il percorso
		char namePath[MAX_PATH];
		strcpy(namePath, DEUtility::GetCurrentPath());
		strcat(namePath, szMeshFilePath);
		strcpy(newMesh.Mesh_Filename, szMeshFilePath);

		//** CARICAMENTO **		 
		hr = D3DXLoadMeshFromX( (LPSTR)namePath, D3DXMESH_MANAGED, DECore::D3DDevice, &pAdjacencyBuffer, &pMtrlBuffer, NULL, &totalParts, &newMesh.Mesh_Point);
		if( FAILED(hr) ) return -1;

		//** MATERIALI (cioè parti della mesh totali, il file x le divide così) **
		newMesh.Mesh_Parts = (unsigned int)totalParts;

		//** OTTIMIZZAZIONE **
		/*Clean
		hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION, newMesh.Mesh_Point, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), &pTempMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL );
		if( FAILED(hr) ) return -1;
		SafeRelease( newMesh.Mesh_Point );
		newMesh.Mesh_Point = pTempMesh;
		//Semplificazione
		//(Perform a weld to try and remove excess vertices. Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too)
		D3DXWELDEPSILONS Epsilons;
		ZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
		hr = D3DXWeldVertices( newMesh.Mesh_Point, 0, &Epsilons, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL );
		if( FAILED(hr) ) return -1;//*/

		/*if (createTangentBinormal)
		{
			//Shader declaration.
			D3DVERTEXELEMENT9 SDec[] =
			{
				{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
				{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
				{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
				{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
				D3DDECL_END()
			};
			hr = newMesh.Mesh_Point->CloneMesh(D3DXMESH_MANAGED, SDec, DECore::D3DDevice, &newMesh.Mesh_Point);
			if (hr == D3D_OK)
			{
				D3DXComputeNormals(newMesh.Mesh_Point, NULL);
				D3DXComputeTangent(newMesh.Mesh_Point, 0, 0, 0, true, NULL);
			}
		}//*/

		//Ottimizazzione InPlace
		newMesh.Mesh_Point->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), 0, 0, 0);
		if( FAILED(hr) ) return -1;//*/

		/*Verifica di validità
		hr = D3DXValidMesh( newMesh.Mesh_Point, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL );
		if( FAILED(hr) ) return -1;*/
		
		//** BOUNDINGS **
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
		void* pVertices;
		hr = newMesh.Mesh_Point->GetVertexBuffer( &pVertexBuffer );
		if( FAILED(hr) ) return -1;
		hr = pVertexBuffer->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
		if( FAILED(hr) ) return -1;
		//Box
		D3DXVECTOR3 pMinMesh;
		D3DXVECTOR3 pMaxMesh;
		hr = D3DXComputeBoundingBox((D3DXVECTOR3 *)pVertices, newMesh.Mesh_Point->GetNumVertices(), newMesh.Mesh_Point->GetNumBytesPerVertex(), &pMinMesh, &pMaxMesh);
		if( FAILED(hr) ) return -1;
		//Sphere
		D3DXVECTOR3 pCenterMesh;
		float radiusMesh;
		hr = D3DXComputeBoundingSphere((D3DXVECTOR3 *)pVertices, newMesh.Mesh_Point->GetNumVertices(), newMesh.Mesh_Point->GetNumBytesPerVertex(), &pCenterMesh, &radiusMesh);
		if( FAILED(hr) ) return -1;
		//Rilascio
		pVertexBuffer->Unlock();
		SafeRelease( pVertexBuffer );
		//Raggio
		newMesh.Mesh_radius = radiusMesh;
		//Pivot
		newMesh.Mesh_pivot.x = abs(pMinMesh.x);
		newMesh.Mesh_pivot.y = abs(pMinMesh.y);
		newMesh.Mesh_pivot.z = abs(pMinMesh.z);
		//Dim
		newMesh.Mesh_minCoord = pMinMesh;
		newMesh.Mesh_maxCoord = pMaxMesh;
		//Box
		newMesh.Mesh_width = pMaxMesh.x - pMinMesh.x;
		newMesh.Mesh_height = pMaxMesh.y - pMinMesh.y;
		newMesh.Mesh_depth = pMaxMesh.z - pMinMesh.z;
		//Center
		newMesh.Mesh_centerCoord.x=(pMaxMesh.x + pMinMesh.x) /2;
		newMesh.Mesh_centerCoord.y=(pMaxMesh.y + pMinMesh.y) /2;
		newMesh.Mesh_centerCoord.z=(pMaxMesh.z + pMinMesh.z) /2;

		/** NORMALI **
		DWORD dw32BitFlag = ( newMesh.Mesh_Point->GetOptions() & D3DXMESH_32BIT );
		//Se non ha le normali le crea
		if ( !( newMesh.Mesh_Point->GetFVF() & D3DFVF_NORMAL ) )
		{
			hr = newMesh.Mesh_Point->CloneMeshFVF( dw32BitFlag | D3DXMESH_MANAGED, newMesh.Mesh_Point->GetFVF() | D3DFVF_NORMAL, DECore::D3DDevice, &pTempMesh );
			if( FAILED(hr) ) return -1;

			hr = D3DXComputeNormals( pTempMesh, NULL );
			if( FAILED(hr) ) return -1;

			SafeRelease( newMesh.Mesh_Point );
			newMesh.Mesh_Point = pTempMesh;
		}//*/

        /*Copy declaration into mesh model.
		D3DVERTEXELEMENT9 SDec[] =
		{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		D3DDECL_END()
		};
		hr = newMesh.Mesh_Point->CloneMesh(D3DXMESH_MANAGED, SDec, DECore::D3DDevice, &newMesh.Mesh_Point);
		if( FAILED(hr) ) return -1;

		// Calculate all the normals of the model.
		//hr = D3DXComputeNormals(newMesh.Mesh_Point, NULL);
		//if( FAILED(hr) ) return -1;

		// Calculate the tangents and binormals.
		hr = D3DXComputeTangent(newMesh.Mesh_Point, 0, 0, 0, true, NULL);
		if( FAILED(hr) ) return -1;


		//hr = D3DXComputeTangent(pMesh, 0, 0, 0, true, NULL);
		//if( FAILED(hr) ) return -1;*/

		/** PROGRESSIVE MESH **
		//Genera la mesh progressiva
		//newMesh.Mesh_Point =  NULL;
		hr = D3DXGeneratePMesh( newMesh.Mesh_Point, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &pMeshProg ); 
		if( FAILED(hr) ) return -1;

		//Clone full size pmesh
		hr = pMeshProg->ClonePMeshFVF( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE, pMeshProg->GetFVF(), DECore::D3DDevice, &newMesh.Mesh_Progressive);
		if( FAILED(hr) ) return -1;

		//Setta il numero attuale(iniziale) dei vertici come quelli totali
		hr = newMesh.Mesh_Progressive->SetNumVertices( newMesh.Mesh_Progressive->GetMaxVertices() );
		if( FAILED(hr) ) return -1;//*/

		//newMesh.Mesh_Progressive = pMeshProg;

		SafeRelease( pAdjacencyBuffer );
		SafeRelease( pMtrlBuffer );
		SafeRelease( pMesh );
		SafeRelease( pMeshProg );
		

		//** FINE **
		//Aggiunge la Mesh correttamente caricato
		newMesh.Mesh_Pos = 1;
		int currentMeshID = PlaceMesh(newMesh);
		return currentMeshID;
	}
	catch(...)
	{
		return -1;
	}
	return -1;
}


DLL_API int DEMeshManager::PlaceMesh(STRUCT_DE_MESH newMesh)
{
	//Se trova un posto vuoto mette il Meshlo li dentro
	for ( int m=0; m<totalMeshes; m++)
	{
		if (MeshList[m].Mesh_Pos==0)
		{
			MeshList[m] = newMesh;
			return m;
		}
	}

	//Setta il nuovo elemento aggiunto
	MeshList.push_back(newMesh);

	//Aggiunto uno alla lista
	totalMeshes = totalMeshes + 1;

	return totalMeshes-1;
}


DLL_API int DEMeshManager::GetLoadedMeshes()
{
	return totalMeshes;
}

DLL_API int DEMeshManager::GetSearchedMeshes()
{
	int searchMeshes=0;
	for (int t=0; t<DEMeshManager::GetLoadedMeshes(); t++)
	{
		searchMeshes = searchMeshes + MeshList[t].Mesh_Pos;
	}
	return searchMeshes;
}

DLL_API STRUCT_DE_MESH DEMeshManager::GetMesh(int meshID)
{
	if (meshID >= totalMeshes)	meshID = totalMeshes;
	if (meshID < 0)
	{
		STRUCT_DE_MESH nullMesh;
		nullMesh = GetNullMesh();
		return nullMesh;	
	}
	return MeshList[meshID];
}

DLL_API STRUCT_DE_MESH DEMeshManager::GetNullMesh()
{
	STRUCT_DE_MESH nullMesh;
	nullMesh.Mesh_Point = NULL;
	nullMesh.Mesh_Progressive = NULL;
	strcpy(nullMesh.Mesh_Filename, "No filename");
	nullMesh.Mesh_Pos = 0;
	nullMesh.Mesh_Parts = 0;
	nullMesh.Mesh_maxCoord.x = -1000000;
	nullMesh.Mesh_maxCoord.y = -1000000;
	nullMesh.Mesh_maxCoord.z = -1000000;
	nullMesh.Mesh_minCoord.x = 1000000;
	nullMesh.Mesh_minCoord.y = 1000000;
	nullMesh.Mesh_minCoord.z = 1000000;
	nullMesh.Mesh_width  = 0.0f;
	nullMesh.Mesh_height = 0.0f;
	nullMesh.Mesh_depth  = 0.0f;
	nullMesh.Mesh_radius = 0.0f;
	nullMesh.Mesh_centerCoord.x = 0.0f;
	nullMesh.Mesh_centerCoord.y = 0.0f;
	nullMesh.Mesh_centerCoord.z = 0.0f;

	return nullMesh;	
}


DLL_API void DEMeshManager::LogReport()
{
	int MeshBlock=0;
	DELog::LogInfo("<br><b>Allocated Meshs Report:</b>");
	for (int t=0; t<DEMeshManager::GetLoadedMeshes(); t++)
	{
		LPD3DXPMESH tempMesh;
		tempMesh = MeshList[t].Mesh_Progressive;
		if (tempMesh != NULL)
		{
			MeshBlock=MeshBlock +1;
			DELog::LogInfo("<li>Mesh[%.0f]: %s - Shared: %.0f", (float)t, MeshList[t].Mesh_Filename, (float)MeshList[t].Mesh_Pos);
		}
		else
		{
			DELog::LogInfo("<li>Mesh[%.0f]: %s - Temporary empty block", (float)t, MeshList[t].Mesh_Filename);
		}
	}
	DELog::LogInfo("<li>Meshes Loaded: %.0f", (float)DEMeshManager::GetLoadedMeshes());
	DELog::LogInfo("<li>Meshes Block Occupated: %.0f", (float)MeshBlock);
	DELog::LogInfo("<li>Meshes Block Empty: %.0f", (float)((float)DEMeshManager::GetLoadedMeshes() - (float)MeshBlock));
	
}



DLL_API	bool DEMeshManager::Invalidate()
{
	//Rilascia tutte le mesh
	for ( int t=0; t<totalMeshes; t++)
	{
		if (MeshList[t].Mesh_Pos != 0)
		{
		}
	}
	return true;
}


DLL_API	bool DEMeshManager::Restore()
{
	for ( int t=0; t<totalMeshes; t++)
	{
		if (MeshList[t].Mesh_Pos != 0)
		{
		}
		else
		{
			continue;
		}
	}
	return true;
}