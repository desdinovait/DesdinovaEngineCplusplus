#ifndef H_XML_H
#define H_XML_H

class DEXML
{
	private:
		IXMLDOMDocument *pXMLDom;
		IXMLDOMNodeList	*nodeList;
		IXMLDOMNode		*nodeChild;
		VARIANT			var;
		VARIANT			VariantString(BSTR str);

	public:
		DLL_API bool	Init();
		DLL_API bool	LoadFile(BSTR filename);
		DLL_API bool	Release();
		//Nodes
		DLL_API char	*ReadNode_String(BSTR nodeName, long nodeNum=0);
		DLL_API int		ReadNode_Int(BSTR nodeName, long nodeNum=0);
		DLL_API float	ReadNode_Float(BSTR nodeName, long nodeNum);
		DLL_API bool	ReadNode_Bool(BSTR nodeName, long nodeNum=0);
		DLL_API long	ReadNodes_Lenght(BSTR nodeName);
		//Attributes
		DLL_API char	*ReadAttribute_String(BSTR nodeName, long nodeNum=0, long attribNum=0);
		DLL_API int		ReadAttribute_Int(BSTR nodeName, long nodeNum=0, long attribNum=0);
		DLL_API float	ReadAttribute_Float(BSTR nodeName, long nodeNum=0, long attribNum=0);
		DLL_API bool	ReadAttribute_Bool(BSTR nodeName, long nodeNum=0, long attribNum=0);
		DLL_API long	ReadAttributes_Lenght(BSTR nodeName, long nodeNum=0);
};

#endif