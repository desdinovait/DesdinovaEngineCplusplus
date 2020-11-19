#ifndef H_MESHMANAGER_H
#define H_MESHMANAGER_H

/*
[CLASS] 
[
    [NOME] 
    DEMeshManager
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei modelli attraverso una sorta di chance
	che permette di non allocare 2 volte contemporaneamente lo stesso modello ma
	riutilizzarla attraverso il puntatore alla sua risorsa.
]
*/
class DEMeshManager
{	
	private:
		//** Variabili **
		static vector<STRUCT_DE_MESH>			MeshList;
		static int								totalMeshes;
		
		//** Funzioni **
		DLL_API static int						AddMesh(const char *szMMeshFilePath, bool createTangentBinormal);
		DLL_API static int						PlaceMesh(STRUCT_DE_MESH newMesh);
	
	public:
		//** Funzioni **
		DLL_API static int						CreateMesh(const char *szMeshFilePath, bool createTangentBinormal);
		DLL_API static void						ReleaseMesh(int meshID);
		DLL_API static bool						ReleaseAll();
		DLL_API static bool						Release();
		DLL_API static int						GetLoadedMeshes();
		DLL_API static int						GetSearchedMeshes();
		DLL_API	static STRUCT_DE_MESH			GetMesh(int meshID);
		DLL_API static STRUCT_DE_MESH			GetNullMesh();
		DLL_API static void						LogReport();
		DLL_API	static bool						Invalidate();
		DLL_API	static bool						Restore();
};


#endif