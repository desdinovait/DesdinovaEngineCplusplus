#include "General.h"
#include "Scene3.h"

bool Scene3_Load()
{
	/*Gruppi di suono
	Scene3_Sound.Create("Sound Group");
	Scene3.AddSoundGroup(&Scene3_Sound);
	Scene3_Music.Create("Music Group");
	Scene3.AddSoundGroup(&Scene3_Music);//*/

	/*Musica
	Scene3_SoundBackground.Create("Music/Battle5-Level3.mp2", true, false, FMOD_HARDWARE | FMOD_2D);
	Scene3_SoundBackground.SetMode(FMOD_LOOP_NORMAL);
	Scene3_SoundBackground.Play();
	Scene3_SoundBackground.SetChannelGroup(Scene3_Music);
	Scene3.AddSound(&Scene3_SoundBackground);//*/

	/*Suoni
	Scene3_SoundFX1.Create("Sounds/torch.wav", false, false, FMOD_HARDWARE | FMOD_3D | FMOD_3D_LINEARROLLOFF);
	Scene3_SoundFX1.Set3DMinMaxDistance(40.0f, 150.0f);	
	Scene3_SoundFX1.SetMode(FMOD_LOOP_OFF);
	Scene3_SoundFX1.Play();
	FMOD_VECTOR pos1 = { 120.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel1 = { 0.0f, 0.0f, 0.0f };
	Scene3_SoundFX1.Set3DAttributes(&pos1, &vel1);
	Scene3_SoundFX1.SetChannelGroup(Scene3_Sound);
	Scene3.AddSound(&Scene3_SoundFX1);

	Scene3_SoundFX2.Create("Sounds/torch.wav", false, false, FMOD_HARDWARE | FMOD_3D | FMOD_3D_LINEARROLLOFF);
	Scene3_SoundFX2.Set3DMinMaxDistance(40.0f, 150.0f);	
	Scene3_SoundFX2.SetMode(FMOD_LOOP_OFF);
	Scene3_SoundFX2.Play();
	FMOD_VECTOR pos2 = { -120.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel2 = { 0.0f, 0.0f, 0.0f };
	Scene3_SoundFX2.Set3DAttributes(&pos2, &vel2);
	Scene3_SoundFX2.SetChannelGroup(Scene3_Sound);
	Scene3.AddSound(&Scene3_SoundFX2);

	Scene3_SoundFX3.Create("Sounds/Explosion01.wav", false, false, FMOD_HARDWARE | FMOD_3D | FMOD_3D_LINEARROLLOFF);
	Scene3_SoundFX3.Set3DMinMaxDistance(50.0f, 200.0f);	
	Scene3_SoundFX3.SetMode(FMOD_LOOP_OFF);
	Scene3_SoundFX3.Play();
	FMOD_VECTOR pos3 = { 0.0f, 0.0f, -450.0f };
	FMOD_VECTOR vel3 = { 0.0f, 0.0f, 0.0f };
	Scene3_SoundFX3.Set3DAttributes(&pos3, &vel3);
	Scene3_SoundFX3.SetChannelGroup(Scene3_Sound);
	Scene3.AddSound(&Scene3_SoundFX3);//*/


	//*** Camere ***
	//Crea le camere
	Scene3_Camera1.Create(D3DXVECTOR3(0,75,-85), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 1.0f, true, "Scene3_Camera1");
	Scene3.AddCamera(&Scene3_Camera1);
	Scene3_Camera2.Create(D3DXVECTOR3(45,45,45), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 1.0f, true, "Scene3_Camera2");
	Scene3.AddCamera(&Scene3_Camera2);
	Scene3.SetCurrentCamera(0);


	/*** Sistemi Partcellari ***
	//Crea i sistemi particellari
	Scene3_Particle1.Create("Scene3_Particle1");
	Scene3_Particle1.Load( "ParticlesSystems\\Fountain.psy");
	//Scene3_Particle1.Load("ParticlesSystems\\Prova.txt");
	Scene3_Particle1.SetPosition(D3DXVECTOR3(0,0,0));
	Scene3.AddParticlesSystem(&Scene3_Particle1);//*/

	/*Crea i sistemi particellari
	char newPSY[MAX_PATH];
	Scene3_Particle2.Create("Scene3_Particle2");
	Scene3_Particle2.Load("ParticlesSystems\\snow.psy");
	Scene3_Particle2.SetPosition(D3DXVECTOR3(0,0,0));
	Scene3.AddParticlesSystem(&Scene3_Particle2);//*/


	//*** Lens FLares ***
	/*Lens flare
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare04.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.1f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare05.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.5f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare03.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.0f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare02.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.7f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare05.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.2f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare03.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, -0.2f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\flare06.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.2f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\Hexagon02.dds",    D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.9f);
	Scene3_LensFlare1.AddFlare("Textures\\Flares\\Hexagon02.dds",    D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.4f);
	Scene3_LensFlare1.AddEmitter2D("Textures\\Flares\\Streak01.dds", D3DCOLOR_ARGB(192,128,128,255), 2.0f); 
	Scene3_LensFlare1.AddEmitter3D("Textures\\Flares\\ray04.dds", D3DCOLOR_ARGB(65,255,255,255), 400.0f);
	Scene3_LensFlare1.AddFlash("Textures\\Flares\\Flash.dds", D3DCOLOR_ARGB(128,204,205,236), D3DCOLOR_XRGB(0,0,0));
	Scene3_LensFlare1.Create(Scene3.GetFOV(), true, 0.05f, 50, true, "Scene3_LensFlare1"); 
	Scene3_LensFlare1.SetPosition(D3DXVECTOR3(-200.0f, 2.0f, 0.0f));
	//Aggiunge i LensFlares alla scena
	Scene3.AddLensFlare(&Scene3_LensFlare1);*/

	/*Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare04.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.1f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare05.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.5f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare03.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.0f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare02.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.7f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare05.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.2f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare03.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, -0.2f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\flare06.dds", D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.2f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\Hexagon02.dds",    D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.9f);
	Scene3_LensFlare2.AddFlare("Textures\\Flares\\Hexagon02.dds",    D3DCOLOR_ARGB(32,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.4f);
	Scene3_LensFlare2.AddEmitter2D("Textures\\Flares\\Streak01.dds", D3DCOLOR_ARGB(192,128,128,255), 2.0f); 
	Scene3_LensFlare2.AddEmitter3D("Textures\\Flares\\ray04.dds", D3DCOLOR_ARGB(65,255,255,255), 400.0f);
	Scene3_LensFlare2.AddFlash("Textures\\Flares\\Flash.dds", D3DCOLOR_ARGB(128,204,205,236), D3DCOLOR_XRGB(0,0,0));
	Scene3_LensFlare2.Create(Scene3.GetFOV(), true, 0.05f, 50, true, "Scene3_LensFlare2"); 
	Scene3_LensFlare2.SetPosition(D3DXVECTOR3(300.0f, 2.0f, 0.0f));
	//Aggiunge i LensFlares alla scena
	Scene3.AddLensFlare(&Scene3_LensFlare2);//*/

	//SkyCylinder
	//Scene3_SkyCylinder.Create("Textures\\Skies\\Cylinder\\01\\Top.bmp", "Textures\\Skies\\Cylinder\\01\\Top.bmp", "Textures\\Skies\\Cylinder\\01\\Wide.bmp", 30.0f, 20.0f, 8, "Scene3_SkyCylinder");
	//Scene3_SkyCylinder.SetRotation(D3DXVECTOR3(0,0,0));
	//Scene3.AddSkyCylinder(&Scene3_SkyCylinder);//*/
	//Scene3_SkyBox.Create("Textures\\Skies\\Box\\05\\Front.tga", "Textures\\Skies\\Box\\05\\Back.tga", "Textures\\Skies\\Box\\05\\Left.tga", "Textures\\Skies\\Box\\05\\Right.tga", "Textures\\Skies\\Box\\05\\Top.tga", "Textures\\Skies\\Box\\05\\Bottom.tga", "Scene3_SkyBox");
	//Scene3_SkyBox.SetScale(D3DXVECTOR3(10,10,10));
	//Scene3.AddSkyBox(&Scene3_SkyBox);//*/


	//Cubi
	int c=0;
	for (int n=0; n<10; n++)
	{
		for (int b=-10; b<10; b++)
		{
			Scene3_Cubes[c].Create("Models\\Cube.dem", "Scene3_Cube");	
			Scene3.AddModel(&Scene3_Cubes[c]);
			Scene3_Cubes[c].SetPosition(D3DXVECTOR3(b * Scene3_Cubes[c].GetModelInfo().Info_Width_Scaled, 3 + (n * Scene3_Cubes[c].GetModelInfo().Info_Height_Scaled), 45));
			c=c+1;
		}
	}//*/
	/*for (int n=0; n<10; n++)
	{
		for (int b=-10; b<10; b++)
		{
			Scene3_Cubes[100+c].Create("Models\\Cube.dem", "Scene3_Cube");	
			Scene3.AddModel(&Scene3_Cubes[100+c]);
			Scene3_Cubes[100+c].SetPosition(D3DXVECTOR3(45,3+(n*5),b*5));
			c=c+1;
		}
	}//*/


	Scene3_Moon.Create("Models\\Plane.dem", "Scene3_Moon");	
	Scene3.AddModel(&Scene3_Moon);
	Scene3_Moon.SetPosition(D3DXVECTOR3(0, 1.0f, 0));	

	/*Scene3_Mars.Create("Models\\Test2.dem", "Scene3_Mars");	
	Scene3.AddModel(&Scene3_Mars);
	Scene3_Mars.SetPosition(D3DXVECTOR3(0, 0, 0));/*/
	
	/*Scene3_Earth.Create("Models\\Test.dem", "Scene3_Earth");	
	Scene3.AddModel(&Scene3_Earth);
	Scene3_Earth.SetPosition(D3DXVECTOR3(0, 20, 0));	
	Scene3_Earth.SetRotation(D3DXVECTOR3(0, 0, 0));	
	Scene3_Earth.SetLinearVelocity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	Scene3_Earth.SetAngularVelocity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	Scene3_Moon.Create("Models\\Test2.dem", "Scene3_Moon");	
	Scene3.AddModel(&Scene3_Moon);
	Scene3_Moon.SetPosition(D3DXVECTOR3(20, 20, 0));	
	Scene3_Moon.SetRotation(D3DXVECTOR3(0, 0, 0));	
	Scene3_Moon.SetLinearVelocity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	Scene3_Moon.SetAngularVelocity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));//*/


	/* SPACE DEMO
	Asteroidi
	for (int n=0; n<1000; n++)
	{
		Scene3_Asteroid[n].Create("Models\\Asteroid.dem", "Scene3_Asteroid");	
		Scene3.AddModel(&Scene3_Asteroid[n]);
		Scene3_Asteroid[n].SetPosition(D3DXVECTOR3(DEUtility::GetRandomMinMax(-500, 500), DEUtility::GetRandomMinMax(-500, 500), DEUtility::GetRandomMinMax(-500, 500)));	
		Scene3_Asteroid[n].SetRotation(D3DXVECTOR3(DEUtility::GetRandomMinMax(0,360), DEUtility::GetRandomMinMax(0,360), DEUtility::GetRandomMinMax(0,360)));	
		float scaleValue = DEUtility::GetRandomMinMax(0.01f,0.1f);
		Scene3_Asteroid[n].SetScale(D3DXVECTOR3(scaleValue, scaleValue, scaleValue));	
	}

	Scene3_Moon.Create("Models\\Moon.dem", "Scene3_Moon");	
	Scene3.AddModel(&Scene3_Moon);
	Scene3_Moon.SetPosition(D3DXVECTOR3(-1000, 25, 0));	
	Scene3_Moon.SetScale(D3DXVECTOR3(50, 50, 50));	

	Scene3_Mars.Create("Models\\Mars.dem", "Scene3_Mars");	
	Scene3.AddModel(&Scene3_Mars);	
	Scene3_Mars.SetPosition(D3DXVECTOR3(500, 500, -500));	
	Scene3_Mars.SetScale(D3DXVECTOR3(50, 50, 50));	
	
	Scene3_Earth.Create("Models\\Earth.dem", "Scene3_Earth");	
	Scene3.AddModel(&Scene3_Earth);
	Scene3_Earth.SetPosition(D3DXVECTOR3(-500, 0, 0));	
	Scene3_Earth.SetScale(D3DXVECTOR3(200, 200, 200));	//*/

	Scene3_Player.Create("Models\\Test.dem", "Scene3_Player");	
	Scene3.AddModel(&Scene3_Player);
	Scene3_Player.SetPosition(D3DXVECTOR3(0, 5, 0));	
	//Scene3_Player.SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));	
	//Scene3_Player.SetRotation(D3DXVECTOR3(0, 90.0f, 0));

	/*for (int f=0; f<500; f++)
	{	
		Scene3_Fighter[f].Create("Models\\Fighter.dem", "Scene3_Fighter");	
		Scene3.AddModel(&Scene3_Fighter[f]);
		Scene3_Fighter[f].SetPosition(D3DXVECTOR3(DEUtility::GetRandomMinMax(1500, 3000), DEUtility::GetRandomMinMax(-1000, 1000), DEUtility::GetRandomMinMax(-1000, 1000)));	
		Scene3_Fighter[f].SetScale(D3DXVECTOR3(0.1f, 0.1f, 0.1f));	
		Scene3_Fighter[f].SetRotation(D3DXVECTOR3(0, 90, 0));	
	}//*/



	//**** Luci ***
	/*Luce
	Scene3_Light1.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light1");
	Scene3_Light1.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light1.SetPosition(D3DXVECTOR3(0,20,-50));
	Scene3_Light1.SetRange(2);
	Scene3_Light1.SetDiffuse(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	Scene3_Light1.SetFalloff(2.0f);
	Scene3_Light1.SetEnable(true);
	Scene3.AddLight(&Scene3_Light1);//*/

	/*Scene3_Light2.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light2");
	Scene3_Light2.SetType(DE_LIGHTTYPE_PERPIXELDIRECTIONAL);
	Scene3_Light2.SetDirection(D3DXVECTOR3(0.0f,100.0f,-200.0f));
	Scene3_Light2.SetDiffuse(D3DXCOLOR(1.0f,0.0f,0.0f,0.0f));
	Scene3_Light2.SetEnable(true);
	Scene3.AddLight(&Scene3_Light2);//*/

	/*Scene3_Light3.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light3");
	Scene3_Light3.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light3.SetPosition(D3DXVECTOR3(-10,2, 0));
	Scene3_Light3.SetRange(20);
	Scene3_Light3.SetDiffuse(D3DXCOLOR(0.0f,0.0f,0.6f,1.0f));
	Scene3_Light3.SetFalloff(10.0f);
	Scene3_Light3.SetEnable(true);
	Scene3.AddLight(&Scene3_Light3);

	Scene3_Light4.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light4");
	Scene3_Light4.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light4.SetPosition(D3DXVECTOR3(10,20,0));
	Scene3_Light4.SetRange(10);
	Scene3_Light4.SetDiffuse(D3DXCOLOR(0.0f,0.6f,0.0f,1.0f));
	Scene3_Light4.SetFalloff(10.0f);
	Scene3_Light4.SetEnable(true);
	Scene3.AddLight(&Scene3_Light4);

	Scene3_Light5.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light5");
	Scene3_Light5.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light5.SetPosition(D3DXVECTOR3(0,5,-20));
	Scene3_Light5.SetRange(10);
	Scene3_Light5.SetDiffuse(D3DXCOLOR(1.6f,0.0f,0.0f,1.0f));
	Scene3_Light5.SetFalloff(10.0f);
	Scene3_Light5.SetEnable(true);
	Scene3.AddLight(&Scene3_Light5);

	/*Scene3_Light6.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light6");
	Scene3_Light6.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light6.SetPosition(D3DXVECTOR3(0,30,0));
	Scene3_Light6.SetRange(10);
	Scene3_Light6.SetDiffuse(D3DXCOLOR(0.0f,1.0f,1.0f,1.0f));
	Scene3_Light6.SetFalloff(10.0f);
	Scene3_Light6.SetEnable(true);
	Scene3.AddLight(&Scene3_Light6);

	Scene3_Light7.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light7");
	Scene3_Light7.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light7.SetPosition(D3DXVECTOR3(10,2,0));
	Scene3_Light7.SetRange(10);
	Scene3_Light7.SetDiffuse(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));
	Scene3_Light7.SetFalloff(10.0f);
	Scene3_Light7.SetEnable(true);
	Scene3.AddLight(&Scene3_Light7);

	Scene3_Light8.Create("Textures\\Particles\\Particle7.bmp", "Scene3_Light8");
	Scene3_Light8.SetType(DE_LIGHTTYPE_PERPIXELPOINT);
	Scene3_Light8.SetPosition(D3DXVECTOR3(0,20,10));
	Scene3_Light8.SetRange(10);
	Scene3_Light8.SetDiffuse(D3DXCOLOR(1.0f,0.0f,1.0f,1.0f));
	Scene3_Light8.SetFalloff(10.0f);
	Scene3_Light8.SetEnable(true);
	Scene3.AddLight(&Scene3_Light8);


	/*Pannelli
	Scene3_Panel.Create("Scene3_Panel1");
	Scene3_Panel.SetTextureID( Scene3_Menu1.AddTexture("Textures\\Panels\\Indicator3.bmp", D3DXCOLOR(255,255,255,255), 1, 1, 0));
	Scene3_Panel.SetDimensions( D3DXVECTOR2(Scene3_Menu1.GetTextureInfoID(0, false).x , Scene3_Menu1.GetTextureInfoID(0, false).y)  );
	Scene3_Panel.SetMode(true);
	Scene3_Panel.SetTrasparency(true, 0.0f);
	Scene3.AddPanel(&Scene3_Panel);//*/

	//Font
	//Scene3_Font.CreateSprite("Verdana", 10, false, false, "Scene3_Font");

	/*Menus
	Scene3_Menu1.Create("Scene3_Menu1");
	Scene3_Menu1.SetTextureID( Scene3_Menu1.AddTexture("Textures\\Panels\\Menu\\Voice1.bmp", D3DXCOLOR(255,0,0,0), 1, 2, 0));
	Scene3_Menu1.SetDimensions( D3DXVECTOR2(Scene3_Menu1.GetTextureInfoID(0, false).x , Scene3_Menu1.GetTextureInfoID(0, false).y)  );
	Scene3_Menu1.SetMode(false);
	Scene3_Menu1.SetPosition(D3DXVECTOR2(20, 20));
	Scene3_Menu1.SetRotation(0.0f);
	Scene3_Menu1.SetTrasparency(true, 0.0f);
	Scene3.AddPanel(&Scene3_Menu1);
	
	Scene3_Menu2.Create("Scene3_Menu2");
	Scene3_Menu2.SetTextureID( Scene3_Menu2.AddTexture("Textures\\Panels\\Menu\\Voice2.bmp", D3DXCOLOR(255,0,0,0), 1, 2, 0));
	Scene3_Menu2.SetDimensions( D3DXVECTOR2(Scene3_Menu2.GetTextureInfoID(0, false).x , Scene3_Menu2.GetTextureInfoID(0, false).y)  );
	Scene3_Menu2.SetMode(false);
	Scene3_Menu2.SetPosition(D3DXVECTOR2(20, 120));
	Scene3_Menu2.SetRotation(0.0f);
	Scene3_Menu2.SetTrasparency(true, 0.0f);
	Scene3.AddPanel(&Scene3_Menu2);

	Scene3_Menu3.Create("Scene3_Menu3");
	Scene3_Menu3.SetTextureID( Scene3_Menu3.AddTexture("Textures\\Panels\\Menu\\Voice3.bmp", D3DXCOLOR(255,0,0,0), 1, 2, 0));
	Scene3_Menu3.SetDimensions( D3DXVECTOR2(Scene3_Menu3.GetTextureInfoID(0, false).x , Scene3_Menu3.GetTextureInfoID(0, false).y)  );
	Scene3_Menu3.SetMode(false);
	Scene3_Menu3.SetPosition(D3DXVECTOR2(20, 220));
	Scene3_Menu3.SetRotation(0.0f);
	Scene3_Menu3.SetTrasparency(true, 0.0f);
	Scene3.AddPanel(&Scene3_Menu3);

	Scene3_Menu4.Create("Scene3_Menu4");
	Scene3_Menu4.SetTextureID( Scene3_Menu4.AddTexture("Textures\\Panels\\Menu\\Voice4.bmp", D3DXCOLOR(255,0,0,0), 1, 2, 0));
	Scene3_Menu4.SetDimensions( D3DXVECTOR2(Scene3_Menu4.GetTextureInfoID(0, false).x , Scene3_Menu4.GetTextureInfoID(0, false).y)  );
	Scene3_Menu4.SetMode(false);
	Scene3_Menu4.SetPosition(D3DXVECTOR2(20, 320));
	Scene3_Menu4.SetRotation(0.0f);
	Scene3_Menu4.SetTrasparency(true, 0.0f);
	Scene3.AddPanel(&Scene3_Menu4);

	Scene3_Ode.Create("Scene3_Menu4");
	Scene3_Ode.SetTextureID( Scene3_Ode.AddTexture("Textures\\Panels\\ODE2.bmp", D3DXCOLOR(255,0,0,0), 1, 1, 0));
	Scene3_Ode.SetDimensions( D3DXVECTOR2(Scene3_Ode.GetTextureInfoID(0, false).x , Scene3_Ode.GetTextureInfoID(0, false).y)  );
	Scene3_Ode.SetMode(false);
	Scene3_Ode.SetPosition(D3DXVECTOR2(Scene3.GetWidth()-Scene3_Ode.GetTextureInfoID(0, false).x, Scene3.GetHeight()-Scene3_Ode.GetTextureInfoID(0, false).y));
	Scene3_Ode.SetRotation(0.0f);
	Scene3_Ode.SetTrasparency(false, 0.0f);
	Scene3.AddPanel(&Scene3_Ode);//*/

	return true;
}

bool Scene3_Input()
{
	//Muove la camera in base alla posizione del mouse
	if (Engine.GetDInput().Pressed_MouseUp(DE_MOUSEBUTTON_RIGHTBUTTON))Scene3_RightPressed =! Scene3_RightPressed;
	if (Scene3_RightPressed)
	{
		if (Scene3.GetCurrentCameraID()==0)
		{
			Scene3_Camera1.MoveCameraByMouse(Engine.GetDInput().GetCursorPosition());
			if(Engine.GetDInput().Pressed_Keyboard(DIK_D)) 	Scene3_Camera1.StrafeRight();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_A)) 	Scene3_Camera1.StrafeLeft();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_W))	Scene3_Camera1.MoveUp(true,true,true);
			if(Engine.GetDInput().Pressed_Keyboard(DIK_S))	Scene3_Camera1.MoveDown(true,true,true);
		}
		if (Scene3.GetCurrentCameraID()==1)
		{
			Scene3_Camera2.MoveCameraByMouse(Engine.GetDInput().GetCursorPosition());
			if(Engine.GetDInput().Pressed_Keyboard(DIK_D)) 	Scene3_Camera2.StrafeRight();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_A)) 	Scene3_Camera2.StrafeLeft();
			if(Engine.GetDInput().Pressed_Keyboard(DIK_W))	Scene3_Camera2.MoveUp(true,true,true);
			if(Engine.GetDInput().Pressed_Keyboard(DIK_S))	Scene3_Camera2.MoveDown(true,true,true);
		}
	}
	//if(Engine.GetDInput().Pressed_Keyboard(DIK_1))	Scene3.SetCurrentCamera(0);
	//if(Engine.GetDInput().Pressed_Keyboard(DIK_2))	Scene3.SetCurrentCamera(1);
	//if(Engine.GetDInput().Pressed_Keyboard(DIK_C,false))	Engine.SetCurrentScene(0,0,true);
		

	if(Engine.GetDInput().Pressed_Keyboard(DIK_X, false))
	{
		Scene3_SoundFX3.Play(true);
		FMOD_VECTOR pos3 = { 0.0f, 0.0f, -450.0f };
		FMOD_VECTOR vel3 = { 0.0f, 0.0f, 0.0f };
		Scene3_SoundFX3.Set3DAttributes(&pos3, &vel3);
		Scene3_SoundFX3.SetChannelGroup(Scene3_Sound);//*/
		Scene3_SoundFX3.SetPaused(false);
	}


	if(Engine.GetDInput().Pressed_Keyboard(DIK_C,false))
	{
		if (Scene3.GetCinematicEffect()) Scene3.SetCinematicEffect(false);
		else							 Scene3.SetCinematicEffect(true);
	}

	//Volumi
	float volumeVar;
	if(Engine.GetDInput().Pressed_Keyboard(DIK_N))
	{
		Scene3_Music.GetVolume(&volumeVar);
		Scene3_Music.SetVolume(volumeVar - 0.0025f);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_M))
	{
		Scene3_Music.GetVolume(&volumeVar);
		Scene3_Music.SetVolume(volumeVar + 0.0025f);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_H))
	{
		Scene3_Sound.GetVolume(&volumeVar);
		Scene3_Sound.SetVolume(volumeVar - 0.0025f);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_J))
	{
		Scene3_Sound.GetVolume(&volumeVar);
		Scene3_Sound.SetVolume(volumeVar + 0.0025f);
	}


	if(Engine.GetDInput().Pressed_Keyboard(DIK_I, false))
	{
		switch (Scene3_Debug.Debug_ShowInfo)
		{
			case false:
			{
				Scene3_Debug.Debug_ShowInfo = true;
				Scene3.SetDebugProperties(Scene3_Debug);
			}
			break;
			case true:
			{
				Scene3_Debug.Debug_ShowInfo = false;
				Scene3.SetDebugProperties(Scene3_Debug);
			}
			break;
		}
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_F, false))
	{
		switch (Scene3_Debug.Debug_BoundingType)
		{
			case -1:
			{
				Scene3_Debug.Debug_BoundingType = 0;
				Scene3.SetDebugProperties(Scene3_Debug);
			}
			break;
			case 0:
			{
				Scene3_Debug.Debug_BoundingType = 1;
				Scene3.SetDebugProperties(Scene3_Debug);
			}
			break;
			case 1:
			{
				Scene3_Debug.Debug_BoundingType = 2;
				Scene3.SetDebugProperties(Scene3_Debug);
			}
			break;
			case 2:
			{
				Scene3_Debug.Debug_BoundingType = -1;
				Scene3.SetDebugProperties(Scene3_Debug);
			}
			break;
		}
	}



	//Settaggi PostProcessing
	if(Engine.GetDInput().Pressed_Keyboard(DIK_P, false))
	{
		Scene3_currentPostprocessEffect = Scene3_currentPostprocessEffect + 1;
		if (Scene3_currentPostprocessEffect >= 20) Scene3_currentPostprocessEffect=0;
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_O, false))
	{
		Scene3_currentPostprocessEffect = Scene3_currentPostprocessEffect - 1;
		if (Scene3_currentPostprocessEffect == -1) Scene3_currentPostprocessEffect=19;
	}
	switch (Scene3_currentPostprocessEffect)
	{
		case 0:
		{
			Scene3.SetPostProcessingEnable(false);
			Scene3.SetPostProcessingTechnique("None");
		}
		break;
		case 1:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBlackAndWhite");
		}
		break;
		case 2:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBlackAndWhite2");
		}
		break;
		case 3:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueInverse");
		}
		break;
		case 4:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueSepia");
		}
		break;
		case 5:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBlur");
		}
		break;
		case 6:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBlur2");
		}
		break;
		case 7:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueTonemap");
		}
		break;
		case 8:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBloom");
		}
		break;
		case 9:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueGlass");
		}
		break;
		case 10:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueEdgeDetection");
		}
		break;
		case 11:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueEmboss");
		}
		break;
		case 12:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueSharpen");
		}
		break;
		case 13:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueRed");
		}
		break;
		case 14:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueGreen");
		}
		break;
		case 15:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBlue");
		}
		break;
		case 16:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueFade");
			Scene3.GetPostProcessing()->SetFloatArray("FadeValue", Scene3_FadeColor, 3);
		}
		break;
		case 17:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueBright");
		}
		break;
		case 18:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueDark");
		}
		break;
		case 19:
		{
			Scene3.SetPostProcessingEnable(true);
			Scene3.SetPostProcessingTechnique("TechniqueUnderwater");
		}
		break;
	}//*/


	/*REMOVE MODELS (DEBUG)
	if(Engine.GetDInput().Pressed_Keyboard(DIK_K, false))
	{
		Scene3.RemoveModel(&Scene3_Earth);
		Scene3_Earth.Release();
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_J, false))
	{
		Scene3.RemoveModel(&Scene3_Mars);
		Scene3_Mars.Release();
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_H, false))
	{
		Scene3.RemoveModel(&Scene3_Base);
		Scene3_Base.Release();
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_G, false))
	{
		for (int f=0; f<500; f++)
		{
			Scene3.RemoveModel(&Scene3_Fighter[f]);	
			Scene3_Fighter[f].Release();
		}
	}*/


	float forceStrength = 5000.0f; 
	NxForceMode forceMode = NX_IMPULSE ;
	if(Engine.GetDInput().Pressed_Keyboard(DIK_K, true))
	{
		Scene3_Player.ApplyForce(D3DXVECTOR3(1,0,0), forceStrength, forceMode);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_H, true))
	{
		Scene3_Player.ApplyForce(D3DXVECTOR3(-1,0,0), forceStrength, forceMode);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_U, true))
	{
		Scene3_Player.ApplyForce(D3DXVECTOR3(0,0,1), forceStrength, forceMode);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_J, true))
	{
		Scene3_Player.ApplyForce(D3DXVECTOR3(0,0,-1), forceStrength, forceMode);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_Y, true))
	{
		Scene3_Player.ApplyForce(D3DXVECTOR3(0,1,0), forceStrength, forceMode);
	}
	if(Engine.GetDInput().Pressed_Keyboard(DIK_I, true))
	{
		Scene3_Player.ApplyForce(D3DXVECTOR3(0,-1,0), forceStrength, forceMode);
	}


	return true;
}

bool Scene3_Render3D()
{

	/*if (Scene3_Player.CheckCollision(NX_NOTIFY_ON_TOUCH))
	{
		Scene3_currentPostprocessEffect = 13;
	}
	else
	{
		Scene3_currentPostprocessEffect	= 0;
	}*/

	/*Effetto Fade-In
	if (Scene3_FadeColor[0] < 1.0f)
	{
		Scene3_currentPostprocessEffect = 16;
		Scene3_FadeColor[0] = Scene3_FadeColor[0] + 0.003f;
		Scene3_FadeColor[1] = Scene3_FadeColor[1] + 0.003f;
		Scene3_FadeColor[2] = Scene3_FadeColor[2] + 0.003f;
		Scene3.GetPostProcessing()->SetFloatArray("FadeValue", Scene3_FadeColor, 3);
	}*/



	//Posizionamento luce dinamica
	static D3DXVECTOR4 gLightPos = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	const float kMinX = -15.5f;
	const float kMinY = 20.0f;
	const float kMaxX = 15.5f;
	const float kMaxY = 20.0f;
	const float kInc = 0.1f;
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
	Scene3_Light1.SetPosition(D3DXVECTOR3(gLightPos.x,gLightPos.y,gLightPos.z));//*/




	//Scene3_Panel.SetPosition(D3DXVECTOR2(Scene3_Mars.GetModelInfo().Info_CenterCoord2D.x, Scene3_Mars.GetModelInfo().Info_CenterCoord2D.y));

	/*if (Engine.engineInput.Pressed_MouseDown(DE_MOUSEBUTTON_LEFTBUTTON))
	{
		//Dimensioni video
		D3DVIEWPORT9 currentViewport;
		Engine.GetD3DDevice()->GetViewport(&currentViewport);
		float engineWidth = (float)currentViewport.Width;
		float engineHeight = (float)currentViewport.Height;
		D3DXVECTOR3 vPickRayDir;
		D3DXVECTOR3 vPickRayOrig;
		D3DXMATRIX m_matProj;
		Engine.GetD3DDevice()->GetTransform(D3DTS_PROJECTION,&m_matProj);
		D3DXVECTOR3 v;
		v.x =  ( ( ( 2.0f * (engineWidth/2.0f) ) / engineWidth  ) - 1 ) / m_matProj._11;
		v.y = -( ( ( 2.0f * (engineHeight/2.0f) ) / engineHeight  ) - 1 ) / m_matProj._22;
		v.z =  1.0f;
		D3DXMATRIX m;
		D3DXMATRIX m_matView;
		Engine.GetD3DDevice()->GetTransform(D3DTS_VIEW,&m_matView);
		D3DXMatrixInverse( &m, NULL, &m_matView );
		vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		Scene3_Cubes[0].Create("Models\\Fire.dem", "Scene3_Cube");	
		Scene3.AddModel(&Scene3_Cubes[0]);
		Scene3_Cubes[0].SetPosition(Scene3_Camera1.GetPosition());	
		Scene3_Cubes[0].SetRotation(D3DXVECTOR3(0, 0, 0));
		Scene3_Cubes[0].SetLinearVelocity(D3DXVECTOR3(vPickRayDir.x*10, vPickRayDir.y*10, vPickRayDir.z * 10));
	}*/

	/*Piking
	if (Engine.engineInput.Pressed_MouseDown(DE_MOUSEBUTTON_LEFTBUTTON))
	{
		for (int n=0; n<1000; n++)
		{
			if (Scene3_Asteroid[n].CheckPicked(Engine.engineInput.GetCursorPosition(), true))
			{
				Scene3_currentPiked = n;
				Scene3.SetCursorTextureID(1);
				break;
			}
			else
			{
				Scene3_currentPiked = -1;
				Scene3.SetCursorTextureID(0);
			}
		}
	}
	else
	{
		Scene3_currentPiked = -1;
		Scene3.SetCursorTextureID(0);
	}*/


	return true;
}

bool Scene3_Render2D()
{
	/*Scene3_Font.BeginTextSprite();
	//sprintf(Scene3_BufferInfo, "Dist: %.2f", (float)Scene3_Model1.GetModelInfo().Info_DistanceFromCamera);
	//Scene3_Font.DrawSprite(Scene3_BufferInfo, 15, 520, D3DCOLOR_XRGB(0, 255, 0));
	if (Scene3_currentPiked >= 0)	sprintf(Scene3_BufferInfo, "Selected Model: %d", Scene3_currentPiked);
	else							sprintf(Scene3_BufferInfo, "Selected Model: None");	
	Scene3_Font.DrawSprite(Scene3_BufferInfo, 15, 500, D3DCOLOR_XRGB(0, 255, 0));
	sprintf(Scene3_BufferInfo, "Scene verticies: %d/%d", Scene3.GetRuntimeInfo().Scene_RuntimeInfo_TotalVerticesVisible, Scene3.GetRuntimeInfo().Scene_RuntimeInfo_TotalVertices);	
	Scene3_Font.DrawSprite(Scene3_BufferInfo, 15, 480, D3DCOLOR_XRGB(0, 255, 0));
	//sprintf(Scene3_BufferInfo, "Vertices: %d/%d", (int)Scene3.GetRuntimeInfo().Scene_RuntimeInfo_TotalVerticesVisible, (int)Scene3.GetRuntimeInfo().Scene_RuntimeInfo_TotalVertices);
	//Scene3_Font.DrawSprite(Scene3_BufferInfo, 15, 540, D3DCOLOR_XRGB(0, 255, 0));
	Scene3_Font.EndTextSprite();//*/

	/*Menus
	if (Scene3_Menu1.CheckPicked(Engine.engineInput.GetCursorPosition()))	Scene3_Menu1.GoToFrame(1);
	else																	Scene3_Menu1.GoToFrame(0);
	if (Scene3_Menu2.CheckPicked(Engine.engineInput.GetCursorPosition()))	Scene3_Menu2.GoToFrame(1);
	else																	Scene3_Menu2.GoToFrame(0);
	if (Scene3_Menu3.CheckPicked(Engine.engineInput.GetCursorPosition()))	Scene3_Menu3.GoToFrame(1);
	else																	Scene3_Menu3.GoToFrame(0);
	if (Scene3_Menu4.CheckPicked(Engine.engineInput.GetCursorPosition()))
	{
		Scene3_Menu4.GoToFrame(1);
		if (Engine.engineInput.Pressed_MouseDown(DE_MOUSEBUTTON_LEFTBUTTON))
		{
			Engine.Exit();
		}
	}
	else
	{
		Scene3_Menu4.GoToFrame(0);
	}//*/

	return true;
}

bool Scene3_Invalidate()
{
	Scene3_Font.Invalidate();
	return true;
}

bool Scene3_Restore()
{
	Scene3_Font.Restore();
	return true;
}

bool Scene3_Release()
{
	Scene3_Font.Release();
	return true;
}