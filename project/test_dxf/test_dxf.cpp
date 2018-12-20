// test_dxf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objects.h>

//#include "../Drawing.h"

// ......................................
int main( )
{
	DL_Dxf* dxf= new DL_Dxf( );
	DL_Codes::version exportVersion= DL_Codes::AC1015;
	//	DL_WriterA* dw = dxf->out( uni_t0_utf8( lpszPathName ), exportVersion );
	DL_WriterA* dw = dxf->out( "testdxf.dxf", exportVersion );
	StartDXF_write( dxf, dw );
	dw->sectionEntities( );

	ArcItem item( bg_point( 0, 2 ), bg_point( PI / 4, PI / 4 ), bg_point( 0, 0 ) );
	ArcItem* p= &item;
	DL_ArcData ad( p->get_o( ).get_x( ),
		p->get_o( ).get_y( ),
		0,
		p->get_rad( ),
		p->get_start_ang( ) * 180,
		p->get_end_ang( ) * 180
		);
		if( p )
	{
		dxf->writeArc( *dw, ad,
			DL_Attributes("mainlayer", 256, 10, "BYLAYER", 1 ) );
	}
	dxf->writeLine(
		*dw,
		DL_LineData(2.0,   // start point
			2.0,
			0.0,
			4.0,   // end point
			4.0,
			0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER",2));



	dw->sectionEnd();

	EndDXF_write( dxf, dw );
	return 0;
}

