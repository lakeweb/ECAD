
#include "stdafx.h"

#include "Geometry.h"
#include "objects.h"
#include "shared.h"
#include "Drawing.h"

#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "CADDoc.h"
#include "BXLDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ..........................................................................
IMPLEMENT_DYNCREATE(CBXLDoc, CADDoc)
BEGIN_MESSAGE_MAP(CBXLDoc, CADDoc)
	//ON_COMMAND(ID_FILE_EXPORT, &CGearsDoc::OnFileExport)
	//ON_COMMAND(ID_FILE_G_CODE, &CGearsDoc::OnFileGCode)
	//	ON_NOTIFY( ID_GEAR_PARAM_CHANGED, 1, &CGearsDoc::OnCmdMsg )
END_MESSAGE_MAP()

// ..........................................................................
CBXLDoc::CBXLDoc()
{
}

// ..........................................................................
void CBXLDoc::test()
{

}

// ..........................................................................
//translate ComponentStore to our CAD
namespace {
	using namespace Component;
	
	SP_BaseItem BXLDrawToCAD(BaseDraw::ptr_BaseDraw& item)
	{
		SP_BaseItem bi;
		typecase(*item,
			[&](Point& pa) {
			bi = PointItem(bg_point(pa.x, -pa.y));
		},
			[&](Line& pa) {
			bi = LineItem(bg_point(pa.a.x, -pa.a.y), bg_point(pa.b.x, -pa.b.y));
		});
		return bi;
	}

	SP_BaseItem BXLPinToCAD(ScPin& item, TextStyle_map_type& fonts)
	{
		bg_point des_text_pos(item.designate.pos.x, -item.designate.pos.y);
		//des_text_pos += bg_point(item.pos.x,item.pos.y);
		bg_point name_text_pos(item.name.pos.x, -item.name.pos.y);
		TextItem pintext(item.designate.name, des_text_pos);
		TextItem nametext(item.name.name, name_text_pos);
		pintext.just = (ObjectSpace::justify)item.designate.just;
		nametext.just = (ObjectSpace::justify)item.name.just;
		auto font = fonts.find(item.designate.text_style);
		pintext.font.height = font->second.height;
		pintext.font.width = font->second.char_width * .8;
		nametext.font.height = font->second.height;
		nametext.font.width = font->second.char_width * .8;
		PinItem pin(pintext, nametext);
		pin.orient = item.rotate;
		if (!int(item.rotate))
			pin.set(bg_line(bg_point(item.pos.x + item.length, -item.pos.y),bg_point(item.pos.x, -item.pos.y)));
		else if(int(item.rotate) == 180)
			pin.set(bg_line(bg_point(item.pos.x - item.length, -item.pos.y),bg_point(item.pos.x, -item.pos.y)));

		//pin.GetStyle().width = 3;
		return pin;
	}
}//namespace

// ..........................................................................
//#define QUICK_BXL
BOOL CBXLDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//could use: Component::ComponentStore GetBXLStoreFromPath(...)

	std::string str;

#if defined(_DEBUG) && defined(QUICK_BXL)
	{
		//decompress takes a bit of time in debug.
		std::ifstream quick("./bxl_output.txt");
		std::stringstream strt;
		strt << quick.rdbuf();
		str = strt.str();
	}
#else
	//open and decompress the bxl file
	std::ifstream in(lpszPathName, std::ios::binary);
	if (!in.is_open())
		assert(false);

	str = ReadBXL(bfs::path(lpszPathName));
	std::ofstream os("./bxl_output.txt");
	boost::replace_all(str, "\r\n", "\n");
	os.write(str.c_str(), str.size());
#endif

	//parse the BXL
	auto comp = ParseBXL(str);//todo, catch errors

	//transfer `Component to CAD`
	ItemSet set;
	for (auto& item : comp.symbol.draws)
		set.push_back(BXLDrawToCAD(item));

	for (auto& item : comp.symbol.pins)
		set.push_back(BXLPinToCAD(item,comp.text_set));

	//for (auto& item : comp.symbol.pins)
	//	BUG_OS(item << std::endl);

	drawobj.push_back(set);

	bg_box bound(get_bound_rect(drawobj.get_objects_set()));
	//std::cout << "CBXLDoc::OnOpenDocument box: " << bound << std::endl;
	draw_extents = bound;
	UpdateAllViews(nullptr, ID_UPDATE_VIEW_TOFIT);

	return TRUE;
}


