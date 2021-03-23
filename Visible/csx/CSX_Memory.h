#pragma once

#include "CSX_Engine.h"

namespace CSX
{
	namespace Memory
	{
		/* IDA Style 00 FF ?? */

		auto FindPattern( const char* pattern , DWORD_PTR start , DWORD_PTR end , DWORD_PTR offset , DWORD read = 0 )->PVOID;
		auto FindPattern( const char* module_name , const char* pattern , DWORD_PTR offset , DWORD read = 0 )->PVOID;
	}
}