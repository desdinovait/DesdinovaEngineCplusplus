#include "DEGeneralIncludes.h"


DLL_API DECamera::DECamera()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	isCreated = false;
	mainHWND = NULL;
	mainWindowed = false;
	totalAnimations = 0;
}

DLL_API bool DECamera::Create(D3DXVECTOR3 positionVector, D3DXVECTOR3 viewVector, D3DXVECTOR3 upVector, float walkCamera, float speedCamera, bool fpsBased, LPCSTR logTitle)
{
	isCreated = false;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{
			walkBias = 0;
			walkBiasAngle = 0;
			walkDivider= 0;
			piover180 = 0.0174532925f;

			PositionCamera(positionVector, viewVector, upVector);
			walkDivider = walkCamera;
			speed = speedCamera;
			isFPSBased = fpsBased;
			isCreated=true;
		}
	}
	catch(...)
	{
		DELog::LogError("<li>%s : Unknown statement error",logTitle);		
	}

	if (!isCreated)	DELog::LogError("<li>%s : NOT Created",logTitle);		

	return isCreated;
}

DLL_API bool DECamera::Invalidate()
{
	try
	{
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DECamera::Restore()
{
	try
	{
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DECamera::Release()
{
	try
	{
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

//Posizione camera
DLL_API void DECamera::PositionCamera(D3DXVECTOR3 positionVector, D3DXVECTOR3 viewVector, D3DXVECTOR3 upVector)
{
	m_vPosition = positionVector;
	m_vView = viewVector;
	m_vUpVector = upVector;
}


//Movimento vista tramite mouse
DLL_API bool DECamera::MoveCameraByMouse(D3DXVECTOR2 mousePos)
{
	if (isCreated)
	{
		//Finestra
		if (mainWindowed)
		{
			RECT area;
			GetWindowRect(mainHWND,&area);

			//Viene riconvertita la posizione del mouse da client a screen
			//perchè la funzione delle direct input fa già il contrario (screen a client)
			POINT winCursor;
			winCursor.x = (LONG)mousePos.x;
			winCursor.y = (LONG)mousePos.y;
			ClientToScreen(mainHWND, &winCursor);
			mousePos.x = (float)winCursor.x;
			mousePos.y = (float)winCursor.y;
			LONG windowWidth = area.right - area.left;
			LONG windowHeight = area.bottom - area.top;
			LONG windowMiddleX = area.left + windowWidth/2;
			LONG windowMiddleY = area.top + windowHeight/2;	
			if (MoveCamera((int)windowMiddleX, (int)windowMiddleY, mousePos))	return true;
			else																return false;
		}
		else	//Fullscreen
		{
			D3DVIEWPORT9 currentViewport;
			DECore::D3DDevice->GetViewport(&currentViewport);
			int middleX = (int)currentViewport.Width/2;	
			int middleY = (int)currentViewport.Height/2;
			if (MoveCamera(middleX, middleY, mousePos))	return true;
			else										return false;
		}//*/
	}

	return false;
}



DLL_API bool DECamera::MoveCamera(int windowMiddleX, int windowMiddleY, D3DXVECTOR2 mousePos)
{
	if (isCreated)
	{
		float deltaY  = 0.0f;							
		float rotateY = 0.0f;

		//If our cursor is still in the middle, we never moved... so don't update the screen
		if( (mousePos.x == windowMiddleX) && (mousePos.y == windowMiddleY) ) return false;

		//Center mouse
		SetCursorPos(windowMiddleX, windowMiddleY);

		//Get the direction the mouse moved in, but bring the number down to a reasonable amount
		rotateY = (float) ( -(windowMiddleX - mousePos.x) ) / 500;		
		deltaY  = (float) ( (windowMiddleY - mousePos.y) ) / 15;

		// Here we just do simple addition to move the camera up and down,
		m_vView.y += deltaY * 2;

		//Here we rotate the view along the X avis depending on the direction (Left of Right)
		RotateView(0, -rotateY, 0);

		return true;
	}
	
	return false;
}


//Rotazione vista
DLL_API void DECamera::RotateView(float X, float Y, float Z)
{
	if (isCreated)
	{
		D3DXVECTOR3 vVector;							// Vector for the position/view.

		// Get our view vector (The direciton we are facing)
		vVector = m_vView - m_vPosition;		// This gets the direction of the X	
		// Rotate the view along the desired axis
		if(X) 
		{
			// Rotate the view vector up or down, then add it to our position
			m_vView.z = (float)(m_vPosition.z + sin(X)*vVector.y + cos(X)*vVector.z);
			m_vView.y = (float)(m_vPosition.y + cos(X)*vVector.y - sin(X)*vVector.z);
		}
		if(Y) 
		{
			// Rotate the view vector right or left, then add it to our position
			m_vView.z = (float)(m_vPosition.z + sin(Y)*vVector.x + cos(Y)*vVector.z);
			m_vView.x = (float)(m_vPosition.x + cos(Y)*vVector.x - sin(Y)*vVector.z);
		}
		if(Z) 
		{
			// Rotate the view vector diagnally right or diagnally down, then add it to our position
			m_vView.x = (float)(m_vPosition.x + sin(Z)*vVector.y + cos(Z)*vVector.x);		
			m_vView.y = (float)(m_vPosition.y + cos(Z)*vVector.y - sin(Z)*vVector.x);
		}
	}
}


//Movimento camera
DLL_API void DECamera::MoveUp(bool freeX, bool freeY, bool freeZ)
{
	if (isCreated)
	{
		//Add current frame value to speed
		float moveSpeed = speed;
		if (isFPSBased)	moveSpeed = moveSpeed * frameInterval;

		D3DXVECTOR3 vVector=D3DXVECTOR3(0,0,0);					// Init a vVector for our view

		// Get our view vector (The direciton we are facing)
		vVector.x = m_vView.x - m_vPosition.x;		// This gets the direction of the X	
		vVector.y = m_vView.y - m_vPosition.y;		// This gets the direction of the Y
		vVector.z = m_vView.z - m_vPosition.z;		// This gets the direction of the Z

		if (freeX)
		{
			m_vPosition.x += vVector.x * moveSpeed;		// Add our acceleration to our position's X
			m_vView.x += vVector.x * moveSpeed;			// Add our acceleration to our view's X
		}

		if (freeY)//Tipo Descent
		{
			m_vView.y += vVector.y * moveSpeed;		// Add our acceleration to our view's y	
			m_vPosition.y += vVector.y * moveSpeed;	// Add our acceleration to our position's y
		}

		if (freeZ)
		{
			m_vPosition.z += vVector.z * moveSpeed;		// Add our acceleration to our position's Z
			m_vView.z += vVector.z * moveSpeed;			// Add our acceleration to our view's Z
		}
	}
}
//Movimento camera
DLL_API void DECamera::MoveDown(bool freeX, bool freeY, bool freeZ)
{
	if (isCreated)
	{
		//Add current frame value to speed
		float moveSpeed = -speed;
		if (isFPSBased)	moveSpeed = moveSpeed * frameInterval;

		D3DXVECTOR3 vVector=D3DXVECTOR3(0,0,0);					// Init a vVector for our view

		// Get our view vector (The direciton we are facing)
		vVector.x = m_vView.x - m_vPosition.x;		// This gets the direction of the X	
		vVector.y = m_vView.y - m_vPosition.y;		// This gets the direction of the Y
		vVector.z = m_vView.z - m_vPosition.z;		// This gets the direction of the Z

		if (freeX)
		{
			m_vPosition.x += vVector.x * moveSpeed;		// Add our acceleration to our position's X
			m_vView.x += vVector.x * moveSpeed;			// Add our acceleration to our view's X
		}

		if (freeY)//Tipo Descent
		{
			m_vView.y += vVector.y * moveSpeed;		// Add our acceleration to our view's y	
			m_vPosition.y += vVector.y * moveSpeed;	// Add our acceleration to our position's y
		}

		if (freeZ)
		{
			m_vPosition.z += vVector.z * moveSpeed;		// Add our acceleration to our position's Z
			m_vView.z += vVector.z * moveSpeed;			// Add our acceleration to our view's Z
		}	
	}
}

//Strafe laterale
DLL_API void DECamera::StrafeRight()
{
	if (isCreated)
	{
		//Add current frame value to speed
		float strafeSpeed = speed;
		if (isFPSBased)	strafeSpeed = strafeSpeed * frameInterval;

		// Strafing is quite simple if you understand what the cross product is.
		// If you have 2 vectors (say the up vVector and the view vVector) you can
		// use the cross product formula to get a vVector that is 90 degrees from the 2 vectors.

		// Initialize a variable for the cross product result
		D3DXVECTOR3 vCross = D3DXVECTOR3(0,0,0);
		// Get the view vVector of our camera and store it in a local variable
		D3DXVECTOR3 vViewVector = D3DXVECTOR3(m_vView.x - m_vPosition.x, m_vView.y - m_vPosition.y, m_vView.z - m_vPosition.z);

		// Here we calculate the cross product of our up vVector and view vVector
		D3DXVec3Cross(&vCross, &m_vUpVector, &vViewVector);
		
		// Add the resultant vVector to our position
		m_vPosition.x += vCross.x * strafeSpeed;
		m_vPosition.z += vCross.z * strafeSpeed;

		// Add the resultant vVector to our view
		m_vView.x += vCross.x * strafeSpeed;
		m_vView.z += vCross.z * strafeSpeed;
	}
}
DLL_API void DECamera::StrafeLeft()
{
	if (isCreated)
	{
		//Add current frame value to speed
		float strafeSpeed = -speed;
		if (isFPSBased)	strafeSpeed = strafeSpeed * frameInterval;

		// Strafing is quite simple if you understand what the cross product is.
		// If you have 2 vectors (say the up vVector and the view vVector) you can
		// use the cross product formula to get a vVector that is 90 degrees from the 2 vectors.

		// Initialize a variable for the cross product result
		D3DXVECTOR3 vCross = D3DXVECTOR3(0,0,0);
		// Get the view vVector of our camera and store it in a local variable
		D3DXVECTOR3 vViewVector = D3DXVECTOR3(m_vView.x - m_vPosition.x, m_vView.y - m_vPosition.y, m_vView.z - m_vPosition.z);

		// Here we calculate the cross product of our up vVector and view vVector
		D3DXVec3Cross(&vCross, &m_vUpVector, &vViewVector);
		
		// Add the resultant vVector to our position
		m_vPosition.x += vCross.x * strafeSpeed;
		m_vPosition.z += vCross.z * strafeSpeed;

		// Add the resultant vVector to our view
		m_vView.x += vCross.x * strafeSpeed;
		m_vView.z += vCross.z * strafeSpeed;
	}
}


//Gira su se stesso
DLL_API void DECamera::RotateAroundPoint(D3DXVECTOR3 vCenter, float X, float Y, float Z)
{
	if (isCreated)
	{
		D3DXVECTOR3 vVector;							// Vector for the position/view.

		// To rotate our position around a point, what we need to do is find
		// a vVector from our position to the center point we will be rotating around.
		// Once we get this vVector then we rotate it along the specified axis with
		// the specified degree.  After that we add it to the center point that we
		// rotated around (vCenter).  That's all it takes.

		// Get the vVector from our position to the center we are rotating around
		vVector.x = m_vPosition.x - vCenter.x;		// This gets the direction of the X	
		vVector.y = m_vPosition.y - vCenter.y;		// This gets the direction of the Y
		vVector.z = m_vPosition.z - vCenter.z;		// This gets the direction of the Z

		// Rotate the position along the desired axis around the desired point vCenter
		if(X) 
		{
			// Rotate the position up or down, then add it to the center point
			m_vPosition.z = (float)(vCenter.z + sin(X)*vVector.y + cos(X)*vVector.z);
			m_vPosition.y = (float)(vCenter.y + cos(X)*vVector.y - sin(X)*vVector.z);
		}
		if(Y) 
		{
			// Rotate the position right or left, then add it to the center point
			m_vPosition.z = (float)(vCenter.z + sin(Y)*vVector.x + cos(Y)*vVector.z);
			m_vPosition.x = (float)(vCenter.x + cos(Y)*vVector.x - sin(Y)*vVector.z);
		}
		if(Z) 
		{
			// Rotate the position diagnally right or diagnally down, then add it to the center point
			m_vPosition.x = (float)(vCenter.x + sin(Z)*vVector.y + cos(Z)*vVector.x);		
			m_vPosition.y = (float)(vCenter.y + cos(Z)*vVector.y - sin(Z)*vVector.x);
		}
	}
}

DLL_API void DECamera::UpdateInput()
{
	if (isFPSBased)
	{
		static float lastFrameTime = 0.0f;// Last frame's time.
		// Get the time in milliseconds (we convert to second by * 0.001).
		float currentTime = timeGetTime() * 0.001f;
		// Calculate the frame interval by the current time minus the last frame's time.
		frameInterval = currentTime - lastFrameTime;
		lastFrameTime = currentTime;
	}
}

DLL_API void DECamera::UpdateView()
{
	D3DXMatrixLookAtLH(&matView, &m_vPosition, &m_vView, &m_vUpVector);	
	DECore::D3DDevice->SetTransform(D3DTS_VIEW,	&matView);
}

DLL_API D3DXMATRIX DECamera::GetCurrentViewMatrix()
{
	return matView;
}

DLL_API bool DECamera::GetCreated()
{
	return isCreated;
}

DLL_API bool DECamera::SetYawPitchRoll(float yaw, float pitch, float roll)
{
	if (isCreated)
	{
		currentQuaternion = EulerToQuaternion(yaw, pitch, roll);
		finalQuaternion = finalQuaternion * currentQuaternion;
		return true;
	}
	return false;
}




DLL_API D3DXMATRIX DECamera::QuaternionToMatrix(D3DXQUATERNION *quat)
{
	D3DXMATRIX finalMatrix;
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	// calculate coefficients used for building the matrix
	x2 = quat->x + quat->x; y2 = quat->y + quat->y;
	z2 = quat->z + quat->z;
	xx = quat->x * x2; xy = quat->x * y2; xz = quat->x * z2;
	yy = quat->y * y2; yz = quat->y * z2; zz = quat->z * z2;
	wx = quat->w * x2; wy = quat->w * y2; wz = quat->w * z2;

	// fill in matrix positions with them
	finalMatrix._11 = 1.0f - (yy + zz); finalMatrix._21 = xy - wz;
	finalMatrix._31 = xz + wy; finalMatrix._41 = 0.0;
	finalMatrix._12 = xy + wz; finalMatrix._22 = 1.0f - (xx + zz);
	finalMatrix._32 = yz - wx; finalMatrix._42 = 0.0;
	finalMatrix._13 = xz - wy; finalMatrix._23 = yz + wx;
	finalMatrix._33 = 1.0f - (xx + yy); finalMatrix._43 = 0.0;
	finalMatrix._14 = 0; finalMatrix._24 = 0;
	finalMatrix._34 = 0; finalMatrix._44 = 1;

	return finalMatrix;
}


DLL_API D3DXQUATERNION DECamera::EulerToQuaternion(float yaw, float pitch, float roll)
{
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, yaw, pitch, roll);
	return quat;
}


DLL_API inline D3DXVECTOR3 DECamera::GetPosition()
{
	return m_vPosition;
}
DLL_API inline D3DXVECTOR3 DECamera::GetView()
{
	return m_vView;
}
DLL_API inline D3DXVECTOR3 DECamera::GetUpVector()
{
	return m_vUpVector;
}
DLL_API inline float DECamera::GetSpeed()
{
	return speed;
}
DLL_API inline HWND DECamera::GetHWND()
{
	return mainHWND;
}
DLL_API inline bool DECamera::GetWindowed()
{
	return mainWindowed;
}

DLL_API inline void DECamera::SetPosition(D3DXVECTOR3 newPosition)
{
	m_vPosition = newPosition;
}
DLL_API inline void DECamera::SetView(D3DXVECTOR3 newView)
{
	m_vView = newView;
}
DLL_API inline void DECamera::SetUpVector(D3DXVECTOR3 newUpVector)
{
	m_vUpVector = newUpVector;
}
DLL_API inline void DECamera::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}
DLL_API inline void DECamera::SetHWND(HWND cameraHWND)
{
	mainHWND = cameraHWND;
}
DLL_API inline void DECamera::SetWindowed(bool inWindow)
{
	mainWindowed = inWindow;
}

DLL_API unsigned int DECamera::AddAnimation(D3DXVECTOR3 *cameraPositionPoints, D3DXVECTOR3 *cameraViewPoints, unsigned int cameraPoints)
{
	animations[totalAnimations].positionPoints = (D3DXVECTOR3 *)calloc(cameraPoints,sizeof(D3DXVECTOR3));
	animations[totalAnimations].viewPoints=(D3DXVECTOR3 *)calloc(cameraPoints,sizeof(D3DXVECTOR3));

	for (unsigned int i=0; i< cameraPoints; i++)
	{
		animations[totalAnimations].positionPoints[i].x = cameraPositionPoints[i].x;
		animations[totalAnimations].positionPoints[i].y = cameraPositionPoints[i].y;
		animations[totalAnimations].positionPoints[i].z = cameraPositionPoints[i].z;

		animations[totalAnimations].viewPoints[i].x = cameraViewPoints[i].x;
		animations[totalAnimations].viewPoints[i].y = cameraViewPoints[i].y;
		animations[totalAnimations].viewPoints[i].z = cameraViewPoints[i].z;
	}

	animations[totalAnimations].points = cameraPoints;
	animations[totalAnimations].currentTime = 0;
	totalAnimations = totalAnimations + 1;
	return totalAnimations-1;	//Ritorna l'animazione aggiunta
}

DLL_API bool DECamera::PlayAnimation(unsigned int animationID, float steps)
{
	currentAnimation = animationID;
	//if (playingAnimation)
	{
		animations[currentAnimation].currentTime = animations[currentAnimation].currentTime + 1;
		float t = (float)animations[currentAnimation].currentTime / (float)steps;
		if (t==1.0f)
		{
			animations[currentAnimation].currentTime = 0;
			//playingAnimation = false;
		}
		//Setta la nuova vista della camera
		SetPosition(DEUtility::PointOnBezierCurveN(animations[currentAnimation].positionPoints, animations[currentAnimation].points, t));
		SetView(DEUtility::PointOnBezierCurveN(animations[currentAnimation].viewPoints, animations[currentAnimation].points, t));
		return true;
	}
	return false;
}

DLL_API bool DECamera::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DECamera::GetTag()
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
}//*/









































/*/////////////////////////////////////////////////////////////////////////////////////////////////////






DLL_API void DECamera::SetPrespective()
{
	m_MaxForwardVelocity = 5.0f;
	m_MaxPitchRate = 5.0f;
	m_MaxHeadingRate = 5.0f;

	D3DXMATRIX Matrix;
	D3DXQUATERNION q;

	//Make the Quaternions that will represent our rotations
	D3DXQuaternionRotationYawPitchRoll(&m_qHeading, D3DXToRadian(m_HeadingDegrees), 0, 0);
	D3DXQuaternionRotationYawPitchRoll(&m_qPitch, 0, D3DXToRadian(m_PitchDegrees), 0);
	
	//Combine the pitch and heading rotations and store the results in q
	D3DXQuaternionMultiply(&q, &m_qPitch, &m_qHeading);
	//Matrix = QuaternionToMatrix(&q);
	D3DXMatrixTransformation(&Matrix,NULL,NULL,NULL,NULL,&q, NULL);

	//Let OpenGL set our new prespective on the world!
	DECore::D3DDevice->SetTransform(D3DTS_VIEW,	&Matrix);

	//Create a matrix from the pitch Quaternion and get the j vector for our direction.
	Matrix = QuaternionToMatrix(&m_qPitch);
	m_DirectionVector.y = Matrix._42;

	//Combine the heading and pitch rotations and make a matrix to get the i and j vectors for our direction.
	D3DXQuaternionMultiply(&q, &m_qHeading, &m_qPitch);
	Matrix = QuaternionToMatrix(&q);

	m_DirectionVector.x = Matrix._41;
	m_DirectionVector.z = Matrix._43;

	// Scale the direction by our speed.
	m_DirectionVector *= m_ForwardVelocity;

	// Increment our position by the vector
	m_vPosition.x += m_DirectionVector.x;
	m_vPosition.y += m_DirectionVector.y;
	m_vPosition.z += m_DirectionVector.z;
}


DLL_API void DECamera::ChangePitch(float degrees)
{
	if(fabs(degrees) < fabs(m_MaxPitchRate))
	{
		// Our pitch is less than the max pitch rate that we 
		// defined so lets increment it.
		m_PitchDegrees += degrees;
	}
	else
	{
		// Our pitch is greater than the max pitch rate that
		// we defined so we can only increment our pitch by the 
		// maximum allowed value.
		if(degrees < 0)
		{
			// We are pitching down so decrement
			m_PitchDegrees -= m_MaxPitchRate;
		}
		else
		{
			// We are pitching up so increment
			m_PitchDegrees += m_MaxPitchRate;
		}
	}

	// We don't want our pitch to run away from us. Although it
	// really doesn't matter I prefer to have my pitch degrees
	// within the range of -360.0f to 360.0f
	if(m_PitchDegrees > 360.0f)
	{
		m_PitchDegrees -= 360.0f;
	}
	else if(m_PitchDegrees < -360.0f)
	{
		m_PitchDegrees += 360.0f;
	}
}

DLL_API void DECamera::ChangeHeading(float degrees)
{
	if(fabs(degrees) < fabs(m_MaxHeadingRate))
	{
		// Our Heading is less than the max heading rate that we 
		// defined so lets increment it but first we must check
		// to see if we are inverted so that our heading will not
		// become inverted.
		if(m_PitchDegrees > 90 && m_PitchDegrees < 270 || (m_PitchDegrees < -90 && m_PitchDegrees > -270))
		{
			m_HeadingDegrees -= degrees;
		}
		else
		{
			m_HeadingDegrees += degrees;
		}
	}
	else
	{
		// Our heading is greater than the max heading rate that
		// we defined so we can only increment our heading by the 
		// maximum allowed value.
		if(degrees < 0)
		{
			// Check to see if we are upside down.
			if((m_PitchDegrees > 90 && m_PitchDegrees < 270) || (m_PitchDegrees < -90 && m_PitchDegrees > -270))
			{
				// Ok we would normally decrement here but since we are upside
				// down then we need to increment our heading
				m_HeadingDegrees += m_MaxHeadingRate;
			}
			else
			{
				// We are not upside down so decrement as usual
				m_HeadingDegrees -= m_MaxHeadingRate;
			}
		}
		else
		{
			// Check to see if we are upside down.
			if(m_PitchDegrees > 90 && m_PitchDegrees < 270 || (m_PitchDegrees < -90 && m_PitchDegrees > -270))
			{
				// Ok we would normally increment here but since we are upside
				// down then we need to decrement our heading.
				m_HeadingDegrees -= m_MaxHeadingRate;
			}
			else
			{
				// We are not upside down so increment as usual.
				m_HeadingDegrees += m_MaxHeadingRate;
			}
		}
	}
	
	// We don't want our heading to run away from us either. Although it
	// really doesn't matter I prefer to have my heading degrees
	// within the range of -360.0f to 360.0f
	if(m_HeadingDegrees > 360.0f)
	{
		m_HeadingDegrees -= 360.0f;
	}
	else if(m_HeadingDegrees < -360.0f)
	{
		m_HeadingDegrees += 360.0f;
	}
}

DLL_API void DECamera::ChangeVelocity(float vel)
{
	if(fabs(vel) < fabs(m_MaxForwardVelocity))
	{
		// Our velocity is less than the max velocity increment that we 
		// defined so lets increment it.
		m_ForwardVelocity += vel;
	}
	else
	{
		// Our velocity is greater than the max velocity increment that
		// we defined so we can only increment our velocity by the 
		// maximum allowed value.
		if(vel < 0)
		{
			// We are slowing down so decrement
			m_ForwardVelocity -= -m_MaxForwardVelocity;
		}
		else
		{
			// We are speeding up so increment
			m_ForwardVelocity += m_MaxForwardVelocity;
		}
	}
}


/*
if(Input.Pressed_Keyboard(DIK_D))	Scene1_Camera.ChangeHeading(-1.0f);
if(Input.Pressed_Keyboard(DIK_A))	Scene1_Camera.ChangeHeading(1.0f);
if(Input.Pressed_Keyboard(DIK_S))	Scene1_Camera.ChangePitch(-1.0f);
if(Input.Pressed_Keyboard(DIK_W))	Scene1_Camera.ChangePitch(1.0f);

if(Input.Pressed_Keyboard(DIK_R)) 	Scene1_Camera.ChangeVelocity(1.0f);
if(Input.Pressed_Keyboard(DIK_F)) 	Scene1_Camera.ChangeVelocity(-1.0f);

float DeltaMouse;
POINT pt;
GetCursorPos(&pt);
MouseX = pt.x;
MouseY = pt.y;

if(MouseX < CenterX)
{
	DeltaMouse = float(CenterX - MouseX);
	Scene1_Camera.ChangeHeading(0.2f * DeltaMouse);	
}
else if(MouseX > CenterX)
{
	DeltaMouse = float(MouseX - CenterX);
	Scene1_Camera.ChangeHeading(-0.2f * DeltaMouse);
}
if(MouseY < CenterY)
{
	DeltaMouse = float(CenterY - MouseY);
	Scene1_Camera.ChangePitch(-0.2f * DeltaMouse);
}
else if(MouseY > CenterY)
{
	DeltaMouse = float(MouseY - CenterY);
	Scene1_Camera.ChangePitch(0.2f * DeltaMouse);
}

MouseX = CenterX;
MouseY = CenterY;
SetCursorPos(CenterX, CenterY);
Scene1_Camera.SetPrespective();//*/
