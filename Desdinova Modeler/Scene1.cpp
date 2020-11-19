#include "General.h"
#include "Scene1.h"

///**********************************
///*********** SCENEA 1 *************
///**********************************

bool DEScene1::Load()
{
	this->Camera.Create(D3DXVECTOR3(0,0,250), D3DXVECTOR3(0,-70,0), D3DXVECTOR3(0,1,0), 0, 1.0f, "Camera");
	this->Camera.SetHWND(Engine.GetCurrentSwapChain().hWnd);
	this->Camera.SetWindowed(Engine.GetCurrentSwapChain().presentParameters.Windowed);

	this->FontInfo.CreateSprite("Verdana", 9, false, false, "FontInfo");

	return true;
}

bool DEScene1::Input()
{
	//Update della camera
	this->Camera.UpdateInput();
	return true;
}

bool DEScene1::Update()
{
	return true;
}

bool DEScene1::Execute3D()
{
	//Update della camera
	this->Camera.UpdateView();
	
	return true;
}

bool DEScene1::Execute2D()
{
	this->FontInfo.BeginTextSprite();
	if (Engine.GetCurrentSwapChainID() == Engine.GetActiveSwapChainID())
		this->FontInfo.DrawSprite("Status: Active", 2,18,D3DCOLOR_XRGB(255, 0, 0));
	else
		this->FontInfo.DrawSprite("Status: Inactive", 2,18,D3DCOLOR_XRGB(0, 255, 0));
	this->FontInfo.EndTextSprite();

	return true;
}


bool DEScene1::Release()
{
	this->Camera.Release();
	this->FontInfo.Release();
	return true;
}

bool DEScene1::Invalidate()
{
	this->Camera.Invalidate();
	this->FontInfo.Invalidate();
	return true;
}

bool DEScene1::Restore()
{
	this->Camera.Restore();
	this->FontInfo.Restore();
	return true;
}
