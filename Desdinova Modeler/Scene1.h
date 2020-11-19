#ifndef H_SCENE1_H
#define H_SCENE1_H

//Scena 1
class DEScene1: public DEScene
{
	public:
		//** OGGETTI **
		DECamera		Camera;
		DEFont			FontInfo;

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