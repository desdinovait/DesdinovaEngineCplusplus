#ifndef H_MATERIALMANAGER_H
#define H_MATERIALMANAGER_H

/*
[CLASS] 
[
    [NOME] 
    DEMaterialManager
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei file .fx attraverso una sorta di chance
	che permette di non allocare 2 volte contemporaneamente uno stesso materiale ma
	riutilizzarlo attraverso il puntatore alla sua risorsa.
]
*/
class DEMaterialManager
{	
	private:
		//** Variabili **
		static vector<STRUCT_DE_MATERIAL>	MaterialList;
		static int							totalMaterials;
		
		//** Funzioni **
		DLL_API static int					AddMaterial(const char *szMaterialFilePath);
		DLL_API static int					PlaceMaterial(STRUCT_DE_MATERIAL newMaterial);

	public:
		//** Funzioni **
		DLL_API static int					CreateMaterial(const char *szMaterialFilePath);
		DLL_API static void					ReleaseMaterial(int materialID);
		DLL_API static bool					ReleaseAll();
		DLL_API static bool					Release();
		DLL_API static STRUCT_DE_MATERIAL	GetMaterial(int materialID);
		DLL_API static STRUCT_DE_MATERIAL	GetNullMaterial();
		DLL_API static int					GetLoadedMaterials();
		DLL_API static int					GetSearchedMaterials();
		DLL_API static void					LogReport();

};


#endif