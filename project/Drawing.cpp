
#include "stdafx.h"
#include "Geometry.h"
#include "objects.h"
#include "shared.h"
//#include "./include/recent_list.h"
#include "Drawing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {
		// ..................................................................

}
// ...................................................................
//https://stackoverflow.com/questions/29825498/sizing-font-to-fit-in-a-rectangle
/*
const int SIZE_IN_POINTS = 12;
LogFont.lfHeight = -MulDiv(SIZE_IN_POINTS, DC.GetDeviceCaps(LOGPIXELSY), 72);
https://stackoverflow.com/questions/12402675/how-to-scale-font-when-the-font-face-is-having-only-one-font-size-using-mfcs-dr
*/
LOGFONTW default_logfont = {
	12,				//LONG lfHeight in tenth of a point, using CreatePointFontIndirect(...);
	6,				//LONG lfWidth;
	0,				//LONG lfEscapement;
	0,				//LONG lfOrientation;
	FW_NORMAL,		//LONG lfWeight;
	FALSE,			//BYTE lfItalic;
	FALSE,			//BYTE lfUnderline;
	FALSE,			//BYTE lfStrikeOut;
	ANSI_CHARSET,	//BYTE lfCharSet;
	OUT_RASTER_PRECIS,//BYTE lfOutPrecision;
	CLIP_DEFAULT_PRECIS,//BYTE lfClipPrecision;
	DEFAULT_QUALITY,//BYTE lfQuality;
	FIXED_PITCH,	//BYTE lfPitchAndFamily;
	L"arial"		//WCHAR lfFaceName[32];
};

// ............................................................................
DrawArea::DrawArea(CDC* pInDC, /*DrawingObects& inBoard, */DrawExtent& iDraw)
	:pDC(pInDC)
	, draw(iDraw)
//	, font(&default_logfont)
	, pCurFont(nullptr)
	, pCurPen(nullptr)
{
	//pDC->SelectStockObject(NULL_BRUSH);
	//pDC->SelectObject(font.hndl);
}

// ............................................................................
void DrawArea::SetPen(COLORREF color, double width, int style)
{
	size_t dwidth = (size_t)(width*draw.zoom/4);
	auto it = pens.insert(PCPEN(color, dwidth, style));
	if (it.second)
		it.first->Create();
	if (&*it.first != pCurPen)
	{
		pCurPen = &*it.first;
		pDC->SelectObject(pCurPen->hndl);
	}
}

void DrawArea::PushPen()
{
	pen_stack.push_back(pCurPen);
}

//// ............................................................................
void DrawArea::PopPen( )
{
	assert(pen_stack.size());
	if (pen_stack.back())//need to have not pushed nullptr
	{
		pCurPen = pen_stack.back();
		pDC->SelectObject(pCurPen->hndl);
	}
	pen_stack.erase(pen_stack.end() - 1);
}

//// ............................................................................
void DrawArea::SetFont(LOGFONTW* lf)
{ //TODO
}

//// ............................................................................
void DrawArea::Setfont(const font_ref& font)
{
	auto& lf = default_logfont;
	auto hsize = font.height * draw.zoom;
	if (!hsize) hsize = 1;
	lf.lfHeight = (long)hsize;
	auto wsize = font.width * draw.zoom;
	if (!wsize) wsize = 1;
	lf.lfWidth = (long)wsize;
	std::cout << "font h: " << lf.lfHeight << " w: " << lf.lfWidth << std::endl;
	//don't go directly to insert so as not to needlessly create fonts.
	PCFONT nf(lf.lfHeight);//pseudo constructor....

	auto it = font_set.find(nf);
	if (it == font_set.end())
	{
		auto fit = font_set.insert(PCFONT(&lf));
		assert(fit.second);
		it = fit.first;
	}
	if (pCurFont != &*it)
	{
		pCurFont = &*it;
		pDC->SelectObject(*pCurFont->sp_CFont);
	}
}

//// ............................................................................
void DrawArea::SetBrush(const style_fill* sp_fill)
{
	auto it = brushes.insert(PCBRUSH(sp_fill->fill_color, sp_fill->fill_pattern));
	if (it.second)
		it.first->Create();
	if (it.first->spCBrush != pCurBrush)
	{
		pCurBrush= const_cast<PCBRUSH*>(&*it.first)->spCBrush;
		pDC->SelectObject(*pCurBrush);
	}
}

void DrawArea::SetStyle(const SP_style_base sp_fill)
{
}

// ............................................................................
void DrawArea::DrawObjects( const DrawingObects& objs )
{
	const layer_set_t& layers= objs.get_layers( );
	for( auto bit= objs.primitives.begin( ); bit != objs.primitives.end( ); ++bit )
	{
		auto layer= layers.find(cad_layer( bit->GetLayer( ), nullptr ) );
		if (! layer->enabled)
			continue;
		if (bit->GetStyleFill())
			SetBrush(bit->GetStyleFill());
		if (bit->GetStyleLine())
			SetPen(bit->GetStyleLine());
		for( auto it= bit->cbegin(); it != bit->cend(); ++it )
		{
			auto pset = dynamic_cast<ItemSet*>(it->get());
			if (pset)
			{
				SetBrush(it->get()->GetStyleFill());
				SetPen(it->get()->GetStyleLine());
			}
			if (!it->get()) continue;
			if (it->get()->HasDraw())
				(*it)->draw(*this);
			else
				DrawObject( *it->get( ));
		}
	}
}

// ............................................................................
// adapted from: http://stackoverflow.com/questions/22822836/type-switch-construct-in-c11
void DrawArea::DrawObject( const BaseItem& obj)
{

	typecase( obj,
		[&](const PointItem& pa)	{ DrawPoint(pa.get_bg_point() ); },
		[&](const LineItem& pa)		{ DrawLine(pa); },
		[&](const ArcItem& pa)		{ DrawArc(pa); },
		[&](const RectItem& pa)		{ DrawRect(pa.get_bg_box()); },
		[&](const EllipesItem& pa)	{ DrawEllipes( pa.get_bg_box()); },
		[&](const BezierCubeItem& pa) { DrawBezier(pa.get_point0(), pa.get_point1(), pa.get_point2(), pa.get_point3()); },
		[&](const TextItem& pa)		{ DrawText(pa); },
		[&](const ItemSet& pa)		{ DrawItemSet(pa); },
		[&](const PolyItem& pa)		{ DrawPoly( pa.get_item() ); }
	);
}

// ............................................................................
void DrawArea::DrawItemSet(const ItemSet& set)
{
	if (!set.size())
		return;
	const style_fill* ss = dynamic_cast<const style_fill*>(set.GetStyleFill());
	if (ss)
	{
		if (ss->fill_pattern != BS_NULL)//so far, here we assume  the item is a closed polygon.
			pDC->BeginPath();
		SetBrush(ss);
		SetPen(ss->line_color, 1, ss->fill_pattern);
	}
	typecase(*set.front().get(),
		[&](const PointItem& pa) {pDC->MoveTo(GetDrawPoint(pa.get_bg_point())); },
		[&](const LineItem& pa) {pDC->MoveTo(GetDrawPoint(pa.get_bg_line().first)); },
		[&](const BezierCubeItem& pa) {pDC->MoveTo(GetDrawPoint(pa.get_point0())); },
		[&](const PolyItem& pa) {}
	);
	for (auto& item : const_cast<ItemSet&>(set))
	{
		typecase(*item,
			[&](const PointItem& pa) {pDC->LineTo(GetDrawPoint(pa.get_bg_point())); },
			[&](const LineItem& pa) {pDC->LineTo(GetDrawPoint(pa.get_bg_line().second)); },
			[&](const BezierCubeItem& pa) {
			POINT pt[3] = { GetDrawPoint(pa.get_point1()), GetDrawPoint(pa.get_point2()), GetDrawPoint(pa.get_point3()) };
			pDC->PolyBezierTo(pt, 3);
#ifdef _DEBUGx
			DrawPoint(pa.get_point0());
			DrawPoint(pa.get_point1());
			DrawPoint(pa.get_point2());
			DrawPoint(pa.get_point3());
			std::cout << "DrawBply: " << pa.get_point0() << ';' << pa.get_point1()
				<< "::" << pa.get_point2() << ';' << pa.get_point3() << std::endl;
#endif
		},
			[&](const PolyItem& pa) {}
		);
	}
	if (ss->fill_pattern != BS_NULL)
	{
		pDC->EndPath();
		pDC->StrokeAndFillPath();
	}
}

// ............................................................................
#define POINT_SIZE (6/draw.zoom)
void DrawArea::DrawPoint( const bg_point pt ) //temp
{
	PushPen();
	SetPen(RGB(0, 0, 255), 3, PS_SOLID);
	bg_point p(pt);
	bg_point off(POINT_SIZE, POINT_SIZE);
	p -= off / 2;
	pDC->MoveTo( GetDrawPoint(p) );
	p += off;
	pDC->LineTo( GetDrawPoint(p) );
	p.y -= off.y;
	pDC->MoveTo( GetDrawPoint(p) );
	p.y += off.y;
	p.x -= off.x;
	pDC->LineTo(GetDrawPoint(p) );
	PopPen();
}

// ............................................................................
// ............................................................................


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
	SetPen( RGB( 255, 0, 0 ), 1, penStylesFlat );

	POINT pt[4] = {GetDrawPoint(p0),GetDrawPoint(p1),GetDrawPoint(p2),GetDrawPoint(p3) };
	pDC->PolyBezier( pt, 4 );

#ifdef _DEBUGx
	std::cout << "DrawBezr: " << p0 << ';' << p1 << "::" << p2 << ';' << p3 << std::endl;
	DrawPoint(p0);
	DrawPoint(p1);
	DrawPoint(p2);
	DrawPoint(p3);
#endif
}

// ............................................................................
void DrawArea::DrawPoly(const bg_linestring_t& ls)
{
	if( ! ls.size( ) )
	{
		std::cout << "atempted to draw a null polyline\n";
		return;
	}
	long t1x= (long)( ls.front( ).get_x( ) * draw.zoom + draw.off_x );
	long t1y= (long)( ls.front( ).get_y( ) * draw.zoom + draw.off_y );
	pDC->MoveTo( t1x, t1y );
	for( auto item : ls )//.begin( ); it != ls.end( ); ++it )
	{
		long t2x= (long)( item.get_x( ) * draw.zoom + draw.off_x );
		long t2y= (long)( item.get_y( ) * draw.zoom + draw.off_y );
		pDC->LineTo( t2x, t2y );
	}
}

inline bg_box GetBGRect(LPRECT pRect)
{
	return bg_box( bg_point(pRect->top, pRect->left), bg_point(pRect->bottom, pRect->right));
}

//// ............................................................................
// ............................................................................
//#define TEXT_TESTING
void DrawArea::DrawText(const TextItem& text)
{
	//if (!text.val.size())
	//	return;
	using namespace ObjectSpace;

	//    TextStyle "H82s5" (FontWidth 5) (FontHeight 82) (FontCharWidth 85)
	//Need to get font params from this.
	//size_t font_height = text.font ? text.font : 100;// phsudo default
	//if(*text.font.face != '\0')//GetTextExtent will fail otherwise
	Setfont(text.font);
	auto ext = pDC->GetTextExtent(text.val.c_str(), text.val.size());
	long tx = (long)(text.pos.get_x() * draw.zoom + draw.off_x);
	long ty = (long)(text.pos.get_y() * draw.zoom + draw.off_y);
	CRect rect(tx, ty, tx + ext.cx, ty + ext.cy); //default is UpperLeft
#ifdef TEXT_TESTING
	SetPen(RGB(0, 200, 0), 3, PS_DASH, true);
	pDC->Rectangle(rect);
	PopPen();
#endif
	//std::cout << pugi::as_utf8( text.val ) << " PRE  RECT: " << rect << std::endl;
	switch (text.just)
	{
	case jLeft: // 1
		rect.OffsetRect(0, -ext.cy / 2);
		break;
	case jRight: //2
		rect.OffsetRect(-ext.cx, -ext.cy / 2);
		break;
	case jLowerRight://8
		rect.OffsetRect(-ext.cx, 0);
		break;
	case jUpperRight://6
		rect.OffsetRect(-ext.cx, -ext.cy);
		break;
	case jUpperLeft://
		rect.OffsetRect(0, -ext.cy);
		break;
	case jLower://1 already right
		rect.OffsetRect(-ext.cx / 2, 0);
		break;
	case jLowerLeft://1 already right
		break;
	default:
		;//assert(false);
	}
	rect.NormalizeRect();
	pDC->DrawText(text.val.c_str(), text.val.size(), rect, DT_NOCLIP);
	//std::cout << font_cur->height << std::endl;
#ifdef TEXT_TESTING
	std::cout << "  RECT: " << rect << std::endl;
	SetPen(RGB(200, 0, 0), 3, PS_DASH, true);
	pDC->Rectangle(rect);
	PopPen();
#endif
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
void DrawArea::DrawCross( CPoint pt )
{
	CPen pen( PS_SOLID, 2, RGB( 0, 200, 0 ) );
	auto hold= pDC->SelectObject( &pen );
	pDC->MoveTo( pt.x - 10, pt.y );
	pDC->LineTo( pt.x + 10, pt.y );
	pDC->MoveTo( pt.x, pt.y - 10 );
	pDC->LineTo( pt.x, pt.y + 10  );
	pDC->SelectObject(hold);
}

// ............................................................................
void DrawArea::DrawBoardCross( bg_point const& pt, size_t size )
{
	long x= (long)( pt.get_x( ) * draw.zoom ) + draw.off_x;
	long y= (long)( pt.get_y( ) * draw.zoom ) + draw.off_y;
	pDC->MoveTo( x - size, y - size);
	pDC->LineTo( x + size, y + size);
	pDC->MoveTo( x + size, y - size);
	pDC->LineTo( x - size, y + size);
}

// ............................................................................
void PinItem::draw(DrawArea& draw)
{
	draw.DrawLine(this->get_bg_line());
	//draw.SetPen(RGB(255, 0, 0), 2, penStylesRnd, true);
	//draw.DrawEllipes(bg_box(get_bg_line().first - 25, get_bg_line().first + 25));
	draw.SetPen(RGB(255, 0, 0), 1, 0);
	draw.DrawBoardCross(get_bg_line().first, 5);
	draw.SetFont(&default_logfont);
	draw.DrawText(text);
	draw.DrawText(pin);
}

// ............................................................................
void SegItem::draw(DrawArea& draw)
{
	draw.SetPen(GetStyleLine());
	draw.DrawLine(this->get_bg_line());
}

std::ostream& operator<<(std::ostream& os, const PinItem& pin)
{
	os << "pin: " << pugi::as_utf8(pin.pin.val) << " name: " << pugi::as_utf8(pin.text.val)
		<< "\n   orient: " << pin.orient << " pin_pos: " << pin.get_bg_line()
		<< "\n   npos: " << pin.text.pos << " " << pin.text.just;
	return os;
}

void sub_dump(std::ostream& os, const ItemSet& items, std::string space) {
	space += "   ";
	for (auto& item : const_cast<ItemSet&>(items))
	{
		auto si = dynamic_cast<ItemSet*>(item.get());
		if (si)
			sub_dump(os, *si, space);
		else if(item.get())
			os << space << *item << std::endl;
	}
};

void DumpDrawingObects(std::ostream& os, DrawingObects& objs) {
	for (const auto& subs : objs)
		sub_dump(os, subs, "");
}
