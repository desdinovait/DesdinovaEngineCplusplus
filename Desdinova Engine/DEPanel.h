#ifndef H_BSPMAP_H
#define H_BSPMAP_H

//** Stutture e altre definizioni **
#define CUSTOMVERTEX_PANEL_D3DFVF (D3DFVF_XYZ|D3DFVF_TEX1)
struct CUSTOMVERTEX_PANEL2
{
	float x, y, z;
	float u, v;
};		

/*
[CLASS] 
[
    [NOME] 
    DEPanel
    
    [DESCRIZIONE]   
    Classe utilizzata per creare dei pannelli 2D posizionabili nell'area di
	rendering attraverso le 2 coordinate X e Y.
	Il pannello può avere anche una sequenza di immagini in modo da formare
	una semplice animazione.
]
*/
class DEPanel
{
	private:
		LPDIRECT3DVERTEXBUFFER9		vertexBuffer;		
		CUSTOMVERTEX_PANEL2			*pVerticesPanel;
		DWORD						vertexProcessingUsage;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		bool						isCreated;
		bool						renderMe;

		D3DXMATRIX					finalMatrix;
		D3DXMATRIX					positionMatrix;
		D3DXMATRIX					rotationMatrix;
		D3DXMATRIX					scaleMatrix;		
		D3DXVECTOR2					panelPosition;
		D3DXVECTOR2					panelPositionPick;
		float						panelRotation;
		D3DXVECTOR2					panelScale;
		D3DXVECTOR2					panelScale2;
		bool						panelFromCenter;
		unsigned int				screenWidth;
		unsigned int				screenHeight;
		
		int							panelTextureID;
		vector<int>					panelTextureList;
		D3DXCOLOR					panelColour;

		vector<D3DXVECTOR2>			panelGridList;
		vector<unsigned int>		panelInitFrameList;
		vector<unsigned int>		panelOldFrameList;
		vector<unsigned int>		panelCurrentFrameList;


	public:
		//Gestione
		DLL_API						DEPanel();
		DLL_API bool				Create(LPCSTR logTitle);
		DLL_API int					AddTexture(LPCSTR textureName, D3DCOLOR dwKeyColour, unsigned int totalRows=1, unsigned int framesPerRow=1, unsigned int initialFrame=0);
		DLL_API void				Render();
		DLL_API bool				Invalidate();
		DLL_API bool				Restore();
		DLL_API bool				Release();
		//Piking
		DLL_API bool				CheckPicked(D3DXVECTOR2 ptCursor);
		//Settaggi
		DLL_API bool				GoToFrame(unsigned int frame);
		DLL_API void				SetMode(bool positionFromCenter);
		DLL_API bool				GetMode();
		DLL_API void				SetPosition(D3DXVECTOR2 newPosition, bool sceenPercentual=false);
		DLL_API D3DXVECTOR2			GetPosition();
		DLL_API void				SetRotation(float newRotation);
		DLL_API float				GetRotation();
		DLL_API void				SetScale(D3DXVECTOR2 newScale);
		DLL_API D3DXVECTOR2			GetScale();
		DLL_API void				SetDimensions(D3DXVECTOR2 newDimensions, bool screenPercentual=false);
		DLL_API D3DXVECTOR2			GetDimensions();
		DLL_API void				SetTextureID(unsigned int textureID);
		DLL_API unsigned int		GetTextureID();
        DLL_API D3DXVECTOR2			GetTextureInfoID(unsigned int textureID, bool fileDimension=true);
		DLL_API void				SetRenderMe(bool setRenderMe);
		DLL_API bool				GetRenderMe();
		DLL_API bool				GetCreated();
		DLL_API void				SetColor(D3DCOLOR newPanelColor);
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();

};

#endif
