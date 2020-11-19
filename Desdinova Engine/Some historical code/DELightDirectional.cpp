#include "DEGeneralIncludes.h"
#include "DELightDirectional.h"

DLL_API DELightDirectional::DELightDirectional()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isEnable = false;
}

DLL_API bool DELightDirectional::Create(LPCSTR logTitle)
{
	HRESULT hr=NULL;
	mainLogTitle = logTitle;
	isEnable = false;
	index=0;
	maxLights = 500;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{	
			//Calcola l'index della luce corrente in base alle luci già settate
			D3DLIGHT9 pLight;
			unsigned int c=0;
			for (c=0; c<maxLights; c++)
			{
				//Trova la prima libera non settata
				hr = DECore::D3DDevice->GetLight(c, &pLight);
				if (hr!=D3D_OK)
				{
					index=c;
					break;
				}
			}

			if (c<maxLights)
			{
				//Inizializza la struttura
				ZeroMemory(&d3dLight, sizeof(D3DLIGHT9));
			
				//DIRECTIONAL Light
				d3dLight.Type = D3DLIGHT_DIRECTIONAL;
				
				d3dLight.Direction.x = 0.0f;
				d3dLight.Direction.y = 0.0f;
				d3dLight.Direction.z = 1000.0f;

				d3dLight.Diffuse.r = 1.0f;
				d3dLight.Diffuse.g = 1.0f;
				d3dLight.Diffuse.b = 1.0f;

				d3dLight.Ambient.r = 0.0f;
				d3dLight.Ambient.g = 0.0f;
				d3dLight.Ambient.b = 0.0f;

				d3dLight.Specular.r = 0.0f;
				d3dLight.Specular.g = 0.0f;
				d3dLight.Specular.b = 0.0f;

				//Priorità generale di abilitazione (10=max, 0=min)
				priority = 0;

				//Assegna la luce
				hr = DECore::D3DDevice->SetLight(index, &d3dLight);
				if (hr==D3D_OK)
				{
					isCreated=true;
				}
				else
				{
					DELog::LogError("<li>%s : DECore::D3DDevice->SetLight(...): %s", logTitle, DXGetErrorDescription9(hr));		
				}
			}
			else
			{
				DELog::LogWarning("<li>%s : Current Light index > MaxActiveLights", logTitle);
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

DLL_API void DELightDirectional::SetPriority(unsigned short newPriority)
{	
	priority = newPriority;
}

DLL_API void DELightDirectional::SetDirection(float dirX, float dirY, float dirZ)
{
	d3dLight.Direction.x = dirX;
	d3dLight.Direction.y = dirY;
	d3dLight.Direction.z = dirZ;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightDirectional::SetDiffuse(float diffuseR, float diffuseG, float diffuseB, float diffuseA)
{
	d3dLight.Diffuse.r = diffuseR;
	d3dLight.Diffuse.g = diffuseG;
	d3dLight.Diffuse.b = diffuseB;
	d3dLight.Diffuse.a = diffuseA;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightDirectional::SetAmbient(float ambientR, float ambientG, float ambientB, float ambientA)
{
	d3dLight.Ambient.r = ambientR;
	d3dLight.Ambient.g = ambientG;
	d3dLight.Ambient.b = ambientB;
	d3dLight.Ambient.a = ambientA;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightDirectional::SetSpecular(float specularR, float specularG, float specularB, float specularA)
{
	d3dLight.Specular.r = specularR;
	d3dLight.Specular.g = specularG;
	d3dLight.Specular.b = specularB;
	d3dLight.Specular.a = specularA;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightDirectional::SetDinamicEnable(bool newDinamicEnable)
{
	isDinamicEnable = newDinamicEnable;
}

DLL_API void DELightDirectional::SetEnable(bool newEnable)
{
	isEnable = newEnable;
}

DLL_API D3DXMATRIX DELightDirectional::CalculateOccupationMatrix()
{
	//***************************************** DA RIVEDERE *************************************************
	//Matrice di scalatura locale
	D3DXMATRIX scaleMatrix;
	D3DXMATRIX posMatrix;
	D3DXMATRIX posMatrix0;
	D3DXMATRIX rotMatrix;
	//D3DXMatrixTranslation(&posMatrix0, 0, 0, 100);
	D3DXMatrixTranslation(&posMatrix, 0, 0, 100);
	D3DXMatrixScaling(&scaleMatrix, 10, 10, 100);
	D3DXMatrixRotationYawPitchRoll(&rotMatrix, 0, 0, 0);
	
	//Matrice finali
	return scaleMatrix * rotMatrix * posMatrix;	
}

DLL_API unsigned short DELightDirectional::GetPriority()
{	
	return priority;
}

DLL_API int DELightDirectional::GetIndex()
{
	return index;
}

DLL_API bool DELightDirectional::GetDinamicEnable()
{
	return isDinamicEnable;
}

DLL_API bool DELightDirectional::GetEnable()
{
	return isEnable;
}

DLL_API bool DELightDirectional::GetCreated()
{
	return isCreated;
}

DLL_API bool DELightDirectional::Invalidate()
{
	try
	{
		if (isEnable)
		{
			DECore::D3DDevice->LightEnable(index, FALSE);
			DEActiveLights = DEActiveLights - 1;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DELightDirectional::Restore()
{
	try
	{
		DECore::D3DDevice->SetLight(index, &d3dLight);
		if (isEnable)
		{
			DECore::D3DDevice->LightEnable(index, TRUE);
			DEActiveLights = DEActiveLights + 1;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DELightDirectional::Release()
{
	try
	{
		DECore::D3DDevice->LightEnable(index, FALSE);
		DEActiveLights = DEActiveLights - 1;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

DLL_API bool DELightDirectional::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DELightDirectional::GetTag()
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


/*
DLL_API void DELightDirectional::Draw()
{

	D3DXMATRIX positionLight;
	D3DXMatrixIdentity(&positionLight);

	if (showRange)
	{
		D3DXMatrixTranslation(&positionLight,d3dLight.Position.x,d3dLight.Position.y,d3dLight.Position.z);
		DECore::D3DDevice->SetTransform(D3DTS_WORLD,&positionLight);
		rangeSphere->DrawSubset(0);
	}

	if (showPosition)
	{
		D3DXMatrixTranslation(&positionLight,d3dLight.Position.x,d3dLight.Position.y,d3dLight.Position.z);
		DECore::D3DDevice->SetTransform(D3DTS_WORLD,&positionLight);
		positionSphere->DrawSubset(0);
	}

}
*/