

#include "stdafx.h"
#include "cad_full.h"
#include "gear_gen.h"

// ..........................................................................
// creates one tooth of the gear using the world origin....
ItemSet gear_gen_one( const gear_params_t& gp )
{
	//most calcs are done with radii
	double root_radius= gp.rd / 2;
	double base_radius= gp.bd / 2;
	double pitch_radius= gp.pd / 2;
	double outer_radius= gp.od / 2;

	//Base Circle: The circle used to form the involute section

	//angle of involute to outside circle
	//see fetch.pdf, equ (6),(7) then from...

	//polar_radius = base_radius * sqrt( 1 + t^2 )
	//where t is the arc AB
	//the limit of polar_radius is the outer_radius

	//   so solve for t at the limit
	// t= ( ( outside_diameter / base_diameter )^2 - 1 )^1/2

	//   then the polar angle
	//O= t - arctan t

	//for the end point of the involute arc
	double Dop= gp.od / gp.bd; //ouside diameter / base diameter
	double t= sqrt( Dop * Dop - 1 ); //angle of tangent to involute arc, at limit
	double Ac= t - atan( t ); // angle of involute arc, at limit

//the output object
	ItemSet out;//= boost::make_shared< ItemSet >( ItemSet( ) );
	//angle between teeth
	double ang_tooth=	2 * PI / gp.tc;
	double ang_half=	PI / gp.tc - gp.of;

//testing show root, base, and pitch circles
#if 0
	{
		SP_ArcItem root_circle( new ArcItem( bg_point( 0, root_radius ), bg_point( 0, root_radius ), bg_point( 0, 0 ) ) );
		root_circle->SetColor( RGB( 0, 0, 255 ) );
		root_circle->SetMill( );
		out.push_back( root_circle );

		SP_ArcItem base_circle( new ArcItem( bg_point( 0, base_radius ), bg_point( 0, base_radius ), bg_point( 0, 0 ) ) );
		base_circle->SetMill( );
		base_circle->SetColor( RGB( 0, 0, 255 ) );
		out.push_back( base_circle );

		SP_ArcItem pitch_circle( new ArcItem( bg_point( 0, pitch_radius ), bg_point( 0, pitch_radius ), bg_point( 0, 0 ) ) );
		pitch_circle->SetMill( );
		pitch_circle->SetColor( RGB( 0, 255, 255 ) );
		out.push_back( pitch_circle );
	}
#endif

//root to det line
	bg_point root_start( root_radius, 0 );
	bg_point det_begin(  base_radius, 0 );
	SP_LineItem di_two( boost::make_shared< LineItem >( root_start, det_begin ) );
	out.push_back( di_two );

	//"Gear Drawing with Bezier Curves" is just not right
	//	addendum and dedendum are hard coded!

	//create line string from involute curve
	//because the radius of the arc grows through the iteration
	//   the arcs that need the most correction come close together to start.
	// with 12 iterations I saw errors of at most .0002 with the gear:
	//			gear_params_t params( 18, 20, 5 );

#define ITERS 12
	SP_PolyItem ls( boost::make_shared< PolyItem >( ) );
	for( size_t step= 0; step <= ITERS; ++step )
	{
		double arc= t / ITERS * step;
		*ls+= bg_point( base_radius * ( cos( arc ) + arc * sin( arc ) ),
						base_radius * ( sin( arc ) - arc * cos( arc ) ) );
	}

#if 0 // CHECK ERROR

		//creates a set of involute points to measure on the screen
		for( size_t step= 0; step <= ITERS; ++step )
		{
			double base_arc= t / ITERS * step;
			double arc= t / ITERS * ( (double)step + .5 );
			bg_point tp( base_radius * ( cos( arc ) + arc * sin( arc ) ),
				base_radius * ( sin( arc ) - arc * cos( arc ) ) );
			SP_PointItem point_test( new PointItem( tp ) );
			out.push_back( point_test );
		}

#endif// CHECK ERROR

	out.push_back( ls );

	//this check is very close, x is equal, y is just a little off.
	//assert( bg_polar( Ac, outer_radius ) == ls->back( ) );
	//stash last point for next op
	bg_point arc_end( ls->back( ) );

	//to examin error of min ITERS
#ifdef EXAMIN_ITERS
	{
		SP_PolyItem lsc( new PolyItem );
		for( size_t step= 0; step <= 55; ++step )
		{
			double arc= t / 55 * step;
			*lsc+= ( bg_point( base_radius * ( cos( arc ) + arc * sin( arc ) ),
				base_radius * ( sin( arc ) - arc * cos( arc ) ) ) );
		}

		lsc->SetMill( );
		out.push_back( lsc );
	}
#endif EXAMIN_ITERS

//top of tooth
	bg_point outside_end( bg_polar( ang_half - Ac, outer_radius ) );
	SP_ArcItem outside_cut( boost::make_shared< ArcItem >( arc_end, outside_end, bg_point( 0, 0 ) ) );
	out.push_back( outside_cut );


	TRACE( "``````\n" );
//mirror the involute use the previous line string or generate new?
	//the mirror method

	//gen new method, order inward
	SP_PolyItem ls2( boost::make_shared< PolyItem >( ) );
	for( auto lit= ls->rbegin( ); lit != ls->rend( ); ++lit )
	{
		//mirror by y axis
		bg_point lspoint( lit->get_x( ), -lit->get_y( ) );
		//rotate from origin to back of tooth and ls2.push_back
		*ls2+= bg_rotate( ang_half, lspoint );
	}

	out.push_back( ls2 );

	//det to root line
	SP_LineItem di_two2(boost::make_shared< LineItem >( bg_polar( ang_half, base_radius ), bg_polar( ang_half, root_radius ) ) );
	out.push_back( di_two2 );

	//root arc
	SP_ArcItem root_arc( boost::make_shared< ArcItem >( di_two2->get_bg_line( ).second, bg_polar( ang_tooth, root_radius ), bg_point( 0, 0 ) ) );
	out.push_back( root_arc );

	return out;
}

// ..........................................................................
//copy and rotate a tooth to create a whole gear....
ItemSet gear_generate( const ItemSet& tooth, const gear_params_t& gp )
{
	double ang_tooth=	2 * PI / gp.tc;
	ItemSet out;

	//fisrt tooth, just copy
	out= tooth;
	for( size_t step= 1; step < gp.tc; ++step )
	{
		double polar= 2 * PI * ( (double)step / gp.tc );
		ItemSet next;
		for(auto it= tooth.cbegin(); it != tooth.cend(); ++it )
			next.push_back( rotate_object( polar, *it ) );
		out.insert( next.begin( ), next.end( ) );
	}
	return out;
}
