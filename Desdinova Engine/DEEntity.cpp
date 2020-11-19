#include "DEGeneralIncludes.h"

DLL_API DEEntity::DEEntity()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	loaded = false;
}

DLL_API bool DEEntity::Create(LPCSTR logTitle)
{
	try
	{
		memset(name, 0, sizeof(char) * MAX_PATH);
		strcpy(name, "NewEntity");
		totalChildren = 0;
		loaded = false;
		mainLogTitle = logTitle;
		mainTag = NULL;

		isPicked = false;

		/*Visualizzazione base
		renderProperties.useLOD = true;
		renderProperties.useBounding = 0;
		renderProperties.useAxes = false;
		renderProperties.useFrustrumCulling = true;
		renderProperties.useWireframe = false;
		renderProperties.useLighting = false;
		renderProperties.useFog = false;*/

		Parent.anchorPointPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		Parent.anchorPointRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		Parent.anchorPointScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Parent.anchorAxes.Create("Parent.anchorAxes");

		//Inizializza lo stack
		D3DXCreateMatrixStack(0, &matrixStack);
		matrixStack->LoadIdentity();

		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}



DLL_API bool DEEntity::Load(LPCTSTR fileName)
{
	char parentName_Load[MAX_PATH];
	memset(parentName_Load, 0, sizeof(char) * MAX_PATH);
	unsigned int totalChildren_Load;

	HANDLE hFile=CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		loaded = false;
		return false;
	}
	else
	{
		//** PADRE **
		//Legge il nome del file del padre
		unsigned long rwBytes5;
		SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
		ReadFile(hFile, &parentName_Load, sizeof(char)*MAX_PATH, &rwBytes5, NULL);

		//Carica del file del parent
		AddParent(parentName_Load);

		//** NUMERO FIGLI **
		//Legge il numero dei figlio contenuti nell'entità
		unsigned long rwBytes7;
		SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
		ReadFile(hFile, &totalChildren_Load, sizeof(unsigned int), &rwBytes7, NULL);

		//** FIGLI **
		//Legge i children con le varie info
		for (unsigned int c=0; c< totalChildren_Load; c++)
		{
			unsigned long rwBytes6;
			SSaveChildren loadChildren;
			SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			ReadFile(hFile, &loadChildren, sizeof(SSaveChildren), &rwBytes6, NULL);
			
			//Carica il figlio
			AddChild(loadChildren.filename,
					 loadChildren.enableChild,
					 loadChildren.enableAnchor,
					 loadChildren.active,
					 loadChildren.anchorPointPos,
					 loadChildren.anchorPointRot,
					 loadChildren.anchorPointScale);

		}

		//Chiude il file
		CloseHandle(hFile);
		
		loaded = true;

		//Nuovo file corrente senza estensione ne percorso(lo ricava dal file passato
		memset(name, 0, sizeof(char) * MAX_PATH);
		char *pdest1;
		int result1=0;
		pdest1 = strrchr( (char *)fileName, '.' );
		result1 = pdest1 - fileName + 1;
		strncpy(name, fileName, result1-1 );	
		char *reversed;
		reversed = (char *)malloc(MAX_PATH);
		memset(reversed, 0, sizeof(char) * MAX_PATH);
		reversed = _strrev( _strdup( name ) );
		char *pdest2;
		int result2=0;
		pdest2 = strrchr( reversed, '\\' );
		result2 = pdest2 - reversed + 1;
		memset(name, 0, sizeof(char) * MAX_PATH);
		strncpy(name, reversed, result2-1 );	
		char *reversed2;
		reversed2 = (char *)malloc(MAX_PATH);
		memset(reversed2, 0, sizeof(char) * MAX_PATH);
		reversed2 = _strrev( _strdup( name ) );
		memset(name, 0, sizeof(char) * MAX_PATH);
		strcpy(name, reversed2);
		free(reversed);
		free(reversed2);

		return true;
	}


	return true;
}



DLL_API bool DEEntity::Save(LPCTSTR fileName)
{
	if (loaded)
	{
		HANDLE hFile;
		hFile=CreateFile(fileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
		{
			return false;
		}
		else
		{
			//Scrive il nome del padre
			SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			//WriteFile(hFile, Parent.Model.GetFilenameFX(), sizeof(char)*MAX_PATH, &rwBytes5, NULL);

			//Scrive il numero di figli
			unsigned long rwBytes7;
			SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			WriteFile(hFile, &totalChildren, sizeof(unsigned int), &rwBytes7, NULL);

			//Scrive i children con le varie info
			for (unsigned int c=0; c< totalChildren; c++)
			{
				unsigned long rwBytes6;
				SSaveChildren saveChildren;
				saveChildren.active = Child[c].active;
				saveChildren.anchorPointPos = Child[c].anchorPointPos;
				saveChildren.anchorPointRot = Child[c].anchorPointRot;
				saveChildren.anchorPointScale = Child[c].anchorPointScale;
				saveChildren.enableAnchor = Child[c].enableAnchor;
				saveChildren.enableChild = Child[c].enableChild;
				//strcpy(saveChildren.filename, Child[c].Model.GetFilenameFX());
				SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
				WriteFile(hFile, &saveChildren, sizeof(SSaveChildren), &rwBytes6, NULL);
			}


			//Chiude il file
			CloseHandle(hFile);

			//Nuovo file corrente senza estensione ne percorso(lo ricava dal file passato
			memset(name, 0, sizeof(char) * MAX_PATH);
			char *pdest1;
			int result1=0;
			pdest1 = strrchr( (char *)fileName, '.' );
			result1 = pdest1 - fileName + 1;
			strncpy(name, fileName, result1-1 );	
			char *reversed;
			reversed = (char *)malloc(MAX_PATH);
			memset(reversed, 0, sizeof(char) * MAX_PATH);
			reversed = _strrev( _strdup( name ) );
			char *pdest2;
			int result2=0;
			pdest2 = strrchr( reversed, '\\' );
			result2 = pdest2 - reversed + 1;
			memset(name, 0, sizeof(char) * MAX_PATH);
			strncpy(name, reversed, result2-1 );	
			char *reversed2;
			reversed2 = (char *)malloc(MAX_PATH);
			memset(reversed2, 0, sizeof(char) * MAX_PATH);
			reversed2 = _strrev( _strdup( name ) );
			memset(name, 0, sizeof(char) * MAX_PATH);
			strcpy(name, reversed2);
			free(reversed);
			free(reversed2);
		
			return true;
		}
	}

	return false;
}


DLL_API unsigned int DEEntity::AddChild(LPCTSTR fileName, bool enableChild, bool enableAnchor, bool active, D3DXVECTOR3 anchorPointPos, D3DXVECTOR3 anchorPointRot, D3DXVECTOR3 anchorPointScale)
{
	if (loaded)
	{
		//Salva temporaneamente l'array
		STRUCT_DE_ENTITYCHILD	*saveChild;
		saveChild = (STRUCT_DE_ENTITYCHILD *)calloc(totalChildren, sizeof(STRUCT_DE_ENTITYCHILD));
		for (unsigned int h=0; h< totalChildren; h++)
		{
			saveChild[h]=Child[h];
		}

		//Ricrea l'array dei figli aggiungendone uno
		Child = (STRUCT_DE_ENTITYCHILD *)calloc((totalChildren+1), sizeof(STRUCT_DE_ENTITYCHILD));
		for (unsigned int h=0; h< totalChildren; h++)
		{
			Child[h] = saveChild[h];
		}
		
		//Child[totalChildren] dovrebbe esserebbe Child[totalChildren +1 -1]
		char loadname[MAX_PATH];
		strcpy(loadname,mainLogTitle);
		char strLog[MAX_PATH];
		sprintf(strLog,".Child[%.0f].Model",float(totalChildren));
		strcat(loadname, strLog);
		Child[totalChildren].Model.Create(fileName, loadname);
		
		char loadname2[MAX_PATH];
		strcpy(loadname2,mainLogTitle);
		char strLog2[MAX_PATH];
		sprintf(strLog2,".Child[%.0f].Model",float(totalChildren));
		strcat(loadname2, strLog2);
		Child[totalChildren].anchorAxes.Create(loadname2);
		Child[totalChildren].enableChild = enableChild;
		Child[totalChildren].enableAnchor = enableAnchor;
		Child[totalChildren].active = active;
		Child[totalChildren].anchorPointPos = anchorPointPos;
		Child[totalChildren].anchorPointRot = anchorPointRot;
		Child[totalChildren].anchorPointScale = anchorPointScale;

		totalChildren = totalChildren + 1;

		free(saveChild);

	}
	else
	{
		return 0;
	}

	return totalChildren;
}


DLL_API unsigned int DEEntity::RemoveChild(unsigned int childID)
{
	if (loaded)
	{
		//Salva la prima e la seconda parte della lista
		STRUCT_DE_ENTITYCHILD	*saveChild1;
		STRUCT_DE_ENTITYCHILD	*saveChild2;
		saveChild1 = (STRUCT_DE_ENTITYCHILD *)calloc(childID, sizeof(STRUCT_DE_ENTITYCHILD));
		saveChild2 = (STRUCT_DE_ENTITYCHILD *)calloc(childID, sizeof(STRUCT_DE_ENTITYCHILD));
		for (unsigned int h=0; h<childID; h++)
		{
			saveChild1[h] = Child[h];
		}
		for (unsigned int h=0; h<totalChildren-childID-1; h++)
		{
			saveChild2[h] = Child[h+childID+1];
		}
		
		//Crea il nuovo array senza un elemento
		Child = (STRUCT_DE_ENTITYCHILD *)calloc((totalChildren-1), sizeof(STRUCT_DE_ENTITYCHILD));
		//Copia la prima e la seconda parte una dopo l'altra
		for (unsigned int c=0; c<childID; c++)
		{
			Child[c] = saveChild1[c];
		}
		for (unsigned int c=0; c<totalChildren-childID-1; c++)
		{
			Child[c+childID] = saveChild2[c];
		}
		
		//Imposta il nuovo numero dei figlio togliendone uno
		totalChildren = totalChildren -1;

		free(saveChild1);
		free(saveChild2);
	}
	return totalChildren;
}




DLL_API bool DEEntity::AddParent(LPCTSTR fileName)
{
	char load[MAX_PATH];
	strcpy(load, mainLogTitle);
	strcat(load, ".Parent.Model");
	if (Parent.Model.Create(fileName, load))
	{
		loaded=true;
		return true;
	}
	loaded=false;
	return false;

}


DLL_API	void DEEntity::SetWorldMatrix(D3DXMATRIX newMatrix)
{
	worldMatrix = newMatrix;
}

DLL_API void DEEntity::Render(D3DXVECTOR3 viewPos, D3DXVECTOR3 centerCamera, STRUCT_DE_FOV fovProperties, D3DXMATRIX externMatrix)
{
	if (loaded)
	{
		//Setta comunque che l'oggetto non è in piking
		isPicked = false;
	
		//*** Padre ***
		//Matrice corrente
		D3DXMATRIX parentMatrix = Parent.Model.CalculateMatrix();
		matrixStack->Push();
		matrixStack->MultMatrix(&parentMatrix);
		//Matrici ancore
		matrixStack->Translate(Parent.anchorPointPos.x, Parent.anchorPointPos.y, Parent.anchorPointPos.z);
		matrixStack->RotateYawPitchRoll(Parent.anchorPointRot.y, Parent.anchorPointRot.x, Parent.anchorPointRot.z);
		matrixStack->Scale(Parent.anchorPointScale.x, Parent.anchorPointScale.y, Parent.anchorPointScale.z);
		//Matrice finale
		D3DXMATRIX topMatrix = *matrixStack->GetTop();
		//Parent.Model.SetMatrix(topMatrix);
		//*** Figli ***
		for (unsigned int s=0; s<totalChildren; s++)
		{
			if ((Child[s].enableAnchor)&&(Child[s].enableChild))
			{
				//Matrice corrente
				D3DXMATRIX childMatrix = Child[s].Model.CalculateMatrix();
				matrixStack->Push();
				matrixStack->MultMatrix(&childMatrix);
				//Matrici ancora
				matrixStack->TranslateLocal(Child[s].anchorPointPos.x, Child[s].anchorPointPos.y, Child[s].anchorPointPos.z);
				matrixStack->RotateYawPitchRollLocal(Child[s].anchorPointRot.y, Child[s].anchorPointRot.x, Child[s].anchorPointRot.z);
				matrixStack->ScaleLocal(Child[s].anchorPointScale.x, Child[s].anchorPointScale.y, Child[s].anchorPointScale.z);
				//Matrice finale
				D3DXMATRIX topMatrix = *matrixStack->GetTop();
				//Child[s].Model.SetMatrix(topMatrix);
				//Render
				//Child[s].Model.Render(viewPos, centerCamera, fovProperties, NULL);
				//Rilascio matrice figlio
				matrixStack->Pop();
			}
		}//*/
		//Rilascio matrice padre
		matrixStack->Pop();


		/*if (renderProperties.useAxes)
		{
			D3DXMATRIX scaleMatrixParentAxes;
			D3DXMATRIX posMatrixParentAxes;
			D3DXMATRIX rotMatrixParentAxes;
			D3DXMATRIX axesMatrixParentAxes;
			D3DXMatrixIdentity(&axesMatrixParentAxes);

			D3DXMatrixScaling(&scaleMatrixParentAxes, 0.05f * Parent.anchorPointScale.x, 0.05f * Parent.anchorPointScale.y, 0.05f * Parent.anchorPointScale.z);
			D3DXMatrixTranslation(&posMatrixParentAxes, Parent.anchorPointPos.x, Parent.anchorPointPos.y, Parent.anchorPointPos.z);
			D3DXMatrixRotationYawPitchRoll(&rotMatrixParentAxes, D3DXToRadian(Parent.anchorPointRot.y), D3DXToRadian(Parent.anchorPointRot.x), D3DXToRadian(Parent.anchorPointRot.z));

			//Setta finalmenteeeee la matrice(posizione dell'ancora x quella del pare
			axesMatrixParentAxes = scaleMatrixParentAxes * rotMatrixParentAxes * posMatrixParentAxes * Parent.Model.GetFinalMatrixNoScale();

			//Disegna l'ancora
			Parent.anchorAxes.Render(axesMatrixParentAxes);

			for (unsigned int s=0; s<totalChildren; s++)
			{
				if (Child[s].enableAnchor)
				{
					D3DXMATRIX scaleMatrix;
					D3DXMATRIX posMatrix;
					D3DXMATRIX rotMatrix;
					D3DXMATRIX axesMatrix;
					D3DXMatrixIdentity(&axesMatrix);
					D3DXMatrixScaling(&scaleMatrix, 0.05f * Child[s].anchorPointScale.x, 0.05f * Child[s].anchorPointScale.y, 0.05f * Child[s].anchorPointScale.z);
					D3DXMatrixTranslation(&posMatrix, Child[s].anchorPointPos.x, Child[s].anchorPointPos.y, Child[s].anchorPointPos.z);
					D3DXMatrixRotationYawPitchRoll(&rotMatrix, D3DXToRadian(Child[s].anchorPointRot.y), D3DXToRadian(Child[s].anchorPointRot.x), D3DXToRadian(Child[s].anchorPointRot.z));

					//Setta finalmenteeeee la matrice(posizione dell'ancora x quella del pare
					axesMatrix = scaleMatrix * rotMatrix * posMatrix * Parent.Model.GetFinalMatrixNoScale();

					//Disegna l'ancora
					Child[s].anchorAxes.Render(axesMatrix);

				}
			}
		}//*/

	}
}

DLL_API bool DEEntity::CheckPicked(D3DXVECTOR2 ptCursor)
{
	if (loaded)
	{
		//Esegue il piking su ogni elemento dell'entità
		//quando anche un solo elemento è stato selezionato vuol dire che l'entità è stata 
		//selezionata quindi pone il piking = true ed esce
		if (Parent.Model.CheckPicked(ptCursor))
		{
			isPicked = true;
			return isPicked;
		}
		else
		{
			for (unsigned int i=0; i< totalChildren; i++)
			{
				//Esegue solo se è presente un'ancora e un figlio ed è attivo
				if ((Child[i].active)&&(Child[i].enableAnchor)&&(Child[i].enableChild))	
				{
					if (Child[i].Model.CheckPicked(ptCursor))
					{
						isPicked = true;
						return isPicked;
					}
				}
			}
		}
	}
	return isPicked;
}

DLL_API bool DEEntity::GetPicked()
{
	return isPicked;
}

DLL_API bool DEEntity::GetCulled()
{
	if 	(loaded)
	{
		unsigned int counter=0;
		for (unsigned int c=0; c<totalChildren; c++)
		{
			if(Child[c].Model.GetModelInfo().Info_IsCulled) counter=counter+1;
		}
		if(Parent.Model.GetModelInfo().Info_IsCulled) counter=counter+1;
		
		//Se il conteggio è uguale al numero dei figlio + il padre allora tutti sono cullati
		//e quindi l'entità è cullata
		if (counter==totalChildren + 1)
		{
			return true;
		}
	}
	return false;
}

DLL_API bool DEEntity::GetFar()
{
	if 	(loaded)
	{
		unsigned int counter=0;
		for (unsigned int c=0; c<totalChildren; c++)
		{
			if(Child[c].Model.GetModelInfo().Info_IsFar) counter=counter+1;
		}
		if(Parent.Model.GetModelInfo().Info_IsFar) counter=counter+1;
		
		//Se il conteggio è uguale al numero dei figli + il padre allora tutti sono lontani
		//e quindi l'entità è cullata
		if (counter==totalChildren + 1)
		{
			return true;
		}
	}
	return false;
}

DLL_API char *DEEntity::GetName()
{
	return name;
}

DLL_API void DEEntity::SetParentProperties(STRUCT_DE_ENTITYPARENT newParentProperties)
{
	Parent.active = newParentProperties.active;
	Parent.anchorAxes = newParentProperties.anchorAxes;
	Parent.anchorPointPos = newParentProperties.anchorPointPos;
	Parent.anchorPointRot = newParentProperties.anchorPointRot;
	Parent.anchorPointScale = newParentProperties.anchorPointScale;
	Parent.enableAnchor = newParentProperties.enableAnchor;
}
DLL_API void DEEntity::SetChildProperties(unsigned int childID, STRUCT_DE_ENTITYCHILD newChildrenProperties)
{
	if (childID < totalChildren)
	{
		Child[childID].active = newChildrenProperties.active;
		Child[childID].anchorAxes = newChildrenProperties.anchorAxes;
		Child[childID].anchorPointPos = newChildrenProperties.anchorPointPos;
		Child[childID].anchorPointRot = newChildrenProperties.anchorPointRot;
		Child[childID].anchorPointScale = newChildrenProperties.anchorPointScale;
		Child[childID].enableAnchor = newChildrenProperties.enableAnchor;
	}
}

DLL_API STRUCT_DE_ENTITYPARENT DEEntity::GetParentProperties()
{
	return Parent;
}
DLL_API STRUCT_DE_ENTITYCHILD DEEntity::GetChildProperties(unsigned int childID)
{
	if (childID < totalChildren)	return	Child[childID];
	else							return	Child[0];
}

DLL_API DEModel DEEntity::GetParent()
{
	return Parent.Model;
}
DLL_API DEModel DEEntity::GetChild(unsigned int childID)
{
	if (childID < totalChildren)	return	Child[childID].Model;
	else							return	Child[0].Model;
}

DLL_API D3DXVECTOR3 DEEntity::GetPosition2D(int mode)
{
	if (loaded)
	{
		//return Parent.Model.Get2DPosition(mode);
	}
	return D3DXVECTOR3(0,0,0);
}

DLL_API D3DXVECTOR3	DEEntity::GetPosition()
{
	//return Parent.Model.GetPosition();
	return D3DXVECTOR3(0,0,0);
}
DLL_API D3DXVECTOR3	DEEntity::GetRotation()
{
	//return Parent.Model.GetRotation();
	return D3DXVECTOR3(0,0,0);
}
DLL_API D3DXVECTOR3	DEEntity::GetScale()
{
	//return Parent.Model.GetScale();
	return D3DXVECTOR3(0,0,0);
}

DLL_API void DEEntity::SetPosition(D3DXVECTOR3 newPosition)
{
	//Parent.Model.SetPosition(newPosition);
}
DLL_API void DEEntity::SetRotation(D3DXVECTOR3 newRotation)
{
	//Parent.Model.SetRotation(newRotation);
}
DLL_API void DEEntity::SetScale(D3DXVECTOR3 newScale)
{
	//Parent.Model.SetScale(newScale);
}



DLL_API bool DEEntity::GetNear()
{
	/*if (Parent.Model.GetNear())	return true;
	for (unsigned int s=0; s<totalChildren; s++)
	{
		if (Child[s].Model.GetNear())	return true;
	}*/
	return false;
}
DLL_API void DEEntity::SetSpatialGrid(DESpatialGrid *worldSparialGrid)
{
	/*Parent.Model.SetSpatialGrid(worldSparialGrid);
	for (unsigned int s=0; s<totalChildren; s++)
	{
		Child[s].Model.SetSpatialGrid(worldSparialGrid);
	}*/
}



DLL_API unsigned int DEEntity::GetTotalChildren()
{
	return totalChildren;
}

DLL_API unsigned int DEEntity::GetCurrentVertices(unsigned int type)
{
	/*Solo padre
	if (type==0)
	{
		return Parent.Model.GetCurrentVertices();
	}
	//Solo figli
	if (type==1)
	{
		unsigned int currentVertices1=0;
		for (unsigned int c=0; c<totalChildren; c++)
		{
			currentVertices1 = currentVertices1 + Child[c].Model.GetCurrentVertices();
		}
		return currentVertices1;
	}
	//Padre e figli
	if (type==2)
	{
		unsigned int currentVertices2=0;
		for (unsigned int c=0; c<totalChildren; c++)
		{
			currentVertices2 = currentVertices2 + Child[c].Model.GetCurrentVertices();
		}

		currentVertices2 = currentVertices2 + Parent.Model.GetCurrentVertices();
		return currentVertices2;
	}*/

	return 0;
}

DLL_API unsigned int DEEntity::GetCurrentVerticesInScene(unsigned int type)
{
	/*Solo padre
	if (type==0)
	{
		return Parent.Model.GetCurrentVerticesInScene();
	}
	//Solo figli
	if (type==1)
	{
		unsigned int currentVertices1=0;
		for (unsigned int c=0; c<totalChildren; c++)
		{
			currentVertices1 = currentVertices1 + Child[c].Model.GetCurrentVerticesInScene();
		}
		return currentVertices1;
	}
	//Padre e figli
	if (type==2)
	{
		unsigned int currentVertices2=0;
		for (unsigned int c=0; c<totalChildren; c++)
		{
			currentVertices2 = currentVertices2 + Child[c].Model.GetCurrentVerticesInScene();
		}
		currentVertices2 = currentVertices2 + Parent.Model.GetCurrentVerticesInScene();
		return currentVertices2;
	}*/

	return 0;
}

DLL_API bool DEEntity::Invalidate()
{
	try
	{
		if (Parent.Model.Invalidate())
		{
			if (Parent.anchorAxes.Invalidate())
			{
				for (unsigned int c=0; c< totalChildren; c++)
				{
					Child[c].Model.Invalidate();
				}
				return true;
			}
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEEntity::Restore()
{
	try
	{
		if (Parent.Model.Restore())
		{
			if (Parent.anchorAxes.Restore())
			{
				for (unsigned int c=0; c< totalChildren; c++)
				{
					Child[c].Model.Restore();
				}
				return true;
			}
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEEntity::Release()
{
	try
	{
		Parent.Model.Release();
		Parent.anchorAxes.Release();
		for (unsigned int c=0; c< totalChildren; c++)
		{
			Child[c].Model.Release();
		}
		SafeDeleteArray(Child);

		totalChildren = 0;
		loaded = false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

DLL_API bool DEEntity::SetTag(LPCSTR newTag)
{
	try
	{
		mainTag = newTag;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API LPCSTR DEEntity::GetTag()
{
	LPCSTR	nullTag="NoTag";
	try
	{
		if (mainTag!=NULL)	return mainTag;
		else				return nullTag;
	}
	catch(...)
	{
		return nullTag;
	}
	return nullTag;
}