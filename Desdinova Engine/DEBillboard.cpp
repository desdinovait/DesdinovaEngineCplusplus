#include "DEGeneralIncludes.h"

DLL_API DEBillboard::DEBillboard()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
	D3DXMatrixIdentity(&worldMatrix);
}

DLL_API bool DEBillboard::Create(const char *szTextureFilePath, DWORD initColor, LPCSTR logTitle)
{
	HRESULT hr;
	VertexBuffer = NULL;
	mainLogTitle = logTitle;
	isCreated = false;

	pVertices = NULL;
	position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	scale = D3DXVECTOR2(0.0f,0.0f);
	rotationType = DE_BBROTATION_NONE; 
	rotationAngleCurrent = 0;
	rotationVelocity = 0;
	visible=true;
	color = initColor;

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
			hr = DECore::D3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX_BILLBOARD), vertexProcessingUsage, CUSTOMVERTEX_BILLBOARD_D3DFVF, D3DPOOL_MANAGED, &VertexBuffer, NULL);
			if (hr==D3D_OK)
			{		
				//Blocca il VertexBuffer
				hr = VertexBuffer->Lock(0, 4 * sizeof(CUSTOMVERTEX_BILLBOARD), (void**)&pVertices, 0);
				if (hr==D3D_OK)
				{
					//Setta il colore base
					pVertices[0].colour = D3DCOLOR_ARGB(255,255,255,255);
					pVertices[1].colour = D3DCOLOR_ARGB(255,255,255,255);
					pVertices[2].colour = D3DCOLOR_ARGB(255,255,255,255);
					pVertices[3].colour = D3DCOLOR_ARGB(255,255,255,255);

					//Nuova posizione
					pVertices[0].pos.x = -1.0f / 2.0f;
					pVertices[0].pos.y = -1.0f / 2.0f;
					pVertices[0].pos.z =  0.0f;

					pVertices[1].pos.x =  1.0f / 2.0f;
					pVertices[1].pos.y = -1.0f / 2.0f;
					pVertices[1].pos.z =  0.0f;

					pVertices[2].pos.x = -1.0f / 2.0f;
					pVertices[2].pos.y =  1.0f / 2.0f;
					pVertices[2].pos.z =  0.0f; 

					pVertices[3].pos.x = 1.0f / 2.0f;
					pVertices[3].pos.y = 1.0f / 2.0f;
					pVertices[3].pos.z = 0.0f;

					//Coordinate texture
					pVertices[0].u = 0.0f;
					pVertices[0].v = 0.0f;
					pVertices[1].u = 0.0f;
					pVertices[1].v = 1.0f;
					pVertices[2].u = 1.0f;
					pVertices[2].v = 0.0f;
					pVertices[3].u = 1.0f;
					pVertices[3].v = 1.0f;

					//Slocca il VertexBuffer
					hr = VertexBuffer->Unlock();
					if (hr==D3D_OK)
					{
						isCreated=true;

						//Crea la texture
						textureID = DETextureManager::CreateTexture(szTextureFilePath, NULL);
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


DLL_API inline void DEBillboard::SetColor(DWORD newColor)
{
	color = newColor;
}
DLL_API inline void DEBillboard::SetPosition(D3DXVECTOR3 newPosition)
{
	position = newPosition;
}
DLL_API inline void DEBillboard::SetScale(D3DXVECTOR2 newScale)
{
	scale = newScale;
}
DLL_API inline void DEBillboard::SetScaleEqual(float newScale)
{
	scale.x = newScale;
	scale.y = newScale;
}
DLL_API inline void DEBillboard::SetRotation(ENUM_DE_BBROTATION rotateType, float velocity)
{
	rotationType = rotateType; 
	rotationVelocity = velocity;
}
DLL_API	inline void DEBillboard::SetWorldMatrix(D3DXMATRIX newMatrix)
{
	worldMatrix = newMatrix;
}
DLL_API void DEBillboard::Render(bool useFadingNear, bool useFadingFar, float initDistance, float nearDistance, float centerDistance,  float farDistance, D3DXVECTOR3 centerCamera, bool useFrustrumCulling, STRUCT_DE_FOV fovProperties)
{
	if ((DECore::D3DDevice!=NULL)&&(isCreated))
	{
		SetDefaultRenderStates();

		//Trova la distanza dell billboard dalla camera
		D3DXVECTOR3 res;
		D3DXVec3Subtract( &res,&centerCamera, &position);
		captureDistance = D3DXVec3Length(&res);
		//In base alla distanza calcola se si vede o meno
		//Calcola la distanza finale in base se è stato definito o meno di fare il fading

		if ((captureDistance <= initDistance + nearDistance + centerDistance + farDistance)&&(captureDistance >= 0))
			visible = true;
		else
			visible = false;

		//Se è visibile fa il rendering
		//if (visible)
		{

			//Matrice di billboard
			D3DXMATRIX mat;
			D3DXMATRIX trasp;
			D3DXMATRIX posMatrix;
			D3DXMATRIX rotMatrix;
			D3DXMATRIX scaleMatrix;
			D3DXMATRIX finalMatrix;
			D3DXMatrixIdentity(&posMatrix);
			D3DXMatrixIdentity(&scaleMatrix);
			D3DXMatrixIdentity(&rotMatrix);
			
			//Preleva la matrice di vista corrente
			DECore::D3DDevice->GetTransform(D3DTS_VIEW,&mat);
			//La trasposta della matrice di vista è rivolta verso l'osservatore
			D3DXMatrixTranspose(&trasp,&mat);
			
			//La matrice di posizione è la trasposta
			posMatrix = trasp;
			
			//Riempie la matrice con i valori utilizzati per traslare il billboard
			posMatrix._11 = trasp._11;
			posMatrix._21 = trasp._21;
			posMatrix._31 = trasp._31;
			posMatrix._41 = position.x;

			posMatrix._12 = trasp._12;
			posMatrix._22 = trasp._22;
			posMatrix._32 = trasp._32;
			posMatrix._42 = position.y;

			posMatrix._13 = trasp._13;
			posMatrix._23 = trasp._23;
			posMatrix._33 = trasp._33;
			posMatrix._43 = position.z;

			//Questa ulteriore specificazione serve perchè altrimenti il billboard non funzionerebbe!!
			posMatrix._14 = 0;
			posMatrix._24 = 0;
			posMatrix._34 = 0;
			posMatrix._44 = 1;

			
			if (rotationType == DE_BBROTATION_CLOCKWISE)
			{
				rotationAngleCurrent = rotationAngleCurrent + rotationVelocity;
				if (rotationAngleCurrent == 360) rotationAngleCurrent = 0;
				D3DXMatrixRotationZ(&rotMatrix, rotationAngleCurrent);
			}

			if (rotationType == DE_BBROTATION_COUNTERCLOCKWISE)
			{
				rotationAngleCurrent = rotationAngleCurrent - rotationVelocity;
				if (rotationAngleCurrent == 360) rotationAngleCurrent = 0;
				D3DXMatrixRotationZ(&rotMatrix, rotationAngleCurrent);
			}

			//Scala il billboard. E' inutile scalare la Z visto che è bidimensionale :-)
			D3DXMatrixScaling(&scaleMatrix,scale.x, scale.y, 0);
			
			//La matrice finale è quella di scalatura x quella di posizione
			//finalMatrix = rotMatrix * scaleMatrix * posMatrix * worldMatrix;
			D3DXMatrixMultiply(&posMatrix, &posMatrix, &worldMatrix);
			D3DXMatrixMultiply(&posMatrix, &scaleMatrix, &posMatrix);
			D3DXMatrixMultiply(&finalMatrix, &rotMatrix, &posMatrix);

			//Setta la matrice finale
			DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );



			//*** FRUSTUM CULLING ***
			if (useFrustrumCulling)
			{
				//Ottiene il valore di scalatura più alto in modo calcolare l'occupazione maggiore e quindi eseguire il culling correttamente
				float scaleValue;
				if (scale.x > scale.y) scaleValue=scale.x;
				if (scale.x < scale.y) scaleValue=scale.y;
				if (scale.x == scale.y) scaleValue=scale.x;

				//Setta la nuova dimensione del raggio della boundign sphere (0.5 perchè inizialmente il lato del billboard vale 1,dunque il raggio 0.5)
				float tempRadius = 0.5f * scaleValue;

				//Check if the object is in the frustum
				D3DXMATRIX viewTransform;
				DECore::D3DDevice->GetTransform(D3DTS_VIEW,& viewTransform);
				if (!DEUtility::FOVCheck(position, tempRadius, viewTransform, fovProperties, &centerCamera))
				{
					cull=true;
				}
				else
				{
					cull=false;
				}
			}
			else
			{
				cull=false;
			}


			//*** RENDERING ***
			//Procede con il rendering se l'oggetto non è cullato
//			if ((cull==false)&&(visible==true))
			if ((cull==false))
			{
				//Colori per il Lerp
 				D3DXCOLOR colorLerp;
				D3DXCOLOR fade= D3DCOLOR_ARGB(255,0,0,0);
				D3DXCOLOR diff= color;
				//Colore di trasparenza massimo del billboard(preso dall'alpha del colore passato dall'utente)
				float traspValue = (float)color.a;
				D3DXColorLerp(&colorLerp, &fade, &diff, traspValue);
				
				//Se è stato specificato di fare il fading in base alla distanza allora calcola il lerp, altrimenti lo lascia così
				if (useFadingFar)
				{
					//Crea il FadeOUT (B = Billboard, C = Camera)
					// 
					//    init   near        center          far
					// C |------|------|------------------|-------|
					// 			
					// C                                  |--|       action
					// 
					// C |-----------------------------------|       capture
					float actionDistance = captureDistance - initDistance - nearDistance - centerDistance;// - farDistance;
					if (actionDistance >= 0)
					{
						//Usa la seguente proporzione per calcolare il lerp che deve essere tra 0 e 1
						//actionDistance : farDistance = lerp : traspValue(potrebbe essere 1 per avere la trasparenza base)
						// quindi:
						//lerp = actionDistance * traspValue / farDistance
						float lerp = (actionDistance * traspValue)/farDistance;
						//Il valore ritornato è un valore che va da 0 a 1...a noi serve da 1 a 0 per cui
						//viene tolto risptto a 1
						lerp = traspValue - lerp;
						//calcola il lerp in base al colore da cui partire e quello a cui arrivare (cioè 0...trasparente al massimo)
						D3DXColorLerp(&colorLerp, &fade, &diff, lerp);
					}
				}

				
				if (useFadingNear)
				{
					//Crea il FadeIN (B = Billboard, C = Camera)
					// 
					//    init   near        center          far
					// C |------|------|------------------|-------|
					// 			
					// C        |-|									action
					// 
					// C |--------|								    capture
					if ((captureDistance < initDistance + nearDistance))
					{
						//Usa la seguente proporzione per calcolare il lerp che deve essere tra 0 e 1
						//(captureDistance - initDistance) : nearDistance = lerp : traspValue(potrebbe essere 1 per avere la trasparenza base)
						// quindi:
						//lerp = (captureDistance - initDistance) * 1 / nearDistance
						float lerp = (captureDistance - initDistance)* traspValue/nearDistance;
						//Il valore ritornato è un valore che va da 1 a 0 qunidi...
						//Calcola il lerp in base al colore da cui partire e quello a cui arrivare (cioè 0...trasparente al massimo)
						D3DXColorLerp(&colorLerp, &fade, &diff, lerp);
					}

				}

				//Setta il nuovo colore
				DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR,colorLerp);


				//Disegna
				DECore::D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(CUSTOMVERTEX_BILLBOARD));
				DECore::D3DDevice->SetFVF(CUSTOMVERTEX_BILLBOARD_D3DFVF);
				DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			}//Cull
		}//Visible

	}
}

DLL_API void DEBillboard::SetDefaultRenderStates()
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
	
	DECore::D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE );
		DECore::D3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x00000000 );
		DECore::D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,	D3DCMP_ALWAYS );

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

DLL_API bool DEBillboard::Invalidate()
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

DLL_API bool DEBillboard::Restore()
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

DLL_API bool DEBillboard::Release()
{
	try
	{
		DETextureManager::ReleaseTexture(textureID);
		SafeRelease(VertexBuffer);
		isCreated=false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

inline DLL_API bool DEBillboard::GetCreated()
{
	return isCreated;
}

DLL_API bool DEBillboard::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEBillboard::GetTag()
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