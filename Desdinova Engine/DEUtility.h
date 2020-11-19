#ifndef H_UTILITY_H
#define H_UTILITY_H

/*
[CLASS] 
[
    [NOME] 
    DEUtility
    
    [DESCRIZIONE] 
	Classe statica utilizzata per funzioni di utilità generale
]
*/
class DEUtility
{
	public:
		//Conversioni
		DLL_API static FLOAT DWtoF( DWORD d ) ;
		DLL_API static DWORD FtoDW( FLOAT f );

		//Occupazione Memoria
		DLL_API static long GetTotalMemory();
		DLL_API static long GetAvailableMemory();
		DLL_API static long GetMemoryUsage();
		DLL_API static long GetCurrentProcessMemoryUsage();
		DLL_API static long GetProcessMemoryUsage(DWORD processID);

		//Info path
		DLL_API static char *GetCurrentPath();
		DLL_API static bool InRoot(const char *completeFilename);
		DLL_API static char *NoRoot(const char *completeFilename);

		//Conversioni
		DLL_API static HRESULT ConvertAnsiStringToWideCch( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar );
		DLL_API static HRESULT ConvertStringToWideCch( WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar );

		DLL_API static int HashString( const char* str );
		DLL_API static int HashString( const std::string& s );
		DLL_API static void TokenizeString( const char* str, std::vector<std::string>& tokens, const char* delimiters, bool bIgnoreQuotes );

		//Cube mapping matrix
		DLL_API static D3DXMATRIX GetCubeMapViewMatrix(DWORD dwFace, D3DXVECTOR3 Position);

		//FOV Clip Planes
		DLL_API static STRUCT_DE_FOV SetupFOVClipPlanes( float Angle , float FrontClip , float BackClip, float AspectRatio );

		//Funzioni ToString
		DLL_API static char* VertexProcessingToString( const DWORD& vp );
		DLL_API static char* DeviceTypeToString( const D3DDEVTYPE& dt );
		DLL_API static char* D3DFormatToString( const D3DFORMAT& fmt );
		DLL_API static char* MultisampleTypeToString( const D3DMULTISAMPLE_TYPE& ms );
		DLL_API static char* PresentIntervalToString( const UINT& pi );

		//Funzioni per ricavare i bit di un formato
		DLL_API static UINT GetStencilBits( const D3DFORMAT& fmt);
		DLL_API static UINT GetDepthBits( const D3DFORMAT& fmt );
		DLL_API static UINT GetBitsPerPixel( const D3DFORMAT& fmt );

		//Generazioni numeri random
		DLL_API static float GetRandomMinMax( float fMin, float fMax );
		DLL_API static D3DXVECTOR3 GetRandomVector(float fMin, float fMax);
		DLL_API static bool GetRandomBool();

		//Genera un piano da 3 punti
		DLL_API static void PlaneFromPoints(D3DXVECTOR3 *pP0 , D3DXVECTOR3 *pP1 , D3DXVECTOR3 *pP2 , D3DXVECTOR4 *pPlane);

		//Curva Bezier
		DLL_API static D3DXVECTOR3 PointOnBezierCurve3(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, float t);
		DLL_API static D3DXVECTOR3 PointOnBezierCurve4(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4, float t);
		DLL_API static D3DXVECTOR3 PointOnBezierCurveN(D3DXVECTOR3 *p, int n, double t);
		
		//Interpolazione lineare
		//DLL_API static D3DXVECTOR3 PointOnLine(D3DXVECTOR3 *P, int totalPoints, float steps);
		DLL_API static D3DXVECTOR3 PointOnLine(D3DXVECTOR4 *P, int totalPoints, float velocity);
		static int currentPoint;
		static float currentTime;

		//Collisioni e test
		DLL_API static bool RayIntersectsSphere2D( D3DXVECTOR3 orig, D3DXVECTOR3 dir, D3DXVECTOR3 center, float radius );
		DLL_API static bool RayIntersectsTriangle( const D3DXVECTOR3& orig,const D3DXVECTOR3& dir, D3DXVECTOR3& v0,D3DXVECTOR3& v1, D3DXVECTOR3& v2,FLOAT* t, FLOAT* u, FLOAT* v );
		DLL_API static bool PointInSphere(D3DXVECTOR3 &point, D3DXVECTOR3 &center,float centerRadius);
		DLL_API static bool SphereInSphere(D3DXVECTOR3 &point, float pointRadius, D3DXVECTOR3 &center,float centerRadius);
		DLL_API static bool PointInAABB(D3DXVECTOR3 &point, D3DXVECTOR3 &boundMin, D3DXVECTOR3 &boundMax);

		//Culling
		DLL_API static bool FOVCheck(D3DXVECTOR3 position, float sphereRadius, D3DXMATRIX viewTransform, STRUCT_DE_FOV fovProperties, D3DXVECTOR3 *pViewPos);

		DLL_API static DWORD VectorToRGB(D3DXVECTOR3* NormalVector);

		//Versione DirectX
		DLL_API static HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );
		
		//Funzioni matematiche sui vettori
		DLL_API static D3DXVECTOR3 Vector(D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2);
		DLL_API static D3DXVECTOR3 AddVector(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2);
		DLL_API static D3DXVECTOR3 DivideVectorByScaler(D3DXVECTOR3 vVector1, float Scaler);
		DLL_API static D3DXVECTOR3 Normalize(D3DXVECTOR3 vNormal);
		DLL_API static D3DXVECTOR3 GetTriangleNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3);
		
		//Versione file e conversioni
		DLL_API static HRESULT			GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion );
		DLL_API static ULARGE_INTEGER	MakeInt64( WORD a, WORD b, WORD c, WORD d );
		DLL_API static int				CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 );

	private:
		//Magnitudine dei una normale (Magnitude = sqrt(x^2 + y^2 + z^2))
		#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))
		
		//Versione DirectX
		DLL_API static HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
		DLL_API static HRESULT GetDirectXVersionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
};

#endif