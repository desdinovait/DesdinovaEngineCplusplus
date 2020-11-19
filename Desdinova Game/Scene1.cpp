#include "General.h"
#include "Scene1.h"
#include "Scene3.h"
#include "main.h"


bool Scene1_Load()
{
	//*** Camere ***
	Scene1_Camera.Create(D3DXVECTOR3(0,105,-90), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 1.0f, true, "Scene1_Camera2");
	Scene1.AddCamera(&Scene1_Camera);
	Scene1.SetCurrentCamera(0);


	//*** Font ***
	Scene1_Font.CreateSprite("Burton's Nightmare", 20, false, false, "Scene1_Font");

	/*** Sky ***
	Scene1_SkyCylinder.Create("Textures\\Skies\\Cylinder\\01\\Top.bmp", "Textures\\Skies\\Cylinder\\01\\Top.bmp", "Textures\\Skies\\Cylinder\\01\\Wide.bmp", 30.0f, 20.0f, 8, "Scene3_SkyCylinder");
	Scene1_SkyCylinder.SetRotation(D3DXVECTOR3(0,0,0));
	Scene1.AddSkyCylinder(&Scene1_SkyCylinder);//*/

	/*** Luci ***
	Scene1_Light1.Create("Textures\\Particles\\Particle7.bmp", "Scene1_Light1");
	Scene1_Light1.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene1_Light1.SetPosition(D3DXVECTOR3(0,20,0));
	Scene1_Light1.SetRange(20);
	Scene1_Light1.SetDiffuse(D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.0f));
	Scene1_Light1.SetFalloff(0.45f);
	Scene1_Light1.SetEnable(true);
	Scene1.AddLight(&Scene1_Light1);//*/



	/** Logo **
	Scene1_Logo.Create("Models\\Pong\\Logo.dem", "Scene1_Logo");	
	Scene1.AddModel(&Scene1_Logo);
	Scene1_Logo.SetPosition(D3DXVECTOR3(0, 50, 0));	//*/


	//** Campo **
	Scene1_Plane.Create("Models\\Pong\\plane.dem", "Scene1_Plane");	
	Scene1.AddModel(&Scene1_Plane);
	Scene1_Plane.SetPosition(D3DXVECTOR3(0, 0, 0));	

	Scene1_Edge1.Create("Models\\Pong\\Edge.dem", "Scene1_Edge1");	
	Scene1.AddModel(&Scene1_Edge1);
	Scene1_Edge1.SetPosition(D3DXVECTOR3(40, Scene1_Edge1.GetModelInfo().Info_Height_Scaled / 2.0f, 0));	

	Scene1_Edge2.Create("Models\\Pong\\Edge.dem", "Scene1_Edge2");	
	Scene1.AddModel(&Scene1_Edge2);
	Scene1_Edge2.SetPosition(D3DXVECTOR3(-40, Scene1_Edge2.GetModelInfo().Info_Height_Scaled / 2.0f, 0));	


	//** Giocatori **
	Scene1_Player1.Create("Models\\Pong\\Player.dem", "Scene1_Player1");	
	Scene1.AddModel(&Scene1_Player1);
	Scene1_Player1.SetPosition(D3DXVECTOR3(0, 30, 40));	

	Scene1_Player2.Create("Models\\Pong\\Player.dem", "Scene1_Player2");	
	Scene1.AddModel(&Scene1_Player2);
	Scene1_Player2.SetPosition(D3DXVECTOR3(0, 30, -40));	


	//** Palla **
	Scene1_Ball.Create("Models\\Pong\\Ball.dem", "Scene1_Ball");	
	Scene1.AddModel(&Scene1_Ball);
	Scene1_Ball.SetPosition(D3DXVECTOR3(0, 30, 0));	


	//*** Particelle ***
	Scene1_Particle1.Create("Scene1_Particle1");
	//Scene1_Particle1.Load("ParticlesSystems\\Fire.psy");
	Scene1_Particle1.Load2("ParticlesSystems\\Prova.txt");
	Scene1_Particle1.SetPosition(Scene1_Ball.GetPosition());
	Scene1.AddParticlesSystem(&Scene1_Particle1);


	//** Variabili **
	Scene1_Player1_Score = 0;
	Scene1_Player2_Score = 0;


	//** Trigger **
	Scene1.AddTrigger_Box(D3DXVECTOR3(-30,0,0), D3DXVECTOR3(5,5,5));
	Scene1.AddTrigger_Sphere(D3DXVECTOR3(20,0,0), 6);


	//** VIDEO (rivedere) ***
	//Scene1_Video.Create(Engine.GetHWND(), "Videos\\Ati.mpg");
	//Scene1.AddVideo(&Scene1_Video);
	//Scene1.PlayVideos();

	return true;
}

bool Scene1_Input()
{
	if(Engine.GetDInput().Pressed_Keyboard(DIK_V,false))	Engine.SetCurrentScene(2, true);
	if (Engine.GetDInput().Pressed_MouseUp(DE_MOUSEBUTTON_RIGHTBUTTON))Scene1_RightPressed =! Scene1_RightPressed;
	if (Scene1_RightPressed)
	{
		if (Scene1.GetCurrentCameraID()==0)
		{
			Scene1_Camera.MoveCameraByMouse(Engine.GetDInput().GetCursorPosition());
			if(Engine.GetDInput().Pressed_Keyboard(DIK_D)) 	Scene1_Camera.StrafeRight();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_A)) 	Scene1_Camera.StrafeLeft();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_W))	Scene1_Camera.MoveUp(true,true,true);
			if(Engine.GetDInput().Pressed_Keyboard(DIK_S))	Scene1_Camera.MoveDown(true,true,true);
		}
	}


	if(Engine.GetDInput().Pressed_Keyboard(DIK_A)) 	Scene1_Player1.ApplyForce(D3DXVECTOR3(-1,0,0),400);
	if(Engine.GetDInput().Pressed_Keyboard(DIK_D)) 	Scene1_Player1.ApplyForce(D3DXVECTOR3(1,0,0),400);
	if(Engine.GetDInput().Pressed_Keyboard(DIK_W)) 	Scene1_Player1.ApplyForce(D3DXVECTOR3(0,0,1),400);
	if(Engine.GetDInput().Pressed_Keyboard(DIK_S)) 	Scene1_Player1.ApplyForce(D3DXVECTOR3(0,0,-1),400);

	if(Engine.GetDInput().Pressed_Keyboard(DIK_J)) 	Scene1_Player2.ApplyForce(D3DXVECTOR3(-1,0,0),400);
	if(Engine.GetDInput().Pressed_Keyboard(DIK_L)) 	Scene1_Player2.ApplyForce(D3DXVECTOR3(1,0,0),400);
	if(Engine.GetDInput().Pressed_Keyboard(DIK_I)) 	Scene1_Player2.ApplyForce(D3DXVECTOR3(0,0,1),400);
	if(Engine.GetDInput().Pressed_Keyboard(DIK_K)) 	Scene1_Player2.ApplyForce(D3DXVECTOR3(0,0,-1),400);


	if(Engine.GetDInput().Pressed_Keyboard(DIK_X,false))
	{
		Scene1_Ball.ApplyForce(D3DXVECTOR3(100, 0, -50), 500);
	}
	
	return true;
}

bool Scene1_Render3D()
{
	//Vincoli di Rotazione
	Scene1_Player1.SetRotation(D3DXVECTOR3(0,0,0));
	Scene1_Player2.SetRotation(D3DXVECTOR3(0,0,0));

	Scene1_Particle1.SetPosition(Scene1_Ball.GetPosition());

	return true;
}

bool Scene1_Render2D()
{
	Scene1_Font.BeginTextSprite();
	sprintf(Scene1_BufferInfo, "Player 1 Score: %d", Scene1_Player1_Score);
	Scene1_Font.DrawSprite(Scene1_BufferInfo, 10, 50, D3DCOLOR_XRGB(0, 0, 255));
	
	//if (Scene1_Player2.InsideTrigger(0, NX_TRIGGER_ON_STAY))
	if (Scene1_Player2.CheckCollisionWith(Scene1_Player1, NX_ALL_SHAPES))
	{
		sprintf(Scene1_BufferInfo, "YESSS");
		Scene1_Font.DrawSprite(Scene1_BufferInfo, 10, 80, D3DCOLOR_XRGB(255, 0, 0));
	}
	else
	{
		sprintf(Scene1_BufferInfo, "NOOOO");
		Scene1_Font.DrawSprite(Scene1_BufferInfo, 10, 80, D3DCOLOR_XRGB(0, 0, 255));
	}//*/
	Scene1_Font.EndTextSprite();

	return true;
}

bool Scene1_Invalidate()
{
	return true;
}

bool Scene1_Restore()
{
	return true;
}

bool Scene1_Release()
{
	return true;
}

