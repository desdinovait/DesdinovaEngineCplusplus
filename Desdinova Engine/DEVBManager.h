#ifndef H_VBMANAGER_H
#define H_VBMANAGER_H

//BoundingBox
#define CUSTOMVERTEX_BOUNDINGBOX_D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
struct CUSTOMVERTEX_BOUNDINGBOX
{
	float x, y, z;
	D3DCOLOR colour;
};					
//BoundingSphere
#define CUSTOMVERTEX_BOUNDINGSPHERE_D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
struct CUSTOMVERTEX_BOUNDINGSPHERE
{
	float x, y, z;
	D3DCOLOR colour;
};
//Axes
#define CUSTOMVERTEX_AXES_D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
struct CUSTOMVERTEX_AXES
{
	D3DXVECTOR3 pos;
	DWORD color;
};

/*
[CLASS] 
[
    [NOME] 
    DEVBManager
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei vertex buffer.
]
*/
class DEVBManager
{	
	private:
		//** Variabili **
		static CUSTOMVERTEX_BOUNDINGBOX				*pVerticesBoundingBox;
		static LPDIRECT3DVERTEXBUFFER9				VertexBufferBoundingBox;		
		static CUSTOMVERTEX_BOUNDINGSPHERE			*pVerticesBoundingSphere;
		static LPDIRECT3DVERTEXBUFFER9				VertexBufferBoundingSphere;		
		static CUSTOMVERTEX_AXES					*pVerticesAxes;
		static LPDIRECT3DVERTEXBUFFER9				VertexBufferAxes;		
		static int									dimensionSphere;
		static bool									isCreatedBoundingBox;
		static bool									isCreatedBoundingSphere;
		static bool									isCreatedBoundingCone;
		static bool									isCreatedAxes;
		static DWORD								vertexProcessingUsage;
		//** Funzioni **
		DLL_API static bool							CreatePanel();
		DLL_API static bool							CreateBoundingBox();
		DLL_API static bool							CreateBoundingSphere(int Segments=9, int Rings=9);
		DLL_API static bool							CreateAxes();
		DLL_API static void							ReleasePanel();
		DLL_API static void							ReleaseBoundingBox();
		DLL_API static void							ReleaseBoundingSphere();
		DLL_API static void							ReleaseBoundingCone();
		DLL_API static void							ReleaseAxes();

	public:
		//** Funzioni **
		//Creazione degli elementi
		DLL_API static bool							CreateStaticElements();
		DLL_API static void							ReleaseStaticElements();
		DLL_API static bool							InvalidateStaticElements();
		DLL_API static bool							RestoreStaticElements();
		//BoundingBox
		DLL_API static LPDIRECT3DVERTEXBUFFER9		GetBoundingBoxVB();
		DLL_API static unsigned int					GetBoundingBoxSize();
		DLL_API static DWORD						GetBoundingBoxFVF();
		DLL_API static CUSTOMVERTEX_BOUNDINGBOX		GetBoundingBoxVertices(unsigned int vertexID);
		DLL_API static int							GetBoundingBoxDimension();
		DLL_API static bool							GetBoundingBoxGetCreated();
		//BoundingSphere
		DLL_API static LPDIRECT3DVERTEXBUFFER9		GetBoundingSphereVB();
		DLL_API static unsigned int					GetBoundingSphereSize();
		DLL_API static DWORD						GetBoundingSphereFVF();
		DLL_API static CUSTOMVERTEX_BOUNDINGSPHERE	GetBoundingSphereVertices(unsigned int vertexID);
		DLL_API static int							GetBoundingSphereDimension();
		DLL_API static bool							GetBoundingSphereGetCreated();
		//Axes
		DLL_API static LPDIRECT3DVERTEXBUFFER9		GetAxesVB();
		DLL_API static unsigned int					GetAxesSize();
		DLL_API static DWORD						GetAxesFVF();
		DLL_API static bool							GetAxesGetCreated();

};


#endif