#include "DEGeneralIncludes.h"

DLL_API DELight::DELight()
{
	mainLogTitle = NULL;
	mainTag = NULL;
}

DLL_API bool DELight::Create(LPCSTR textureName, LPCSTR logTitle)
{
	HRESULT hr=NULL;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{	
			lightBill.Create(textureName, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), "lightBillboard"); 

			light.Light_Type = DE_LIGHTTYPE_PERPIXELPOINT;
			
			light.Light_Direction.x = 0.0f;
			light.Light_Direction.y = 0.0f;
			light.Light_Direction.z = 0.0f;

			light.Light_Position.x = 0.0f;
			light.Light_Position.y = 0.0f;
			light.Light_Position.z = 0.0f;

			light.Light_Color.r = 1.0f;
			light.Light_Color.g = 1.0f;
			light.Light_Color.b = 1.0f;
			light.Light_Color.a = 1.0f;

			light.Light_Falloff = 128.0f;
			light.Light_Range = 100.0f;

			light.Light_Enable = true;
			light.Light_DinamicEnable = true;

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


DLL_API void DELight::SetDinamicEnable(bool newDinamicEnable)
{
	light.Light_DinamicEnable = newDinamicEnable;
}

DLL_API void DELight::SetEnable(bool newEnable)
{
	light.Light_Enable = newEnable;
}

DLL_API void DELight::SetType(ENUM_DE_LIGHTTYPE type)
{
	light.Light_Type = type;
}

DLL_API void DELight::SetDirection(D3DXVECTOR3 dir)
{
	light.Light_Direction = dir;
}

DLL_API void DELight::SetPosition(D3DXVECTOR3 pos)
{	
	light.Light_Position = pos;
	lightBill.SetPosition(pos);
}

DLL_API void DELight::SetDiffuse(D3DXCOLOR color)
{
	light.Light_Color = color;
	lightBill.SetColor(color);
}

DLL_API void DELight::SetFalloff(float falloff)
{
	light.Light_Falloff = falloff;
}

DLL_API void DELight::SetRange(float range)
{
	light.Light_Range = range;
	lightBill.SetScaleEqual(range/5.0f);
}



DLL_API void DELight::Render(D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties)
{
	lightBill.Render(false,false,0,0,0,0,centerCamera, false, fovProperties);
}


DLL_API D3DXMATRIX DELight::CalculateOccupationMatrix()
{
	D3DXMATRIX nullMatrix;
	D3DXMatrixIdentity(&nullMatrix);

	if (light.Light_Type == 1) //Solo omni
	{
		//Matrice di scalatura locale
		D3DXMATRIX scaleMatrix;
		D3DXMATRIX posMatrix;
		D3DXMATRIX rotMatrix;
		D3DXMatrixScaling(&scaleMatrix, light.Light_Range, light.Light_Range, light.Light_Range);
		D3DXMatrixTranslation(&posMatrix, light.Light_Position.x, light.Light_Position.y, light.Light_Position.z);
		D3DXMatrixRotationYawPitchRoll(&rotMatrix, 0, 0, 0);
		
		//Matrice finali
		return scaleMatrix * rotMatrix * posMatrix;	
	}
	else
	{
		return nullMatrix;
	}
	return nullMatrix;
}
DLL_API bool DELight::CheckDistance(D3DXVECTOR3 centerCamera)
{
	if (light.Light_Type == 1) //Solo omni
	{
		//*** DISTANZA DALLA CAMERA ***
		//Calcola la distanza del centro dell'oggetto alla camera
		D3DXVECTOR3 diffFar;
		D3DXVec3Subtract( &diffFar, &centerCamera, &(D3DXVECTOR3)light.Light_Position);
		//Rapporto tra raggio e distanza dalla camera
		float distanceRapp = light.Light_Range / D3DXVec3Length(&diffFar);
		//Se il rapporto scende sotto l'1%(1/100=0.01) allora si trascura il modello
		if ( distanceRapp < 0.01f)	isFar = true;
		else						isFar = false;
		return isFar;
	}
	else
	{
		return false;
	}
	return false;
}
DLL_API bool DELight::CheckPosition(D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties)
{
	try
	{
		if (light.Light_Type == 1) //Solo omni
		{
			D3DXMATRIX viewTransform;
			DECore::D3DDevice->GetTransform(D3DTS_VIEW,& viewTransform);
			if (DEUtility::FOVCheck(light.Light_Position, light.Light_Range, viewTransform, fovProperties, &viewPos))	
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
		else
		{
			isCulled = true;
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}


DLL_API bool DELight::GetDinamicEnable()
{
	return light.Light_DinamicEnable;
}

DLL_API bool DELight::GetEnable()
{
	return light.Light_Enable;
}

DLL_API STRUCT_DE_LIGHT DELight::GetLightProperties()
{
	return light;
}

DLL_API bool DELight::GetCulled()
{
	return isCulled;
}

DLL_API bool DELight::GetFar()
{
	return isFar;
}

DLL_API bool DELight::GetCreated()
{
	return isCreated;
}

DLL_API bool DELight::Invalidate()
{
	try
	{
		if (light.Light_Enable)
		{
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DELight::Restore()
{
	try
	{
		if (light.Light_Enable)
		{
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DELight::Release()
{
	try
	{
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

DLL_API bool DELight::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DELight::GetTag()
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