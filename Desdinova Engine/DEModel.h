#ifndef H_MODEL_H
#define H_MODEL_H

/*
[CLASS] 
[
    [NOME] 
    DEModel
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei modelli costituenti la parte principale
	del motore grafico facendo parte della classe più generale delle entità.
]
*/
class DEModel
{
	private:
		//** Variabili **
		//Informazioni sul modello
		STRUCT_DE_MODELPROPERTIES			modelProperties;
		STRUCT_DE_MODELINFO					modelInfo;
		STRUCT_DE_MATERIAL					modelMaterial;

		DESpatialGrid						*spatialGrid;
		//Attore motore fisico
		NxActor*							mainPhysXActor;
		NxMaterial*							mainPhysXMaterial;
		NxScene*							sceneAssociatedPhysX;

		//** Funzioni **
		//Caricamento
		DLL_API bool						LoadProperties(LPCTSTR propertiesFilename);
		DLL_API bool						LoadMesh();
		DLL_API bool						LoadMaterial();
		DLL_API bool						LoadTextures();

	public:
		//** Variabili **

		//** Funzioni **
		//Inizializzazione e rilascio
		DLL_API								DEModel();
		DLL_API	bool						Create(LPCTSTR propertiesFilename, LPCSTR logTitle="DEModel");
		DLL_API bool						Release();
		DLL_API bool						Invalidate();
		DLL_API bool						Restore();
		//Rendering
		DLL_API void						CalculatePosition(D3DXVECTOR3 viewPos, D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties);
		DLL_API void						Render();
		//Piking
		DLL_API bool						CheckPicked(D3DXVECTOR2 ptCursor, bool includeFar=false);
		//Collisions
		DLL_API bool						CheckCollision(NxContactPairFlag mode=NX_NOTIFY_ON_TOUCH);
		DLL_API bool						CheckCollisionWith(DEModel model, NxShapesType shapeType=NX_ALL_SHAPES);
		DLL_API bool						InsideTrigger(unsigned int triggerID, unsigned int mode);
		//Info
		DLL_API STRUCT_DE_MODELPROPERTIES	GetModelProperties();
		DLL_API STRUCT_DE_MODELINFO			GetModelInfo();
		DLL_API STRUCT_DE_MATERIAL			GetMaterial();
		DLL_API NxActor*					GetPhysXActor();

		//Spatial grid
		DLL_API void						SetSpatialGrid(DESpatialGrid *worldSparialGrid);
		DLL_API bool						SetPhysicsScene(NxScene *sceneAssociated, NxCompartment* sceneCompartment, unsigned int id);
		//DLL_API bool						SetPhysicsScene(NxScene *sceneAssociated, unsigned int id);
		DLL_API void						SetCollisionGroup(unsigned int collisionGroup);

		DLL_API void						SetArrayID(int arrayID);
		DLL_API int							GetArrayID();


		DLL_API	void						SetWorldMatrix(D3DXMATRIX newMatrix);
		DLL_API void						SetPosition(D3DXVECTOR3 position);
		DLL_API D3DXVECTOR3					GetPosition();
		DLL_API void						SetRotation(D3DXVECTOR3 rotation);
		DLL_API D3DXVECTOR3					GetRotation();

		DLL_API void						ApplyForce(D3DXVECTOR3 forceDir, float forceStrength, NxForceMode mode = NX_FORCE );
		DLL_API void						ApplyTorque(D3DXVECTOR3 forceDir, float forceStrength, NxForceMode mode = NX_FORCE );

		DLL_API D3DXMATRIX					CalculateMatrix();


};

#endif
