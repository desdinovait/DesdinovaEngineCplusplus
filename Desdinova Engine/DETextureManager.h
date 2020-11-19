#ifndef H_TEXTUREMANAGER_H
#define H_TEXTUREMANAGER_H

/*
[CLASS] 
[
    [NOME] 
    DETextureManager
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione delle textures attraverso una sorta di chance
	che permette di non allocare 2 volte contemporaneamente una stessa texture ma
	riutilizzarla attraverso il puntatore alla sua risorsa.
	E' possibile caricare la texture contenuta in un archivio(ZIP); in questo caso
	è necessario passare anche il percorso dell'archivio e, più importante, anche
	il percorso interno della texture che può essere stata salvata in una sottodirectory.
	La directory deve essere specificata con \\ e non \
]
*/
class DETextureManager
{	
	private:
		//** Variabili **
		static vector<STRUCT_DE_TEXTURE>	TextureList;
		static int							totalTextures;
		
		//** Funzioni **
		DLL_API static int					AddTexture(const char *szTextureFilePath, DWORD dwKeyColour, bool createMipMaps);
		DLL_API static int					AddTexture(const char *szArchiveFilePath, const char *szTextureFilePath, DWORD dwKeyColour, bool createMipMaps);
		DLL_API static int					PlaceTexture(STRUCT_DE_TEXTURE newTexture);
	
	public:
		//** Funzioni **
		DLL_API static int					CreateTexture(const char *szTextureFilePath, DWORD dwKeyColour, bool createMipMaps=true);
		DLL_API static int					CreateTextureFromArchive(const char *szArchiveFilePath, const char *szTextureFilePath, DWORD dwKeyColour, bool createMipMaps);
		DLL_API static void					ReleaseTexture(int textureID);
		DLL_API static bool					ReleaseAll();
		DLL_API static bool					Release();
		DLL_API static int					GetLoadedTextures();
		DLL_API static int					GetSearchedTextures();
		DLL_API static STRUCT_DE_TEXTURE	GetTexture(int textureID);
		DLL_API static STRUCT_DE_TEXTURE	GetNullTexture();
		DLL_API static void					LogReport();
		DLL_API	static bool					Invalidate();
		DLL_API	static bool					Restore();

};


#endif