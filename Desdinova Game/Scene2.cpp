#include "General.h"
#include "Scene2.h"

bool Scene2_Load()
{
	//Sound
	Scene2_Music.Create("Scene2_MusicGroup");
	Scene2.AddSoundGroup(&Scene2_Music);//*/
	Scene2_SoundBackground.Create("Music/Monstro.mp3", true, false, FMOD_HARDWARE | FMOD_2D);
	Scene2_SoundBackground.SetMode(FMOD_LOOP_NORMAL);
	Scene2_SoundBackground.Play();
	Scene2_SoundBackground.SetChannelGroup(Scene2_Music);
	Scene2.AddSound(&Scene2_SoundBackground);//*/

	//Camere
	Scene2_Camera.Create(D3DXVECTOR3(-45,45,-45), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0), 0, 5.0f, true, "Scene2_Camera");
	Scene2.AddCamera(&Scene2_Camera);
	Scene2.SetCurrentCamera(0);

	//PostProcessing
	Scene2.SetPostProcessingInclude2D(false);
	Scene2.SetPostProcessingEnable(true);
	Scene2.SetPostProcessingTechnique("TechniqueBlur");

	//Font
	Scene2_Font1.CreateSprite("Czechosl", 20, true, false, "Scene2_Font1");
	Scene2_Font2.CreateSprite("Czechosl", 20, true, false, "Scene2_Font2");
	Scene2_Font3.CreateSprite("Czechosl", 20, true, false, "Scene2_Font3");
	Scene2_Font4.CreateSprite("Czechosl", 20, true, false, "Scene2_Font4");
	Scene2_Font5.CreateSprite("Czechosl", 20, true, false, "Scene2_Font5");
	Scene2_Font6.CreateSprite("Czechosl", 20, true, false, "Scene2_Font6");

	//Pannelli
	Scene2_Panel1.Create("Scene2_Panel1");
	Scene2_Panel1.SetTextureID( Scene2_Panel1.AddTexture("Textures\\Panels\\Panel_1.bmp", D3DXCOLOR(255,323,0,255), 1, 1, 0));
	Scene2_Panel1.SetDimensions( D3DXVECTOR2(Scene2_Panel1.GetTextureInfoID(0, false).x , Scene2_Panel1.GetTextureInfoID(0, false).y)  );
	Scene2_Panel1.SetMode(true);
	Scene2_Panel1.SetTrasparency(false, 0.5f);
	Scene2.AddPanel(&Scene2_Panel1);//*/

	Scene2_Panel2.Create("Scene2_Panel2");
	Scene2_Panel2.SetTextureID( Scene2_Panel2.AddTexture("Textures\\Panels\\Panel_2.bmp", D3DXCOLOR(0,0,0,0), 1, 1, 0));
	Scene2_Panel2.SetDimensions( D3DXVECTOR2(Scene2_Panel2.GetTextureInfoID(0, false).x , Scene2_Panel1.GetTextureInfoID(0, false).y)  );
	Scene2_Panel2.SetMode(true);
	Scene2_Panel2.SetTrasparency(false, 0.5f);
	Scene2.AddPanel(&Scene2_Panel2);//*/

	Scene2_Panel3.Create("Scene2_Panel3");
	Scene2_Panel3.SetTextureID( Scene2_Panel3.AddTexture("Textures\\Panels\\Panel_3.bmp", D3DXCOLOR(0,0,0,0), 1, 1, 0));
	Scene2_Panel3.SetDimensions( D3DXVECTOR2(Scene2_Panel3.GetTextureInfoID(0, false).x , Scene2_Panel3.GetTextureInfoID(0, false).y)  );
	Scene2_Panel3.SetMode(true);
	Scene2_Panel3.SetTrasparency(false, 0.5f);
	Scene2.AddPanel(&Scene2_Panel3);//*/

	Scene2_Panel1.SetRenderMe(false);
	Scene2_Panel2.SetRenderMe(false);
	Scene2_Panel3.SetRenderMe(false);

	return true;
}

bool Scene2_Input()
{
	if (Engine.GetDInput().Pressed_MouseDown(DE_MOUSEBUTTON_LEFTBUTTON))Scene2_LeftPressed =! Scene2_LeftPressed;
	if (Scene2_LeftPressed)
	{
		Scene2_Camera.MoveCameraByMouse(Engine.GetDInput().GetCursorPosition());
		if(Engine.GetDInput().Pressed_Keyboard(DIK_D)) 	Scene2_Camera.StrafeRight();
		if(Engine.GetDInput().Pressed_Keyboard(DIK_A)) 	Scene2_Camera.StrafeLeft();
		if(Engine.GetDInput().Pressed_Keyboard(DIK_W))	Scene2_Camera.MoveUp(true,true,true);
		if(Engine.GetDInput().Pressed_Keyboard(DIK_S))	Scene2_Camera.MoveDown(true,true,true);
	}

	return true;
}

bool Scene2_Render3D()
{
	return true;
}

bool Scene2_Render2D()
{
	D3DCOLOR testoColor = D3DXCOLOR(255,255,255,255);
	int testoX = 10;
	int testoY = 500;
	float testoVelocity = 0.0075f;
	LPSTR testo1 = "Daniele 'Duff' Ferla";
	LPSTR testo2 = "presents                    ";
	LPSTR testo3 = "in association with";
	LPSTR testo4 = "Universita' degli Studi di Milano - Sede di Crema                    ";
	LPSTR testo5 = "accademy correlator";
	LPSTR testo6 = "prof. Stefano Ferrari                    ";

	LPSTR testo7 = "3D models";
	LPSTR testo8 = "Antonio 'Ghost999' Mondonico                    ";
	LPSTR testo9 = "load tester";
	LPSTR testo10 = "Dario 'Macman' Arduini                    ";


	if (cur2<strlen(testo2))
	{
		LPSTR testoFinale1 = Scene2_Font1.GenerateTypedText(testo1, testoVelocity, false, cur1);
		Scene2_Font1.BeginTextSprite();
		Scene2_Font1.DrawSprite(testoFinale1, testoX, testoY, testoColor);
		Scene2_Font1.EndTextSprite();

		if (cur1>=strlen(testo1))
		{
			LPSTR testoFinale2 = Scene2_Font2.GenerateTypedText(testo2, testoVelocity, false, cur2);
			Scene2_Font2.BeginTextSprite();
			Scene2_Font2.DrawSprite(testoFinale2, testoX, testoY + 25, testoColor);
			Scene2_Font2.EndTextSprite();
		}
	}
	else
	{
		if (cur4<strlen(testo4))
		{
			LPSTR testoFinale3 = Scene2_Font3.GenerateTypedText(testo3, testoVelocity, false, cur3);
			Scene2_Font3.BeginTextSprite();
			Scene2_Font3.DrawSprite(testoFinale3, testoX, testoY, testoColor);
			Scene2_Font3.EndTextSprite();
			if (cur3>=strlen(testo3))
			{
				LPSTR testoFinale4 = Scene2_Font4.GenerateTypedText(testo4, testoVelocity, false, cur4);
				Scene2_Font4.BeginTextSprite();
				Scene2_Font4.DrawSprite(testoFinale4, testoX, testoY + 25, testoColor);
				Scene2_Font4.EndTextSprite();
			}
		}
		else
		{
			LPSTR testoFinale5 = Scene2_Font5.GenerateTypedText(testo5, testoVelocity, false, cur5);
			Scene2_Font5.BeginTextSprite();
			Scene2_Font5.DrawSprite(testoFinale5, testoX, testoY, testoColor);
			Scene2_Font5.EndTextSprite();

			if (cur5>=strlen(testo5))
			{
				LPSTR testoFinale6 = Scene2_Font6.GenerateTypedText(testo6, testoVelocity, false, cur6);
				Scene2_Font6.BeginTextSprite();
				Scene2_Font6.DrawSprite(testoFinale6, testoX, testoY + 25, testoColor);
				Scene2_Font6.EndTextSprite();
			}
			//Scene2_Panel1.SetRenderMe(true);
		}
	}



	/**PROVA SENZA IL CASINO DEGLI IF..Da finire
	//Scrive
	Scene2_Font1.BeginTextSprite();
	Scene2_Font1.DrawSprite(testoFinale1, testoX, testoY, testoColor);
	Scene2_Font1.EndTextSprite();
	if (cur1 >= strlen(testo1))
	{
		Scene2_Font2.BeginTextSprite();
		Scene2_Font2.DrawSprite(testoFinale2, testoX, testoY + 25, testoColor);
		Scene2_Font2.EndTextSprite();
	}


	//*/


	return true;
}

bool Scene2_Invalidate()
{
	return true;
}

bool Scene2_Restore()
{
	return true;
}

bool Scene2_Release()
{
	return true;
}