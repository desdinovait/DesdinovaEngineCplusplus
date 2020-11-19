#ifndef H_CAMERA_H
#define H_CAMERA_H

/*
[CLASS] 
[
    [NOME] 
    DECamera
    
    [DESCRIZIONE]   
    Classe utilizzata per la gestione della camera(vista) specialmente via mouse.
]*/


typedef struct STRUCT_DE_CAMERAANIMATION
{
	D3DXVECTOR3		*positionPoints;
	D3DXVECTOR3		*viewPoints;
	unsigned int	points;
	float			currentTime;
}STRUCT_DE_CAMERAANIMATION;

class DECamera
{
	private:
		//** Variabili **
		LPCSTR						mainLogTitle;
		LPCSTR						mainTag;
		D3DXMATRIX					matView;
		D3DXVECTOR3					m_vPosition;
		D3DXVECTOR3					m_vView;
		D3DXVECTOR3					m_vUpVector;
		HWND						mainHWND;
		bool						mainWindowed;

		float						walkBias;
		float						walkBiasAngle;
		float						piover180;
		bool						isCreated;
		float						frameInterval;
		
		D3DXVECTOR3					m_vStrafe;
		float						speed;
		float						walkDivider;
		bool						isFPSBased;

		STRUCT_DE_CAMERAANIMATION	animations[256];
		unsigned int				totalAnimations;
		unsigned int				currentAnimation;
		bool						playingAnimation;

		//** Funzioni **
		DLL_API bool				MoveCamera(int windowMiddleX, int windowMiddleY, D3DXVECTOR2 mousePos);

	public:
		//** Funzioni **
		DLL_API						DECamera();
		DLL_API bool				Create(D3DXVECTOR3 positionVector, D3DXVECTOR3 viewVector, D3DXVECTOR3 upVector, float walkCamera, float speedCamera, bool fpsBased=true, LPCSTR logTitle="DECamera");
		DLL_API bool				Invalidate();
		DLL_API bool				Restore();
		DLL_API bool				Release();
		
		DLL_API void				PositionCamera(D3DXVECTOR3 positionVector, D3DXVECTOR3 viewVector, D3DXVECTOR3 upVector);
		DLL_API bool				MoveCameraByMouse(D3DXVECTOR2 mousePos);
		DLL_API void				RotateView(float X, float Y, float Z);
		DLL_API void				StrafeLeft();
		DLL_API void				StrafeRight();
		DLL_API void				MoveDown(bool freeX, bool freeY, bool freeZ);
		DLL_API void				MoveUp(bool freeX, bool freeY, bool freeZ);
		DLL_API void				RotateAroundPoint(D3DXVECTOR3 vCenter, float X, float Y, float Z);
		DLL_API D3DXMATRIX			GetCurrentViewMatrix();
		DLL_API void				UpdateView();
		DLL_API void				UpdateInput();
		DLL_API bool				GetCreated();

		DLL_API inline D3DXVECTOR3	GetPosition();
		DLL_API inline D3DXVECTOR3	GetView();
		DLL_API inline D3DXVECTOR3	GetUpVector();
		DLL_API inline float		GetSpeed();
		DLL_API inline HWND			GetHWND();
		DLL_API inline bool			GetWindowed();

		DLL_API inline void			SetPosition(D3DXVECTOR3 newPosition);
		DLL_API inline void			SetView(D3DXVECTOR3 newView);
		DLL_API inline void			SetUpVector(D3DXVECTOR3 newUpVector);
		DLL_API inline void			SetSpeed(float newSpeed);
		DLL_API inline void			SetHWND(HWND cameraHWND);
		DLL_API inline void			SetWindowed(bool inWindow);

		DLL_API unsigned int		AddAnimation(D3DXVECTOR3 *cameraPositionPoints, D3DXVECTOR3 *cameraViewPoints, unsigned int cameraPoints);
		DLL_API bool				PlayAnimation(unsigned int animationID, float steps);

		//Tag
		DLL_API bool				SetTag(LPCSTR newTag);
		DLL_API LPCSTR				GetTag();





		//Quaternioni
		D3DXQUATERNION				currentQuaternion;
		D3DXQUATERNION				finalQuaternion;
		DLL_API bool				SetYawPitchRoll(float yaw, float pitch, float roll);
		DLL_API D3DXMATRIX			QuaternionToMatrix(D3DXQUATERNION *quat);
		DLL_API D3DXQUATERNION		EulerToQuaternion(float yaw, float pitch, float roll);

		float m_MaxPitchRate;
		float m_MaxHeadingRate;
		float m_HeadingDegrees;
		float m_PitchDegrees;
		float m_MaxForwardVelocity;
		float m_ForwardVelocity;
		D3DXQUATERNION m_qHeading;
		D3DXQUATERNION m_qPitch;
		D3DXVECTOR3 m_DirectionVector;

		DLL_API void ChangeVelocity(float vel);
		DLL_API void ChangeHeading(float degrees);
		DLL_API void ChangePitch(float degrees);
		DLL_API void SetPrespective(void);

};


#endif