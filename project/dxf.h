#pragma once


void DXF_Stream( std::wofstream& os, DrawingObects& drawobj, DL_Dxf* dxf, DL_WriterA* dw );
void DXF_WriteFile( bfs::path& pathname,  DrawingObects& drawobj );
#ifdef _DEBUG
#pragma comment(lib, "./dxfd.lib")
#else
#pragma comment(lib, "./dxf.lib")
#endif

