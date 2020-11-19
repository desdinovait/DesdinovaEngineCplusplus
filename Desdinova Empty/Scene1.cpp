#include "General.h"

bool Scene1_Load()
{
	//*** Scena ***
	Scene1.SetCinematicEffect(false);
	Scene1.SetPostProcessingEnable(false);
	Scene1.SetPostProcessingInclude2D(false);
	Scene1.SetPostProcessingTechnique("TechniqueBloom");

	//*** Gruppi di suono ***
	Scene1_MusicGroup.Create("Scene1_MusicGroup");
	Scene1_SoundGroup.Create("Scene1_SoundGroup");
	Scene1.AddSoundGroup(&Scene1_MusicGroup);
	Scene1.AddSoundGroup(&Scene1_SoundGroup);
	
	/*** Souni ***
	Scene1_SoundBackground.Create("Music/Monstro.mp3", true, false, FMOD_HARDWARE | FMOD_2D);
	Scene1_SoundBackground.SetMode(FMOD_LOOP_NORMAL);
	Scene1_SoundBackground.Play();
	Scene1_SoundBackground.SetChannelGroup(Scene1_MusicGroup);
	Scene1.AddSound(&Scene1_SoundBackground);//*/

	/*Scene1_SoundFX1.Create("Sounds/Torch.wav", false, false, FMOD_HARDWARE | FMOD_3D | FMOD_3D_LINEARROLLOFF);
	Scene1_SoundFX1.Set3DMinMaxDistance(20.0f, 100.0f);	
	Scene1_SoundFX1.SetMode(FMOD_LOOP_NORMAL);
	Scene1_SoundFX1.Play();
	FMOD_VECTOR sound_pos1 = { 30.0f, 0.0f, -20.0f };
	FMOD_VECTOR sound_vel1 = { 0.0f, 0.0f, 0.0f };
	Scene1_SoundFX1.Set3DAttributes(&sound_pos1, &sound_vel1);
	Scene1_SoundFX1.SetChannelGroup(Scene1_SoundGroup);
	Scene1.AddSound(&Scene1_SoundFX1);//*/

	/*Scene1_SoundFX2.Create("Sounds/Torch.wav", false, false, FMOD_HARDWARE | FMOD_3D | FMOD_3D_LINEARROLLOFF);
	Scene1_SoundFX2.Set3DMinMaxDistance(20.0f, 100.0f);	
	Scene1_SoundFX2.SetMode(FMOD_LOOP_NORMAL);
	Scene1_SoundFX2.Play();
	FMOD_VECTOR sound_pos2 = { -30.0f, 0.0f, -20.0f };
	FMOD_VECTOR sound_vel2 = { 0.0f, 0.0f, 0.0f };
	Scene1_SoundFX2.Set3DAttributes(&sound_pos2, &sound_vel2);
	Scene1_SoundFX2.SetChannelGroup(Scene1_SoundGroup);
	Scene1.AddSound(&Scene1_SoundFX2);//*/


	//*** Camere ***
	Scene1_Camera.Create(D3DXVECTOR3(0,105,-90), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 0.015f, false, "Scene1_Camera2");
	Scene1.AddCamera(&Scene1_Camera);
	Scene1.SetCurrentCamera(0);

	//SkyCylinder
	Scene1_Sky.Create("Textures\\Skies\\Cylinder\\01\\Top.bmp", "Textures\\Skies\\Cylinder\\01\\Top.bmp", "Textures\\Skies\\Cylinder\\01\\Wide.bmp", 30.0f, 20.0f, 8, "Scene1_Sky");
	Scene1_Sky.SetScale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));
	//Scene1_Sky.Create("Textures\\Skies\\Sphere\\01\\Sky.bmp", 30, 20, "Scene1_Sky");
	//Scene1_Sky.SetScale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));
	//Scene1_Sky.Create("Textures\\Skies\\Box\\08\\Front.jpg", "Textures\\Skies\\Box\\08\\Back.jpg", "Textures\\Skies\\Box\\08\\Left.jpg", "Textures\\Skies\\Box\\08\\Right.jpg", "Textures\\Skies\\Box\\08\\Top.jpg", "Textures\\Skies\\Box\\08\\Bottom.jpg", "Scene1_Sky");
	//Scene1_Sky.SetScale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));
	Scene1_Sky.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	Scene1.AddSky(&Scene1_Sky);//*/

	//*** Luci ***
	Scene1_Light1.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light1");
	Scene1_Light1.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene1_Light1.SetPosition(D3DXVECTOR3(0,30,0));
	Scene1_Light1.SetDiffuse(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	Scene1_Light1.SetFalloff(20.0f);
	Scene1_Light1.SetRange(40.0f);
	Scene1_Light1.SetEnable(true);
	Scene1.AddLight(&Scene1_Light1);//*/

	Scene1_Light2.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light2");
	Scene1_Light2.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene1_Light2.SetPosition(D3DXVECTOR3(10,30,0));
	Scene1_Light2.SetDiffuse(D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f));
	Scene1_Light2.SetFalloff(20.0f);
	Scene1_Light2.SetRange(40.0f);
	Scene1_Light2.SetEnable(true);
	Scene1.AddLight(&Scene1_Light2);//*/

	/*Scene1_Light3.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light3");
	Scene1_Light3.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene1_Light3.SetPosition(D3DXVECTOR3(-30, 0,-20));
	Scene1_Light3.SetDiffuse(D3DXCOLOR(0.50f, 0.50f, 1.0f, 1.0f));
	Scene1_Light3.SetFalloff(1000.0f);
	Scene1_Light3.SetRange(2000.0f);
	Scene1_Light3.SetEnable(true);
	Scene1.AddLight(&Scene1_Light3);//*/

	/*Scene1_Light4.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light4");
	Scene1_Light4.SetType(DE_LIGHTTYPE_PERPIXELDIRECTIONAL);
	Scene1_Light4.SetDirection(D3DXVECTOR3(0,0,-200));
	Scene1_Light4.SetDiffuse(D3DXCOLOR(1.0f, 0.8f, 0.8f, 1.0f));
	Scene1_Light4.SetEnable(true);
	Scene1.AddLight(&Scene1_Light4);//*/

	/*Scene1_Light5.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light3");
	Scene1_Light5.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene1_Light5.SetPosition(D3DXVECTOR3(2000,0,-20));
	Scene1_Light5.SetDiffuse(D3DXCOLOR(1.0f, 0.65f, 0.0f, 1.0f));
	Scene1_Light5.SetFalloff(300.0f);
	Scene1_Light5.SetRange(600.0f);
	Scene1_Light5.SetEnable(true);
	Scene1.AddLight(&Scene1_Light5);//*/

	//** Lens Flares ***
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare04.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.1f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare05.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.5f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare03.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 1.0f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare02.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.7f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare05.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.2f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare03.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, -0.2f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\flare06.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 1.2f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\Hexagon02.dds",   D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.9f);
	Scene1_LensFlare1.AddFlare("Textures\\Flares\\Hexagon02.dds",   D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.4f);
	Scene1_LensFlare1.AddEmitter2D("Textures\\Flares\\Streak01.dds",D3DXCOLOR(0.85f, 0.85f, 1.0f, 1.0f), 3.0f); 
	Scene1_LensFlare1.AddEmitter3D("Textures\\Flares\\ray03.dds",	D3DXCOLOR(0.85f, 0.85f, 1.0f, 1.0f), 20.0f);
	Scene1_LensFlare1.AddEmitter3D("Textures\\Flares\\flare08.dds",	D3DXCOLOR(0.85f, 0.85f, 1.0f, 1.0f), 8.0f);
	Scene1_LensFlare1.AddFlash("Textures\\Flares\\Flash.dds",		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
	Scene1_LensFlare1.Create(Scene1.GetFOV(), true, 0.05f, 50, true, "Scene1_LensFlare1"); 
	Scene1_LensFlare1.SetPosition(D3DXVECTOR3(20.0f, 20.0f, 200.0f));
	//Aggiunge i LensFlares alla scena
	Scene1.AddLensFlare(&Scene1_LensFlare1);//*/

	/*** Modelli ***
	Scene1_Planet[0].Create("Models\\Planet_Earth.dem", "Scene1_Planet[0]");	
	Scene1_Planet[1].Create("Models\\Planet_Moon.dem", "Scene1_Planet[1]");	
	Scene1_Planet[2].Create("Models\\Planet_Mars.dem", "Scene1_Planet[2]");	
	Scene1_Planet[3].Create("Models\\Planet_Mercury.dem", "Scene1_Planet[3]");	
	Scene1_Planet[4].Create("Models\\Planet_Venus.dem", "Scene1_Planet[4]");	
	Scene1_Planet[5].Create("Models\\Planet_Jupiter.dem", "Scene1_Planet[5]");	
	Scene1_Planet[6].Create("Models\\Planet_Saturn.dem", "Scene1_Planet[6]");	
	Scene1_Planet[7].Create("Models\\Planet_Uranus.dem", "Scene1_Planet[7]");
	for (int i=0;i<8;i++)
	{
		Scene1.AddModel(&Scene1_Planet[i]);
		Scene1_Planet[i].SetPosition(D3DXVECTOR3(i*250,15,0));
	}//*/

	//A lot of Cubes
	int c=0;
	for (int n=0; n<10; n++)
	{
		for (int b=-5; b<5; b++)
		{
			Scene1_Cube[c].Create("Models\\Cube.dem", "Scene1_Cube[n]");	
			Scene1.AddModel(&Scene1_Cube[c]);
			Scene1_Cube[c].SetPosition(D3DXVECTOR3(b * Scene1_Cube[c].GetModelInfo().Info_Width_Scaled, 3 + (n * Scene1_Cube[c].GetModelInfo().Info_Height_Scaled), 25));
			//Scene1_Cube[c].SetPosition(D3DXVECTOR3(b * Scene1_Cube[c].GetModelInfo().Info_Width_Scaled, 33 + (n * Scene1_Cube[c].GetModelInfo().Info_Height_Scaled), 10));
			c=c+1;
		}
	}//*/


	/*** Panels ***
	Scene1_Panel1.Create("Scene1_Panel1");
	Scene1_Panel1.SetTextureID( Scene1_Panel1.AddTexture("Textures\\Panels\\PhysX.bmp", D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1, 1, 0));
	Scene1_Panel1.SetDimensions( D3DXVECTOR2(Scene1_Panel1.GetTextureInfoID(Scene1_Panel1.GetTextureID()).x, Scene1_Panel1.GetTextureInfoID(Scene1_Panel1.GetTextureID()).y), false  );
	Scene1_Panel1.SetMode(true);
	Scene1_Panel1.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	Scene1_Panel1.SetPosition( D3DXVECTOR2(50, 50), true  );
	Scene1.AddPanel(&Scene1_Panel1);//*/

	Scene1_Panel2.Create("Scene1_Panel2");
	Scene1_Panel2.SetTextureID( Scene1_Panel2.AddTexture("Textures\\Panels\\Menu\\Voice.dds", D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 1, 1, 0));
	Scene1_Panel2.SetDimensions( D3DXVECTOR2(Scene1_Panel2.GetTextureInfoID(Scene1_Panel2.GetTextureID()).x, Scene1_Panel2.GetTextureInfoID(Scene1_Panel2.GetTextureID()).y), false  );
	Scene1_Panel2.SetMode(false);
	Scene1_Panel1.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	Scene1_Panel2.SetPosition( D3DXVECTOR2(Scene1.GetWidth()/2.0f - Scene1_Panel2.GetDimensions().x / 2.0f, 0), false  );
	Scene1.AddPanel(&Scene1_Panel2);//*/

	/*** Sistemi particellari ***
	Scene1_Particle1.Create("Scene1_Particle1");
	Scene1_Particle1.Load("ParticlesSystems\\Fire.psy");
	Scene1_Particle1.SetPosition(D3DXVECTOR3(-30,0,-20));
	Scene1.AddParticlesSystem(&Scene1_Particle1);
	
	Scene1_Particle2.Create("Scene1_Particle2");
	Scene1_Particle2.Load("ParticlesSystems\\Fire.psy");
	Scene1_Particle2.SetPosition(D3DXVECTOR3(30,0,-20));
	Scene1.AddParticlesSystem(&Scene1_Particle2);//*/

	//*** Fonts ***
	Scene1_Font1.Create("Verdana", 15, false, false, "Scene1_Font1");
	Scene1_Font1.SetText("Desdinova Engine");
	Scene1_Font1.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	Scene1_Font1.SetRectSprite(0, 23, Scene1.GetWidth(), 200, DT_CENTER);
	
	Scene1_Font2.Create("Verdana", 10, false, false, "Scene1_Font2");
	Scene1_Font2.SetText("Techdemo 1.0");
	Scene1_Font2.SetColor(D3DXCOLOR(0.2f, 0.2f, 1.0f, 1.0f));
	Scene1_Font2.SetRectSprite(0, 45, Scene1.GetWidth(), 200, DT_CENTER);
	
	/*Scene1_Font3.Create("Textures\\Fonts\\Bad.dds", D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 7, 13, "Scene1_Font3");
	Scene1_Font3.SetBitmapLettersString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789   .,:()-'!+\?/[]^ %=$#@*");
	Scene1_Font3.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.85f));
	Scene1_Font3.SetText("Techdemo 1.0");
	Scene1_Font3.SetRectBitmap(0, 45, 1.0f, 1, 0);*/

	Scene1.AddFont(&Scene1_Font1);
	Scene1.AddFont(&Scene1_Font2);
	Scene1.AddFont(&Scene1_Font3);//*/

	return true;
}

bool Scene1_Input()
{
	if (Engine.GetDInput().Pressed_MouseUp(DE_MOUSEBUTTON_RIGHTBUTTON))
	{
		Scene1_RightPressed =! Scene1_RightPressed;
	}
	//if (Scene1_RightPressed)
	{
		if (Scene1.GetCurrentCameraID()==0)
		{
			Scene1_Camera.MoveCameraByMouse(Engine.GetDInput().GetCursorPosition());
			if(Engine.GetDInput().Pressed_Keyboard(DIK_D)) 	Scene1_Camera.StrafeRight();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_A)) 	Scene1_Camera.StrafeLeft();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_W))	Scene1_Camera.MoveUp(true,true,true);
			if(Engine.GetDInput().Pressed_Keyboard(DIK_S))	Scene1_Camera.MoveDown(true,true,true);
		}
	}//*/
	return true;
}

bool Scene1_Render3D()
{
	/*Model Piking
	Scene1_Font2.SetColor(D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f));
	for (int i=0; i<8; i++)
	{
		if (Scene1_Planet[i].CheckPicked(Scene1.GetCursorPosition(), true))
		{
			Scene1_Font2.SetColor(D3DXCOLOR(0.2f, 1.0f, 0.2f, 1.0f));
			break;
		}
	}//*/

	/*Posizionamento luce dinamica
	static D3DXVECTOR4 gLightPos = D3DXVECTOR4(0.0f, 10.0f, -30.0f, 0.0f);
	const float kMinX = -45.5f;
	const float kMinY = 5.0f;
	const float kMaxX = 45.5f;
	const float kMaxY = 25.0f;
	const float kInc = 0.5f;
	static float vec[2] = { kInc, 0.0f };
	gLightPos.x += vec[0];
	gLightPos.y += vec[1];
	if(gLightPos.x > kMaxX)
	{	
		gLightPos.x = kMaxX;
		vec[0] = 0.0f;
		vec[1] = -kInc;
	}		
	else if(gLightPos.y < kMinY)
	{
		gLightPos.y = kMinY;
		vec[0] = -kInc;
		vec[1] = 0.0f;
	}
	else if(gLightPos.x < kMinX)
	{
		gLightPos.x = kMinX;
		vec[0] = 0.0f;
		vec[1] = kInc;
	}
	else if(gLightPos.y > kMaxY)
	{
		gLightPos.y = kMaxY;
		vec[0] = kInc;
		vec[1] = 0.0f;
	}
	Scene1_Light3.SetPosition(D3DXVECTOR3(gLightPos.x,gLightPos.y,gLightPos.z));//*/

	//Sta collindendo con l'attore specifico
	//if (Scene1_Cube[0].CheckCollisionWith(Scene1_Cube[1]))	Scene1_Font2.SetColor(D3DXCOLOR(0.2f, 1.0f, 0.2f, 1.0f));
	//														Scene1_Font2.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//Sta collidendo con qualcosa(in generale) oppure no
	//if (Scene1_Cube[0].CheckCollision(NX_NOTIFY_ON_TOUCH))	Scene1_Font2.SetColor(D3DXCOLOR(0.2f, 1.0f, 0.2f, 1.0f));
	//else														Scene1_Font2.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));


	return true;
}

bool Scene1_Render2D()
{
	static int cur1=0;
	D3DCOLOR testoColor = D3DXCOLOR(255,255,255,255);
	int testoX = 10;
	int testoY = 500;
	float testoVelocity = 0.05f;
	LPSTR testo1 = "Desdinova Engine";
	
	LPSTR testoFinale1 = Scene1_Font1.GenerateTypedText(testo1, testoVelocity, false, cur1);
	Scene1_Font1.SetText(testoFinale1);


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