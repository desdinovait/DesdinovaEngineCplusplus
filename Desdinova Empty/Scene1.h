#ifndef H_SCENE1_H
#define H_SCENE1_H

//Scena 1
DEScene								Scene1;
STRUCT_DE_SCENE						Scene1_Properties;
STRUCT_DE_GENERAL					Scene1_General;
STRUCT_DE_CLEAR						Scene1_Clear;
STRUCT_DE_DEBUG						Scene1_Debug;
STRUCT_DE_FOG						Scene1_Fog;
STRUCT_DE_PERSPECTIVE				Scene1_Perspective;
STRUCT_DE_GRIDS						Scene1_Grids;
STRUCT_DE_SCENE_PHYSICSPROPERTIES	Scene1_Physics;
DECamera							Scene1_Camera;
DESoundChannelGroup					Scene1_MusicGroup;
DESoundChannelGroup					Scene1_SoundGroup;
DESound								Scene1_SoundBackground;
DESound								Scene1_SoundFX1;
DESound								Scene1_SoundFX2;
DESky								Scene1_Sky;
DELight								Scene1_Light1;
DELight								Scene1_Light2;
DELight								Scene1_Light3;
DELight								Scene1_Light4;
DELight								Scene1_Light5;
DEModel								Scene1_Planet[10];
DEModel								Scene1_Cube[100];
DEModel								Scene1_Satiro[10];
DEPanel								Scene1_Panel1;
DEPanel								Scene1_Panel2;
bool								Scene1_RightPressed;
DELensFlare							Scene1_LensFlare1;
DEParticleSystem					Scene1_Particle1;
DEParticleSystem					Scene1_Particle2;
DEFont								Scene1_Font1;
DEFont								Scene1_Font2;
DEFont								Scene1_Font3;

bool Scene1_Load();
bool Scene1_Input();
bool Scene1_Render3D();
bool Scene1_Render2D();
bool Scene1_Invalidate();
bool Scene1_Restore();
bool Scene1_Release();

#endif