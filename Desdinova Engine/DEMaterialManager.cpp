#include "DEGeneralIncludes.h"

vector<STRUCT_DE_MATERIAL>	DEMaterialManager::MaterialList;
int							DEMaterialManager::totalMaterials=0;

DLL_API void DEMaterialManager::ReleaseMaterial(int materialID)
{
	if ((materialID>=0)&&(materialID<totalMaterials))
	{
		MaterialList[materialID].Material_Pos = MaterialList[materialID].Material_Pos - 1;
		//Rilascia completamente la Material solo se non è più usata
		if (MaterialList[materialID].Material_Pos==0)
		{
			//Rilascia il materiale
			MaterialList[materialID] = GetNullMaterial();
		}
	}
}

DLL_API bool DEMaterialManager::ReleaseAll()
{
	try
	{
		//Rilascia tutte le Material incondizionatamente
		for ( int t=0; t<totalMaterials; t++)
		{
			MaterialList[t] = GetNullMaterial();
		}
		totalMaterials=0;
		MaterialList.clear();
		return true;
	}
	catch(...)
	{
		return false;
	}

	return false;

}

DLL_API bool DEMaterialManager::Release()
{
	DELog::LogInfo("<br><b>Release Materials Manager:</b>");
	//Rilascia tutte gli effetti
    if (ReleaseAll())
	{
		//Cancella tutta la memoria occupata
		MaterialList.clear();
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

DLL_API int DEMaterialManager::CreateMaterial(const char *szMaterialFilePath)
{
	int currentMaterial=-1;

	try
	{
		//Passa tutti i nomi di Materials già presenti
		for ( int t=0; t< totalMaterials; t++)
		{
			//Confronta le stringhe
			//Viene utilizzata una copia lowercase delle 2 stringhe in modo da renderle uguali per
			//il confronto che fallirebbe se una delle deu stringhe avesse una lettera minusco o maiuscola anche
			//se uguale all'altra.
			int result = strncmp( (const char *)strlwr(strdup(szMaterialFilePath)), (const char *)strlwr(strdup(MaterialList[t].Material_Filename)) , strlen(szMaterialFilePath) );
			//Le stringhe sono uguali
			if (result == 0)
			{
				//La Material esiste già per cui fa riferimento a quella trovata(t)
				MaterialList[t].Material_Pos = MaterialList[t].Material_Pos + 1;
				currentMaterial = t;
				return currentMaterial;
			}
		}
		//Aggiunge la nuova Material all'array
		currentMaterial = AddMaterial(szMaterialFilePath);
		return currentMaterial;

	}
	catch(...)
	{
		return -1;
	}

	return -1;
}


DLL_API int DEMaterialManager::AddMaterial(const char *szMaterialFilePath)
{
	try
	{
		HRESULT hr;
		LPD3DXEFFECT tempMaterial = NULL;
		ID3DXBuffer* pd3dErrors = NULL;

		char namePath[MAX_PATH];
		strcpy(namePath, DEUtility::GetCurrentPath());
		strcat(namePath, szMaterialFilePath);

		hr = D3DXCreateEffectFromFile(DECore::D3DDevice, namePath, NULL,NULL,D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,NULL,&tempMaterial, &pd3dErrors );
		if (hr==D3D_OK)
		{
			//** INFO **
			//Materiale
			STRUCT_DE_MATERIAL newMaterial;
			strcpy (newMaterial.Material_Filename, strdup(szMaterialFilePath));
			newMaterial.Material_Pos = 1;
			//Nome materiale
			LPCSTR materialName;
			hr = tempMaterial->GetString("Material_Name", &materialName);
			if (hr!=D3D_OK)	strcpy(newMaterial.Material_Name, "No Name");
			else			strcpy(newMaterial.Material_Name, materialName);

			//** ANIMAZIONE **
			//Density
			FLOAT materialDensity;
			hr = tempMaterial->GetFloat("Material_Density", &materialDensity);
			if (hr!=D3D_OK)	newMaterial.Material_Density = 1000.0f; //Water
			else			newMaterial.Material_Density = materialDensity;
			//Restitution
			FLOAT materialRestitution;
			hr = tempMaterial->GetFloat("Material_Restitution", &materialRestitution);
			if (hr!=D3D_OK)	newMaterial.Material_Restitution = 1.0f;
			else			newMaterial.Material_Restitution = materialRestitution;
			//StaticFriction
			FLOAT materialStaticFriction;
			hr = tempMaterial->GetFloat("Material_StaticFriction", &materialStaticFriction);
			if (hr!=D3D_OK)	newMaterial.Material_StaticFriction = 0.0f;
			else			newMaterial.Material_StaticFriction = materialStaticFriction;
			//DynamicFriction
			FLOAT materialDynamicFriction;
			hr = tempMaterial->GetFloat("Material_DynamicFriction", &materialDynamicFriction);
			if (hr!=D3D_OK)	newMaterial.Material_DynamicFriction = 0.0f;
			else			newMaterial.Material_DynamicFriction = materialDynamicFriction;
			//IsAnisotropic
			BOOL materialIsAnisotropic;
			hr = tempMaterial->GetBool("Material_IsAnisotropic", &materialIsAnisotropic);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_IsAnisotropic = false;
			}
			else
			{
				if (materialIsAnisotropic == 0)	newMaterial.Material_IsAnisotropic = false;
				else 							newMaterial.Material_IsAnisotropic = true;
			}	
			//DynamicFrictionV
			FLOAT materialDynamicFrictionV;
			hr = tempMaterial->GetFloat("Material_DynamicFrictionV", &materialDynamicFrictionV);
			if (hr!=D3D_OK)	newMaterial.Material_DynamicFrictionV = 0.8f;
			else			newMaterial.Material_DynamicFrictionV = materialDynamicFrictionV;
			//StaticFrictionV
			FLOAT materialStaticFrictionV;
			hr = tempMaterial->GetFloat("Material_StaticFrictionV", &materialStaticFrictionV);
			if (hr!=D3D_OK)	newMaterial.Material_StaticFrictionV = 1.0f;
			else			newMaterial.Material_StaticFrictionV = materialStaticFrictionV;
			//DirOfAnisotropy
			FLOAT *materialDirOfAnysotropy = new FLOAT[3];
			hr = tempMaterial->GetValue("Material_DirOfAnisotropy", materialDirOfAnysotropy, 3*sizeof(float));
			if (hr==D3D_OK)
			{
				newMaterial.Material_DirOfAnisotropy.x = materialDirOfAnysotropy[0];
				newMaterial.Material_DirOfAnisotropy.y = materialDirOfAnysotropy[1];
				newMaterial.Material_DirOfAnisotropy.z = materialDirOfAnysotropy[2];
			}
			else
			{
				newMaterial.Material_DirOfAnisotropy.x = 0.0f;
				newMaterial.Material_DirOfAnisotropy.y = 0.0f;
				newMaterial.Material_DirOfAnisotropy.z = 1.0f;
			}
			delete materialDirOfAnysotropy;
			//Ambient
			FLOAT *materialAmbient = new FLOAT[4];
			hr = tempMaterial->GetValue("Material_Ambient", materialAmbient, 4*sizeof(float));
			if (hr==D3D_OK)
			{			
				newMaterial.Material_Ambient.r = materialAmbient[0];
				newMaterial.Material_Ambient.g = materialAmbient[1];
				newMaterial.Material_Ambient.b = materialAmbient[2];
				newMaterial.Material_Ambient.a = materialAmbient[3];
			}
			else
			{
				newMaterial.Material_Ambient.r = 1.0f;
				newMaterial.Material_Ambient.g = 1.0f;
				newMaterial.Material_Ambient.b = 1.0f;
				newMaterial.Material_Ambient.a = 1.0f;
			}
			delete materialAmbient;
			//Diffuse
			FLOAT *materialDiffuse = new FLOAT[4];
			hr = tempMaterial->GetValue("Material_Diffuse", materialDiffuse, 4*sizeof(float));
			if (hr==D3D_OK)
			{			
				newMaterial.Material_Diffuse.r = materialDiffuse[0];
				newMaterial.Material_Diffuse.g = materialDiffuse[1];
				newMaterial.Material_Diffuse.b = materialDiffuse[2];
				newMaterial.Material_Diffuse.a = materialDiffuse[3];
			}
			else
			{
				newMaterial.Material_Diffuse.r = 1.0f;
				newMaterial.Material_Diffuse.g = 1.0f;
				newMaterial.Material_Diffuse.b = 1.0f;
				newMaterial.Material_Diffuse.a = 1.0f;
			}
			delete materialDiffuse;
			//Shininess
			FLOAT materialShininess;
			hr = tempMaterial->GetFloat("Material_Shininess", &materialShininess);
			if (hr!=D3D_OK)	newMaterial.Material_Shininess = 100.0f;
			else			newMaterial.Material_Shininess = materialShininess;

			//Alpha
			FLOAT materialAlpha;
			hr = tempMaterial->GetFloat("Material_Alpha", &materialAlpha);
			if (hr!=D3D_OK)	newMaterial.Material_Alpha = 0.5f;
			else			newMaterial.Material_Alpha = materialAlpha;

			//UseDirectionalLights
			BOOL materialUseDirectionalLights;
			hr = tempMaterial->GetBool("Material_UseDirectionalLights", &materialUseDirectionalLights);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_UseDirectionalLights = false;
			}
			else
			{
				if (materialUseDirectionalLights == 0)	newMaterial.Material_UseDirectionalLights = false;
				else 									newMaterial.Material_UseDirectionalLights = true;
			}	
			//UsePointLights
			BOOL materialUsePointLights;
			hr = tempMaterial->GetBool("Material_UsePointLights", &materialUsePointLights);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_UsePointLights = false;
			}
			else
			{
				if (materialUsePointLights == 0)	newMaterial.Material_UsePointLights = false;
				else 								newMaterial.Material_UsePointLights = true;
			}
			//IsSpecular
			BOOL materialIsSpecular;
			hr = tempMaterial->GetBool("Material_IsSpecular", &materialIsSpecular);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_IsSpecular = false;
			}
			else
			{
				if (materialIsSpecular == 0)	newMaterial.Material_IsSpecular = false;
				else 							newMaterial.Material_IsSpecular = true;
			}	
			//IsBumpmapped
			BOOL materialIsBumpmapped;
			hr = tempMaterial->GetBool("Material_IsBumpmapped", &materialIsBumpmapped);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_IsBumpmapped = false;
			}
			else
			{
				if (materialIsBumpmapped == 0)	newMaterial.Material_IsBumpmapped = false;
				else 							newMaterial.Material_IsBumpmapped = true;
			}	
			//IsTransparent
			BOOL materialIsTransparent;
			hr = tempMaterial->GetBool("Material_IsTransparent", &materialIsTransparent);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_IsTransparent = false;
			}
			else
			{
				if (materialIsTransparent == 0)	newMaterial.Material_IsTransparent = false;
				else 							newMaterial.Material_IsTransparent = true;
			}	
			//IsTransparent
			BOOL materialIsEnvironmental;
			hr = tempMaterial->GetBool("Material_IsEnvironmental", &materialIsEnvironmental);
			if (hr!=D3D_OK)
			{
				newMaterial.Material_IsEnvironmental = false;
			}
			else
			{
				if (materialIsEnvironmental == 0)	newMaterial.Material_IsEnvironmental = false;
				else 								newMaterial.Material_IsEnvironmental = true;
			}	

			
			//Il numero ID del materiale fisico è identico a quello dell'effetto
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//newMaterial.Material_Physics_MaterialID = DECore::NXDevice->addMaterial(newMaterial.Material_Physics_MaterialProperties);
			//Imposta il materiale
			int currentMaterialID = PlaceMaterial(newMaterial);
			//Il Material_Physics_MaterialID e il currentMaterialID non devono necessariamente essere uguali
			//Probabilmente il prima è uguale al secondo +1 perchè il primo inizia il conto da 1 mentre
			//il secondo inizia il conto da 0.

			return currentMaterialID;			
		}
		else
		{
			DELog::LogError("<li>D3DXCreateEffectFromFile( %s ): %s", szMaterialFilePath, (const char*)pd3dErrors->GetBufferPointer());
			pd3dErrors->Release();
			return -1;
		}
	}
	catch(...)
	{
		return -1;
	}

	return -1;
}

DLL_API int DEMaterialManager::PlaceMaterial(STRUCT_DE_MATERIAL newMaterial)
{
	//Se trova un posto vuoto mette la Material li dentro
	for ( int t=0; t< totalMaterials; t++)
	{
		if (MaterialList[t].Material_Pos==0)
		{
			MaterialList[t] = newMaterial;
			return t;
		}
	}

	//Setta il nuovo elemento aggiunto
	MaterialList.push_back(newMaterial);

	//Aggiunto uno alla lista
	totalMaterials = totalMaterials + 1;

	return totalMaterials-1;
}

DLL_API int DEMaterialManager::GetLoadedMaterials()
{
	return totalMaterials;
}

DLL_API int DEMaterialManager::GetSearchedMaterials()
{
	int searchMaterials=0;
	for (int t=0; t<DEMaterialManager::GetLoadedMaterials(); t++)
	{
		searchMaterials = searchMaterials + MaterialList[t].Material_Pos;
	}
	return searchMaterials;
}


DLL_API STRUCT_DE_MATERIAL DEMaterialManager::GetMaterial(int materialID)
{
	if (materialID >= totalMaterials)	materialID = totalMaterials;
	if (materialID < 0)
	{
		STRUCT_DE_MATERIAL nullMaterial;
		nullMaterial = GetNullMaterial();
		return nullMaterial;	
	}
	return MaterialList[materialID];
}

DLL_API STRUCT_DE_MATERIAL DEMaterialManager::GetNullMaterial()
{
	STRUCT_DE_MATERIAL nullMaterial;
	strcpy(nullMaterial.Material_Filename, "No filename");
	strcpy(nullMaterial.Material_Name, "No Name");
	nullMaterial.Material_Pos = 0;
	//...
	return nullMaterial;	
}


DLL_API void DEMaterialManager::LogReport()
{
	int MaterialBlock = 0;
	DELog::LogInfo("<br><b>Allocated Materials Report:</b>");
	for (int t=0; t<DEMaterialManager::GetLoadedMaterials(); t++)
	{
		//LPD3DXEFFECT tempMaterial;
		//tempMaterial = MaterialList[t].Material_Pointer;
		//if (tempMaterial != NULL)
		{
			MaterialBlock = MaterialBlock + 1;
			DELog::LogInfo("<li>Material[%.0f]: %s - Shared: %.0f", (float)t, MaterialList[t].Material_Filename, (float)MaterialList[t].Material_Pos);
		}
		//else
		{
			//DELog::LogInfo("<li>Material[%.0f]: %s - Temporary empty block", (float)t, MaterialList[t].Material_Filename);
		}

	}
	DELog::LogInfo("<li>Materials Loaded: %.0f", (float)DEMaterialManager::GetLoadedMaterials());
	DELog::LogInfo("<li>Materials Block Occupated: %.0f", (float)MaterialBlock);
	DELog::LogInfo("<li>Materials Block Empty: %.0f", (float)((float)DEMaterialManager::GetLoadedMaterials() - (float)MaterialBlock));	
}