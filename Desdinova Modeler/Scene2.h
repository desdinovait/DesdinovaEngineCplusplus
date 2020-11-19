#ifndef H_SCENE2_H
#define H_SCENE2_H

//Scena 2
class DEScene2: public DEScene
{
	public:
		//** OGGETTI **
		DECamera		Camera;
		DEFont			FontInfo;
		DELensFlare		Flare;
		DESkyCylinder	SkyCylinder;
		
		//** VARIABILI **
		bool			rightPressed;
		
		//** FUNZIONI **
		bool			Load();
		bool			Update();
		bool			Execute3D();
		bool			Execute2D();
		bool			Input();
		bool			Release();
		bool			Restore();
		bool			Invalidate();
};

#endif