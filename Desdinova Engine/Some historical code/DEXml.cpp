#include "DEGeneralIncludes.h"
#include "DEXml.h"

VARIANT DEXML::VariantString(BSTR str)
{
	VARIANT var;
	VariantInit(&var);
	V_BSTR(&var) = SysAllocString(str);
	V_VT(&var) = VT_BSTR;
	return var;
}


DLL_API bool DEXML::Init()
{
	try
	{
		//Inizializzazione XMLDOM
		CoCreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER,  __uuidof(IXMLDOMDocument), (void**)&pXMLDom);
		if (!pXMLDom)
		{
			Release();
			return false;
		}
		else
		{
			pXMLDom->put_async(VARIANT_FALSE);
			pXMLDom->put_validateOnParse(VARIANT_FALSE);
			pXMLDom->put_resolveExternals(VARIANT_FALSE);
			return true;
		}
	}
	catch(...)
	{
		Release();
		return false;
	}
	Release();
	return false;
}

DLL_API bool DEXML::LoadFile(BSTR filename)
{
	try
	{
		//Nome file
		VARIANT var;
		VariantInit(&var);
		V_BSTR(&var) = SysAllocString(filename);
		V_VT(&var) = VT_BSTR;

		//Caricamento da file
		VARIANT_BOOL	status;
		pXMLDom->load(var, &status);
		if (status!=VARIANT_TRUE) 
		{
			VariantClear(&var);
			Release();
			return false;
		}
		else
		{
			VariantClear(&var);
			return true;
		}
	}
	catch(...)
	{
		Release();
		return false;
	}
	Release();
	return false;
}




DLL_API char *DEXML::ReadNode_String(BSTR nodeName, long nodeNum)
{
	char Name[MAX_PATH];
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (!nodeList) 
	{
		strcpy(Name, "");
	}
	else 
	{
		unsigned int lenght=0;
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			nodeList->get_item(nodeNum, &nodeChild);
			BSTR text;
			nodeChild->get_text(&text);
			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1, Name, SysStringLen(text)+1, NULL, NULL );
		}
		else
		{
			strcpy(Name, "");
		}
	}
	return Name;
}


DLL_API int DEXML::ReadNode_Int(BSTR nodeName, long nodeNum)
{
	char Master[MAX_PATH];
	int	valueInt=0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (!nodeList) 
	{
		valueInt = 0;
	}
	else 
	{
		unsigned int lenght=0;
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			nodeList->get_item(nodeNum, &nodeChild);
			BSTR text;
			nodeChild->get_text(&text);
			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1, Master, SysStringLen(text)+1, NULL, NULL );
			valueInt = atoi(Master);
		}
		else
		{
			valueInt = 0;
		}
	}
	return valueInt;
}



DLL_API float DEXML::ReadNode_Float(BSTR nodeName, long nodeNum)
{
	char Master[MAX_PATH];
	float	valueFloat=0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (!nodeList) 
	{
		valueFloat = 0.0f;
	}
	else 
	{
		unsigned int lenght=0;
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			nodeList->get_item(nodeNum, &nodeChild);
			BSTR text;
			nodeChild->get_text(&text);
			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1, Master, SysStringLen(text)+1, NULL, NULL );
			valueFloat = atof(Master);
		}
		else
		{
			valueFloat = 0.0f;
		}
	}
	return valueFloat;
}



DLL_API bool DEXML::ReadNode_Bool(BSTR nodeName, long nodeNum)
{
	char Master[MAX_PATH];
	bool valueBool=false;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (!nodeList) 
	{
		valueBool = false;
	}
	else 
	{
		unsigned int lenght=0;
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			nodeList->get_item(nodeNum, &nodeChild);
			BSTR text;
			nodeChild->get_text(&text);
			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1, Master, SysStringLen(text)+1, NULL, NULL );
			int result1 = strncmp( (const char *)(strdup(Master)), "1" , strlen(Master) );
			if (result1 == 0)	//Uguali
			{
				valueBool = true;
			}
			else
			{
				//Forse l'utente ha scritto "true" invece di 1 ?!? Proviamo...
				int result2 = strncmp( (const char *)strlwr(strdup(Master)), "true" , strlen(Master) );
				if (result2 == 0)//Uguali
				{
					valueBool = true;
				}
				else
				{
					valueBool = false;
				}
			}
		}
		else
		{
			valueBool = false;
		}
	}
	return valueBool;
}


DLL_API long DEXML::ReadNodes_Lenght(BSTR nodeName)
{
	//Lenght
	IXMLDOMNodeList		*nodeList = NULL;
	long		 		lengthList=0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (!nodeList) 
	{
		lengthList = 0;
	}
	else 
	{
		nodeList->get_length(&lengthList);
	}
	if (nodeList)
	{
		nodeList->Release();
		nodeList = NULL;
	}
	return lengthList;
}













DLL_API char *DEXML::ReadAttribute_String(BSTR nodeName, long nodeNum, long attribNum)
{
	char Name[MAX_PATH];
	long lengthAttrib = 0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (nodeList) 
	{
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			//Preleva gli Attributi
			IXMLDOMNamedNodeMap *attrib = NULL;
			IXMLDOMNode			*nodeAttrib = NULL;
			nodeList->get_item(nodeNum, &nodeChild);
			nodeChild->get_attributes(&attrib);
			if (attrib!=0)
			{
				attrib->get_length(&lengthAttrib);
				attrib->get_item(attribNum, &nodeAttrib);
				VARIANT	att;
				nodeAttrib->get_nodeValue(&att);
				WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, att.bstrVal, -1, Name, SysStringLen(att.bstrVal)+1, NULL, NULL );
			}
			else
			{
				strcpy(Name, "ERROR: No attrib");
			}
			if (attrib)
			{
				attrib->Release();
				attrib = NULL;
			}
			if (nodeAttrib)
			{
				nodeAttrib->Release();
				nodeAttrib = NULL;
			}
		}
		else
		{
			strcpy(Name, "ERROR: Out of Lenght");
		}
	}
	return Name;
}

DLL_API int DEXML::ReadAttribute_Int(BSTR nodeName, long nodeNum, long attribNum)
{
	int valueInt=0;
	char Name[MAX_PATH];
	long lengthAttrib = 0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (nodeList) 
	{
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			//Preleva gli Attributi
			IXMLDOMNamedNodeMap *attrib = NULL;
			IXMLDOMNode			*nodeAttrib = NULL;
			nodeList->get_item(nodeNum, &nodeChild);
			nodeChild->get_attributes(&attrib);
			if (attrib!=0)
			{
				attrib->get_length(&lengthAttrib);
				attrib->get_item(attribNum, &nodeAttrib);
				VARIANT	att;
				nodeAttrib->get_nodeValue(&att);
				WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, att.bstrVal, -1, Name, SysStringLen(att.bstrVal)+1, NULL, NULL );
				valueInt = atoi(Name);
			}
			else
			{
				valueInt = 0;
			}
			if (attrib)
			{
				attrib->Release();
				attrib = NULL;
			}
			if (nodeAttrib)
			{
				nodeAttrib->Release();
				nodeAttrib = NULL;
			}
		}
		else
		{
			valueInt = 0;
		}
	}
	return valueInt;
}


DLL_API float DEXML::ReadAttribute_Float(BSTR nodeName, long nodeNum, long attribNum)
{
	int valueFloat=0;
	char Name[MAX_PATH];
	long lengthAttrib = 0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (nodeList) 
	{
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			//Preleva gli Attributi
			IXMLDOMNamedNodeMap *attrib = NULL;
			IXMLDOMNode			*nodeAttrib = NULL;
			nodeList->get_item(nodeNum, &nodeChild);
			nodeChild->get_attributes(&attrib);
			if (attrib!=0)
			{
				attrib->get_length(&lengthAttrib);
				attrib->get_item(attribNum, &nodeAttrib);
				VARIANT	att;
				nodeAttrib->get_nodeValue(&att);
				WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, att.bstrVal, -1, Name, SysStringLen(att.bstrVal)+1, NULL, NULL );
				valueFloat = atof(Name);
			}
			else
			{
				valueFloat = 0;
			}
			if (attrib)
			{
				attrib->Release();
				attrib = NULL;
			}
			if (nodeAttrib)
			{
				nodeAttrib->Release();
				nodeAttrib = NULL;
			}
		}
		else
		{
			valueFloat = 0;
		}
	}
	return valueFloat;
}



DLL_API bool DEXML::ReadAttribute_Bool(BSTR nodeName, long nodeNum, long attribNum)
{
	int valueBool=false;
	char Name[MAX_PATH];
	long lengthAttrib = 0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (nodeList) 
	{
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			//Preleva gli Attributi
			IXMLDOMNamedNodeMap *attrib = NULL;
			IXMLDOMNode			*nodeAttrib = NULL;
			nodeList->get_item(nodeNum, &nodeChild);
			nodeChild->get_attributes(&attrib);
			if (attrib!=0)
			{
				attrib->get_length(&lengthAttrib);
				attrib->get_item(attribNum, &nodeAttrib);
				VARIANT	att;
				nodeAttrib->get_nodeValue(&att);
				WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, att.bstrVal, -1, Name, SysStringLen(att.bstrVal)+1, NULL, NULL );			
				int result1 = strncmp( (const char *)(strdup(Name)), "1" , strlen(Name) );
				if (result1 == 0)	//Uguali
				{
					valueBool = true;
				}
				else
				{
					int result2 = strncmp( (const char *)strlwr(strdup(Name)), "true" , strlen(Name) );
					if (result2 == 0)//Uguali
					{
						valueBool = true;
					}
					else
					{
						valueBool = false;
					}
				}
			}
			else
			{
				valueBool = false;
			}
			if (attrib)
			{
				attrib->Release();
				attrib = NULL;
			}
			if (nodeAttrib)
			{
				nodeAttrib->Release();
				nodeAttrib = NULL;
			}
		}
		else
		{
			valueBool = false;
		}
	}
	return valueBool;
}


DLL_API long DEXML::ReadAttributes_Lenght(BSTR nodeName, long nodeNum)
{
	long lengthAttrib = 0;
	pXMLDom->selectNodes(nodeName, &nodeList);
	if (!nodeList) 
	{
		lengthAttrib = 0;
	}
	else 
	{
		IXMLDOMNamedNodeMap *attrib = NULL;
		long lengthList=0;
		nodeList->get_length(&lengthList);
		if (nodeNum < lengthList)
		{
			//Preleva gli Attributi
			nodeList->get_item(nodeNum, &nodeChild);
			nodeChild->get_attributes(&attrib);
			if (attrib!=0)
			{
				attrib->get_length(&lengthAttrib);
			}
		}
		else
		{
			lengthAttrib = 0;
		}
		if (attrib)
		{
			attrib->Release();
			attrib = NULL;
		}
	}
	return lengthAttrib;
}











DLL_API bool DEXML::Release()
{
	try
	{
		if (pXMLDom)
		{
			pXMLDom->Release();
			pXMLDom = NULL;
		}
		if (nodeList)	
		{
			nodeList->Release();
			nodeList = NULL;
		}
		if (nodeChild)	
		{
			nodeChild->Release();
			nodeChild = NULL;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}


