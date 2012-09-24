#pragma once

#include <string>
#include <map>
#include <unordered_map>

using namespace std;

namespace EDGOCA
{
	class StringTable
	{
		map< size_t, string > UIntStringMap;

		hash<string> hasher;

		size_t HashString(const char* str);

	public:

		size_t AddString(const char* str);

		void RemoveString(const char* str);

		size_t GetStringKey(const char* str);

		const char* GetKeyString(size_t key);
	};
}