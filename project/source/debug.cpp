
#include "stdafx.h"
#include <wincon.h>

/*
         BG_GREY    = BACKGROUND_BLUE+BACKGROUND_GREEN+BACKGROUND_RED,
         BG_WHITE   = BACKGROUND_INTENSITY+BG_GREY,
*/

void Create_STD_OUT_Console( )
{
//create a console for standard output
	AllocConsole( );
	SetConsoleTitle( _T("stdout") );
	HANDLE con;
    con= CreateFile( _T("CON"), GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, 0, 0, 0 );
	FILE *stream;
    freopen_s( &stream, "CON", "wt", stdout );
	SetConsoleTextAttribute( con, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY );
}
