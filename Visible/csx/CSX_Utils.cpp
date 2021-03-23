#include "CSX_Utils.h"

static const DWORD dwModuleDelay = 100;
#define INT_DIGITS 19

namespace CSX
{
	namespace Utils
	{
		MODULEINFO GetModuleInfo( const char* szModule )
		{
			MODULEINFO modinfo = { 0 };
			HMODULE hModule = GetModuleHandleA( szModule );

			if ( hModule == 0 )
				return modinfo;

			GetModuleInformation( GetCurrentProcess() , hModule , &modinfo , sizeof( MODULEINFO ) );
			return modinfo;
		}

		/* Wait dwMsec Load Module */

		bool IsModuleLoad( const char* szModule , DWORD dwMsec )
		{
			HMODULE hModule = GetModuleHandleA( szModule );

			if ( !hModule )
			{
				DWORD dwMsecFind = 0;

				while ( !hModule )
				{
					if ( dwMsecFind == dwMsec )
						return false;

					hModule = GetModuleHandleA( szModule );

					HANDLE hEvent = CreateEventA( 0 , true , false , 0 );
					WaitForSingleObject( hEvent , dwModuleDelay );
					CloseHandle( hEvent );

					dwMsecFind += dwModuleDelay;
				}
			}

			return true;
		}

		// convert int to char
		PCHAR itoa( int i )
		{
			static char buf[INT_DIGITS + 2];
			char *p = buf + INT_DIGITS + 1;

			if ( i >= 0 )
			{
				do
				{
					*--p = '0' + ( i % 10 );
					i /= 10;
				} while ( i != 0 );
				return p;
			}
			else
			{
				do
				{
					*--p = '0' - ( i % 10 );
					i /= 10;
				} while ( i != 0 );
				*--p = '-';
			}

			return p;
		}

		int atoi( PCHAR pStr1 )
		{
			int Value = 0 , Digit;
			char c;
			bool negt = false;

			if ( *pStr1 == '-' )
			{
				pStr1++;
				negt = true;
			}

			while ( ( c = *pStr1++ ) != '\0' )
			{
				if ( c >= '0' && c <= '9' )
					Digit = (UINT)( c - '0' );
				else
					break;

				Value = ( Value * 10 ) + Digit;
			}

			if ( !negt )
				return Value;
			else
				return -Value;
		}
	}
}