#include "DEGeneralIncludes.h"
#include "DERenderToTexture.h"

DLL_API DERenderToTexture::DERenderToTexture()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}

DLL_API bool DERenderToTexture::Create(unsigned int textureWidth, unsigned int textureHeight, unsigned int textureMips, D3DFORMAT textureFormat, LPCSTR logTitle)
{
	HRESULT hr;
	isCreated = false;	
	mainLogTitle = logTitle;

	try
	{
		mainWidth=textureWidth;
		mainHeight=textureHeight;
		mainMips=textureMips;
		mainFormat=textureFormat;
		//Texture dinamica
		hr = D3DXCreateTexture(DECore::D3DDevice, textureWidth, textureHeight, textureMips, D3DUSAGE_RENDERTARGET, textureFormat, D3DPOOL_DEFAULT, &SurfaceTexture);
		if (hr==D3D_OK)
		{
			//Preleva le info della superficie
			D3DSURFACE_DESC desc;
			SurfaceTexture->GetSurfaceLevel(0, &OffScreenSurface);
			OffScreenSurface->GetDesc(&desc);
			//Crea la off-screen surface dove verrà dsegnata la texture dinamica
			hr = D3DXCreateRenderToSurface(DECore::D3DDevice, desc.Width, desc.Height, desc.Format, TRUE, D3DFMT_D16, &RenderToSurface);
			if (hr==D3D_OK)
			{
				isCreated=true;
			}
			else
			{
				DELog::LogError("<li>%s : D3DXCreateRenderToSurface(...): %s", logTitle, DXGetErrorDescription9(hr));		
			}		
		}
		else
		{
			DELog::LogError("<li>%s : D3DXCreateTexture(...): %s", logTitle, DXGetErrorDescription9(hr));		
		}

	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}		


DLL_API bool DERenderToTexture::BeginScene()
{
	HRESULT hr=NULL;
	if (isCreated)
	{
		hr = RenderToSurface->BeginScene(OffScreenSurface, NULL);
		if (hr==D3D_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}

DLL_API bool DERenderToTexture::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	HRESULT hr=NULL;
	if (isCreated)
	{
		hr = DECore::D3DDevice->Clear(Count,pRects,Flags,Color,Z,Stencil );
		if (hr==D3D_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}

DLL_API bool DERenderToTexture::EndScene(DWORD mipFilter)
{
	HRESULT hr=NULL;
	if (isCreated)
	{
		hr = RenderToSurface->EndScene(mipFilter);
		if (hr==D3D_OK)	return true;
		else			return false;
	}
	else
	{
		return false;
	}
}

DLL_API LPDIRECT3DTEXTURE9 DERenderToTexture::GetTexture()
{
	if (isCreated)	return SurfaceTexture;
	else			return NULL;
}

DLL_API bool DERenderToTexture::GetCreated()
{
	return isCreated;
}

DLL_API bool DERenderToTexture::Invalidate()
{
	try
	{
		HRESULT hr = RenderToSurface->OnLostDevice();
		if (hr=D3D_OK)
		{
			SafeRelease(SurfaceTexture);
			SafeRelease(OffScreenSurface);
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DERenderToTexture::Restore()
{
	try
	{
		HRESULT hr=NULL;
		if (isCreated)
		{
			RenderToSurface->OnResetDevice();
			hr = D3DXCreateTexture(DECore::D3DDevice, mainWidth, mainHeight, mainMips, D3DUSAGE_RENDERTARGET, mainFormat, D3DPOOL_DEFAULT, &SurfaceTexture);
			if (hr==D3D_OK)
			{
				//Preleva le info della superficie
				D3DSURFACE_DESC desc;
				SurfaceTexture->GetSurfaceLevel(0, &OffScreenSurface);
				OffScreenSurface->GetDesc(&desc);
				//Crea la off-screen surface dove verrà dsegnata la texture dinamica
				hr = D3DXCreateRenderToSurface(DECore::D3DDevice, desc.Width, desc.Height, desc.Format, TRUE, D3DFMT_D16, &RenderToSurface);
				if (hr==D3D_OK)
				{
					return true;
				}
			}
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DERenderToTexture::Release()
{
	try
	{
		SafeRelease(RenderToSurface);
		SafeRelease(SurfaceTexture);
		SafeRelease(OffScreenSurface);
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

DLL_API bool DERenderToTexture::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DERenderToTexture::GetTag()
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