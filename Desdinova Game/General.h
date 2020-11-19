#ifndef H_GENERAL_H
#define H_GENERAL_H

//** INCLUDES DESDINOVA **
#include "D:\\Desdinova Project\\Desdinova\\Desdinova Engine\\Desdinova Engine.h"
#pragma comment(lib, "D:\\Desdinova Project\\Desdinova\\Desdinova Engine\\Desdinova Engine.lib")
#include "Resource.h"

//** INSTANZE DI CLASSE **
//Motore grafico
DECore						Engine;

//Scena 1
STRUCT_DE_SCENE				Scene1_Properties;
STRUCT_DE_CLEAR				Scene1_Clear;
STRUCT_DE_DEBUG				Scene1_Debug;
STRUCT_DE_FOG				Scene1_Fog;
STRUCT_DE_PERSPECTIVE		Scene1_Perspective;
STRUCT_DE_GRIDS				Scene1_Grids;
STRUCT_DE_SCENE_PHYSICSPROPERTIES Scene1_Physics;
DEScene						Scene1;
DECamera					Scene1_Camera;
bool						Scene1_RightPressed;
DEFont						Scene1_Font;
char						Scene1_BufferInfo[50];
DESkyCylinder				Scene1_SkyCylinder;
DEParticleSystem			Scene1_Particle1;
DELight						Scene1_Light1;
DEModel						Scene1_Logo;
DEModel						Scene1_Ball;
DEModel						Scene1_Player1;
DEModel						Scene1_Player2;
DEModel						Scene1_Plane;
DEModel						Scene1_Edge1;
DEModel						Scene1_Edge2;
int							Scene1_CurrentPlayer;
int							Scene1_Player1_Score;
int							Scene1_Player2_Score;


//Scena 2
DEScene						Scene2;
STRUCT_DE_SCENE				Scene2_Properties;
STRUCT_DE_CLEAR				Scene2_Clear;
STRUCT_DE_DEBUG				Scene2_Debug;
STRUCT_DE_FOG				Scene2_Fog;
STRUCT_DE_PERSPECTIVE		Scene2_Perspective;
STRUCT_DE_GRIDS				Scene2_Grids;
STRUCT_DE_SCENE_PHYSICSPROPERTIES Scene2_Physics;
bool						Scene2_LeftPressed;
DECamera					Scene2_Camera;
DESoundChannelGroup			Scene2_Music;
DESound						Scene2_SoundBackground;

DEPanel						Scene2_Panel1;
DEPanel						Scene2_Panel2;
DEPanel						Scene2_Panel3;
DEFont						Scene2_Font1;
DEFont						Scene2_Font2;
DEFont						Scene2_Font3;
DEFont						Scene2_Font4;
DEFont						Scene2_Font5;
DEFont						Scene2_Font6;
int cur1=0;
int cur2=0;
int cur3=0;
int cur4=0;
int cur5=0;
int cur6=0;


//Scena 3
DEScene						Scene3;
STRUCT_DE_SCENE				Scene3_Properties;
STRUCT_DE_CLEAR				Scene3_Clear;
STRUCT_DE_DEBUG				Scene3_Debug;
STRUCT_DE_FOG				Scene3_Fog;
STRUCT_DE_PERSPECTIVE		Scene3_Perspective;
STRUCT_DE_GRIDS				Scene3_Grids;
STRUCT_DE_SCENE_PHYSICSPROPERTIES Scene3_Physics;
unsigned short				Scene3_LoadCounter;
unsigned short				Scene3_ReleaseCounter;
DESkyCylinder				Scene3_SkyCylinder;
DESkyBox					Scene3_SkyBox;
DELensFlare					Scene3_LensFlare1;
DELensFlare					Scene3_LensFlare2;
DECamera					Scene3_Camera1;
DECamera					Scene3_Camera2;
DEParticleSystem			Scene3_Particle1;
DEParticleSystem			Scene3_Particle2;
DEModel						Scene3_Asteroid[1000];
DEModel						Scene3_Cubes[200];
DEModel						Scene3_Earth;
DEModel						Scene3_Moon;
DEModel						Scene3_Mars;
DEModel						Scene3_Fighter[1000];
DEModel						Scene3_Player;
DEPanel						Scene3_Panel;
DEFont						Scene3_Font;
bool						Scene3_RightPressed;
char						Scene3_BufferInfo[50];
bool						Scene3_ModelPiked;
//Luci
DELight						Scene3_Light1;
DELight						Scene3_Light2;
DELight						Scene3_Light3;
DELight						Scene3_Light4;
DELight						Scene3_Light5;
DELight						Scene3_Light6;
DELight						Scene3_Light7;
DELight						Scene3_Light8;

//Suoni
DESoundChannelGroup			Scene3_Sound;
DESoundChannelGroup			Scene3_Music;
DESound						Scene3_SoundFX1;
DESound						Scene3_SoundFX2;
DESound						Scene3_SoundFX3;
DESound						Scene3_SoundBackground;

DEPanel						Scene3_Menu1;
DEPanel						Scene3_Menu2;
DEPanel						Scene3_Menu3;
DEPanel						Scene3_Menu4;
DEPanel						Scene3_Ode;

unsigned int				Scene3_currentPostprocessEffect=0;
int							Scene3_currentPiked=-1;
float						Scene3_FadeColor[3] = { 1.0f, 1.0f, 1.0f };

#endif

