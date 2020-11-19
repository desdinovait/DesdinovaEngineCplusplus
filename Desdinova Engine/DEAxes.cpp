#include "DEGeneralIncludes.h"

DLL_API DEAxes::DEAxes()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DEAxes::Create(LPCSTR logTitle)
{
	mainLogTitle = logTitle;
	isCreated = false;

	try
	{
		D3DXMatrixIdentity(&finalMatrix);
		isCreated = DEVBManager::GetAxesGetCreated();
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);	

	return isCreated;
}

DLL_API void DEAxes::CalculateMatrix(D3DXMATRIX newMatrix)
{
	//Setta la matrice
	finalMatrix = newMatrix;
}

DLL_API void DEAxes::CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation)
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


DLL_API void DEAxes::Render()
{
	if (isCreated)
	{
		SetDefaultRenderStates();

		DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );

		DECore::D3DDevice->SetStreamSource( 0, DEVBManager::GetAxesVB(), 0,  DEVBManager::GetAxesSize() );
		DECore::D3DDevice->SetFVF( DEVBManager::GetAxesFVF() );
		DECore::D3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
	}
}

DLL_API void DEAxes::SetDefaultRenderStates()
{
	DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	
	DECore::D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );				
		//DECore::D3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(96,96,96));
	
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

DLL_API bool DEAxes::Invalidate()
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

DLL_API bool DEAxes::Restore()
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

DLL_API bool DEAxes::Release()
{
	if (isCreated)
	{
		try
		{
			isCreated=false;
			return true;
		}
		catch(...)
		{
			DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);
			return false;
		}
	}
	else
	{
		if (mainLogTitle == NULL)	DELog::LogWarning("<li>DEAxes : Object NOT created - Impossible to release");
		else						DELog::LogWarning("<li>%s : Object NOT Created - Impossible to Release", mainLogTitle);
		return false;
	}
	return false;
}

DLL_API inline bool DEAxes::GetCreated()
{
	return isCreated;
}

DLL_API bool DEAxes::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEAxes::GetTag()
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