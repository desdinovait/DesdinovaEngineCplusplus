#ifndef H_SCENE0_H
#define H_SCENE0_H

//Scena 1
class DEScene1: public DEScene
{
	public:
		//** OGGETTI **
		DECamera		Camera;
		DEAxes			SceneAxes;
		DEGrid			Grid;
		DEPanel			Panel;
		DEFont			FontInfo;
		DEFont			FontHit;
		DELight			Light;
		DEVideo			Video;
		//Oggetti principali
		DEParticleSystem ParticleSystems;
		DEEntity		 Entity;
		DEModel			 Model;

		//** VARIABILI **
		//Figlio corrente
		int currentCHILDID;
		//Flags apertura
		int	currentCreationMode;	//0=None, 1=Model, 2=Entity, 3=Particles
		bool opened;
		//Mouse
		bool rightPressed;
		bool leftPressed;
		//Altro
		bool showInfo;
		bool showGrid;
		bool showAxes;
		//Proprietà visualizzazione
		STRUCT_DE_RENDERPROPERTIES renderProperties;
		//Finistre attive
		bool showEntitiesManager;
		bool showModelsManager;
		bool showParticlesManager;
		bool showLightsManager;
		bool showCameraControl;
		//Luci
		unsigned int totalLights;
		unsigned int currentLight;

		//** FUNZIONI **
		bool		Load();
		bool		Update();
		bool		Execute3D();
		bool		Execute2D();
		bool		Input();
		bool		Release();
		bool		Restore();
		bool		Invalidate();
};

#endif