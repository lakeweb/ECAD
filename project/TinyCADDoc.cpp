
#include "stdafx.h"

#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "TinyCAD/SQLite/CppSQLite3U.h"
#include "cad_full.h"
#include "app_share.h"
#include "CADDoc.h"

#include "TinyLIB.h"
#include "view_share.h"
#include "TinyCADDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//C:\\cpp\\Tiny_CAD\\Tiny_CAD\\TinyCAD\\trunk\\libs
void GetRenderTiny(pugi::xml_node& xml, DrawingObects& objects);

// ..........................................................................
IMPLEMENT_DYNCREATE(TinyCADDoc, CADDoc)
BEGIN_MESSAGE_MAP(TinyCADDoc, CADDoc)
END_MESSAGE_MAP()

// ..........................................................................
TinyCADDoc::TinyCADDoc()
{
}

void TinyCADDoc::DisplaySymbol(tinylib_item& item)
{
	XML::CXML test_xml;
	test_xml.load_string(pugi::as_utf8(item.symbol.c_str()).c_str());
	drawobj.clear();
	GetRenderTiny(XMLNODE(test_xml.root()), drawobj);
	bg_box bound(get_bound_rect(drawobj.get_objects_set()));
	inflate(bound, (bound.max_corner().get_x() - bound.min_corner().get_x()) / 10);
	style_fill fill;

	RectItem ri(bound);

	ItemSet set;
	set.push_back(ri);
	drawobj.push_back(set);
	draw_extents = bound;
	UpdateAllViews(nullptr);
}

// ...........................................................................
tinylib_item TinyCADDoc::OpenTinyLib(uint64_t id)
{
	auto it = std::find_if(lib_list.begin(), lib_list.end(),
		[id](const doc_lib_item& dl) {return dl.id == id; });

	std::wstring symble;
	if (it != lib_list.end() && it->parent)
	{
		auto lib = get_tinylib_data(it->parent->pathname);
		auto item = lib.get_by_name(it->pathname);
		if (item != lib.end())
		{
			item->symbol = lib.load_symble(item);
			return *item;
		}
	}
	size_t i = 0;
	return tinylib_item();
}

// ..........................................................................
void tree_thread(tread_info* sema, doc_lib_list* plib_list) {
	auto test = load_tinylib_paths();
	for (bfs::path& path : test)
	{
		tree_view_struct tv;
		tv.icon_pos = 1;
		std::wstring str(path.stem().wstring());
		tv.label = str.c_str();
		//std::cout << path << std::endl;
		//this needs to block. So if MFC, use SendMessage not Post.
		AddTreeViewNode(&tv);
		plib_list->push_back(doc_lib_item(path.wstring(), tv.id));
		auto parent = &plib_list->back();
		auto lib = get_tinylib_data(path);
		uint64_t th = tv.id;
		tv.icon_pos = 2;
		for (auto& item : lib)
		{
			//std::cout << pugi::as_utf8(item.name) << std::endl;
			if (sema->bBreak)
			{
				//std::cout << "s true\n" << std::endl;;
				sema->bDone = true;
				return;
			}
			tv.id = th;
			tv.label = item.name.c_str();
			//blocking...
			AddTreeViewNode(&tv);
			plib_list->push_back(doc_lib_item(item.name.c_str(), tv.id, parent));
		}
	}
	sema->bDone = true;
};

// ..........................................................................
extern char* test_symbol;
extern char* test_symbol2;
extern char* test_symbol_Bezier;
extern char* test_MSP430F2012;
extern char* test_or_gate;
BOOL TinyCADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//populate the tree view with libs....
	thread= std::thread(tree_thread, &sema, &lib_list);

	//testing..........................
	XML::CXML test_xml;
	test_xml.load_string(test_symbol2);

	GetRenderTiny(XMLNODE(test_xml.root()), drawobj);
	bg_box bound(get_bound_rect(drawobj.get_objects_set()));
	inflate(bound, (bound.max_corner().get_x() - bound.min_corner().get_x())/10);
	draw_extents = bound;
	UpdateAllViews( nullptr, ID_UPDATE_VIEW_TOFIT );
	//DumpDrawingObects(std::cout, drawobj);
	return TRUE;
}

// ..........................................................................
void TinyCADDoc::OnCloseDocument()
{
	sema.bBreak = true;
	for (; ! sema.bDone; )
		CLEARMESSAGEPUMP;
	thread.join();
	CDocument::OnCloseDocument();
}

// ..........................................................................
BOOL TinyCADDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	auto cmd = LOWORD(nCode);
	//assert(cmd != ID_LIB_TREE_SELECTED);
	if ( HIWORD(nCode) == WM_NOTIFY )
	{
		DocNotifyObject* pHdr = (DocNotifyObject*)((AFX_NOTIFY*)pExtra)->pNMHDR;

		switch (cmd)
		{
		case ID_LIB_TREE_SELECTED:
		{
			auto sym = OpenTinyLib(pHdr->id);
			std::wofstream os(L"test.xml");
			os << sym.symbol;
			DisplaySymbol(sym);

			break;
		}
		default:
			break;
		}
	}
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

// ...........................................................................
using namespace ObjectSpace;
namespace {
	enum enumTinyRend
	{
		tenNull,
		tenPoly,
		tenPin,
		tenElip,
		tenRect,
		tenFont,
		tenStyle,
		tenFill,
	};

	static const char* labelTinyRend[] = {
		"NULL",
		"POLYGON",
		"PIN",
		"ELLIPSE",
		"RECTANGLE",
		"FONT",
		"STYLE",
		"FILL",
	};

	enum en_tiny_layers
	{
		enDraw,
		enPins,
		enOther,
	};
	using dir_pair = std::pair<size_t, double>;
	static std::unordered_map< size_t, double >dir= {
		dir_pair(0, 0),
		dir_pair(2,90),
		dir_pair(3,270),
		dir_pair(4,180),
	};
/*
	const static std::vector<poly_style> poly_fills={
		{0,BS_NULL},
		{RGB(0,0,0),BS_SOLID},
		{RGB(0,222,255),BS_HATCHED}//testing, not tiny
	};
	poly_style get_tiny_fill(const char* id) {return poly_fills[boost::lexical_cast<int>(id)];}
*/
	AStrTFlag<enumTinyRend, char> TinyRendSet(labelTinyRend, _countof(labelTinyRend));

	auto layer_draw = constexpr cad_layer(enDraw, L"draw");
	auto layer_pins = constexpr cad_layer(enPins, L"pins");
	auto layer_other = constexpr cad_layer(enOther, L"other");
}//namespace

#ifdef _DEBUG
#define BUG(x){x;}
#else
#define BUG __noop
#endif;
// ...........................................................................
#define TINY_FACT 25.4 //we will convert from mm to inches....
SP_BaseItem GetTinyArc(const bg_point& a, const bg_point& b, size_t arc);
void GetRenderTiny(pugi::xml_node& xml, DrawingObects& objects)
{
	// READ: Notes in ReadMe.txt !!!!!!!!!!!!!!!
	layer_set_t& layers = const_cast<layer_set_t&>(objects.get_layers());
	layers.insert(layer_draw);
	layers.insert(layer_pins);
	layers.insert(layer_other);

	ItemSet items(layer_draw.id);
	ItemSet pins(layer_pins.id);

	setof_info infoset; //generic for reading other styles...
	auto get_base = [&infoset](size_t id) {
		return std::find_if(infoset.begin(), infoset.end(),
			[&](const sp_info info) {return info->id == id; }); };

	auto get_fill = [&](size_t id) {
		auto it = get_base(id);
		style_info* p;
		style_info fill; fill = it == infoset.end() ? style_info() : (
			p= dynamic_cast<style_info*>(it->get()), p ? *p : style_info());
		return fill; };

	auto get_width = [&](size_t id) {
		auto it = get_base(id);
		style_info* p;
		size_t w = it == infoset.end() ? 1 : (
			p = dynamic_cast<style_info*>(it->get()), p ? p->width : 1);
		return w; };

	XMLNODE node = xml.child("TinyCAD").first_child();
	for (; node; node = node.next_sibling())
	{
		//will assune the infos are read first........
		auto r_type = TinyRendSet.GetFlags(node.name());
		switch (r_type)
		{
		case tenFont:{
			font_info info;
			info.id = node.GetAttributeInt("id");
			info.height = node.GetElementValueInt("HEIGHT");
			info.weight = node.GetElementValueInt("WEIGHT");
			info.bItalic = !!node.GetElementValue("ITALIC");
			info.bUnderline = !!node.GetElementValue("UNDERLINE");
			info.bStrikeout = !!node.GetElementValue("STRIKEOUT");
			info.face_name = node.GetElementValue("FACENAME");
			infoset.push_back(info);
			continue;
		}
		case tenStyle:{
			style_info info;
			info.id = node.GetAttributeInt("id");
			info.color = node.GetElementValueInt("COLOR");
			info.width = node.GetElementValueInt("THICKNESS");
			info.pattern= node.GetElementValueInt("STYLE");
			infoset.push_back(info);
			continue;
		}
		case tenFill:{
			style_info info;
			info.id = node.GetAttributeInt("id");
			info.color = node.GetElementValueInt("COLOR");
			info.width = 1;
			info.pattern = node.GetElementValueInt("INDEX");//??
			infoset.push_back(info);
			continue;
		}
		case tenPoly:{
			ItemSet poly(layer_draw.id);
			bg_point pos(get_point(node.GetAttribute("pos"))*TINY_FACT);
			auto& fill_info = get_fill(node.GetAttributeInt("fill"));
			style_fill fill;
			fill.fill_color = fill_info.color;
			fill.fill_pattern = fill_info.pattern == -1 ? BS_HOLLOW : BS_SOLID;
			fill.width = get_width(node.GetAttributeInt("style"));
			poly.sp_style = fill;
			auto it = node.begin();
			XMLNODE child(*it);//TOTO XML wrapper handle iterators...
			assert(std::string(child.name()) == "POINT");
			bg_point last_point(pos + get_point(child.GetAttribute("pos"))*TINY_FACT);

			for (; it != node.end(); ++it)
			{
				XMLNODE xitem(*it);
				bg_point point = get_point(xitem.GetAttribute("pos"))*TINY_FACT+pos;
				uint32_t arc = boost::lexical_cast<int>(xitem.GetAttribute("arc"));
				switch (arc)
				{
				case 0: {//Arc_none
					poly.push_back(LineItem(last_point, point));
					break;
				}
				case 1: //Arc_in
				case 2:{ //Arc_out
					poly.push_back(GetTinyArc(last_point, point, arc));
					break;
				}
				default:
					;
				} //switch( ) poly arcs
				last_point = point;
			}//for( ) each POLYGON item

			items.push_back(poly);
			break;
		}
		case tenPin:{
			//<PIN pos='81.00000,8.00000' which='4' elec='0' direction='1'
			//	part='0' number='8' show='3' length='30' number_pos='0' centre_name='1'>+5</PIN>

			bg_point pos(get_point(node.GetAttribute("pos"))*TINY_FACT);
			//still ferriting out what they did with length
			double length = boost::lexical_cast<double>(node.GetAttribute("length")) * TINY_FACT / 5;

			auto rdir = boost::lexical_cast<size_t>(node.GetAttribute("direction"));
			std::string spin(node.GetAttribute("number"));
			std::string sname(node.GetValue());

			PinItem the_pin(TextItem(spin, pos), TextItem(sname, pos));
			the_pin.text.font = 75;
			the_pin.pin.font = 75;
			switch (rdir)
			{
			case 0:	{ //down
				bg_line the_line(pos, bg_point(pos.x, pos.y - length));
				the_pin.set(the_line);
				the_pin.text.just = ObjectSpace::jLower;
				break;
			}
			case 1:	{ //up
				bg_line the_line(pos, bg_point(pos.x, pos.y + length));
				the_pin.set(the_line);
				the_pin.text.just = ObjectSpace::jUpper;
				break;
			}
			case 2:	{ //left
				bg_line the_line(pos, bg_point(pos.x - length, pos.y));
				the_pin.set(the_line);
				the_pin.text.just = ObjectSpace::jRight;
				the_pin.text.pos.x -= length;
				the_pin.pin.just = jUpperRight;
				break;
			}
			case 3:	{ //right
				bg_line the_line(pos, bg_point(pos.x + length, pos.y));
				the_pin.set(the_line);
				the_pin.text.just = ObjectSpace::jLeft;
				the_pin.pin.just = jUpperLeft;
				the_pin.text.pos.x += length;
				break;
			}
			}//switch(rdir)

			pins.push_back(the_pin);
			break;
		}
		case tenElip:{
			bg_point a(get_point(node.GetAttribute("a"))*TINY_FACT);
			bg_point b(get_point(node.GetAttribute("b"))*TINY_FACT);
			bg_box abox(a, b);
			//tiny may not have a normalized box.
			normal_bg_box(abox);

			ArcItem cir(abox);
			//cir.SetColor(RGB(0, 255, 0));
			items.push_back(cir);
			break;
		}
		case tenRect:{
			bg_point a(get_point(node.GetAttribute("a"))*TINY_FACT);
			bg_point b(get_point(node.GetAttribute("b"))*TINY_FACT);
			RectItem rect(a, b);
			auto& fill_info = get_fill(node.GetAttributeInt("fill"));
			style_fill sfill;
			sfill.fill_color = fill_info.color;
			sfill.fill_pattern = fill_info.pattern == -1 ? BS_HOLLOW : BS_SOLID;
			sfill.width = get_width(node.GetAttributeInt("style"));
			rect.sp_style = sfill;
			items.push_back(rect);
			break;
		}
		default:
			;
		}//switch( tiny rend enum )
	}//for all XML

	objects.push_back(items);
	objects.push_back(pins);

	//get the bounding box
	bg_box box = get_bound_rect(objects.get_objects_set());
	//and set the viewport, no zooming here...
	objects.set_viewport(box);
}

// ...........................................................................
//convert the tinyCAD arc to a BezierCubeItem; This may be the same for other ECAD systems
SP_BaseItem GetTinyArc(const bg_point& a, const bg_point& b, size_t arc)
{
	bg_box abox(a, b);
	bool bUp = false;
	if (arc == 2)
		bUp = true;
	else
		assert(arc == 1);

	auto halfx = ((abox.max_corner().get_x() - abox.min_corner().get_x()) / 2);
	auto halfy = ((abox.max_corner().get_y() - abox.min_corner().get_y()) / 2);

	//found an exception in an or gate, was really handed a line
	//but it is confounding!!! this line won't print!
	//if (!halfx || !halfy)
	//	return LineItem(abox.max_corner(), abox.min_corner());

	bg_point pa(abox.min_corner().get_x() + (! bUp ? halfx : 0),
		abox.min_corner().get_y() + (! bUp ? 0 : halfy));

	bg_point pb(abox.max_corner().get_x() - (bUp ? halfx : 0),
		abox.max_corner().get_y() - (bUp ? 0 : halfy));

	BezierCubeItem bz(abox.min_corner(), pa, pb,abox.max_corner());
	//std::cout << bz << std::endl;
	return bz;
}

// .............................................................
char* test_symbol = R"(<?xml version="1.0" encoding="UTF-8"?>
<TinyCAD>
	<FONT id='1'>
		<HEIGHT>-10</HEIGHT>
		<WIDTH>0</WIDTH>
		<WEIGHT>400</WEIGHT>
		<ITALIC>0</ITALIC>
		<UNDERLINE>0</UNDERLINE>
		<STRIKEOUT>0</STRIKEOUT>
		<CHARSET>0</CHARSET>
		<FACENAME>Arial</FACENAME>
	</FONT>
	<FONT id='3'>
		<HEIGHT>-24</HEIGHT>
		<WIDTH>11</WIDTH>
		<WEIGHT>400</WEIGHT>
		<ITALIC>0</ITALIC>
		<UNDERLINE>0</UNDERLINE>
		<STRIKEOUT>0</STRIKEOUT>
		<CHARSET>0</CHARSET>
		<FACENAME>Arial</FACENAME>
	</FONT>
	<STYLE id='3'>
		<STYLE>0</STYLE>
		<COLOR>000000</COLOR>
		<THICKNESS>5</THICKNESS>
	</STYLE>
	<FILL id='0'>
		<INDEX>-1</INDEX>
		<COLOR>000000</COLOR>
	</FILL>
	<RECTANGLE a='71.00000,14.00000' b='117.00000,96.00000' style='3' fill='0'/>
	<PIN pos='81.00000,8.00000' which='4' elec='0' direction='1' part='0' number='8' show='3' length='30' number_pos='0' centre_name='1'>+5</PIN>
	<PIN pos='104.00000,8.00000' which='4' elec='0' direction='1' part='0' number='35' show='3' length='30' number_pos='0' centre_name='1'>+5</PIN>
	<PIN pos='65.00000,21.00000' which='0' elec='0' direction='3' part='0' number='38' show='3' length='30' number_pos='0' centre_name='0'>CLK1</PIN>
	<PIN pos='65.00000,25.00000' which='0' elec='0' direction='3' part='0' number='39' show='3' length='30' number_pos='0' centre_name='0'>CLK2</PIN>
	<PIN pos='65.00000,34.00000' which='0' elec='0' direction='3' part='0' number='7' show='3' length='30' number_pos='0' centre_name='0'>BA</PIN>
	<PIN pos='65.00000,40.00000' which='0' elec='0' direction='3' part='0' number='34' show='3' length='30' number_pos='0' centre_name='0'>R/*W</PIN>
	<PIN pos='65.00000,44.00000' which='0' elec='0' direction='3' part='0' number='37' show='3' length='30' number_pos='0' centre_name='0'>E</PIN>
	<PIN pos='65.00000,48.00000' which='0' elec='0' direction='3' part='0' number='5' show='3' length='30' number_pos='0' centre_name='0'>VMA</PIN>
	<PIN pos='65.00000,52.00000' which='0' elec='0' direction='3' part='0' number='36' show='3' length='30' number_pos='0' centre_name='0'>RE</PIN>
	<PIN pos='65.00000,56.00000' which='0' elec='0' direction='3' part='0' number='3' show='3' length='30' number_pos='0' centre_name='0'>MR</PIN>
	<PIN pos='65.00000,60.00000' which='0' elec='0' direction='3' part='0' number='40' show='3' length='30' number_pos='0' centre_name='0'>*RESET</PIN>
	<PIN pos='65.00000,64.00000' which='0' elec='0' direction='3' part='0' number='4' show='3' length='30' number_pos='0' centre_name='0'>*IRQ</PIN>
	<PIN pos='65.00000,68.00000' which='0' elec='0' direction='3' part='0' number='6' show='3' length='30' number_pos='0' centre_name='0'>*NMI</PIN>
	<PIN pos='65.00000,72.00000' which='0' elec='0' direction='3' part='0' number='2' show='3' length='30' number_pos='0' centre_name='0'>*HALT</PIN>
	<PIN pos='82.00000,102.00000' which='4' elec='0' direction='0' part='0' number='1' show='3' length='30' number_pos='0' centre_name='1'>GND</PIN>
	<PIN pos='103.00000,102.00000' which='4' elec='0' direction='0' part='0' number='21' show='3' length='30' number_pos='0' centre_name='1'>GND</PIN>
	<PIN pos='123.00000,19.00000' which='0' elec='0' direction='2' part='0' number='9' show='3' length='30' number_pos='0' centre_name='0'>A0</PIN>
	<PIN pos='123.00000,22.00000' which='0' elec='0' direction='2' part='0' number='10' show='3' length='30' number_pos='0' centre_name='0'>A1</PIN>
	<PIN pos='123.00000,25.00000' which='0' elec='0' direction='2' part='0' number='11' show='3' length='30' number_pos='0' centre_name='0'>A2</PIN>
	<PIN pos='123.00000,28.00000' which='0' elec='0' direction='2' part='0' number='12' show='3' length='30' number_pos='0' centre_name='0'>A3</PIN>
	<PIN pos='123.00000,31.00000' which='0' elec='0' direction='2' part='0' number='13' show='3' length='30' number_pos='0' centre_name='0'>A4</PIN>
	<PIN pos='123.00000,34.00000' which='0' elec='0' direction='2' part='0' number='14' show='3' length='30' number_pos='0' centre_name='0'>A5</PIN>
	<PIN pos='123.00000,37.00000' which='0' elec='0' direction='2' part='0' number='15' show='3' length='30' number_pos='0' centre_name='0'>A6</PIN>
	<PIN pos='123.00000,40.00000' which='0' elec='0' direction='2' part='0' number='16' show='3' length='30' number_pos='0' centre_name='0'>A7</PIN>
	<PIN pos='123.00000,43.00000' which='0' elec='0' direction='2' part='0' number='17' show='3' length='30' number_pos='0' centre_name='0'>A8</PIN>
	<PIN pos='123.00000,46.00000' which='0' elec='0' direction='2' part='0' number='18' show='3' length='30' number_pos='0' centre_name='0'>A9</PIN>
	<PIN pos='123.00000,49.00000' which='0' elec='0' direction='2' part='0' number='19' show='3' length='30' number_pos='0' centre_name='0'>A10</PIN>
	<PIN pos='123.00000,52.00000' which='0' elec='0' direction='2' part='0' number='20' show='3' length='30' number_pos='0' centre_name='0'>A11</PIN>
	<PIN pos='123.00000,55.00000' which='0' elec='0' direction='2' part='0' number='22' show='3' length='30' number_pos='0' centre_name='0'>A12</PIN>
	<PIN pos='123.00000,58.00000' which='0' elec='0' direction='2' part='0' number='23' show='3' length='30' number_pos='0' centre_name='0'>A13</PIN>
	<PIN pos='123.00000,61.00000' which='0' elec='0' direction='2' part='0' number='24' show='3' length='30' number_pos='0' centre_name='0'>A14</PIN>
	<PIN pos='123.00000,64.00000' which='0' elec='0' direction='2' part='0' number='25' show='3' length='30' number_pos='0' centre_name='0'>A15</PIN>
	<PIN pos='122.00000,69.00000' which='0' elec='0' direction='2' part='0' number='33' show='3' length='30' number_pos='0' centre_name='0'>D0</PIN>
	<PIN pos='122.00000,72.00000' which='0' elec='0' direction='2' part='0' number='32' show='3' length='30' number_pos='0' centre_name='0'>D1</PIN>
	<PIN pos='122.00000,75.00000' which='0' elec='0' direction='2' part='0' number='31' show='3' length='30' number_pos='0' centre_name='0'>D2</PIN>
	<PIN pos='122.00000,78.00000' which='0' elec='0' direction='2' part='0' number='30' show='3' length='30' number_pos='0' centre_name='0'>D3</PIN>
	<PIN pos='122.00000,81.00000' which='0' elec='0' direction='2' part='0' number='29' show='3' length='30' number_pos='0' centre_name='0'>D4</PIN>
	<PIN pos='122.00000,84.00000' which='0' elec='0' direction='2' part='0' number='28' show='3' length='30' number_pos='0' centre_name='0'>D5</PIN>
	<PIN pos='122.00000,87.00000' which='0' elec='0' direction='2' part='0' number='27' show='3' length='30' number_pos='0' centre_name='0'>D6</PIN>
	<PIN pos='122.00000,90.00000' which='0' elec='0' direction='2' part='0' number='26' show='3' length='30' number_pos='0' centre_name='0'>D7</PIN>
	<LABEL pos='88.00000,40.00000' direction='3' font='3' color='208000' style='0'>6802</LABEL>
</TinyCAD>
)";

char* test_symbol2 = R"(<?xml version="1.0" encoding="UTF-8"?>
<TinyCAD>
	<FONT id='1'>
		<HEIGHT>-10</HEIGHT>
		<WIDTH>0</WIDTH>
		<WEIGHT>400</WEIGHT>
		<ITALIC>0</ITALIC>
		<UNDERLINE>0</UNDERLINE>
		<STRIKEOUT>0</STRIKEOUT>
		<CHARSET>0</CHARSET>
		<FACENAME>Arial</FACENAME>
	</FONT>
	<STYLE id='4'>
		<STYLE>0</STYLE>
		<COLOR>000000</COLOR>
		<THICKNESS>1</THICKNESS>
	</STYLE>
	<STYLE id='5'>
		<STYLE>0</STYLE>
		<COLOR>000000</COLOR>
		<THICKNESS>2</THICKNESS>
	</STYLE>
	<FILL id='0'>
		<INDEX>-1</INDEX>
		<COLOR>000000</COLOR>
	</FILL>
	<FILL id='1'>
		<INDEX>0</INDEX>
		<COLOR>000000</COLOR>
	</FILL>
	<RECTANGLE a='13.00000,6.00000' b='27.00000,30.00000' style='4' fill='0'/>
	<POLYGON pos='13.00000,11.00000' style='4' fill='0'>
		<POINT pos='4.00000,4.00000' arc='0'/>
		<POINT pos='4.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='13.00000,18.00000' style='4' fill='0'>
		<POINT pos='4.00000,0.00000' arc='0'/>
		<POINT pos='4.00000,5.00000' arc='0'/>
		<POINT pos='0.00000,5.00000' arc='0'/>
	</POLYGON>
	<PIN pos='10.00000,11.00000' which='0' elec='4' direction='3' part='0' number='1' show='3' length='15' number_pos='0' centre_name='0'></PIN>
	<PIN pos='10.00000,23.00000' which='0' elec='4' direction='3' part='0' number='2' show='3' length='15' number_pos='0' centre_name='0'></PIN>
	<PIN pos='10.00000,28.00000' which='0' elec='6' direction='3' part='0' number='3' show='3' length='15' number_pos='0' centre_name='0'></PIN>
	<PIN pos='30.00000,21.00000' which='0' elec='0' direction='2' part='0' number='4' show='3' length='15' number_pos='0' centre_name='0'></PIN>
	<PIN pos='30.00000,12.00000' which='0' elec='0' direction='2' part='0' number='5' show='3' length='15' number_pos='0' centre_name='0'></PIN>
	<PIN pos='30.00000,8.00000' which='0' elec='0' direction='2' part='0' number='6' show='3' length='15' number_pos='0' centre_name='0'></PIN>
	<POLYGON pos='15.40000,18.00000' style='5' fill='0'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='3.20000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='15.00000,15.00000' style='4' fill='1'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='2.00000,3.00000' arc='0'/>
		<POINT pos='4.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='23.00000,15.00000' style='4' fill='0'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,4.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='23.00000,14.00000' style='4' fill='0'>
		<POINT pos='0.00000,2.00000' arc='0'/>
		<POINT pos='2.00000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='23.00000,18.00000' style='4' fill='0'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='2.00000,2.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='25.00000,13.00000' style='4' fill='0'>
		<POINT pos='0.00000,1.00000' arc='0'/>
		<POINT pos='0.00000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='25.00000,20.00000' style='4' fill='0'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,1.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='24.00000,19.00000' style='4' fill='1'>
		<POINT pos='1.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,1.00000' arc='0'/>
		<POINT pos='1.00000,1.00000' arc='0'/>
		<POINT pos='1.00000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='21.00000,8.00000' style='4' fill='0'>
		<POINT pos='2.00000,9.00000' arc='0'/>
		<POINT pos='0.00000,9.00000' arc='0'/>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='6.00000,0.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='25.00000,12.00000' style='4' fill='0'>
		<POINT pos='2.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='0.00000,1.00000' arc='0'/>
	</POLYGON>
	<POLYGON pos='25.00000,21.00000' style='4' fill='0'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='2.00000,0.00000' arc='2'/>
	</POLYGON>
</TinyCAD>
)";

char* test_symbol_Bezier = R"(<?xml version="1.0" encoding="UTF-8"?>
	<TinyCAD>
		<FONT id = '1'>
			<HEIGHT>-10</HEIGHT>
			<WIDTH>0</WIDTH>
			<WEIGHT>400</WEIGHT>
			<ITALIC>0</ITALIC>
			<UNDERLINE>0</UNDERLINE>
			<STRIKEOUT>0</STRIKEOUT>
			<CHARSET>0</CHARSET>
			<FACENAME>Arial</FACENAME>
		</FONT>
		<STYLE id = '2'>
			<STYLE>0</STYLE>
			<COLOR>000000</COLOR>
			<THICKNESS>1</THICKNESS>
		</STYLE>
		<FILL id = '0'>
			<INDEX>-1</INDEX>
			<COLOR>000000</COLOR>
		</FILL>
		<POLYGON pos = '110.00000,28.00000' style = '2' fill = '0'>
			<POINT pos = '0.00000,10.00000' arc = '0'/>
			<POINT pos = '11.00000,5.00000' arc = '1'/>
			<POINT pos = '0.00000,0.00000' arc = '2'/>
			<POINT pos = '2.75000,5.00000' arc = '1'/>
			<POINT pos = '0.00000,10.00000' arc = '2'/>
		</POLYGON>
		<PIN pos = '109.00000,30.00000' which = '0' elec = '0' direction = '3' part = '0' number = '1' show = '0' length = '15' number_pos = '0' centre_name = '0'>A</PIN>
		<PIN pos = '109.00000,36.00000' which = '0' elec = '0' direction = '3' part = '0' number = '2' show = '0' length = '15' number_pos = '0' centre_name = '0'>A</PIN>
		<PIN pos = '124.00000,33.00000' which = '0' elec = '1' direction = '2' part = '0' number = '3' show = '0' length = '15' number_pos = '0' centre_name = '0'>A</PIN>
	</TinyCAD>
)";
char* test_MSP430F2012 = R"(<?xml version="1.0" encoding="UTF-8"?>
<TinyCAD>
	<FONT id='1'>
		<HEIGHT>-10</HEIGHT>
		<WIDTH>0</WIDTH>
		<WEIGHT>400</WEIGHT>
		<ITALIC>0</ITALIC>
		<UNDERLINE>0</UNDERLINE>
		<STRIKEOUT>0</STRIKEOUT>
		<CHARSET>0</CHARSET>
		<FACENAME>Arial</FACENAME>
	</FONT>
	<STYLE id='3'>
		<STYLE>0</STYLE>
		<COLOR>000000</COLOR>
		<THICKNESS>5</THICKNESS>
	</STYLE>
	<STYLE id='4'>
		<STYLE>0</STYLE>
		<COLOR>000000</COLOR>
		<THICKNESS>1</THICKNESS>
	</STYLE>
	<FILL id='0'>
		<INDEX>-1</INDEX>
		<COLOR>000000</COLOR>
	</FILL>
	<RECTANGLE a='37.00000,27.00000' b='80.00000,59.00000' style='3' fill='0'/>
	<PIN pos='31.00000,31.00000' which='0' elec='0' direction='3' part='0' number='1' show='3' length='30' number_pos='0' centre_name='0'>VCC</PIN>
	<PIN pos='31.00000,35.00000' which='0' elec='0' direction='3' part='0' number='2' show='3' length='30' number_pos='0' centre_name='0'>P1.0/TACLK/ACLK/AO</PIN>
	<PIN pos='31.00000,39.00000' which='0' elec='0' direction='3' part='0' number='3' show='3' length='30' number_pos='0' centre_name='0'>P1.1/TA0/A1</PIN>
	<PIN pos='31.00000,43.00000' which='0' elec='0' direction='3' part='0' number='4' show='3' length='30' number_pos='0' centre_name='0'>P1.2/TA1/A1</PIN>
	<PIN pos='31.00000,47.00000' which='4' elec='0' direction='3' part='0' number='5' show='3' length='30' number_pos='0' centre_name='0'>P1.3/ADCCK/A3/VRe-</PIN>
	<PIN pos='31.00000,51.00000' which='0' elec='0' direction='3' part='0' number='6' show='3' length='30' number_pos='0' centre_name='0'>P1.4/SMCLK/A4/VRe+/TCK</PIN>
	<PIN pos='31.00000,55.00000' which='0' elec='0' direction='3' part='0' number='7' show='3' length='30' number_pos='0' centre_name='0'>P1.5/TA0/A5/SCLK/TMS</PIN>
	<PIN pos='86.00000,55.00000' which='0' elec='0' direction='2' part='0' number='8' show='3' length='30' number_pos='0' centre_name='0'>P1.6/TA1/A6/SOL/..</PIN>
	<PIN pos='86.00000,51.00000' which='0' elec='0' direction='2' part='0' number='9' show='3' length='30' number_pos='0' centre_name='0'>P1.7/A7/SID/..</PIN>
	<PIN pos='86.00000,47.00000' which='4' elec='0' direction='2' part='0' number='10' show='3' length='30' number_pos='0' centre_name='0'>TEST/SBWTCK  </PIN>
	<PIN pos='86.00000,43.00000' which='0' elec='0' direction='2' part='0' number='11' show='3' length='30' number_pos='0' centre_name='0'>_RST/NMI/SBW..</PIN>
	<PIN pos='86.00000,39.00000' which='0' elec='0' direction='2' part='0' number='12' show='3' length='30' number_pos='0' centre_name='0'>XOUT/P2.7 </PIN>
	<PIN pos='86.00000,35.00000' which='0' elec='0' direction='2' part='0' number='13' show='3' length='30' number_pos='0' centre_name='0'>XIN/P2.6/TA1 </PIN>
	<PIN pos='86.00000,31.00000' which='0' elec='0' direction='2' part='0' number='14' show='3' length='30' number_pos='0' centre_name='0'>VSS </PIN>
	<POLYGON pos='56.00000,27.00000' style='4' fill='0'>
		<POINT pos='0.00000,3.00000' arc='0'/>
		<POINT pos='3.00000,0.00000' arc='1'/>
	</POLYGON>
	<POLYGON pos='53.00000,27.00000' style='4' fill='0'>
		<POINT pos='0.00000,0.00000' arc='0'/>
		<POINT pos='3.00000,3.00000' arc='2'/>
	</POLYGON>
</TinyCAD>
)";

char* test_or_gate = R"(<?xml version="1.0" encoding="UTF-8"?>
<TinyCAD>
	<FONT id='1'>
		<HEIGHT>-10</HEIGHT>
		<WIDTH>0</WIDTH>
		<WEIGHT>400</WEIGHT>
		<ITALIC>0</ITALIC>
		<UNDERLINE>0</UNDERLINE>
		<STRIKEOUT>0</STRIKEOUT>
		<CHARSET>0</CHARSET>
		<FACENAME>Arial</FACENAME>
	</FONT>
	<STYLE id='2'>
		<STYLE>0</STYLE>
		<COLOR>000000</COLOR>
		<THICKNESS>1</THICKNESS>
	</STYLE>
	<FILL id='0'>
		<INDEX>-1</INDEX>
		<COLOR>000000</COLOR>
	</FILL>
	<POLYGON pos='110.00000,28.00000' style='2' fill='0'>
		<POINT pos='0.00000,10.00000' arc='0'/>
		<POINT pos='11.00000,5.00000' arc='1'/>
		<POINT pos='0.00000,0.00000' arc='2'/>
		<POINT pos='2.75000,5.00000' arc='1'/>
		<POINT pos='0.00000,10.00000' arc='2'/>
<!--	--></POLYGON>
	<PIN pos='109.00000,30.00000' which='0' elec='0' direction='3' part='0' number='1' show='0' length='15' number_pos='0' centre_name='0'>A</PIN>
	<PIN pos='109.00000,36.00000' which='0' elec='0' direction='3' part='0' number='2' show='0' length='15' number_pos='0' centre_name='0'>A</PIN>
	<PIN pos='124.00000,33.00000' which='0' elec='1' direction='2' part='0' number='3' show='0' length='15' number_pos='0' centre_name='0'>A</PIN>
</TinyCAD>
)";