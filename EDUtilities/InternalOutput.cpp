#include "../EDMemoryManager/MMAllocator.h"
#include "InternalOutput.h"

#include <Windows.h>

namespace EDUtilities
{

	InternalOutput *InternalOutput::instancePtr = 0;

	InternalOutput::~InternalOutput(void) 
	{
		CloseHandle(outputHandle);
	}
	
	InternalOutput &InternalOutput::GetReference()
	{
		if(0 == instancePtr)
		{
			instancePtr = new InternalOutput;
			instancePtr->Create();
		}
		return *instancePtr;
	}
	void InternalOutput::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = 0;
	}

	void InternalOutput::Create()
	{
		AllocConsole();

		//outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		outputHandle = CreateFile(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	}

	void InternalOutput::Error(const char *formatString, ...)
	{
		va_list ap;
		va_start(ap, formatString);

		char buffer[MAX_STRING_LENGTH];
		vsnprintf_s(buffer, MAX_STRING_LENGTH, MAX_STRING_LENGTH-1, formatString, ap);

		SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
		(*this) << buffer;
		SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_BLUE 
			| FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		va_end(ap);
	}

	InternalOutput &operator<<(InternalOutput &out, 
		const char *string)
	{
		WCHAR *buffer = new WCHAR[strlen(string)+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, 
			static_cast<int>(strlen(string)), buffer, static_cast<int>(strlen(string)));

		buffer[strlen(string)] = 0;

		out << buffer;

		delete [] buffer;
		return out;
	}

	InternalOutput &operator<<(InternalOutput &out, 
		const WCHAR *string)
	{
		DWORD charWritten;
		WriteConsole(out.outputHandle, string, static_cast<DWORD>(wcslen(string)), 
			&charWritten, 0);
		return out;
	}

	InternalOutput &operator<<(InternalOutput &out, 
		const std::string &inString)
	{
		return out << inString.c_str();
	}

	InternalOutput &operator<<(InternalOutput &out,
		char letter)
	{
		out << &letter;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		float fVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%f", fVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		double dVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%f", dVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		int iVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%d", iVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		UINT iVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%d", iVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		long lVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%d", lVal);

		out << buffer;

		return out;
	}
}