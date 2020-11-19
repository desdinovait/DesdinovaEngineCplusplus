#include "DEGeneralIncludes.h"

vector<STRUCT_DE_MODELPROPERTIES>	DEModelManager::ModelList;
int									DEModelManager::totalModels=0;

DLL_API void DEModelManager::ReleaseModel(int modelID)
{
	if ((modelID>=0)&&(modelID<totalModels))
	{
		ModelList[modelID].Model_Pos = ModelList[modelID].Model_Pos - 1;
		//Rilascia completamente la Model solo se non è più usata
		if (ModelList[modelID].Model_Pos==0)
		{
			//Rilascia il materiale
			ModelList[modelID] = GetNullModel();
		}
	}
}

DLL_API bool DEModelManager::ReleaseAll()
{
	try
	{
		//Rilascia tutte le Model incondizionatamente
		for ( int t=0; t<totalModels; t++)
		{
			ModelList[t] = GetNullModel();
		}
		totalModels=0;
		ModelList.clear();
		return true;
	}
	catch(...)
	{
		return false;
	}

	return false;

}

DLL_API bool DEModelManager::Release()
{
	DELog::LogInfo("<br><b>Release Models Manager:</b>");
	//Rilascia tutte gli effetti
    if (ReleaseAll())
	{
		//Cancella tutta la memoria occupata
		ModelList.clear();
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

DLL_API int DEModelManager::CreateModel(const char *szModelFilePath)
{
	int currentModel=-1;

	try
	{
		//Passa tutti i nomi di Models già presenti
		for ( int t=0; t< totalModels; t++)
		{
			//Confronta le stringhe
			//Viene utilizzata una copia lowercase delle 2 stringhe in modo da renderle uguali per
			//il confronto che fallirebbe se una delle deu stringhe avesse una lettera minusco o maiuscola anche
			//se uguale all'altra.
			int result = strncmp( (const char *)strlwr(strdup(szModelFilePath)), (const char *)strlwr(strdup(ModelList[t].Model_Filename)) , strlen(szModelFilePath) );
			//Le stringhe sono uguali
			if (result == 0)
			{
				//La Model esiste già per cui fa riferimento a quella trovata(t)
				ModelList[t].Model_Pos = ModelList[t].Model_Pos + 1;
				currentModel = t;
				return currentModel;
			}
		}
		//Aggiunge la nuova Model all'array
		currentModel = AddModel(szModelFilePath);
		return currentModel;

	}
	catch(...)
	{
		return -1;
	}

	return -1;
}


DLL_API int DEModelManager::AddModel(const char *szModelFilePath)
{
	try
	{
		STRUCT_DE_MODELPROPERTIES newModel;

		HRESULT hr;
		LPD3DXEFFECT tempMaterial = NULL;
		LPD3DXBUFFER pBufferErrors = NULL;
		char namePath[MAX_PATH];
		strcpy(namePath, DEUtility::GetCurrentPath());
		strcat(namePath, szModelFilePath);
		strcpy(newModel.Model_Filename, szModelFilePath);

		hr = D3DXCreateEffectFromFile(DECore::D3DDevice, namePath, NULL,NULL,D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,NULL,&tempMaterial, &pBufferErrors );
		if (hr==D3D_OK)
		{
			newModel.Model_Pos = 1;
			//Mesh
			LPCSTR modelMesh;
			hr = tempMaterial->GetString("Model_File_Mesh", &modelMesh);
			if (hr!=D3D_OK)
			{
				//Il file della mesh non è specificato
				strcpy(newModel.Model_File_Mesh, "No Name");
				return false;
			}
			else
			{
				//Il file della mesh è specificato quindi carica tutto il resto
				strcpy(newModel.Model_File_Mesh, modelMesh);

				//** PROPERTIES **
				//Nome
				LPCSTR modelName;
				hr = tempMaterial->GetString("Model_Name", &modelName);
				if (hr!=D3D_OK)	strcpy(newModel.Model_Name, "No Name");
				else			strcpy(newModel.Model_Name, modelName);
				//Pickable
				BOOL modelPickable;
				hr = tempMaterial->GetBool("Model_Pickable", &modelPickable);
				if (hr!=D3D_OK)
				{
					newModel.Model_IsPickable = false;
				}
				else
				{
					if (modelPickable == 0)	newModel.Model_IsPickable = false;
					else 					newModel.Model_IsPickable = true;
				}
				//ScreenAligned
				BOOL modelScreenAligned;
				hr = tempMaterial->GetBool("Model_ScreenAligned", &modelScreenAligned);
				if (hr!=D3D_OK)
				{
					newModel.Model_IsScreenAligned = false;
				}
				else
				{
					if (modelScreenAligned == 0)	newModel.Model_IsScreenAligned = false;
					else 							newModel.Model_IsScreenAligned = true;
				}
				//Cullable
				BOOL modelCullable;
				hr = tempMaterial->GetBool("Model_Cullable", &modelCullable);
				if (hr!=D3D_OK)
				{
					newModel.Model_IsCullable = false;
				}
				else
				{
					if (modelCullable == 0)	newModel.Model_IsCullable = false;
					else 					newModel.Model_IsCullable = true;
				}				
				//Trascurable
				BOOL modelTrascurable;
				hr = tempMaterial->GetBool("Model_Trascurable", &modelTrascurable);
				if (hr!=D3D_OK)
				{
					newModel.Model_IsTrascurable = false;
				}
				else
				{
					if (modelTrascurable == 0)	newModel.Model_IsTrascurable = false;
					else 						newModel.Model_IsTrascurable = true;
				}
				//Scalatura
				float *modelScale;
				modelScale = new float[3];
				hr = tempMaterial->GetFloatArray("Model_Scale", modelScale, 3);
				if (hr!=D3D_OK)	newModel.Model_Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				else			newModel.Model_Scale = D3DXVECTOR3(modelScale[0], modelScale[1], modelScale[2]);				

				//** FILES **
				//Shader
				LPCSTR modelShader;
				hr = tempMaterial->GetString("Model_File_Shader", &modelShader);
				if (hr!=D3D_OK)	strcpy(newModel.Model_File_Shader, "No Name");
				else			strcpy(newModel.Model_File_Shader, modelShader);
				//Material
				LPCSTR modelMaterial;
				hr = tempMaterial->GetString("Model_File_Material", &modelMaterial);
				if (hr!=D3D_OK)	strcpy(newModel.Model_File_Material, "No Name");
				else			strcpy(newModel.Model_File_Material, modelMaterial);

				//***DEBUG **
				INT modelFillMode;
				hr = tempMaterial->GetInt("Model_Debug_FillMode", &modelFillMode);
				if (hr!=D3D_OK)	newModel.Model_Debug_FillMode = 0;
				else			newModel.Model_Debug_FillMode = modelFillMode;
				INT modelBoundingType;
				hr = tempMaterial->GetInt("Model_Debug_BoundingType", &modelBoundingType);
				if (hr!=D3D_OK)	newModel.Model_Debug_BoundingType = 0;
				else			newModel.Model_Debug_BoundingType = modelBoundingType;
				INT modelShowAxes;
				hr = tempMaterial->GetInt("Model_Debug_ShowAxes", &modelShowAxes);
				if (hr!=D3D_OK)	newModel.Model_Debug_ShowAxes = 0;
				else			newModel.Model_Debug_ShowAxes = modelShowAxes;

				//** TEXTURES **
				//Texture 0
				LPCSTR modelTexture0;
				hr = tempMaterial->GetString("Model_Texture0_Filename", &modelTexture0);
				if (hr!=D3D_OK)
				{
					strcpy(newModel.Model_Texture_Filename0, "");
					newModel.Model_Texture_ColorKey0 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
					newModel.Model_Texture_MipMaps0 = false;
				}
				else
				{
					//Esiste il nome della texture quindi carica ANCHE il resto
					strcpy(newModel.Model_Texture_Filename0, modelTexture0);
					//Texture 0 RGBA
					float *modelTextureRGBA0;
					modelTextureRGBA0 = new float[4];
					hr = tempMaterial->GetFloatArray("Model_Texture0_RGBA", modelTextureRGBA0, 4);
					if (hr!=D3D_OK)	newModel.Model_Texture_ColorKey0 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
					else			newModel.Model_Texture_ColorKey0 = D3DXCOLOR(modelTextureRGBA0[0], modelTextureRGBA0[1], modelTextureRGBA0[2], modelTextureRGBA0[3]);
					delete modelTextureRGBA0;
					//MipMaps 0
					BOOL modelMipmaps0;
					hr = tempMaterial->GetBool("Model_Texture0_MipMaps", &modelMipmaps0);
					if (hr!=D3D_OK)
					{
						newModel.Model_Texture_MipMaps0 = false;
					}
					else
					{
						if (modelMipmaps0 == 0)	newModel.Model_Texture_MipMaps0 = false;
						else 					newModel.Model_Texture_MipMaps0 = true;
					}	
				}

				//Texture 1
				LPCSTR modelTexture1;
				hr = tempMaterial->GetString("Model_Texture1_Filename", &modelTexture1);
				if (hr!=D3D_OK)
				{
					strcpy(newModel.Model_Texture_Filename1, "");
					newModel.Model_Texture_ColorKey1 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
					newModel.Model_Texture_MipMaps1 = false;
				}
				else
				{
					strcpy(newModel.Model_Texture_Filename1, modelTexture1);
					//Texture 1 RGBA
					float *modelTextureRGBA1;
					modelTextureRGBA1 = new float[4];
					hr = tempMaterial->GetFloatArray("Model_Texture1_RGBA", modelTextureRGBA1, 4);
					if (hr!=D3D_OK)	newModel.Model_Texture_ColorKey1 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
					else			newModel.Model_Texture_ColorKey1 = D3DXCOLOR(modelTextureRGBA1[0], modelTextureRGBA1[1], modelTextureRGBA1[2], modelTextureRGBA1[3]);
					delete modelTextureRGBA1;
					//MipMaps 1
					BOOL modelMipmaps1;
					hr = tempMaterial->GetBool("Model_Texture1_MipMaps", &modelMipmaps1);
					if (hr!=D3D_OK)
					{
						newModel.Model_Texture_MipMaps1 = false;
					}
					else
					{
						if (modelMipmaps1 == 0)	newModel.Model_Texture_MipMaps1 = false;
						else 					newModel.Model_Texture_MipMaps1 = true;
					}	
				}

				//Texture 2
				LPCSTR modelTexture2;
				hr = tempMaterial->GetString("Model_TextureNormals_Filename", &modelTexture2);
				if (hr!=D3D_OK)
				{
					strcpy(newModel.Model_Texture_Filename2, "");
					newModel.Model_Texture_ColorKey2 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
					newModel.Model_Texture_MipMaps2 = false;
				}
				else
				{
					strcpy(newModel.Model_Texture_Filename2, modelTexture2);
					//Texture 2 RGBA
					float *modelTextureRGBA2;
					modelTextureRGBA2 = new float[4];
					hr = tempMaterial->GetFloatArray("Model_TextureNormals_RGBA", modelTextureRGBA2, 4);
					if (hr!=D3D_OK)	newModel.Model_Texture_ColorKey2 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
					else			newModel.Model_Texture_ColorKey2 = D3DXCOLOR(modelTextureRGBA2[0], modelTextureRGBA2[1], modelTextureRGBA2[2], modelTextureRGBA2[3]);
					delete modelTextureRGBA2;
					//MipMaps 2
					BOOL modelMipmaps2;
					hr = tempMaterial->GetBool("Model_TextureNormals_MipMaps", &modelMipmaps2);
					if (hr!=D3D_OK)
					{
						newModel.Model_Texture_MipMaps2 = false;
					}
					else
					{
						if (modelMipmaps2 == 0)	newModel.Model_Texture_MipMaps2 = false;
						else 					newModel.Model_Texture_MipMaps2 = true;
					}	
				}

				//Restitution
				FLOAT modelBlendfactor = 0.5f;
				hr = tempMaterial->GetFloat("Model_Texture_BlendFactor", &modelBlendfactor);
				if (hr!=D3D_OK)	newModel.Model_Texture_BlendFactor = 0.5f;
				else			newModel.Model_Texture_BlendFactor = modelBlendfactor;


				//** PHYSICS **
				//Restitution
				FLOAT modelPhysicsMass = 0.0f;
				hr = tempMaterial->GetFloat("Model_Physics_Mass", &modelPhysicsMass);
				if (hr!=D3D_OK)	newModel.Model_Physics_Mass = 0.5f;
				else			newModel.Model_Physics_Mass = modelPhysicsMass;
				//Restitution
				INT modelPhysicsClass = 0;
				hr = tempMaterial->GetInt("Model_Physics_Class", &modelPhysicsClass);
				if (hr!=D3D_OK)	newModel.Model_Physics_Class = 0;
				else			newModel.Model_Physics_Class = modelPhysicsClass;
				//CCD
				BOOL modelPhysicsCCD;
				hr = tempMaterial->GetBool("Model_Physics_CCD", &modelPhysicsCCD);
				if (hr!=D3D_OK)
				{
					newModel.Model_Physics_CCD = false;
				}
				else
				{
					if (modelPhysicsCCD == 0)
					{
						newModel.Model_Physics_CCD = false;
					}
					else
					{
						newModel.Model_Physics_CCD = true;
						//Almeno un modello ha il ccd abilitato quindi lo abilita in tutta l'applicazione
						DECore::enginePhysX->setParameter(NX_CONTINUOUS_CD, 1);
					}
				}					
				//CCDScale
				FLOAT modelPhysicsCCDScale = 0.8f;
				hr = tempMaterial->GetFloat("Model_Physics_CCDScale", &modelPhysicsCCDScale);
				if (hr!=D3D_OK)	newModel.Model_Physics_CCDScale = 0.8f;
				else			newModel.Model_Physics_CCDScale = modelPhysicsCCDScale;
				//CCDScale
				FLOAT modelPhysicsCCDMotion = 1.0f;
				hr = tempMaterial->GetFloat("Model_Physics_CCDMotionThreshold", &modelPhysicsCCDMotion);
				if (hr!=D3D_OK)	newModel.Model_Physics_CCDMotionThreshold = 1.0f;
				else			newModel.Model_Physics_CCDMotionThreshold = modelPhysicsCCDMotion;
				//CCD
				BOOL modelPhysicsKinematic;
				hr = tempMaterial->GetBool("Model_Physics_Kinematic", &modelPhysicsKinematic);
				if (hr!=D3D_OK)
				{
					newModel.Model_Physics_Kinematic = false;
				}
				else
				{
					if (modelPhysicsKinematic == 0)	newModel.Model_Physics_Kinematic = false;
					else 							newModel.Model_Physics_Kinematic = true;
				}

				//** INFO **
				//Author
				LPCSTR modelAuthor;
				hr = tempMaterial->GetString("Model_Info_Author", &modelAuthor);
				if (hr!=D3D_OK)	strcpy(newModel.Model_Info_Author, "No Author");
				else			strcpy(newModel.Model_Info_Author, modelAuthor);
				//Date
				LPCSTR modelDate;
				hr = tempMaterial->GetString("Model_Info_Date", &modelDate);
				if (hr!=D3D_OK)	strcpy(newModel.Model_Info_Date, "No Date");
				else			strcpy(newModel.Model_Info_Date, modelDate);
				//Revision
				LPCSTR modelRevision;
				hr = tempMaterial->GetString("Model_Model_Info_Revision", &modelRevision);
				if (hr!=D3D_OK)	strcpy(newModel.Model_Info_Revision, "No Revision");
				else			strcpy(newModel.Model_Info_Revision, modelRevision);
				//Comment
				LPCSTR modelComment;
				hr = tempMaterial->GetString("Model_Info_Comment", &modelComment);
				if (hr!=D3D_OK)	strcpy(newModel.Model_Info_Comment, "No Comment");
				else			strcpy(newModel.Model_Info_Comment, modelComment);
				//Other
				LPCSTR modelOther;
				hr = tempMaterial->GetString("Model_Info_Other", &modelOther);
				if (hr!=D3D_OK)	strcpy(newModel.Model_Info_Other, "No Other");
				else			strcpy(newModel.Model_Info_Other, modelOther);

			}

		
			int currentModelID = PlaceModel(newModel);


			return currentModelID;			
		}
		else
		{
			return -1;
		}
	}
	catch(...)
	{
		return -1;
	}

	return -1;
}

DLL_API int DEModelManager::PlaceModel(STRUCT_DE_MODELPROPERTIES newModel)
{
	//Se trova un posto vuoto mette la Model li dentro
	for ( int t=0; t< totalModels; t++)
	{
		if (ModelList[t].Model_Pos==0)
		{
			ModelList[t] = newModel;
			return t;
		}
	}

	//Setta il nuovo elemento aggiunto
	ModelList.push_back(newModel);

	//Aggiunto uno alla lista
	totalModels = totalModels + 1;

	return totalModels-1;
}

DLL_API int DEModelManager::GetLoadedModels()
{
	return totalModels;
}

DLL_API int DEModelManager::GetSearchedModels()
{
	int searchModels=0;
	for (int t=0; t<DEModelManager::GetLoadedModels(); t++)
	{
		searchModels = searchModels + ModelList[t].Model_Pos;
	}
	return searchModels;
}


DLL_API STRUCT_DE_MODELPROPERTIES DEModelManager::GetModel(int modelID)
{
	if (modelID >= totalModels)	modelID = totalModels;
	if (modelID < 0)
	{
		STRUCT_DE_MODELPROPERTIES nullModel;
		nullModel = GetNullModel();
		return nullModel;	
	}
	return ModelList[modelID];
}

DLL_API STRUCT_DE_MODELPROPERTIES DEModelManager::GetNullModel()
{
	STRUCT_DE_MODELPROPERTIES nullModel;
	strcpy(nullModel.Model_Name, "No Name");
	strcpy(nullModel.Model_Filename, "");	
	strcpy(nullModel.Model_File_Material, "No Material");
	strcpy(nullModel.Model_File_Mesh, "No Model");
	strcpy(nullModel.Model_File_Shader, "No Shader");
	strcpy(nullModel.Model_Texture_Filename0, "");	
	strcpy(nullModel.Model_Texture_Filename1, "");
	strcpy(nullModel.Model_Texture_Filename2, "");
	nullModel.Model_Texture_ColorKey0 = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	nullModel.Model_Texture_ColorKey1 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	nullModel.Model_Texture_ColorKey2 = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	nullModel.Model_Texture_MipMaps0 = true;
	nullModel.Model_Texture_MipMaps1 = true;
	nullModel.Model_Texture_MipMaps2 = true;
	nullModel.Model_IsScreenAligned = false;
	nullModel.Model_IsPickable = true;
	nullModel.Model_IsCullable = true;
	nullModel.Model_IsTrascurable = false;
	nullModel.Model_Debug_FillMode = 0;
	nullModel.Model_Debug_BoundingType = 1;
	nullModel.Model_Debug_ShowAxes = 1;
	nullModel.Model_Physics_Mass = 10;
	nullModel.Model_Physics_Class = 1;
	nullModel.Model_Physics_CCD = false;
	nullModel.Model_Physics_CCDScale = 0.8f;
	nullModel.Model_Physics_CCDMotionThreshold = 0.0f;
	nullModel.Model_Physics_Kinematic = false;
	strcpy(nullModel.Model_Info_Author, "Default Author");
	strcpy(nullModel.Model_Info_Date, "Default Date");
	strcpy(nullModel.Model_Info_Revision, "Dafault Revision");
	strcpy(nullModel.Model_Info_Comment, "Default Comment");
	strcpy(nullModel.Model_Info_Other, "Default Other");

	return nullModel;	
}


DLL_API void DEModelManager::LogReport()
{
	int ModelBlock = 0;
	DELog::LogInfo("<br><b>Allocated Models Report:</b>");
	for (int t=0; t<DEModelManager::GetLoadedModels(); t++)
	{
		//LPD3DXEFFECT tempModel;
		///tempModel = ModelList[t].Model_Pointer;
		//if (tempModel != NULL)
		{
			ModelBlock = ModelBlock + 1;
			DELog::LogInfo("<li>Model[%.0f]: %s - Shared: %.0f", (float)t, ModelList[t].Model_Filename, (float)ModelList[t].Model_Pos);
		}
		//else
		{
			//DELog::LogInfo("<li>Model[%.0f]: %s - Temporary empty block", (float)t, ModelList[t].Model_PropertiesFilename);
		}

	}
	DELog::LogInfo("<li>Models Loaded: %.0f", (float)DEModelManager::GetLoadedModels());
	DELog::LogInfo("<li>Models Block Occupated: %.0f", (float)ModelBlock);
	DELog::LogInfo("<li>Models Block Empty: %.0f", (float)((float)DEModelManager::GetLoadedModels() - (float)ModelBlock));	
}