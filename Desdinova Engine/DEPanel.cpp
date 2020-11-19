#include "DEGeneralIncludes.h"

DLL_API DEPanel::DEPanel()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMatrixIdentity(&positionMatrix);
	D3DXMatrixIdentity(&rotationMatrix);
	D3DXMatrixIdentity(&scaleMatrix);
	panelFromCenter = false;
	isCreated = false;
}


DLL_API bool DEPanel::Create(LPCSTR logTitle)
{
	HRESULT hr;
	isCreated=false;
	renderMe=true;
	mainLogTitle = logTitle;
	panelPosition.x = 0.0f;
	panelPosition.y = 0.0f;
	panelRotation = 0.0f;
	panelScale.x = 50.0f;
	panelScale.y = 50.0f;
	panelScale2.x = 1.0f;
	panelScale2.y = 1.0f;
	panelFromCenter = false;
	panelColour = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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
			hr = DECore::D3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX_PANEL2), vertexProcessingUsage, CUSTOMVERTEX_PANEL_D3DFVF, D3DPOOL_MANAGED, &vertexBuffer, NULL);
			if (hr == D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = vertexBuffer->Lock(0, 4 * sizeof(CUSTOMVERTEX_PANEL2), (void**)&pVerticesPanel, 0);
				if (hr == D3D_OK)
				{
					//Set the positions of the vertices
					pVerticesPanel[0].x = -1.0f / 2.0f;
					pVerticesPanel[0].y = -1.0f / 2.0f;
					
					pVerticesPanel[1].x = -1.0f / 2.0f;
					pVerticesPanel[1].y = 1.0f / 2.0f;
					
					pVerticesPanel[2].x = 1.0f / 2.0f;
					pVerticesPanel[2].y = -1.0f / 2.0f;
					
					pVerticesPanel[3].x = 1.0f / 2.0f;
					pVerticesPanel[3].y = 1.0f / 2.0f;
					
					
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


DLL_API void DEPanel::SetPosition(D3DXVECTOR2 newPosition, bool screenPercentual)
{
	panelPosition = newPosition;
	panelPositionPick = newPosition;	//Evita errori con il pick
	//Dimensioni video
	D3DVIEWPORT9 currentViewport;
	DECore::D3DDevice->GetViewport(&currentViewport);
	screenWidth = currentViewport.Width;
	screenHeight = currentViewport.Height;
	
	//Se è in percentuale ridefinisce la posizione
	if (screenPercentual)
	{
		D3DVIEWPORT9 currentViewport;
		DECore::D3DDevice->GetViewport(&currentViewport);
		screenWidth = currentViewport.Width;
		screenHeight = currentViewport.Height;
		panelPosition.x = ((float)screenWidth / 100.0f) * panelPosition.x;
		panelPosition.y = ((float)screenHeight / 100.0f) * panelPosition.y;
		panelPositionPick = panelPosition;	//Evita errori con il pick
	}
		
	//Stabilisce la posizione partendo dal centro o meno
	if (panelFromCenter)
	{
		panelPosition.x -= (screenWidth/2.0f) - (panelScale.x/2.0f) + (GetDimensions().x/2.0f );
		panelPosition.y -= (screenHeight/2.0f) - (panelScale.y/2.0f) + (GetDimensions().y/2.0f );
	}
	else
	{
		panelPosition.x -= (screenWidth/2.0f) - (panelScale.x/2.0f);
		panelPosition.y -= (screenHeight/2.0f) - (panelScale.y/2.0f);
	}

}
DLL_API D3DXVECTOR2 DEPanel::GetPosition()
{
	return panelPosition;
}

DLL_API void DEPanel::SetRotation(float newRotation)
{
	panelRotation = newRotation;
}
DLL_API float DEPanel::GetRotation()
{
	return panelRotation;
}

DLL_API void DEPanel::SetScale(D3DXVECTOR2 newScale)
{
	panelScale2 = newScale;
}
DLL_API D3DXVECTOR2 DEPanel::GetScale()
{
	return panelScale2;
}

DLL_API void DEPanel::SetDimensions(D3DXVECTOR2 newDimensions, bool screenPercentual)
{
	if (screenPercentual)
	{
		D3DVIEWPORT9 currentViewport;
		DECore::D3DDevice->GetViewport(&currentViewport);
		screenWidth = currentViewport.Width;
		screenHeight = currentViewport.Height;
		panelScale.x = ((float)screenWidth / 100.0f) * newDimensions.x;
		panelScale.y = ((float)screenHeight / 100.0f) * newDimensions.y;
	}
	else
	{
		panelScale = newDimensions;
	}
}
DLL_API D3DXVECTOR2 DEPanel::GetDimensions()
{
	return panelScale;
}

DLL_API void DEPanel::SetMode(bool positionFromCenter)
{
	panelFromCenter = positionFromCenter;
}
DLL_API bool DEPanel::GetMode()
{
	return panelFromCenter;
}

DLL_API void DEPanel::SetColor(D3DCOLOR newPanelColor)
{
	if (newPanelColor != panelColour)	panelColour = newPanelColor;
}

DLL_API int DEPanel::AddTexture(LPCSTR textureName, D3DCOLOR dwKeyColour, unsigned int totalRows, unsigned int framesPerRow, unsigned int initialFrame)
{
	if (textureName != NULL)
	{
		int sceneCursorTextureID = DETextureManager::CreateTexture(textureName, dwKeyColour, false);
		if ((sceneCursorTextureID < 0)||(sceneCursorTextureID > 1000))
		{								
			DELog::LogWarning("<li>D3DXCreateTextureFromFileEx(%s) - Panel may appear without texture.", textureName);
			return -1;
		}
		else
		{
			panelTextureList.push_back(sceneCursorTextureID);
			D3DXVECTOR2	vectorR;
			vectorR.x = (float)totalRows;
			vectorR.y = (float)framesPerRow;
			panelGridList.push_back(vectorR);
			panelInitFrameList.push_back(initialFrame);
			panelCurrentFrameList.push_back(0);
			panelOldFrameList.push_back(0);
			panelTextureID=0;
			GoToFrame(0);

			return panelTextureList.size()-1;
		}
	}
	return -1;
}
DLL_API void DEPanel::SetTextureID(unsigned int textureID)
{
	panelTextureID = textureID;
}
DLL_API unsigned int DEPanel::GetTextureID()
{
	return panelTextureID;
}
DLL_API D3DXVECTOR2 DEPanel::GetTextureInfoID(unsigned int textureID, bool fileDimension)
{
	D3DXVECTOR2 aaa;
	if (fileDimension)
	{
		//Dimensione totale della texture
		aaa.x = (float)DETextureManager::GetTexture(panelTextureList.at(textureID)).Texture_Info.Width;
		aaa.y = (float)DETextureManager::GetTexture(panelTextureList.at(textureID)).Texture_Info.Height;
	}
	else
	{
		//Diemnsione del singolo frame :-)
		aaa.x = (float)DETextureManager::GetTexture(panelTextureList.at(textureID)).Texture_Info.Width / panelGridList.at(panelTextureID).y;
		aaa.y = (float)DETextureManager::GetTexture(panelTextureList.at(textureID)).Texture_Info.Height / panelGridList.at(panelTextureID).x;
	}
	return aaa;
}



DLL_API bool DEPanel::GoToFrame(unsigned int frame)
{	
	unsigned int totalFrames = (unsigned int)panelGridList.at(panelTextureID).x * (unsigned int)panelGridList.at(panelTextureID).y;
	if ((totalFrames>=1)&&(frame <= totalFrames))
	{
		//if (panelGridList.at(panelTextureID).x != panelOldFrameList.at(panelTextureID))
		{
			CUSTOMVERTEX_PANEL2 *verts;
			//Accediamo alla memoria del vertex buffer
			HRESULT hr = vertexBuffer->Lock(0,0,(void**)&verts,0);
			if (hr == D3D_OK)
			{
				verts = pVerticesPanel;
				

				panelCurrentFrameList.insert(panelCurrentFrameList.begin( ) + panelTextureID, frame);
				panelCurrentFrameList.erase(panelCurrentFrameList.begin( ) + panelTextureID + 1);
				
				panelOldFrameList.insert(panelOldFrameList.begin( ) + panelTextureID, frame);
				panelOldFrameList.erase(panelOldFrameList.begin( ) + panelTextureID + 1);
				
				float moverX  = 1.0f / (float)panelGridList.at(panelTextureID).y;	//frames x row
				float moverY  = 1.0f / (float)panelGridList.at(panelTextureID).x;	//row
				unsigned int frameY = (unsigned int)(frame/panelGridList.at(panelTextureID).y);

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





DLL_API void DEPanel::Render()
{
	if ((isCreated)&&(renderMe))
	{
		//Azzeramento matrice
		D3DXMatrixIdentity(&finalMatrix);
		D3DXMatrixIdentity(&positionMatrix);
		D3DXMatrixIdentity(&rotationMatrix);
		D3DXMatrixIdentity(&scaleMatrix);

		D3DXMatrixTranslation(&positionMatrix, panelPosition.x , -panelPosition.y, 0.0f);//*/
		D3DXMatrixScaling(&scaleMatrix, panelScale.x * panelScale2.x, panelScale.y * panelScale2.y, 1.0f);
		D3DXMatrixRotationZ(&rotationMatrix, D3DXToRadian(panelRotation));
		
		//Matrice finale
		finalMatrix = scaleMatrix * rotationMatrix * positionMatrix;

		D3DXMATRIX m_matView;
		D3DXMATRIX m_matProj;
		DECore::D3DDevice->GetTransform(D3DTS_PROJECTION, &m_matProj);
		DECore::D3DDevice->GetTransform(D3DTS_VIEW, &m_matView);
		D3DXMATRIX worldViewProj = finalMatrix * m_matView * m_matProj;
		DECore::engineEffect->SetMatrix("matWorldViewProj", &worldViewProj);
		DECore::engineEffect->SetTechnique("Panel");
		DECore::engineEffect->SetTexture("texture0", DETextureManager::GetTexture(panelTextureList.at(panelTextureID)).Texture_Pointer);

		float alphaColor[4] = {panelColour.r, panelColour.g, panelColour.b, panelColour.a};
		DECore::engineEffect->SetFloatArray("panelColor", alphaColor, 4);		


		DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


		//Disegna i vertici		
		UINT uPasses;
		DECore::engineEffect->Begin( &uPasses, 0 ); 
		for( UINT uPass = 0; uPass < uPasses; ++uPass )
		{
			DECore::engineEffect->BeginPass( uPass );
			DECore::engineEffect->CommitChanges();			

			DECore::D3DDevice->SetStreamSource(0,vertexBuffer,0,sizeof(CUSTOMVERTEX_PANEL2));
			DECore::D3DDevice->SetFVF(CUSTOMVERTEX_PANEL_D3DFVF);		
			DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			
			DECore::engineEffect->EndPass();
		}
		DECore::engineEffect->End();


		//Reimposta la trasparenza nulla
		DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	}
}



DLL_API bool DEPanel::CheckPicked(D3DXVECTOR2 ptCursor)
{
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;
	
	//Dimensioni video
	D3DVIEWPORT9 currentViewport;
	DECore::D3DDevice->GetViewport(&currentViewport);
	float engineWidth = (float)currentViewport.Width;
	float engineHeight = (float)currentViewport.Height;

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

DLL_API void DEPanel::SetRenderMe(bool setRenderMe)
{
	renderMe = setRenderMe;
}
DLL_API bool DEPanel::GetRenderMe()
{
	return renderMe;
}

DLL_API bool DEPanel::GetCreated()
{
	return isCreated;
}

DLL_API bool DEPanel::Invalidate()
{
	if (isCreated)
	{

	}
	return false;
}

DLL_API bool DEPanel::Restore()
{
	if (isCreated)
	{
	}
	return false;
}

DLL_API bool DEPanel::Release()
{
	try
	{
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

