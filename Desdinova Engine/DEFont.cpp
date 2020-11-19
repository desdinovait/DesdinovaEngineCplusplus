#include "DEGeneralIncludes.h"

DLL_API DEFont::DEFont()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
}



DLL_API bool DEFont::Create(LPSTR face, int height, bool bold, bool italic, LPCSTR logTitle)
{
	HRESULT hr;
	isCreated = false;
	mainLogTitle = logTitle;
	mainTag = NULL;
	isSprite = true;
	isBitmap = false;
	mainText = "No text";
	mainColour = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	try
	{
		if (DECore::D3DDevice != NULL)
		{	
			//La dimensione che vuole D3DXCreateFont è in logical units
			//e per convertirla in logical pixels(pixel effettivi di dimensione)
			//è necessaria la seguente formula:
			//             -( point_size * LOGPIXELSY )
			//    height = ----------------------------
			//                          72
			HDC hDC;
			hDC = GetDC( NULL );
			height = -( MulDiv( height, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );
			ReleaseDC( NULL, hDC );

			//Crea il font con le DX
			hr = D3DXCreateFont( DECore::D3DDevice, height, 0, bold ? FW_BOLD : FW_NORMAL, 0, italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT(face), &m_pFont );
			if (hr==D3D_OK)
			{
				hr = D3DXCreateSprite( DECore::D3DDevice, &m_pD3DXSprite );
				if (hr==D3D_OK)	
				{
					isCreated=true;
				}
				else
				{
					isCreated=false;
					DELog::LogError("<li>%s : D3DXCreateSprite(...): %s", logTitle, DXGetErrorDescription9(hr));
				}
			}
			else
			{
				DELog::LogError("<li>%s : D3DXCreateFont(...): %s", logTitle, DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>%s : DECore::D3DDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}

DLL_API bool DEFont::Create(LPSTR faceFile, DWORD fontKeyColor, unsigned int totalRows, unsigned int lettersPerRow, LPCSTR logTitle)
{
	isCreated = false;
	mainLogTitle = logTitle;
	mainTag = NULL;
	isSprite = false;
	isBitmap = true;
	mainText = "";

	try
	{
		if (DECore::D3DDevice != NULL)
		{	
			//Crea il pannello
			panelBitmap.Create("panelBitmap");
			panelBitmap.SetTextureID( panelBitmap.AddTexture(faceFile, fontKeyColor, totalRows, lettersPerRow, 0));
			panelBitmap.SetDimensions( D3DXVECTOR2(panelBitmap.GetTextureInfoID(0, false).x , panelBitmap.GetTextureInfoID(0, false).y)  );
			panelBitmap.SetMode(false);
			panelBitmap.SetPosition(D3DXVECTOR2(0, 0));
			panelBitmap.SetRotation(0.0f);
			panelBitmap.SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));



			letterDimX = (int)panelBitmap.GetTextureInfoID(0, false).x;
			letterDimY = (int)panelBitmap.GetTextureInfoID(0, false).y;
			
			lettersString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!?:;'(),-.'@#";

			isCreated=true;		
		}
		else
		{
			DELog::LogError("<li>%s : DECore::D3DDevice: NULL", logTitle);
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}

DLL_API char *DEFont::GenerateTypedText(LPSTR pText, float velocity, bool restart, int &strlenCurrent)
{
	//Alloca e resetta la memoria necessaria
	if (allocCounter==false)
	{
		currentText = (char *)malloc(strlen(pText)*sizeof(char));
		allocCounter=true;
	}
	memset(currentText, 0, strlen(pText)*sizeof(char));
	//Aggiorna il contatore in base alla velocità
	timeCounter = timeCounter + velocity;

	if (timeCounter <= strlen(pText))	
	{
		//Copia il numero di carattere contati fino a ora
		strncpy(currentText, pText, (int)timeCounter);
	}
	else
	{
		//E' arrivato alla fine
		if (restart)
		{
			//Resetta il contatore
			timeCounter = 0;
			free(currentText);
			allocCounter = false;
		}
		else 
		{
			//Scrive direttamente il testo desiderato in modo che venga sempre visualizzato interamente
			strcpy(currentText, pText);
			//Questo evita di continuare ad aumentare il valore di timeCounter
			//e quindi alla lunga causare anche un overflow
			timeCounter = timeCounter - velocity;
		}
	}

	//Il valore strlenCurrent viene restituito alla funzione indicando la lunghezza corrente
	//della stringa restituita, usare:
	//float cur=0;
	//GenerateTypedText("Desdinova", 0.05f, cur);
	strlenCurrent = (int)strlen(currentText);

	return currentText;
}

DLL_API void DEFont::SetBitmapLettersString(LPSTR newLettersString)
{
	lettersString = newLettersString;
}
DLL_API void DEFont::SetText(LPSTR pText)
{
	mainText = (LPSTR)pText;
}
DLL_API void DEFont::SetRectSprite(float x, float y, float width, float height, DWORD format)
{
	mainRect.left = (LONG)x;
	mainRect.top = (LONG)y;
	mainFormat = format;
	//Se una delle due dimensioni è uguale a 
	if ((width==0) || (height==0))
	{
		mainRect.right = 0;
		mainRect.bottom = 0;
		mainFormat = DT_NOCLIP;	//Imposta il formato normale
	}
	else
	{
		mainRect.right=(LONG)x + (LONG)width;
		mainRect.bottom=(LONG)y + (LONG)height;
		//Il formato rimane quello definito da utente
	}
}

DLL_API void DEFont::SetColor(D3DCOLOR newColor)
{
	mainColour = newColor;
}


DLL_API void DEFont::SetRectBitmap(float x, float y, float scale, float interLetter, int mode)
{
	if ((isCreated)&&(isBitmap))
	{
		unsigned int totalLetters=(unsigned int)strlen(mainText);
		D3DXVECTOR3 pos;
		pos.z = 1;

		if (mode==1)
		{
			pos.x = x - (((float)((letterDimX*scale)+ interLetter) * (float)totalLetters))/2 +(((letterDimX*scale)+interLetter)/2);
			pos.y = y; 
		}
		else
		{
			pos.x = x;
			pos.y = y; 
		}
		mainRect.left = (LONG)pos.x;
		mainRect.top = (LONG)pos.y;
		mainScale = scale;
		mainInner = interLetter;
		mainMode = mode;
	}
}


DLL_API void DEFont::Render()
{
	if (isCreated)
	{
		if (isSprite)
		{
			m_pFont->DrawText( m_pD3DXSprite, mainText, -1, &mainRect, mainFormat, mainColour);
		}
		if (isBitmap)
		{
			unsigned int totalLetters=(unsigned int)strlen(mainText);
			float posX = (float)mainRect.left;
			float posY = (float)mainRect.top;
			for (unsigned int t=0; t<totalLetters; t++)
			{
				int currentFrameLetter = CalculateBitmapFrame(mainText[t]);
				panelBitmap.GoToFrame(currentFrameLetter);
				panelBitmap.SetPosition(D3DXVECTOR2(posX, posY));
				panelBitmap.SetScale(D3DXVECTOR2(mainScale,mainScale));
				if (mainMode==0)	panelBitmap.SetMode(false);
				else				panelBitmap.SetMode(true);
				panelBitmap.SetColor(mainColour);
				panelBitmap.Render();
				posX = posX + (letterDimX*mainScale) + mainInner;	
			}
		}
	}
}




DLL_API int DEFont::CalculateBitmapFrame(char offsetChar)
{
	//"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789   .,:()-'!+\?/[]^ %=$#@*";
	//"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!?:;'(),-.'@# ";
   
	char *pdest;
	pdest = strchr( lettersString, offsetChar );
    int result = (int)(pdest - lettersString);
	return result;


	/*Lettere maiuscole
	if (offsetChar == 'A') return 0;
	if (offsetChar == 'B') return 1;
	if (offsetChar == 'C') return 2;
	if (offsetChar == 'D') return 3;
	if (offsetChar == 'E') return 4;
	if (offsetChar == 'F') return 5;
	if (offsetChar == 'G') return 6;
	if (offsetChar == 'H') return 7;
	if (offsetChar == 'I') return 8;
	if (offsetChar == 'J') return 9;
	if (offsetChar == 'K') return 10;
	if (offsetChar == 'L') return 11;
	if (offsetChar == 'M') return 12;
	if (offsetChar == 'N') return 13;
	if (offsetChar == 'O') return 14;
	if (offsetChar == 'P') return 15;
	if (offsetChar == 'Q') return 16;
	if (offsetChar == 'R') return 17;
	if (offsetChar == 'S') return 18;
	if (offsetChar == 'T') return 19;
	if (offsetChar == 'U') return 20;
	if (offsetChar == 'V') return 21;
	if (offsetChar == 'W') return 22;
	if (offsetChar == 'X') return 23;
	if (offsetChar == 'Y') return 24;
	if (offsetChar == 'Z') return 25;
	//lettere minuscole
	if (offsetChar == 'a') return 26;
	if (offsetChar == 'b') return 27;
	if (offsetChar == 'c') return 28;
	if (offsetChar == 'd') return 29;
	if (offsetChar == 'e') return 30;
	if (offsetChar == 'f') return 31;
	if (offsetChar == 'g') return 32;
	if (offsetChar == 'h') return 33;
	if (offsetChar == 'i') return 34;
	if (offsetChar == 'j') return 35;
	if (offsetChar == 'k') return 36;
	if (offsetChar == 'l') return 37;
	if (offsetChar == 'm') return 38;
	if (offsetChar == 'n') return 39;
	if (offsetChar == 'o') return 40;
	if (offsetChar == 'p') return 41;
	if (offsetChar == 'q') return 42;
	if (offsetChar == 'r') return 43;
	if (offsetChar == 's') return 44;
	if (offsetChar == 't') return 45;
	if (offsetChar == 'u') return 46;
	if (offsetChar == 'v') return 47;
	if (offsetChar == 'w') return 48;
	if (offsetChar == 'x') return 49;
	if (offsetChar == 'y') return 50;
	if (offsetChar == 'z') return 51;
	//Numeri
	if (offsetChar == '0') return 52;
	if (offsetChar == '1') return 53;
	if (offsetChar == '2') return 54;
	if (offsetChar == '3') return 55;
	if (offsetChar == '4') return 56;
	if (offsetChar == '5') return 57;
	if (offsetChar == '6') return 58;
	if (offsetChar == '7') return 59;
	if (offsetChar == '8') return 60;
	if (offsetChar == '9') return 61;
	//Simboli
	if (offsetChar == '!') return 62;
	if (offsetChar == '?') return 63;
	if (offsetChar == ':') return 64;
	if (offsetChar == ';') return 65;
	if (offsetChar == '"') return 66;
	if (offsetChar == '(') return 67;
	if (offsetChar == ')') return 68;
	if (offsetChar == ',') return 69;
	if (offsetChar == '-') return 70;
	if (offsetChar == '.') return 71;
	if (offsetChar == '#') return 72;
	if (offsetChar == '@') return 73;
	if (offsetChar == '*') return 74;
	if (offsetChar == ' ') return 75;*/
}

DLL_API bool DEFont::BeginTextSprite()
{
	if ((isCreated)&&(isSprite))
	{
		HRESULT hr = m_pD3DXSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
		if (FAILED(hr))return false;
		return true;
	}
	return false;
}
DLL_API bool DEFont::EndTextSprite()
{
	if ((isCreated)&&(isSprite))
	{
		HRESULT hr = m_pD3DXSprite->End();
		if (FAILED(hr))return false;
		return true;
	}
	return false;
}


DLL_API bool DEFont::GetCreated()
{
	return isCreated;
}

DLL_API bool DEFont::Invalidate()
{
	try
	{
		if((isSprite)&&(FAILED(m_pD3DXSprite->OnLostDevice())))	return false;
		if(isBitmap)panelBitmap.Invalidate();
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEFont::Restore()
{
	try
	{
		if((isSprite)&&(FAILED(m_pD3DXSprite->OnResetDevice()))) return false;
		if(isBitmap)panelBitmap.Restore();
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEFont::Release()
{
	try
	{
		if(isSprite)	SafeRelease(m_pD3DXSprite);
		if(isBitmap)
		{
			panelBitmap.Release();
		}
		isCreated=false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

DLL_API bool DEFont::SetTag(LPCSTR newTag)
{
	try
	{
		mainTag = newTag;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API LPCSTR DEFont::GetTag()
{
	LPCSTR	nullTag="NoTag";
	try
	{
		if (mainTag!=NULL)	return mainTag;
		else				return nullTag;
	}
	catch(...)
	{
		return nullTag;
	}
	return nullTag;
}