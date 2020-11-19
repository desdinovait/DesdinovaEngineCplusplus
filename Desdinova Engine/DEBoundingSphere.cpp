#include "DEGeneralIncludes.h"

DLL_API DEBoundingSphere::DEBoundingSphere()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DEBoundingSphere::Create(LPCSTR logTitle)
{
	mainLogTitle = logTitle;
	isCreated = false;

	try
	{
		D3DXMatrixIdentity(&finalMatrix);
		isCreated = DEVBManager::GetBoundingSphereGetCreated();

		if (DECore::engineEffect != NULL)
		{
			SetColor(1.0f, 1.0f, 1.0f);
		}
		else
		{
			isCreated=false;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}


DLL_API void DEBoundingSphere::SetColor(float r, float g, float b)
{
	color[0] =  r;
	color[1] =  g;
	color[2] =  b;
	color[3] =  0.0f;
}


DLL_API void DEBoundingSphere::CalculateMatrix(D3DXMATRIX newMatrix)
{
	//Setta la matrice
	finalMatrix = newMatrix;
}

DLL_API void DEBoundingSphere::CalculateMatrix(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 rotation)
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


DLL_API int DEBoundingSphere::Render()
{

	if (isCreated)
	{
		//DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );
	
		D3DXMATRIX m_matView;
		D3DXMATRIX m_matProj;
		DECore::D3DDevice->GetTransform(D3DTS_PROJECTION, &m_matProj);
		DECore::D3DDevice->GetTransform(D3DTS_VIEW, &m_matView);
		D3DXMATRIX worldViewProj = finalMatrix * m_matView * m_matProj;
		DECore::engineEffect->SetMatrix("matWorldViewProj", &worldViewProj);
		DECore::engineEffect->SetFloatArray("wireColor", color, 4);
		DECore::engineEffect->SetTechnique("Wireframe");
		
		UINT uPasses;
		DECore::engineEffect->Begin( &uPasses, 0 ); 
		for( UINT uPass = 0; uPass < uPasses; ++uPass )
		{
			//Inizia il passo corrente
			DECore::engineEffect->BeginPass( uPass );
			DECore::engineEffect->CommitChanges();			
			//Disegna la primitiva		
			DECore::D3DDevice->SetStreamSource(0, DEVBManager::GetBoundingSphereVB(), 0, DEVBManager::GetBoundingSphereSize());
			DECore::D3DDevice->SetFVF(DEVBManager::GetBoundingSphereFVF());
			DECore::D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, DEVBManager::GetBoundingSphereDimension() - 2);
			//Finisce il passo corrente
			DECore::engineEffect->EndPass();
		}
		DECore::engineEffect->End();
	
		return 2;
	}
	else
	{
		return 0;
	}
}


DLL_API bool DEBoundingSphere::Invalidate()
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

DLL_API bool DEBoundingSphere::Restore()
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

DLL_API bool DEBoundingSphere::Release()
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
	return false;
}

DLL_API inline D3DXMATRIX DEBoundingSphere::GetFinalMatrix()
{
	return finalMatrix;
}

DLL_API inline bool DEBoundingSphere::GetCreated()
{
	return isCreated;
}

DLL_API bool DEBoundingSphere::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEBoundingSphere::GetTag()
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