#ifndef H_PROPERTY_H
#define H_PROPERTY_H

template <class T> class HashTable
{
	//----- Constants -----//
protected:
	enum
	{
		DEFAULT_HASH_TABLE_SIZE = 256
	};
public:

	template <class T> class HashTableNode
	{
	public:
		string	m_key;
		T* m_data;

		HashTableNode(std::string const &key, T* new_data)
		{
			m_key = key;
			m_data = new_data;
		}
	};

	//----- Properties -----//
private:
protected:
	std::list< HashTableNode<T> >*	m_hash_table;
	int							m_table_size;
	int							m_count;
public:

	//----- Friends -----//

	//----- Methods -----//
private:
	// declared to prevent unintentional use...
	// (Don't forget to move to public access if you declare them!)
	HashTable& Copy(const HashTable& source_object);
	HashTable& operator= (const HashTable& source_object);
//	{
//		return Copy(source_object);
//	}
protected:
	HashTableNode<T>* LookupNode(std::string const &key)
	{
		unsigned int index = Hash(key) % GetTableSize();
		HashTableNode<T>* return_value = 0;

		if(m_hash_table[index].size() == 0)
			return 0;

		std::list< HashTableNode<T> >::iterator walker;
		walker = m_hash_table[index].begin();

		while(walker != m_hash_table[index].end())
		{
			if(walker->m_key == key)
			{
				return_value = &(*walker);
				break;
			}
			walker++;
		}

		return return_value;
	}

public:
	HashTable()
	{
		m_count = 0;
		Initialize();
	}

	HashTable(int size)
	{
		m_count = 0;
		Initialize(size);
	}

	HashTable(const HashTable& source)
	{
		Copy(source);
	}

	// remove virtual if class will not be inherited to improve efficiency
	virtual ~HashTable()
	{
		if(m_hash_table)
			delete [] m_hash_table;
	}

	void Initialize(int size = DEFAULT_HASH_TABLE_SIZE)
	{
		m_hash_table = new std::list< HashTableNode<T> >[size];
		m_table_size = size;
	}

	unsigned int Hash(std::string const &name)
	{
		unsigned int return_value = 0;
		int character;
		unsigned int i;


		for(i = 0; i < name.size(); ++i)
		{
			character = name[i];
			return_value ^= i;
			return_value <<= 1;
		}

		return return_value;
	}

	// insert key into hash table.
	// returns pointer to old data with the key, if any, or
	// NULL if the key wasn't in the table previously.
	T* Insert(std::string const &key, T* data)
	{
		if(!m_hash_table)
			return 0;

		unsigned int index = Hash(key) % GetTableSize();
		HashTableNode<T> new_node(key,data);

		m_hash_table[index].push_back(new_node);
		m_count++;
		return new_node.m_data;

	}


	T* Lookup(std::string const &key)
	{
		if(!m_hash_table)
			return 0;

		HashTableNode<T>* new_node = LookupNode(key);

		if(new_node)
			return new_node->m_data;

		return 0;
	}

	// returns the list that contains this hash key...
	// (for instance, if you have multiple matching keys)
	std::list<T>* LookupList(std::string const& key)
	{
		if(!m_hash_table)
			return 0;
		unsigned int index = Hash(key) % GetTableSize();

		if(m_hash_table[index].size() == 0)
			return 0;

		return &m_hash_table[index];
	}

	T* Delete(std::string const &key)
	{
		if(!m_hash_table)
			return 0;

		unsigned int index = Hash(key) % GetTableSize();

		if(m_hash_table[index].size() == 0)
			return 0;

		std::list< HashTableNode<T> >::iterator walker;

		walker = m_hash_table[index].begin();

		while( walker != m_hash_table[index].end() )
		{
			if(walker->m_key == key)
			{
				T* return_value = walker->m_data;
				m_hash_table[index].erase(walker);
				m_count--;
				return return_value;
			}

			walker++;
		}

		return 0;
	}

	std::list< HashTableNode<T> >* GetHashTable()
	{
		return m_hash_table;
	}

	int GetTableSize()
	{
		return m_table_size;
	}

	int Count()
	{
		return m_count;
	}

	//----- overridables -----//
};






class DEProperty
{
	protected:
		union Data
		{
			int* m_int;
			float* m_float;
			std::string* m_string;
			bool* m_bool;
		};

		enum Type
		{
			INT,
			FLOAT,
			STRING,
			BOOL,
			EMPTY
		};

		Data	m_data;
		Type	m_type;
		std::string m_name;

		void EraseType()
		{
			m_type = EMPTY;
			m_data.m_int = 0;
			m_name = "";
		}

		void Register(int* value)
		{
			m_type = INT;
			m_data.m_int = value;
		}

		void Register(float* value)
		{
			m_type = FLOAT;
			m_data.m_float = value;
		}

		void Register(std::string* new_string)
		{
			m_type = STRING;
			m_data.m_string = new_string;
		}

		void Register(bool* value)
		{
			m_type = BOOL;
			m_data.m_bool = value;
		}


	private:
		// declared to prevent unintentional use...
		// (Don't forget to move to public access if you declare them!)
		DEProperty& Copy(const DEProperty& source_object)
		{
			m_data = source_object.m_data;
			m_type = source_object.m_type;
			m_name = source_object.m_name;

			return *this;
		}
		DEProperty& operator= (const DEProperty& source_object)
		{
			return Copy(source_object);
		}


	public:
		DEProperty()
		{
			EraseType();
		}
		DEProperty(const DEProperty& source)
		{
			Copy(source);
		}

		DEProperty(std::string const& name)
		{
			EraseType();
			m_name = name;
		}

		DEProperty(std::string const& name, int* value)
		{
			m_name = name;
			Register(value);
		}

		DEProperty(std::string const& name, float* value)
		{
			m_name = name;
			Register(value);
		}

		DEProperty(std::string const& name, std::string* value)
		{
			m_name = name;
			Register(value);
		}

		DEProperty(std::string const& name, bool* value)
		{
			m_name = name;
			Register(value);
		}


		// remove virtual if class will not be inherited to improve efficiency
	//	virtual ~DEProperty();

		bool SetUnknownValue(std::string const& value);
		bool Set(int value);
		bool Set(float value);
		bool Set(bool value);
		bool Set(std::string const& value);


		void SetName(std::string const& name)
		{
			m_name = name;
		}

		std::string GetName() const
		{
			return m_name;
		}

		int GetInt()
		{
			assert(m_type == INT);
			return *m_data.m_int;
		}

		float GetFloat()
		{
			assert(m_type == FLOAT);
			return *m_data.m_float;
		}

		std::string GetString()
		{
			assert(m_type == STRING);
			return *m_data.m_string;
		}

		bool GetBool()
		{
			assert(m_type == BOOL);
			return *m_data.m_bool;
		}

		// generic return...
		std::string GetValue();
};



class DEPropertySet
{
	protected:
		HashTable<DEProperty>	m_properties;

	private:
		// declared to prevent unintentional use...
		// (Don't forget to move to public access if you declare them!)
		DEPropertySet& Copy(const DEPropertySet& source_object);
		DEPropertySet& operator= (const DEPropertySet& source_object);

	public:
		DEPropertySet();
		DEPropertySet(const DEPropertySet& source)
		{
			Copy(source);
		}

		// remove virtual if class will not be inherited to improve efficiency
		virtual ~DEPropertySet();

		void Register(std::string const& name, int* value); 
		void Register(std::string const& name, float* value); 
		void Register(std::string const& name, std::string* value); 
		void Register(std::string const& name, bool* value); 

		DEProperty* Lookup(std::string const& name);

		// use this for generic access when the data type is unknown
		// this will convert to the proper format.
		bool SetValue(std::string const& name, std::string const& value);

		// use these if you know the data type
		bool Set(std::string const& name, std::string const& value);
		bool Set(std::string const& name, int value);
		bool Set(std::string const& name, float value);
		bool Set(std::string const& name, bool value);
		bool Set(std::string const& name, char* value);
};

#endif
