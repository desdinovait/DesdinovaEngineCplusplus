#ifndef H_RENDERTOTEXTURE_H
#define H_RENDERTOTEXTURE_H

/*
[CLASS] 
[
    [NOME] 
    DERenderToTexture
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle superfici dinamiche di rendering con relativa generazione di texture

*/
class DERenderToTexture
{
	private:
		//** Variabili **
		LPD3DXRENDERTOSURFACE		RenderToSurface;
		LPDIRECT3DTEXTURE9			SurfaceTexture;
		LPDIRECT3DSURFACE9			OffScreenSurface;
		unsigned int				mainWidth;
		unsigned int				mainHeight;
		unsigned int				mainMips;
		D3DFORMAT					mainFormat;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		bool						isCreated;

	public:
		//** Funzioni **
		//gestione
		DLL_API						DERenderToTexture();
		DLL_API bool				Create(unsigned int textureWidth, unsigned int textureHeight, unsigned int textureMips=1, D3DFORMAT textureFormat=D3DFMT_A8R8G8B8, LPCSTR logTitle="DERenderToTexture");
		DLL_API bool				Invalidate();
		DLL_API bool				Restore();
		DLL_API bool				Release();
		//Scena
		DLL_API bool				BeginScene();
		DLL_API bool				Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
		DLL_API bool				EndScene(DWORD mipFilter=D3DX_DEFAULT);
		//Settaggi correnti
		DLL_API LPDIRECT3DTEXTURE9	GetTexture();
		DLL_API bool				GetCreated();
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();

};

/*

DERenderToTexture Surf;
...
Surf.Create(256, 256, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, "Render");
...
if (Surf.Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER ,D3DCOLOR_XRGB(0,0,0),1.0f,0))
{
	if (Surf.BeginScene())
	{
		Engine.Set3D();
		...
		...
	Surf.EndScene();
	}
}
*/

#endif 
