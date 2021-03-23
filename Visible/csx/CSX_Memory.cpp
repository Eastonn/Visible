#include "CSX_Memory.h"

#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))

namespace CSX
{
	namespace Memory
	{
		auto FindPattern( const char* pattern , DWORD_PTR start , DWORD_PTR end , DWORD_PTR offset , DWORD read )->PVOID
		{
			DWORD_PTR found_address = 0;
			auto p_pattern = pattern;

			for ( DWORD_PTR dwPtr = start; dwPtr < end; dwPtr++ )
			{
				if ( !*p_pattern )
					return (PVOID)found_address;

				if ( *(PBYTE)p_pattern == '\?' || *(PBYTE)dwPtr == getByte( p_pattern ) )
				{
					if ( !found_address )
						found_address = (DWORD_PTR)dwPtr;

					if ( !p_pattern[1] || !p_pattern[2] )
						break;

					if ( *(PWORD)p_pattern == '\?\?' || *(PBYTE)p_pattern != '\?' )
						p_pattern += 3;
					else
						p_pattern += 2;
				}
				else
				{
					p_pattern = pattern;
					found_address = 0;
				}
			}

			if ( found_address )
			{
				found_address += (DWORD_PTR)offset;

				for ( DWORD i = 0; i < read; i++ )
					found_address = *(PDWORD_PTR)( found_address );
			}

			return (PVOID)found_address;
		}

		auto FindPattern( const char* module_name , const char* pattern , DWORD_PTR offset , DWORD read )->PVOID
		{
			auto Info = CSX::Utils::GetModuleInfo( module_name );
			auto Start = (DWORD_PTR)Info.lpBaseOfDll;
			auto Size = Info.SizeOfImage;

			return FindPattern( pattern , Start , Start + Size , offset , read );
		}
	}
}