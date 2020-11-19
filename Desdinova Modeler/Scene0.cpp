#include "General.h"
#include "Scene0.h"

///**********************************
///*********** SCENEA 0 *************
///**********************************

bool DEScene1::Load()
{	
	currentCHILDID = -1;
	//Flags apertura
	currentCreationMode = 0;	//0=None, 1=Model, 2=Entity, 3=Particles
	opened=false;
	//Mouse
	rightPressed=false;
	leftPressed=false;
	//Altro
	showInfo=true;
	showGrid=false;
	showAxes=true;
	//Finistre attive
	showEntitiesManager=false;
	showModelsManager=false;
	showParticlesManager=false;
	showLightsManager=false;
	showCameraControl=false;
	//Luci
	totalLights = 1;
	currentLight = 0;
	//Render prop
	renderProperties.useLOD = true;
	renderProperties.useBounding = 0;
	renderProperties.useAxes = true;
	renderProperties.useFrustrumCulling = true;
	renderProperties.useWireframe = false;
	renderProperties.useLighting = false;
	renderProperties.useFog = false;

	//Font
	FontInfo.CreateSprite("Verdana", 9, false, false, "FontInfo");
	FontHit.Create("Verdana", 9, true, false, "FontHit");
	
	//Point light
	Light.Create(D3DLIGHT_POINT);
	Light.SetPosition(0.0f, 0.0f, -100.0f);
	Light.SetRange(500.0f);
	Light.SetDiffuse(1.0f, 1.0f, 1.0f, 0.0f);
	Light.SetSpecular(1.0f, 1.0f, 1.0f, 0.0f);
	Light.SetAmbient(1.0f, 1.0f, 1.0f, 0.0f);
	Light.SetEnable(true);
	//Luce ambientale
	Engine.GetD3DDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(168,168,168));//*/

	//Axes
	SceneAxes.Create("SceneAxes");

	//Grid
	Grid.Create(200, 1.0f, D3DCOLOR_XRGB(0, 255, 0), "Grid");

	return true;
}

bool DEScene1::Input()
{
	//** INPUT MOUSE **
	if (Engine.engineInput.Pressed_Mouse(DE_MOUSEBUTTON_RIGHTBUTTON))	rightPressed =! rightPressed;
	if (Engine.engineInput.Pressed_Mouse(DE_MOUSEBUTTON_LEFTBUTTON))	leftPressed = true;

	//** CONTROLLI INPUT JOYSTICK **
	if (Engine.engineInput.Pressed_Joy(DE_JOYBUTTON_0BUTTON))			rightPressed =! rightPressed;
	if (Engine.engineInput.Pressed_Joy(DE_JOYBUTTON_1BUTTON))			leftPressed = true;

	//** MOVIMENTO CAMERA **
	if (rightPressed)
	{
		Camera.MoveCameraByMouse(Engine.engineInput.GetCursorPosition());

		if (!Engine.engineConsole.GetEnable())
		{
			//Tastiera...solo se non è attiva la console
			if(Engine.engineInput.Pressed_Keyboard(DIK_D)) 	Camera.StrafeRight();
			if(Engine.engineInput.Pressed_Keyboard(DIK_A)) 	Camera.StrafeLeft();
			if(Engine.engineInput.Pressed_Keyboard(DIK_W))	Camera.MoveUp(true,true,true);
			if(Engine.engineInput.Pressed_Keyboard(DIK_S))	Camera.MoveDown(true,true,true);				
		}
	}
	
	//Update della camera
	//Camera.UpdateInput();

	return true;
}

bool DEScene1::Update()
{
	return true;
}

bool DEScene1::Execute3D()
{
	//Update della camera
	//Camera.UpdateView();

	//** GRIGLIA **
	if (showGrid)
	{
		Grid.CalculateMatrix(D3DXVECTOR3(0,0,0), D3DXVECTOR3(1,1,1), D3DXVECTOR3(0,0,0));
		Grid.Render();
	}//*/

	//** ASSI CENTRALI **
	if (showAxes)
	{	
		SceneAxes.CalculateMatrix(D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,100,100), D3DXVECTOR3(0,0,0));		
		SceneAxes.Render();
	}//*/

	//Qualcosa è aperto
	if (opened)
	{
		//** MODELLO **
		if (currentCreationMode==1)
		{
			//Disegna il modello
			D3DXMATRIX modelIdentity;
			D3DXMatrixIdentity(&modelIdentity);
			static float aaa=0;
			aaa=aaa+0.1f;
			Model.SetMatrix(Model.CalculateMatrix());
			Model.Render(Camera.GetView(), Camera.GetPosition(), Scene1.GetFOV());
			//Per arma usare un modello screen aligned e le seguenti righe
			//Model.posValue.x = Camera.m_vPosition.x;
			//Model.posValue.y = Camera.m_vPosition.y-2.0f;
			//Model.posValue.z = Camera.m_vPosition.z;			
			//Piking
			if (leftPressed)	
			{					
				D3DXVECTOR2 ptCursor = Engine.engineInput.GetCursorPosition();
				Model.CheckPicked(ptCursor);	
				leftPressed = false;
			}
		}

		//** ENTITA' **
		if (currentCreationMode==2)
		{
			//Disegna l'entità
			D3DXMATRIX	entityIdentity;
			D3DXMatrixIdentity(&entityIdentity);
			static float bbb=0;
			bbb = bbb + 0.1f;
			Entity.SetRotation(D3DXVECTOR3(0,D3DXToRadian(bbb),0));
			Entity.SetPosition(D3DXVECTOR3(200,0,0));
			Entity.SetRenderProperties(renderProperties);
			Entity.Render(Camera.GetView(), Camera.GetPosition(), Scene1.GetFOV(), entityIdentity);
			//Piking
			if (leftPressed)
			{
				D3DXVECTOR2 ptCursor = Engine.engineInput.GetCursorPosition();
				Entity.CheckPicked(ptCursor);	
				leftPressed = false;
			}
		}

		//** SISTEMA PARTICELLARE **
		if (currentCreationMode==3)
		{	
			ParticleSystems.Execute(0);	
		}
	}//opened*/

	return true;
}





bool DEScene1::Execute2D()
{
	D3DCOLOR	pickColor = D3DCOLOR_XRGB(255, 255, 255);
	//Mostra le informazioni a video
	if (showInfo)
	{
		//Qualcosa è aperto
		if (opened)
		{			
			//** MODELLO **
			if ((currentCreationMode==1)&&(Model.GetCulled()==false))
			{
				//Is picked
				if (Model.GetPicked())	pickColor=D3DCOLOR_XRGB(0, 255, 0);
				char bufferVertices[50]="";
				sprintf(bufferVertices,"Vertices: %.0f",(float)Model.GetCurrentVertices());

				FontInfo.BeginTextSprite();
				FontInfo.DrawSprite(bufferVertices, 50, 350, D3DCOLOR_XRGB(0, 255, 255));
				FontInfo.EndTextSprite();

				/*LOD
				char bufferLOD[50]="";
				char bufferVertices[50]="";
				char bufferDistance[50]="";
				sprintf(bufferLOD,"LOD: %.0f - %.0f",(float)Model.currentLod,(float)Model.totalLods-1);
				sprintf(bufferVertices,"Vertices: %.0f",(float)Model.Object[Model.currentLod].totalVertices);
				D3DXVECTOR3 res;
				D3DXVec3Subtract( &res,&Camera.GetPosition(), &Entity.Parent.Model.finalCenterCoord);
				float distanceCaptured = D3DXVec3Length(&res);
				sprintf(bufferDistance,"Distance: %.3f",(float)distanceCaptured);
				FontInfo.BeginTextSprite();
				FontInfo.DrawSprite(bufferLOD, 2, 30,  pickColorModel);
				FontInfo.DrawSprite(bufferVertices, 2, 40, pickColorModel);
				FontInfo.DrawSprite(bufferDistance, 2, 50, pickColorModel);			
				FontInfo.EndTextSprite();*/
			}

			//** ENTITA' **
			if (currentCreationMode==2)
			{
				//Is picked
				if (Entity.GetPicked())	pickColor=D3DCOLOR_XRGB(0, 255, 0);
				//ID Figlio corrente
				char bufferChildID[50]="";
				if (currentCHILDID >=0)	sprintf(bufferChildID,"Child ID: %.0f",(float)currentCHILDID);
				else					sprintf(bufferChildID,"Parent",(float)currentCHILDID);
				//LOD
				//char bufferLOD[50]="";
				//if (currentCHILDID >=0)	sprintf(bufferLOD,"LOD: %.0f - %.0f",(float)Entity.Child[currentCHILDID].Model.currentLod,(float)Entity.Child[currentCHILDID].Model.totalLods-1);
				//else					sprintf(bufferLOD,"LOD: %.0f - %.0f",(float)Entity.Parent.Model.currentLod,(float)Entity.Parent.Model.totalLods-1);
				//Info vertices
				//char bufferVertices[50]="";
				//if (currentCHILDID >=0)	sprintf(bufferVertices,"Vertices: %.0f",(float)Entity.Child[currentCHILDID].Model.Object[Entity.Child[currentCHILDID].Model.currentLod].totalVertices);
				//sprintf(bufferVertices,"Vertices: %.0f",(float)Entity.GetParent().GetCurrentVertices());
				/*Distanza della camera dal centro dell'oggetto
				D3DXVECTOR3 res;
				if (currentCHILDID >=0)	D3DXVec3Subtract( &res,&Camera.GetPosition(), &Entity.Child[currentCHILDID].Model.finalCenterCoord);
				else					D3DXVec3Subtract( &res,&Camera.GetPosition(), &Entity.Parent.Model.finalCenterCoord);					
				float distanceCaptured = D3DXVec3Length(&res);
				char bufferDistance[50]="";
				sprintf(bufferDistance,"Distance: %.3f",(float)distanceCaptured);*/
				
				//FontInfo.BeginTextSprite();
				//FontInfo.DrawSprite(bufferChildID, 2, 30, pickColorEntity);				
				//FontInfo.DrawSprite(bufferLOD, 2, 40, pickColorEntity);
				//FontInfo.DrawSprite(bufferVertices, 50, 350, D3DCOLOR_XRGB(0, 255, 255));
				//FontInfo.DrawSprite(bufferDistance, 2, 60, pickColorEntity);
				//FontInfo.EndTextSprite();
			}		
		}//opened//*/		


		//Note: This TESTER RELEASE is preliminary and is subject to change. Check out www.desdinova.altervista.org for latest versions and more informations.
		int cur=0;
		FontHit.DrawFormatted(FontHit.GenerateTypedText("Note: This TESTER RELEASE is preliminary and is subjected to change.\n\rCheck out www.desdinova.altervista.org for latest versions and more informations.", 0.075f, false, cur), 2, Engine.GetSwapChainHeight(0)-50, 550, 100, DT_LEFT | DT_WORDBREAK | DT_EXPANDTABS, pickColor);


		//** INFORMAZIONI GENERALI **
		//FrameRate
		FontInfo.BeginTextSprite();
		if (Engine.GetCurrentSwapChainID() == Engine.GetActiveSwapChainID())
			FontInfo.DrawSprite("Status: Active", 2,18,D3DCOLOR_XRGB(255, 0, 0));
		else
			FontInfo.DrawSprite("Status: Inactive", 2,18,D3DCOLOR_XRGB(0, 255, 0));
		FontInfo.EndTextSprite();//*/


	}//info//*/	
	return true;
}


bool DEScene1::Release()
{
	Light.Release();
	Camera.Release();
	SceneAxes.Release();
	Grid.Release();
	FontInfo.Release();
	FontHit.Release();
	if (currentCreationMode==1) Model.Release();
	if (currentCreationMode==2) Entity.Release();
	if (currentCreationMode==3) ParticleSystems.Release();
	return true;
}

bool DEScene1::Invalidate()
{
	Camera.Invalidate();
	Light.Invalidate();
	SceneAxes.Invalidate();
	Grid.Invalidate();
	FontInfo.Invalidate();
	FontHit.Invalidate();
	if (currentCreationMode==1) Model.Invalidate();
	if (currentCreationMode==2) Entity.Invalidate();
	if (currentCreationMode==3) ParticleSystems.Invalidate();
	return true;
}

bool DEScene1::Restore()
{
	Camera.Restore();
	Light.Restore();
	SceneAxes.Restore();
	Grid.Restore();
	FontInfo.Restore();
	FontHit.Restore();
	if (currentCreationMode==1) Model.Restore();
	if (currentCreationMode==2) Entity.Restore();
	if (currentCreationMode==3) ParticleSystems.Restore();
	return true;
}