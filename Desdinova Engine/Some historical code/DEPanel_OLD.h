#ifndef H_PANEL_H
#define H_PANEL_H

//** Stutture e altre definizioni **
#define CUSTOMVERTEX_PANEL_D3DFVF (D3DFVF_XYZ|D3DFVF_TEX1)
struct CUSTOMVERTEX_PANEL
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
		//** Variabili **
		LPDIRECT3DVERTEXBUFFER9		vertexBuffer;		
		CUSTOMVERTEX_PANEL			*pVerticesPanel;
		DWORD						vertexProcessingUsage;
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		D3DXMATRIX					finalMatrix;
		D3DXMATRIX					positionMatrix;
		D3DXMATRIX					rotationMatrix;
		D3DXMATRIX					scaleMatrix;		
		D3DXIMAGE_INFO				textureInfo;
		unsigned int				screenWidth;
		unsigned int				screenHeight;
		D3DCOLOR					panelColour;
		D3DCOLOR					panelKeyColour;
		D3DXVECTOR2					panelPosition;
		float						panelRotation;
		D3DXVECTOR2					panelScale;
		bool						panelFromCenter;
		LPDIRECT3DTEXTURE9			dynamicTexture;
		bool						useTransparent;
		D3DXCOLOR					transparentColour;
		int							frames;
		int							rows;
		bool						isCreated;
		unsigned int				textureID;
		int							panelWidth;
		int							panelHeight;
		int							currentFrame;
		int							startFrame;
		int							oldFrame;
		//Animazione
		bool						animationExecute;
		DWORD						animationStart;
		DWORD						animationSpeed;
		bool						animationCycle;
		bool						animationForward;
		//** Funzioni **


	public:
		//** Funzioni **
		//Gestione
		DLL_API						DEPanel();
		DLL_API bool				Create(const char *szTextureFilePath, D3DCOLOR dwKeyColour, int width, int height, unsigned int totalRows=1, unsigned int framesPerRow=1, unsigned int initialFrame=0, LPCSTR logTitle="DEPanel");
		DLL_API bool				Invalidate();
		DLL_API bool				Restore();
		DLL_API bool				Release();
		//Settaggi
		DLL_API void				SetTexture(LPDIRECT3DTEXTURE9 newTexture);
		DLL_API void				SetColor(D3DCOLOR newPanelColor);
		DLL_API void				SetMode(bool positionFromCenter);
		DLL_API void				SetPosition(D3DXVECTOR2 newPosition);
		DLL_API void				SetRotation(float newRotation);
		DLL_API void				SetScale(D3DXVECTOR2 newScale);
		DLL_API	void				SetTrasparency(bool isTransparent, float transparentValue);
		//Settaggi correnti
		DLL_API bool				GetMode();
		DLL_API D3DXVECTOR2			GetPosition();
		DLL_API float				GetRotation();
		DLL_API D3DXVECTOR2			GetScale();
		DLL_API int					GetWidth();
		DLL_API int					GetHeight();
		DLL_API int					GetCurrentFrame();
		DLL_API bool				GetCreated();
		//Frame
		DLL_API bool				GoToFrame(unsigned int frame);
		//Picking
		DLL_API bool				CheckPicked(D3DXVECTOR2 ptCursor);
		//Render
		DLL_API void				Render();
		//Animazione
		DLL_API void				AnimationPlay();
		DLL_API void				AnimationStop();
		DLL_API void				AnimationPause();
		DLL_API void				AnimationSettings(DWORD everyMilliseconds, bool cycleAnimation=false, bool forwardAnimation=true);
		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();

};

#endif 
