
#pragma once

// ..............................................................
struct cad_layer
{
	size_t id;
	uint32_t tree_id;
	std::wstring label;
	bool enabled;
	uint32_t color;

	cad_layer(size_t id, const wchar_t* str, bool enabled = true)
		:id(id)
		, label(str ? str : L"")
		, enabled(enabled)
	{ }
	cad_layer(size_t id, const std::string str, bool enabled = true)
		:id(id)
		, label(str.size() ? pugi::as_wide(str.c_str()) : L"")
		, enabled(enabled)
	{ }
	void SetColor(uint32_t in) const { *const_cast<uint32_t*>(&color) = in; }
	bool operator < ( const cad_layer& a ) const { return id < a.id; }
	operator size_t ( ) const { return id; }

};

typedef std::set< cad_layer > layer_set_t;

// ..............................................................
// TODO these are no longer active, depreciate???? or upgrade like the tiny stuff?
#define APP_INFO_NOTIFY ( WM_USER + 100 )
#define VIEW_INFO_NOTIFY ( WM_USER + 101 )

//for messaging between views for update notifications...........
struct info_notify
{
	enum call
	{
		output_info,
		other_info,
		find_info,
		layer,
	};
	bool bClear;
	call the_call;
	std::string str;
#ifdef _DEBUG
	size_t sig = 1234;
#endif

	info_notify( call which )
		:the_call( which )
		,bClear( false )
	{ }

};

// ..............................................................
typedef struct _dock_notify
{
	enum docker {
		layer,
	};

	docker the_docker;
	size_t info;
	uint32_t tree_id;

	_dock_notify( docker d, size_t i= 0, uint32_t id = 0 )
		:the_docker( d )
		,info( i )
		,tree_id(id)
	{ }
} dock_notify_t;

// ..............................................................
// ..............................................................
//app messaging
//#define MSG_APP_BEGIN                   32801
//#define MSG_APP_END                     32850

#define MSG_LAYER_ENABLE				( MSG_APP_BEGIN + 1 )
#define CMD_TEST						( MSG_APP_BEGIN + 2 )


//for sending notifications to CDocument Objects
//https://www.codeproject.com/articles/14706/notifying-the-document
typedef struct tagNMHDROBJECT
{
	NMHDR nmHdr;
	void* pObject; 
} NMHDROBJECT, *PNMHDROBJECT;

inline double TO_RAD(double x) { return x * PI / 180; }

//some structures have attributes, like TinyCAD, FONT,STYLE,FILL
//This is a try at generics.....

struct base_info : boost::enable_shared_from_this< base_info > {
	using sp_info = boost::shared_ptr< base_info >;
	size_t id;//?
	virtual ~base_info() {}
	virtual sp_info get_sp() { return boost::make_shared<base_info>(*this); }
	operator sp_info() { return get_sp(); }
};
using sp_info = boost::shared_ptr< base_info >;

struct font_info : base_info {
	size_t height;
	size_t weight;
	bool bItalic;
	bool bStrikeout;
	bool bUnderline;
	std::string face_name;
	virtual sp_info get_sp() { return boost::make_shared<font_info>(*this); }
};

struct style_info : base_info { //could also be used for fill?????
	uint32_t color;
	size_t width;
	uint32_t pattern;//brush or pen
	style_info():color(-1), width(1), pattern(BS_NULL){}
	virtual sp_info get_sp() { return boost::make_shared<style_info>(*this); }
};
/*
//* Brush Styles
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3
#define BS_INDEXED          4
#define BS_DIBPATTERN       5
#define BS_DIBPATTERNPT     6
#define BS_PATTERN8X8       7
#define BS_DIBPATTERN8X8    8
#define BS_MONOPATTERN      9

//* Hatch Styles
#define HS_HORIZONTAL       0       //* -----
#define HS_VERTICAL         1       //* |||||
#define HS_FDIAGONAL        2       //* \\\\\
#define HS_BDIAGONAL        3       //* /////
#define HS_CROSS            4       //* +++++
#define HS_DIAGCROSS        5       //* xxxxx
#define HS_API_MAX          12

/* Pen Styles
#define PS_SOLID            0
#define PS_DASH             1       //* ------- 
#define PS_DOT              2       //* ....... 
#define PS_DASHDOT          3       //* _._._._ 
#define PS_DASHDOTDOT       4       //* _.._.._ 
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8
#define PS_STYLE_MASK       0x0000000F

#define PS_ENDCAP_ROUND     0x00000000
#define PS_ENDCAP_SQUARE    0x00000100
#define PS_ENDCAP_FLAT      0x00000200
#define PS_ENDCAP_MASK      0x00000F00

#define PS_JOIN_ROUND       0x00000000
#define PS_JOIN_BEVEL       0x00001000
#define PS_JOIN_MITER       0x00002000
#define PS_JOIN_MASK        0x0000F000

#define PS_COSMETIC         0x00000000
#define PS_GEOMETRIC        0x00010000
#define PS_TYPE_MASK        0x000F0000

#define AD_COUNTERCLOCKWISE 1
#define AD_CLOCKWISE        2

*/
using  setof_info = std::vector<sp_info>;