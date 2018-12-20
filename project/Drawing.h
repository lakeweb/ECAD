

// Drawing.h

#pragma once
#ifndef DRAWING_H_12_18_2018
#define DRAWING_H_12_18_2018
#define SET_OSFLOAT std::fixed << std::setw( 7 ) << std::setprecision( 3 ) << std::setfill( L' ' )

// ..........................................................................
// ..........................................................................
class DrawArea;
class DrawingObects// : public BaseItem
{
	friend DrawArea;
	typedef object_set_t::iterator iterator;

public:
	DrawingObects( layer_set_t& layers )
		:layers( layers )
		//,id( 0 )
	{ }
	void set_offset( const bg_point& pt ) { offset= pt; }
	void push_back( const ItemSet& obj ) { primitives.push_back( obj ); }
	iterator begin( ) { return primitives.begin( ); }
	iterator end( ) { return primitives.end( ); }
	size_t size( ) const { return primitives.size( ); }
	object_set_t& get_set( ) { return primitives; }
	const layer_set_t& get_layers( ) const { return layers; }
	bg_box& get_viewport( ) { return viewport; }
	ItemSet* back_ptr( ) { return &primitives.back( ); }
	void InsertLayer(size_t id, const std::tstring& label) { layers.insert(cad_layer(id, label.c_str())); }
	void clear() { layers.clear(); primitives.clear(); offset.set_x(0); offset.set_y(0); }
	const object_set_t& get_objects_set() const { return primitives; }
	void set_viewport(const bg_box& in) { viewport = in; }
	void insert_font(const font_ref& font) { fontref_set.insert(font); }
	const font_ref_set* get_fontset() const { return &fontref_set; }
private:
	bg_box viewport;
	layer_set_t& layers;
	bg_point offset;
	object_set_t primitives;
	//new 11/22/2018 here we keep a session font set
	font_ref_set fontref_set; //fixed given for size, etc.
};

// ..........................................................................
//drawing.....
typedef boost::shared_ptr< CPen > SP_CPen;
//typedef std::vector< SP_CPen > VSP_CPen;
/////typedef std::vector< SP_CBrush > VSP_CBrush;

AFX_STATIC_DATA	int penStylesRnd=  PS_SOLID | PS_ENDCAP_ROUND | PS_GEOMETRIC;
AFX_STATIC_DATA	int penStylesFlat=  PS_SOLID | PS_ENDCAP_FLAT |PS_GEOMETRIC;

/*
PS_SOLID 		The pen is solid.
PS_DASH			The pen is dashed.
PS_DOT		 	The pen is dotted.
PS_DASHDOT		The pen has alternating dashes and dots.
PS_DASHDOTDOT 	The pen has dashes and double dots.
PS_NULL 		The pen is invisible.
PS_INSIDEFRAME 	The pen is solid. When this pen is used in any GDI drawing function that takes a bounding rectangle,
				the dimensions of the figure are shrunk so that it fits entirely in the bounding rectangle,
				taking into account the width of the pen. This applies only to geometric pens.
*/
// ..........................................................................
struct PCPEN
{
	HPEN hndl;
	LOGPEN lp;

	PCPEN( COLORREF icolor, size_t iwidth, int istyles= PS_SOLID) //for comp
	{
		hndl = nullptr;
		lp.lopnColor = icolor;
		lp.lopnStyle = istyles;
		lp.lopnWidth.x = iwidth;
	}
	~PCPEN() { DeleteObject(hndl); }
	void Create() const { assert(!hndl); const_cast<HPEN>(hndl)=::CreatePenIndirect(&lp); }
	friend bool operator < (const PCPEN& a, const PCPEN& b) { return 0 > memcmp(&a.lp, &b.lp, sizeof(LOGPEN)); }
};
typedef std::set< PCPEN > PenSet;
typedef PenSet::iterator PenSetIt;

extern LOGFONTW default_logfont;

// ..........................................................................
struct PCFONT //: public CFont
{
	HGDIOBJ hndl;
	boost::shared_ptr<CFont> sp_CFont;
	size_t height;
	PCFONT(const LOGFONTW* lf)
	{
		//for what ever reason, could not get this to work properly
//		hndl= ::CreateFontIndirect(lf);
		//So, just use a CFont object, at that, it has to be used as a 'new', derive won't work....
		sp_CFont = boost::make_shared<CFont>();
		BOOL bDid= sp_CFont->CreateFontIndirectW(lf);
		height = lf->lfHeight;
	}
	PCFONT(size_t size)//dummy for find
		:height(size) {}

	PCFONT() :height(0) {}
	~PCFONT() {}// { ::DeleteObject(hndl); }
//	PCFONT(PCFONT& f) :height(f.height) { hndl = f.hndl; }

	friend bool operator<(const PCFONT& a, const PCFONT& b) { return a.height < b.height; }
};
using FontSet = std::set< PCFONT >;
using FontSetIt = FontSet::iterator;

// ..........................................................................
//brush is not used yet! or never?
using SP_CBrush = boost::shared_ptr< CBrush >;
struct PCBRUSH
{
	LOGBRUSH lb;
	SP_CBrush spCBrush;
	PCBRUSH(COLORREF icolor, uint64_t inStyles) //for comp
	{
		spCBrush = boost::make_shared<CBrush>();//this is uninitialized....
		lb.lbColor = icolor;
		lb.lbStyle = (UINT)inStyles;
		lb.lbHatch = 0;
	}
	~PCBRUSH() {}
	void Create() const { spCBrush->CreateBrushIndirect(&lb); }

	friend bool operator < (const PCBRUSH& a, const PCBRUSH& b) {return 0 < memcmp(&a.lb,&b.lb,sizeof(LOGBRUSH));}
	operator CBrush* () { return spCBrush.get(); }
	operator HGDIOBJ () { return *spCBrush; }//doesn't work
	HGDIOBJ Handle() { return (HGDIOBJ)*spCBrush; }
};
typedef std::set< PCBRUSH > BrushSet;
typedef std::set< PCBRUSH >::iterator BrushSetIt;

// ..........................................................................
//struct BrushInfo
//{
//	COLORREF color;
//	UINT	type;
//};

// ..........................................................................
struct DrawExtent
{
	CRect& rect_target;
	long& off_x;
	long& off_y;
	double& zoom;
	const font_ref_set* fonts;
	bool bVert;
	bool bHorz;
	//font_ref_set fontref_set; //fixed given for size, etc.

	DrawExtent(CRect& rect, long& ix, long& iy, double& izoom)
		:rect_target(rect)
		, off_x(ix)
		, off_y(iy)
		, zoom(izoom)
		, fonts(nullptr)
		, bVert(false)
		, bHorz(false)
	{ }
	void set_all() { bVert = true; bHorz = true; }
	void clear_all() { bVert = false; bHorz = false; }
	void set_font_set(const font_ref_set* fontsin) { fonts = fontsin; }
};

// ..........................................................................
class DrawArea
{
	CDC* pDC;
	DrawExtent draw;
	//PCFONT font; //for default

	FontSet font_set; //dynamic font set for drawing
	const PCFONT* pCurFont;

	PenSet pens;
	const PCPEN* pCurPen;
	std::vector<const PCPEN*> pen_stack;

	BrushSet brushes;
	SP_CBrush pCurBrush;
//	BrushInfo lastBrush;
	//LOGBRUSH pen_logbrush;

public:
	DrawArea(CDC* pInDC, DrawExtent& iDraw);
	~DrawArea() {}
	void SetFonts(const font_ref_set* fonts) { draw.set_font_set(fonts); }

	void SetPen(COLORREF lb, double width, int inStyle= PS_SOLID);
	void SetPen(const style_line style){ SetPen(style.line_color, style.width, style.pen_style);}
	void SetPen(const style_line* style) { if (style)SetPen(*style); }
	void PushPen();
	void PopPen();
	void SetFont(LOGFONTW* plog_font);
	void Setfont(const font_ref& font);
	void SetBrush(const style_fill* p_fill);
	void DrawArea::DrawLine(const bg_line& line){pDC->MoveTo(GetDrawPoint(line.first));	pDC->LineTo(GetDrawPoint(line.second));	}
	void DrawLine(const LineItem& line) {void DrawLine(const bg_line& line);}
	void DrawCross( CPoint pt );
	void DrawBoardCross( bg_point const& pt, size_t size );
	void DrawRect(const bg_box& rect) { pDC->Rectangle(GetDrawRect(rect)); }
	void DrawArc(const ArcItem& arc) { pDC->Arc(GetDrawRect(arc.get_box()), GetDrawPoint(arc.get_a()), GetDrawPoint(arc.get_b())); }
	void DrawEllipes( const bg_box& box );
	void DrawObject( const BaseItem& obj);
	void DrawObjects( const DrawingObects& obj );
	void DrawPoint( const bg_point );
	void DrawRawPoint( const bg_point );
	void DrawBezier( const bg_point& p0, const bg_point& p1, const bg_point& p2, const bg_point& p3 );
	void DrawPoly( const bg_linestring_t& ls );
	void DrawText(const TextItem& text);
	void DrawItemSet(const ItemSet& set);
	void DrawTest( );
	void DrawGrid( );
	CPoint GetDrawPoint(const bg_point& pt) const {
		return CPoint((int)(pt.get_x() * draw.zoom + draw.off_x), (int)(pt.get_y() * draw.zoom+ draw.off_y));}
	CRect GetDrawRect(const bg_box& box) const {
		return CRect(GetDrawPoint(box.min_corner()), GetDrawPoint(box.max_corner()));}
	void SetStyle(const SP_style_base fill);

};

// ............................................................................
inline void DrawArea::DrawRawPoint( const bg_point pt ) //temp
{
	pDC->Rectangle(
		(long)( pt.get_x( ) - 1 )
		,(long)( pt.get_y( )- 1 )
		,(long)( pt.get_x( )+ 1 )
		,(long)( pt.get_y( )+ 1 )
		);
}

// ............................................................................
inline void DrawArea::DrawEllipes(const bg_box& box)
{
	CPoint a(GetDrawPoint(box.min_corner()));
	CPoint b(GetDrawPoint(box.max_corner()));
	pDC->Ellipse(a.x, a.y, b.x, b.y);
}

//Pin item for electtrical CAD This is an `Object.h` but is an extention to basic drawings.....
// ............................................................................
struct PinItem : public LineItem
{
	//electrical id, pin number
	TextItem pin;
	double orient;
	TextItem text;
	PinItem(const TextItem& pin, const TextItem& name)
		:pin(pin)
		,text(name)
	{
		bHasDraw = true;
	}
	PinItem(Component::ScPin& lin)
		:text(lin.name.name, bg_point(lin.name.pos.x, lin.name.pos.y))
		, pin(lin.designate.name, bg_point(lin.designate.pos.x, lin.designate.pos.y))
		, orient(lin.rotate)
	{
		text.just = (ObjectSpace::justify)lin.name.just;
		pin.just = (ObjectSpace::justify)lin.designate.just;
		if (orient == 180)
			set(bg_line(bg_point(lin.pos.x - lin.length, lin.pos.y), bg_point(lin.pos.x, lin.pos.y)));
		else
			set(bg_line(bg_point(lin.pos.x, lin.pos.y), bg_point(lin.pos.x + lin.length, lin.pos.y)));

		bHasDraw = true;
	}
	const style_line& GetStyle() const { return *dynamic_cast<style_line*>(sp_style.get()); }
	virtual void draw(DrawArea& draw);
	virtual SP_BaseItem get_SP() { return boost::make_shared< PinItem >(*this); }
};

struct SegItem : public LineItem
{
	double width;
	size_t net;
	SegItem(const bg_point& start, const bg_point& end, const size_t ilayer, const double width, size_t net)
		:LineItem(start, end), width(width), net(net) {
		layer = ilayer;
		bHasDraw = true;
	}
	virtual SP_BaseItem get_SP() { return boost::make_shared< SegItem >(*this); }
	virtual void draw(DrawArea& draw);
};

#endif// DRAWING_H_12_18_2018
