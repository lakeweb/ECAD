
/*
	objects.h
*/

#pragma once
#ifndef OBJECT_H
#define OBJECT_H

// ..........................................................................
//will use justfy as right's require measure and print
// ..................................................
namespace ObjectSpace {
	enum justify
	{
		jCenter,
		jLeft,
		jRight,
		jUpper,
		jLower,
		jUpperLeft,//5
		jUpperRight,
		jLowerLeft,
		jLowerRight,
	};
	//enum fill//where as this maybe should be pure functional, but for now, follow Tiny
	//{
	//	feNone,
	//	feBlack,
	//};
	/*
	HS_BDIAGONAL Downward hatch (left to right) at 45 degrees
    HS_CROSS Horizontal and vertical crosshatch
    HS_DIAGCROSS Crosshatch at 45 degrees
    HS_FDIAGONAL Upward hatch (left to right) at 45 degrees
    HS_HORIZONTAL Horizontal hatch
    HS_VERTICAL Vertical hatch
	*/
}//namespace

 // ..........................................................................
struct style_base : boost::enable_shared_from_this<style_base> {
	typedef boost::shared_ptr<style_base> SP_style_base;
	virtual SP_style_base get_SP() { return boost::make_shared<style_base>(*this); }
	operator SP_style_base () {return get_SP();}
};
using SP_style_base = boost::shared_ptr<style_base>;

// ..........................................................................
struct style_line : style_base
{
	uint32_t line_color;
	uint32_t pen_style;
	size_t   width; //boarder lines of polys/rects
	virtual SP_style_base get_SP() { return boost::make_shared<style_line>(*this); }
	operator SP_style_base() { return get_SP(); }
};

// ..........................................................................
struct style_fill : style_base
{
	//see info in shared.h for fill and pen styles..............
	uint32_t line_color;
	uint32_t pen_style;
	size_t   width; //boarder lines of polys/rects
	uint32_t fill_color;
	uint64_t fill_pattern;//will use Windows GDI flags for now.

	style_fill(const COLORREF col, const int brsh, const COLORREF coll, const size_t wdth)
		:fill_color(col), fill_pattern(brsh), line_color(coll), width(wdth) {}

	style_fill()
		: fill_color(RGB(255,255,255)), fill_pattern(BS_NULL), line_color(RGB(0,0,0)), width(1) {}

	virtual SP_style_base get_SP() { return boost::make_shared<style_fill>(*this); }
};
static const style_fill default_fill = { RGB(255,255,255),BS_NULL,RGB(0,0,0),1 };

// ..........................................................................
struct font_ref
{
	font_ref(size_t ref, size_t height= 100, size_t width = 50) :ref(ref), height(height), width(width) {} //for lookup
	size_t ref;
	std::string face;
	size_t height;
	size_t width;
	friend bool operator <(const font_ref& a, const font_ref& b){return a.ref < b.ref;}
};
const static font_ref default_font_ref(10);
using font_ref_set = std::set<font_ref>;

 // ..........................................................................
class DrawArea;
struct BaseItem
	:public boost::enable_shared_from_this< BaseItem >// and works for all derived
{
	SP_style_base sp_style;
	//double width;
	size_t layer;
	bool bHasDraw;
//	COLORREF color;

	virtual ~BaseItem() { }
	BaseItem()
		: layer(0)
		, sp_style(style_base().get_SP())
		, bHasDraw(false)
	{ }
	BaseItem(const BaseItem& o)
		: layer(o.layer)
		, sp_style(o.sp_style)
		, bHasDraw(o.bHasDraw)
	{ }
	BaseItem(size_t layer, SP_style_base ins)
		: layer(layer)
		, sp_style(ins)
		, bHasDraw(false)
	{ }
	BaseItem& operator = (BaseItem other) { return *this; }

	typedef boost::shared_ptr< BaseItem > SP_BaseItem;
	virtual SP_BaseItem get_SP() { return boost::make_shared< BaseItem >(*this); }
	operator SP_BaseItem() { return get_SP(); }
	size_t GetLayer()const { return layer; }
	void SetLayer(size_t i) { layer = i; }
	const style_line* GetStyleLine() const { return dynamic_cast<style_line*>(sp_style.get()); }
	const style_fill* GetStyleFill() const { return dynamic_cast<style_fill*>(sp_style.get()); }
	bool HasDraw() { return bHasDraw; }
	SP_style_base GetStyle() const { return sp_style; }
	virtual void draw(DrawArea&) { assert(false); }
	virtual void operator +=(const bg_point& pt) { assert(false); }
	virtual void rotate(const bg_point& pos, double angle) { assert(false); }
};

typedef boost::shared_ptr< BaseItem > SP_BaseItem;
typedef std::vector< SP_BaseItem > sp_obj_vect_type;
typedef std::vector< SP_BaseItem >::const_iterator sp_obj_vect_cit_t;

// ..........................................................................
struct PointItem : public BaseItem
{
	bg_point point;

	PointItem( const bg_point& a )
		:point( a )
	{ }
	operator const bg_point& ( ) { return point; }
	const bg_point& get_bg_point( ) const { return point; }
	void set( bg_point& in ) { point= in; }
	void operator = ( const PointItem& in ) { point= in.get_bg_point( ); }
	virtual void operator +=(const bg_point& pt) { point += pt; }
	virtual SP_BaseItem get_SP() { return boost::make_shared< PointItem >(*this); }
};
typedef boost::shared_ptr< PointItem > SP_PointItem;

// ..........................................................................
struct LineItem : public BaseItem
{
	bg_line line;

	LineItem( const bg_point& a, const bg_point& b )
		:line( a, b )
	{ }
	LineItem( const bg_line& a )
		:line( a )
	{ }
	LineItem( ) { }
	virtual SP_BaseItem get_SP( ) {	return boost::make_shared< LineItem >( *this ); }
	operator const bg_line& ( ) const { return line; }
	const bg_line& get_bg_line( ) const { return line; }
	void set( bg_line& in ) { line= in; }
};
typedef boost::shared_ptr< LineItem > SP_LineItem;

// ..........................................................................
/* in:
	a as start of arc
	b as end of arc
	o center of arc
*/
struct ArcItem : public BaseItem
{
	bg_point a;
	bg_point b;
	bg_point o;
	//
	double rad;

	friend void operator += ( ArcItem& a, const bg_point& b );
	ArcItem( ) { }

	ArcItem( const bg_point& a, const bg_point& b, const bg_point& o )
		:a( a )
		,b( b )
		,o( o )
	{
		rad= sqrt( ( ( o.get_x( ) - a.get_x( ) ) * ( o.get_x( ) - a.get_x( ) ) )
			+ ( ( o.get_y( ) - a.get_y( ) ) * ( o.get_y( ) - a.get_y( ) ) ) );
	}
	ArcItem(const bg_box& box) //used for creating box bound circle
	{
		//check that box is normalized in DEBUG.
		assert(box.min_corner().get_x() <= box.max_corner().get_x() && box.min_corner().get_y() <= box.max_corner().get_y());
		auto xm = (box.max_corner().get_x() - box.min_corner().get_x()) / 2;
		auto ym = (box.max_corner().get_y() - box.min_corner().get_y()) / 2;
		o.set_x(box.min_corner().get_x() + xm);
		o.set_y(box.min_corner().get_y() + ym);
		a.set_x(box.min_corner().get_x() + xm);
		a.set_y(box.min_corner().get_y());
		b = a;
		rad = sqrt(((o.get_x() - a.get_x()) * (o.get_x() - a.get_x()))
			+ ((o.get_y() - a.get_y()) * (o.get_y() - a.get_y())));
	}
	//box for DC
	bg_box get_box( ) const { return bg_box( bg_point( o.get_x( ) - rad, o.get_y( ) - rad ), bg_point( o.get_x( ) + rad, o.get_y( ) + rad ) ); }
	bg_point get_a( ) const { return a; }
	bg_point get_b( ) const { return b; }
	bg_point get_o( ) const { return o; }
	GEO_NUM get_rad( ) const { return rad; }
	//angles for DXF
	GEO_NUM get_start_ang( ) const;
	GEO_NUM get_end_ang( ) const;
	void rotate( const GEO_NUM angle, const bg_point& origin );
//	boost::shared_ptr<ArcItem> get_SP() { return boost::make_shared< ArcItem >(*this); }
	virtual SP_BaseItem get_SP() { return boost::make_shared< ArcItem >(*this); }
	//void set( const bg_point& ia, const bg_point& ib, const bg_point& io ) { a= ia; b= ib; o= io; }
};
typedef boost::shared_ptr< ArcItem > SP_ArcItem;

// ..........................................................................
struct RectItem : public BaseItem
{
	bg_box box;

	RectItem( const bg_point& a, const bg_point& b )
		:box( a, b )
		,BaseItem(0, style_fill(default_fill).get_SP())
	{ }
	RectItem( const bg_box& a )
		:box( a )
		, BaseItem(0, style_fill(default_fill).get_SP())
	{ }
	virtual SP_BaseItem get_SP() { return boost::make_shared< RectItem >(*this); }
	const bg_box& get_bg_box( ) const { return box; }
	GEO_NUM width( ) const { return box.max_corner( ).get_x( ) - box.min_corner( ).get_x( ); }
	GEO_NUM height( ) const { return box.max_corner( ).get_y( ) - box.min_corner( ).get_y( ); }

	bg_point& get_min_point( ) { return box.min_corner( ); }
	bg_point& get_max_point( ) { return box.max_corner( ); }
	void SetFill(const SP_style_base infill) { sp_style = infill; }
	virtual void operator +=(const bg_point& pt) { box.min_corner() += pt; box.max_corner() += pt; }
};
typedef boost::shared_ptr< RectItem > SP_RectItem;

// ..........................................................................
struct EllipesItem : public BaseItem
{
	bg_box box;

	EllipesItem( const bg_point& a, const bg_point& b )
		:box( a, b )
	{ }

	EllipesItem( bg_point& at, const double diameter )
		:box(at - diameter / 2, at + diameter / 2 )
	{ }

	EllipesItem( )
		:box( bg_point( 0, 0 ), bg_point( 0, 0 ) )
	{ }
	virtual SP_BaseItem get_SP() { return boost::make_shared< EllipesItem >(*this); }
	const bg_box& get_bg_box( ) const { return box; }
	void set( bg_box& in ) { box= in; }
};
typedef boost::shared_ptr< EllipesItem > SP_EllipesItem;

// ..........................................................................
struct BezierCubeItem : public BaseItem
{
	bg_point p0;
	bg_point p1;
	bg_point p2;
	bg_point p3;

	BezierCubeItem( const bg_point& p0, const bg_point& p1, const bg_point& p2, const bg_point& p3 )
		:p0( p0 )
		,p1( p1 )
		,p2( p2 )
		,p3( p3 )
	{ }

	BezierCubeItem( ) { }

	const bg_point& get_point0( ) const { return p0; }
	const bg_point& get_point1( ) const { return p1; }
	const bg_point& get_point2( ) const { return p2; }
	const bg_point& get_point3( ) const { return p3; }

	void set_point0( const bg_point& i ) { p0= i; }
	void set_point1( const bg_point& i ) { p1= i; }
	void set_point2( const bg_point& i ) { p2= i; }
	void set_point3( const bg_point& i ) { p3= i; }
	virtual SP_BaseItem get_SP() { return boost::make_shared< BezierCubeItem >(*this); }

	virtual void operator +=(const bg_point& pt) { p0 += pt; p1 += pt; p2 += pt; p3 += pt; }
};
typedef boost::shared_ptr< BezierCubeItem > SP_BezierCubeItem;

// ..........................................................................
struct PolyItem : public BaseItem
{
	bg_linestring_t ls;
	using iter = bg_linestring_t::const_iterator;
	using riter = bg_linestring_t::const_reverse_iterator;

	void operator += ( const bg_point& i ) { ls.push_back( i ); }
	iter begin( ) const { return ls.begin( ); }
	iter end( ) const { return ls.end( ); }
	riter rbegin( ) const { return ls.rbegin( ); }
	riter rend( ) const { return ls.rend( ); }
	size_t size( ) const { return ls.size( ); }
	const bg_linestring_t& get_item( ) const { return ls; }
	bg_point& back( ) { return ls.back( ); }
	bg_point& at (size_t i ) { return ls.at( i ); }
	void erase_back() { if(ls.size()) ls.erase(ls.end()-1); }
	virtual SP_BaseItem get_SP() { return boost::make_shared< PolyItem >(*this); }
};
typedef boost::shared_ptr< PolyItem > SP_PolyItem;

// ..........................................................................
struct TextItem : BaseItem
{
	std::wstring val;
	bg_point pos;
	double orient;
	ObjectSpace::justify just;
	font_ref font;
//	size_t font;

	TextItem(const std::string& inval, const bg_point& pos, const font_ref& fr= default_font_ref, const ObjectSpace::justify just= ObjectSpace::jRight)
		:val(pugi::as_wide(inval))
		,pos(pos)
		,orient(0)
		,just(just)
		,font(fr)
	{}
	virtual SP_BaseItem get_SP() { return boost::make_shared< TextItem >(*this); }
};
typedef boost::shared_ptr< TextItem > SP_TextItem;

// ..........................................................................
//wrapper for sp_obj_vect_type
//ItemSet is a special case of a primative
//	can be a filled polygon or a concrete description of... or anything as it can be full of more ItemSet's
struct ItemSet : public BaseItem
{
	sp_obj_vect_type set;
	typedef sp_obj_vect_type::iterator iterator;
	typedef sp_obj_vect_type::const_iterator citerator;
	size_t id;
	//offset TODO not used yet........
	bg_point offset;

	ItemSet( )
		:id( 0 )
		, BaseItem(0, style_fill(default_fill).get_SP())
	{ }
	ItemSet( size_t layer )
		: id(0)
		, BaseItem(0, style_fill(default_fill).get_SP())
	{ }
	void push_back( SP_BaseItem spi ) { set.push_back( spi ); }
	iterator begin() { return set.begin(); }
	iterator end() { return set.end(); }
	citerator cbegin() const { return set.begin(); }
	citerator cend() const { return set.end(); }
	const sp_obj_vect_type& get_set( ) const { return set; }
	void clear( ) { set.clear( ); }
	void insert( sp_obj_vect_type::const_iterator first, sp_obj_vect_type::const_iterator last ) { set.insert( set.begin( ), first, last ); }
	void set_id( size_t in ) { id= in; }
	size_t get_id( ) const { return id; }
	void set_offset( const bg_point& in ) { offset= in; }
	const bg_point& get_offset( ) const { return offset; }
	//boost::shared_ptr< ItemSet > make_SP( ) {	return boost::make_shared< ItemSet >( *this ); }
	virtual SP_BaseItem get_SP() { return boost::make_shared< ItemSet >(*this); }
	size_t size() const { return set.size(); }
	SP_BaseItem back() { return set.back(); }
	SP_BaseItem front() const { return set.front(); }
};
typedef boost::shared_ptr< ItemSet > SP_ItemSet;
//using a list so the objects won't move around on addition/deletion
typedef std::list< ItemSet > object_set_t;

//globals .................................
//both of these rotate/tranform a copied object leaving the origanal in place
SP_BaseItem rotate_object( GEO_NUM angle, const SP_BaseItem& input, bg_point origin= bg_point( ) );
SP_BaseItem transform_object( const SP_BaseItem& input, bg_point offset );

bg_box get_rect_hull( const SP_BaseItem& obs );
bg_box get_rect_hull( const sp_obj_vect_type& obs );
bg_box get_rect_hull( const object_set_t& objects );

bg_box get_bound_rect(const sp_obj_vect_type& obs);
bg_box get_bound_rect(const object_set_t& objects);

std::ostream& operator << ( std::ostream& os, const SP_BaseItem& in );


#endif //OBJECT_H
