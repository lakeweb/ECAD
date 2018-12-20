#pragma once



//component.h
#if !defined(KICAD_COMPONENT_DEC_14_2018)
#define KICAD_COMPONENT_DEC_14_2018

#pragma once

//components are source/target agnostic

namespace NSAssembly
{
	using string_vect = std::vector<std::string>;
	//I was going to go this old school way but 'variant' looks better?
	//https://stackoverflow.com/questions/31224643/variant-vs-inheritance
	// ..................................................
	//just simple drawing objects here
	struct BaseDraw : public boost::enable_shared_from_this< BaseDraw >
	{
		using ptr_BaseDraw = boost::shared_ptr<BaseDraw>;
		virtual ~BaseDraw() {}
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<BaseDraw>(*this); }
		std::string layer;
		operator ptr_BaseDraw() { return get_SP(); }
	};
	struct Point : public BaseDraw
	{
		double x;
		double y;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Point>(*this); }
	};
	struct PointEx : public BaseDraw //not used by BXL so won't be pushed
	{
		double x;
		double y;
		double rot;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<PointEx>(*this); }
	};
	struct Line : public BaseDraw
	{
		Point a;
		Point b;
		double width;
		size_t net;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Line>(*this); }
	};
	using LineSet = std::vector<Line>;
	struct Rect : public BaseDraw //not used, delete? just use Line?
	{
		Point a;//min corner
		Point b;//max corner
		int32_t width;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Rect>(*this); }
	};
	struct Arcc : public BaseDraw
	{
		Point origin;
		double radius;
		double start_angle;
		double sweep_angle;
		int32_t width;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Arcc>(*this); }
	};

	using PolylineVect = std::vector< Point >;
	struct Polyline : public BaseDraw, boost::enable_shared_from_this< Polyline >
	{
		PolylineVect poly_line;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Polyline>(*this); }
	};
	using DrawContainer = std::vector< BaseDraw::ptr_BaseDraw >;

	// General ..........................................
	enum ROTATE
	{
		rot0,
		rot90,
		rot180,
		rot270,
	};
	// ..................................................
	enum JUSTIFY
	{
		eCenter,
		ejLeft,
		ejRight,
		ejUpper,
		ejLower,
		ejUpperLeft,
		ejUpperRight,
		ejLowerLeft,
		ejLowerRight,
	};
	static struct pad_shapes {
		const char* lable;
		size_t id;
	} pad_shapes_set[] = {
		{"round",1}
	};
	// ..................................................
	struct TextStyle
	{
		//	//TextStyle "H50s3" (FontWidth 3) (FontHeight 50) (FontCharWidth 25)
		size_t width;
		size_t height;
		size_t char_width;

	};
	using TextStyle_pair_type = std::pair< std::string, TextStyle >;
	using TextStyle_map_type = std::unordered_map< std::string, TextStyle >;

	// Symbol .........................................
	struct Attribute
	{
		Point origin;
		std::string name;
		std::string attr;
		bool bVisable;
		JUSTIFY just;
		std::string text_style;
	};
	typedef std::vector< Attribute > Attributes;

	// ..................................................
	struct Designate //Same for PinName!!
	{
		/*
		PinDes "7" (100, -600) (TextStyleRef "H82s5") (Justify LowerRight) (IsVisible True)
		PinName "P1.5/TA0.0/UCB0CLK/UCA0STE/A5/CA5/TMS" (310, -600) (TextStyleRef "H82s5") (Justify Left) (IsVisible True)
		*/
		std::string name;
		Point pos;
		std::string text_style;
		JUSTIFY just;
		bool bVisable;
	};

	// ..................................................
	class ScPin
	{
		// Pin(PinNum 7) (Origin 300, -600) (PinLength 300) (Rotate 180) (Width 8) (IsVisible True)
	public:
		size_t number;
		Point pos;
		double length;
		//		ROTATE rotate; as BXL could be any angle using double instead of enum
		double rotate;
		double width;
		bool bIsFlipped;
		bool bVisable;
		Designate designate;
		Designate name;
	};
	using ScPinVect = std::vector< ScPin >;

	// ..................................................
	class ScSymbol
	{
	public:
		std::string name;
		Point origin;
		ScPinVect pins;
		DrawContainer draws;
		Attributes attrs;
	};

	struct Layer //bxl does not use a designator, 'id', but the likes of KiCAD does
	{
		uint32_t id;
		std::string name;
		std::string type;
	};
	using LayerSet = std::vector<Layer>;

	//yet another text type!!??
	struct CompText {
		bool bVisable;
		PointEx pos;
		std::string layer;
		std::string ref;
		std::string text;
		std::string text_style;
	};
	using CompTextSet = std::vector<CompText>;

	// ..................................................
	struct PadShape
	{
		std::string name;
		std::string layer;
		double height;
		double width;
		uint32_t type;
	};
	using PadShapeSet = std::vector<PadShape>;

	// ..................................................
	struct PadStack
	{
		std::string name;
		double diam;//hole diameter, if used
		bool bSurface; //??
		bool bPlated;
		PadShapeSet shapes;
	};

	// ..................................................
	struct PcPad //
	{
		size_t number;
		std::string name;
		std::string style;
		PointEx pos;
		std::string layer;
		CompTextSet textset;
		LineSet lineset;
	};

	//Pad Bridge KiCAD to std...........................
	using net_pair = std::pair<int, std::string>;
	struct KiPad //
	{
		size_t number;
		string_vect attrs;
		PointEx pos;
		Point size;
		double drill;
		string_vect layers;
		net_pair net;
	};
	using KiPadSet = std::vector<KiPad>;

	struct KiArc : public BaseDraw
	{				//   (fp_circle (center 0 0) (end 2.54 1.905) (layer Dessus.SilkS) (width 0.381))
		Point center;
		Point radius;
		// .!/ std::string layer;
		double width;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<KiArc>(*this); }
	};
	using KiArcSet = std::vector<KiArc>;

	// ..................................................
	struct PcSubComp
	{
		CompTextSet textset;
		DrawContainer lineset;
		KiPadSet padset;
	};

	// ..................................................
	using attr_pair = std::pair<std::string, std::string>;
	using comp_attributes_type = std::vector<attr_pair>;
	struct PcComponent //aka module, pattern....
	{
		PointEx pos;
		std::string name;
		std::string layer;
		comp_attributes_type comp_attrs; //tag,content
		PcSubComp items;
	};
	using PcComponentSet = std::vector<PcComponent>;

	// ..................................................
	//  (segment (start 115.57 86.36) (end 121.92 86.36) (width 0.25) (layer F.Cu) (net 1) (tstamp 5C151114) (status 800000))
	struct Segment
	{
		Line line;
		//double width;
		//std::string layer;
		//size_t net;
	};
	using SegmentSet = std::vector<Segment>;

	struct KiAssemLine : Line
	{
		double angle;
	};

	using BarePoint = std::pair<double, double>;
	using BarePointSet = std::vector<BarePoint>;
	struct Zone
	{
		size_t net;
		std::string layer;
		BarePointSet desig_poly;
		BarePointSet fill_poly;
	};
	using ZoneSet = std::vector<Zone>;

	struct KiBoardItems
	{
		DrawContainer lineset;
		SegmentSet segments;
		ZoneSet zones;
	};

	// ..................................................
	struct Assembly
	{
		Rect area;
		uint32_t file_ver;
		TextStyle_map_type text_set;
		//symbol(s) (s)..todo
		ScSymbol symbol;
		//components
		LayerSet layers;
		////Layer layer;
		PcComponentSet components;
		KiBoardItems assem_items; //segments, gr_line, gr_arc, zones...
	};
}//namespace NSAssembly

//debug......
// ...................................................................
inline std::ostream& operator << (std::ostream& os, const NSAssembly::Assembly& st)
{
	os << st.text_set.size() << std::endl
		<< st.symbol.name << std::endl
		<< st.symbol.pins.size();
	return os;
}

// ..................................................................
inline std::ostream& operator<<(std::ostream& os, const NSAssembly::Point& pt) { os << pt.x << "," << pt.y; return os; }

// ..................................................................
inline std::ostream& operator << (std::ostream& os, const NSAssembly::BaseDraw::ptr_BaseDraw& in)
{
	using namespace NSAssembly;
	typecase(*in,

	//[&](const Line& pa) {
	//	os << "Point: " << pa.get_bg_point(); },

	[&](const Line& pa) {
		os << "Line from: " << pa.a << "\t to " << pa.b; }

	//[&](const ArcItem& pa) {
	//	os << "Arc a: " << pa.get_a() << " b: " << pa.get_b() << " o: " << pa.get_o(); },

	//[&](const RectItem& pa) {
	//	os << "Rect min " << const_cast<RectItem&>(pa).get_min_point() << " max " << const_cast<RectItem&>(pa).get_max_point(); },

	//[&](const EllipesItem& pa) {},
	//[&](const BezierCubeItem& pa) {
	//	os << "Bezier p1: " << pa.get_point0() << " h1: " << pa.get_point1() << " p2: " << pa.get_point2() << " h2: " << pa.get_point3(); },

	//[&](const PolyItem& pa) {
	//for (auto & item : pa)
	//	os << "Poly "<< item.get_x() << "," << item.get_y() << "; "; },

	//[&](const TextItem& pa) {
	//	os << "Text: " << pugi::as_utf8(pa.val); },

	//[&](const ItemSet& pa) {
	//	os << "Set: " << pa.size() << "\n";
	//	for (auto& item : pa)
	//		os << "  " << item;}
	);//typecase

	return os;
}


namespace std {// namespace spirit { namespace x3 {
	inline std::ostream& operator<<(std::ostream& os, const NSAssembly::LineSet& pt) { return os; }
	inline std::ostream& operator<<(std::ostream& os, const NSAssembly::PcPad& pt)
	{
		return os << pt.name << " " << pt.layer << " " << pt.lineset;
	}
}//}}


 //the lib exposed interface.
 //TODO make stream capable.....
NSAssembly::Assembly ParseKiCAD(const std::string& buf);


#endif //KICAD_COMPONENT_DEC_14_2018
