
#include "stdafx.h"
#include "Geometry.h"
#include "objects.h"
#include "shared.h"
#include "Drawing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {
		// ..................................................................

#define OBJ_OS(x) {std::cout << x;}
//#define OBJ_OS(x) __noop;
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
DrawArea::DrawArea(CDC* pInDC, DrawExtent& iDraw)//, const sp_layer_set_type& layers)
	:pDC(pInDC)
	, draw(iDraw)
	//, layers(layers)
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
	size_t dwidth = (size_t)(width* (draw.zoom>1?draw.zoom:1));
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
	//don't go directly to insert so as not to needlessly create fonts.
	PCFONT nf(lf.lfHeight,font.color);//pseudo constructor....

	auto it = font_set.find(nf);
	if (it == font_set.end())
	{
		auto fit = font_set.insert(PCFONT(&lf,font.color));
		assert(fit.second);
		it = fit.first;
		//std::cout << "font h: " << lf.lfHeight << " w: " << lf.lfWidth << std::endl;
	}
	if (pCurFont != &*it)
	{
		pDC->SelectObject(*it->sp_CFont);
		if (pDC && (!pCurFont || it->color != pCurFont->color))
			pDC->SetTextColor(it->color);
		pCurFont = &*it;
	}
}

//// ............................................................................
void DrawArea::SetBrush(const SP_draw_style sp_fill)
{
	auto it = brushes.insert(PCBRUSH(sp_fill->fill_color, sp_fill->fill_style));
	if (it.second)
		it.first->Create();
	if (it.first->spCBrush != pCurBrush)
	{
		pCurBrush= const_cast<PCBRUSH*>(&*it.first)->spCBrush;
		pDC->SelectObject(*pCurBrush);
	}
}

using namespace ObjectSpace;
void DrawArea::SetStyle(const SP_draw_style sp_style)
{
	if (sp_style == sp_last_style)
		return;

	sp_last_style = sp_style;

	if (sp_style->pen_style != psNull) {
		SetPen(sp_style);
	}
	if( sp_style->fill_style != bsNull) {
		SetBrush(sp_style);
	}
}

// ............................................................................
void DrawArea::DrawObjects( const DrawingObect& objs )
{
//	players = &objs.get_layers();

	OBJ_OS( "\nDrawObjects: " << objs.size() << std::endl);

	for( auto bit : objs.primitives)
	{
		auto lit = std::find_if(objs.layers.begin(), objs.layers.end(),
			[&bit](const cad_layer& is) { return bit.layer_id == is.id; });

		if (lit != objs.layers.end() && !lit->enabled)
			continue;

		//OBJ_OS("  Drawing Primatives layer: " << lit->id << std::endl);
		SetStyle(bit.sp_style);
		for( auto it= bit.cbegin(); it != bit.cend(); ++it )
		{
			if (!it->get()) continue;
			auto pset = dynamic_cast<ItemSet*>(it->get());
			if (it->get()->HasDraw())
				(*it)->draw(*this);
			else
				DrawObject( *it->get( ));
		}
	}
}

// ............................................................................
// adapted from: http://stackoverflow.com/questions/22822836/type-switch-construct-in-c11
void DrawArea::DrawObject(const BaseItem& obj)
{
	if (auto sp_is = dynamic_cast<const ItemSet*>(&obj))
	{
		//if (sp_is->layer != -1)// && (layers.size() || !layers[obj.layer].enabled)
		//	//&& !dynamic_cast<const ItemSet*>(&obj))
		//	return;
	}
	auto test = dynamic_cast<const ItemSet*>(&obj);
	if (test && test->bIsPolygon)
		int thetest = 1;

	if (obj.sp_style)
		SetStyle(obj.GetStyle());

	OBJ_OS("  " << typeid(obj).name() << "  L: " << obj.layer << std::endl);
	std::cout << obj.sp_style << std::endl;
	
	if (obj.fdraw)
		obj.fdraw(*this);
	else
		typecase(obj,
			[&](const PointItem& pa) { DrawPoint(pa.get_bg_point()); },
			[&](const LineItem& pa) { DrawLine(pa); },
			[&](const ArcItem& pa) { DrawArc(pa); },
			[&](const RectItem& pa) { DrawRect(pa.get_bg_box()); },
			[&](const EllipesItem& pa) { DrawEllipes(pa.get_bg_box()); },
			[&](const BezierCubeItem& pa) { DrawBezier(pa.get_point0(), pa.get_point1(), pa.get_point2(), pa.get_point3()); },
			[&](const TextItem& pa) { DrawText(pa); },
			[&](const PolyItem& pa) { DrawPoly(pa.get_item()); },
			[&](const ItemSet& pa) { if (pa.bIsPolygon) DrawPolySet(pa); else DrawItemSet(pa); }
			);
}

void DrawArea::DrawItemSet(const ItemSet& obj)
{
	SetStyle(obj.GetStyle());
	if (obj.sp_font)
		Setfont(*obj.sp_font);
	for (auto it = obj.cbegin(); it != obj.cend(); ++it)
		DrawObject(*it->get());
		//typecase(*it->get(),
		//	[&](const PointItem& pa) { DrawPoint(pa.get_bg_point()); },
		//	[&](const LineItem& pa) { DrawLine(pa); },
		//	[&](const ArcItem& pa) { DrawArc(pa); },
		//	[&](const RectItem& pa) { DrawRect(pa.get_bg_box()); },
		//	[&](const EllipesItem& pa) { DrawEllipes(pa.get_bg_box()); },
		//	[&](const BezierCubeItem& pa) { DrawBezier(pa.get_point0(), pa.get_point1(), pa.get_point2(), pa.get_point3()); },
		//	[&](const TextItem& pa) { DrawText(pa); },
		//	[&](const ItemSet& pa) { if (pa.bIsPolygon) DrawPolySet(pa); else DrawItemSet(pa); },
		//	[&](const PolyItem& pa) { DrawPoly(pa.get_item()); }
		//);
}

// ............................................................................
void DrawArea::DrawPolySet(const ItemSet& set)
{
	if (!set.size())
		return;

	std::cout << std::boolalpha << "Drawing Polyset: " << set.bIsPolygon << " \n";
	if (set.bIsPolygon)
	{
		if (set.GetStyle()->fill_style != bsNull)//so far, here we assume  the item is a closed polygon.
			pDC->BeginPath();

		SetStyle(set.GetStyle());
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
	if (set.bIsPolygon && set.GetStyle()->fill_style != bsNull)
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
void DrawArea::DrawLine(const LineItem& line)
{
	//if (line.sp_style && GetFillStyle() != line.sp_style.get())
	//	SetPen(dynamic_cast<style_fill*>(line.sp_style.get()));
	//BUG_OS(" width: " << dynamic_cast<const SegItem&>(line).width
	//	<< " color: " << dynamic_cast<style_fill*>(line.sp_style.get())->line_color
	//	<< std::endl;
	//);
	pDC->MoveTo(GetDrawPoint(line.get_bg_line().first));
	pDC->LineTo(GetDrawPoint(line.get_bg_line().second));
	void DrawLine(const bg_line& line);
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
#define TEXT_TESTING
void DrawArea::DrawText(const TextItem& text)
{
	//if (!text.val.size())
	//	return;
	using namespace ObjectSpace;

	//    TextStyle "H82s5" (FontWidth 5) (FontHeight 82) (FontCharWidth 85)
	//Need to get font params from this.
	//size_t font_height = text.font ? text.font : 100;// phsudo default
	//if(*text.font.face != '\0')//GetTextExtent will fail otherwise
	if(text.sp_font)
		Setfont(*text.sp_font);
	auto ext = pDC->GetTextExtent(text.val.c_str(), text.val.size());
	long tx = (long)(text.pos.get_x() * draw.zoom + draw.off_x);
	long ty = (long)(text.pos.get_y() * draw.zoom + draw.off_y);
	CRect rect(tx, ty, tx + ext.cx, ty + ext.cy); //default is UpperLeft
#ifdef TEXT_TESTING
//	SetPen(draw_style(RGB(200, 100, 100), psSolid, 2, RGB(255, 0, 0)));
#endif
	OBJ_OS("   " << pugi::as_utf8( text.val ) << " PRE  RECT: " << rect << std::endl);
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

void sub_dump(std::ostream& os, const ItemSet& items, std::string space) {
	space += "   ";
	for (auto& item : const_cast<ItemSet&>(items))
	{
		auto si = dynamic_cast<ItemSet*>(item.get());
		if (si)
		{
			uint32_t clr = 0xffffffff;
			if (si->GetStyle())
				clr= si->GetStyle()->fill_color;
			os << space << "id: " << si->id << " clr: " << clr << std::endl;
			sub_dump(os, *si, space);
		}
		else if(item.get())
			os << space << *item << std::endl;
	}
};

void DumpDrawingObects(std::ostream& os, DrawingObect& objs) {
	for (const auto& subs : objs)
		sub_dump(os, subs, "");
}

void DrawExtent::SaveXML(XMLNODE node)
{
	auto child = node.GetElement("ExtentState");
	child.SetAttribute("x", off_x);
	child.SetAttribute("y", off_y);
	child.SetAttribute("zoom", zoom);
	child.SetElementArray("rect", (DWORD*)(LPCRECT)rect_target,4);
}

void DrawExtent::ReadXML(XMLNODE node)
{
	auto child = node.GetElement("ExtentState");
	off_x = child.GetAttributeInt("x");
	off_y = child.GetAttributeInt("y");
	zoom = child.GetAttributeDouble("zoom");
	DWORD* ptr;
	size_t check;
	auto test= child.GetElementArray("rect", &ptr, &check);
	if (!test)
		return;
	rect_target = (LPCRECT)ptr;
	delete[]ptr;
	bInitialized = true;
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
//	draw.SetPen(GetStyleLine());
	draw.DrawLine(this->get_bg_line());
}

