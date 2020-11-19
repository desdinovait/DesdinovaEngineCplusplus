#ifndef H_ENTITY_H
#define H_ENTITY_H

//Padre
typedef struct STRUCT_DE_ENTITYPARENT 
{
	DEModel			Model;
	D3DXVECTOR3		anchorPointPos;
	D3DXVECTOR3		anchorPointRot;
	D3DXVECTOR3		anchorPointScale;
	DEAxes			anchorAxes;
	bool			enableAnchor;
	bool			active;

}STRUCT_DE_ENTITYPARENT;
//Figlio
typedef struct STRUCT_DE_ENTITYCHILD
{
	DEModel			Model;
	D3DXVECTOR3		anchorPointPos;
	D3DXVECTOR3		anchorPointRot;
	D3DXVECTOR3		anchorPointScale;
	DEAxes			anchorAxes;
	bool			enableAnchor;
	bool			enableChild;
	bool			active;
}STRUCT_DE_ENTITYCHILD;

/*
[CLASS] 
[
    [NOME] 
    DEEntity
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle entità (entity) presenti nel motore
	grafico. Una entità è costituita da un modello padre e dei figli, i quali sono
	dipendenti, nel movimento e nella posizione, dal padre.
	L'aggiunta e la rimozione dei figli avviene in modo dinamico dunque è possibile
	avere virtualmente un numero illimitato di figli.
	La funzione di visualizzazione dell'entità può ricevere in ingresso anche una matrice
	per cui è possibile passare la matrice del padre di un'altra entità in modo da
	costruire qualcosa di più complesso con figli che sono a loro volta entità.
]
*/
class DEEntity
{	
	private:
		//** Strutture **
		//Salvataggio figlio
		typedef struct SSaveChildren
		{
			char			filename[MAX_PATH];
			D3DXVECTOR3		anchorPointPos;
			D3DXVECTOR3		anchorPointRot;
			D3DXVECTOR3		anchorPointScale;
			bool			enableAnchor;
			bool			enableChild;
			bool			active;
		}SSaveChildren;

		//** Variabili **
		//Padre e figli
		STRUCT_DE_ENTITYPARENT				Parent;
		STRUCT_DE_ENTITYCHILD				*Child;
		LPD3DXMATRIXSTACK 					matrixStack;
		//Caricamento riuscito
		bool								loaded;
		//Nome corrente dell'entità
		char								name[MAX_PATH];
		LPCSTR								mainLogTitle;
		LPCSTR								mainTag;
		//Figli totali correnti
		unsigned int						totalChildren;
		//Piking
		bool								isPicked;

		D3DXMATRIX worldMatrix;

	public:
		//** Funzioni **
		//Inizialzzazione e rilascio
		DLL_API								DEEntity();
		DLL_API bool						Create(LPCSTR logTitle="DEEntity");
		DLL_API bool						Invalidate();
		DLL_API bool						Restore();
		DLL_API bool						Release();
		//Caricamento e salvataggio
		DLL_API bool						Load(LPCTSTR fileName);
		DLL_API bool						Save(LPCTSTR fileName);
		//Gestione figli
		DLL_API bool						AddParent(LPCTSTR fileName);
		DLL_API unsigned int				AddChild(LPCTSTR fileName, bool enableChild, bool enableAnchor, bool active, D3DXVECTOR3 anchorPointPos, D3DXVECTOR3 anchorPointRot, D3DXVECTOR3 anchorPointScale);
		DLL_API unsigned int				RemoveChild(unsigned int childID);
		//Rendering a video
		DLL_API void						Render(D3DXVECTOR3 viewPos, D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties, D3DXMATRIX externMatrix);
		//Piking
		DLL_API bool						CheckPicked(D3DXVECTOR2 ptCursor);
		//Proprietà		
		DLL_API void						SetPosition(D3DXVECTOR3 newPosition);
		DLL_API void						SetRotation(D3DXVECTOR3 newRotation);
		DLL_API void						SetScale(D3DXVECTOR3 newScale);		
		DLL_API D3DXVECTOR3					GetPosition();
		DLL_API D3DXVECTOR3					GetRotation();
		DLL_API D3DXVECTOR3					GetScale();	

		DLL_API bool						GetNear();
		DLL_API void						SetSpatialGrid(DESpatialGrid *worldSparialGrid);

		//Settaggi generali
		DLL_API D3DXVECTOR3					GetPosition2D(int mode);
		DLL_API bool						GetPicked();
		DLL_API bool						GetCulled();
		DLL_API bool						GetFar();
		DLL_API unsigned int				GetCurrentVertices(unsigned int type=2);
		DLL_API unsigned int				GetCurrentVerticesInScene(unsigned int type=2);
		DLL_API unsigned int				GetTotalChildren();
		DLL_API char						*GetName();
		//Padre/figli
		DLL_API DEModel						GetParent();
		DLL_API STRUCT_DE_ENTITYPARENT		GetParentProperties();
		DLL_API void						SetParentProperties(STRUCT_DE_ENTITYPARENT newParentProperties);
		DLL_API DEModel						GetChild(unsigned int childID);
		DLL_API STRUCT_DE_ENTITYCHILD		GetChildProperties(unsigned int childID);
		DLL_API void						SetChildProperties(unsigned int childID, STRUCT_DE_ENTITYCHILD newChildrenProperties);
		//Tag
		DLL_API bool						SetTag(LPCSTR newTag);
		DLL_API LPCSTR						GetTag();

		DLL_API	void SetWorldMatrix(D3DXMATRIX newMatrix);

};

#endif