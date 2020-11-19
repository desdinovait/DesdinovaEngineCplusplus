#ifndef H_MODELMANAGER_H
#define H_MODELMANAGER_H

/*
[CLASS] 
[
    [NOME] 
    DEModelManager
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione dei file .dem attraverso una sorta di chance
	che permette di non allocare 2 volte contemporaneamente uno stesso modello ma
	riutilizzarlo attraverso il puntatore alla sua risorsa.
]
*/
class DEModelManager
{	
	private:
		//** Variabili **
		static vector<STRUCT_DE_MODELPROPERTIES>	ModelList;
		static int									totalModels;
		
		//** Funzioni **
		DLL_API static int							AddModel(const char *szModelFilePath);
		DLL_API static int							PlaceModel(STRUCT_DE_MODELPROPERTIES newModel);

	public:
		//** Funzioni **
		DLL_API static int							CreateModel(const char *szModelFilePath);
		DLL_API static void							ReleaseModel(int ModelID);
		DLL_API static bool							ReleaseAll();
		DLL_API static bool							Release();
		DLL_API static STRUCT_DE_MODELPROPERTIES	GetModel(int ModelID);
		DLL_API static STRUCT_DE_MODELPROPERTIES	GetNullModel();
		DLL_API static int							GetLoadedModels();
		DLL_API static int							GetSearchedModels();
		DLL_API static void							LogReport();

};


#endif