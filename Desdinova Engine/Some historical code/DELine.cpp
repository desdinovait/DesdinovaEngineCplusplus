#include "DEGeneralIncludes.h"
#include "DELine.h"

DLL_API DELines2D::DELines2D()
{
	//mainLogTitle = NULL;
	//mainTag = NULL;
	//isCreated = false;
}

//--------------------------------------------------------------------------------------
DLL_API bool DELines2D::Create(LPCSTR logTitle)
{
	HRESULT hr=NULL;
	mainLogTitle = logTitle;

	try
	{
		if (DECore::D3DDevice!=NULL)
		{	
			hr = D3DXCreateLine( DECore::D3DDevice, &m_pD3DXLine );
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



//--------------------------------------------------------------------------------------
DLL_API bool DELines2D::Render()
{
    HRESULT hr;
    bool bDrawingHasBegun = false;
    float fLastWidth = 0.0f;
    bool bLastAntiAlias = false;
    
    for( int i=0; i<m_LinesList.size(); i++ )
    {
        LINE_NODE* pLineNode = m_LinesList.at(i);
        if( pLineNode )
        {
            if( !bDrawingHasBegun || 
                fLastWidth != pLineNode->fWidth || 
                bLastAntiAlias != pLineNode->bAntiAlias )
            {
                if( bDrawingHasBegun )
                {
                    hr = m_pD3DXLine->End();
                    if( FAILED(hr) )
                        return hr;
                }

                m_pD3DXLine->SetWidth( pLineNode->fWidth );
                m_pD3DXLine->SetAntialias( pLineNode->bAntiAlias );

                fLastWidth = pLineNode->fWidth;
                bLastAntiAlias = pLineNode->bAntiAlias;

                hr = m_pD3DXLine->Begin();
                if( FAILED(hr) )
                    return hr;
                bDrawingHasBegun = true;
            }

            hr = m_pD3DXLine->Draw( pLineNode->pVertexList, pLineNode->dwVertexListCount, pLineNode->Color );
            if( FAILED(hr) )
                return hr;
        }
    }

    if( bDrawingHasBegun )
    {
        hr = m_pD3DXLine->End();
        if( FAILED(hr) )
            return hr;
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
DLL_API bool DELines2D::AddLine( int* pnLineID, D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR Color, float fWidth, float fScaleRatio, bool bAntiAlias )
{
    if( pVertexList == NULL || dwVertexListCount == 0 )
        return false;

    LINE_NODE* pLineNode = new LINE_NODE;
    if( pLineNode == NULL )
        return false;
    ZeroMemory( pLineNode, sizeof(LINE_NODE) );

    pLineNode->nLineID = m_LinesList.size();
    pLineNode->Color = Color;
    pLineNode->fWidth = fWidth;
    pLineNode->bAntiAlias = bAntiAlias;
    pLineNode->dwVertexListCount = dwVertexListCount;

    if( pnLineID )
        *pnLineID = pLineNode->nLineID;

    pLineNode->pVertexList = new D3DXVECTOR2[dwVertexListCount];
    if( pLineNode->pVertexList == NULL )
    {
        delete pLineNode;
        return false;
    }
    for( DWORD i=0; i<dwVertexListCount; i++ )
    {
        pLineNode->pVertexList[i] = pVertexList[i] * fScaleRatio;
    }

    m_LinesList.push_back( pLineNode );

    return true;
}


//--------------------------------------------------------------------------------------
DLL_API bool DELines2D::AddRect( int* pnLineID, RECT rc, D3DCOLOR Color, float fWidth, float fScaleRatio, bool bAntiAlias )
{
    if( fWidth > 2.0f )
    {
        D3DXVECTOR2 vertexList[8];

        vertexList[0].x = (float)rc.left;
        vertexList[0].y = (float)rc.top - (fWidth/2.0f);

        vertexList[1].x = (float)rc.left;
        vertexList[1].y = (float)rc.bottom + (fWidth/2.0f);

        vertexList[2].x = (float)rc.left;
        vertexList[2].y = (float)rc.bottom - 0.5f;

        vertexList[3].x = (float)rc.right;
        vertexList[3].y = (float)rc.bottom - 0.5f;

        vertexList[4].x = (float)rc.right;
        vertexList[4].y = (float)rc.bottom + (fWidth/2.0f);

        vertexList[5].x = (float)rc.right;
        vertexList[5].y = (float)rc.top - (fWidth/2.0f);

        vertexList[6].x = (float)rc.right;
        vertexList[6].y = (float)rc.top;

        vertexList[7].x = (float)rc.left;
        vertexList[7].y = (float)rc.top;
        
        return AddLine( pnLineID, vertexList, 8, Color, fWidth, fScaleRatio, bAntiAlias );
    }
    else
    {
        D3DXVECTOR2 vertexList[5];
        vertexList[0].x = (float)rc.left;
        vertexList[0].y = (float)rc.top;

        vertexList[1].x = (float)rc.left;
        vertexList[1].y = (float)rc.bottom;

        vertexList[2].x = (float)rc.right;
        vertexList[2].y = (float)rc.bottom;

        vertexList[3].x = (float)rc.right;
        vertexList[3].y = (float)rc.top;

        vertexList[4].x = (float)rc.left;
        vertexList[4].y = (float)rc.top;
        
        return AddLine( pnLineID, vertexList, 5, Color, fWidth, fScaleRatio, bAntiAlias );
    }
}


DLL_API bool DELines2D::GetCreated()
{
	return isCreated;
}

DLL_API bool DELines2D::Invalidate()
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

DLL_API bool DELines2D::Restore()
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

DLL_API bool DELines2D::Release()
{
	try
	{
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);		
		return false;
	}
	return false;
}

DLL_API bool DELines2D::SetTag(LPCSTR newTag)
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

DLL_API LPCSTR DELines2D::GetTag()
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



/*--------------------------------------------------------------------------------------
HRESULT DELine::RemoveLine( int nLineID )
{
    for( int i=0; i<m_LinesList.GetSize(); i++ )
    {
        LINE_NODE* pLineNode = m_LinesList.GetAt(i);
        if( pLineNode && pLineNode->nLineID == nLineID )
        {
            SAFE_DELETE_ARRAY( pLineNode->pVertexList );
            delete pLineNode;
            m_LinesList.SetAt(i, NULL);
        }
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT DELine::RemoveAllLines()
{
    for( int i=0; i<m_LinesList.GetSize(); i++ )
    {
        LINE_NODE* pLineNode = m_LinesList.GetAt(i);
        if( pLineNode )
        {
            SAFE_DELETE_ARRAY( pLineNode->pVertexList );
            delete pLineNode;
        }
    }
    m_LinesList.RemoveAll();

    return S_OK;
}*/

