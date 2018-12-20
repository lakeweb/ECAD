
#include "stdafx.h"
#include "shared.h"
#include "Geometry.h"
#include "objects.h"
#include "Drawing.h"

// ............................................................................
void DrawArea::SetPen( COLORREF const& color, size_t width, int inStyle )
{
	width= width ? width : 1;
	pen_logbrush.lbColor= color;
	PCPEN pen( color, width, inStyle );
	PenSetIt it= pen_set.find( pen );
	if( it == pen_set.end( ) )
	{
		pen.sp_Pen= SP_CPen( new CPen(inStyle, width, &pen_logbrush ) );
		pDC->SelectObject( const_cast< CPen* >( pen.sp_Pen.get( ) ) );
		pen_set.insert( pen );
		spLastPen= pen.sp_Pen;
		//TRACE( "%3.0d NewPen: %I64d\n", pen_set.size( ), (__int64)pen_logbrush.lbColor | ( (__int64)width << 32 ) );
	}
	else if( it->sp_Pen.get( ) != spLastPen.get( ) )
	{
		pDC->SelectObject( const_cast< CPen* >( it->sp_Pen.get( ) ) );
		spLastPen= it->sp_Pen;
	}
}

//// ............................................................................
void DrawArea::PopPenBrush( )
{
}

//// ............................................................................
void DrawArea::SetDCBrush( COLORREF const& color )
{
	pDC->SelectStockObject( DC_BRUSH );
	pDC->SetDCBrushColor( color );
}

//// ............................................................................
//void DrawArea::SetBrush( LOGBRUSH& lb, int inStyle )
//{
//	PCBRUSH brush( lb.lbColor, inStyle );
//	BrushSetIt it= brush_set.find( brush );
//	if( it == brush_set.end( ) )
//	{
//		brush.sp_Brush= SP_CBrush( new CBrush( &lb,  ) );
//		pDC->SelectObject( const_cast< CPen* >( brush.sp_Pen.get( ) ) );
//		pen_set.insert( pen );
//		pLastPen= pen.sp_Pen.get( );
//		//TRACE( "%3.0d NewPen: %I64d\n", pen_set.size( ), (__int64)lb.lbColor | ( (__int64)width << 32 ) );
//	}
//	else if( it->sp_Brush.get( ) != pLastPen )
//	{
//		pDC->SelectObject( const_cast< CPen* >( it->sp_Brush.get( ) ) );
//		pLastPen= const_cast< CBrush* >( it->sp_Brush.get( ) );
//	}
//}
//

// ............................................................................
// adapted from: http://stackoverflow.com/questions/22822836/type-switch-construct-in-c11
#include <typeswitch.h>
void DrawArea::DrawObject( const ItemObj& obj )
{
	SetPen( obj.GetColor( ), 1, penStylesRnd );

	typecase( obj,

	[ & ]( const LineItem& pa ) {
		DrawLine( pa.get_bg_line( ) );
	},
	[ & ]( const ArcItem& pa ) {
		DrawArc( pa );
	},
	[ & ]( const RectItem& pa ) {
		DrawRect( pa.get_bg_box( ) );
	},
	[ & ]( const EllipesItem& pa ) {
		DrawEllipes( pa.get_bg_box( ) );
	},
	[ & ]( const BezierCubeItem& pa ) {
		DrawBezier( pa.get_point0( ), pa.get_point1( ), pa.get_point2( ), pa.get_point3( ) );
	},
	[ & ]( const PolyItem& pa ) {
		DrawPoly( pa.get_item( ) );
	});
}

// ............................................................................
void DrawArea::DrawObjects( const DrawingObects& objs )
{
	const layer_set_t& layers= objs.get_layers( );
	for( auto bit= objs.primitives.begin( ); bit != objs.primitives.end( ); ++bit )
	{
		 auto layer= layers.find( layer_t( bit->GetLayer( ), nullptr ) );
		 if( layer == layers.end( ) || ! layer->enabled )
			continue;
		for( auto it= bit->begin( ); it != bit->end( ); ++it )
		{
			DrawObject( *it->get( ) );
		}
	}
}

// ............................................................................
void DrawArea::DrawArc( const ArcItem& arc )
{
	CRect rect( GetDrawRect( arc.get_box( ) ) );
	//SetPen( RGB( 220, 220, 220 ), 1, penStylesFlat );
	//pDC->Rectangle( rect );
	CPoint pt1( GetDrawPoint( arc.get_a( ) ) );
	CPoint pt2( GetDrawPoint( arc.get_b( ) ) );
	SetPen( RGB( 0, 0, 0 ), 1, penStylesFlat );
	pDC->Arc( rect, pt2, pt1 );

#ifdef TEST_POINT
	SetPen( RGB( 255, 0, 0 ), 2, penStylesFlat );
	DrawPoint( arc.get_b( ) );
#endif
}

// ............................................................................
void DrawArea::DrawLine( const bg_line& line )
{
	long t1x= (long)( line.first.get_x( ) * draw.zoom + draw.of_x );
	long t1y= (long)( line.first.get_y( ) * draw.zoom + draw.of_y );
	pDC->MoveTo( t1x, t1y );
	long t2x= (long)( line.second.get_x( ) * draw.zoom + draw.of_x );
	long t2y= (long)( line.second.get_y( ) * draw.zoom + draw.of_y );
	pDC->LineTo( t2x, t2y );

#ifdef TEST_POINT
	SetPen( RGB( 255, 0, 0 ), 2, penStylesFlat );
	DrawPoint( bg_point( t2x, t2y ) );
#endif
}

// ............................................................................
void DrawArea::DrawEllipes( const bg_box& box ) const
{
	long t1x= (long)( ( box.min_corner( ).get_x( ) ) * draw.zoom + draw.of_x );
	long t1y= (long)( ( box.min_corner( ).get_y( ) ) * draw.zoom + draw.of_y );
	long t2x= (long)( ( box.max_corner( ).get_x( ) ) * draw.zoom + draw.of_x );
	long t2y= (long)( ( box.max_corner( ).get_y( ) ) * draw.zoom + draw.of_y );
	pDC->Ellipse( t1x, t1y, t2x, t2y );
}

// ............................................................................
void DrawArea::DrawTest( )
{
	POINT pt[4] = { {  20,  12 }, {  40, 50 }, { 60,  50 }, { 80, 50 } };
	pDC->PolyBezier( pt, 4 );

	SetPen( RGB( 255, 0, 0 ), 4, penStylesFlat );
	DrawRawPoint( bg_point( pt[ 0 ].x, pt[ 0 ].y ) );
	DrawRawPoint( bg_point( pt[ 1 ].x, pt[ 1 ].y ) );
	DrawRawPoint( bg_point( pt[ 2 ].x, pt[ 2 ].y ) );
	DrawRawPoint( bg_point( pt[ 3 ].x, pt[ 3 ].y ) );
}

// ............................................................................
void DrawArea::DrawGrid( )
{
	CSize size= pDC->GetWindowOrg( );
	for( size_t i= 0; i < 12; ++i )
		DrawLine( bg_line( bg_point( 0, i ), bg_point( 12, i ) ) );
	//POINT pt[4] = { {  20,  12 }, {  40, 50 }, { 60,  50 }, { 80, 50 } };
	//pDC->PolyBezier( pt, 4 );

	//SetPen( RGB( 255, 0, 0 ), 4, penStylesFlat );
	//DrawRawPoint( bg_point( pt[ 0 ].x, pt[ 0 ].y ) );
	//DrawRawPoint( bg_point( pt[ 1 ].x, pt[ 1 ].y ) );
	//DrawRawPoint( bg_point( pt[ 2 ].x, pt[ 2 ].y ) );
	//DrawRawPoint( bg_point( pt[ 3 ].x, pt[ 3 ].y ) );
}

// ............................................................................
void DrawArea::DrawBezier( const bg_point& p0, const bg_point& p1, const bg_point& p2, const bg_point& p3 )
{
    POINT pt[ 4 ];
	pt[ 0 ].x= (long)( p0.get_x( ) * draw.zoom + draw.of_x );
	pt[ 0 ].y= (long)( p0.get_y( ) * draw.zoom + draw.of_y );
	pt[ 1 ].x= (long)( p1.get_x( ) * draw.zoom + draw.of_x );
	pt[ 1 ].y= (long)( p1.get_y( ) * draw.zoom + draw.of_y );
	pt[ 3 ].x= (long)( p2.get_x( ) * draw.zoom + draw.of_x );
	pt[ 3 ].y= (long)( p2.get_y( ) * draw.zoom + draw.of_y );
	pt[ 2 ].x= (long)( p3.get_x( ) * draw.zoom + draw.of_x );
	pt[ 2 ].y= (long)( p3.get_y( ) * draw.zoom + draw.of_y );
	pDC->PolyBezier( pt, 4 );
	DrawPoint( p1 );
	DrawPoint( p3 );
}

// ............................................................................
void DrawArea::DrawPoly( const bg_linestring_t& ls )
{
	if( ! ls.size( ) )
	{
		std::cout << "atempted to draw a null polyline\n";
		return;
	}
	long t1x= (long)( ls.front( ).get_x( ) * draw.zoom + draw.of_x );
	long t1y= (long)( ls.front( ).get_y( ) * draw.zoom + draw.of_y );
	pDC->MoveTo( t1x, t1y );
	for( bg_linestring_cit_t it= ls.begin( ); it != ls.end( ); ++it )
	{
		long t2x= (long)( it->get_x( ) * draw.zoom + draw.of_x );
		long t2y= (long)( it->get_y( ) * draw.zoom + draw.of_y );
		pDC->LineTo( t2x, t2y );
	}
	//SetPen( RGB( 255, 0, 0 ), 2, penStylesFlat );
	//DrawPoint( bg_point( t2x, t2y ) );
}

// ............................................................................
/*
void DrawArea::DrawShape( bg_point const& bound_min, bg_point const& bound_max, const pad_flag flag )
{
	CRect srect(
		(long)( bound_min.get_x( ) / draw.zoom + draw.of_x ),
		(long)( bound_min.get_y( ) / draw.zoom + draw.of_y ),
		(long)( bound_max.get_x( ) / draw.zoom + draw.of_x ),
		(long)( bound_max.get_y( ) / draw.zoom + draw.of_y )
	);
	switch( flag & PCFS_SHAPE_MASK )
	{
	case PCSF_ROUND:
		pDC->Ellipse( srect );
		break;

	case PCSF_RECT:
		pDC->Rectangle( srect );
		break;

	default:
		assert( false );
	}
}

/*
// ............................................................................
void DrawArea::DrawConnect( Connect& con )
{
	long thick= (long)( con.size / 2 );
	CRect rect(
		(long)( ( con.x1( ) - thick ) / draw.zoom + draw.of_x )
		,(long)( ( con.y1( ) - thick ) / draw.zoom + draw.of_y )
		,(long)( ( con.x1( ) + thick ) / draw.zoom + draw.of_x )
		,(long)( ( con.y1( ) + thick ) / draw.zoom + draw.of_y )
	);
	pDC->Ellipse( rect );
}
*/
// ............................................................................
void DrawArea::DrawCross( CPoint pt )
{
	CPen pen( PS_SOLID, 2, RGB( 100, 100, 100 ) );
	pDC->SelectObject( &pen );
	pDC->MoveTo( pt.x - 10, pt.y );
	pDC->LineTo( pt.x + 10, pt.y );
	pDC->MoveTo( pt.x, pt.y - 10 );
	pDC->LineTo( pt.x, pt.y + 10  );
}

// ............................................................................
void DrawArea::DrawBoardCross( bg_point const& pt )
{
	CPen pen( PS_SOLID, 2, RGB( 230, 230, 230 ) );
	CPen* op= pDC->SelectObject( &pen );
	long x= (long)( pt.get_x( ) * draw.zoom ) + draw.of_x;
	long y= (long)( pt.get_y( ) * draw.zoom ) + draw.of_y;
	pDC->MoveTo( x - 10, y - 10 );
	pDC->LineTo( x + 10, y + 10 );
	pDC->MoveTo( x + 10, y - 10 );
	pDC->LineTo( x - 10, y + 10  );
	pDC->SelectObject( op );
}

// ............................................................................
// ............................................................................
// ............................................................................
/*
SP_SSP_Node Locator::FindVia( bg_point in )
{
	SP_SSP_Node nodes( new SSP_Node );
	VSP_Connect vias( board.vias );

	UINT64 last= -1;
	auto it= vias.begin( );
	for( ; it != vias.end( ); ++it )
	{
		if( *it->get( ) == in )
		{
			SP_Node spN= SP_Node( new Node( *it, it->get( )->pt1 ) );
			nodes->insert( spN );
			break;
		}
	}
	return nodes;
}

// ............................................................................
UINT64 Locator::FindLines( bg_point in, SP_SSP_Node spNodes, VSP_Line spLines, UINT64 dist )
{
	UINT64 last= dist;
	auto it= spLines.begin( );
	for( ; it != spLines.end( ); ++it )
	{
		SP_Node spN1= SP_Node( new Node( *it, it->get( )->pt1 ) );
		SP_Node spN2= SP_Node( new Node( *it, it->get( )->pt2 ) );

		UINT64 dist1= (UINT64)bg::comparable_distance( spN1->pt( ), in );
		UINT64 dist2= (UINT64)bg::comparable_distance( spN2->pt( ), in );

		if( dist1 <= last )// || dist1 < last )
		{
			if( dist1 != last )
			{
				spNodes->clear( );
				last= dist1;
			}
			spNodes->insert( spN1 );
//			TRACE( "less1 dx: %d dy: %d \tdist: %I64u\n", bg::get< 0 >( spN1->pt( ) ), bg::get< 1 >( spN1->pt( ) ), dist1 );
		}
		if( dist2 <= last )// || dist1 < last )
		{
			if( dist2 != last )
			{
				spNodes->clear( );
				last= dist2;
			}
			spNodes->insert( spN2 );
		}
	}
	return last;
}

// ............................................................................
SP_SSP_Node Locator::FindClosestNodes( bg_point in )
{
	if( ! board.layers.size( ) )
		return SP_SSP_Node( new SSP_Node );

	SP_SSP_Node nodes( new SSP_Node );
	Layer* pLayer= board.GetLayer( 0 );
	if( ! pLayer )
		return SP_SSP_Node( new SSP_Node );

	VSP_Line lines= pLayer->vsp_lines;
	UINT64 top= FindLines( in, nodes, lines, boost::numeric::bounds< UINT64 >::highest( ) );

	auto tit= board.layers.begin( );
	++tit;
	SP_Layer sp_layerb( *tit );
	lines= sp_layerb->vsp_lines;
	UINT64 bottom= FindLines( in, nodes, lines, top );

	if( ! nodes->size( ) )
		return nodes;

	VSP_Connect sp_con( board.vias );
	for( auto it= sp_con.begin( ); it != sp_con.end( ); ++it )
		if( it->get( )->pt_1( ) == nodes->begin( )->get( )->pt( ) )
			nodes->insert( new Node( *it, it->get( )->pt1 ) );

	return nodes;
}
*/

