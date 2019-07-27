
#include "stdafx.h"
#include "shared.h"
#include "Geometry.h"
#include "Objects.h"
#include "view_share.h"

//open a TinyLib by interface id.


//tinylib_item get_tinylib_item_by_id(uint64_t id);

void Create_STD_OUT_Console()
{
	//create a console for standard output
	AllocConsole();
	SetConsoleTitle(_T("stdout"));
	HANDLE con;
	con = CreateFile(_T("CON"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 0, 0, 0);
	FILE *stream;
	freopen_s(&stream, "CON", "wt", stdout);
	SetConsoleTextAttribute(con, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
}

