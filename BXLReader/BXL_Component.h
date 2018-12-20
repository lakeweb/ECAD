
//component.h
#if !defined(BXL_COMPONENT_MAY_04_2018)
#define BXL_COMPONENT_MAY_04_2018

#pragma once

//components are source/target agnostic

namespace Component
{
	//I was going to go this old school way but 'variant' looks better?
	//https://stackoverflow.com/questions/31224643/variant-vs-inheritance
	// ..................................................
	//just simple drawing objects here
	struct BaseDraw : public boost::enable_shared_from_this< BaseDraw >
	{
		using ptr_BaseDraw = boost::shared_ptr<BaseDraw>;
		virtual ~BaseDraw(){}
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<BaseDraw>(*this); }
		operator ptr_BaseDraw() { return get_SP(); }
	};
	
	struct Point : public BaseDraw, boost::enable_shared_from_this< Point >
	{
		double x;
		double y;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Point>(*this); }
	};
	
	struct Line : public BaseDraw, boost::enable_shared_from_this< Line >
	{
		Point a;
		Point b;
		int32_t width;
		virtual ptr_BaseDraw get_SP() const { return boost::make_shared<Line>(*this); }
	};
	//using SP_Line = boost::shared_ptr<Line>;

	struct Arcc : public BaseDraw, boost::enable_shared_from_this< Arcc >
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


	//using boost rather than std
	//using DrawVars= boost::variant<Point,Line,Arc,Polyline>
	// ..................................................
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

	// ..................................................
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

	// ..................................................
	class ComponentStore
	{
	public:
		TextStyle_map_type text_set;
		ScSymbol symbol;
	};

}//namespace Component

 //debug......
inline std::ostream& operator<<(std::ostream& os, const Component::Point& pt) { os << pt.x << " " << pt.y; return os; }

// ...................................................................
std::string ReadBXL(const bfs::path& path);
Component::ComponentStore ParseBXL(const std::string& buf);

// ...................................................................
inline std::ostream& operator << (std::ostream& os, const Component::ComponentStore& st)
{
	os << st.text_set.size() << std::endl
		<< st.symbol.name << std::endl
		<< st.symbol.pins.size();
	return os;
}

// ...................................................................
inline Component::ComponentStore GetBXLStoreFromPath(const bfs::path& path)
{
	//path= "C:\\cpp\\TinyBXL\\project\\test_files\\MSP430G2253_PW_20.bxl";
	Component::ComponentStore store = ParseBXL(ReadBXL(path));
	std::cout << store << std::endl;
	return store;
}

#endif //BXL_COMPONENT_MAY_04_2018
