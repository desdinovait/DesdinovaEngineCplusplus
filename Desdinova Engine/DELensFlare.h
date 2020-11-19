#ifndef H_LENSFLARE_H
#define H_LENSFLARE_H

/*
[CLASS] 
[
    [NOME] 
    DELensFlare
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione dell'effetto di lens flare attraverso pannelli 2D.
	La creazione dei componenti dell'effetto sono allocati dinamicamente in modo
	da poter personalizzare l'effetto a piacimento.
]
*/
class DELensFlare
{	
	private:
		//** Strutture e altre definizioni **
		//Struttura del singolo flare
		struct SFlareProperties
		{
			char		*textureFilename;			//Texture
			D3DXCOLOR	colorkey;					//Colorkey
			D3DXCOLOR	color;						//Colore
			float		scaleFactor;				//Fattore di scalatura
			float		factor;						//Fattore di divisione sulla lunghezza
			float		maxAlpha;					//Alpha massimo, prelevato dal colore
		};

		//Stuttura del singolo flash
		struct SFlashProperties
		{
			char		*textureFilename;			//Texture
			D3DXCOLOR	colorkey;					//Colorkey
			D3DXCOLOR	color;						//Colore
			float		maxAlpha;					//Alpha massimo, prelevato dal colore
		};

		//Stuttura del singolo emettitore
		struct SEmitterProperties
		{
			char		*textureFilename;			//Texture
			D3DXCOLOR	color;						//Colore
			float		scaleFactor;				//valore massimo di trasparenza
			float		maxAlpha;					//Alpha massimo, prelevato dal colore
		};

		//** Variabili **
		LPCSTR				mainLogTitle;
		LPCSTR				mainTag;
		//Pannelli 2D
		DEPanel				Flare;
		DEPanel				Flash;
		DEPanel				Emitter2D;
		DEBillboard			*Emitter3D;
		//Struttura
		SFlareProperties	*FlareProperties;
		SFlashProperties	*FlashProperties;
		SEmitterProperties	*Emitter3DProperties;
		SEmitterProperties	*Emitter2DProperties;
		//Viewport corrente
		D3DVIEWPORT9		currentViewport;
		//Dimensioni area di rendering
		int					screenWidth;
		int					screenHeight;
		float				middleScreenWidth;
		float				middleScreenHeight;
		//View
		STRUCT_DE_FOV		mainFovProperties;
		//Numero di oggetti
		unsigned int		totalFlares;
		unsigned int		totalFlash;
		unsigned int		totalEmitters3D;
		unsigned int		totalEmitters2D;
		//Fattore alpha
		float				alphaAttenuation;
		float				alphaAttenuationVelocity;
		//Dimensione totale
		float				flareDimension;
		//Riduzione
		bool				reduceFlareScale;
		//Creazione
		bool				isCreated;
		//Occlusioni
		bool				cull;
		bool				occluded;
		float				occludedValue;
		//Camera based
		bool				isCameraBased;
		//Posizione
		D3DXVECTOR3			pos3Dlens;
		D3DXVECTOR3			pos2Dlens;
		D3DXVECTOR3			pos3DCamera;
		D3DXVECTOR3			pos3DCameraView;




	public :

		//** Funzioni **
		DLL_API				DELensFlare();
		DLL_API bool		Create(STRUCT_DE_FOV fovProperties, bool cameraBased=true, float alphaAttenuation_velocity=0.01f, float flare_dimension=50, bool reduceFlare_scale=true, LPCSTR logTitle="DELensFlare");
		DLL_API bool		Invalidate();
		DLL_API bool		Restore();		
		DLL_API bool		Release();
		
		DLL_API	void		AddFlare(char *flare_textureFilename, D3DXCOLOR flare_color, D3DXCOLOR flare_colorkey, float flare_scaleFactor, float flare_factor);
		DLL_API	void		AddFlash(char *flash_textureFilename, D3DXCOLOR flash_color, D3DXCOLOR flash_colorkey);
		DLL_API	void		AddEmitter3D(char *emitter3D_textureFilename, D3DXCOLOR emitter3D_color, float emitter3D_scaleFactor);
		DLL_API	void		AddEmitter2D(char *Emitter2D_textureFilename, D3DXCOLOR Emitter2D_color, float Emitter2D_scaleFactor);
		DLL_API void		CalculatePosition();
		DLL_API bool		CalculateOcclusion(vector<DEModel*> modelArray);
		DLL_API NxRay		CalculateRay();	//Se si vuole fare la collisione con PhysX
		DLL_API bool		FOVCheck();
		DLL_API void		Render2D();
		DLL_API void		Render3D();

		DLL_API void		SetCamera(D3DXVECTOR3 newCameraPosition, D3DXVECTOR3 newCameraView);
		DLL_API void		SetPosition(D3DXVECTOR3 newPosition);
		DLL_API D3DXVECTOR3 GetPosition();


		DLL_API bool		GetCulled();
		DLL_API void		SetOccluded(bool isOccluded);
		DLL_API bool		GetOccluded();
		DLL_API bool		GetCreated();
		//Tag
		DLL_API bool		SetTag(LPCSTR newTag);
		DLL_API LPCSTR		GetTag();

};

#endif