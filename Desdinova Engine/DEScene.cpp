#include "DEGeneralIncludes.h"

bool compareDistanceModel(const void *a, const void *b)
{
	DEModel* p1 = (DEModel*)a;
    DEModel* p2 = (DEModel*)b;
	return p1->GetModelInfo().Info_DistanceFromCamera > p2->GetModelInfo().Info_DistanceFromCamera;
}

DLL_API DEScene::DEScene()
{
	mainTag = NULL;
	bCreated = false;
	bLoaded = false;
}

DLL_API void DEScene::PreparePhysics()
{
	//Prepara, una sola volta, le coppie di collisione (in questo caso, tutti con tutti)
	if (preparedPhysics==false)
	{
		NxU32 nbActors = scenePhysX->getNbActors();
		for (unsigned int a = 0; a< nbActors; a++)
		{
			NxActor* actor1 = scenePhysX->getActors()[a];
			for (unsigned int b = 0; b< nbActors; b++)
			{
				NxActor* actor2 = scenePhysX->getActors()[b];
				scenePhysX->setActorPairFlags(*actor1, *actor2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
			}
		}
		preparedPhysics = true;
	}//*/
}

DLL_API void DEScene::UpdatePhysics()
{
	if ((bLoaded)&&(isPaused==false))
	{
		float deltaTime;
		Time = timeGetTime()*0.001f;
		deltaTime = Time - TimeLast;
		TimeLast = Time;

		
		scenePhysX->simulate(deltaTime);
		scenePhysX->flushStream();
		scenePhysX->fetchResults(NX_RIGID_BODY_FINISHED, true);
		//while (!scenePhysX->fetchResults(NX_RIGID_BODY_FINISHED, false));

		//Sistemi Particellari
		for (unsigned int p=0; p< totalParticles; p++)
		{
			//Disegna il modello corrente
			particlesList.at(p)->Update(deltaTime);
		}

		/*Contatti
		NxU32 nbActors = scenePhysX->getNbActors();
		NxActor** actors = scenePhysX->getActors();
		while(nbActors--)
		{
			NxActor* actor = *actors++;

			// Get our object back
			ActorUserData* ud = (ActorUserData*)actor->userData;
			if (!ud)  continue;

			if (ud->contactEvents != 0)
			{
				int a=0;
				// Code for every contact event: starting, ending, during
			}
			
			// Execute code according to events
			if (ud->contactEvents & NX_NOTIFY_ON_START_TOUCH)
			{
				// Code for the pair starting contact
			}
			else if (ud->contactEvents & NX_NOTIFY_ON_END_TOUCH)
			{
				// Code for pair ending contact
			}
			else
			{
				// Code for the pair in continual contact
			}
		}//*/

	}
}


DLL_API bool DEScene::UpdateInput()
{
	//Funzione definita da utente
	if (sceneProperties.Scene_FuncPointer_Input!=NULL)
	{
		(* sceneProperties.Scene_FuncPointer_Input) ();
	}

	//Update degli input della camera corrente
	camerasList.at(currentCameraID)->UpdateInput();
	return true;
}


DLL_API void DEScene::UpdateView()
{
	//*** CAMERA ***
	camerasList.at(currentCameraID)->SetHWND(mainHWND);
	camerasList.at(currentCameraID)->SetWindowed(mainWindowed);
	camerasList.at(currentCameraID)->UpdateView();

	//*** GRIGLIA ***
	spatialGridPosition = spatialGrid.GetCellID(camerasList.at(currentCameraID)->GetPosition(), true);
	spatialGrid.Update(camerasList.at(currentCameraID)->GetView(), sFOV);

}


DLL_API bool DEScene::Load()
{
	bLoaded = false;
	try
	{	
		//L'inizializzazione del load(per schermate di loading) è solo 2D !!!!!?!??!?!
		//Calcola le matrici globali in base alla nuova scena	
		D3DXMatrixOrthoLH(&matrixOrtho, (float)mainWidth, (float)mainHeight, 0.0f, 1.0f);
		DECore::D3DDevice->SetTransform(D3DTS_PROJECTION, &matrixOrtho);
		DECore::D3DDevice->SetTransform(D3DTS_VIEW, &matrixIdentity);
		DECore::D3DDevice->SetTransform(D3DTS_WORLD, &matrixIdentity);

		//Apply default render states
		if (sceneDefaultPixelstateBlock != NULL)	sceneDefaultPixelstateBlock->Apply();
		if (sceneDefaultVertexstateBlock != NULL)	sceneDefaultVertexstateBlock->Apply();

		//Assi
		mainAxes.Create("mainExes");

		//Font
		mainFont.Create("Verdana", 10, false, false, "mainFont");

		//Crea il bounding
		modelBoundingBox.Create("modelBoundingBox");
		modelBoundingSphere.Create("modelBoundingSphere");
		//Crea gli assi
		modelAxes.Create("modelAxes");

		//Triggers Bounding
		triggerBoundingBox.Create("triggerBoundingBox");
		triggerBoundingSphere.Create("triggerBoundingSphere");

		//Bounding e posizione luci
		lightBoundingSphere.Create("lightBoundingSphere");
		lightBoundingBox.Create("lightBoundingBox");

		//Bounding e posizione suoni
		soundBoundingSphere.Create("soundBoundingSphere");

		//Matrici iniziali
		float ratio = (float)mainWidth / (float)mainHeight;
		D3DXMatrixIdentity(&matrixIdentity);
		D3DXMatrixPerspectiveFovLH(&matrixProj, ScenePerspective.Perspective_Angle, ratio, ScenePerspective.Perspective_Near, ScenePerspective.Perspective_Far);
		//FOV
		sFOV = DEUtility::SetupFOVClipPlanes(ScenePerspective.Perspective_Angle, ScenePerspective.Perspective_Near, ScenePerspective.Perspective_Far, ratio);

		//Pannelli 16/9
		sceneUseCinematic = false;
		sceneCinematicPanel.Create("sceneCinematicPanel");
		sceneCinematicPanel.SetTextureID( sceneCinematicPanel.AddTexture("Textures\\Panels\\Cine.bmp", D3DXCOLOR(0,0,0,0)) );
		sceneCinematicPanel.SetMode(false);

		//Puntatore mouse
		if (sceneCursor.Create("sceneCursor"))
		{
			for (int i=0; i<50; i++)
			{
				if (sceneProperties.Scene_Cursor_Filename[i] != NULL)
				{
					sceneCursor.AddTexture(sceneProperties.Scene_Cursor_Filename[i], D3DXCOLOR(0,0,0,0) );
					//E' stata aggiunta almeno una texture dunque è stato creato tutto OK
					sceneCursorEnable = true;
				}
			}
			sceneCursor.SetMode(false);
		}
		else
		{
			sceneCursorEnable = false;
		}


		//Funzione definita da utente
		if (sceneProperties.Scene_FuncPointer_Load!=NULL)
		{
			if ((* sceneProperties.Scene_FuncPointer_Load) ()==true)
			{
				bLoaded = true;
			}
		}
	}
	catch(...)
	{
		bLoaded = false;
	}
	return bLoaded;
}


DLL_API bool DEScene::Execute3D()
{
	try
	{
		//Impostazioni camera corrente
		D3DXVECTOR3 currentCamPosition = camerasList[currentCameraID]->GetPosition();
		D3DXVECTOR3 currentCamView = camerasList[currentCameraID]->GetView();

		//Setta le matrici di generali
		DECore::D3DDevice->SetTransform(D3DTS_PROJECTION, &this->GetProjectionMatrix());
		DECore::D3DDevice->SetTransform(D3DTS_WORLD, &matrixIdentity);
	
		//Funzione definita da utente 
		if (sceneProperties.Scene_FuncPointer_Render3D!=NULL)
		{
			(* sceneProperties.Scene_FuncPointer_Render3D) ();
		}//*/

   		//SkyBoxes
		for (unsigned int s=0; s< totalSky; s++)
		{
			DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
			DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			DECore::D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE );
			DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE );
			DECore::D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
			skyList.at(s)->SetPosition(currentCamPosition);
			skyList.at(s)->Render();
		}
				
		//Abilita lo Z...valido da qui in poi
		DECore::D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );

		//LensFlares
		for (unsigned int f=0; f< totalFlares; f++)
		{
			flaresList.at(f)->SetCamera(currentCamPosition, currentCamView);
			flaresList.at(f)->CalculateOcclusion(modelsList);
			flaresList.at(f)->CalculatePosition();
			flaresList.at(f)->Render3D();
		}		
		
		//*** Luci ***
		totalActiveLights = 0;
		for (unsigned int lo=0; lo< totalLight; lo++)
		{
			if (lightList.at(lo)->GetLightProperties().Light_Type == 1)
			{
				//Calcolo dinamico luci presenti in scena
				//La luce non deve essere accesa perchè definito dall'utente
				if (lightList.at(lo)->GetEnable() == false)
				{
					lightList.at(lo)->SetDinamicEnable(false);
				}
				else
				{
					lightList.at(lo)->CheckPosition(currentCamView, sFOV);
					if (lightList.at(lo)->GetCulled() == true)
					{
						lightList.at(lo)->SetDinamicEnable(false);
					}
					else
					{
						//La luce e dentro il frustrum
						lightList.at(lo)->CheckDistance(currentCamPosition);
						if (lightList.at(lo)->GetFar())
						{
							lightList.at(lo)->SetDinamicEnable(false);
						}
						else
						{
							lightList.at(lo)->SetDinamicEnable(true);
							totalActiveLights = totalActiveLights + 1;
						}
					}
				}

				//Disegna le luci (se definito)
				//Se ABILITATA il colore sarà GIALLO
				//Se NON ABILITATA il colore sarà ROSSO
				//Se NON ABILITATA dinamicamente il colore sarà BLU
				if (SceneDebug.Debug_ShowLights)
				{
					DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
					DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
					DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
					lightBoundingSphere.CalculateMatrix(lightList.at(lo)->CalculateOccupationMatrix());
					if (lightList.at(lo)->GetEnable()==false)
					{
						lightBoundingSphere.SetColor(1.0f, 0.0f, 0.0f);	
					}
					else
					{
						if (lightList.at(lo)->GetDinamicEnable())
						{
							lightBoundingSphere.SetColor(1.0f, 1.0f, 0.0f);
						}
						else
						{
							lightBoundingSphere.SetColor(0.0f, 0.0f, 1.0f);
						}
					}
					lightBoundingSphere.Render();
				}	
			}
			else
			{
				//Luce direzionale...sempre attiva
				totalActiveLights = totalActiveLights + 1;
			}
		}//*/


		
		//*** Triggers ***
		if (SceneDebug.Debug_ShowTriggers)
		{
			DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
			for (unsigned int t=0; t< totalTriggers; t++)
			{
				if (scenePhysXTriggerActor.at(t))
				{
					NxVec3 pos = scenePhysXTriggerActor.at(t)->getGlobalPosition();
					D3DXVECTOR3 vector;
					D3DXVECTOR3 dim;
					vector.x = pos.x;
					vector.y = pos.y;
					vector.z = pos.z;

					NxShape*const* shape = scenePhysXTriggerActor.at(t)->getShapes();
					if (NxBoxShape* boxShape = shape[0]->isBox())
					{
						NxBoxShapeDesc boxDesc;
						boxShape->saveToDesc(boxDesc);
						dim.x = boxDesc.dimensions.x;
						dim.y = boxDesc.dimensions.y;
						dim.z = boxDesc.dimensions.z;
 						triggerBoundingBox.SetColor(1.0f, 0.5f, 1.0f);
						triggerBoundingBox.CalculateMatrix(vector, dim, D3DXVECTOR3(0,0,0));
						triggerBoundingBox.Render();
					}
					else
					{
						if (NxSphereShape* sphereShape = shape[0]->isSphere())
						{
							NxSphereShapeDesc sphereDesc;
							sphereShape->saveToDesc(sphereDesc);
							dim.x = sphereDesc.radius;
							dim.y = sphereDesc.radius;
							dim.z = sphereDesc.radius;
							triggerBoundingSphere.SetColor(1.0f, 0.5f, 1.0f);
							triggerBoundingSphere.CalculateMatrix(vector, dim, D3DXVECTOR3(0,0,0));
							triggerBoundingSphere.Render();
						}
					}
				}
			}
		}


		//*** SUONI ***
		//Disegna la zona di influenza del suono in base alla sua DistanzaMassima
		if (SceneDebug.Debug_ShowSounds)
		{
			for (unsigned int s=0; s< totalSounds; s++)
			{
				float max;
				float min;
				FMOD_VECTOR pos;
				//Esegue solo se è 3D, cioè sono impostate le proprietà del 3D
				if (soundsList.at(s)->Get3DMinMaxDistance(&min,&max))
				{
					DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
					DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
					DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

					soundsList.at(s)->Get3DAttributes(&pos, NULL);
					D3DXVECTOR3 soundPos;
					soundPos.x = pos.x;
					soundPos.y = pos.y;
					soundPos.z = pos.z;
					bool isPlaying;
					bool isVirtual;
					soundsList.at(s)->GetIsPlaying(&isPlaying);
					//Si sta eseguendo il suono
					if (isPlaying)
					{
						soundsList.at(s)->GetIsVirtual(&isVirtual);
						if (isVirtual==false)
						{
							//MIN BoundignSphere
							soundBoundingSphere.CalculateMatrix(soundPos,D3DXVECTOR3(min,min,min),D3DXVECTOR3(0,0,0));
							soundBoundingSphere.SetColor(0.05f,1.0f,0.05f);
							soundBoundingSphere.Render();
							//MAX BoundignSphere
							soundBoundingSphere.CalculateMatrix(soundPos,D3DXVECTOR3(max,max,max),D3DXVECTOR3(0,0,0));
							soundBoundingSphere.SetColor(0.0f,100,0.0f);
							soundBoundingSphere.Render();
						}
						else
						{
							//MIN BoundignSphere
							soundBoundingSphere.CalculateMatrix(soundPos,D3DXVECTOR3(min,min,min),D3DXVECTOR3(0,0,0));
							soundBoundingSphere.SetColor(1.0f,0.5f,0.05f);
							soundBoundingSphere.Render();
							//MAX BoundignSphere
							soundBoundingSphere.CalculateMatrix(soundPos,D3DXVECTOR3(max,max,max),D3DXVECTOR3(0,0,0));
							soundBoundingSphere.SetColor(100,0.0f,0.0f);
							soundBoundingSphere.Render();
						}
					}
				}
			}//*/
		}


		//Matrici generali
		D3DXMATRIX m_matProj;
		D3DXMATRIX m_matView;
		DECore::D3DDevice->GetTransform(D3DTS_PROJECTION,&m_matProj);
		DECore::D3DDevice->GetTransform(D3DTS_VIEW,&m_matView);
		//Azzereramento calcolo vertici/faccie
		totalVerticies = 0;
		totalVerticiesCurrent = 0;
		totalFaces = 0;
		totalFacesCurrent = 0;
		totalVerticiesPassCurrent = 0;
		totalFacesPassCurrent = 0;
		//Rendering Modelli
		for (unsigned int m=0; m< totalModels; m++)
		{
			//Abilita lo Z...valido da qui in poi
			DECore::D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE );
			DECore::D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
			DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );

			//Calcola le matrici del modello
			modelsList.at(m)->CalculateMatrix();
			modelsList.at(m)->CalculatePosition(currentCamView, currentCamPosition, sFOV);
			//Bounding
			unsigned short currentBounding = 0;
			if (SceneDebug.Debug_BoundingType == -1)
			{
				currentBounding = modelsList.at(m)->GetModelProperties().Model_Debug_BoundingType;	//Dipendente dal modello
			}
			else
			{
				if (SceneDebug.Debug_BoundingType == 0)		currentBounding = 0;
				if (SceneDebug.Debug_BoundingType == 1)		currentBounding = 1;
				if (SceneDebug.Debug_BoundingType == 2)		currentBounding = 2;
			}
			if (currentBounding==1)
			{
				DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
				if (modelsList.at(m)->GetPhysXActor()->isSleeping())	modelBoundingBox.SetColor(0.0f, 0.0f, 1.0f);
				else													modelBoundingBox.SetColor(1.0f, 1.0f, 1.0f);
				modelBoundingBox.CalculateMatrix(modelsList.at(m)->GetModelInfo().Info_BoundingBoxMatrix);
				modelBoundingBox.Render();
			}
			if (currentBounding==2)
			{
				DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
				if (modelsList.at(m)->GetPhysXActor()->isSleeping())	modelBoundingSphere.SetColor(0.0f, 0.0f, 1.0f);
				else													modelBoundingSphere.SetColor(1.0f, 1.0f, 1.0f);
				modelBoundingSphere.CalculateMatrix(modelsList.at(m)->GetModelInfo().Info_BoundingSphereMatrix);
				modelBoundingSphere.Render();
			}


			//Assi
			unsigned short currentShowAxes = 0;
			if (SceneDebug.Debug_ShowAxes == -1)
			{
				currentShowAxes = modelsList.at(m)->GetModelProperties().Model_Debug_ShowAxes;	//Dipendente dal modello
			}	
			else
			{
				if (SceneDebug.Debug_ShowAxes == 0)		currentShowAxes = 0;
				if (SceneDebug.Debug_ShowAxes == 1)		currentShowAxes = 1;
			}
			if (currentShowAxes == 1)
			{
				DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
				modelAxes.CalculateMatrix(modelsList.at(m)->GetModelInfo().Info_AxesMatrix);
				modelAxes.Render();
			}
				
			
			//FillMode
			unsigned short currentFillMode = 1;
			if (SceneDebug.Debug_FillMode == -1)
			{
				currentFillMode = modelsList.at(m)->GetModelProperties().Model_Debug_FillMode;//Dipendente dal modello (da vedere)
			}
			else
			{
				if (SceneDebug.Debug_FillMode == 0 )	currentFillMode = 0;
				if (SceneDebug.Debug_FillMode == 1 )	currentFillMode = 1;
				if (SceneDebug.Debug_FillMode == 2 )	currentFillMode = 2;
			}
			if (currentFillMode==0) DECore::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
			if (currentFillMode==1) DECore::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			if (currentFillMode==2) DECore::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT );

			//Nebbia
			if (SceneFog.Fog_Enable)
			{
				float fogColor[3] = {SceneFog.Fog_Color.r, SceneFog.Fog_Color.g, SceneFog.Fog_Color.b};
				DECore::engineEffect->SetFloatArray("fogColor", fogColor, 3);
				DECore::engineEffect->SetFloat("fogRange", SceneFog.Fog_Range);
				DECore::engineEffect->SetFloat("fogStart", SceneFog.Fog_Start);
				DECore::engineEffect->SetBool("fogEnable", TRUE);
			}
			else
			{
				DECore::engineEffect->SetBool("fogEnable", FALSE);
			}//*/

			//Ambient
			float ambientCol[4] = {SceneGeneral.General_AmbientColor.r, SceneGeneral.General_AmbientColor.g, SceneGeneral.General_AmbientColor.b, SceneGeneral.General_AmbientColor.a};
			DECore::engineEffect->SetFloatArray("ambientCol", ambientCol, 4);		

			//Textures
			DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(modelsList.at(m)->GetModelInfo().Info_TextureID[0]).Texture_Pointer);
			DECore::engineEffect->SetTexture("texture1", DETextureManager::GetTexture(modelsList.at(m)->GetModelInfo().Info_TextureID[1]).Texture_Pointer);
			DECore::engineEffect->SetTexture("textureNormals", DETextureManager::GetTexture(modelsList.at(m)->GetModelInfo().Info_TextureID[2]).Texture_Pointer);
			DECore::engineEffect->SetFloat("textureBlendFactor", modelsList.at(m)->GetModelProperties().Model_Texture_BlendFactor);		

			//Posizioni
			float cameraPos[3] = {currentCamPosition.x, currentCamPosition.y, currentCamPosition.z};
			DECore::engineEffect->SetFloatArray("cameraPos", currentCamPosition, 3);		
			
			//Matrici
			DECore::engineEffect->SetMatrix("matProj", &m_matProj);
			DECore::engineEffect->SetMatrix("matView", &m_matView);
			DECore::engineEffect->SetMatrix("matWorld", &modelsList.at(m)->GetModelInfo().Info_FinalMatrix);
			D3DXMATRIX modelWorldViewProj = modelsList.at(m)->GetModelInfo().Info_FinalMatrix * m_matView * m_matProj;
			DECore::engineEffect->SetMatrix("matWorldViewProj", &modelWorldViewProj);
			
			//Effetti
			DECore::engineEffect->SetBool("useBump", modelsList.at(m)->GetMaterial().Material_IsBumpmapped);
			DECore::engineEffect->SetBool("useSpecular", modelsList.at(m)->GetMaterial().Material_IsSpecular);
			DECore::engineEffect->SetBool("useTransparent", modelsList.at(m)->GetMaterial().Material_IsTransparent);
			DECore::engineEffect->SetBool("useEnvironment", modelsList.at(m)->GetMaterial().Material_IsEnvironmental);
			
			//Reazione alla luce (materiale)
			float matAmbient[4] = {modelsList.at(m)->GetMaterial().Material_Ambient.r, modelsList.at(m)->GetMaterial().Material_Ambient.g, modelsList.at(m)->GetMaterial().Material_Ambient.b, modelsList.at(m)->GetMaterial().Material_Ambient.a};
			float matDiffuse[4] = {modelsList.at(m)->GetMaterial().Material_Diffuse.r, modelsList.at(m)->GetMaterial().Material_Diffuse.g, modelsList.at(m)->GetMaterial().Material_Diffuse.b, modelsList.at(m)->GetMaterial().Material_Diffuse.a};
			DECore::engineEffect->SetFloatArray("ambientMat", matAmbient, 4);		
			DECore::engineEffect->SetFloatArray("diffuseMat", matDiffuse, 4);		
			DECore::engineEffect->SetFloat("shininess", modelsList.at(m)->GetMaterial().Material_Shininess);		
			DECore::engineEffect->SetFloat("alpha", modelsList.at(m)->GetMaterial().Material_Alpha);		

													
			//*** LUCI ***
			//Esegue solo se il fillmode è SOLID
			if (currentFillMode==0)
			{
				//Esegue le luci (se specificato)
				unsigned short currentUseLighting = 1;
				if (SceneDebug.Debug_UseLighting == -1)
				{
					currentUseLighting = 1;//Dipende dal modello
				}
				else
				{
					if (SceneDebug.Debug_UseLighting == 0)
					{
						currentUseLighting = 0;
					}
					if (SceneDebug.Debug_UseLighting == 1)
					{
						currentUseLighting = 1; 
					}
				}
				//E' stata abilitata la luce
				if (currentUseLighting == 1)
				{
					//Luci
					unsigned int ownLight = 0;
					for (unsigned int lo=0; lo< totalLight; lo++)
					{
						//la luce è abilitata
						if ((lightList.at(lo)->GetLightProperties().Light_Enable)&&(lightList.at(lo)->GetLightProperties().Light_DinamicEnable))
						{	
							//Il modello e la luce si influenzano
							if (DEUtility::SphereInSphere(modelsList.at(m)->GetPosition(), modelsList.at(m)->GetModelInfo().Info_Radius_Scaled, lightList.at(lo)->GetLightProperties().Light_Position, lightList.at(lo)->GetLightProperties().Light_Range))
							{
								//Luci che influenzano il modello aumentate di 1
								ownLight = ownLight + 1;
								
								//Luce corrente 
								float lightPos[4] = { lightList.at(lo)->GetLightProperties().Light_Position.x, lightList.at(lo)->GetLightProperties().Light_Position.y, lightList.at(lo)->GetLightProperties().Light_Position.z, 1.0f };
								float lightDir[4] = { lightList.at(lo)->GetLightProperties().Light_Direction.x, lightList.at(lo)->GetLightProperties().Light_Direction.y, lightList.at(lo)->GetLightProperties().Light_Direction.z, 1.0f };
								float lightCol[4] = { lightList.at(lo)->GetLightProperties().Light_Color.r, lightList.at(lo)->GetLightProperties().Light_Color.g, lightList.at(lo)->GetLightProperties().Light_Color.b, lightList.at(lo)->GetLightProperties().Light_Color.a };
								DECore::engineEffect->SetFloatArray("lightPos", lightPos, 4);
								DECore::engineEffect->SetFloatArray("lightDirection", lightDir, 4);
								DECore::engineEffect->SetFloatArray("lightColor", lightCol, 4);
								DECore::engineEffect->SetFloat("lightFalloff", lightList.at(lo)->GetLightProperties().Light_Falloff);
								
								DECore::engineEffect->SetInt("lightCurrent", ownLight);

								//Luce direzionale (se specificato e se abilitata)
								if (modelsList.at(m)->GetMaterial().Material_UseDirectionalLights)
								{
									if (lightList.at(lo)->GetLightProperties().Light_Type == DE_LIGHTTYPE_PERPIXELDIRECTIONAL)
									{
										DECore::engineEffect->SetTechnique("PerPixel_Directional");
									}
								}
								//Luce point (se specificato e se abilitata)
								bool prova = modelsList.at(m)->GetMaterial().Material_UsePointLights;
								if (modelsList.at(m)->GetMaterial().Material_UsePointLights)
								{
									if (lightList.at(lo)->GetLightProperties().Light_Type == DE_LIGHTTYPE_PERPIXELPOINT)
									{
										DECore::engineEffect->SetTechnique("PerPixel_Point");
									}
								}

								if (ownLight==1)	//Se è la prima luce allora il modello non è trasparente
								{
									//Materiale trasparente
									if (modelsList.at(m)->GetMaterial().Material_IsTransparent)
									{								
										DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
										DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE  );										
										DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
										DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
										DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
									}
									else
									{
										DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
										DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
										DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
										DECore::D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
										DECore::D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
									}
								}
								else
								{
									DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
									DECore::D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE );
									DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE );
									DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
									DECore::D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
									DECore::D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
								}

								//***************************************************************
								//Si potrebbe pensare di disegnare il primo modello con il massimo dei vertici
								//e i modelli successivi disegnati dalle luci con un numero di vertici inferiori
								//***************************************************************
								modelsList.at(m)->Render(); 

								//Calcolo dei vertici effettivamente disegnati dai passaggi delle luci
								if (modelsList.at(m)->GetModelInfo().Info_IsCulled==false)
								{
									totalVerticiesPassCurrent = totalVerticiesPassCurrent + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumVertices();
									totalFacesPassCurrent = totalFacesPassCurrent + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumFaces();
								}		
							}
						}
					}

					//Non ci sono luci che influenzano il modello, lo disegna con l'ambient solamente
					if (ownLight == 0)
					{
						if (modelsList.at(m)->GetMaterial().Material_IsTransparent)
						{										
							DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
							DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );										
							DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
							DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
						}
						else
						{
							DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
							DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
							DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
							DECore::D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
							DECore::D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
						}

						DECore::engineEffect->SetTechnique("Ambient");
						modelsList.at(m)->Render();
						//Calcolo dei vertici effettivamente disegnati dai passaggi delle luci
						if (modelsList.at(m)->GetModelInfo().Info_IsCulled==false)
						{
							totalVerticiesPassCurrent = totalVerticiesPassCurrent + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumVertices();
							totalFacesPassCurrent = totalFacesPassCurrent + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumFaces();
						}	
					}
				}
			}//*/

			//Calcolo Vertici/Faccie Totali
			totalVerticies = totalVerticies + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumVertices();
			totalFaces = totalFaces + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumFaces();
			//Vertici Correnti
			if (modelsList.at(m)->GetModelInfo().Info_IsCulled==false)
			{
				totalVerticiesCurrent = totalVerticiesCurrent + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumVertices();
				totalFacesCurrent = totalFacesCurrent + DEMeshManager::GetMesh(modelsList.at(m)->GetModelInfo().Info_MeshID).Mesh_Point->GetNumFaces();
			}
		}//*/


		//Reimposta lo Z
		DECore::D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );

		//Renderizza i billboard delle luci
		for (unsigned int lo=0; lo< totalLight; lo++)
		{
			lightList.at(lo)->Render(currentCamPosition, sFOV);
		}//*/

		//LensFlares
		for (unsigned int f=0; f< totalFlares; f++)
		{
			flaresList.at(f)->SetCamera(currentCamPosition, currentCamView);
			flaresList.at(f)->CalculateOcclusion(modelsList);
			flaresList.at(f)->CalculatePosition();
			flaresList.at(f)->Render3D();
		}
		//Griglia
		if (SceneDebug.Debug_ShowGroundGrid)
		{
			DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			mainGrid.CalculateMatrix(D3DXVECTOR3(0,0,0), D3DXVECTOR3(1,1,1), D3DXVECTOR3(0,0,0));
			mainGrid.Render();
		}
		//Assi
		if (SceneDebug.Debug_ShowCenterAxes)
		{
			DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			mainAxes.CalculateMatrix(D3DXVECTOR3(0,0,0), D3DXVECTOR3(25,25,25), D3DXVECTOR3(0,0,0));		
			mainAxes.Render();
		}
		//Spatial grid
		if (SceneDebug.Debug_ShowSpatialGrid)
		{
			DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			spatialGrid.Render(false);
		}

		//Sistemi Particellari
		for (unsigned int p=0; p< totalParticles; p++)
		{
			particlesList.at(p)->Render();
		}

	}
	catch(...)
	{
		DELog::LogError("<li>Unknown 3D runtime '%s' error", mainLogTitle);
		return false;
	}
	return true;
}

DLL_API bool DEScene::Execute2D()
{
	try
	{
		//Calcola le matrici globali in base alla nuova scena	
		D3DXMatrixOrthoLH(&matrixOrtho, (float)mainWidth, (float)mainHeight, 0.0f, 1.0f);
		DECore::D3DDevice->SetTransform(D3DTS_PROJECTION, &matrixOrtho);
		DECore::D3DDevice->SetTransform(D3DTS_VIEW, &matrixIdentity);
		DECore::D3DDevice->SetTransform(D3DTS_WORLD, &matrixIdentity);

		//RenderStates x l'esecuzione 2d
		DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		DECore::D3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE);
		DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);
		DECore::D3DDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL  );

		//Funzione definita da utente
		if (sceneProperties.Scene_FuncPointer_Render2D!=NULL)
		{
			(* sceneProperties.Scene_FuncPointer_Render2D) ();
		}

		//LensFlares
		for (unsigned int f=0; f< totalFlares; f++)
		{
			flaresList.at(f)->Render2D();
		}
		//Pannelli
		for (unsigned int p=0; p< totalPanels; p++)
		{
			panelsList.at(p)->Render();
		}
		//Fonts
		for (unsigned int f=0; f< totalFonts; f++)
		{
			fontsList.at(f)->BeginTextSprite();
			fontsList.at(f)->Render();
			fontsList.at(f)->EndTextSprite();
		}

		//Renderizza i pannelli 16/9
		if (sceneUseCinematic)
		{
			//Per calcolare i 16/9 effettivi
			float rapHeight = ((float)mainHeight - ((float)mainWidth / (16.0f / 9.0f)))/2.0f;
			sceneCinematicPanel.SetDimensions(D3DXVECTOR2((float)mainWidth, rapHeight));		
			sceneCinematicPanel.SetPosition(D3DXVECTOR2(0,0));
			sceneCinematicPanel.Render();
			sceneCinematicPanel.SetPosition(D3DXVECTOR2(0, mainHeight - rapHeight));
			sceneCinematicPanel.Render();
		}

		//Cursore
		if ((sceneCursorEnable)&&(sceneCursor.GetCreated()))
		{
			sceneCursor.SetDimensions(D3DXVECTOR2(sceneCursor.GetTextureInfoID(sceneCursor.GetTextureID()).x, sceneCursor.GetTextureInfoID(sceneCursor.GetTextureID()).y)  );
			sceneCursor.SetPosition(D3DXVECTOR2(sceneCursorPosition.x, sceneCursorPosition.y));
			sceneCursor.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			sceneCursor.Render();
		}//*/

	}
	catch(...)
	{
		DELog::LogError("<li>Unknown 2D runtime '%s' error", mainLogTitle);	
		return false;
	}

	return true;
}



DLL_API bool DEScene::Execute2DInfo()
{
	//Info
	if (SceneDebug.Debug_ShowInfo)
	{
		float infoX = (float)mainWidth - 260;
		float infoYvar = 15;
		float infoY=5;
		char bufferInfo[MAX_PATH]="";

		//PhysX Stats
		NxSceneStats stat;
		scenePhysX->getStats(stat);

		mainFont.BeginTextSprite();
		mainFont.SetColor(D3DCOLOR_XRGB(255, 255, 138));

		sprintf(bufferInfo, "Name: %s",this->GetName());
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*0), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "Angle: %.2f",(float)ScenePerspective.Perspective_Angle);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*1), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "AspectRatio: %.2f",(float)((float)mainWidth / (float)mainHeight));
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*2), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Far: %.2f",(float)ScenePerspective.Perspective_Far);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*3), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Near: %.2f",(float)ScenePerspective.Perspective_Near);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*4), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Cameras: %d",(int)totalCameras);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*5), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Models: %d",(int)totalModels);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*6), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Panels: %d",(int)totalPanels);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*7), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total PSystems: %d",(int)totalParticles);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*8), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total SoundGroups: %d",(int)totalSoundsGroup);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*9), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Sounds: %d",(int)totalSounds);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*10), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total LensFlares: %d",(int)totalFlares);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*11), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Sky: %d",(int)totalSky);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*12), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Fonts: %d",(int)totalFonts);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*13), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Lights(Act/Tot): %d/%d",(int)totalActiveLights, totalLight);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*14), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Cells Visible/Total: %d/%d",(int)spatialGrid.GetCellsInFrustrum(),(int)spatialGrid.GetTotalCells());
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*15), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Camera Current: %d",(int)currentCameraID);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*16), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Camera Cell: %d",(int)spatialGridPosition);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*17), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Camera X: %.2f",(float)camerasList.at(currentCameraID)->GetPosition().x);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*18), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Camera Y: %.2f",(float)camerasList.at(currentCameraID)->GetPosition().y);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*19), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Camera Z: %.2f",(float)camerasList.at(currentCameraID)->GetPosition().z);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*20), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Verts: %d",(int)totalVerticies);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*21), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Faces: %d",(int)totalFaces);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*22), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Visible Verts: %d/%d",(int)totalVerticiesCurrent, (int)totalVerticiesPassCurrent);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*23), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "Total Visible Faces: %d/%d",(int)totalFacesCurrent, (int)totalFacesPassCurrent);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*24), 0, 0, DT_NOCLIP);
			mainFont.Render();
		if (scenePostProcessEnable)	sprintf(bufferInfo, "PPTech: %s/On", scenePostProcessTechnique);
		else						sprintf(bufferInfo, "PPTech: %s/Off", scenePostProcessTechnique);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*25), 0, 0, DT_NOCLIP);
			mainFont.Render();
		if (sceneUseCinematic)	sprintf(bufferInfo, "Cinematic Effect: On");
		else					sprintf(bufferInfo, "Cinematic Effect: Off");
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*26), 0, 0, DT_NOCLIP);
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_Actors: %d/%d",(int)stat.numActors,(int)stat.maxActors);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*27), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_DynActors: %d/%d",(int)stat.numDynamicActors,(int)stat.maxDynamicActors);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*28), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_DynActorsInAwakeGrp: %d/%d",(int)stat.numDynamicActorsInAwakeGroups,(int)stat.maxDynamicActorsInAwakeGroups);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*29), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_StaticShapes: %d/%d",(int)stat.numStaticShapes,(int)stat.maxStaticShapes);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*30), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_DynShapes: %d/%d",(int)stat.numDynamicShapes,(int)stat.maxDynamicShapes);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*31), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_AxisConstraints: %d/%d",(int)stat.numAxisConstraints,(int)stat.maxAxisConstraints);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*32), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_Contacts: %d/%d",(int)stat.numContacts,(int)stat.maxContacts);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*33), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_Joints: %d/%d",(int)stat.numJoints,(int)stat.maxJoints);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*34), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_Pairs: %d/%d",(int)stat.numPairs,(int)stat.maxPairs);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*35), 0, 0, DT_NOCLIP);	
			mainFont.Render();
		sprintf(bufferInfo, "PhysX_SolverBodies: %d/%d",(int)stat.numSolverBodies,(int)stat.maxSolverBodies);
			mainFont.SetText(bufferInfo);
			mainFont.SetRectSprite(infoX, infoY + (infoYvar*36), 0, 0, DT_NOCLIP);	
			mainFont.Render();

		mainFont.EndTextSprite();
	}
	return true;
}


/*******************************
/*            LUCI             *
/*******************************/
DLL_API unsigned int DEScene::AddLight(DELight *newLight)
{
	try
	{
		lightList.push_back(newLight);
		totalLight = totalLight + 1;
		return (unsigned int)lightList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}




/*******************************
/*           CAMERA            *
/*******************************/
DLL_API unsigned int DEScene::AddCamera(DECamera *newCamera)
{
	try
	{
		camerasList.push_back(newCamera);
		totalCameras = totalCameras + 1;
		return (unsigned int)camerasList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}
DLL_API bool DEScene::SetCurrentCamera(unsigned int newCurrentCameraID)
{
	if ((newCurrentCameraID>=0)&&(newCurrentCameraID<totalCameras))
	{
		currentCameraID = newCurrentCameraID;
		return true;
	}
	else
	{
		return false;
	}
	return false;
}
DLL_API unsigned int DEScene::GetCurrentCameraID()
{
	return currentCameraID;
}
DLL_API DECamera *DEScene::GetCurrentCamera()
{
	return camerasList.at(currentCameraID);
}


/*******************************
/*            MODELS           *
/*******************************/
DLL_API bool DEScene::AddModel(DEModel *newModel)
{
	try
	{
		newModel->SetSpatialGrid(&spatialGrid);
		newModel->SetArrayID((int)modelsList.size());
		newModel->SetPhysicsScene(scenePhysX, sceneCompartment, (unsigned int)modelsList.size());

		modelsList.push_back(newModel);
		totalModels = totalModels + 1;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}


DLL_API bool DEScene::RemoveModel(DEModel *removeModel)
{
	try
	{
		if (removeModel->GetArrayID() != -1)
		{
			int IDtoRemover = removeModel->GetArrayID();
			modelsList.erase(modelsList.begin( ) + IDtoRemover);
			for (unsigned int i=IDtoRemover; i< modelsList.size(); i++)
			{
				modelsList[i]->SetArrayID(modelsList[i]->GetArrayID() - 1);
			}
			totalModels = totalModels - 1;
			removeModel->SetArrayID(-1);
			return true;
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

/*******************************
/*            PANELS           *
/*******************************/
DLL_API unsigned int DEScene::AddPanel(DEPanel *newPanel)
{
	try
	{
		panelsList.push_back(newPanel);
		totalPanels = totalPanels + 1;
		return (unsigned int)panelsList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}



/*******************************
/*     SISTEMI PARTICELLARI    *
/*******************************/
DLL_API unsigned int DEScene::AddParticlesSystem(DEParticleSystem *newParticlesSystem)
{
	try
	{
		particlesList.push_back(newParticlesSystem);
		totalParticles = totalParticles + 1;
		return (unsigned int)particlesList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}



/*******************************
/*            SUONI            *
/*******************************/
DLL_API unsigned int DEScene::AddSound(DESound *newSound)
{
	try
	{
		soundsList.push_back(newSound);
		totalSounds = totalSounds + 1;
		return (unsigned int)soundsList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}


/*******************************
/*         GRUPPI SUONI        *
/*******************************/
DLL_API unsigned int DEScene::AddSoundGroup(DESoundChannelGroup *newSoundGroup)
{
	try
	{
		soundsGroupList.push_back(newSoundGroup);
		totalSoundsGroup = totalSoundsGroup + 1;
		return (unsigned int)soundsGroupList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}


/*******************************
/*         LENS FLARES         *
/*******************************/
DLL_API unsigned int DEScene::AddLensFlare(DELensFlare *newLensFlare)
{
	try
	{
		flaresList.push_back(newLensFlare);
		totalFlares = totalFlares + 1;
		return (unsigned int)flaresList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}


/*******************************
/*         SKY BOXES           *
/*******************************/
DLL_API unsigned int DEScene::AddSky(DESky *newSky)
{
	try
	{
		skyList.push_back(newSky);
		totalSky = totalSky + 1;
		return (unsigned int)skyList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}




/*******************************
/*            FONT             *
/*******************************/
DLL_API unsigned int DEScene::AddFont(DEFont *newFont)
{
	try
	{
		fontsList.push_back(newFont);
		totalFonts = totalFonts + 1;
		return (unsigned int)fontsList.size()-1;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}


/*******************************
/*          CREAZIONE          *
/*******************************/
DLL_API bool DEScene::Create(STRUCT_DE_SCENE newSceneProperties, LPCSTR logTitle)
{
	bCreated = false;
	bLoaded = false;

	mainLogTitle = logTitle;
	DELog::LogInfo("<br><b>%s Initialization</b>", mainLogTitle);
	try
	{
		//Proprietà Scena
		sceneProperties = newSceneProperties;

		//Default render states
		CreateDefaultRenderstatesBlocks();

		//Impostazioni di default
		//Clear
		SceneClear.Clear_RectsCount = 0;
		SceneClear.Clear_Rects = NULL;
		SceneClear.Clear_ClearFlags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
		SceneClear.Clear_EnvironmentColor = D3DCOLOR_XRGB(161,161,161);
		SceneClear.Clear_Z = 1.0f;
		SceneClear.Clear_Stencil = 0;
		//Debug
		SceneDebug.Debug_ShowCenterAxes = false;
		SceneDebug.Debug_ShowInfo = false;
		SceneDebug.Debug_ShowGroundGrid= false;
		SceneDebug.Debug_ShowSpatialGrid = false;
		SceneDebug.Debug_ShowLights = false;
		SceneDebug.Debug_ShowSounds = false;
		SceneDebug.Debug_ShowTriggers = false;
		SceneDebug.Debug_FillMode = -1;				//-1=No overwrite, 0=Solid, 1=Wireframe, 2=Point 
		SceneDebug.Debug_UseLighting = -1;			//-1=No overwrite, 0=No lighting, 1=Force lighting
		SceneDebug.Debug_BoundingType = -1;			//-1=No overwrite, 0=No bounding, 1=Box, 2=Sphere
		SceneDebug.Debug_ShowAxes = -1;				//-1=No overwrite, 0=No axes, 1=Show axes
		SceneDebug.Debug_UseFrustrumCulling = -1;	//-1=No overwrite, 0=No frustrum cull, 1=Force frustrum cull
		SceneDebug.Debug_UseLOD = -1;				//-1=No overwrite, 0=No lods, 1=Force lods
		//Prospettiva
		ScenePerspective.Perspective_Angle = D3DX_PI/4;
		ScenePerspective.Perspective_Near = 1.0f;
		ScenePerspective.Perspective_Far = 1000.0f;
		//Fog
		SceneFog.Fog_Color = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		SceneFog.Fog_Start = 1.0f;
		SceneFog.Fog_Range = ScenePerspective.Perspective_Far * 3;
		SceneFog.Fog_Enable = false;
		//Griglie
		SceneGrids.GridsProperties_SpatialGrid_DimensionCell = 1000;
		SceneGrids.GridsProperties_SpatialGrid_PerSideCells = 6;
		SceneGrids.GridsProperties_GroundGrid_DimensionCell = 1;
		SceneGrids.GridsProperties_GroundGrid_PerSideCells = 50;
		//Proprietà fisiche
		scenePhysicsProperties.Scene_PhysicsProperties_Ground = true;
		scenePhysicsProperties.Scene_PhysicsProperties_GroundNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		scenePhysicsProperties.Scene_PhysicsProperties_GroundY = 0.0f;
		scenePhysicsProperties.Scene_PhysicsProperties_StepSize = 1.0f/60.0f;
		scenePhysicsProperties.Scene_PhysicsProperties_NumIterations = 8;
		scenePhysicsProperties.Scene_PhysicsProperties_Gravity = D3DXVECTOR3(0.0f, -9.8f, 0.0f);	
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_Restitution = 0.5f;
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_StaticFriction = 0.5f;
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_DynamicFriction = 0.5f;
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV = 0.8f;
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV = 1.0f;
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy = D3DXVECTOR3(0.0f,0.0f,1.0f);
		scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_Flags = NX_MF_ANISOTROPIC;

		NxSceneDesc sceneDesc;
		sceneDesc.groundPlane			= false;
		if (sceneProperties.Scene_Physics_Type == NX_SIMULATION_HW)
		{
			if (DECore::enginePhysX->getHWVersion() != NX_HW_VERSION_NONE)
			{
				sceneDesc.simType = NX_SIMULATION_HW;
			}
			else
			{
				DELog::LogWarning("<li>Scene PhysX forced to Software");		
				sceneDesc.simType = NX_SIMULATION_SW;
				sceneProperties.Scene_Physics_Type = NX_SIMULATION_SW;
			}
		}
		else
		{
			sceneDesc.simType = NX_SIMULATION_SW;
		}
		sceneDesc.gravity               = NxVec3(0,-9.8,0);
		sceneDesc.userContactReport     = &gContactReport;		//Report contatti
		sceneDesc.userTriggerReport		= &gTriggerReport;		//Report triggers
		scenePhysX = DECore::enginePhysX->createScene(sceneDesc);
		if (scenePhysX != NULL)
		{
			//Compartment for rigid body
			sceneCompartment = NULL;
			NxCompartmentDesc cdesc;
			cdesc.type = NX_SCT_RIGIDBODY;
			cdesc.deviceCode = NX_DC_PPU_AUTO_ASSIGN;
			sceneCompartment = scenePhysX->createCompartment(cdesc);
			
			//Setta le proprietà
			SetClearProperties(SceneClear);
			SetFogProperties(SceneFog);
			SetDebugProperties(SceneDebug);
			SetPerspectiveProperties(ScenePerspective);
			SetGridsProperties(SceneGrids);
			SetPhysicsProperties(scenePhysicsProperties);	//Deve essere fatto per forza dopo la creazione del mondo fisico

			//Crea il post-processing
			CreatePostProcessing();

			bCreated=true;
		}
		else
		{
			DELog::LogError("<li>enginePhysX->createScene(...) NOT Created");		
			bCreated=false;
		}

	}
	catch(...)
	{
		DELog::LogError("<li>Unknown statement error");		
	}

	if (bCreated)	DELog::LogInfo("<li>Created successfully");		
	else			DELog::LogError("<li>NOT Created");		

	return bCreated;
}



DLL_API	NxSceneStats DEScene::GetPhysXStats() 
{
	NxSceneStats prova;
	scenePhysX->getStats(prova);
	return prova;
}




DLL_API bool DEScene::CreatePostProcessing( void )
{
	// Create a test texture for our effect to use...
	HRESULT hr;
	LPD3DXBUFFER pBufferErrors = NULL;
	postProcessIsCreated = false;

	try
	{
		char namePath[MAX_PATH];
		strcpy(namePath, DEUtility::GetCurrentPath());
		strcat(namePath, sceneProperties.Scene_PostProcess_Filename);
		hr = D3DXCreateEffectFromFile( DECore::D3DDevice, namePath, NULL, NULL, D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, NULL, &g_pEffect, &pBufferErrors );
		if( hr == D3D_OK )
		{
			// Retrieve info from the current frame buffer
			hr = DECore::D3DDevice->GetRenderTarget(0, &g_pFrameBufferSurface);
			if ( hr == D3D_OK)
			{
				//Get the description for the texture we want to filter
				D3DSURFACE_DESC ddsd;
				hr=g_pFrameBufferSurface->GetDesc(&ddsd);
				if ( hr==D3D_OK )
				{
					//Se si usa D3DUSAGE_RENDERTARGET non si può usare D3DPOOL_MANAGED
					hr = DECore::D3DDevice->CreateTexture(ddsd.Width, ddsd.Height, 1, D3DUSAGE_RENDERTARGET, ddsd.Format, D3DPOOL_DEFAULT,&g_pRenderTargetTexture,0);
					if ( hr==D3D_OK )
					{
						hr = g_pRenderTargetTexture->GetSurfaceLevel(0, &g_pRenderTargetSurface);
						if ( hr==D3D_OK )
						{
							//Guarda se è stato settato il software processing
							DWORD vertexProcessingUsage;
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

							hr = DECore::D3DDevice->CreateVertexBuffer( 4*sizeof(Vertex), vertexProcessingUsage, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pFullScreenVertexBuffer, NULL );
							if (hr==D3D_OK)
							{
								void *pVertices = NULL;
								Vertex g_quadVertices[] =
								{
									{-1.0f, 1.0f, 0.0f,  0.0f,0.0f },
									{ 1.0f, 1.0f, 0.0f,  1.0f,0.0f },
									{-1.0f,-1.0f, 0.0f,  0.0f,1.0f },
									{ 1.0f,-1.0f, 0.0f,  1.0f,1.0f }
								};
								hr = g_pFullScreenVertexBuffer->Lock( 0, sizeof(g_quadVertices), (void**)&pVertices, 0 );
								if (hr==D3D_OK)
								{
									memcpy( pVertices, g_quadVertices, sizeof(g_quadVertices) );
									hr = g_pFullScreenVertexBuffer->Unlock();
									if (hr==D3D_OK)
									{
										//Il postprocess è stato creato, se successivamente la texture non viene creata al max non va un effetto :-)
										postProcessIsCreated = true;

										//Carica la texture perturbatrice
										LPCSTR normalName;
										g_pEffect->GetString("normalFilename", &normalName);
										char nameFileName[MAX_PATH];
										strcpy(nameFileName, DEUtility::GetCurrentPath());
										strcat(nameFileName, normalName);
										hr = D3DXCreateTextureFromFileEx(DECore::D3DDevice, nameFileName, 0,0,0,0, D3DFMT_UNKNOWN,D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,0,NULL,NULL,&g_pNormalTexture);
										if( FAILED(hr) )
										{
											DELog::LogWarning("<li>PostProcess Effect : Normal map %s file not found. Some post-process effect may be corrupted.", normalName);
										}
									}
								}
							}
							else
							{
								DELog::LogError("<li>CreateVertexBuffer(...): %s", DXGetErrorDescription9(hr));		
							}
						}
						else
						{
							DELog::LogError("<li>GetSurfaceLevel(...): %s", DXGetErrorDescription9(hr));		
						}
					}
					else
					{
						DELog::LogError("<li>CreateTexture(...): %s", DXGetErrorDescription9(hr));		
					}
				}
				else
				{
					DELog::LogError("<li>GetDesc(...): %s", DXGetErrorDescription9(hr));		
				}
			}
			else
			{
				DELog::LogError("<li>GetRenderTarget(...): %s", DXGetErrorDescription9(hr));		
			}
		}
		else
		{
			DELog::LogError("<li>D3DXCreateEffectFromFile( %s ): %s", sceneProperties.Scene_PostProcess_Filename, DXGetErrorDescription9(hr));		
		}
	}
	catch(...)
	{
		DELog::LogError("<li>PostProcess Effect : Unknown statement error");		
	}

	if (postProcessIsCreated ==  false)	DELog::LogError("<li>PostProcess Effect : NOT Created");	
	else								DELog::LogInfo("<li>PostProcess Effect : Created successfully");
	return postProcessIsCreated;
}

DLL_API void DEScene::PreparePostProcessing( void )
{
	HRESULT hr;
	hr = DECore::D3DDevice->SetRenderTarget(0, g_pRenderTargetSurface);
	hr = DECore::D3DDevice->Clear(SceneClear.Clear_RectsCount, SceneClear.Clear_Rects, SceneClear.Clear_ClearFlags, SceneClear.Clear_EnvironmentColor, SceneClear.Clear_Z, SceneClear.Clear_Stencil);
	DECore::D3DDevice->BeginScene();

	UpdateView();
	Execute3D();
	//Esegue il 2D dentro il PostProcessing
	if (scenePostProcessInclude2D)
	{
		Execute2D();
	}

	// Restore the render target to the frame buffer to output to the screen
	hr = DECore::D3DDevice->SetRenderTarget(0, g_pFrameBufferSurface);
	assert(hr == D3D_OK);

	DECore::D3DDevice->EndScene();	
}



/*******************************
/*         MOUSE CURSOR        *
/*******************************/
DLL_API void DEScene::SetCursorEnable(bool enable)
{
	sceneCursorEnable = enable;
}
DLL_API bool DEScene::GetCursorEnable()
{
	return sceneCursorEnable;
}
DLL_API void DEScene::SetCursorPosition(D3DXVECTOR2 position)
{
	sceneCursorPosition = position;
}
DLL_API D3DXVECTOR2	DEScene::GetCursorPosition()
{
	return sceneCursorPosition;
}

DLL_API unsigned int DEScene::GetCursorTextureID()
{
	return sceneCursorTextureID;
}
DLL_API void DEScene::SetCursorTextureID(unsigned int textureID)
{
	sceneCursorTextureID = textureID;
	sceneCursor.SetTextureID(textureID);
}




/*******************************
/*         POSTPROCESS         *
/*******************************/
DLL_API void DEScene::SetPostProcessingTechnique(LPCTSTR techniqueName)
{
	strcpy(scenePostProcessTechnique, techniqueName);
}
DLL_API void DEScene::SetPostProcessingEnable(bool enable)
{
	scenePostProcessEnable = enable;
}
DLL_API void DEScene::SetPostProcessingInclude2D(bool include)
{
	scenePostProcessInclude2D = include;
}
DLL_API LPCTSTR	DEScene::GetPostProcessingTechnique()
{
	return scenePostProcessTechnique;
}
DLL_API bool DEScene::GetPostProcessingEnable()
{
	return scenePostProcessEnable;
}
DLL_API bool DEScene::GetPostProcessingInclude2D()
{
	return scenePostProcessInclude2D;
}
DLL_API LPD3DXEFFECT DEScene::GetPostProcessing()
{
	return g_pEffect;
}


DLL_API void DEScene::ExecutePostProcessing()
{
	// Render our textured quad using a Sobel Edge Detection shader...
	g_pEffect->SetTechnique( scenePostProcessTechnique );
	g_pEffect->SetTexture( "offscreenTexture", g_pRenderTargetTexture );
	g_pEffect->SetTexture( "normalTexture", g_pNormalTexture );

	// The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will 
	// save and restore all state modified by the effect.
	UINT uPasses;
	g_pEffect->Begin( &uPasses, 0 );

	for( UINT uPass = 0; uPass < uPasses; ++uPass )
	{
		g_pEffect->BeginPass( uPass );
		//Renderizza il quadrilatero di dimensioni pari allo schermo
		DECore::D3DDevice->SetStreamSource( 0, g_pFullScreenVertexBuffer, 0, sizeof(Vertex) );
		DECore::D3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		DECore::D3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		g_pEffect->EndPass();
	}

	g_pEffect->End();

	//Esegue il 2D fuori dal PostProcessing
	if (scenePostProcessInclude2D==false)
	{
		Execute2D();
	}
}

DLL_API bool DEScene::Execute()
{
	//Prima fase del postprocessing (se abilitato)
	if ((scenePostProcessEnable)&&(postProcessIsCreated))
	{
		PreparePostProcessing();
	}


	//RenderSceneIntoEnvMap(D3DXVECTOR3(0,0,0));




	HRESULT hr = DECore::D3DDevice->Clear(SceneClear.Clear_RectsCount, SceneClear.Clear_Rects, SceneClear.Clear_ClearFlags, SceneClear.Clear_EnvironmentColor, SceneClear.Clear_Z, SceneClear.Clear_Stencil);
	if (hr ==D3D_OK)
	{
		//Inizio scena
		if (DECore::D3DDevice->BeginScene()==D3D_OK)
		{
			if ((scenePostProcessEnable)&&(postProcessIsCreated))
			{
				ExecutePostProcessing();
			}
			else
			{
				UpdateView();
				Execute3D();
				Execute2D();
			}
			//Esegue sempre e cmq le info fuori dagli effetti di post-process
			Execute2DInfo();

			//Fine scena
			DECore::D3DDevice->EndScene();
		}
	}
	return true;
}




/*******************************
/*            PAUSA            *
/*******************************/
DLL_API void DEScene::SetPause(bool pause)
{
	isPaused = pause;
}
DLL_API void DEScene::TogglePause()
{
	isPaused = !isPaused;
}
DLL_API bool DEScene::GetPause()
{
	return isPaused;
}

/******************************
/*            HWND            *
/******************************/
DLL_API void DEScene::SetHWND(HWND associatedHWND)
{
	mainHWND = associatedHWND;
}
DLL_API HWND DEScene::GetHWND()
{
	return mainHWND;
}

/******************************
/*          WINDOWED          *
/******************************/
DLL_API void DEScene::SetWindowed(bool windowed)
{
	mainWindowed = windowed;
}
DLL_API bool DEScene::GetWindowed()
{
	return mainWindowed;
}



/******************************
/*         DIMENSIONI         *
/******************************/
DLL_API void DEScene::SetWidth(unsigned int width)
{
	mainWidth = width;
}
DLL_API unsigned int DEScene::GetWidth()
{
	return mainWidth;
}
DLL_API void DEScene::SetHeight(unsigned int height)
{
	mainHeight = height;
}
DLL_API unsigned int DEScene::GetHeight()
{
	return mainHeight;
}



/******************************
/*       MAIN PROPERTIES      *
/******************************/
DLL_API STRUCT_DE_SCENE DEScene::GetSceneProperties()
{
	return sceneProperties;
}
DLL_API bool DEScene::GetCreated()
{
	return bCreated;
}
DLL_API bool DEScene::isLoaded()
{
	return bLoaded;
}
DLL_API STRUCT_DE_FOV DEScene::GetFOV()
{
	return sFOV;
}
DLL_API LPCSTR DEScene::GetName()
{
	return sceneProperties.Scene_Name;
}



/*******************************
/*           PHYSICS           *
/*******************************/
DLL_API void DEScene::SetPhysicsProperties(STRUCT_DE_SCENE_PHYSICSPROPERTIES newProperties)
{
	scenePhysicsProperties = newProperties;

	if (scenePhysX != NULL)
	{
		scenePhysX->setTiming(scenePhysicsProperties.Scene_PhysicsProperties_StepSize, scenePhysicsProperties.Scene_PhysicsProperties_NumIterations, NX_TIMESTEP_FIXED);
		scenePhysX->setGravity(NxVec3(	scenePhysicsProperties.Scene_PhysicsProperties_Gravity.x,
										scenePhysicsProperties.Scene_PhysicsProperties_Gravity.y,
										scenePhysicsProperties.Scene_PhysicsProperties_Gravity.z));

		NxMaterial* defaultMaterial = scenePhysX->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_Restitution);
		defaultMaterial->setStaticFriction(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_StaticFriction);
		defaultMaterial->setDynamicFriction(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_DynamicFriction);
		defaultMaterial->setDynamicFrictionV(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_DynamicFrictionV);
		defaultMaterial->setStaticFrictionV(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_StaticFrictionV);
		defaultMaterial->setDirOfAnisotropy(NxVec3(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy.x, scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy.y, scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_SetDirOfAnisotropy.z));
		defaultMaterial->setFlags(scenePhysicsProperties.Scene_PhysicsProperties_DefaultMaterial_Flags); 
		

		//Crea il piano base
		if (scenePhysXGround != NULL)
		{
			scenePhysX->releaseActor(*scenePhysXGround);
		}		
		if (scenePhysicsProperties.Scene_PhysicsProperties_Ground == true)
		{

			NxPlaneShapeDesc planeDesc;
			NxActorDesc actorDesc;
			NxMaterialDesc materialDesc;
			planeDesc.normal = NxVec3(scenePhysicsProperties.Scene_PhysicsProperties_GroundNormal.x,
										scenePhysicsProperties.Scene_PhysicsProperties_GroundNormal.y,
										scenePhysicsProperties.Scene_PhysicsProperties_GroundNormal.z);
			planeDesc.d = scenePhysicsProperties.Scene_PhysicsProperties_GroundY;
			planeDesc.materialIndex = defaultMaterial->getMaterialIndex();

			actorDesc.shapes.pushBack(&planeDesc);
			scenePhysXGround = scenePhysX->createActor(actorDesc);
		}///*/
	}
}

DLL_API STRUCT_DE_SCENE_PHYSICSPROPERTIES DEScene::GetPhysicsProperties()
{
	return scenePhysicsProperties;
}


DLL_API bool DEScene::SetGroupCollisionFlag(unsigned int group1, unsigned int group2, bool collide)
{
	if ((group1>=0)&&(group1<=30)&&(group2>=0)&&(group2<=30))
	{
		scenePhysX->setGroupCollisionFlag(group1, group2, collide);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}
DLL_API bool DEScene::GetGroupCollisionFlag(unsigned int group1, unsigned int group2)
{
	if ((group1>=0)&&(group1<=30)&&(group2>=0)&&(group2<=30))
	{
		return scenePhysX->getGroupCollisionFlag(group1, group2);
	}
	else
	{
		return false;
	}
	return false;
}




/*******************************
/*       CINEMATIC 16/9        *
/*******************************/
DLL_API void DEScene::SetCinematicEffect(bool useCinematic)
{
	sceneUseCinematic = useCinematic;
}
DLL_API bool DEScene::GetCinematicEffect()
{
	return sceneUseCinematic;
}


//******************************
//*          PROPERTIES        *
//******************************
DLL_API STRUCT_DE_DEBUG DEScene::GetDebugProperties()
{
	return	SceneDebug;
}
DLL_API bool DEScene::SetDebugProperties(STRUCT_DE_DEBUG newDebugProperties)
{
	SceneDebug = newDebugProperties;
	return true;
}//*/

DLL_API STRUCT_DE_GENERAL DEScene::GetGeneralProperties()
{
	return SceneGeneral;
}
DLL_API bool DEScene::SetGeneralProperties(STRUCT_DE_GENERAL newGeneralProperties)
{
	SceneGeneral = newGeneralProperties;
	return true;
}

DLL_API STRUCT_DE_CLEAR DEScene::GetClearProperties()
{
	return SceneClear;
}
DLL_API bool DEScene::SetClearProperties(STRUCT_DE_CLEAR newClearProperties)
{
	SceneClear = newClearProperties;
	return true;
}

DLL_API STRUCT_DE_PERSPECTIVE DEScene::GetPerspectiveProperties()
{
	return ScenePerspective;
}
DLL_API bool DEScene::SetPerspectiveProperties(STRUCT_DE_PERSPECTIVE newPerspectiveProperties)
{
	ScenePerspective = newPerspectiveProperties;
	return true;
}	

DLL_API STRUCT_DE_FOG DEScene::GetFogProperties()
{
	return SceneFog;
}
DLL_API bool DEScene::SetFogProperties(STRUCT_DE_FOG newFogProperties)
{
	SceneFog = newFogProperties;
	return true;
}

DLL_API STRUCT_DE_GRIDS DEScene::GetGridsProperties()
{
	return SceneGrids;
}
DLL_API bool DEScene::SetGridsProperties(STRUCT_DE_GRIDS newGridsProperties)
{
	SceneGrids = newGridsProperties;
	
	spatialGrid.Release();
	spatialGrid.Create(SceneGrids.GridsProperties_SpatialGrid_DimensionCell, SceneGrids.GridsProperties_SpatialGrid_PerSideCells);
	
	mainGrid.Release();
	mainGrid.Create(SceneGrids.GridsProperties_GroundGrid_PerSideCells, SceneGrids.GridsProperties_GroundGrid_DimensionCell, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(128, 128, 128), "mainGrid");

	return true;
}



DLL_API STRUCT_DE_SCENE_RUNTIMEINFO DEScene::GetRuntimeInfo()
{
	//Riempie le info di runtime
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalCameras = totalCameras;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalModels = totalModels;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalPanels = totalPanels;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalParticles = totalParticles;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalSoundGroup = totalSoundsGroup;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalSounds = totalSounds;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalFlares = totalFlares;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalSky = totalSky;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalFonts = totalFonts;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalVertices = totalVerticies;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalFaces = totalFaces;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalVerticesVisible = totalVerticiesCurrent;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalFacesVisible = totalFacesCurrent;	
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalVerticesPassVisible = totalVerticiesPassCurrent;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalFacesPassVisible = totalFacesPassCurrent;	
	sceneRuntimeinfo.Scene_RuntimeInfo_CurrentCameraID = currentCameraID;
	sceneRuntimeinfo.Scene_RuntimeInfo_CurrentCameraGridPosition = spatialGridPosition;
	sceneRuntimeinfo.Scene_RuntimeInfo_CurrentCameraPosition = camerasList.at(currentCameraID)->GetPosition();
	sceneRuntimeinfo.Scene_RuntimeInfo_CurrentCameraView = camerasList.at(currentCameraID)->GetView();
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalLights = totalLight;
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalLightsActive = totalActiveLights;
	sceneRuntimeinfo.Scene_RuntimeInfo_CurrentSpatialCellsVisible = spatialGrid.GetCellsInFrustrum();
	sceneRuntimeinfo.Scene_RuntimeInfo_TotalSpatialCells = spatialGrid.GetTotalCells();
	return sceneRuntimeinfo;
}






/******************************
/*          MATRICIES         *
/******************************/
DLL_API D3DXMATRIX DEScene::GetProjectionMatrix()
{
	return matrixProj;
}
DLL_API D3DXMATRIX DEScene::GetOrthoMatrix()
{
	return matrixOrtho;
}
DLL_API D3DXMATRIX DEScene::GetIdentityMatrix()
{
	return matrixIdentity;
}
DLL_API void DEScene::SetProjectionMatrix(D3DXMATRIX newMatrix)
{
	matrixProj = newMatrix;
}
DLL_API void DEScene::SetOrthoMatrix(D3DXMATRIX newMatrix)
{
	matrixOrtho = newMatrix;
}
DLL_API void DEScene::SetIdentityMatrix(D3DXMATRIX newMatrix)
{
	matrixIdentity = newMatrix;
}










DLL_API bool DEScene::Invalidate()
{
	try
	{
		//Funzione definita da utente
		if (sceneProperties.Scene_FuncPointer_Invalidate!=NULL)
		{
			(* sceneProperties.Scene_FuncPointer_Invalidate) ();
		}

		//Luci
		for (unsigned int lo=0; lo<totalLight; lo++)
		{
			lightList[lo]->Invalidate();
		}
		//Camera
		for (unsigned int c=0; c<totalCameras; c++)
		{
			camerasList[c]->Invalidate();
		}
		//Pannelli
		for (unsigned int p=0; p<totalPanels; p++)
		{
			panelsList[p]->Invalidate();
		}
		//Models
		for (unsigned int m=0; m<totalModels; m++)
		{
			modelsList[m]->Invalidate();
		}
		//Sistemi particellari
		for (unsigned int p=0; p<totalParticles; p++)
		{
			particlesList[p]->Invalidate();
		}
		//Souni
		for (unsigned int s=0; s<totalSounds; s++)
		{
			soundsList[s]->Invalidate();
		}
		//Gruppi
		for (unsigned int g=0; g<totalSoundsGroup; g++)
		{
			soundsGroupList[g]->Invalidate();
		}
		//LensFlares
		for (unsigned int f=0; f<totalFlares; f++)
		{
			flaresList[f]->Invalidate();
		}
		//Font
		for (unsigned int f=0; f<totalFonts; f++)
		{
			fontsList[f]->Invalidate();
		}
		//Skies
		for (unsigned int s=0; s<totalSky; s++)
		{
			skyList[s]->Invalidate();
		}


		//Bounding modelli
		modelBoundingBox.Invalidate();
		modelBoundingSphere.Invalidate();
		modelAxes.Invalidate();
		//Triggers
		triggerBoundingBox.Invalidate();
		triggerBoundingSphere.Invalidate();
		//Bounding luci
		lightBoundingSphere.Invalidate();
		lightBoundingBox.Invalidate();
		//Bounding suoni
		soundBoundingSphere.Invalidate();

		//Oggetti
		mainAxes.Invalidate();
		mainGrid.Invalidate();
		mainFont.Invalidate();

		//Cursore
		sceneCursor.Invalidate();

		//16/9
		sceneCinematicPanel.Invalidate();

		//Spatial grid
		spatialGrid.Invalidate();

		//Post Process
		if (postProcessIsCreated) SafeRelease(g_pRenderTargetTexture);

		//StateBlocks
		if (sceneDefaultPixelstateBlock != NULL)	sceneDefaultPixelstateBlock->Release();
		if (sceneDefaultVertexstateBlock != NULL)	sceneDefaultVertexstateBlock->Release();

		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEScene::Restore()
{
	try
	{
		//Funzione definita da utente
		if (sceneProperties.Scene_FuncPointer_Restore!=NULL)
		{
			(* sceneProperties.Scene_FuncPointer_Restore) ();
		}
		//Luci
		for (unsigned int lo=0; lo<totalLight; lo++)
		{
			lightList[lo]->Restore();
		}
		//Camera
		for (unsigned int c=0; c<totalCameras; c++)
		{
			camerasList[c]->Restore();
		}
		//Pannelli
		for (unsigned int p=0; p<totalPanels; p++)
		{
			panelsList[p]->Restore();
		}
		//Models
		for (unsigned int m=0; m<totalModels; m++)
		{
			modelsList[m]->Restore();
		}
		//Sistemi particellari
		for (unsigned int p=0; p<totalParticles; p++)
		{
			particlesList[p]->Restore();
		}
		//Souni
		for (unsigned int s=0; s<totalSounds; s++)
		{
			soundsList[s]->Restore();
		}
		//Gruppi
		for (unsigned int g=0; g<totalSoundsGroup; g++)
		{
			soundsGroupList[g]->Restore();
		}
		//LensFlares
		for (unsigned int f=0; f<totalFlares; f++)
		{
			flaresList[f]->Restore();
		}
		//Font
		for (unsigned int f=0; f<totalFonts; f++)
		{
			fontsList[f]->Restore();
		}
		//Skies
		for (unsigned int s=0; s<totalSky; s++)
		{
			skyList[s]->Restore();
		}




		//Bounding modelli
		modelBoundingBox.Restore();
		modelBoundingSphere.Restore();
		modelAxes.Restore();
		//Triggers
		triggerBoundingBox.Restore();
		triggerBoundingSphere.Restore();
		//Bounding luci
		lightBoundingSphere.Restore();
		lightBoundingBox.Restore();
		//Bounding suoni
		soundBoundingSphere.Restore();

		//Oggetti
		mainAxes.Restore();
		mainGrid.Restore();
		mainFont.Restore();

		//Cursore
		sceneCursor.Restore();

		//16/9
		sceneCinematicPanel.Restore();

		//Spatial grid
		spatialGrid.Restore();

		//Post process
		if (postProcessIsCreated)
		{
			D3DSURFACE_DESC ddsd;
			HRESULT hr;
			hr=g_pFrameBufferSurface->GetDesc(&ddsd);
			if ( hr==D3D_OK )
			{
				hr = DECore::D3DDevice->CreateTexture(ddsd.Width, ddsd.Height, 1, D3DUSAGE_RENDERTARGET, ddsd.Format, D3DPOOL_DEFAULT,&g_pRenderTargetTexture,0);
				if ( hr==D3D_OK )
				{
					g_pRenderTargetTexture->GetSurfaceLevel(0, &g_pRenderTargetSurface);
				}
			}
		}

		//StateBlocks
		CreateDefaultRenderstatesBlocks();

		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEScene::Release()
{
	try
	{
		DELog::LogInfo("<br><b>Release Scene: %s</b>", this->GetName());
		//Luci
		for (unsigned int lo=0; lo<totalLight; lo++)
		{
			lightList[lo]->Release();
		}
		lightList.clear();
		//Camera
		for (unsigned int c=0; c<totalCameras; c++)
		{
			camerasList[c]->Release();
		}
		camerasList.clear();
		//Sky
		for (unsigned int c=0; c<totalSky; c++)
		{
			skyList[c]->Release();
		}
		skyList.clear();		
		//Pannelli
		for (unsigned int p=0; p<totalPanels; p++)
		{
			panelsList[p]->Release();
		}
		panelsList.clear();
		//Modelli
		for (unsigned int m=0; m<totalModels; m++)
		{
			modelsList[m]->Release();
		}
		modelsList.clear();
		//Sistemi particellari
		for (unsigned int p=0; p<totalParticles; p++)
		{
			particlesList[p]->Release();
		}
		particlesList.clear();
		//Souni
		for (unsigned int s=0; s<totalSounds; s++)
		{
			soundsList[s]->Release();
		}
		soundsList.clear();
		//Gruppi
		for (unsigned int g=0; g<totalSoundsGroup; g++)
		{
			soundsGroupList[g]->Release();
		}
		soundsGroupList.clear();
		//LensFlares
		for (unsigned int f=0; f<totalFlares; f++)
		{
			flaresList[f]->Release();
		}
		flaresList.clear();
		//Font
		for (unsigned int f=0; f<totalFonts; f++)
		{
			fontsList[f]->Release();
		}
		fontsList.clear();

		//Physics
		if(scenePhysX != NULL)
		{
			if (scenePhysXGround != NULL)
			{
				scenePhysX->releaseActor(*scenePhysXGround);
			}		
			DECore::enginePhysX->releaseScene(*scenePhysX);
		}



		//Bounding modelli
		modelBoundingBox.Release();
		modelBoundingSphere.Release();
		modelAxes.Release();
		//Triggers
		triggerBoundingBox.Release();
		triggerBoundingSphere.Release();
		//Bounding luci
		lightBoundingSphere.Release();
		lightBoundingBox.Release();
		//Bounding suoni
		soundBoundingSphere.Release();

		//Oggetti
		mainAxes.Release();
		mainGrid.Release();
		mainFont.Release();
		
		//Cursore
		sceneCursor.Release();

		//16/9
		sceneCinematicPanel.Release();

		//Spatial grid
		spatialGrid.Release();

		//Release default render states
		if (sceneDefaultPixelstateBlock != NULL)	sceneDefaultPixelstateBlock->Release();
		if (sceneDefaultVertexstateBlock != NULL)	sceneDefaultVertexstateBlock->Release();

		//Post Process
		if (postProcessIsCreated)
		{
			SafeRelease(g_pRenderTargetTexture);
			SafeRelease(g_pRenderTargetSurface);
		}

		//Azzera le variabili di lista(e altro)
		totalLight = 0;
		totalCameras = 0;
		currentCameraID = 0;
		totalModels = 0;
		totalPanels = 0;
		totalParticles = 0;
		totalSounds = 0;
		totalFlares = 0;
		totalSky = 0;
		totalFonts = 0;

		//Funzione definita da utente
		if (sceneProperties.Scene_FuncPointer_Release!=NULL)
		{
			if ((* sceneProperties.Scene_FuncPointer_Release) ()==true)
			{
				bLoaded=false;
				DELog::LogInfo("<li>The function completed successfully");
				return true;
			}
		}
		else
		{
			bLoaded=false;
			DELog::LogInfo("<li>The function completed successfully");
			return true;
		}
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}

	DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
	return false;
}

DLL_API bool DEScene::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEScene::GetTag()
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

DLL_API bool DEScene::CreateDefaultRenderstatesBlocks()
{
	//Pixel Pipeline
	DECore::D3DDevice->CreateStateBlock( D3DSBT_PIXELSTATE, &sceneDefaultPixelstateBlock );
	{
		DECore::D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
		DECore::D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
		DECore::D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_LASTPIXEL, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
		DECore::D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
		DECore::D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		DECore::D3DDevice->SetRenderState(D3DRS_ALPHAREF, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
		DECore::D3DDevice->SetRenderState(D3DRS_DITHERENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_DEPTHBIAS, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILREF, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff );
		DECore::D3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff );
		DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP0, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP1, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP2, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP3, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP4, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP5, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP6, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP7, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP8, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP9, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP10, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP11, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP12, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP13, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP14, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_WRAP15, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_LOCALVIEWER, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
		DECore::D3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		DECore::D3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		DECore::D3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 );
		DECore::D3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000f );
		DECore::D3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
		DECore::D3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP );
		DECore::D3DDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP );
		DECore::D3DDevice->SetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP );
		DECore::D3DDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS );
		DECore::D3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE1, 0x0000000f );
		DECore::D3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE2, 0x0000000f );
		DECore::D3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE3, 0x0000000f );
		DECore::D3DDevice->SetRenderState(D3DRS_BLENDFACTOR, 0xffffffff );
		DECore::D3DDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
		DECore::D3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO );
		DECore::D3DDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
	}
	DECore::D3DDevice->EndStateBlock( &sceneDefaultPixelstateBlock );


	//Vertex Pipeline
	DECore::D3DDevice->CreateStateBlock( D3DSBT_PIXELSTATE, &sceneDefaultVertexstateBlock );
	{
		DECore::D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
		DECore::D3DDevice->SetRenderState(D3DRS_FOGCOLOR, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		DECore::D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		DECore::D3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_AMBIENT, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
		DECore::D3DDevice->SetRenderState(D3DRS_FOGSTART, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_FOGEND, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_FOGDENSITY, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_CLIPPING, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_LOCALVIEWER, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
		DECore::D3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		DECore::D3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		DECore::D3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 );
		DECore::D3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
		DECore::D3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSIZE, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSCALE_A, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSCALE_B, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSCALE_C, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE );
		DECore::D3DDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff );
		DECore::D3DDevice->SetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE );
		DECore::D3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
		DECore::D3DDevice->SetRenderState(D3DRS_TWEENFACTOR, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC );
		DECore::D3DDevice->SetRenderState(D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR );
		DECore::D3DDevice->SetRenderState(D3DRS_MINTESSELLATIONLEVEL, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_X, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_Y, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_Z, 1 );
		DECore::D3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_W, 0 );
		DECore::D3DDevice->SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE );
	}
	DECore::D3DDevice->EndStateBlock( &sceneDefaultVertexstateBlock );

	return true;
}










DLL_API void DEScene::MoveTrigger(unsigned int triggerID, D3DXVECTOR3 position)
{
	if (scenePhysXTriggerActor.at(triggerID))
	{
		NxVec3 pos;
		pos.x = position.x;
		pos.y = position.y;
		pos.z = position.z;
		scenePhysXTriggerActor.at(triggerID)->setGlobalPosition(pos);
	}
}

DLL_API unsigned int DEScene::AddTrigger_Box(D3DXVECTOR3 pos, D3DXVECTOR3 dim)
{
	if (scenePhysX)
	{
		NxVec3 posVec;
		NxVec3 dimVec;
		posVec.x = pos.x;
		posVec.y = pos.y;
		posVec.z = pos.z;
		dimVec.x = dim.x;
		dimVec.y = dim.y;
		dimVec.z = dim.z;

		NxActorDesc actorDesc;
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions = dimVec;
		boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;

		actorDesc.shapes.pushBack(&boxDesc);
		actorDesc.globalPose.t = posVec + NxVec3(0, 0, 0);
		 
		NxActor *triggerActor = scenePhysX->createActor(actorDesc);
		triggerActor->userData = (void*)-1;

		//AGGIUNGE LO USERDATA tramite una struttura con id,flags,ecc
		triggerActor->userData = new ActorUserData;
		((ActorUserData *)(triggerActor->userData))->id = totalTriggers;
		// FIXME: After setting it, actorDesc.managedHwSceneIndex gets cleared for some reason...
		#if 0
		NxActorDesc actorDesc2;
		triggerActor->saveToDesc(actorDesc2);
		if (actorDesc2.managedHwSceneIndex > 0)  
		{
			// To identify the actor when render it, set a bit in the userdata flags
			((ActorUserData *)(triggerActor->userData))->flags |= UD_IS_HW_ACTOR;
		}
		#endif
		AddUserDataToShapes(triggerActor, totalTriggers);

		((ActorUserData*)(triggerActor->userData))->flags |= UD_IS_TRIGGER;

		scenePhysXTriggerActor.push_back(triggerActor);
		totalTriggers = totalTriggers + 1;
		return (unsigned int)scenePhysXTriggerActor.size()-1;
	}
	else
	{
		return 0;
	}
	return 0;
}

DLL_API unsigned int DEScene::AddTrigger_Sphere(D3DXVECTOR3 pos, float radius)
{
	if (scenePhysX)
	{
		NxVec3 posVec;
		NxVec3 dimVec;
		posVec.x = pos.x;
		posVec.y = pos.y;
		posVec.z = pos.z;

		NxActorDesc actorDesc;
		NxSphereShapeDesc sphereDesc;
		sphereDesc.radius = radius;
		sphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;

		actorDesc.shapes.pushBack(&sphereDesc);
		actorDesc.globalPose.t = posVec + NxVec3(0, 0, 0);
		 
		NxActor *triggerActor = scenePhysX->createActor(actorDesc);
		triggerActor->userData = (void*)-1;

		//AGGIUNGE LO USERDATA tramite una struttura con id,flags,ecc
		triggerActor->userData = new ActorUserData;
		((ActorUserData *)(triggerActor->userData))->id = totalTriggers;
		// FIXME: After setting it, actorDesc.managedHwSceneIndex gets cleared for some reason...
		#if 0
		NxActorDesc actorDesc2;
		triggerActor->saveToDesc(actorDesc2);
		if (actorDesc2.managedHwSceneIndex > 0)  
		{
			// To identify the actor when render it, set a bit in the userdata flags
			((ActorUserData *)(triggerActor->userData))->flags |= UD_IS_HW_ACTOR;
		}
		#endif
		AddUserDataToShapes(triggerActor, totalTriggers);

		((ActorUserData*)(triggerActor->userData))->flags |= UD_IS_TRIGGER;

		scenePhysXTriggerActor.push_back(triggerActor);
		totalTriggers = totalTriggers + 1;
		return (unsigned int)scenePhysXTriggerActor.size()-1;
	}
	else
	{
		return 0;
	}
	return 0;
}






/*
DLL_API void DEScene::AddCursorTexture(const char* strFilename, DWORD dwKeyColor)
{
	//Crea la texture
	if (strFilename!=NULL)
	{
		int textureID = DETextureManager::CreateTexture(strFilename, dwKeyColor, false);
		if ((textureID < 0)||(textureID > 10000))
		{								
			DELog::LogWarning("<li>D3DXCreateTextureFromFileEx(%s) - Panel may appear without texture.", strFilename);
		}
		else
		{
			textureList.push_back(textureID);
		}
	}
}

DLL_API void DEScene::SetCursorTexture(unsigned int textureID)
{
	Cursor.SetTexture(DETextureManager::GetTexture(textureList[textureID]).Texture_Pointer);
}*/
