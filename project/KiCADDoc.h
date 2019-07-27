#pragma once

#include "../KiCAD_parse/KiCADStore.h"
// ..........................................................................

/*
	The first PCB execrsize, KiCAD.

	NSAssembly is from the parser: KiCADStore.h

	ki_layers: temperary to load ki layer ids from
		a string set of layers, like: "*.Cu *.Mask"

	layer_map: key by kicad layer id to ECAD layer

	the_pad_attrs, so far just a list of colors in KiCAD order
		and may have to keep different sets/lookups in xml, KiCAD is not consistant


*/
// ..........................................................................
class KiCADDoc : public CADDoc
{
protected:
	KiCADDoc();
	DECLARE_DYNCREATE(KiCADDoc)
	//need to handle layers speciallty
	NSAssembly::LayerSet pLayers;
	//layer_set is populated on each pad translation
	using layer_set = std::vector<size_t>;
	layer_set ki_layers;
	struct layer_attr{
		layer_attr(size_t i) : ecad_layer(i) {}
		layer_attr() {}
		size_t ecad_layer;
		SP_draw_style sp_style;
		friend bool operator < (const layer_attr& a, const layer_attr& b) { return a.ecad_layer < b.ecad_layer; }
	};
	std::map<size_t, layer_attr> layer_map;
	//returns the ECAD index to layer from KiCAD layer id.
	size_t layer_map_at(size_t i) { assert(layer_map.find(i) != layer_map.end()); return layer_map[i].ecad_layer; }

	void load_layers(const NSAssembly::string_vect& in);
	void SetColors(DrawingObect& objs);
	SP_BaseItem KiPadToCAD(NSAssembly::KiPad& item, const bg_point& offset);
	SP_BaseItem KiDrawToCAD(NSAssembly::BaseDraw::ptr_BaseDraw& item, size_t layer, const bg_point& offset);

public:
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	DECLARE_MESSAGE_MAP()
};

//NOT USED>...............................
namespace KiCAD {
	// ..........................................................................
	enum EDA_COLOR_T
	{
		UNSPECIFIED_COLOR = -1,
		BLACK = 0,
		DARKDARKGRAY,
		DARKGRAY,
		LIGHTGRAY,
		WHITE,
		LIGHTYELLOW,
		DARKBLUE,
		DARKGREEN,
		DARKCYAN,
		DARKRED,
		DARKMAGENTA,
		DARKBROWN,
		BLUE,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		LIGHTBLUE,
		LIGHTGREEN,
		LIGHTCYAN,
		LIGHTRED,
		LIGHTMAGENTA,
		YELLOW,
		PUREBLUE,
		PUREGREEN,
		PURECYAN,
		PURERED,
		PUREMAGENTA,
		PUREYELLOW,
		NBCOLORS,                    ///< Number of colors
		HIGHLIGHT_FLAG = (1 << 19),
		MASKCOLOR = 31       ///< mask for color index into g_ColorRefs[]
	};
	struct StructColors {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		EDA_COLOR_T type;
		std::string name;
		EDA_COLOR_T alt_type;

	};
#define _HKI(x) x
	const StructColors g_ColorRefs[] =
	{
		{ 0,    0,   0,   BLACK,         _HKI("Black"),     DARKDARKGRAY },
	{ 72,   72,  72,  DARKDARKGRAY,  _HKI("Gray 1"),    DARKGRAY },
	{ 132,  132, 132, DARKGRAY,      _HKI("Gray 2"),    LIGHTGRAY },
	{ 194,  194, 194, LIGHTGRAY,     _HKI("Gray 3"),    WHITE },
	{ 255,  255, 255, WHITE,         _HKI("White"),     WHITE },
	{ 194,  255, 255, LIGHTYELLOW,   _HKI("L.Yellow"),  WHITE },
	{ 72,   0,   0,   DARKBLUE,      _HKI("Blue 1"),    BLUE },
	{ 0,    72,  0,   DARKGREEN,     _HKI("Green 1"),   GREEN },
	{ 72,   72,  0,   DARKCYAN,      _HKI("Cyan 1"),    CYAN },
	{ 0,    0,   72,  DARKRED,       _HKI("Red 1"),     RED },
	{ 72,   0,   72,  DARKMAGENTA,   _HKI("Magenta 1"), MAGENTA },
	{ 0,    72,  72,  DARKBROWN,     _HKI("Brown 1"),   BROWN },
	{ 132,  0,   0,   BLUE,          _HKI("Blue 2"),    LIGHTBLUE },
	{ 0,    132, 0,   GREEN,         _HKI("Green 2"),   LIGHTGREEN },
	{ 132,  132, 0,   CYAN,          _HKI("Cyan 2"),    LIGHTCYAN },
	{ 0,    0,   132, RED,           _HKI("Red 2"),     LIGHTRED },
	{ 132,  0,   132, MAGENTA,       _HKI("Magenta 2"), LIGHTMAGENTA },
	{ 0,    132, 132, BROWN,         _HKI("Brown 2"),   YELLOW },
	{ 194,  0,   0,   LIGHTBLUE,     _HKI("Blue 3"),    PUREBLUE, },
	{ 0,    194, 0,   LIGHTGREEN,    _HKI("Green 3"),   PUREGREEN },
	{ 194,  194, 0,   LIGHTCYAN,     _HKI("Cyan 3"),    PURECYAN },
	{ 0,    0,   194, LIGHTRED,      _HKI("Red 3"),     PURERED },
	{ 194,  0,   194, LIGHTMAGENTA,  _HKI("Magenta 3"), PUREMAGENTA },
	{ 0,    194, 194, YELLOW,        _HKI("Yellow 3"),  PUREYELLOW },
	{ 255,  0,   0,   PUREBLUE,      _HKI("Blue 4"),    WHITE },
	{ 0,    255, 0,   PUREGREEN,     _HKI("Green 4"),   WHITE },
	{ 255,  255, 0,   PURECYAN,      _HKI("Cyan 4"),    WHITE },
	{ 0,    0,   255, PURERED,       _HKI("Red 4"),     WHITE },
	{ 255,  0,   255, PUREMAGENTA,   _HKI("Magenta 4"), WHITE },
	{ 0,    255, 255, PUREYELLOW,    _HKI("Yellow 4"),  WHITE },
	};

	static const EDA_COLOR_T default_items_color[] = {
		LIGHTGRAY, // unused
		CYAN,      // VIA_MICROVIA_VISIBLE
		BROWN,     // VIA_BBLIND_VISIBLE
		LIGHTGRAY, // VIA_THROUGH_VISIBLE
		YELLOW,    // NON_PLATED_VISIBLE
		LIGHTGRAY, // MOD_TEXT_FR_VISIBLE
		BLUE,      // MOD_TEXT_BK_VISIBLE
		DARKGRAY,  // MOD_TEXT_INVISIBLE
		BLUE,      // ANCHOR_VISIBLE
		RED,       // PAD_FR_VISIBLE
		GREEN,     // PAD_BK_VISIBLE
		LIGHTGRAY, // RATSNEST_VISIBLE
		DARKGRAY,  // GRID_VISIBLE
		LIGHTRED,  LIGHTGRAY, LIGHTGRAY, LIGHTGRAY,
		LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY,
		LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY,
		LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY,
		LIGHTGRAY, LIGHTGRAY, LIGHTGRAY
	};
}
