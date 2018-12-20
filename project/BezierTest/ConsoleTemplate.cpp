// ConsoleTemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Geometry.h"
#include "../Objects.h"

// ..........................................................................
#ifdef DXFLIB
bool StartDXF_write( DL_Dxf* dxf, DL_WriterA* dw );
bool EndDXF_write( DL_Dxf* dxf, DL_WriterA* dw );
#endif // DXFLIB

// ..........................................................................
typedef struct gear_params
{
	//given
	size_t tc; //number of teeth
	double pa; //presure angle
	double dp; //diametrical pitch, "teeth per inch of pitch circle"
			   //calced
	double pd; //pitch circle
	double rd; //root diameter = ( N - 2 ) / D;
	double ad; //addendum, inverse of dp
	double bd; //base diameter for involute
	double od; //outside diameter
			   //methods will be added for leaser used stuff

			   //only three parameters to define a gear
	gear_params( size_t tooth_count, double presure_anglee, double diametrical_pitch )
		:tc( tooth_count )
		,pa( presure_anglee )
		,dp( diametrical_pitch )
		,pd( (double)tooth_count / diametrical_pitch )
		,rd( ( tooth_count - 2 ) / diametrical_pitch )
		,ad( 1 / diametrical_pitch )
		,od( ( tooth_count + 2 ) / diametrical_pitch )
	{
		bd= pd * cos( pa  * PI / 180 );
	}
}gear_params_t;

// ..........................................................................
void gear2( gear_params_t& gp, ObjectSet& out )
{
	double root_radius= gp.rd / 2;
	double base_radius= gp.bd / 2;
	double pitch_radius= gp.pd / 2;
	double outside_radius= gp.od / 2;
	//angle between teeth
	double ang_tooth=	2 * PI / gp.tc;
	double base_outside_div_3= ( outside_radius - base_radius ) / 3;

	//Base Circle: The circle used to form the involute section

	//"Gear Drawing with Bezier Curves" is just not right
	//	addendum and dedendum are hard coded!

	//angle of involute to outside circle
	//see fetch.pdf, equ (6),(7) then from...
	//polar_radius = base_radius * sqrt( 1 + t^2 )
	//where t is the arc AB
	//so solve for t
	// t= ( ( outside_diameter / pitch_diameter )^2 - 1 )^1/2
	//then the polar angle
	// O= t - arctan t
	//for the end point of the involute arc
	//
	double Ac;
	double Dop= gp.od / gp.bd;
	double t= sqrt( Dop * Dop - 1 );
	Ac= t - atan( t );

#define TEST_INVOLUTE
#ifdef TEST_INVOLUTE
	//TESTING !
	//create linestring from involute curve
	//	bg_linestring_t ls;
	SP_PolyItem ls( new PolyItem );
	for( size_t step= 0; step <= 100; ++step )
	{
		double arc= t / 100 * step;
		*ls+= ( bg_point( base_radius * ( cos( arc ) + arc * sin( arc ) ),
			base_radius * ( sin( arc ) - arc * cos( arc ) ) ) );
	}
	ls->SetMill( );
	out.push_back( ls );

	SP_ArcItem spo( new ArcItem( bg_point( outside_radius, 0 ), bg_point( outside_radius, 0 ), bg_point( 0, 0 ) ) );
	spo->SetMill( );
	out.push_back( spo );

	SP_ArcItem spb( new ArcItem( bg_point( base_radius, 0 ), bg_point( base_radius, 0 ), bg_point( 0, 0 ) ) );
	spb->SetMill( );
	out.push_back( spb );

	SP_LineItem oe( new LineItem( bg_point( 0, 0 ), bg_polar( Ac, outside_radius ) ) );
	oe->SetMill( );
	out.push_back( oe );
	//END TESTING !
#endif // TEST_INVOLUTE

	//now with ratios ................................


	//create the base involutes one a mirror of the other.
	//we will use a single Bezier for now, two are recomended, so they say
	bg_point inv_start_point(  base_radius, 0 );
	bg_point inv_end_point( bg_polar( Ac, outside_radius ) );
	bg_point inv_p1( base_radius * 3.00518841265 / 2.90444292113, inv_end_point.get_y( ) * 0.00667325332195 / 0.119488833399 );
	bg_point inv_p2( base_radius * 3.13881236607 / 2.90444292113, inv_end_point.get_y( ) * 0.059582618946 / 0.119488833399 );

	SP_BezierCubeItem inv_base( new BezierCubeItem( inv_start_point, inv_p1, inv_end_point, inv_p2 ) );
	inv_base->SetMill( );

	SP_BezierCubeItem inv_base_mir( new BezierCubeItem(
		mirror_y( inv_start_point ), mirror_y( inv_p1 ), mirror_y( inv_end_point ), mirror_y( inv_p2 ) ) );
	inv_base_mir->SetMill( );

	//testing
	out.push_back( inv_base );
	out.push_back( inv_base_mir );
	double test_mark= ( gp.od - gp.ad ) / 2;
	SP_LineItem line_test( new LineItem( bg_point( test_mark, 0 ), bg_point( test_mark, 1 ) ) );
	out.push_back( line_test );

	SP_EllipesItem vos( new EllipesItem( bg_point( -outside_radius, -outside_radius ), bg_point( outside_radius, outside_radius ) ) );
	vos->SetColor( RGB( 0, 0, 255 ) );
	out.push_back( vos );

	SP_EllipesItem vrs( new EllipesItem( bg_point( -root_radius, -root_radius ), bg_point( root_radius, root_radius ) ) );
	vrs->SetColor( RGB( 0, 0, 255 ) );
	out.push_back( vrs );
}

// ..........................................................................
void test( DL_Dxf* dxf, DL_WriterA* dw, const BezierCubeItem& item )
{
	size_t degree= 3;
	size_t ctl_pts= 4;
	size_t knots= ctl_pts + degree + 1;
	dxf->writeSpline(
		*dw,
		DL_SplineData( 3, 8, 4, 0, 8 ),
		DL_Attributes("mainlayer", 256, 1, "BYLAYER",2)
		);

	//bg_point kn;
	//for( size_t i= 1; i <= knots; ++i )
	//{
	//	if( i < degree )
	//		kn.set_xy( 0 );
	//	else if( i < knots - degree )
	//		kn.set_xy( 1.0 / ( knots - 2 * degree + 2 ) * ( i - degree + 1 ) );
	//	else
	//		kn.set_xy( 1 );
	//	dxf->writeKnot( *dw, DL_KnotData( kn.get_x( ) ) );
	//}
	bg_point kn;
	for( size_t i= 1; i <= knots; ++i )
	{
		dxf->writeKnot( *dw, DL_KnotData( ( i + 3 < knots ? 0 : 1 ) ) );
	}
	// write spline control points:
	dxf->writeControlPoint( *dw,
		DL_ControlPointData( item.get_point0( ).get_x( ),
			item.get_point0( ).get_y( ),
			0.0, 1.0
			)
		);
	dxf->writeControlPoint( *dw,
		DL_ControlPointData( item.get_point1( ).get_x( ),
			item.get_point1( ).get_y( ),
			0.0, 1.0
			)
		);
	dxf->writeControlPoint( *dw,
		DL_ControlPointData( item.get_point3( ).get_x( ),
			item.get_point3( ).get_y( ),
			0.0, 1.0
			)
		);
	dxf->writeControlPoint( *dw,
		DL_ControlPointData( item.get_point2( ).get_x( ),
			item.get_point2( ).get_y( ),
			0.0, 1.0
			)
		);
}


// ..........................................................................
//Quadratic Bézier curve
//http://www.arc.id.au/GearDrawing.html
using coffxy= std::function< double ( double ) >;
double chebyExpnCoeffs( double j, coffxy func )
{
	size_t N= 50;      // a suitably large number  N>>p
	size_t c= 0;
	size_t k= 1;

	for( ; k < N; k++ )
	{
		c += func( cos( PI * ( k - 0.5 ) / N ) ) * cos( PI * j * ( k - 0.5 ) / N );
	}
	return 2 * c / N;
}

// ..........................................................................
int _tmain( int argc, _TCHAR* argv[ ] )
{
	gear_params gp( 36, 20, 36 );
	ObjectSet oset( 0 );
	gear2( gp, oset );

	DL_Dxf* dxf= new DL_Dxf( );
	DL_Codes::version exportVersion= DL_Codes::AC1015;
	//	DL_WriterA* dw = dxf->out( uni_t0_utf8( lpszPathName ), exportVersion );
	DL_WriterA* dw = dxf->out( "test.dxf", exportVersion );
	StartDXF_write( dxf, dw );

	for( auto item : oset )
	{
		if( dynamic_cast< const BezierCubeItem* >( item.get( ) ) )
			test( dxf, dw, dynamic_cast< const BezierCubeItem& >( *item ) );
	}
	EndDXF_write( dxf, dw );
	return 0;
}
