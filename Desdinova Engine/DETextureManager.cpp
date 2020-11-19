#include "DEGeneralIncludes.h"

vector<STRUCT_DE_TEXTURE>	DETextureManager::TextureList;
int							DETextureManager::totalTextures=0;

DLL_API void DETextureManager::ReleaseTexture(int textureID)
{
	if ((textureID>=0)&&(textureID<totalTextures))
	{
		TextureList[textureID].Texture_Pos = TextureList[textureID].Texture_Pos - 1;
		//Rilascia completamente la texture solo se non è più usata
		if (TextureList[textureID].Texture_Pos==0)
		{
			//Rilascia la texture
			TextureList[textureID] = GetNullTexture();
		}
	}
}

DLL_API bool DETextureManager::ReleaseAll()
{
	try
	{
		//Rilascia tutte le texture incondizionatamente
		for ( int t=0; t<totalTextures; t++)
		{
			TextureList[t] = GetNullTexture();
		}
		totalTextures=0;
		TextureList.clear();
		return true;
	}
	catch(...)
	{
		return false;
	}

	return false;
}

DLL_API bool DETextureManager::Release()
{
	DELog::LogInfo("<br><b>Release Textures Manager:</b>");
	//Rilascia tutte le texture
    if (ReleaseAll())
	{
		//Cancella tutta la memoria occupata
		TextureList.clear();
		DELog::LogInfo("<li>The function completed successfully");
		return true;
	}
	else
	{
		DELog::LogError("<li>Unknown statement error");		
		return false;
	}

	return false;
}

DLL_API int DETextureManager::CreateTexture(const char *szTextureFilePath, DWORD dwKeyColour, bool createMipMaps)
{
	int currentTexture=-1;

	try
	{
		//Passa tutti i nomi di textures già presenti
		for ( int t=0; t< totalTextures; t++)
		{
			//La stringa non è vuota
			int result1 = strncmp( (const char *)(strdup(szTextureFilePath)), "" , strlen(szTextureFilePath) );
			if (result1 != 0)
			{
				//Confronta le stringhe
				//Viene utilizzata una copia lowercase delle 2 stringhe in modo da renderle uguali per
				//il confronto che fallirebbe se una delle due stringhe avesse una lettera minusco o maiuscola anche
				//se uguale all'altra.
				int result2 = strncmp( (const char *)strlwr(strdup(szTextureFilePath)), (const char *)strlwr(strdup(TextureList[t].Texture_FileName)) , strlen(szTextureFilePath) );
				//Le stringhe sono uguali
				if (result2 == 0)
				{
					//La texture esiste già per cui fa riferimento a quella trovata(t)
					TextureList[t].Texture_Pos = TextureList[t].Texture_Pos + 1;
					currentTexture = t;
					return currentTexture;
				}
			}
			else
			{
				return -1;
			}
		}
		//Aggiunge la nuova texture all'array
		currentTexture = AddTexture(szTextureFilePath, dwKeyColour, createMipMaps);
		return currentTexture;
	}
	catch(...)
	{
		return -1;
	}

	return -1;
}


DLL_API int DETextureManager::AddTexture(const char *szTextureFilePath, DWORD dwKeyColour, bool createMipMaps)
{
	try
	{
		HRESULT hr;
		LPDIRECT3DTEXTURE9 tempTexture=NULL;
		D3DXIMAGE_INFO tempInfo;

		//Path corrente + nome del file fx perchè le DX vogliono tutto il percorso
		char namePath[MAX_PATH];
		strcpy(namePath, DEUtility::GetCurrentPath());
		strcat(namePath, szTextureFilePath);

		unsigned int mipMaps=0;
		if (createMipMaps)  mipMaps=0;	//Crea tutte le mipmaps
		else				mipMaps=1;	//Crea solo una mipmaps

		hr = D3DXCreateTextureFromFileEx(DECore::D3DDevice, (LPSTR)namePath, 0,0,mipMaps,0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,dwKeyColour,&tempInfo,NULL,&tempTexture);
		if (hr==D3D_OK)
		{
			STRUCT_DE_TEXTURE newTexture;
			newTexture.Texture_Pointer = tempTexture;
			newTexture.Texture_Info = tempInfo;
			strcpy (newTexture.Texture_FileName, strdup(szTextureFilePath));
			newTexture.Texture_Pos = 1;
			newTexture.Texture_KeyColour = dwKeyColour;
			newTexture.Texture_MipMaps = createMipMaps;
			int currentTextureID = PlaceTexture(newTexture);
			return currentTextureID;
		}
		else
		{
			return -1;
		}
	}
	catch(...)
	{
		return -1;
	}
	return -1;
}


DLL_API int DETextureManager::PlaceTexture(STRUCT_DE_TEXTURE newTexture)
{
	//Se trova un posto vuoto mette la texture li dentro
	for ( int t=0; t< totalTextures; t++)
	{
		if (TextureList[t].Texture_Pos==0)
		{
			TextureList[t] = newTexture;
			return t;
		}
	}

	//Setta il nuovo elemento aggiunto
	TextureList.push_back(newTexture);

	//Aggiunto uno alla lista
	totalTextures = totalTextures + 1;

	return totalTextures-1;
}


DLL_API int DETextureManager::GetLoadedTextures()
{
	return totalTextures;
}

DLL_API int DETextureManager::GetSearchedTextures()
{
	int searchTextures=0;
	for (int t=0; t<DETextureManager::GetLoadedTextures(); t++)
	{
		searchTextures = searchTextures + TextureList[t].Texture_Pos;
	}
	return searchTextures;
}

DLL_API STRUCT_DE_TEXTURE DETextureManager::GetTexture(int textureID)
{
	if (textureID >= totalTextures)	textureID = totalTextures;
	if (textureID < 0)
	{
		STRUCT_DE_TEXTURE nullTexture;
		nullTexture = GetNullTexture();
		return nullTexture;	
	}
	return TextureList[textureID];
}

DLL_API STRUCT_DE_TEXTURE DETextureManager::GetNullTexture()
{
	STRUCT_DE_TEXTURE nullTexture;
	nullTexture.Texture_Pointer = NULL;
	SafeRelease(nullTexture.Texture_Pointer);
	ZeroMemory(&nullTexture.Texture_Info, sizeof(D3DXIMAGE_INFO));
	strcpy(nullTexture.Texture_FileName, "No filename");
	nullTexture.Texture_Pos = 0;
	nullTexture.Texture_KeyColour = 0;
	nullTexture.Texture_MipMaps = false;
	return nullTexture;	
}

DLL_API void DETextureManager::LogReport()
{
	int textureBlock=0;
	DELog::LogInfo("<br><b>Allocated Textures Report:</b>");
	for (int t=0; t<DETextureManager::GetLoadedTextures(); t++)
	{
		LPDIRECT3DTEXTURE9 tempTexture;
		tempTexture = DETextureManager::GetTexture(t).Texture_Pointer;
		if (tempTexture != NULL)
		{
			textureBlock=textureBlock +1;
			float currentMipMaps = (float)tempTexture->GetLevelCount();
			DELog::LogInfo("<li>Texture[%.0f]: %s - MipMaps: %.0f -	Shared: %.0f", (float)t, DETextureManager::GetTexture(t).Texture_FileName, (float)currentMipMaps, (float)DETextureManager::GetTexture(t).Texture_Pos);
		}
		else
		{
			DELog::LogInfo("<li>Texture[%.0f]: %s - Temporary empty block", (float)t, DETextureManager::GetTexture(t).Texture_FileName);
		}
	}
	DELog::LogInfo("<li>Textures Loaded: %.0f", (float)DETextureManager::GetLoadedTextures());
	DELog::LogInfo("<li>Textures Block Occupated: %.0f", (float)textureBlock);
	DELog::LogInfo("<li>Textures Block Empty: %.0f", (float)((float)DETextureManager::GetLoadedTextures() - (float)textureBlock));
	if (DECore::D3DDevice!=NULL)
	{
		DELog::LogInfo("<li>Textures Memory Available: %.0fMb", (float)DECore::D3DDevice->GetAvailableTextureMem()/(1024.0f*1024.0f));
	}
	else
	{
		DELog::LogWarning("<li>No Textures Memory Available Info");
	}

}

DLL_API	bool DETextureManager::Invalidate()
{
	/*Rilascia tutte le texture
	for ( int t=0; t<totalTextures; t++)
	{
		if (TextureList[t].Texture_Pos != 0)
		{
			SafeRelease(TextureList[t].Texture_Pointer);
		}
	}*/
	return true;
}

DLL_API	bool DETextureManager::Restore()
{
	/*unsigned int mipMaps=0;
	for ( int t=0; t<totalTextures; t++)
	{
		if (TextureList[t].Texture_Pos != 0)
		{
			if (TextureList[t].Texture_MipMaps)  mipMaps=0;	//Crea tutte le mipmaps
			else								 mipMaps=1;	//Crea solo una mipmaps
			D3DXCreateTextureFromFileEx(DECore::D3DDevice, TextureList[t].Texture_FileName, 0, 0, mipMaps, 0, D3DFMT_UNKNOWN,D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, TextureList[t].Texture_KeyColour, NULL, NULL, &TextureList[t].Texture_Pointer);
		}
		else
		{
			continue;
		}
	}*/
	return true;
}