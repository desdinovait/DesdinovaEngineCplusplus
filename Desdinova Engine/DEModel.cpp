#include "DEGeneralIncludes.h"

struct D3DVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;

    static const DWORD FVF;
};
const DWORD D3DVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


DLL_API DEModel::DEModel()
{
	modelInfo.Info_LogTitle = NULL;
	modelInfo.Info_Tag = NULL;
}

DLL_API bool DEModel::Invalidate()
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


DLL_API bool DEModel::Restore()
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

DLL_API bool DEModel::Release()
{
	try
	{
		//Material
		DEMaterialManager::ReleaseMaterial(modelInfo.Info_MaterialID);
		//Mesh
		DEMeshManager::ReleaseMesh(modelInfo.Info_MeshID);
		//Mesh
		DEModelManager::ReleaseModel(modelInfo.Info_ModelID);
		//Textures
		for (int t=0; t<3; t++)
		{
			DETextureManager::ReleaseTexture(modelInfo.Info_TextureID[t]);
		}

		//Rilasciare l'attore corrente
		if (mainPhysXActor->userData)  delete mainPhysXActor->userData;
		ReleaseUserDataFromShapes(mainPhysXActor);


		modelInfo.Info_IsCreated = false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>DEModel : Unknown statement error - Probably NOT Released");		
		return false;
	}
	return false;
}


DLL_API bool DEModel::Create(LPCTSTR propertiesFilename, LPCSTR logTitle)
{
	modelInfo.Info_ArrayID = -1; //Significa che non ha ancora una posizione nell'array della scena
	modelInfo.Info_LogTitle = logTitle;
	modelInfo.Info_Tag = NULL;
	modelInfo.Info_IsCreated = false;
	modelInfo.Info_IsPicked = false;
	modelInfo.Info_IsCulled = false;
	modelInfo.Info_IsTrascured = false;
	modelInfo.Info_CellIsVisible = true;
	modelInfo.Info_CellIsNear = true;

	modelInfo.Info_Position = D3DXVECTOR3(0,0,0);
	modelInfo.Info_Rotation = D3DXVECTOR3(0,0,0);		//Specificarli in gradi
	modelInfo.Info_Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	try
	{
		//Carica il file delle proprietà
		if (LoadProperties((LPCTSTR)propertiesFilename))
		{
			//Carica il file del materiale
			if (LoadMaterial())
			{	
				//Carica il file dei poligoni (.x)
				if (LoadMesh())
				{
					//Carica le textures(Non è indispensabile per cui se non ci sono la mesh è bianca)
					LoadTextures();

					//Se arrivato fin qui il modello è stato caricato correttamente
					modelInfo.Info_IsCreated=true;
				}
				else
				{
					DELog::LogError("<li>%s : LoadMesh(%s): File not found or error in file.", modelInfo.Info_LogTitle, modelProperties.Model_File_Mesh);
				}
			}
			else
			{
				DELog::LogError("<li>%s : LoadMaterial(%s): Failed - File not found or error in file.", modelInfo.Info_LogTitle, modelProperties.Model_File_Material);
			}
		}
		else
		{
			DELog::LogError("<li>%s : LoadProperties(%s): File not found or error in file.", modelInfo.Info_LogTitle, modelProperties.Model_Filename);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", modelInfo.Info_LogTitle);		
	}

	if (!modelInfo.Info_IsCreated)	DELog::LogError("<li>%s : NOT Created", modelInfo.Info_LogTitle);

	return modelInfo.Info_IsCreated;
}

DLL_API bool DEModel::LoadProperties(LPCTSTR propertiesFilename)
{
	//Carica il file
	try
	{
		modelInfo.Info_ModelID = DEModelManager::CreateModel((LPCTSTR)propertiesFilename);
		if (modelInfo.Info_ModelID < 0 )
		{
			return false;
		}
		else
		{
			modelProperties = DEModelManager::GetModel(modelInfo.Info_ModelID);
			return true;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", modelInfo.Info_LogTitle);		
	}

	return true;
}


DLL_API bool DEModel::LoadMesh()
{
	//Carica il file
	try
	{
		//carica il modello (internamente caricherà o meno le tangenti/binormali, se specificato che ha il bump)
		modelInfo.Info_MeshID = DEMeshManager::CreateMesh((LPCTSTR)modelProperties.Model_File_Mesh, this->GetMaterial().Material_IsBumpmapped);
		if (modelInfo.Info_MeshID < 0 )
		{
			return false;
		}
		else
		{
			//Calcola sempre la posizione dei bounding box e sphere perchè servono per il calcolo delle collisioni e altro
			modelInfo.Info_CenterCoord = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_centerCoord;
			modelInfo.Info_MaxCoord = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_maxCoord;
			modelInfo.Info_MinCoord = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_minCoord;
			modelInfo.Info_Width  = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_width;
			modelInfo.Info_Height = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_height;
			modelInfo.Info_Depth  = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_depth;
			modelInfo.Info_Radius = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_radius;

			return true;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", modelInfo.Info_LogTitle);		
	}

	return false;
}


DLL_API bool DEModel::LoadMaterial()
{
	modelInfo.Info_MaterialID = DEMaterialManager::CreateMaterial((LPCTSTR)modelProperties.Model_File_Material);
	if (modelInfo.Info_MaterialID < 0 )
	{
		return false;
	}
	else
	{
		modelMaterial = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID);
		return true;
	}
	return false;
}


DLL_API bool DEModel::LoadTextures()
{
	modelInfo.Info_TextureID[0] = -1;
	modelInfo.Info_TextureID[1] = -1;
	modelInfo.Info_TextureID[2] = -1;

	try
	{
		//Texture 0
		if (strcmp(modelProperties.Model_Texture_Filename0, "")!=0)
		{
			modelInfo.Info_TextureID[0] = DETextureManager::CreateTexture(modelProperties.Model_Texture_Filename0, modelProperties.Model_Texture_ColorKey0, modelProperties.Model_Texture_MipMaps0);
		}
		//Texture 1
		if (strcmp(modelProperties.Model_Texture_Filename1, "")!=0)
		{
			modelInfo.Info_TextureID[1] = DETextureManager::CreateTexture(modelProperties.Model_Texture_Filename1, modelProperties.Model_Texture_ColorKey1, modelProperties.Model_Texture_MipMaps1);
		}
		//Texture 2
		if (strcmp(modelProperties.Model_Texture_Filename2, "")!=0)
		{
			modelInfo.Info_TextureID[2] = DETextureManager::CreateTexture(modelProperties.Model_Texture_Filename2, modelProperties.Model_Texture_ColorKey2, modelProperties.Model_Texture_MipMaps2);
		}

		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}





DLL_API bool DEModel::SetPhysicsScene(NxScene *sceneAssociated, NxCompartment*	sceneCompartment, unsigned int id)
{
	sceneAssociatedPhysX = sceneAssociated;
	modelInfo.Info_ArrayID = id;

	//Scalature
	modelInfo.Info_Scale = modelProperties.Model_Scale;	
	float radiusScaled = modelInfo.Info_Scale.x;
	if (modelInfo.Info_Scale.y > modelInfo.Info_Scale.x) radiusScaled = modelInfo.Info_Scale.y;
	if (modelInfo.Info_Scale.z > modelInfo.Info_Scale.y) radiusScaled = modelInfo.Info_Scale.z;
	modelInfo.Info_Radius_Scaled = modelInfo.Info_Radius * radiusScaled;
	modelInfo.Info_Width_Scaled = modelInfo.Info_Width * modelInfo.Info_Scale.x;
	modelInfo.Info_Height_Scaled = modelInfo.Info_Height * modelInfo.Info_Scale.y;
	modelInfo.Info_Depth_Scaled = modelInfo.Info_Depth * modelInfo.Info_Scale.z;

	//Materiale (eseguire controllo sui valori massimi !!!)
	NxMaterialDesc materialDesc;
	materialDesc.restitution = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_Restitution;
	materialDesc.staticFriction = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_StaticFriction;
	materialDesc.dynamicFriction = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_DynamicFriction;
	materialDesc.staticFrictionV = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_StaticFrictionV;
	materialDesc.dynamicFrictionV = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_DynamicFrictionV;
	materialDesc.dirOfAnisotropy.x = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_DirOfAnisotropy.x;
	materialDesc.dirOfAnisotropy.y = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_DirOfAnisotropy.y;
	materialDesc.dirOfAnisotropy.z = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_DirOfAnisotropy.z;
	if (DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_IsAnisotropic == true)	materialDesc.flags = NX_MF_ANISOTROPIC;
	mainPhysXMaterial = sceneAssociatedPhysX->createMaterial(materialDesc);


	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	//**CCD**
	NxCCDSkeleton *ccds;
	bool useCCD = modelProperties.Model_Physics_CCD;
	if (useCCD)
	{
		NxTriangleMesh * staticTriangleMesh;
		NxReal ccdScale = modelProperties.Model_Physics_CCDScale;
		NxReal dimX = (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_width/2.0f * modelInfo.Info_Scale.x) * ccdScale;
		NxReal dimY = (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_height/2.0f * modelInfo.Info_Scale.y)* ccdScale;
		NxReal dimZ = (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_depth/2.0f * modelInfo.Info_Scale.z)* ccdScale;

		NxVec3 dim(	dimX, dimY, dimZ);
		NxU32 triangles[3 * 12] = 
		{ 
			0,1,3,
			0,3,2,
			3,7,6,
			3,6,2,
			1,5,7,
			1,7,3,
			4,6,7,
			4,7,5,
			1,0,4,
			5,1,4,
			4,0,2,
			4,2,6
		};

		NxVec3 points[8];
		points[0].set( dim.x, -dim.y, -dim.z);
		points[1].set( dim.x, -dim.y,  dim.z);
		points[2].set( dim.x,  dim.y, -dim.z);
		points[3].set( dim.x,  dim.y,  dim.z);
		points[4].set(-dim.x, -dim.y, -dim.z);
		points[5].set(-dim.x, -dim.y,  dim.z);
		points[6].set(-dim.x,  dim.y, -dim.z);
		points[7].set(-dim.x,  dim.y,  dim.z);


		NxTriangleMeshDesc		meshDesc;
		meshDesc.numVertices = 8;
		meshDesc.numTriangles = 12;
		meshDesc.pointStrideBytes = sizeof(NxVec3);
		meshDesc.triangleStrideBytes = 3 * sizeof(NxU32);
		meshDesc.points = points;
		meshDesc.triangles = triangles;


		MemoryWriteBuffer buf;
  		bool status2 = DECore::enginePhysXCooking->NxCookTriangleMesh(meshDesc,buf);
		if (status2) 
		{
  			MemoryReadBuffer readBuffer(buf.data);
			staticTriangleMesh = DECore::enginePhysX->createTriangleMesh(readBuffer);

			NxSimpleTriangleMesh stm;
			stm.numVertices = 8;
			stm.numTriangles = 6*2;
			stm.pointStrideBytes = sizeof(NxVec3);
			stm.triangleStrideBytes = sizeof(NxU32)*3;

			for (NxU32 i = 0; i < 8; i++)
				points[i].arrayMultiply(points[i], NxVec3(0.8f, 0.8f, 0.9f));

			stm.points = points;
			stm.triangles = triangles;
			stm.flags |= NX_MF_FLIPNORMALS;
			ccds = DECore::enginePhysX->createCCDSkeleton(stm);
		}
	}








	//**Sphere**
	if (modelProperties.Model_Physics_Class <= 0)
	{
		float radiusScaled = modelInfo.Info_Scale.x;
		if (modelInfo.Info_Scale.y > modelInfo.Info_Scale.x) radiusScaled = modelInfo.Info_Scale.y;
		if (modelInfo.Info_Scale.z > modelInfo.Info_Scale.y) radiusScaled = modelInfo.Info_Scale.z;

		NxSphereShapeDesc sphereDesc;
		float radius = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_radius * radiusScaled;
		sphereDesc.radius = radius;
		//sphereDesc.localPose.t = NxVec3(0,0,0);
		sphereDesc.materialIndex= mainPhysXMaterial->getMaterialIndex();
		if ((useCCD)&&(ccds!=NULL))
		{
			sphereDesc.ccdSkeleton = ccds;
			sphereDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  
		}
		actorDesc.shapes.pushBack(&sphereDesc);
	}
	//**Box**
	else if (modelProperties.Model_Physics_Class == 1)
	{
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions.set((DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_width/2.0f) * modelInfo.Info_Scale.x, (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_height/2.0f) * modelInfo.Info_Scale.y, (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_depth/2.0f) * modelInfo.Info_Scale.z);
		//boxDesc.localPose.t = NxVec3(0, 0, 0);
		boxDesc.materialIndex= mainPhysXMaterial->getMaterialIndex();
		if ((useCCD)&&(ccds!=NULL))
		{
			boxDesc.ccdSkeleton = ccds;
			boxDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  
		}
		actorDesc.shapes.pushBack(&boxDesc);
	}//*/
	//**Mesh**
	else if (modelProperties.Model_Physics_Class >= 2)
	{
		if (DECore::enginePhysXCooking != NULL)
		{

			//Used to retrieve information from X file
			typedef struct 
			{
				D3DXVECTOR3 VertexPos;
				D3DXVECTOR3 Normal;
				D3DXVECTOR2 TexCoord;
				//D3DXVECTOR3 Tangent;
				//D3DXVECTOR3 Binormal;
			} Mesh_FVF;

			int NumVerticies = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->GetNumVertices();
			int NumTriangles = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->GetNumFaces();
			DWORD FVFSize = D3DXGetFVFVertexSize(DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->GetFVF());

			//Create pointer for vertices
			NxVec3* verts = new NxVec3[NumVerticies];
			char *DXMeshPtr;
			DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->LockVertexBuffer(D3DLOCK_READONLY, (void**)&DXMeshPtr);
			for(int i = 0; i < NumVerticies; i++)
			{
				Mesh_FVF *DXMeshFVF = (Mesh_FVF*)DXMeshPtr;
				verts[i] = NxVec3(DXMeshFVF->VertexPos.x, DXMeshFVF->VertexPos.y, DXMeshFVF->VertexPos.z);
				DXMeshPtr += FVFSize;
			}
			DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->UnlockVertexBuffer();

			//Create pointer for indices
			typedef struct 
			{
				short IBNumber[3];
			} IndexBufferStruct;
			IndexBufferStruct *tris = new IndexBufferStruct[NumTriangles];
			IndexBufferStruct *DXMeshIBPtr;
			DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->LockIndexBuffer(D3DLOCK_READONLY, (void**)&DXMeshIBPtr);
			for(int NumInd = 0; NumInd < NumTriangles; NumInd++)
			{
				tris[NumInd].IBNumber[0] = DXMeshIBPtr[NumInd].IBNumber[0];
				tris[NumInd].IBNumber[1] = DXMeshIBPtr[NumInd].IBNumber[1];
				tris[NumInd].IBNumber[2] = DXMeshIBPtr[NumInd].IBNumber[2];
			}
			DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->UnlockIndexBuffer();

			//Descriptor
			NxTriangleMeshDesc TriMeshDesc;
			TriMeshDesc.numVertices = NumVerticies;
			TriMeshDesc.numTriangles = NumTriangles;
			TriMeshDesc.pointStrideBytes = sizeof(NxVec3);
			TriMeshDesc.triangleStrideBytes = 3*sizeof(short);
			TriMeshDesc.points = verts;
			TriMeshDesc.triangles = tris;
			TriMeshDesc.flags = NX_MF_16_BIT_INDICES;
			
			delete[] verts;
			delete[] tris;


			MemoryWriteBuffer buf;
			//if(DECore::enginePhysXCooking->NxCookTriangleMesh(TriMeshDesc, UserStream("c:\\tmp.bin", false)))
			if(DECore::enginePhysXCooking->NxCookTriangleMesh(TriMeshDesc, buf))
			{
				NxTriangleMeshShapeDesc triangleShapeDesc;
				//triangleShapeDesc.meshData = DECore::enginePhysX->createTriangleMesh(UserStream("c:\\tmp.bin", true));			
				triangleShapeDesc.meshData = DECore::enginePhysX->createTriangleMesh(MemoryReadBuffer(buf.data));
				//triangleShapeDesc.localPose.t = NxVec3(0,0,0);
				triangleShapeDesc.materialIndex = mainPhysXMaterial->getMaterialIndex();
				if(triangleShapeDesc.meshData)
				{
					// Create body
					if ((useCCD)&&(ccds!=NULL))
					{
						triangleShapeDesc.ccdSkeleton = ccds;
						triangleShapeDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  
					}				
					actorDesc.shapes.pushBack(&triangleShapeDesc);
				}
			}
			else
			{
				//Se non è riesce a fare il Cooking usa di default il Box
				NxBoxShapeDesc boxDesc;
				boxDesc.dimensions.set((DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_width/2.0f) * modelInfo.Info_Scale.x, (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_height/2.0f) * modelInfo.Info_Scale.y, (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_depth/2.0f) * modelInfo.Info_Scale.z);
				//boxDesc.localPose.t = NxVec3(0, 0, 0);
				boxDesc.materialIndex = mainPhysXMaterial->getMaterialIndex();
				if ((useCCD)&&(ccds!=NULL))
				{
					boxDesc.ccdSkeleton = ccds;
					boxDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  
				}
				actorDesc.shapes.pushBack(&boxDesc);//*/
			}
		}
		else
		{
			//Se non è abilitato il Cooking usa di default il Box
			NxBoxShapeDesc boxDesc;
			boxDesc.dimensions.set((DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_width/2.0f) * modelInfo.Info_Scale.x, (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_height/2.0f) * modelInfo.Info_Scale.y, (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_depth/2.0f) * modelInfo.Info_Scale.z);
			//boxDesc.localPose.t = NxVec3(0, 0, 0);
			boxDesc.materialIndex = mainPhysXMaterial->getMaterialIndex();
			if ((useCCD)&&(ccds!=NULL))
			{
				boxDesc.ccdSkeleton = ccds;
				boxDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  
			}
			actorDesc.shapes.pushBack(&boxDesc);//*/
		}//*/
	}






	//Crea l'attore (controlla se è definita la massa, altrimenti crea un'attore statico)
    if (modelProperties.Model_Physics_Mass > 0.0f )
    {
        actorDesc.body = &bodyDesc;
 		actorDesc.density = DEMaterialManager::GetMaterial(modelInfo.Info_MaterialID).Material_Restitution;
    }
    else
    {
        actorDesc.body = NULL;
    }
	actorDesc.globalPose.t = NxVec3(0,0,0);
	
	//Crea l'attore PhysX (associato al compartimento)
	actorDesc.compartment = sceneCompartment;
	mainPhysXActor = sceneAssociatedPhysX->createActor(actorDesc);




	//AGGIUNGE LO USERDATA tramite una struttura con id,flags,ecc
    mainPhysXActor->userData = new ActorUserData;
	((ActorUserData *)(mainPhysXActor->userData))->id = modelInfo.Info_ArrayID;
	// FIXME: After setting it, actorDesc.managedHwSceneIndex gets cleared for some reason...
	#if 0
	NxActorDesc actorDesc2;
	mainPhysXActor->saveToDesc(actorDesc2);
	if (actorDesc2.managedHwSceneIndex > 0)  
	{
		// To identify the actor when render it, set a bit in the userdata flags
		((ActorUserData *)(mainPhysXActor->userData))->flags |= UD_IS_HW_ACTOR;
	}
	#endif
    AddUserDataToShapes(mainPhysXActor, modelInfo.Info_ArrayID);


	//Setta la velocità minima per cui applicare il CCD
	mainPhysXActor->setCCDMotionThreshold(modelProperties.Model_Physics_CCDMotionThreshold);
	//KINEMATIC actor
	if (modelProperties.Model_Physics_Kinematic == true)
	{
		mainPhysXActor->raiseBodyFlag(NX_BF_KINEMATIC);
	}

	return true;
}




DLL_API void DEModel::SetCollisionGroup(unsigned int collisionGroup)
{
    NxU32 nbShapes = mainPhysXActor->getNbShapes();
    NxShape*const* shapes = mainPhysXActor->getShapes();
    while (nbShapes--)
    {
        shapes[nbShapes]->setGroup(collisionGroup);
    }
}


DLL_API void DEModel::SetArrayID(int arrayID)
{
	modelInfo.Info_ArrayID = arrayID;
}
DLL_API NxActor* DEModel::GetPhysXActor() 
{
	return mainPhysXActor;
}



DLL_API bool DEModel::InsideTrigger(unsigned int triggerID, unsigned int mode)
{
	ActorUserData* ud = (ActorUserData*)mainPhysXActor->userData;
	if (!ud)  return false;
    if (ud->flags & mode)
    {
	   if (ud->triggerID == triggerID)
	   {
			ud->flags &= ~mode;
			return true;
	   }
	   else
	   {
	     return false;
	   }
	}
	else
	{
		return false;
	}
	return false;
}



DLL_API bool DEModel::CheckCollision(NxContactPairFlag mode)
{
	ActorUserData* ud = (ActorUserData*)mainPhysXActor->userData;
	if (!ud)  return false;
	if (ud->contactEvents & mode)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}



DLL_API bool DEModel::CheckCollisionWith(DEModel model, NxShapesType shapeType)
{

	/*return NxBoxBoxIntersect(
		NxVec3((modelInfo.Info_Width / 2.0f) * modelInfo.Info_Scale.x, (modelInfo.Info_Height/2.0f) * modelInfo.Info_Scale.y, (modelInfo.Info_Depth/2.0f) * modelInfo.Info_Scale.z) ,
		mainPhysXActor->getGlobalPosition() ,
		mainPhysXActor->getGlobalOrientation(),
		NxVec3((model.GetModelInfo().Info_Width/2.0f) * model.GetModelInfo().Info_Scale.x, (model.GetModelInfo().Info_Height/2.0f) * model.GetModelInfo().Info_Scale.y, (model.GetModelInfo().Info_Depth/2.0f) * model.GetModelInfo().Info_Scale.z) , 
		model.GetPhysXActor()->getGlobalPosition(),
		model.GetPhysXActor()->getGlobalOrientation(),
		fullTest);*/

		//Tutti i tipi
		NxU32 nbShapes=0;
		if (shapeType ==  NX_ALL_SHAPES)
		{
			nbShapes = sceneAssociatedPhysX->getNbDynamicShapes() + sceneAssociatedPhysX->getNbStaticShapes(); //Tipo statico + dinamico
		}
		else if (shapeType ==  NX_DYNAMIC_SHAPES)
		{
			nbShapes = sceneAssociatedPhysX->getNbDynamicShapes();
		}
		else if (shapeType ==  NX_STATIC_SHAPES)
		{
			nbShapes = sceneAssociatedPhysX->getNbStaticShapes();
		}

	    //NxShape** shapes = (NxShape**)NxAlloca(nbShapes*sizeof(NxShape*));
		NxShape** shapes = new NxShape*[nbShapes];
		for (NxU32 i = 0; i < nbShapes; i++)  shapes[i] = NULL;
		NxU32 activeGroups = 0xffffffff;
		NxGroupsMask* groupsMask = NULL;
		NxU32 collided = 0;	//Numero di attori collisi
		
		//Sphere
		if (modelProperties.Model_Physics_Class <= 0)
		{
			NxSphere worldSphere(mainPhysXActor->getGlobalPosition(), modelInfo.Info_Radius_Scaled);
			collided = sceneAssociatedPhysX->overlapSphereShapes(worldSphere, shapeType, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask);
		}
		//Box
		else if (modelProperties.Model_Physics_Class == 1)
		{
			NxBox worldBox(mainPhysXActor->getGlobalPosition(), NxVec3(modelInfo.Info_Width_Scaled/2.0f, modelInfo.Info_Height_Scaled/2.0f, modelInfo.Info_Depth_Scaled/2.0f), mainPhysXActor->getGlobalOrientation());
			collided = sceneAssociatedPhysX->overlapOBBShapes(worldBox, shapeType, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask);
		}
		//Mesh
		else if (modelProperties.Model_Physics_Class >= 2)
		{
		    //bool sceneAssociatedPhysX->overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxU32& nb, const NxU32*& indices);
		}

		//Cerca tra gli attori in collisione se c'è quello definito tramite l'ID
		for (unsigned int s=0; s<collided; s++)
		{
			NxU32 current = model.GetModelInfo().Info_ArrayID;
			NxActor* actor = &shapes[s]->getActor();
			NxU32 cur =((ActorUserData *)(actor->userData))->id;
			if (cur == current)
			{
				delete shapes;
				return true;
			}
		}
		delete shapes;
		return false;
}



DLL_API D3DXMATRIX DEModel::CalculateMatrix()
{
	//***** MATRICI ***
	D3DXMATRIX finalMatrix;
	D3DXMATRIX scaleMatrix;
	D3DXMATRIX posMatrix;
	D3DXMATRIX rotMatrix;
	D3DXMATRIX traslMatrix;
	//Inizializza le matrici
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMatrixIdentity(&traslMatrix);

	//Il modello è sempre rivolto verso la camera
	if (modelProperties.Model_IsScreenAligned)
	{
		D3DXMATRIX	 mat;
		D3DXMATRIX	 trasp;
		D3DXMATRIX	 traslMatrix;
		//Preleva la matrice di vista corrente
		DECore::D3DDevice->GetTransform(D3DTS_VIEW,&mat);
		//La trasposta della matrice di vista è rivolta verso l'osservatore
		D3DXMatrixTranspose(&trasp,&mat);
					
		//Riempie la matrice con i valori utilizzati per traslare il billboard
		traslMatrix._11 = trasp._11;
		traslMatrix._21 = trasp._21;
		traslMatrix._31 = trasp._31;
		traslMatrix._41 = 0;

		traslMatrix._12 = trasp._12;
		traslMatrix._22 = trasp._22;
		traslMatrix._32 = trasp._32;
		traslMatrix._42 = 0;

		traslMatrix._13 = trasp._13;
		traslMatrix._23 = trasp._23;
		traslMatrix._33 = trasp._33;
		traslMatrix._43 = 0;

		//Questa ulteriore specificazione serve perchè altrimenti il billboard non funzionerebbe!!
		traslMatrix._14 = 0;
		traslMatrix._24 = 0;
		traslMatrix._34 = 0;
		traslMatrix._44 = 1;

		/*dMatrix3 matr;
		matr[0] = trasp._11;
		matr[1] = trasp._21;
		matr[2] = trasp._31;
		matr[3] = 0;
		matr[4] = trasp._12;
		matr[5] = trasp._22;
		matr[6] = trasp._32;
		matr[7] = 0;
		matr[8] = trasp._13;
		matr[9] = trasp._23;
		matr[10] = trasp._33;
		matr[11] = 0;

		//Setta la posizione
		dGeomSetRotation(modelGeom[0], matr);*/
	}

		
	//*** MOTORE FISICO ***
	float R[16];
	mainPhysXActor->getGlobalPose().getColumnMajor44(R);
	rotMatrix._11 = R[0];	rotMatrix._12 = R[1];	rotMatrix._13 = R[2];	rotMatrix._14 = R[3];
	rotMatrix._21 = R[4];	rotMatrix._22 = R[5];	rotMatrix._23 = R[6];	rotMatrix._24 = R[7];
	rotMatrix._31 = R[8];	rotMatrix._32 = R[9];	rotMatrix._33 = R[10];	rotMatrix._34 = R[11];
	rotMatrix._41 = R[12];	rotMatrix._42 = R[13];	rotMatrix._43 = R[14];	rotMatrix._44 = R[15];

	D3DXMatrixScaling(&scaleMatrix, modelInfo.Info_Scale.x, modelInfo.Info_Scale.y, modelInfo.Info_Scale.z);
	finalMatrix = scaleMatrix * rotMatrix;	



	//*** BOUNDINGS ***
	//Posizione in base al centro giusto dell'oggetto(non il pivot) per tutti i bounding
	D3DXMatrixTranslation(&posMatrix, modelInfo.Info_CenterCoord.x, modelInfo.Info_CenterCoord.y, modelInfo.Info_CenterCoord.z);	
	//Setta il BoundingBox con anche la scalatura appropriata
	D3DXMatrixScaling(&scaleMatrix, modelInfo.Info_Width/2, modelInfo.Info_Height/2, modelInfo.Info_Depth/2);
	modelInfo.Info_BoundingBoxMatrix = scaleMatrix * posMatrix * finalMatrix;
	//Setta il BoundingSphere con anche la scalatura appropriata
	D3DXMatrixScaling(&scaleMatrix, modelInfo.Info_Radius, modelInfo.Info_Radius, modelInfo.Info_Radius);
	modelInfo.Info_BoundingSphereMatrix = scaleMatrix * posMatrix * finalMatrix;
	//Setta gli assi
	D3DXMATRIX	 scaleAxesMatrix;
	D3DXMatrixScaling(&scaleAxesMatrix, modelInfo.Info_Width, modelInfo.Info_Height, modelInfo.Info_Depth);
	modelInfo.Info_AxesMatrix = scaleAxesMatrix * finalMatrix;

	//*** CENTRO MODIFICATO ***
	D3DXVec3TransformCoord(&modelInfo.Info_CenterCoord3D, &modelInfo.Info_CenterCoord, &modelInfo.Info_FinalMatrix);			

	//*** POSIZIONE 2D DEL CENTRO FINALE ***
	D3DXMATRIX	viewMat;
	D3DXMATRIX	projMat;
	D3DVIEWPORT9 Viewport;
	DECore::D3DDevice->GetTransform(D3DTS_VIEW,&viewMat);
	DECore::D3DDevice->GetTransform(D3DTS_PROJECTION,&projMat);
	DECore::D3DDevice->GetViewport(&Viewport);
	//Posizione del centro perfetto dell modello nella sistuazione attuale
	//In questo modo viene detto che il il modello si trova in una posizione che è quella al
	//centro del mondo, che però ha una matrice di posizione (world) che comunque
	//corrisponde a quella corrente di World per cui il modello si trova nella
	//sua posizione effettiva, sarebbe come fare: D3DXVECTOR3(0,0,0) * finalPos = worldMat 
	D3DXVec3Project(&modelInfo.Info_CenterCoord2D, &modelInfo.Info_CenterCoord, &Viewport, &projMat, &viewMat, &finalMatrix);

	//** MATRICE FINALE DEL MODELLO NELLO SPAZIO **
	modelInfo.Info_FinalMatrix = finalMatrix;

	return finalMatrix;
}


DLL_API void DEModel::SetPosition(D3DXVECTOR3 position)
{
	modelInfo.Info_Position = position;
	NxVec3 pos;
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;
	if (modelProperties.Model_Physics_Kinematic == false)
	{
		//Muove normalmente
		mainPhysXActor->setGlobalPosition(pos);
	}
	else
	{
		mainPhysXActor->moveGlobalPosition(pos);
	}
}
DLL_API D3DXVECTOR3 DEModel::GetPosition()
{
	//return modelInfo.Info_Position;
	NxVec3 position;
	D3DXVECTOR3 pos;
	position = mainPhysXActor->getGlobalPosition();
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;
	return pos;
}

DLL_API void DEModel::SetRotation(D3DXVECTOR3 rotation)
{
	modelInfo.Info_Rotation = rotation;
	NxVec3 angles;
	angles.x = rotation.x;
	angles.y = rotation.y;
	angles.z = rotation.z;
	NxQuat mat;
	mat = AnglesToQuat(angles);
	if (modelProperties.Model_Physics_Kinematic == false)
	{
		//Muove normalmente
		mainPhysXActor->setGlobalOrientationQuat(mat);
	}
	else
	{
		mainPhysXActor->moveGlobalOrientationQuat(mat);
	}
}


DLL_API D3DXVECTOR3 DEModel::GetRotation()
{
	//Non funziona...provare a fare Set e poi get per vedere!

	//return modelInfo.Info_Rotation;
	NxQuat rotation;
	D3DXVECTOR3 rot;
	rotation = mainPhysXActor->getGlobalOrientationQuat();
	rot.x = rotation.x;
	rot.y = rotation.y;
	rot.z = rotation.z;
	return rot;
}

DLL_API void DEModel::ApplyForce(D3DXVECTOR3 forceDir, float forceStrength, NxForceMode mode)
{
	NxVec3 fDir;
	fDir.x = forceDir.x;
	fDir.y = forceDir.y;
	fDir.z = forceDir.z;
	NxVec3 forceVec = forceStrength * fDir;
	mainPhysXActor->addForce(forceVec, mode);
}
DLL_API void DEModel::ApplyTorque(D3DXVECTOR3 forceDir, float forceStrength, NxForceMode mode)
{
	NxVec3 fDir;
	fDir.x = forceDir.x;
	fDir.y = forceDir.y;
	fDir.z = forceDir.z;
	NxVec3 forceVec = forceStrength * fDir;
	mainPhysXActor->addTorque(forceVec, mode);
}




DLL_API void DEModel::CalculatePosition(D3DXVECTOR3 viewPos, D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties)
{
	//Il modello è stato creato
	if (modelInfo.Info_IsCreated)						
	{
		//*** AZZERAMENTO VARIABILI ***
		//Setta comunque che l'oggetto non è in piking
		modelInfo.Info_IsPicked = false;

		//*** MATRICE FINALE ***
		DECore::D3DDevice->SetTransform( D3DTS_WORLD, &modelInfo.Info_FinalMatrix);
		
		//*** GRIGLIA ***
		if (spatialGrid != NULL)
		{
			//Nel frustrum
			if (spatialGrid->GetCellInFrustrum(modelInfo.Info_CenterCoord3D))	
			{
				modelInfo.Info_CellIsVisible = true;
			}
			else															
			{
				modelInfo.Info_CellIsVisible = false;
			}
			//In griglia
			if (spatialGrid->GetCellNeightbourhood(modelInfo.Info_CenterCoord3D))	
			{
				modelInfo.Info_CellIsNear = true;
			}
			else																
			{
				modelInfo.Info_CellIsNear = false;
			}
		}
		else
		{
			//Ummm...il caso peggiore perchè non esiste una griglia
			modelInfo.Info_CellIsVisible = true;
			modelInfo.Info_CellIsNear = true;
		}//*/
		modelInfo.Info_CellIsVisible = true;
		modelInfo.Info_CellIsNear = true;


		//*** FRUSTUM CULLING ***
		if (modelProperties.Model_IsCullable)
		{
			//if ((modelInfo.Info_CellIsVisible)&&(modelInfo.Info_CellIsNear))
			{
				D3DXMATRIX viewTransform;
				DECore::D3DDevice->GetTransform(D3DTS_VIEW,& viewTransform);
				if (DEUtility::FOVCheck(modelInfo.Info_CenterCoord3D, modelInfo.Info_Radius_Scaled, viewTransform, fovProperties, &viewPos))	
				{
					modelInfo.Info_IsCulled=false;
				}
				else
				{
					modelInfo.Info_IsCulled=true;
				}
			}
		}
		else
		{
			modelInfo.Info_IsCulled = false;
		}


		//*** RENDERING ***
		//Procede con il rendering se l'oggetto non è cullato
		if (modelInfo.Info_IsCulled==false)
		{
			//*** DISTANZA DALLA CAMERA ***
			//Calcola la distanza del centro dell'oggetto alla camera
			D3DXVECTOR3 diffFar;
			D3DXVec3Subtract( &diffFar, &centerCamera, &modelInfo.Info_CenterCoord3D);
			modelInfo.Info_DistanceFromCamera = D3DXVec3Length(&diffFar);//*/
			
			//**** TRASCURABILITA' ***
			//Rapporto tra raggio e distanza dalla camera
			float distanceRapp = modelInfo.Info_Radius_Scaled / (modelInfo.Info_DistanceFromCamera - modelInfo.Info_Radius_Scaled);
			//Se il rapporto scende sotto l'1%(1/100=0.01) allora si trascura il modello
			if ( distanceRapp < 0.01f)	modelInfo.Info_IsFar = true;
			else						modelInfo.Info_IsFar = false;//*/

			/*** LEVELS OF DETAIL ***
			unsigned short totalLods = 30;
			//modelInfo.Info_CurrentLod = 1;
			DWORD maxV = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Progressive->GetMaxVertices();
			DWORD minV = DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Progressive->GetMinVertices();
			DWORD totalVerticies = maxV - minV;
			DWORD verticiesXLOD = totalVerticies / totalLods;
			DWORD percLOD = 100 / totalLods; //In %
			DWORD percRapp = distanceRapp * 100.0f;
			modelInfo.Info_CurrentLod = totalLods - (percRapp / percLOD);
			//Setta i vertici in base a LOD corrente
			DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Progressive->SetNumVertices(DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Progressive->GetMaxVertices() - ( verticiesXLOD * (modelInfo.Info_CurrentLod - 1)));
			//*/
		}
			
	}//Loaded
}



DLL_API void DEModel::Render()
{
	if (modelInfo.Info_IsCulled==false)
	{
		if (modelInfo.Info_IsFar == false)
		{
			UINT uPasses;
			if (DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point != NULL)
			{
				DECore::engineEffect->Begin( &uPasses, 0 ); 
				for( UINT uPass = 0; uPass < uPasses; ++uPass )
				{
					//Inizia il passo corrente
					DECore::engineEffect->BeginPass( uPass );
					DECore::engineEffect->CommitChanges();			
					//Disegna la primitiva		
					for( DWORD i=0; i<DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Parts; i++ )
					{
						DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point->DrawSubset(i);
					}
					//Finisce il passo corrente
					DECore::engineEffect->EndPass();
				}
				DECore::engineEffect->End();
			}//*/
		}//Not far
	}//Not culled
}



DLL_API int DEModel::GetArrayID()
{
	return modelInfo.Info_ArrayID;
}

DLL_API bool DEModel::CheckPicked(D3DXVECTOR2 ptCursor, bool includeFar)
{
	//** PIKING **
	if (modelInfo.Info_IsCreated)
	{
		//Se è abilitato per il picking, non è cullato e non è lontano
		if ((modelProperties.Model_IsPickable==true)&&(modelInfo.Info_IsCulled==false))
		{
			if ((modelInfo.Info_IsFar==false) || ((modelInfo.Info_IsFar)&&(includeFar)))
			{
				//Dimensioni video
				D3DVIEWPORT9 currentViewport;
				DECore::D3DDevice->GetViewport(&currentViewport);
				float engineWidth = (float)currentViewport.Width;
				float engineHeight = (float)currentViewport.Height;

				D3DXVECTOR3 vPickRayDir;
				D3DXVECTOR3 vPickRayOrig;
				
				//Preleva la matrice di proiezione
				D3DXMATRIX m_matProj;
				DECore::D3DDevice->GetTransform(D3DTS_PROJECTION,&m_matProj);

				//Crea il vettore di selezione tramite lo screen space
				D3DXVECTOR3 v;
				v.x =  ( ( ( 2.0f * ptCursor.x ) / engineWidth  ) - 1 ) / m_matProj._11;
				v.y = -( ( ( 2.0f * ptCursor.y ) / engineHeight  ) - 1 ) / m_matProj._22;
				v.z =  1.0f;

 				//Preleva la matrice di vista e ne trova l'inversa
				D3DXMATRIX m;
				D3DXMATRIX m_matView;
				DECore::D3DDevice->GetTransform(D3DTS_VIEW,&m_matView);
				D3DXMatrixInverse( &m, NULL, &m_matView );

				//Trasforma il raggio nello spazio 3D
				vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
				vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
				vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
				vPickRayOrig.x = m._41;
				vPickRayOrig.y = m._42;
				vPickRayOrig.z = m._43;


				FLOAT fBary1, fBary2, fDist;
				//Intersezione con la bounding sphere
				//if (DEUtility::RayIntersectsSphere2D(vPickRayOrig, vPickRayDir, modelInfo.Info_FinalCenterCoord, modelInfo.Info_FinalRadius))
				if (D3DXSphereBoundProbe(&modelInfo.Info_CenterCoord3D, modelInfo.Info_Radius_Scaled, &vPickRayOrig, &vPickRayDir))
				{	
					//Intersezione con il bounding box
					D3DXVECTOR3 v0(0,0,0);
					D3DXVECTOR3 v1(0,0,0);
					D3DXVECTOR3 v2(0,0,0);
					for (unsigned long h=0; h<36; h=h+3)	//36 xkè il BBox è composto da 36 triangoli
					{			
						D3DXVec3TransformCoord(&v0, &D3DXVECTOR3(DEVBManager::GetBoundingBoxVertices(h).x,DEVBManager::GetBoundingBoxVertices(h).y,DEVBManager::GetBoundingBoxVertices(h).z),&modelInfo.Info_BoundingBoxMatrix);
						D3DXVec3TransformCoord(&v1, &D3DXVECTOR3(DEVBManager::GetBoundingBoxVertices(h+1).x,DEVBManager::GetBoundingBoxVertices(h+1).y,DEVBManager::GetBoundingBoxVertices(h+1).z),&modelInfo.Info_BoundingBoxMatrix);
						D3DXVec3TransformCoord(&v2, &D3DXVECTOR3(DEVBManager::GetBoundingBoxVertices(h+2).x,DEVBManager::GetBoundingBoxVertices(h+2).y,DEVBManager::GetBoundingBoxVertices(h+2).z),&modelInfo.Info_BoundingBoxMatrix);
						//Senza D3DX !!! SEMBRA ESSERE PIU VELOCE !!!
						if( DEUtility::RayIntersectsTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2, &fDist, &fBary1, &fBary2 ) )
						{
							//Intersezione con i triangoli della mesh
							//Crea 2 vettori temporanei(sono dir e orig in model-space)
							//perchè la mesh non terrebbe altrimenti conto della sua matrice attuale
							D3DXVECTOR3 vOrig,vDir;
							D3DXMATRIX invMat;
							D3DXMatrixInverse(&invMat,NULL,&modelInfo.Info_FinalMatrix);
							D3DXVec3TransformCoord(&vOrig,&vPickRayOrig,&invMat);
							D3DXVec3TransformNormal(&vDir,&vPickRayDir,&invMat);
							BOOL bHit;
							//D3DXIntersect(DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Progressive, &vOrig, &vDir, &bHit, NULL, &fBary1, &fBary2, &fDist, NULL, NULL);
							D3DXIntersect(DEMeshManager::GetMesh(modelInfo.Info_MeshID).Mesh_Point, &vOrig, &vDir, &bHit, NULL, &fBary1, &fBary2, &fDist, NULL, NULL);
							if( bHit )
							{
								modelInfo.Info_IsPicked = true;
								return true;
							}
						}
					}
				}//*/
			}//*/
		}
	}//isCreated*/
	return modelInfo.Info_IsPicked;
}

//Modello
DLL_API STRUCT_DE_MODELPROPERTIES	DEModel::GetModelProperties()
{
	return modelProperties;
}
DLL_API STRUCT_DE_MODELINFO	DEModel::GetModelInfo()
{
	return modelInfo;
}
DLL_API STRUCT_DE_MATERIAL DEModel::GetMaterial()
{
	return modelMaterial;
}




DLL_API	void DEModel::SetWorldMatrix(D3DXMATRIX newMatrix)
{
	//worldMatrix = newMatrix;
}
DLL_API void DEModel::SetSpatialGrid(DESpatialGrid *worldSparialGrid)
{
	spatialGrid = worldSparialGrid;
}