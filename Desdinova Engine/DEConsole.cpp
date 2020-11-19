#include "DEGeneralIncludes.h"

DLL_API void DEConsoleManager::AddFunction( const char* fnName, STRUCT_DE_CONSOLEFUNCTION& fi )
{

	if( !fnName )
		return;

	char buffer[256] = {0};
	
	int idx = 0;

	while( *fnName )
	{
		buffer[idx] = tolower( *fnName );
		++idx;
		++fnName;
	}

	buffer[idx] = '\0';
	int key = DEUtility::HashString( buffer );

	std::pair<int, STRUCT_DE_CONSOLEFUNCTION> p( key, fi );

	mFunctionMap.insert( p );
}

DLL_API DWORD DEConsoleManager::Call_cdecl( const void* args, size_t sz, DWORD func )
{
	DWORD rc;               /// here's our return value...
	__asm
	{
		mov   ecx, sz       /// get size of buffer
		mov   esi, args     /// get buffer
		sub   esp, ecx      /// allocate stack space
		mov   edi, esp      /// start of destination stack frame
		shr   ecx, 2        /// make it dwords
		rep   movsd         /// copy params to real stack
		call  [func]        /// call the function
		mov   rc,  eax      /// save the return value
		add   esp, sz       /// restore the stack pointer
	}
	return ( rc );
}

	
DLL_API bool DEConsoleManager::ParseFloat( const char* str, float& f )
{
	f = 0;
	float fPlace = 1.0f;
	bool  bStarted = false, bNegative = false, bDecimal = false;
	char c = 0;

	while( (c = *(str)) != static_cast<char>(0) )
	{
		if( isdigit( c ) )
		{
			bStarted = true;
	        
			if( bDecimal )
			{
				f += static_cast<float>(c - '0') / fPlace;
				fPlace *= 10.0; 
			}
			else
			{
				f *= 10.0;  
				f += static_cast<float>(c - '0' ); 
			}
		}
		else if( c == ' ' || c == '\n' || c == '\r' )
		{
			if( bStarted )
			{
				break;
			}
		}
		else if( c == '.' )
		{
			if( bDecimal )
				return false;

			bDecimal = true;
			fPlace = 10.0;
		}
		else if( c == '-' )
		{
			if( bStarted )
			{
				return false;
			}

			bNegative = true;
		}
		else
			return false;

		++str;
	}

	if( bNegative )
		f = -f;

	return true;
}

DLL_API bool DEConsoleManager::ParseInt( const char* str, int& res )
{
	char c = 0;
	bool bNegative = false;
	bool bStarted = false;
	res = 0;  
	int count = 0;

	while( (c = *str) )
	{
		if( isdigit( c ) )
		{
			bStarted = true;
			res *= 10;
			res += ( c - '0' );
		}
		else if( c == ' ' || c == '\n' || c == '\r' )
		{
			if( bStarted )
			{
				break;
			}
		}
		else if( c == '-' )
		{
			if( bStarted )
			{
				return false;
			}

			bNegative = true;
		}
		else
		{
			return false;
		}

		++str;
	}

	if( bNegative )
		res = -res;

	return true;
}

DLL_API bool DEConsoleManager::ParseChar( const char* str, char& c )
{
	/*while( str )
	{
		c = *str;

		if( c == 0 )
		{
			return false;
		}

		++str;

		if( c == ' ' || c == '\n' || c == '\r' )
		{
			continue;
		}


		//return true;
	}*/
	//return false;

	strcpy(&c, str);
	return true;
}

DLL_API bool DEConsoleManager::ParseBool( const char* str, bool& b )
{
	if( !str )
		return false;

	if( strlen(str) > 1 )
		return false;
	
	if( *str == '0' )
	{
		b = false;
		return true;
	}
	if( *str == '1' )
	{
		b = true;
		return true;
	}

	return false;
}

DLL_API bool DEConsoleManager::ParseDword( const char* str, DWORD& d )
{
	char* ptr = 0;

	d = strtoul( str, &ptr, 16 );

	if( !d || !ptr )
		return false;

	if( *ptr != '\0' )
		return false;

	return true;
}

#pragma warning( push )
#pragma warning( disable : 4311 ) // Pointer cast

DLL_API std::string DEConsoleManager::CallFunction( const char* cmd, int size )
{
	std::vector<std::string> tokens;
	std::vector<DWORD> params;

	/// Count number of quotes. Error if uneven...
	bool bEven = true;
	for( int i = 0; i < size; ++i )
	{
		if( cmd[i] == '\"' )
			bEven = !bEven;
	}

	if( !bEven )
		return std::string( "Expected close quote. Enter new command:\n" );

	DEUtility::TokenizeString( cmd, tokens, " ,();", false );

	if( tokens.size() == 0 )
		return std::string( "Could not parse command. Enter new command:\n" );

	char strFunc[256];
	int idx = 0;

	for( unsigned int i = 0; i < tokens[0].size(); ++i )
	{
		strFunc[idx++] = tolower(tokens[0][i]);
	}		

	if( strFunc[--idx] != '\0' )
		strFunc[++idx] = '\0';

	MapIter_t iter = mFunctionMap.find( DEUtility::HashString( strFunc ) );

	if( iter == mFunctionMap.end() )
		return "Error in function or not found. Enter new command:\n";

	STRUCT_DE_CONSOLEFUNCTION cf = iter->second;

	int numNonParamParams = 0;
	for( unsigned int i = 0; i < cf.params.size(); ++i )
	{
		if( cf.params[i] == DE_CONSOLEPARAM_CORE_PTR ||
			cf.params[i] == DE_CONSOLEPARAM_GFX_PTR )
		{
			++numNonParamParams;
		}
	}		
	
	if( tokens.size() + numNonParamParams != cf.params.size() + 1 )
	{
		char numBuff[16];
		itoa((int)cf.params.size() - numNonParamParams, numBuff, 10 );
		char errBuff[128] = "Error in number of parameters. Expecting ";
		strcat( errBuff, numBuff );
		strcat( errBuff, " parameters of type: " );

		for( unsigned int i = numNonParamParams; i < cf.params.size(); ++i )
		{
			switch( cf.params[i] )
			{
			case DE_CONSOLEPARAM_BOOL:
				strcat( errBuff, " bool " );
				break;

			case DE_CONSOLEPARAM_INT:
				strcat( errBuff, " int " );
				break;

			case DE_CONSOLEPARAM_FLOAT:
				strcat( errBuff, " float " );
				break;

			case DE_CONSOLEPARAM_DWORD:
				strcat( errBuff, " dword " );
				break;					
			}
			
			strcat( errBuff, (i == cf.params.size() - 1) ? "\n" : "," );
		}
		return std::string( errBuff );
	}

	int anInt = 0;
	bool aBool = false;
	float aFloat = 0;
	DWORD aDword = 0;

	for( unsigned int i = 0; i < cf.params.size(); ++i )
	{
		int ii = i - numNonParamParams + 1;

		if( cf.params[i] == DE_CONSOLEPARAM_BOOL )
		{
			if( !ParseBool( tokens[ii].c_str(), aBool ) )
			{
				char numBuff[16];
				itoa(i-numNonParamParams+1, numBuff, 10 );
				char errBuff[128] = "Error with param ";
				strcat( errBuff, numBuff );
				strcat( errBuff, ": Must be 1 for true and 0 for false\n" );
				return std::string(errBuff);
			}

			params.push_back( (DWORD)aBool );
		}
		else if( cf.params[i] == DE_CONSOLEPARAM_INT )
		{
			if( !ParseInt( tokens[ii].c_str(), anInt ) )
			{
				char numBuff[16];
				itoa(i-numNonParamParams+1, numBuff, 10 );
				char errBuff[64] = "Error with param ";
				strcat( errBuff, numBuff );
				strcat( errBuff, ": Must be integer.\n" );
				return std::string(errBuff);
			}

			params.push_back( (DWORD)anInt );
		}
		else if( cf.params[i] == DE_CONSOLEPARAM_FLOAT )
		{
			if( !ParseFloat( tokens[ii].c_str(), aFloat ) )
			{
				char numBuff[16];
				itoa(i-numNonParamParams+1, numBuff, 10 );
				char errBuff[64] = "Error with param ";
				strcat( errBuff, numBuff );
				strcat( errBuff, ": Must be float.\n" );
				return std::string(errBuff);
			}

			params.push_back( *(DWORD*)&aFloat );
		}
		else if( cf.params[i] == DE_CONSOLEPARAM_DWORD )
		{
			bool bError = false;

			if( tokens[ii].size() == 1 )
			{
				if( tokens[ii][0] != '0' )
					bError = true;
			}
			else
			{
				if( !ParseDword( tokens[ii].c_str(), aDword ) )
				{
					bError = true;
				}
			}

			if( bError )
			{					
				char numBuff[16];
				itoa(i-numNonParamParams+1, numBuff, 10 );
				char errBuff[128] = "Error with param ";
				strcat( errBuff, numBuff );
				strcat( errBuff, ": Must be DWORD. Must start \'0x\' unless it's a zero (then just pass in \'0\').\n" );
				return std::string(errBuff);
			}

			params.push_back( aDword );
		}
	}


	const void* paramBuffer = 0;
	if( params.size() )
		paramBuffer = &(*params.begin());

	DWORD theRet = 0;
	theRet = this->Call_cdecl( paramBuffer, params.size() * sizeof(DWORD), (DWORD)cf.proc );

	if( !theRet )
	{
		return std::string( "Command failed. Enter new command:\n" );
	}
	
	return std::string();
}

#pragma warning( pop )





DLL_API DEConsole::DEConsole()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
	cursorFlicker = 100;
	position = D3DXVECTOR2(0,0);
}

DLL_API bool DEConsole::Create(LPSTR fontFace, int fontHeight, bool fontBold, bool fontItalic, DWORD fontColor, bool initialOn, LPCSTR logTitle)
{
	mainLogTitle = logTitle;
	isCreated = false;
	mConsoleOn = false;

	try
	{
		mpConsole = new DEConsoleManager;
		mConsoleTextHeight = fontHeight;
		mConsoleTextColor = fontColor;
		mConsoleCaptionLines = 1;
		isCreated = mConsoleFont.Create(fontFace, fontHeight, fontBold, fontItalic, "mConsoleFont");
		if (initialOn) SwitchEnable();
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error", logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created", logTitle);		

	return isCreated;
}


DLL_API bool DEConsole::Add(void* prc, const char* name, ENUM_DE_CONSOLEPARAM p1, ENUM_DE_CONSOLEPARAM p2, ENUM_DE_CONSOLEPARAM p3, ENUM_DE_CONSOLEPARAM p4, ENUM_DE_CONSOLEPARAM p5, ENUM_DE_CONSOLEPARAM p6, ENUM_DE_CONSOLEPARAM p7 )
{
	if (isCreated)
	{
		try
		{
			STRUCT_DE_CONSOLEFUNCTION f;
			if (p1 != NULL) f.params.push_back( p1 ); 
			if (p2 != NULL) f.params.push_back( p2 );
			if (p3 != NULL) f.params.push_back( p3 ); 
			if (p4 != NULL) f.params.push_back( p4 );
			if (p5 != NULL) f.params.push_back( p5 ); 
			if (p6 != NULL) f.params.push_back( p6 );
			if (p7 != NULL) f.params.push_back( p7 ); 
			f.proc = prc; 
			mpConsole->AddFunction( name, f );
			return true;
		}
		catch(...)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return false;
}



DLL_API inline void DEConsole::SetPosition(D3DXVECTOR2 newPosition)
{
	position = newPosition;
}
DLL_API inline void DEConsole::SetCursorFlick(float newCursorFlicker)
{
	cursorFlicker = newCursorFlicker;
}

DLL_API inline D3DXVECTOR2 DEConsole::GetPosition()
{
	return position;
}
DLL_API inline float DEConsole::GetCursorFlicker()
{
	return cursorFlicker;
}

DLL_API void DEConsole::Render()
{
	if ((mConsoleOn)&&(isCreated))
	{
		mConsoleTime = mConsoleTime ++;
		if (mConsoleTime >= (unsigned int)cursorFlicker)
		{
			if( mConsoleBuff[mConsoleIdx] == (char)'_' )
			{
				mConsoleBuff[mConsoleIdx] = ' ';
			}
			else
			{
				mConsoleBuff[mConsoleIdx] = (char)'_';
			}
			mConsoleTime = 0;
		}

		mConsoleFont.BeginTextSprite();
		//Scrive il caption se specificato
		if (mConsoleMain!="")
		{
			//Scrive il caption
			mConsoleFont.SetText(mConsoleMain);
			mConsoleFont.SetColor(mConsoleTextColor);
			mConsoleFont.SetRectSprite(position.x, position.y, 0, 0, DT_NOCLIP );
			mConsoleFont.Render();
			//Scrive il buffer info
			mConsoleFont.SetText(mConsoleInfo);
			mConsoleFont.SetColor(mConsoleTextColor);
			mConsoleFont.SetRectSprite(position.x, position.y + (mConsoleTextHeight*mConsoleCaptionLines) + (mConsoleCaptionLines*5), 0, 0, DT_NOCLIP );
			mConsoleFont.Render();
			//Scrive il buffer
			mConsoleFont.SetText(mConsoleBuff);
			mConsoleFont.SetColor(mConsoleTextColor);
			mConsoleFont.SetRectSprite(position.x, position.y + (mConsoleTextHeight*mConsoleCaptionLines) + (mConsoleCaptionLines*6) + mConsoleTextHeight, 0, 0, DT_NOCLIP );
			mConsoleFont.Render();
		}
		else
		{
			//Scrive il buffer info
			mConsoleFont.SetText(mConsoleInfo);
			mConsoleFont.SetColor(mConsoleTextColor);
			mConsoleFont.SetRectSprite(position.x, position.y, 0, 0, DT_NOCLIP );
			mConsoleFont.Render();
			//Scrive il buffer
			mConsoleFont.SetText(mConsoleBuff);
			mConsoleFont.SetColor(mConsoleTextColor);
			mConsoleFont.SetRectSprite(position.x, position.y + mConsoleTextHeight, 0, 0, DT_NOCLIP );
			mConsoleFont.Render();
		}
		mConsoleFont.EndTextSprite();
	}
}


DLL_API void DEConsole::CmdHistoryUp()
{
	currentConsoleHistory = currentConsoleHistory +1;
	if (currentConsoleHistory >= totalConsoleHistory-1) currentConsoleHistory = totalConsoleHistory-1;

	if ((mConsoleOn)&&(isCreated))
	{
		memset(mConsoleBuff, 0, sizeof(char) * 128);
		strcpy(mConsoleBuff, mainConsoleCmd[currentConsoleHistory]);
		mConsoleIdx = mainConsoleCmdIdx[currentConsoleHistory];

		memset(mConsoleCmd, 0, sizeof(char) * 128);
		strcpy(mConsoleCmd, mainConsoleCmd[currentConsoleHistory]);
		mConsoleCmdIdx = mainConsoleCmdIdx[currentConsoleHistory];
	}
}

DLL_API void DEConsole::CmdHistoryDown()
{
	currentConsoleHistory = currentConsoleHistory -1;
	if ((currentConsoleHistory <= 0 )||(currentConsoleHistory >200))
	{
		currentConsoleHistory = 0;
	}

	if ((mConsoleOn)&&(isCreated))
	{
		memset(mConsoleBuff, 0, sizeof(char) * 128);
		strcpy(mConsoleBuff, mainConsoleCmd[currentConsoleHistory]);
		mConsoleIdx = mainConsoleCmdIdx[currentConsoleHistory];

		memset(mConsoleCmd, 0, sizeof(char) * 128);
		strcpy(mConsoleCmd, mainConsoleCmd[currentConsoleHistory]);
		mConsoleCmdIdx = mainConsoleCmdIdx[currentConsoleHistory];
	}
}

DLL_API void DEConsole::CmdBack()
{
	if ((mConsoleOn)&&(isCreated))
	{
		if( mConsoleIdx > 0 )
		{
			mConsoleBuff[--mConsoleIdx] = '\0';
			mConsoleBuff[mConsoleIdx] = ' ';
			mConsoleBuff[mConsoleIdx+1] = '\0';
		}
		if( mConsoleCmdIdx > 0 )
		{
			mConsoleCmd[--mConsoleCmdIdx] = '\0';
		}
	}
}

DLL_API void DEConsole::CmdReturn()
{
	if ((mConsoleOn)&&(isCreated))
	{
		strcpy(mainConsoleCmd[totalConsoleHistory],mConsoleCmd);
		mainConsoleCmdIdx[totalConsoleHistory] = mConsoleCmdIdx;
		totalConsoleHistory = totalConsoleHistory + 1;
		currentConsoleHistory = totalConsoleHistory;

		std::string console_ret;
		if( mConsoleCmd[0] != '\0' )
		{
			console_ret = mpConsole->CallFunction( mConsoleCmd, mConsoleCmdIdx );
			if( console_ret.size() )
			{
				strcpy_s(mConsoleInfo, console_ret.c_str() );
				mConsoleIdx = 0;
			}
			else
			{
				_tcscpy( mConsoleInfo,	_T("Done! Enter new command:\n"));
				mConsoleIdx = 0;
				SetEnable(false);
			}
			mConsoleBuff[mConsoleIdx] = ' ';
			mConsoleBuff[mConsoleIdx+1] = '\0';
			mConsoleCmdIdx = 0;
			mConsoleCmd[mConsoleCmdIdx] = '\0';//*/
		}
	}
}

DLL_API void DEConsole::CmdDefault(int wParamValue)
{
	if ((mConsoleOn)&&(isCreated))
	{
		if( mConsoleIdx+1 >= 1024 || mConsoleCmdIdx >= 128 )
		{
			_tcscpy( mConsoleInfo,	_T("Overflow! Enter new command:\n"));
			mConsoleIdx = 0;
			mConsoleBuff[mConsoleIdx] = ' ';
			mConsoleBuff[mConsoleIdx+1] = '\0';
			mConsoleCmdIdx = 0;
			mConsoleCmd[mConsoleCmdIdx] = '\0';
			goto exitCons;
		}

		mConsoleBuff[mConsoleIdx++] = tolower((int)wParamValue);
		mConsoleBuff[mConsoleIdx] = ' ';
		mConsoleBuff[mConsoleIdx+1] = '\0';

		mConsoleCmd[mConsoleCmdIdx++] = tolower((int)wParamValue);
		mConsoleCmd[mConsoleCmdIdx] = '\0';
	}
exitCons:
	unsigned short exitConsVar=0;
}

DLL_API void DEConsole::CmdDefault(char wParamValue)
{
	if ((mConsoleOn)&&(isCreated))
	{
		if( mConsoleIdx+1 >= 1024 || mConsoleCmdIdx >= 128 )
		{
			_tcscpy( mConsoleInfo,	_T("Overflow! Enter new command:\n"));
			mConsoleIdx = 0;
			mConsoleBuff[mConsoleIdx] = ' ';
			mConsoleBuff[mConsoleIdx+1] = '\0';
			mConsoleCmdIdx = 0;
			mConsoleCmd[mConsoleCmdIdx] = '\0';
			goto exitCons;
		}

		mConsoleBuff[mConsoleIdx++] = wParamValue;
		mConsoleBuff[mConsoleIdx] = ' ';
		mConsoleBuff[mConsoleIdx+1] = '\0';

		mConsoleCmd[mConsoleCmdIdx++] = wParamValue;
		mConsoleCmd[mConsoleCmdIdx] = '\0';
	}
exitCons:
	unsigned short exitConsVar=0;
}


DLL_API inline void DEConsole::SetTextColor(DWORD textColor)
{
	mConsoleTextColor = textColor;
}
DLL_API inline DWORD DEConsole::GetTextColor()
{
	return mConsoleTextColor;
}

DLL_API inline void DEConsole::SetEnable(bool enableConsole)
{
	if (isCreated)
	{
		mConsoleOn = enableConsole;
		if (mConsoleOn)
		{
			_tcscpy( mConsoleInfo, _T("Enter new command:\n"));

			mConsoleIdx = 0;
			mConsoleBuff[mConsoleIdx] = ' ';
			mConsoleBuff[mConsoleIdx+1] = '\0';
			mConsoleCmdIdx = 0;
			mConsoleCmd[0] = '\0';
		}
	}
}

DLL_API inline bool DEConsole::GetEnable()
{
	if (isCreated)
	{
		return mConsoleOn;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API inline void DEConsole::SwitchEnable()
{
	if (isCreated)
	{
		mConsoleOn = !mConsoleOn;
		SetEnable(mConsoleOn);
	}
}

DLL_API inline void DEConsole::SetCaption(const char *maincaption, int occupedLines)
{
	strcpy(mConsoleMain, maincaption);
	mConsoleCaptionLines = occupedLines;
}

DLL_API void DEConsole::LogReport()
{
	int textureBlock=0;
	DELog::LogInfo("<br><b>Console Report:</b>");
	DELog::LogInfo("<li>Commands: %d ", totalConsoleHistory);
	if (totalConsoleHistory>0)
	{
		for (unsigned int t=0; t<totalConsoleHistory; t++)
		{
			DELog::LogInfo("<li>Command[%d]: %s ", t, mainConsoleCmd[t]);
		}
	}
}

DLL_API inline bool DEConsole::GetCreated()
{
	return isCreated;
}

DLL_API bool DEConsole::Invalidate()
{
	try
	{
		if (mConsoleFont.Invalidate())	return true;
		else							return false;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEConsole::Restore()
{
	try
	{
		if (mConsoleFont.Restore())	return true;
		else						return false;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DEConsole::Release()
{
	try
	{
		SafeDelete(mpConsole);
		mpConsole = NULL;
		mConsoleFont.Release();
		mConsoleOn=false;
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

DLL_API bool DEConsole::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DEConsole::GetTag()
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