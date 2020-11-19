#include "DEGeneralIncludes.h"

LPDIRECTINPUT8			DEInput::DirectInputObject=NULL;
LPDIRECTINPUTDEVICE8	DEInput::KeyboardDevice=NULL;
LPDIRECTINPUTDEVICE8	DEInput::MouseDevice=NULL;
LPDIRECTINPUTDEVICE8	DEInput::JoyDevice=NULL;

DLL_API BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
    HRESULT hr;
	hr = DEInput::DirectInputObject->CreateDevice( pdidInstance->guidInstance, &DEInput::JoyDevice, NULL );
    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    return DIENUM_STOP;
}


DLL_API BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 
    
        // Set the range for the axis
        if( FAILED( DEInput::JoyDevice->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;    
    }

    return DIENUM_CONTINUE;
}

DLL_API void DEInput::Release()
{
	if(KeyboardDevice != NULL)
	{
		KeyboardDevice->Unacquire();
		SafeRelease(KeyboardDevice);
	}

	if(MouseDevice != NULL)
	{
		MouseDevice->Unacquire();
		SafeRelease(MouseDevice);
	}

	if(JoyDevice != NULL)
	{
		JoyDevice->Unacquire();
		SafeRelease(JoyDevice);
	}

	SafeRelease(DirectInputObject);

	isCreated = false;
	keyboardCreated = false;
	mouseCreated = false;
	joyCreated = false;
	mainHWND = NULL;
}

DLL_API bool DEInput::Init(HWND engineHWND)
{
	DirectInputObject = NULL;
	KeyboardDevice = NULL;
	MouseDevice = NULL;
	isCreated = false;
	keyboardCreated = false;
	mouseCreated = false;

	DELog::LogInfo("<br><b>DirectInput Initialization</b>");

	D3DVIEWPORT9 currentViewport;
	hr = DECore::D3DDevice->GetViewport(&currentViewport);
	if (hr == D3D_OK)
	{
		m_fCursorX = (float)currentViewport.Width / 2;
		m_fCursorY = (float)currentViewport.Height / 2;
		m_dwScreenWidth = currentViewport.Width;
		m_dwScreenHeight = currentViewport.Height;
		DELog::LogInfo("<li>CurrentViewport Dimensions: %dx%d", m_dwScreenWidth, m_dwScreenHeight);
	}
	else
	{
		DELog::LogWarning("<li>Device->GetViewport(...): %s - Impossible to retrive viewport informations", DXGetErrorDescription9(hr));
	}

	//DirectInput
	hr = DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&DirectInputObject,NULL);
	if (hr==DI_OK)
	{
		DELog::LogInfo("<li>DirectInput8Create(...): %s", DXGetErrorDescription9(hr));
		isCreated = true;
	}
	else
	{
		DELog::LogError("<li>DirectInput8Create(...): %s", DXGetErrorDescription9(hr));
		isCreated = false;
	}


	//Creazione periferiche
	if (isCreated)
	{
		//Keyboard
		hr = DirectInputObject->CreateDevice(GUID_SysKeyboard,&KeyboardDevice,NULL);
		if (hr==DI_OK)
		{
			hr = KeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
			if (hr==DI_OK)
			{
				hr = KeyboardDevice->SetCooperativeLevel(engineHWND, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
				if (hr==DI_OK)
				{
					hr = KeyboardDevice->Acquire();
					if (hr==DI_OK)
					{
						DELog::LogInfo("<li>KeyboardDevice->Acquire(...): %s", DXGetErrorDescription9(hr));
						keyboardCreated = true;
					}
					else
					{
						DELog::LogError("<li>KeyboardDevice->Acquire(...): %s", DXGetErrorDescription9(hr));
					}
				}
				else
				{
					DELog::LogError("<li>KeyboardDevice->SetCooperativeLevel(...): %s", DXGetErrorDescription9(hr));
				}
			}
			else
			{
				DELog::LogError("<li>KeyboardDevice->SetDataFormat(...): %s", DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>KeyboardDevice: NULL - %s", DXGetErrorDescription9(hr));
		}


		//Mouse
		DIDEVCAPS MouseCapabilities;
		hr = DirectInputObject->CreateDevice(GUID_SysMouse,&MouseDevice,NULL);
		if (hr==DI_OK)
		{
			hr = MouseDevice->SetDataFormat(&c_dfDIMouse2);
			if (hr==DI_OK)
			{
				hr = MouseDevice->SetCooperativeLevel(engineHWND,DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
				if (hr==DI_OK)
				{				
					hr = MouseDevice->Acquire();
					if (hr==DI_OK)
					{					
						DELog::LogInfo("<li>MouseDevice->Acquire(...): %s", DXGetErrorDescription9(hr));
						MouseCapabilities.dwSize = sizeof(MouseCapabilities);
						MouseDevice->GetCapabilities(&MouseCapabilities);
						if(!(MouseCapabilities.dwFlags & DIDC_ATTACHED))
						{
							DELog::LogWarning("<li>MouseCapabilities.dwFlags: Mouse not currently attached");
						}
						m_dwAxes = MouseCapabilities.dwAxes;
						m_dwButtons = MouseCapabilities.dwButtons;
						mouseCreated=true;
					}
					else
					{
						DELog::LogError("<li>MouseDevice->Acquire(...): %s", DXGetErrorDescription9(hr));
					}
				}
				else
				{
					DELog::LogError("<li>MouseDevice->SetCooperativeLevel(...): %s", DXGetErrorDescription9(hr));
				}
			}
			else
			{
				DELog::LogError("<li>MouseDevice->SetDataFormat(...): %s", DXGetErrorDescription9(hr));
			}
		}
		else
		{
			DELog::LogError("<li>MouseDevice: NULL - %s", DXGetErrorDescription9(hr));
		}


		//Joy
		hr = DirectInputObject->EnumDevices( DI8DEVCLASS_GAMECTRL,  EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
		if (hr==DI_OK)
		{
			if( JoyDevice != NULL )
			{
				hr = JoyDevice->SetDataFormat( &c_dfDIJoystick2 );
				if (hr==DI_OK)
				{
					hr = JoyDevice->SetCooperativeLevel( engineHWND, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
					if (hr==DI_OK)
					{
						hr = JoyDevice->EnumObjects( EnumObjectsCallback, NULL, DIDFT_ALL );
						if (hr==DI_OK)
						{
							hr = JoyDevice->Poll(); 
							if (FAILED(hr)) 
							{
								hr = JoyDevice->Acquire();
								while(hr == DIERR_INPUTLOST) 
								{
									hr = JoyDevice->Acquire();
								}
								DIDEVICEINSTANCE joyInfo;
								hr = JoyDevice->GetDeviceInfo(&joyInfo);

								DELog::LogInfo("<li>JoyDevice->Acquire(...): %s", DXGetErrorDescription9(hr));
								joyCreated=true;
							}
						}
						else
						{
							DELog::LogInfo("<li>JoyDevice->EnumObjects(...): %s", DXGetErrorDescription9(hr));
						}
					}
					else
					{
						DELog::LogError("<li>JoyDevice->SetCooperativeLevel(...): %s", DXGetErrorDescription9(hr));
					}
				}
				else
				{
					DELog::LogError("<li>JoyDevice->SetDataFormat(...): %s", DXGetErrorDescription9(hr));
				}	
			}
			else
			{
				DELog::LogWarning("<li>JoyDevice: NULL");
			}
		}
		else
		{
			DELog::LogInfo("<li>JoyDevice->EnumDevices(...): %s", DXGetErrorDescription9(hr));
		}
	
	}
	else
	{
		DELog::LogError("<li>DirectInputObject: NULL");
	}

	if (isCreated)	DELog::LogInfo("<li>Created successfully");	
	else			DELog::LogError("<li>NOT Created");

	return isCreated;
}



DLL_API bool DEInput::Update(bool useKeyboard, bool useMouse, bool useJoy)
{
	if ((useKeyboard)&&(keyboardCreated))
	{
		hr = KeyboardDevice->GetDeviceState(sizeof(m_KeyBuffer),(LPVOID)&m_KeyBuffer);
		if (hr==DIERR_INPUTLOST)
		{
			KeyboardDevice->Acquire();
		}
	}
	 
	if ((useMouse)&&(mouseCreated))
	{
		hr=MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2),(LPVOID)&m_MouseState);
		if (hr==DIERR_INPUTLOST)
		{
			MouseDevice->Acquire();
		}
		//Asse X
		m_fCursorX += m_MouseState.lX * m_fSensitivity;
		if(m_fCursorX < 0)						m_fCursorX = 0;
		else if(m_fCursorX > m_dwScreenWidth)	m_fCursorX = (float)m_dwScreenWidth;
		//Asse Y
		if(m_bInverted)		m_fCursorY -= m_MouseState.lY * m_fSensitivity;
		else				m_fCursorY += m_MouseState.lY * m_fSensitivity;
		if(m_fCursorY < 0)						m_fCursorY = 0;
		else if(m_fCursorY > m_dwScreenHeight)	m_fCursorY = (float)m_dwScreenHeight;
	}

	if ((useJoy)&&(joyCreated))
	{
		hr = JoyDevice->Poll(); 
		if (hr==DIERR_INPUTLOST)
		{
			JoyDevice->Acquire();
		}
		hr = JoyDevice->GetDeviceState(sizeof(DIJOYSTATE2), &m_JoyState);
		if (hr==DIERR_INPUTLOST)
		{
			hr = JoyDevice->Acquire();
		}
	}

	return true;
}


DLL_API bool DEInput::Pressed_Keyboard(int Key, bool loop)
{
	bool ret=false;
	if (keyboardCreated)
	{	
		if (loop)
		{
			if(KEYDOWN(m_KeyBuffer, Key))
			{
				ret=true;
			}
		}
		else
		{
			if(KEYDOWN(m_KeyBuffer, Key) && !KEYDOWN(m_KeyBufferLoop, Key))
			{
				ret=true;
			}
		}
	}

	if (loop==false) m_KeyBufferLoop[Key] = m_KeyBuffer[Key];

	return ret;
}

DLL_API char DEInput::Pressed_KeyboardKey()
{
	char key;
	key = '\0';

	//Lettere
	if(KEYDOWN(m_KeyBuffer, DIK_A) && !KEYDOWN(m_KeyBufferPrev, DIK_A))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'A';
		else
			key = 'a';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_B) && !KEYDOWN(m_KeyBufferPrev, DIK_B))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'B';
		else
			key = 'b';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_C) && !KEYDOWN(m_KeyBufferPrev, DIK_C))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'C';
		else
			key = 'c';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_D) && !KEYDOWN(m_KeyBufferPrev, DIK_D))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'D';
		else
			key = 'd';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_E) && !KEYDOWN(m_KeyBufferPrev, DIK_E))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'E';
		else
			key = 'e';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_F) && !KEYDOWN(m_KeyBufferPrev, DIK_F))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'F';
		else
			key = 'f';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_G) && !KEYDOWN(m_KeyBufferPrev, DIK_G))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'G';
		else
			key = 'g';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_H) && !KEYDOWN(m_KeyBufferPrev, DIK_H))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'H';
		else
			key = 'h';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_I) && !KEYDOWN(m_KeyBufferPrev, DIK_I))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'I';
		else
			key = 'i';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_J) && !KEYDOWN(m_KeyBufferPrev, DIK_J))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'J';
		else
			key = 'j';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_K) && !KEYDOWN(m_KeyBufferPrev, DIK_K))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'K';
		else
			key = 'k';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_L) && !KEYDOWN(m_KeyBufferPrev, DIK_L))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'L';
		else
			key = 'l';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_M) && !KEYDOWN(m_KeyBufferPrev, DIK_M))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'M';
		else
			key = 'm';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_N) && !KEYDOWN(m_KeyBufferPrev, DIK_N))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'N';
		else
			key = 'n';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_O) && !KEYDOWN(m_KeyBufferPrev, DIK_O))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'O';
		else
			key = 'o';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_P) && !KEYDOWN(m_KeyBufferPrev, DIK_P))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'P';
		else
			key = 'p';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_Q) && !KEYDOWN(m_KeyBufferPrev, DIK_Q))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'Q';
		else
			key = 'q';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_R) && !KEYDOWN(m_KeyBufferPrev, DIK_R))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'R';
		else
			key = 'r';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_S) && !KEYDOWN(m_KeyBufferPrev, DIK_S))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'S';
		else
			key = 's';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_T) && !KEYDOWN(m_KeyBufferPrev, DIK_T))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'T';
		else
			key = 't';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_U) && !KEYDOWN(m_KeyBufferPrev, DIK_U))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'U';
		else
			key = 'u';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_V) && !KEYDOWN(m_KeyBufferPrev, DIK_V))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'V';
		else
			key = 'v';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_W) && !KEYDOWN(m_KeyBufferPrev, DIK_W))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'W';
		else
			key = 'w';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_X) && !KEYDOWN(m_KeyBufferPrev, DIK_X))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'X';
		else
			key = 'x';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_Y) && !KEYDOWN(m_KeyBufferPrev, DIK_Y))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'Y';
		else
			key = 'y';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_Z) && !KEYDOWN(m_KeyBufferPrev, DIK_Z))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = 'Z';
		else
			key = 'z';
	}

	//Numeri
	if((KEYDOWN(m_KeyBuffer, DIK_1) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD1)) && !KEYDOWN(m_KeyBufferPrev, DIK_1) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD1))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '!';
		else
			key = '1';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_2) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD2)) && !KEYDOWN(m_KeyBufferPrev, DIK_2) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD2))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '"';
		else
			key = '2';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_3) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD3)) && !KEYDOWN(m_KeyBufferPrev, DIK_3) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD3))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '£';
		else
			key = '3';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_4) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD4)) && !KEYDOWN(m_KeyBufferPrev, DIK_4) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD4))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '$';
		else
			key = '4';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_5) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD5)) && !KEYDOWN(m_KeyBufferPrev, DIK_5) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD5))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '%';
		else
			key = '5';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_6) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD6)) && !KEYDOWN(m_KeyBufferPrev, DIK_6) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD6))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '&';
		else
			key = '6';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_7) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD7)) && !KEYDOWN(m_KeyBufferPrev, DIK_7) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD7))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '/';
		else
			key = '7';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_8) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD8)) && !KEYDOWN(m_KeyBufferPrev, DIK_8) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD8))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '(';
		else
			key = '8';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_9) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD9)) && !KEYDOWN(m_KeyBufferPrev, DIK_9) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD9))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = ')';
		else
			key = '9';
	}
	if((KEYDOWN(m_KeyBuffer, DIK_0) || KEYDOWN(m_KeyBuffer, DIK_NUMPAD0)) && !KEYDOWN(m_KeyBufferPrev, DIK_0) && !KEYDOWN(m_KeyBufferPrev, DIK_NUMPAD0))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '=';
		else
			key = '0';
	}

	//Simboli
	if(KEYDOWN(m_KeyBuffer, DIK_SLASH) && !KEYDOWN(m_KeyBufferPrev, DIK_SLASH))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = '_';
		else
			key = '-';
	}
	if(KEYDOWN(m_KeyBuffer, DIK_SPACE) && !KEYDOWN(m_KeyBufferPrev, DIK_SPACE))
	{
		key = ' ';
	}

	if(KEYDOWN(m_KeyBuffer, DIK_COMMA) && !KEYDOWN(m_KeyBufferPrev, DIK_COMMA))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = ';';
		else
			key = ',';
	}
	
	if(KEYDOWN(m_KeyBuffer,DIK_PERIOD) && !KEYDOWN(m_KeyBufferPrev, DIK_PERIOD))
	{
		if(KEYDOWN(m_KeyBuffer, DIK_LSHIFT) || KEYDOWN(m_KeyBuffer, DIK_RSHIFT))
			key = ':';
		else
			key = '.';
	}

	//Imposta il vecchio valore come quello attuale
	//Permette che se si tiene premuto il pulsante non riscrive la lettere
	memcpy(m_KeyBufferPrev, m_KeyBuffer, 256);

	return key;
}

DLL_API bool DEInput::Pressed_MouseDown(ENUM_DE_MOUSEBUTTON Button)
{
	if (mouseCreated)
	{
		if(m_MouseState.rgbButtons[Button] & 0x80)
		{
			return true;
		}
	}
	return false;
}

DLL_API bool DEInput::Pressed_MouseUp(ENUM_DE_MOUSEBUTTON Button)
{
	if (mouseCreated)
	{
		if(m_MouseState.rgbButtons[Button] & 0x80)
		{
			mouseTemp[Button] = true;
			return false;
		}
		else
		{
			if (mouseTemp[Button] == true)
			{
				mouseTemp[Button] = false;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

DLL_API LONG DEInput::Wheel_Mouse()
{
	if (mouseCreated)
	{
		return m_MouseState.lZ;
	}
	return false;
}

DLL_API bool DEInput::Pressed_Joy(ENUM_DE_JOYBUTTON Button)
{
	if (joyCreated)
	{
		if (Button<=8)
		{
			if(m_JoyState.rgbButtons[Button] & 0x80)
			{
				return true;
			}
		}
		else
		{
			if ((Button==DE_JOYBUTTON_LEFT)&&(m_JoyState.lX<=-1000))	return true;
			if ((Button==DE_JOYBUTTON_RIGHT)&&(m_JoyState.lX>=1000))	return true;
			if ((Button==DE_JOYBUTTON_UP)&&(m_JoyState.lY<=-1000))		return true;
			if ((Button==DE_JOYBUTTON_DOWN)&&(m_JoyState.lY>=1000))		return true;
		}

	}
	return false;
}


DLL_API bool DEInput::RunControlPanel()
{
	if (isCreated)
	{
		HRESULT hr = DirectInputObject->RunControlPanel(NULL, 0);
		if (FAILED(hr))	return false;
		else			return true;
	}
	return false;
}
DLL_API bool DEInput::RunControlPanel_Keyboard()
{
	if (keyboardCreated)
	{
		HRESULT hr = KeyboardDevice->RunControlPanel(NULL, 0);
		if (FAILED(hr))	return false;
		else			return true;
	}
	return false;
}
DLL_API bool DEInput::RunControlPanel_Mouse()
{
	if (mouseCreated)
	{
		HRESULT hr = MouseDevice->RunControlPanel(NULL, 0);
		if (FAILED(hr))	return false;
		else			return true;
	}
	return false;
}
DLL_API bool DEInput::RunControlPanel_Joy()
{
	if (joyCreated)
	{
		HRESULT hr = JoyDevice->RunControlPanel(NULL, 0);
		if (FAILED(hr))	return false;
		else			return true;
	}
	return false;
}

DLL_API LPDIDEVCAPS DEInput::GetCapabilities_Keyboard()
{
	DIDEVCAPS DIDevCaps;
	DIDevCaps.dwSize = sizeof(DIDEVCAPS);
	if (keyboardCreated)
	{
		KeyboardDevice->GetCapabilities(&DIDevCaps);
	}
	return &DIDevCaps;
}
DLL_API LPDIDEVCAPS DEInput::GetCapabilities_Mouse()
{
	DIDEVCAPS DIDevCaps;
	DIDevCaps.dwSize = sizeof(DIDEVCAPS);
	if (mouseCreated)
	{
		MouseDevice->GetCapabilities(&DIDevCaps);
	}
	return &DIDevCaps;
}
DLL_API LPDIDEVCAPS DEInput::GetCapabilities_Joy()
{
	DIDEVCAPS DIDevCaps;
	DIDevCaps.dwSize = sizeof(DIDEVCAPS);
	if (joyCreated)
	{
		JoyDevice->GetCapabilities(&DIDevCaps);
	}
	return &DIDevCaps;
}

DLL_API LPDIDEVICEINSTANCE DEInput::GetDeviceInfo_Keyboard()
{
	DIDEVICEINSTANCE pdidi;
	pdidi.dwSize = sizeof(DIDEVICEINSTANCE);
	if (keyboardCreated)
	{
		KeyboardDevice->GetDeviceInfo(&pdidi);
	}
	return &pdidi;
}
DLL_API LPDIDEVICEINSTANCE DEInput::GetDeviceInfo_Mouse()
{
	DIDEVICEINSTANCE pdidi;
	pdidi.dwSize = sizeof(DIDEVICEINSTANCE);
	if (mouseCreated)
	{
		MouseDevice->GetDeviceInfo(&pdidi);
	}
	return &pdidi;
}
DLL_API LPDIDEVICEINSTANCE DEInput::GetDeviceInfo_Joy()
{
	DIDEVICEINSTANCE pdidi;
	pdidi.dwSize = sizeof(DIDEVICEINSTANCE);
	if (joyCreated)
	{
		JoyDevice->GetDeviceInfo(&pdidi);
	}
	return &pdidi;
}

DLL_API DIMOUSESTATE2 DEInput::GetState_Mouse()
{
	return m_MouseState;
}		
DLL_API DIJOYSTATE2	DEInput::GetState_Joy()
{
	return m_JoyState;
}



DLL_API void DEInput::SetCursorAttributes(bool bInvert,float fNewSensitivity)
{
	m_bInverted = bInvert;
	m_fSensitivity = fNewSensitivity;
}
DLL_API void DEInput::SetCursorPosition(float fNewX,float fNewY)
{
	m_fCursorX = fNewX;
	m_fCursorY = fNewY;
}
DLL_API void DEInput::SetCursorClip(HWND clipHWND)
{
	RECT rcClip;
	GetWindowRect(clipHWND, &rcClip); 
	ClipCursor(&rcClip);  
}
DLL_API void DEInput::ShowWindowCursor(bool showCursor)
{
	ShowCursor(showCursor);
}


DLL_API D3DXVECTOR2 DEInput::GetCursorPosition()
{
	D3DXVECTOR2 ptCursor;
	POINT winCursor;
	GetCursorPos(&winCursor);
	if (mainWindowed)
	{
		ScreenToClient(mainHWND, &winCursor);
		ptCursor.x = (float)winCursor.x;
		ptCursor.y = (float)winCursor.y;	
	}
	else
	{
		ptCursor.x = (float)winCursor.x;
		ptCursor.y = (float)winCursor.y;	
	}
	return ptCursor;
}


DLL_API float DEInput::GetRelativeX_Mouse(void)
{
	return (float)m_MouseState.lX * m_fSensitivity;
}
DLL_API float DEInput::GetRelativeY_Mouse(void)
{
	return (float)m_MouseState.lY * m_fSensitivity;
}
DLL_API float DEInput::GetRelativeZ_Mouse(void)
{
	return (float)m_MouseState.lZ * m_fSensitivity;
}
DLL_API float DEInput::GetAbsoluteX_Mouse(void)
{
	return m_fCursorX;
}
DLL_API float DEInput::GetAbsoluteY_Mouse(void)
{
	return m_fCursorY;
}




DLL_API bool DEInput::GetCreated()
{
	return isCreated;
}
DLL_API bool DEInput::isCreated_Keyboard()
{
	return keyboardCreated;
}
DLL_API bool DEInput::isCreated_Mouse()
{
	return mouseCreated;
}
DLL_API bool DEInput::isCreated_Joy()
{
	return joyCreated;
}




DLL_API HWND DEInput::GetHWND()
{
	return mainHWND;
}
DLL_API void DEInput::SetHWND(HWND cursorHWND)
{
	mainHWND = cursorHWND;
}

DLL_API bool DEInput::GetWindowed()
{
	return mainWindowed;
}
DLL_API void DEInput::SetWindowed(bool inWindow)
{
	mainWindowed = inWindow;
	if (inWindow==false)
	{
		//Clippa e nasconde il cursore
		SetCursorClip(mainHWND);
		ShowWindowCursor(false);
	}
	else
	{
		//Non clippare e mostra il cursore
		SetCursorClip(NULL);
		ShowWindowCursor(true);
	}

}
