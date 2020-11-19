#include "DEGeneralIncludes.h"

DLL_API DELensFlare::DELensFlare()
{
	totalFlares = 0;
	totalFlash = 0;
	totalEmitters3D = 0;
	totalEmitters2D = 0;
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DELensFlare::Create(STRUCT_DE_FOV fovProperties, bool cameraBased, float alphaAttenuation_velocity, float flare_dimension, bool reduceFlare_scale, LPCSTR logTitle)
{
	char strLog[200];
	mainLogTitle = logTitle;
	isCreated = false;
	occluded=false;
	occludedValue = 1.0f;
	isCameraBased = cameraBased;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{
			pos2Dlens.x=0.0f;
			pos2Dlens.y=0.0f;
			pos2Dlens.z=0.0f;
			pos3Dlens.x=0.0f;
			pos3Dlens.y=0.0f;
			pos3Dlens.z=0.0f;

			//Fov
			mainFovProperties = fovProperties;

			//Attenuation
			alphaAttenuation = 0.0f;
			alphaAttenuationVelocity = alphaAttenuation_velocity;
			flareDimension = flare_dimension;

			//Riduzione scala
			reduceFlareScale = reduceFlare_scale;

			//Preleva il vieport corrente
			DECore::D3DDevice->GetViewport(&currentViewport);
			screenWidth = currentViewport.Width;
			screenHeight = currentViewport.Height;
			
			//Centro dello schermo
			middleScreenWidth = (float)screenWidth / 2.0f;
			middleScreenHeight = (float)screenHeight / 2.0f;
			
			//Flares
			Flare.Create("Flare");
			Flare.SetMode(true);
			Flare.SetPosition(D3DXVECTOR2(20, 320));
			Flare.SetRotation(0.0f);
			Flare.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			Flare.SetScale(D3DXVECTOR2(1.0f, 1.0f));
			for (unsigned int currentFlare=0; currentFlare< totalFlares; currentFlare++)
			{
				Flare.AddTexture(FlareProperties[currentFlare].textureFilename, FlareProperties[currentFlare].colorkey, 1, 1, 0);
			}


			//Flash
			Flash.Create("Flash");
			Flash.SetMode(false);
			Flash.SetPosition(D3DXVECTOR2(20, 320));
			Flash.SetRotation(0.0f);
			Flash.SetScale(D3DXVECTOR2(1.0f, 1.0f));
			Flash.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	
			for (unsigned int currentFlash=0; currentFlash< totalFlash; currentFlash++)
			{
				Flash.AddTexture(FlashProperties[currentFlash].textureFilename, FlashProperties[currentFlash].colorkey, 1, 1, 0);
			}

			//Emitter3D
			Emitter3D = new DEBillboard[totalEmitters3D];
			for (unsigned int currentEmitter3D=0; currentEmitter3D< totalEmitters3D; currentEmitter3D++)
			{
				sprintf(strLog,"Emitter3D[%.0f]",float(currentEmitter3D));
				Emitter3D[currentEmitter3D].Create(Emitter3DProperties[currentEmitter3D].textureFilename, Emitter3DProperties[currentEmitter3D].color, strLog);
			}

			//Emitter2D
			Emitter2D.Create("Emitter2D");
			Emitter2D.SetMode(true);
			Emitter2D.SetPosition(D3DXVECTOR2(20, 320));
			Emitter2D.SetRotation(0.0f);
			Emitter2D.SetScale(D3DXVECTOR2(1.0f, 1.0f));
			Emitter2D.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	
			for (unsigned int currentEmitter2D=0; currentEmitter2D< totalEmitters2D; currentEmitter2D++)
			{
				Emitter2D.AddTexture(Emitter2DProperties[currentEmitter2D].textureFilename, Emitter2DProperties[currentEmitter2D].color, 1, 1, 0);
			}


			isCreated=true;
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

DLL_API	void DELensFlare::AddEmitter3D(char *Emitter3D_textureFilename, D3DXCOLOR Emitter3D_color, float Emitter3D_scaleFactor)
{
	//Salva temporaneamente l'array
	SEmitterProperties	*saveEmitter3DProperties;
	saveEmitter3DProperties = (SEmitterProperties *)calloc(totalEmitters3D, sizeof(SEmitterProperties));
	for (unsigned int h=0; h< totalEmitters3D; h++)
	{
		saveEmitter3DProperties[h] = Emitter3DProperties[h];
	}

	//Ricrea l'array dei figli aggiungendone uno
	Emitter3DProperties = (SEmitterProperties *)calloc((totalEmitters3D+1), sizeof(SEmitterProperties));
	for (unsigned int h=0; h< totalEmitters3D; h++)
	{
		Emitter3DProperties[h] = saveEmitter3DProperties[h];
	}

	Emitter3DProperties[totalEmitters3D].textureFilename = Emitter3D_textureFilename;
	Emitter3DProperties[totalEmitters3D].color = Emitter3D_color;
	Emitter3DProperties[totalEmitters3D].scaleFactor = Emitter3D_scaleFactor;

	//Aggiunto un flare alla lista
	totalEmitters3D = totalEmitters3D+1;

	free(saveEmitter3DProperties);

}

DLL_API	void DELensFlare::AddEmitter2D(char *Emitter2D_textureFilename, D3DXCOLOR Emitter2D_color, float Emitter2D_scaleFactor)
{
	//Salva temporaneamente l'array
	SEmitterProperties	*saveEmitter2DProperties;
	saveEmitter2DProperties = (SEmitterProperties *)calloc(totalEmitters2D, sizeof(SEmitterProperties));
	for (unsigned int h=0; h< totalEmitters2D; h++)
	{
		saveEmitter2DProperties[h] = Emitter2DProperties[h];
	}

	//Ricrea l'array dei figli aggiungendone uno
	Emitter2DProperties = (SEmitterProperties *)calloc((totalEmitters2D+1), sizeof(SEmitterProperties));
	for (unsigned int h=0; h< totalEmitters2D; h++)
	{
		Emitter2DProperties[h] = saveEmitter2DProperties[h];
	}

	Emitter2DProperties[totalEmitters2D].textureFilename = Emitter2D_textureFilename;
	Emitter2DProperties[totalEmitters2D].color = Emitter2D_color;
	Emitter2DProperties[totalEmitters2D].scaleFactor = Emitter2D_scaleFactor;
	D3DXCOLOR color = D3DXCOLOR(Emitter2D_color);
	Emitter2DProperties[totalEmitters2D].maxAlpha = color.a;

	//Aggiunto un flare alla lista
	totalEmitters2D = totalEmitters2D+1;

	free(saveEmitter2DProperties);

}

DLL_API	void DELensFlare::AddFlash(char *flash_textureFilename, D3DXCOLOR flash_color, D3DXCOLOR flash_colorkey)
{
	//Salva temporaneamente l'array
	SFlashProperties	*saveFlashProperties;
	saveFlashProperties = (SFlashProperties *)calloc(totalFlash, sizeof(SFlashProperties));
	for (unsigned int h=0; h< totalFlash; h++)
	{
		saveFlashProperties[h]=FlashProperties[h];
	}

	//Ricrea l'array dei figli aggiungendone uno
	FlashProperties = (SFlashProperties *)calloc((totalFlash+1), sizeof(SFlashProperties));
	for (unsigned int h=0; h< totalFlash; h++)
	{
		FlashProperties[h] = saveFlashProperties[h];
	}

	FlashProperties[totalFlash].textureFilename = flash_textureFilename;
	FlashProperties[totalFlash].color = flash_color;
	FlashProperties[totalFlash].colorkey = flash_colorkey;
	D3DXCOLOR color = D3DXCOLOR(flash_color);
	FlashProperties[totalFlash].maxAlpha = color.a;

	//Aggiunto un flare alla lista
	totalFlash = totalFlash+1;

	free(saveFlashProperties);

}


DLL_API	void DELensFlare::AddFlare(char *flare_textureFilename, D3DXCOLOR flare_color, D3DXCOLOR flare_colorkey, float flare_scaleFactor, float flare_factor)
{
	//Salva temporaneamente l'array
	SFlareProperties	*saveFlareProperties;
	saveFlareProperties = (SFlareProperties *)calloc(totalFlares, sizeof(SFlareProperties));
	for (unsigned int h=0; h< totalFlares; h++)
	{
		saveFlareProperties[h]=FlareProperties[h];
	}

	//Ricrea l'array dei figli aggiungendone uno
	FlareProperties = (SFlareProperties *)calloc((totalFlares+1), sizeof(SFlareProperties));
	for (unsigned int h=0; h< totalFlares; h++)
	{
		FlareProperties[h] = saveFlareProperties[h];
	}

	FlareProperties[totalFlares].textureFilename = flare_textureFilename;
	FlareProperties[totalFlares].color = flare_color;
	FlareProperties[totalFlares].colorkey = flare_colorkey;
	FlareProperties[totalFlares].scaleFactor = flare_scaleFactor;
	FlareProperties[totalFlares].factor = flare_factor;
	FlareProperties[totalFlares].factor = flare_factor;
	D3DXCOLOR color = D3DXCOLOR(flare_color);
	FlareProperties[totalFlares].maxAlpha = color.a;
	//Aggiunto un flare alla lista
	totalFlares = totalFlares+1;

	free(saveFlareProperties);

}



DLL_API void DELensFlare::SetCamera(D3DXVECTOR3 newCameraPosition, D3DXVECTOR3 newCameraView)
{
	pos3DCameraView = newCameraView;
	if (isCameraBased)	pos3DCamera = newCameraPosition;
	else				pos3DCamera = D3DXVECTOR3(0,0,0);
}


DLL_API void DELensFlare::SetPosition(D3DXVECTOR3 newPosition)
{
	pos3Dlens.x = newPosition.x;
	pos3Dlens.y = newPosition.y;
	pos3Dlens.z = newPosition.z;
}

DLL_API D3DXVECTOR3 DELensFlare::GetPosition()
{
	return pos3Dlens;
}

DLL_API void DELensFlare::Render3D()
{
	//Usa la sorgente di luce
	for (unsigned int m=0; m<totalEmitters3D; m++)
	{
		Emitter3D[m].SetPosition(pos3Dlens + pos3DCamera);
		Emitter3D[m].SetScaleEqual(Emitter3DProperties[m].scaleFactor);
		Emitter3D[m].Render(false, false, 200, 300, 800, 300, pos3DCameraView, true, mainFovProperties);
	}
}



DLL_API void DELensFlare::CalculatePosition()
{
	if ((DECore::D3DDevice!=NULL)&&(isCreated))
	{

		//Matrici correnti
		D3DXMATRIX	viewMat;
		D3DXMATRIX	projMat;
		DECore::D3DDevice->GetTransform(D3DTS_VIEW,&viewMat);
		DECore::D3DDevice->GetTransform(D3DTS_PROJECTION,&projMat);

		//Calcola la posizione 2D del punto per flares e flash
		D3DXVECTOR3 newPos;
		newPos = pos3Dlens + pos3DCamera;
		D3DXVec3Project(&pos2Dlens, &newPos, &currentViewport, &projMat, &viewMat, NULL);

		//Calcola se il punto è dentro o fuori il frustrum
		if (DEUtility::FOVCheck(newPos, flareDimension, viewMat, mainFovProperties, &pos3DCameraView ))
		{
			cull=false;
		}
		else
		{
			cull=true;
		}
	}
}

DLL_API void DELensFlare::Render2D()
{
	if (isCreated)
	{
		if (cull==false)
		{
			//Width
			float currentValue = 0.0f;
			float currentValueT = 0.0f;
			if (pos2Dlens.x < screenWidth/2)
			{
				currentValueT = ((pos2Dlens.x)/(screenWidth/2.0f));
				currentValue = 1.0f - currentValueT;
			}
			if (pos2Dlens.x >= screenWidth/2)
			{
				currentValueT = ((pos2Dlens.x - (screenWidth/2.0f)))/ (screenWidth/2.0f);
				currentValue = currentValueT;
			}
			
			//Height
			if (pos2Dlens.y < screenHeight/2)
			{
				currentValueT = ((pos2Dlens.y)/(screenHeight/2.0f));
				currentValue = currentValue + (1.0f - currentValueT);
			}
			if (pos2Dlens.y >= screenHeight/2)
			{
				currentValueT = ((pos2Dlens.y - (screenHeight/2.0f)))/ (screenHeight/2.0f);
				currentValue = currentValue + (currentValueT);
			}


			float currentAlpha = (1.0f - currentValue);
			if (occluded)
			{
				alphaAttenuation = alphaAttenuation + alphaAttenuationVelocity;
				if (alphaAttenuation>=1.0f - occludedValue)
				{
					alphaAttenuation = 1.0f - occludedValue;
				}
			}
			else
			{
				alphaAttenuation = 0.0f;
			}
			currentAlpha = currentAlpha - alphaAttenuation;

			//** EMITTER2D **
			for (unsigned int currentEmitter2D=0; currentEmitter2D<totalEmitters2D; currentEmitter2D++)
			{
				Emitter2D.SetPosition(D3DXVECTOR2(pos2Dlens.x, pos2Dlens.y));
				Emitter2D.SetScale(D3DXVECTOR2(Emitter2DProperties[currentEmitter2D].scaleFactor, Emitter2DProperties[currentEmitter2D].scaleFactor));				
				Emitter2D.SetTextureID( currentEmitter2D);	
				Emitter2D.SetDimensions( D3DXVECTOR2(Emitter2D.GetTextureInfoID(currentEmitter2D, false).x , Emitter2D.GetTextureInfoID(currentEmitter2D, false).y)  );
				Emitter2D.SetMode(true);
				Emitter2DProperties[currentEmitter2D].color.a = currentAlpha;
				Emitter2D.SetColor(Emitter2DProperties[currentEmitter2D].color);
				Emitter2D.Render();
			}

			//Esegue solo se il bagliore è minore di 1
			//if (currentValue <= 1.0f)
			{
				 // Destination is opposite side of centre from source
				float dx = middleScreenWidth + (middleScreenWidth - pos2Dlens.x);
				float dy = middleScreenHeight + (middleScreenHeight - pos2Dlens.y);

				//** FLARES **
				for (unsigned int currentFlare=0; currentFlare<totalFlares; currentFlare++)
				{
					//Posizione
					D3DXVECTOR2 posNew;
					posNew.x = (1.0f - FlareProperties[currentFlare].factor)*pos2Dlens.x + (FlareProperties[currentFlare].factor*dx) ;
					posNew.y = (1.0f - FlareProperties[currentFlare].factor)*pos2Dlens.y + (FlareProperties[currentFlare].factor*dy) ;		
				
					//Scala
					D3DXVECTOR2 scaleNew;
					if (reduceFlareScale)
					{
						float currentFlareScale = FlareProperties[currentFlare].scaleFactor * (1.0f - currentValue);
						scaleNew.x = currentFlareScale;
						scaleNew.y = currentFlareScale;
					}
					else
					{
						scaleNew.x = 1.0f;
						scaleNew.y = 1.0f;
					}
			
					//Alpha
					//float currentFlareAlpha = (FlareProperties[currentFlare].maxAlpha * (1.0f - currentValue));
					//if (currentFlareAlpha>=1.0f) continue;
	
					//Disegna il flare corrente nella posizione trovata, il fattore di scalatura scelto e il valore di trasparenza calcolato
					Flare.SetPosition(posNew);
					Flare.SetScale(scaleNew);
					FlareProperties[currentFlare].color.a = currentAlpha;
					Flare.SetColor(FlareProperties[currentFlare].color);
					Flare.SetTextureID( currentFlare );
					Flare.SetDimensions( D3DXVECTOR2(Flare.GetTextureInfoID(currentFlare, false).x , Flare.GetTextureInfoID(currentFlare, false).y)  );
					Flare.SetMode(true);
					Flare.Render();
				}

				//** FLASH **
				for (unsigned int currentFlash=0; currentFlash<totalFlash; currentFlash++)
				{
					//Calcola il valore dell'alpha in base a quello massimo definito dall'utente
					//float currentFlashAlpha = (FlashProperties[currentFlash].maxAlpha * (1.0f - currentValue));
					//if (currentFlashAlpha>=1.0f) continue;

					Flash.SetPosition(D3DXVECTOR2(0,0));
					FlashProperties[currentFlash].color.a = currentAlpha;
					Flash.SetColor(FlashProperties[currentFlash].color);
					Flash.SetDimensions(D3DXVECTOR2((float)screenWidth, (float)screenHeight));
					Flash.SetTextureID(currentFlash );
					Flash.SetMode(false);
					Flash.Render();
				}
			}//occluded
		} //FOVCheck
	}
}

DLL_API bool DELensFlare::Invalidate()
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

DLL_API bool DELensFlare::Restore()
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


DLL_API bool DELensFlare::Release()
{
	try
	{
		//Rilascia il flares
		if ((isCreated))
		{
			Flare.Release();
			Flash.Release();
			Emitter2D.Release();
			//Rilascia l'emettitore 3d
			if (totalEmitters3D>0)
			{
				for (unsigned int currentEmitter3D=0; currentEmitter3D< totalEmitters3D; currentEmitter3D++)
				{
					Emitter3D[currentEmitter3D].Release();
				}
				delete [] Emitter3D;
			}
		}
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



DLL_API bool DELensFlare::GetCulled()
{
	return cull;
}

DLL_API void DELensFlare::SetOccluded(bool isOccluded)
{
	occluded = isOccluded;
}

DLL_API bool DELensFlare::GetOccluded()
{
	return occluded;
}

DLL_API bool DELensFlare::GetCreated()
{
	return isCreated;
}

DLL_API bool DELensFlare::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DELensFlare::GetTag()
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


DLL_API bool DELensFlare::CalculateOcclusion(vector<DEModel*> modelArray)
{
	D3DXVECTOR3 m_vRayDirection = pos3Dlens;
	D3DXVECTOR3 m_vRayOrigin =  pos3DCamera;
	occluded = false;
	for (unsigned int e=0; e< modelArray.size(); e++)
	{	
		//Il flare è visibile
		if (cull==false)
		{
			//Il modello non è lontano,non è trascurabile
			if (modelArray[e]->GetModelInfo().Info_IsFar==false)	
			{
				if (modelArray[e]->GetModelInfo().Info_IsCulled==false)
				{
				unsigned int modelID = modelArray[e]->GetModelInfo().Info_MeshID;
					unsigned int currentLod = modelArray[e]->GetModelInfo().Info_CurrentLod;
					D3DXMATRIX finalMatrix = modelArray[e]->GetModelInfo().Info_FinalMatrix;
					//Sfera
					if (D3DXSphereBoundProbe(&modelArray[e]->GetModelInfo().Info_CenterCoord3D, modelArray[e]->GetModelInfo().Info_Radius_Scaled, &m_vRayOrigin, &m_vRayDirection ))
					{
						FLOAT fBary1, fBary2, fDist;
						D3DXVECTOR3 v0(0,0,0);
						D3DXVECTOR3 v1(0,0,0);
						D3DXVECTOR3 v2(0,0,0);
						for (unsigned long h=0; h<36; h=h+3)	//36 xkè il BBox è composto da 36 triangoli
						{		
							//Box
							D3DXVec3TransformCoord(&v0, &D3DXVECTOR3(DEVBManager::GetBoundingBoxVertices(h).x,DEVBManager::GetBoundingBoxVertices(h).y,DEVBManager::GetBoundingBoxVertices(h).z),&modelArray[e]->GetModelInfo().Info_BoundingBoxMatrix);
							D3DXVec3TransformCoord(&v1, &D3DXVECTOR3(DEVBManager::GetBoundingBoxVertices(h+1).x,DEVBManager::GetBoundingBoxVertices(h+1).y,DEVBManager::GetBoundingBoxVertices(h+1).z),&modelArray[e]->GetModelInfo().Info_BoundingBoxMatrix);
							D3DXVec3TransformCoord(&v2, &D3DXVECTOR3(DEVBManager::GetBoundingBoxVertices(h+2).x,DEVBManager::GetBoundingBoxVertices(h+2).y,DEVBManager::GetBoundingBoxVertices(h+2).z),&modelArray[e]->GetModelInfo().Info_BoundingBoxMatrix);
							if( DEUtility::RayIntersectsTriangle( m_vRayOrigin, m_vRayDirection, v0, v1, v2, &fDist, &fBary1, &fBary2 ) )
							{
								BOOL bHit;
								DWORD dwIndex;
								float u,v;
								float dist;
								D3DXVECTOR3 vOrig,vDir;
								D3DXMATRIX invMat;
								//Si modificano i raggi in modo da "allinearli" alla matrice del mondo perchè la mesh è in 0,0,0
								D3DXMatrixInverse(&invMat,NULL,&modelArray[e]->GetModelInfo().Info_FinalMatrix);
								D3DXVec3TransformCoord(&vOrig,&m_vRayOrigin,&invMat);
								D3DXVec3TransformNormal(&vDir,&m_vRayDirection,&invMat);
								D3DXIntersect(DEMeshManager::GetMesh(modelID).Mesh_Point, &vOrig, &vDir, &bHit,&dwIndex,&u,&v,&dist,NULL,NULL);
								if (bHit)
								{
									occluded = true;
									if (modelArray[e]->GetMaterial().Material_IsTransparent)
									{
										occludedValue = 1.0f - modelArray[e]->GetMaterial().Material_Alpha;
									}
									else
									{
										occludedValue = 0.0f;
									}
									return true;
								}//*/

								/*senza DX bisognerebbe fare il ciclo su tutti i vertici del modello
								Vertici
								for (unsigned long h=0; h<DEMeshManager::GetMesh(modelID).Mesh_Point->GetNumVertices(); h=h+3)
								{
									D3DXVec3TransformCoord(&v0, &DEMeshManager::GetMesh(modelID).Mesh_Point[currentLod].pVerts[h],&finalMatrix);
									D3DXVec3TransformCoord(&v1, &DEModelManager::GetMesh(modelID).Object[currentLod].pVerts[h+1],&finalMatrix);
									D3DXVec3TransformCoord(&v2, &DEModelManager::GetMesh(modelID).Object[currentLod].pVerts[h+2],&finalMatrix);
									if (D3DXIntersectTri(&v0, &v1, &v2, &m_vRayOrigin, &m_vRayDirection, &fBary1, &fBary2, &fDist))
									{
										occluded = true;
										return true;
									}	
								}//*/
							}
						}
					}
				}//Culled
			}//Far
		}//Cull
	}//*/
	return false;
}


///////////////////////////////////////////////////////////////////////////////////
DLL_API NxRay DELensFlare::CalculateRay()
{
	D3DXVECTOR3 m_vRayDirection = pos3Dlens;
	D3DXVECTOR3 m_vRayOrigin =  pos3DCamera;
	D3DXVec3Normalize(&m_vRayDirection, &m_vRayDirection);
	NxVec3	orig;
	NxVec3	dir;
	orig.x = m_vRayOrigin.x;
	orig.y = m_vRayOrigin.y;
	orig.z = m_vRayOrigin.z;
	dir.x = m_vRayDirection.x;
	dir.y = m_vRayDirection.y;
	dir.z = m_vRayDirection.z;
	NxRay ray(orig, dir);
	return ray;

	/* Inserire le seguenti righe se si vuole usare il calcolo ray del PhysX
	//Il seguente calcolo è effettuato sul tipo definito durante la creazione dell'attore
	//Potrebbe essere un box, una sphere o una tringlemesh
	NxReal nShapes = scenePhysX->raycastAllShapes(flaresList.at(f)->CalculateRay(currentCamPosition), gRaycastReport, NX_ALL_SHAPES);
	if ( nShapes >= 1)
	{
		flaresList.at(f)->SetOccluded(true);
	}
	else
	{
		flaresList.at(f)->SetOccluded(false);
	}//*/

}