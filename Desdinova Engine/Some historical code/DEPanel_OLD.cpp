#include "DEGeneralIncludes.h"
#include "DEPanel.h"

DLL_API DEPanel::DEPanel()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMatrixIdentity(&positionMatrix);
	D3DXMatrixIdentity(&rotationMatrix);
	D3DXMatrixIdentity(&scaleMatrix);
	panelFromCenter = false;
	dynamicTexture = NULL;
	useTransparent = false;
}

DLL_API bool DEPanel::Create(const char *szTextureFilePath, D3DCOLOR dwKeyColour, int width, int height, unsigned int totalRows, unsigned int framesPerRow, unsigned int initialFrame, LPCSTR logTitle)
{
	HRESULT hr;
	isCreated=false;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{
			//Dimensioni video
			D3DVIEWPORT9 currentViewport;
			DECore::D3DDevice->GetViewport(&currentViewport);
			screenWidth = currentViewport.Width;
			screenHeight = currentViewport.Height;

			//Colore
			panelKeyColour = dwKeyColour;
			panelColour = D3DCOLOR_XRGB(255, 255, 255);

			//Dimensioni
			panelWidth = width;
			panelHeight = height;

			//Crea la texture
			if (szTextureFilePath!=NULL)
			{
				textureID = DETextureManager::CreateTexture(szTextureFilePath, panelKeyColour, false);
				if ((textureID < 0)||(textureID > 10000))
				{								
					DELog::LogWarning("<li>%s : D3DXCreateTextureFromFileEx(%s) - Panel may appear without texture.", logTitle, szTextureFilePath);
				}
				else
				{
					ZeroMemory(&textureInfo, sizeof(D3DXIMAGE_INFO));
					textureInfo = DETextureManager::GetTexture(textureID).Texture_Info;
					if ((width==0)&&(height==0))
					{
						panelWidth = textureInfo.Width/framesPerRow;
						panelHeight = textureInfo.Height/totalRows;
					}
				}
			}


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
			//Crea il VertexBuffer in base se si vuole un solo frame o più di uno (Managed o Default)
			if ((framesPerRow==1)&&(totalRows==1))
			{
				hr = DECore::D3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX_PANEL), vertexProcessingUsage, CUSTOMVERTEX_PANEL_D3DFVF,D3DPOOL_MANAGED, &vertexBuffer, NULL);
			}
			else
			{
				hr = DECore::D3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX_PANEL), vertexProcessingUsage | D3DUSAGE_DYNAMIC, CUSTOMVERTEX_PANEL_D3DFVF,D3DPOOL_DEFAULT, &vertexBuffer, NULL);
			}
			if (hr == D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = vertexBuffer->Lock(0, 4 * sizeof(CUSTOMVERTEX_PANEL), (void**)&pVerticesPanel, 0);
				if (hr == D3D_OK)
				{
					//Set the positions of the vertices
					pVerticesPanel[0].x = -panelWidth / 2.0f;
					pVerticesPanel[0].y = -panelHeight / 2.0f;
					
					pVerticesPanel[1].x = -panelWidth / 2.0f;
					pVerticesPanel[1].y = panelHeight / 2.0f;
					
					pVerticesPanel[2].x = panelWidth / 2.0f;
					pVerticesPanel[2].y = -panelHeight / 2.0f;
					
					pVerticesPanel[3].x = panelWidth / 2.0f;
					pVerticesPanel[3].y = panelHeight / 2.0f;
					
					
					pVerticesPanel[0].z = 1.0f;
					pVerticesPanel[1].z = 1.0f;
					pVerticesPanel[2].z = 1.0f;
					pVerticesPanel[3].z = 1.0f;
					//Set the texture coordinates of the vertices
					pVerticesPanel[0].u = 0.0f;
					pVerticesPanel[0].v = 1.0f;
					pVerticesPanel[1].u = 0.0f;
					pVerticesPanel[1].v = 0.0f;
					pVerticesPanel[2].u = 1.0f;
					pVerticesPanel[2].v = 1.0f;
					pVerticesPanel[3].u = 1.0f;
					pVerticesPanel[3].v = 0.0f;

					//Slocca il VertexBuffer
					hr = vertexBuffer->Unlock();
					if (hr==D3D_OK)
					{
						//Frames
						frames = framesPerRow;
						rows = totalRows;
						startFrame = initialFrame;
						animationStart = GetTickCount();
						oldFrame = -1;
						GoToFrame(initialFrame);

						isCreated=true;
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
				DELog::LogError("<li%s : DECore::D3DDevice->CreateVertexBuffer(...): %s", logTitle, DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>%s: DECore::D3DDevice: NULL", logTitle);
		}

	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);	

	return isCreated;
}

DLL_API D3DXVECTOR2 DEPanel::GetPosition()
{
	return panelPosition;
}
DLL_API float DEPanel::GetRotation()
{
	return panelRotation;
}
DLL_API D3DXVECTOR2 DEPanel::GetScale()
{
	return panelScale;
}
DLL_API bool DEPanel::GetMode()
{
	return panelFromCenter;
}
DLL_API int DEPanel::GetHeight()
{
	return panelHeight;
}
DLL_API int DEPanel::GetWidth()
{
	return panelWidth;
}
DLL_API int DEPanel::GetCurrentFrame()
{
	return currentFrame;
}

DLL_API bool DEPanel::GoToFrame(unsigned int frame)
{	
	if ((rows==1)&&(frames==1))
	{
		return true;
	}
	else
	{
		if (frame != oldFrame)
		{
			CUSTOMVERTEX_PANEL *verts;
			//Accediamo alla memoria del vertex buffer
			HRESULT hr = vertexBuffer->Lock(0,0,(void**)&verts,0);
			if (hr == D3D_OK)
			{
				verts = pVerticesPanel;
				currentFrame = frame;
				oldFrame = currentFrame;
				float moverX  = 1.0f / (float)frames;
				float moverY  = 1.0f / (float)rows;
				unsigned int frameY = (unsigned int)(frame/frames);

				//Set the texture coordinates of the vertices
				verts[1].u = moverX * frame;
				verts[1].v = moverY * frameY;	
				
				verts[0].u = moverX * frame;
				verts[0].v = pVerticesPanel[1].v + moverY;
				
				verts[2].u = pVerticesPanel[0].u + moverX;
				verts[2].v = pVerticesPanel[1].v + moverY;

				verts[3].u = pVerticesPanel[1].u + moverX;
				verts[3].v = moverY * frameY;
				hr = vertexBuffer->Unlock();
				if (hr == D3D_OK)
				{
					return true;
				}
			}
		}
	}
	return false;
}

DLL_API void DEPanel::SetColor(D3DCOLOR newPanelColor)
{
	if (newPanelColor != panelColour)	panelColour = newPanelColor;
}
DLL_API void DEPanel::SetMode(bool positionFromCenter)
{
	panelFromCenter = positionFromCenter;
}
DLL_API void DEPanel::SetPosition(D3DXVECTOR2 newPosition)
{
	panelPosition = newPosition;
	//Setta la posizione globale
	float newPosX = panelPosition.x;
	float newPosY = panelPosition.y;
	//La nuova posizione e in base all'angolo in alto a sinistra
	if (panelFromCenter==false)
	{
		newPosX -= (screenWidth / 2) - (panelWidth / 2);
		newPosY -= (screenHeight / 2) - (panelHeight / 2);
	}
	else	//La nuova posizione è in base al centro
	{
		newPosX -= (screenWidth / 2); 
		newPosY -= (screenHeight / 2);
	}
	D3DXMatrixTranslation(&positionMatrix, newPosX, -newPosY, 0.0f);//*/
}
DLL_API void DEPanel::SetRotation(float newRotation)
{
	panelRotation = newRotation;
	D3DXMatrixRotationZ(&rotationMatrix, newRotation);
}
DLL_API void DEPanel::SetScale(D3DXVECTOR2 newScale)
{
	panelScale = newScale;
	D3DXMatrixScaling(&scaleMatrix, panelScale.x, panelScale.y, 1.0f);
}
DLL_API void DEPanel::SetTexture(LPDIRECT3DTEXTURE9 newTexture)
{
	dynamicTexture = newTexture;
}
DLL_API	void DEPanel::SetTrasparency(bool isTransparent, float transparentValue)
{
	//Colore di trasparenza x TextureFactor
	useTransparent = isTransparent;
	if (useTransparent)
	{
		D3DXCOLOR fade= panelColour;
		D3DXCOLOR diff= D3DCOLOR_ARGB(0,0,0,0);
		D3DXColorLerp(&transparentColour, &fade, &diff, transparentValue);
	}
}

DLL_API void DEPanel::Render()
{
	if (isCreated)
	{
		//Setta la texture
		if (dynamicTexture != NULL)	DECore::D3DDevice->SetTexture(0, dynamicTexture);
		else						DECore::D3DDevice->SetTexture(0, DETextureManager::GetTexture(textureID).Texture_Pointer);

		DECore::D3DDevice->SetTexture(1, NULL);
		DECore::D3DDevice->SetTexture(2, NULL);
		DECore::D3DDevice->SetTexture(3, NULL);
		DECore::D3DDevice->SetTexture(4, NULL);
		DECore::D3DDevice->SetTexture(5, NULL);
		DECore::D3DDevice->SetTexture(6, NULL);
		DECore::D3DDevice->SetTexture(7, NULL);

		//Setta la matrice finale
		//finalMatrix = scaleMatrix * rotationMatrix * positionMatrix;
		D3DXMatrixMultiply(&finalMatrix, &rotationMatrix, &positionMatrix);
		D3DXMatrixMultiply(&finalMatrix, &scaleMatrix, &finalMatrix);
		DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );

		//Trasparenza
		if (useTransparent)
		{
			DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
 				DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

			//Colore di trasparenza tramite TextureFactor
			DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, transparentColour);
			DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		}//*/
		else
		{
			if (panelKeyColour!=NULL)
			{
			DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
				DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
				
				DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR,panelColour);
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			}
			else
			{
				DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
 					//DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
					//DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
					DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
					DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
					DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
					DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

				DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR,panelColour);
				DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			}
		}//*/


		/* Texture stage states da alpha channel in file dds
		DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);  	
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);*/

		//Animazione
		if (animationExecute)
		{
			if( GetTickCount() - animationStart >= animationSpeed )
			{
				//Avanti
				if (animationForward)
				{
					currentFrame = currentFrame++;
					if (currentFrame >= frames)
					{	
						currentFrame = 0;
					}

					if (currentFrame==startFrame)
					{
						if (animationCycle==false)
						{
							currentFrame = startFrame;
							animationExecute = false;
						}
					}
				}
				else	//Indietro
				{
					currentFrame = currentFrame--;
					if (currentFrame < 0)
					{
						currentFrame = frames - 1;
					}

					if (currentFrame==startFrame)
					{
						if (animationCycle==false)
						{
							currentFrame = startFrame;
							animationExecute = false;
						}
					}
				}
				//Mostra il frame corrente
				GoToFrame(currentFrame);
				animationStart = GetTickCount();
			}
		}

		//Settaggio generale che evita alcuni errori se viene attivato il multitexturing
		DECore::D3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		DECore::D3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		
		DECore::D3DDevice->SetStreamSource(0,vertexBuffer,0,sizeof(CUSTOMVERTEX_PANEL));
		DECore::D3DDevice->SetFVF(CUSTOMVERTEX_PANEL_D3DFVF);
			
		DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
}


DLL_API void DEPanel::AnimationPlay()
{
	animationExecute = true;
}

DLL_API void DEPanel::AnimationStop()
{
	animationExecute=false;
	currentFrame=0;
}

DLL_API void DEPanel::AnimationPause()
{
	animationExecute=false;
}

DLL_API void DEPanel::AnimationSettings(DWORD everyMilliseconds, bool cycleAnimation, bool forwardAnimation)
{
	animationForward = forwardAnimation;
	animationCycle = cycleAnimation;
	animationSpeed = everyMilliseconds;
}

DLL_API bool DEPanel::CheckPicked(D3DXVECTOR2 ptCursor)
{
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;
	
	//Dimensioni video
	D3DVIEWPORT9 currentViewport;
	DECore::D3DDevice->GetViewport(&currentViewport);
	float engineWidth = currentViewport.Width;
	float engineHeight = currentViewport.Height;

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
	D3DXVECTOR3 v0(0,0,0);
	D3DXVECTOR3 v1(0,0,0);
	D3DXVECTOR3 v2(0,0,0);
	for (unsigned long h=0; h<2; h=h+1)	//2 xkè il pannello è composto da 2 triangoli
	{			
		D3DXVec3TransformCoord(&v0, &D3DXVECTOR3(pVerticesPanel[h].x,	pVerticesPanel[h].y,	pVerticesPanel[h].z),	&finalMatrix);
		D3DXVec3TransformCoord(&v1, &D3DXVECTOR3(pVerticesPanel[h+1].x,	pVerticesPanel[h+1].y,	pVerticesPanel[h+1].z),	&finalMatrix);
		D3DXVec3TransformCoord(&v2, &D3DXVECTOR3(pVerticesPanel[h+2].x,	pVerticesPanel[h+2].y,	pVerticesPanel[h+2].z),	&finalMatrix);

		//Senza D3DX !!! SEMBRA ESSERE PIU VELOCE !!!
		if( DEUtility::RayIntersectsTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2, &fDist, &fBary1, &fBary2 ) )
		{
			return true;
		}
	}
	return false;
}

DLL_API bool DEPanel::GetCreated()
{
	return isCreated;
}

DLL_API bool DEPanel::Invalidate()
{
	if (isCreated)
	{
		try
		{
			if ((frames==1)&&(rows==1))
			{
				return true;
			}
			else
			{
				SafeRelease(vertexBuffer);
				return true;
			}
		}
		catch(...)
		{
			return false;
		}
	}
	return false;
}

DLL_API bool DEPanel::Restore()
{
	HRESULT hr;
	if (isCreated)
	{
		isCreated=false;
		//Dimensioni video
		D3DVIEWPORT9 currentViewport;
		DECore::D3DDevice->GetViewport(&currentViewport);
		screenWidth = currentViewport.Width;
		screenHeight = currentViewport.Height;//*/

		if ((frames==1)&&(rows==1))
		{
			return true;
		}
		else
		{
			//Ricrea il VertexBuffer se non è stato creato in modo managed
			hr = DECore::D3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX_PANEL), vertexProcessingUsage| D3DUSAGE_DYNAMIC, CUSTOMVERTEX_PANEL_D3DFVF,D3DPOOL_DEFAULT, &vertexBuffer, NULL);
			if (hr == D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = vertexBuffer->Lock(0, 4 * sizeof(CUSTOMVERTEX_PANEL), (void**)&pVerticesPanel, 0);
				if (hr == D3D_OK)
				{
					//Set the positions of the vertices
					pVerticesPanel[0].x = -(panelWidth) / 2.0f;
					pVerticesPanel[0].y = -(panelHeight) / 2.0f;						
					pVerticesPanel[1].x = -(panelWidth) / 2.0f;
					pVerticesPanel[1].y = panelHeight / 2.0f;
					pVerticesPanel[2].x = (panelWidth) / 2.0f;
					pVerticesPanel[2].y = -(panelHeight) / 2.0f;
					pVerticesPanel[3].x = (panelWidth) / 2.0f;
					pVerticesPanel[3].y = panelHeight / 2.0f;
					pVerticesPanel[0].z = 1.0f;
					pVerticesPanel[1].z = 1.0f;
					pVerticesPanel[2].z = 1.0f;
					pVerticesPanel[3].z = 1.0f;
					//Set the texture coordinates of the vertices
					pVerticesPanel[0].u = 0.0f;
					pVerticesPanel[0].v = 1.0f;
					pVerticesPanel[1].u = 0.0f;
					pVerticesPanel[1].v = 0.0f;
					pVerticesPanel[2].u = 1.0f;
					pVerticesPanel[2].v = 1.0f;
					pVerticesPanel[3].u = 1.0f;
					pVerticesPanel[3].v = 0.0f;

					//Sblocca il VertexBuffer
					hr = vertexBuffer->Unlock();
					if (hr==D3D_OK)
					{
						//Frames
						oldFrame = -1;
						GoToFrame(currentFrame);
						isCreated=true;
						return true;
					}
				}
			}//*/
		}
	}
	return false;
}

DLL_API bool DEPanel::Release()
{
	try
	{
		DETextureManager::ReleaseTexture(textureID);
		SafeRelease(vertexBuffer);
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

DLL_API bool DEPanel::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEPanel::GetTag()
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