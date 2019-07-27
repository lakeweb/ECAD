#pragma once


void DXF_Stream( std::wofstream& os, DrawingObect& drawobj, DL_Dxf* dxf, DL_WriterA* dw );
void DXF_WriteFile( bfs::path& pathname,  DrawingObect& drawobj );
#ifdef _DEBUG
#pragma comment(lib, "./dxfd.lib")
#else
#pragma comment(lib, "./dxf.lib")
#endif

