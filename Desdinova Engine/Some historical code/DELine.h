#ifndef H_LINE_H
#define H_LINE_H

/*
[CLASS] 
[
    [NOME] 
    DELine
    
    [DESCRIZIONE]   
    Classe utilizzata per la creazione linee
]
*/
class DELines2D
{
	private:
		struct LINE_NODE
		{
			int					nLineID;
			D3DCOLOR			Color;
			float				fWidth;
			bool				bAntiAlias;
			float				fScaleRatio;
			D3DXVECTOR2*		pVertexList;
			DWORD				dwVertexListCount;
		};

		vector<LINE_NODE*>		m_LinesList;
		ID3DXLine*				m_pD3DXLine;
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;
		bool					isCreated;


	public:
		DLL_API					DELines2D();
		DLL_API bool			Create(LPCSTR logTitle="DEGrid");
		DLL_API bool			Render();
		DLL_API bool			AddLine( int* pnLineID, D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR Color, float fWidth, float fScaleRatio, bool bAntiAlias );
		DLL_API bool			AddRect( int* pnLineID, RECT rc, D3DCOLOR Color, float fWidth, float fScaleRatio, bool bAntiAlias );
		DLL_API bool			GetCreated();
		DLL_API bool			Invalidate();
		DLL_API bool			Restore();
		DLL_API bool			Release();
		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();
};



#endif