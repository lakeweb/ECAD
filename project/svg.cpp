
#include "stdafx.h"
#include "typeswitch.h"
#include "Geometry.h"
#include "objects.h"
#include "shared.h"
#include "Drawing.h"
#include "svg.h"
#include <sstream>

// ......................................................................
void svg_stream( std::ostream& os, const SP_BaseItem& in )
{
	typecase( *in,

	[ & ]( PointItem& pa ) {
		os << " " << pa.get_bg_point( ).get_x( ) << " " << pa.get_bg_point( ).get_y( );
	},
	[ & ]( LineItem& pa ) {
		os << " " << pa.get_bg_line( ).first.get_x( ) << " "
			<< pa.get_bg_line( ).first.get_y( ) << "  "
			<< pa.get_bg_line( ).second.get_x( ) << " "
			<< pa.get_bg_line( ).second.get_y( )
			;
	},
	[ & ]( const ArcItem& pa ) {
		double rad= bg::distance( pa.get_o( ), pa.get_a( ) );
		os << "M"
			<< " " << pa.get_a( ).get_x( )
			<< " " << pa.get_a( ).get_y( )
			<< " A "
			<< " " << rad
			<< " " << rad
			<< " 0 0 1"//					<-- TODO!!!
			<< " " << pa.get_b( ).get_x( )
			<< " " << pa.get_b( ).get_y( )
			<< " M "
			;
	},
	[ & ]( const RectItem& pa ) {
	},
	[ & ]( const EllipesItem& pa ) {
	},
	[ & ]( const BezierCubeItem& pa ) {
	},
	[ & ]( const PolyItem& pa ) {
		for( auto l : pa )
			os << " " << l.get_x( ) << " " << l.get_y( ) << " ";

	}
	);//typecase
}

// ......................................................................
void SVG_WriteFile( bfs::path& path, DrawingObects& draw )
{
	pugi::xml_document xml;
	//xml.load_file(path.wstring().c_str());// , "svg", CXML::modeProgFile );
	//xml.Clear( );
	//xml.CreateEmptyDoc( "svg" );
	//xml.type
	//xml.SetDOCTYPE( L"!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"" );
	pugi::xml_node top = xml;
	top.set_name("svg");
//	top.set_value();

	top.attribute("width").set_value("10cm");
	top.attribute("height").set_value("10cm");

	std::wstringstream strstm;
	strstm << draw.get_viewport( ).min_corner( ).get_x( )
		<< L' ' << draw.get_viewport( ).min_corner( ).get_y( )
		<< L' ' << draw.get_viewport( ).max_corner( ).get_x( )
		<< L' ' << draw.get_viewport( ).max_corner( ).get_y( );

	top.attribute("viewBox").set_value(strstm.str( ).c_str( ) );
	top.attribute("xmlns").set_value("http://www.w3.org/2000/svg");
	top.attribute("version").set_value("1.1" );
	top.attribute("title").set_value("tiny component");

	top.append_child("metadata").set_value("spur_gear_gen Copyright: Highlands Electronics 1996-2018");

	std::ostringstream os;
	os << "M ";
	for( auto it : draw )
		for( auto im : it )
			os << svg_obj( im ) << " ";

	//std::wstring buf( utf8_to_uni( ( os.str( ).c_str( ) ) ) );

	//XMLNODE node= svg.AddChild( "path" );
	//node.SetAttribute( "stroke-width", L".1" );
	//node.SetAttribute( "stroke", L"black" );
	//node.SetAttribute( "fill", L"none" );
	//node.SetAttribute( "d", buf.c_str( ) );

	//os.str("");
	//os.clear( );
	//node= svg.AddChild( "circle" );
	////<circle cx="10" cy="10" r="2" fill="red"/>
	//node.SetAttribute( "cx", L".0" ); 
	//node.SetAttribute( "cy", L".0" ); 
	//node.SetAttribute( "r", L".1" ); 
	//node.SetAttribute( "fill", L"blue" ); 

	//	node.SetAttribute( "fill", L"none" );

}

// ......................................................................
void SVG_ReadFile( bfs::path& path, object_set_t& draw )
{
	//CXML xml;
	//xml.Open( path.wstring( ).c_str( ), CXML::modeRead );
}

