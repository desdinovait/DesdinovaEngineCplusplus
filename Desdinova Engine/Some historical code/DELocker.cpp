#include "DEGeneralIncludes.h"
#include "DELocker.h"

DLL_API void DELocker::Release()
{
	try
	{
		Locker.Release();
		isCreated = false;
	}
	catch(...)
	{
		DELog::LogWarning("<li>DELocker : Unknown statement error - Probably NOT Released");		
	}
}

DLL_API bool DELocker::Create(char *logTitle)
{
	isCreated = false;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{
			screenWidth=800;
			screenHeight=600;

			pos2D.x=0.0f;
			pos2D.y=0.0f;
			pos2D.z=0.0f;
			pos3D.x=0.0f;
			pos3D.y=0.0f;
			pos3D.z=0.0f;

			//Trasparenza
			currentAlpha = 1.0f;

			//Preleva il vieport corrente
			DECore::D3DDevice->GetViewport(&currentViewport);
			screenWidth = currentViewport.Width;
			screenHeight = currentViewport.Height;
			
			//Crea tutti i flares
			Locker.Create("Locker");
			Locker.SetTextureID( Locker.AddTexture("Textures\\Other\\Indicator.bmp", D3DXCOLOR(0,0,0,0), 1, 1, 0));
			Locker.SetDimensions( D3DXVECTOR2(Locker.GetTextureInfoID(0, false).x , Locker.GetTextureInfoID(0, false).y)  );
			Locker.SetMode(true);
			Locker.SetPosition(D3DXVECTOR2(0, 0));
			Locker.SetRotation(0.0f);
			Locker.SetTrasparency(true, 0.0f);



			isCreated=true;
		}
		else
		{
			DELog::LogError("<li>%s : DECore::D3DDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}


DLL_API void DELocker::CalculatePosition(D3DXVECTOR3 newPosition, D3DXVECTOR3 viewPos, STRUCT_DE_FOV fovProperties)
{
	if ((DECore::D3DDevice!=NULL)&&(isCreated))
	{
		//Preleva la matrici della vista corrente
		D3DXMATRIX viewTransform;
		DECore::D3DDevice->GetTransform(D3DTS_VIEW,& viewTransform);
		//Calcola se il punto è dentro o fuori il frustrum
		if (!DEUtility::FOVCheck(pos3D, 1, viewTransform, fovProperties, &viewPos ))	cull=true;
		else cull=false;

		//Nuova posizione
		pos3D = newPosition;
		
		D3DXMATRIX	viewMat;
		D3DXMATRIX	projMat;
		DECore::D3DDevice->GetTransform(D3DTS_VIEW,&viewMat);
		DECore::D3DDevice->GetTransform(D3DTS_PROJECTION,&projMat);
			
		
		D3DXVec3Project(&pos2D, &pos3D, &currentViewport, &projMat, &viewMat, NULL);

	}

}



DLL_API void DELocker::Render()
{
	if (isCreated)
	{
		//if (cull==false)
		{
			if (pos2D.x - (unsigned int)((float)Locker.GetDimensions().x/2) <=0)						pos2D.x = (float)(Locker.GetDimensions().x/2);
			if (pos2D.x + (unsigned int)((float)Locker.GetDimensions().x/2) >=screenWidth)				pos2D.x = (float)screenWidth - (float)(Locker.GetDimensions().x/2);
			if (pos2D.y - (unsigned int)((float)Locker.GetDimensions().y/2) <=0)						pos2D.y = ((float)Locker.GetDimensions().y/2);
			if (pos2D.y + (unsigned int)((float)Locker.GetDimensions().y/2) >=screenHeight)			pos2D.y = (float)screenHeight - (float)(Locker.GetDimensions().y/2);//*/
			//Disegna il pannello
			Locker.SetPosition(D3DXVECTOR2(pos2D.x - (Locker.GetDimensions().x/2), pos2D.y - (Locker.GetDimensions().y/2)));
			Locker.SetTrasparency(true, 0.0f);
			Locker.Render();
		}

	}
}

DLL_API bool DELocker::GetCreated()
{
	return isCreated;
}