
#include "stdafx.h"
#include <iostream>
#include <string>

#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "cad_full.h"
#include "app_share.h"
#include "CADDoc.h"

#include "Geometry.h"
#include "view_share.h"
#include "KiCADDoc.h"

#include "..\KiCAD_parse\KiCADStore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace ObjectSpace;
// ..........................................................................
IMPLEMENT_DYNCREATE(KiCADDoc, CADDoc)
BEGIN_MESSAGE_MAP(KiCADDoc, CADDoc)
END_MESSAGE_MAP()

// ..........................................................................
KiCADDoc::KiCADDoc()
{
}

//testing colors
// ...........................................................................
using namespace NSAssembly;
// ...........................................................................
//for loading the pad stack layers...............
void KiCADDoc::load_layers(const string_vect& in)
{
	ki_layers.clear();
	for (const auto& i : in)
	{
		//this will work for now as 'B' and 'F' designate sides of pads
		for (auto& l : GetLayers())
		{
			if (i.front() == '*') //double sided
			{
				if (l.label.substr(2) == pugi::as_wide(i.substr(2)))
					ki_layers.push_back(l.id);
			}
			else if (l.label == pugi::as_wide(i))//single sided
				ki_layers.push_back(l.id);
		}
	}
}

// ...........................................................................
//till there is more information...................
namespace {
	static struct ki_pad_attr
	{
		char* name;
		size_t pos;

	} the_pad_attrs[] = {
		{ "", 0 },
		{ "connect", 1 },
		{ "thru_hole", 2 },
		{ "rect", 3 },
		{ "circle", 4 },
	};
	constexpr size_t pad_attrs_size = sizeof(the_pad_attrs)/sizeof(ki_pad_attr);
	constexpr ki_pad_attr* the_pad_attrs_end = the_pad_attrs + pad_attrs_size;
	auto get_pad_attr(const std::string& name){
		//std::cout << sizeof(the_pad_attrs) << std::endl;
		auto it = std::find_if(the_pad_attrs, the_pad_attrs_end
			, [&name](const ki_pad_attr& attr) {return name == attr.name; });
		return it == the_pad_attrs_end ? 0 : it->pos;
	}
}

SP_BaseItem KiCADDoc::KiPadToCAD(KiPad& item, const bg_point& offset)
{
	auto test_style = draw_style(RGB(100,100,200), psSolid, 2, CLR_REDISH, bsHatched);
	load_layers(item.layers); //pad to ki_layers
	ItemSet set;//this is a pad stack.....
	//KiCAD pos specs the center of the pad
	bg_point center(-item.size.x / 2, -item.size.y / 2);
	SP_BaseItem spi;
	bg_point from(item.pos.x, item.pos.y);
	bg_box box(from + offset, from + bg_point(item.size.x, item.size.y) + offset);
	box += center;
	if (item.attrs.size() > 1)//sanity
	{
		auto type = get_pad_attr(*(item.attrs.begin() + 1));
		if (type == 3)
			spi = RectItem(box);
		else if (type == 4)
			spi = EllipesItem(box);
		else //new one, at least show a point
			spi = PointItem(box_mid(box));
		spi->sp_style = draw_style(CLR_REDISH,psSolid,2, CLR_REDISH,bsHatched);
		set.push_back(spi);
		if (get_pad_attr(*item.attrs.begin()) == 2) //thru hole
			for (auto& i : set);
	}
	for (size_t l : ki_layers)
	{
		SP_BaseItem sp = *spi;
		sp->layer = layer_map[l].ecad_layer;
		sp->sp_style = layer_map[l].sp_style;
		set.push_back(sp);
	}
	if (item.attrs.size() && get_pad_attr(item.attrs.front()) == 2)
	{
		inflate(box, -item.drill);
		EllipesItem drill(box.min_corner(), box.max_corner());
		drill.sp_style = test_style;
		drill.layer = 0;
		set.push_back(drill);
	}
	set.SetLayer(-1);
	return set;
}

SP_BaseItem KiCADDoc::KiDrawToCAD(BaseDraw::ptr_BaseDraw& item, size_t layer, const bg_point& offset)
{
	//bg_point offset(kioff.x, kioff.y);
	SP_BaseItem bi;
	typecase(*item,
		[&](Point& pa) {
		bi = PointItem(bg_point(pa.x, -pa.y) + offset);
	},
		[&](KiArc& pa) {
		bi = ArcItem(bg_point(pa.center.x, pa.center.y) + offset,
			bg_point(pa.radius.x, pa.radius.y)+ offset,
			bg_point(pa.radius.x, pa.radius.y) + offset);
	},
		[&](Line& pa) {
		bi = LineItem(bg_point(pa.a.x, -pa.a.y) + offset,
			bg_point(pa.b.x, -pa.b.y)+ offset);
	});
	assert(bi.get());
	bi->SetLayer(layer_map[layer].ecad_layer);
	return bi;
}

//at that! every pcb file seems to have different layer names
//for that reason, will just populate by order instead of name...............
struct KiCAD_colors { //will use our own colors, but for now.................
	uint32_t color;
	operator COLORREF () { return color; }
} kicolors[] = {
	RGB(255,0,0)
	,RGB(0,255,0)
	,RGB(0,0,255)
	,RGB(155,50,150)
	,RGB(155,0,150)
	,RGB(155,150,50)
	,RGB(15,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
	,RGB(155,50,150)
};
static size_t kicolors_size = (sizeof(KiCAD_colors) - 1) * sizeof(kicolors);

// ..........................................................................
BOOL KiCADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	std::ofstream test("./dump.txt");

	//will need to interface to the project, if avaliable
	//CXML xml;
	//xml.Open()
//parse...............................................
	std::ifstream fbuf(lpszPathName);
	std::stringstream  bufstr;
	bufstr << fbuf.rdbuf();
	std::string input(bufstr.str());
	//TODO use the stream directly...
	Assembly board = ParseKiCAD(input);

	//keep the set of styles for re-use, container: style_set
	auto style_comp = [](SP_draw_style pa, SP_draw_style pb)
		{ return 0 > memcmp(pa.get(), pb.get(), sizeof(draw_style)); };
	using style_set_type = std::set<SP_draw_style, decltype(style_comp)>;
	style_set_type style_set(style_comp);

//as this is destination work on translation of object will be done here....
//layers..............................................
	//returns iterator to NSAssembly::Layer, lookup by name
	auto to_layer = [&board](const std::string& name)
	{ return std::find_if(board.layers.begin(), board.layers.end(),
		[&name](const Layer& layer) { return layer.name == name; });};

	//populate our layers from KiCAD and map
	for (auto& layer : board.layers)
	{
		GetLayers().push_back(cad_layer(layer.id, layer.name));
		layer_map[layer.id] = GetLayers().size() - 1;
		draw_style ds( kicolors[GetLayers().size() - 1], psSolid, 1);
		layer_map[layer.id].sp_style = ds;
	}
	//set the colors like KiCAD, TODO
	for (auto cit = GetLayers().begin(); cit != GetLayers().end(); ++cit)
		cit->color = kicolors[cit - GetLayers().begin()].color;

	for (auto& test : GetLayers())
		std::cout << "layer: " << test.id << std::endl;
#if 0
//filled polygons.......................................
	//testing, just stuff the one ecc83-pp.kicad_pcb fill_poly
	//TODO ItemSet discriptors set from 
	if (board.assem_items.zones.size())//for other docs without fill
	{
		ItemSet setpoly;
		for (const auto& apoly : board.assem_items.zones.front().fill_polys)
			for( const auto& point : apoly )
				setpoly.push_back(PointItem(bg_point(point.first, point.second)));

		setpoly.bIsPolygon = true;
		ItemSet polys;
		polys.push_back(setpoly);
		drawobj.push_back(polys);
	}
#endif

//wire segments.........................................
	ItemSet segset0(0);
	ItemSet segset1(1);
	
	//segment style stash and lookup.....
	std::vector<SP_draw_style> segs;
	auto fseg = [&segs](COLORREF col, double width) {
		return std::find_if(segs.begin(), segs.end(), [col, width](SP_draw_style spd) { return col == spd->line_color && width == spd->fill_color; });
	};
	for (auto& seg : board.assem_items.segments)
	{
		auto lay = layer_map[to_layer(seg.line.layer)->id];
		auto col = lay.sp_style->line_color;
		auto it = fseg(col, seg.line.width);
		SP_draw_style spdraw;
		if (it == segs.end())
			spdraw = draw_style(col, (psSolid | psEndRound), seg.line.width);
		else
			spdraw = *it;

		SegItem sitem(bg_point(seg.line.a.x, seg.line.a.y)
			, bg_point(seg.line.b.x, seg.line.b.y)
			, lay.ecad_layer
			, seg.line.width
			, seg.line.net);
		sitem.sp_style = spdraw;
		switch (lay.ecad_layer)
		{
		case 0:
			segset0.push_back(sitem);
			break;
		case 1:
			segset1.push_back(sitem);
			break;
		}
	}
	drawobj.push_back(segset0);
	drawobj.push_back(segset1);

#if 1
//components.............................................
	for (auto& comp : board.components)
	{
		bg_point comp_pos(comp.pos.x, comp.pos.y);
		ItemSet ecomp;
		//ecomp.set_offset(bg_point(comp.pos.x, comp.pos.y));

		for (auto& pad : comp.items.padset)
			ecomp.push_back(KiPadToCAD(pad, comp_pos));

		for (auto& draw : comp.items.lineset)
			ecomp.push_back(KiDrawToCAD(draw, to_layer(draw->layer)->id, comp_pos));

		if (comp.pos.rot)
			for (auto& item : ecomp)
				item = rotate_object(TO_RAD(360 - comp.pos.rot), item, comp_pos);

		//for(ecomp.)
		drawobj.push_back(ecomp);
	}
#endif

//	DumpDrawingObects(test, drawobj);

	//populate the layer view.
	info_notify notice(info_notify::layer);
	SetInfoWnd(notice, &GetLayers());

	//kicad colors are purely by layer.....
	//SetColors(drawobj);

	bg_box bound(get_bound_rect(drawobj.get_objects_set()));
#ifdef _DEBUGx
	auto show = RectItem(bound);
	ItemSet extra;
	draw_style fill( RGB(0, 255, 0), psSolid, 5, RGB(0, 0, 255), bsSolid);
	extra.sp_style = fill;
	//show.sp_style = fill;
	extra.push_back(show);
	drawobj.push_back(extra);
#endif
	//inflate(bound, 20);
	draw_extents = bound;

	drawobj.SetBackgroundColor( RGB(120, 120, 120));//not black for now to see stray stuff.....

	std::cout << style_set.size() << " the style_set size\n";
	CADDoc::OnOpenDocument(lpszPathName);
	UpdateAllViews(nullptr, ID_UPDATE_VIEW_TOFIT);

	return TRUE;
}

// ..........................................................................
void KiCADDoc::SetColors(DrawingObect& objs)
{
	std::unordered_map<size_t, draw_style> clrs;
	for (const auto& lay : objs.GetLayers())
		clrs[lay.id]= draw_style(lay.color, psSolid, 1, lay.color, bsSolid);

	for (auto& obj : objs)
	{
		obj.sp_style = clrs[obj.layer];
		std::cout << "LAYER: " << obj.layer << std::endl;
		for (auto& item : obj)
			if (item.get())
			{
				std::cout << "   LAYER: " << item->layer << std::endl;
				item->sp_style = clrs[item->layer];
			}
	}
}

// ..........................................................................
void KiCADDoc::OnCloseDocument()
{
	CADDoc::OnCloseDocument();
}

// ..........................................................................
BOOL KiCADDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	auto cmd = LOWORD(nCode);
	if (HIWORD(nCode) == WM_NOTIFY)
	{
		DocNotifyObject* pHdr = (DocNotifyObject*)((AFX_NOTIFY*)pExtra)->pNMHDR;
		switch (cmd)
		{
		case ID_LAYERTREE_CHANGED:
		{
			pHdr->id;
			auto it = std::find_if(GetLayers().begin(), GetLayers().end(),
				[&pHdr](const cad_layer& layer){return layer.tree_id == pHdr->id; });
			if (it != GetLayers().end())
			{
				const_cast<cad_layer&>(*it).enabled = pHdr->bVal;
				UpdateAllViews(nullptr, ID_INVALIDATE);
			}
			break;
		}
		default:
			break;
		}
	}
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

// ...........................................................................
