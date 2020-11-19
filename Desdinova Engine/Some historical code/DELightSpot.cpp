#include "DEGeneralIncludes.h"
#include "DELightSpot.h"

DLL_API DELightSpot::DELightSpot()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isEnable = false;
}

DLL_API bool DELightSpot::Create(LPCSTR logTitle)
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
			
				//SPOT Light
				d3dLight.Type = D3DLIGHT_SPOT;
				
				d3dLight.Position.x = 0.0f;
				d3dLight.Position.y = 0.0f;
				d3dLight.Position.z = -100.0f;

				d3dLight.Direction.x = 0.0f;
				d3dLight.Direction.y = 0.0f;
				d3dLight.Direction.z = 100.0f;

				d3dLight.Diffuse.r = 1.0f;
				d3dLight.Diffuse.g = 1.0f;
				d3dLight.Diffuse.b = 1.0f;
				d3dLight.Diffuse.a = 0.0f;

				d3dLight.Ambient.r = 0.0f;
				d3dLight.Ambient.g = 0.0f;
				d3dLight.Ambient.b = 0.0f;
				d3dLight.Ambient.a = 0.0f;

				d3dLight.Specular.r = 0.0f;
				d3dLight.Specular.g = 0.0f;
				d3dLight.Specular.b = 0.0f;
				d3dLight.Specular.a = 0.0f;

				d3dLight.Attenuation0 = 1.0f; 
				d3dLight.Attenuation1 = 0.0f; 
				d3dLight.Attenuation2 = 0.0f; 
				d3dLight.Range = 100.0f;

				//Solo per SPOT Light :-)
				d3dLight.Falloff = 1.0f;
				d3dLight.Phi = D3DXToRadian(50);
				d3dLight.Theta = D3DXToRadian(60);

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

DLL_API void DELightSpot::SetPriority(unsigned short newPriority)
{	
	priority = newPriority;
}

DLL_API void DELightSpot::SetPosition(float posX, float posY, float posZ)
{	
	d3dLight.Position.x = posX;
	d3dLight.Position.y = posY;
	d3dLight.Position.z = posZ;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetDirection(float dirX, float dirY, float dirZ)
{
	d3dLight.Direction.x = dirX;
	d3dLight.Direction.y = dirY;
	d3dLight.Direction.z = dirZ;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetDiffuse(float diffuseR, float diffuseG, float diffuseB, float diffuseA)
{
	d3dLight.Diffuse.r = diffuseR;
	d3dLight.Diffuse.g = diffuseG;
	d3dLight.Diffuse.b = diffuseB;
	d3dLight.Diffuse.a = diffuseA;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetAmbient(float ambientR, float ambientG, float ambientB, float ambientA)
{
	d3dLight.Ambient.r = ambientR;
	d3dLight.Ambient.g = ambientG;
	d3dLight.Ambient.b = ambientB;
	d3dLight.Ambient.a = ambientA;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetSpecular(float specularR, float specularG, float specularB, float specularA)
{
	d3dLight.Specular.r = specularR;
	d3dLight.Specular.g = specularG;
	d3dLight.Specular.b = specularB;
	d3dLight.Specular.a = specularA;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetAttenuation(float attenuation0, float attenuation1, float attenuation2)
{
	d3dLight.Attenuation0 = attenuation0; 
	d3dLight.Attenuation1 = attenuation1; 
	d3dLight.Attenuation2 = attenuation2; 
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetRange(float range)
{
	d3dLight.Range = range;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetFalloff(float falloff)
{
	d3dLight.Falloff = falloff;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetPhi(float phi)
{
	d3dLight.Phi = phi;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetTheta(float theta)
{
	d3dLight.Theta = theta;
	DECore::D3DDevice->SetLight(index, &d3dLight);
}

DLL_API void DELightSpot::SetDinamicEnable(bool newDinamicEnable)
{
	isDinamicEnable = newDinamicEnable;
}

DLL_API void DELightSpot::SetEnable(bool newEnable)
{
	isEnable = newEnable;
}

DLL_API D3DXMATRIX DELightSpot::CalculateOccupationMatrix()
{
	//Matrice di scalatura locale
	D3DXMATRIX scaleMatrix;
	D3DXMATRIX posMatrix;
	D3DXMATRIX rotMatrix;
	D3DXMatrixScaling(&scaleMatrix, d3dLight.Range, d3dLight.Range, d3dLight.Range);
	D3DXMatrixTranslation(&posMatrix, d3dLight.Position.x, d3dLight.Position.y, d3dLight.Position.z);
	D3DXMatrixRotationYawPitchRoll(&rotMatrix, 0, 0, 0);
	
	//Matrice finali
	return scaleMatrix * rotMatrix * posMatrix;	
}

DLL_API bool DELightSpot::CheckDistance(D3DXVECTOR3 centerCamera)
{
	//*** DISTANZA DALLA CAMERA ***
	//Calcola la distanza del centro dell'oggetto alla camera
	D3DXVECTOR3 diffFar;
	D3DXVec3Subtract( &diffFar, &centerCamera, &(D3DXVECTOR3)d3dLight.Position);
	//Rapporto tra raggio e distanza dalla camera
	float distanceRapp = d3dLight.Range / D3DXVec3Length(&diffFar);
	//Se il rapporto scende sotto l'1%(1/100=0.01) allora si trascura il modello
	if ( distanceRapp < 0.01f)	isFar = true;
	else						isFar = false;

	return isFar;
}

DLL_API bool DELightSpot::CheckPosition(D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties)
{
	try
	{
		D3DXMATRIX viewTransform;
		DECore::D3DDevice->GetTransform(D3DTS_VIEW,& viewTransform);
		if (DEUtility::FOVCheck(d3dLight.Position, d3dLight.Range, viewTransform, fovProperties, &viewPos))	
		{
			//La luce è nel frustrum
			isCulled = false;
			return false;
		}
		else
		{
			//la luce è fuori dal frustrum
			isCulled = true;
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API unsigned short DELightSpot::GetPriority()
{	
	return priority;
}

DLL_API int DELightSpot::GetIndex()
{
	return index;
}

DLL_API bool DELightSpot::GetDinamicEnable()
{
	return isDinamicEnable;
}

DLL_API bool DELightSpot::GetEnable()
{
	return isEnable;
}

DLL_API bool DELightSpot::GetCulled()
{
	return isCulled;
}

DLL_API bool DELightSpot::GetFar()
{
	return isFar;
}

DLL_API bool DELightSpot::GetCreated()
{
	return isCreated;
}

DLL_API bool DELightSpot::Invalidate()
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

DLL_API bool DELightSpot::Restore()
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

DLL_API bool DELightSpot::Release()
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

DLL_API bool DELightSpot::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DELightSpot::GetTag()
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
DLL_API void DELightSpot::Draw()
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