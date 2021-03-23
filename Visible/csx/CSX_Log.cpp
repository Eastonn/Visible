#include "CSX_Log.h"

namespace CSX
{
	namespace Log
	{
		const char* szLogFilePath = nullptr;
		HANDLE hLogFile = nullptr;

		void SetLogFile( const char* File )
		{
			szLogFilePath = File;
		}

		void AddLog( const char* Text )
		{
			if ( !hLogFile && szLogFilePath )
				hLogFile = CreateFileA( szLogFilePath , GENERIC_WRITE , FILE_SHARE_READ , 0 , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , 0 );
			
			if ( hLogFile && szLogFilePath )
				WriteFile( hLogFile , (PVOID)Text , lstrlenA( Text ) , 0 , 0 );
		}
	}
}