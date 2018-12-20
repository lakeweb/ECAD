
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
//#include "typeswitch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ..........................................................................
IMPLEMENT_DYNCREATE(KiCADDoc, CADDoc)
BEGIN_MESSAGE_MAP(KiCADDoc, CADDoc)
END_MESSAGE_MAP()

// ..........................................................................
KiCADDoc::KiCADDoc()
{
}

// ...........................................................................
using namespace NSAssembly;
// ...........................................................................
void KiCADDoc::load_layers(const string_vect& in)
{
	ki_layers.clear();
	for (const auto& i : in)
	{
		//this will work for now as 'B' and 'F' designate sides of pads
		for (auto& l : layers)
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
	constexpr size_t pad_attrs_size = sizeof(the_pad_attrs);
	constexpr ki_pad_attr* the_pad_attrs_end = the_pad_attrs + pad_attrs_size;
	auto get_pad_attr(const std::string& name){
		std::cout << sizeof(the_pad_attrs) << std::endl;
		auto it = std::find_if(the_pad_attrs, the_pad_attrs_end, [&name](const ki_pad_attr& attr) {return name == attr.name; });
		return it == the_pad_attrs_end ? 0 : it->pos;
	}
}

SP_BaseItem KiPadToCAD(KiPad& item, size_t layer, const bg_point& offset)
{
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
			spi = EllipesItem(box.min_corner(), box.max_corner());
		else //new one, at least show a point
			spi = PointItem(box_mid(box));
		//spi->Get
//		spi->SetWidth(3);
	}
	set.push_back(spi);
	if (item.attrs.size() && get_pad_attr(item.attrs.front()) == 2)
	{
		inflate(box, -item.drill);
		set.push_back(EllipesItem(box.min_corner(), box.max_corner()));
	}

	spi->SetLayer(layer);
	return spi;
}

SP_BaseItem KiDrawToCAD(BaseDraw::ptr_BaseDraw& item, size_t layer, const bg_point& offset)
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
	bi->SetLayer(layer);
	return bi;
}

// ..........................................................................
//at that! every pcb file seems to have different layer names
//for that reason, will just populate by order instead of name...............
struct KiCAD_colors { //will use our own colors, but for now.................
	size_t id;
	char* tag;
	uint32_t color;
} kicolors[] = {
	{0, "F.Cu", RGB(0,255,0)},
	{31, "B.Cu", RGB(255,0,0) },
	{32, "B.Adhes", RGB(120,200,0) },
	{33, "F.Adhes", RGB(120,200,0) },
	{34, "B.Paste", RGB(120,100,255) },//4 silk
	{35, "F.Paste", RGB(120,100,255) },
	{36, "B.SilkS", RGB(120,200,0) },
	{37, "F.SilkS", RGB(120,200,0) },
	{38, "B.Mask", RGB(120,200,0) },
	{39, "F.Mask", RGB(120,200,0) },
	{40, "Dwgs.User", RGB(120,200,0) },
	{41, "Cmts.User", RGB(120,200,0) },
	{42, "Eco1.User", RGB(120,200,0) },
	{43, "Eco2.User", RGB(120,200,0) },
	{44, "Edge.Cuts", RGB(120,200,0) },
	{45, "Margin", RGB(120,200,0) },
	{46, "B.CrtYd", RGB(120,200,0) },
	{47, "F.CrtYd", RGB(120,200,0) },
	{48, "B.Fab", RGB(120,200,0) },
	{49, "F.Fab", RGB(120,200,0) },
	//{120,"", RGB(0,0,255)}
};
static size_t kicolors_size = (sizeof(KiCAD_colors) - 1) * sizeof(kicolors);

// ..........................................................................
BOOL KiCADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
//parse...............................................
	std::ifstream fbuf(lpszPathName);
	std::stringstream  bufstr;
	bufstr << fbuf.rdbuf();
	std::string input(bufstr.str());
	//TODO use the stream directly...
	Assembly board = ParseKiCAD(input);

	//as this is destination work on translation of object will be done here....
//layers..............................................
	auto to_layer = [&board](const std::string& name)
	{ return std::find_if(board.layers.begin(), board.layers.end(),
		[&name](const Layer& layer) { return layer.name == name; });};

	//layer_set_t layers;
	for (auto& layer : board.layers)
		layers.insert(cad_layer(layer.id, layer.name));

	//set the colors like KiCAD
	for (auto it = layers.begin(); it != layers.end(); ++it)
		const_cast<cad_layer&>(*it).color = kicolors[std::distance(layers.begin(), it)].color;

//filled polygons.......................................
	//testing, just stuff the one ecc83-pp.kicad_pcb fill_poly
	if (board.assem_items.zones.size())//for other docs without fill
	{
		ItemSet setpoly;
		for (const auto& point : board.assem_items.zones.front().fill_poly)
			setpoly.push_back(PointItem(bg_point(point.first, point.second)));

		//ItemSet setpolyline;
		//for (auto& const point : board.assem_items.zones.front().desig_poly)
		//	setpolyline.push_back(PointItem(bg_point(point.first, point.second)));

		//drawobj.push_back(setpoly);
		style_fill ps(RGB(100, 200, 50), BS_SOLID, 0, 0);
		ps.pen_style = PS_NULL;
		setpoly.sp_style = ps;
		//filled polygons are not seen from the top: drawobj, so, until so...
		ItemSet polys;
		polys.push_back(setpoly);
		drawobj.push_back(polys);
	}
//components.............................................
	for (auto& comp : board.components)
	{
		bg_point comp_pos(comp.pos.x, comp.pos.y);
		ItemSet ecomp;
		//ecomp.set_offset(bg_point(comp.pos.x, comp.pos.y));

		for (auto& pad : comp.items.padset)
		{
			load_layers(pad.layers);
			for (auto& layer : ki_layers)
			{
				ecomp.push_back(KiPadToCAD(pad, layer, comp_pos));
			}
		}
		for (auto& draw : comp.items.lineset)
			ecomp.push_back(KiDrawToCAD(draw, to_layer(draw->layer)->id, comp_pos));

		if (comp.pos.rot)
			for (auto& item : ecomp)
				item = rotate_object(TO_RAD(360 - comp.pos.rot), item, comp_pos);
		drawobj.push_back(ecomp);
	}

//wire segments.........................................
	ItemSet set0;
	set0.set_id(123);
	ItemSet set1;//for now, just read as if the segs are layer 0 and 1......
	set1.SetLayer(31);
	set1.set_id(321);
	for (auto& seg : board.assem_items.segments)
	{
		if(to_layer(seg.line.layer) == board.layers.begin())
			set0.push_back(SegItem(bg_point(seg.line.a.x, seg.line.a.y)
				, bg_point(seg.line.b.x, seg.line.b.y)
				, to_layer(seg.line.layer)->id
				, seg.line.width, seg.line.net));
		else
			set1.push_back(SegItem(bg_point(seg.line.a.x, seg.line.a.y)
				, bg_point(seg.line.b.x, seg.line.b.y)
				, to_layer(seg.line.layer)->id
				, seg.line.width, seg.line.net));
	}
	style_line linest;
	linest.line_color = RGB(123, 222, 111);
	linest.width = 7;
	linest.pen_style = PS_ENDCAP_ROUND | PS_SOLID;
	set0.sp_style = linest;
	set1.sp_style = linest;
	drawobj.push_back(set0);
	drawobj.push_back(set1);

	//	DumpDrawingObects(std::cout, drawobj);

	//populate the layer view.
	info_notify notice(info_notify::layer);
	SetInfoWnd(notice, &layers);

	bg_box bound(get_bound_rect(drawobj.get_objects_set()));
	inflate(bound, 20);
	draw_extents = bound;
	UpdateAllViews(nullptr, ID_UPDATE_VIEW_TOFIT);

	return TRUE;
}

// ..........................................................................
void KiCADDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
}

// ..........................................................................
BOOL KiCADDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	auto cmd = LOWORD(nCode);
	//assert(cmd != ID_LIB_TREE_SELECTED);
	if (HIWORD(nCode) == WM_NOTIFY)
	{
		DocNotifyObject* pHdr = (DocNotifyObject*)((AFX_NOTIFY*)pExtra)->pNMHDR;
		switch (cmd)
		{
		case ID_LIB_TREE_SELECTED:
		{
			pHdr->id;
			auto it = std::find_if(layers.begin(), layers.end(),
				[&pHdr](const cad_layer& layer){return layer.tree_id == pHdr->id; });
			if (it != layers.end())
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
