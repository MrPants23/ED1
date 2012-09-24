#include "StringTable.h"

namespace EDGOCA
{
	size_t StringTable::HashString(const char* str)
	{
		return hasher(str) + 1;
	}

	size_t StringTable::AddString(const char* str)
	{
		size_t key = HashString(str);
		UIntStringMap.insert( pair<size_t, string>( key, string(str) ) );

		return key;
	}

	void StringTable::RemoveString(const char* str)
	{
		UIntStringMap.erase( HashString(str) );
	}

	size_t StringTable::GetStringKey(const char* str)
	{
		size_t key = HashString(str);
		map<size_t, string>::iterator mapIter = UIntStringMap.find( key );

		if( mapIter == UIntStringMap.end() )
			return 0;

		return key;
	}

	const char* StringTable::GetKeyString(size_t key)
	{
		map<size_t, string>::iterator mapIter = UIntStringMap.find( key );

		if( mapIter == UIntStringMap.end() )
			return 0;
		
		return mapIter->second.c_str();
	}
}
