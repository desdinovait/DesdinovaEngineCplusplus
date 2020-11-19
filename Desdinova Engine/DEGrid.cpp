#include "DEGeneralIncludes.h"

DLL_API DEGrid::DEGrid()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DEGrid::Create(int sides, float sideLenght, D3DCOLOR color, D3DCOLOR centerColor, LPCSTR logTitle)
{
	HRESULT hr;
	VertexBuffer = NULL;
	isCreated=false;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice != NULL)
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

			//Controlla i valori inseriti
			if (sideLenght<=0.0f)	sideLenght=1.0f;
			if (sides<=3)			sides=4;

			//Allocca dinamicamente i vertici
			totalVertices = (sides+1)*(sides+1);
			Vertices = (CUSTOMVERTEX_GRID *)calloc( totalVertices, sizeof(CUSTOMVERTEX_GRID));

			int currentIndex=0;

			for(int i = -sides/2; i <= sides/2; i = i++)
			{
				Vertices[currentIndex].pos = D3DXVECTOR3(-((sides/2)*sideLenght), 0, i*sideLenght);
				if (i==0)	Vertices[currentIndex].color = centerColor;					
				else		Vertices[currentIndex].color = color;
				
				Vertices[currentIndex+1].pos = D3DXVECTOR3((sides/2)*sideLenght, 0, i*sideLenght);
				if (i==0)	Vertices[currentIndex+1].color = centerColor;					
				else		Vertices[currentIndex+1].color = color;

				Vertices[currentIndex+2].pos = D3DXVECTOR3(i*sideLenght, 0, -((sides/2)*sideLenght));
				if (i==0)	Vertices[currentIndex+2].color = centerColor;					
				else		Vertices[currentIndex+2].color = color;
				
				Vertices[currentIndex+3].pos = D3DXVECTOR3(i*sideLenght, 0, (sides/2)*sideLenght);
				if (i==0)	Vertices[currentIndex+3].color = centerColor;					
				else		Vertices[currentIndex+3].color = color;
				
				currentIndex = currentIndex + 4;
			}

			VOID* pVertices;
				
			//Crea il VertexBuffer
			hr = DECore::D3DDevice->CreateVertexBuffer( totalVertices*sizeof(CUSTOMVERTEX_GRID), vertexProcessingUsage, CUSTOMVERTEX_GRID_D3DFVF, D3DPOOL_MANAGED, &VertexBuffer, NULL );
			if (hr == D3D_OK)
			{
				//Blocca il VertexBuffer
				hr = VertexBuffer->Lock( 0, totalVertices*sizeof(CUSTOMVERTEX_GRID), (void**)&pVertices, 0 );
				if (hr == D3D_OK)
				{
					memcpy( pVertices, Vertices, totalVertices*sizeof(CUSTOMVERTEX_GRID) );
					//Slocca il VertexBuffer
					hr = VertexBuffer->Unlock();
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
 				DELog::LogError("<li>%s : DECore::D3DDevice->CreateVertexBuffer(...): %s", logTitle, DXGetErrorDescription9(hr));		
			}

			free(Vertices);
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

DLL_API void DEGrid::CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation)
{
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMATRIX scaleMatrix;
	D3DXMATRIX posMatrix;
	D3DXMATRIX rotMatrix;
	D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&posMatrix, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&rotMatrix, rotation.y, rotation.x, rotation.z);
	//Setta la matrice
	finalMatrix = scaleMatrix * rotMatrix * posMatrix;
}

DLL_API void DEGrid::CalculateMatrix(D3DXMATRIX newMatrix)
{
	//Setta la matrice
	finalMatrix = newMatrix;
}


DLL_API void DEGrid::Render()
{
	if (isCreated)
	{
		SetDefaultRenderStates();
			
		DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );

		DECore::D3DDevice->SetStreamSource( 0, VertexBuffer, 0, sizeof(CUSTOMVERTEX_GRID) );
		DECore::D3DDevice->SetFVF( CUSTOMVERTEX_GRID_D3DFVF );
		DECore::D3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, totalVertices/2 );
	}
}


DLL_API void DEGrid::SetDefaultRenderStates()
{
	DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	
	DECore::D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );				
		//DECore::D3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(0,255,0));
	
	DECore::D3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE);

	DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,TRUE);
		DECore::D3DDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	DECore::D3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE);
	DECore::D3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);
	
	DECore::D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
 		//DECore::D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		//DECore::D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	
	DECore::D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE );
		//DECore::D3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x00000000 );
		//DECore::D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,	D3DCMP_ALWAYS );

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

	DECore::D3DDevice->SetTexture(0, NULL);
		/*//DECore::D3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		//DECore::D3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );*/

}

DLL_API bool DEGrid::Invalidate()
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

DLL_API bool DEGrid::Restore()
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

DLL_API bool DEGrid::Release()
{
	try
	{
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

DLL_API bool DEGrid::GetCreated()
{
	return isCreated;
}

DLL_API bool DEGrid::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEGrid::GetTag()
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