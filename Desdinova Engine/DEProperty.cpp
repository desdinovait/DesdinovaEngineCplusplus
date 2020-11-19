#include "DEGeneralIncludes.h"

bool DEProperty::SetUnknownValue(std::string const& value)
{
	bool return_value = true;
	switch(m_type)
	{
		case INT:
			*m_data.m_int = atoi(value.c_str());
			break;
		case FLOAT:
			*m_data.m_float = atof(value.c_str());
			break;
		case STRING:
			*m_data.m_string = value;
			break;
		case BOOL:
			*m_data.m_bool = (value.compare("true") == 0) || (value.compare("TRUE") == 0);
			break;
		default:
			return_value = false;
			break;
	}
	return return_value;
}

bool DEProperty::Set(int value)
{
	bool return_value = false;
	if(m_type == INT)
	{
		*m_data.m_int = value;
		return_value = true;
	}
	return return_value;
}

bool DEProperty::Set(float value)
{
	bool return_value = false;
	if(m_type == FLOAT)
	{
		*m_data.m_float = value;
		return_value = true;
	}
	return return_value;
}

bool DEProperty::Set(bool value)
{
	bool return_value = false;
	if(m_type == BOOL)
	{
		*m_data.m_bool = value;
		return_value = true;
	}
	return return_value;
}

bool DEProperty::Set(std::string const& value)
{
	bool return_value = false;
	if(m_type == STRING)
	{
		*m_data.m_string = value;
		return_value = true;
	}
	return return_value;
}

std::string DEProperty::GetValue()
{
	std::string return_value;
	char buffer[64];
	switch(m_type)
	{
		case INT:
			sprintf(buffer,"%d",*m_data.m_int);
			return_value = buffer;
			break;
		case FLOAT:
			sprintf(buffer,"%f",*m_data.m_float);
			return_value = buffer;
			break;
		case STRING:
			return_value = *m_data.m_string;
			break;
		case BOOL:
			if(*m_data.m_bool)
				sprintf(buffer,"true");
			else
				sprintf(buffer,"false");
			return_value = buffer;
			break;
	}
	return return_value;
}





//----- DEPropertySet -------------------------------

DEPropertySet::DEPropertySet()
{
}

DEPropertySet::~DEPropertySet()
{
}

void DEPropertySet::Register(std::string const& name, int* value)
{
	DEProperty* new_DEProperty = new DEProperty(name,value);
	m_properties.Insert(name,new_DEProperty);
}

void DEPropertySet::Register(std::string const& name, float* value)
{
	DEProperty* new_DEProperty = new DEProperty(name,value);
	m_properties.Insert(name,new_DEProperty);
}

void DEPropertySet::Register(std::string const& name, std::string* value)
{
	DEProperty* new_DEProperty = new DEProperty(name,value);
	m_properties.Insert(name,new_DEProperty);
}

void DEPropertySet::Register(std::string const& name, bool* value)
{
	DEProperty* new_DEProperty = new DEProperty(name,value);
	m_properties.Insert(name,new_DEProperty);
}

DEProperty* DEPropertySet::Lookup(std::string const& name)
{
	return m_properties.Lookup(name);
}

bool DEPropertySet::SetValue(std::string const& name, std::string const& value)
{
	bool return_value = false;
	DEProperty* DEProperty = Lookup(name);
	if(DEProperty)
	{
		return_value = DEProperty->SetUnknownValue(value);
	}
	return return_value;

}

bool DEPropertySet::Set(std::string const& name, std::string const& value)
{
	bool return_value = false;
	DEProperty* DEProperty = Lookup(name);
	if(DEProperty)
	{
		return_value = DEProperty->Set(value);
	}
	return return_value;
}

bool DEPropertySet::Set(std::string const& name, int value)
{
	bool return_value = false;
	DEProperty* DEProperty = Lookup(name);
	if(DEProperty)
	{
		return_value = DEProperty->Set(value);
	}
	return return_value;
}

bool DEPropertySet::Set(std::string const& name, float value)
{
	bool return_value = false;
	DEProperty* DEProperty = Lookup(name);
	if(DEProperty)
	{
		return_value = DEProperty->Set(value);
	}
	return return_value;
}

bool DEPropertySet::Set(std::string const& name, bool value)
{
	bool return_value = false;
	DEProperty* DEProperty = Lookup(name);
	if(DEProperty)
	{
		return_value = DEProperty->Set(value);
	}
	return return_value;
}

bool DEPropertySet::Set(std::string const& name, char* value)
{
	bool return_value = false;
	DEProperty* DEProperty = Lookup(name);
	if(DEProperty)
	{
		return_value = DEProperty->Set(std::string(value));
	}
	return return_value;
}