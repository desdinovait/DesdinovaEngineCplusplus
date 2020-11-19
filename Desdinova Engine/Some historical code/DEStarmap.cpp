#include "DEGeneralIncludes.h"
#include "DEStarmap.h"

DLL_API DEStarmap::DEStarmap()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DEStarmap::Create(STRUCT_DE_FOV fovProperties, LPCSTR logTitle)
{
	char strLog[200];
	mainLogTitle = logTitle;
	isCreated = false;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{
			//Fov
			mainFovProperties = fovProperties;

			Planet.Create("Textures\\Flares\\Ray01.dds", D3DXCOLOR(255,0,0,0), "Planet");
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

DLL_API void DEStarmap::CalculatePosition(D3DXVECTOR3 viewPos)
{
	if ((DECore::D3DDevice!=NULL)&&(isCreated))
	{
		mainViewPos = viewPos;
	}
}

DLL_API void DEStarmap::Render3D()
{
	if (isCreated)
	{
		Planet.SetPosition(D3DXVECTOR3(0,20,0));
		Planet.SetScaleEqual(25.0f);
		Planet.Render(false, false, 200, 300, 800, 300, mainViewPos, false, mainFovProperties);
	}
}

DLL_API void DEStarmap::Render2D()
{
	if (isCreated)
	{
	}
}

DLL_API bool DEStarmap::Invalidate()
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

DLL_API bool DEStarmap::Restore()
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


DLL_API bool DEStarmap::Release()
{
	try
	{
		//Rilascia il flares
		if ((isCreated))
		{

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


DLL_API bool DEStarmap::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEStarmap::GetTag()
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
