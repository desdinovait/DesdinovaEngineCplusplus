#include "General.h"
#include "Scene2.h"

///**********************************
///*********** SCENEA 2 *************
///**********************************
bool DEScene2::Load()
{
	this->Camera.Create(D3DXVECTOR3(0,0,250), D3DXVECTOR3(0,-70,0), D3DXVECTOR3(0,1,0), 0, 1.0f, "Camera");
	this->Camera.SetHWND(Engine.GetCurrentSwapChain().hWnd);
	this->Camera.SetWindowed(Engine.GetCurrentSwapChain().presentParameters.Windowed);
	
	this->FontInfo.CreateSprite("Verdana", 9, false, false, "FontInfo");

	this->SkyCylinder.Create("Textures\\Skies\\Cylinder\\03\\Top.dds", "Textures\\Skies\\Cylinder\\03\\Top.dds", "Textures\\Skies\\Cylinder\\03\\Wide.dds", 30.0f, 20.0f, 8, "SkyCylinder");


	//Flare
	this->Flare.AddFlare("Textures\\Flares\\flare4.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.3f);
	this->Flare.AddFlare("Textures\\Flares\\flare5.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.5f);
	this->Flare.AddFlare("Textures\\Flares\\flare3.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.0f);
	this->Flare.AddFlare("Textures\\Flares\\flare2.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.4f);
	this->Flare.AddFlare("Textures\\Flares\\flare5.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.2f);
	this->Flare.AddFlare("Textures\\Flares\\flare3.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, -0.1f);
	this->Flare.AddFlare("Textures\\Flares\\flare6.dds", D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.6f);
	this->Flare.AddFlare("Textures\\Flares\\esa.dds",    D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 1.1f);
	this->Flare.AddFlare("Textures\\Flares\\esa.dds",    D3DCOLOR_ARGB(64,204,205,236), D3DCOLOR_XRGB(0,0,0), 1.0f, 0.4f);
	this->Flare.AddFlash("Textures\\Flares\\Flash.dds", D3DCOLOR_ARGB(128,204,205,236), D3DCOLOR_XRGB(0,0,0));
	this->Flare.Create(Scene2.GetFOV(), 0.01f, false, "Flare"); 
	//*/

	return true;
}

bool DEScene2::Input()
{
	//** INPUT MOUSE **
	if (Engine.engineInput.Pressed_Mouse(DE_MOUSEBUTTON_RIGHTBUTTON))	this->rightPressed =! this->rightPressed;

	//** MOVIMENTO CAMERA **
	if (this->rightPressed)
	{
		this->Camera.MoveCameraByMouse(Engine.engineInput.GetCursorPosition());

		//Tastiera
		if(Engine.engineInput.Pressed_Keyboard(DIK_D)) 	this->Camera.StrafeRight();
		if(Engine.engineInput.Pressed_Keyboard(DIK_A)) 	this->Camera.StrafeLeft();
		if(Engine.engineInput.Pressed_Keyboard(DIK_W))	this->Camera.MoveUp(true,true,true);
		if(Engine.engineInput.Pressed_Keyboard(DIK_S))	this->Camera.MoveDown(true,true,true);				
	}

	//Update della camera
	this->Camera.UpdateInput();

	return true;
}

bool DEScene2::Update()
{
	return true;
}

bool DEScene2::Execute3D()
{
	//Update della camera
	this->Camera.UpdateView();

	//Skybox
	this->SkyCylinder.SetPosition(Camera.GetPosition());
	this->SkyCylinder.Render();

	//Lens flare 3D
	D3DXVECTOR3 lensPos = this->Camera.GetPosition();
	lensPos.x = lensPos.x - 220;	lensPos.y = lensPos.y + 50;	lensPos.z = lensPos.z - 900;
	this->Flare.CalculatePosition(this->Camera.GetView());//*/
	this->Flare.Render3D();


	return true;
}

bool DEScene2::Execute2D()
{
	this->FontInfo.BeginTextSprite();
	if (Engine.GetCurrentSwapChainID() == Engine.GetActiveSwapChainID())
		this->FontInfo.DrawSprite("Status: Active", 2,18,D3DCOLOR_XRGB(255, 0, 0));
	else
		this->FontInfo.DrawSprite("Status: Inactive", 2,18,D3DCOLOR_XRGB(0, 255, 0));
	this->FontInfo.EndTextSprite();				
	
	//Lens Flare 2D
	this->Flare.Render2D();

	return true;
}

bool DEScene2::Release()
{
	this->Camera.Release();
	this->FontInfo.Release();
	this->SkyCylinder.Release();
	this->Flare.Release();
	return true;
}

bool DEScene2::Invalidate()
{
	this->Camera.Invalidate();
	this->FontInfo.Invalidate();
	this->SkyCylinder.Invalidate();
	this->Flare.Invalidate();
	return true;
}

bool DEScene2::Restore()
{
	this->Camera.Restore();
	this->FontInfo.Restore();
	this->SkyCylinder.Restore();
	this->Flare.Restore();
	return true;
}
