

#include "stdafx.h"
#include "shared.h"
#include "view_share.h"
#include "Geometry.h"
#include "objects.h"
#include "Drawing.h"

#include "MemDC.h"
#include "Scroller.h"
#include "CADDoc.h"
#include "CADView.h"
#include "TestCADDoc.h"
#include "TestCADView.h"

// ..................................................................
IMPLEMENT_DYNCREATE(TestCADView, CADView)
BEGIN_MESSAGE_MAP(TestCADView, CADView)
END_MESSAGE_MAP()

// ..................................................................
TestCADView::TestCADView()
{
	winScale = 1;
	rectTarget.SetRect(-100,-100,500,600);
}
TestCADView::~TestCADView(){}

void TestCADView::OnInitialUpdate()
{
	GetParentFrame()->ShowWindow(SW_MAXIMIZE);
	SetTargetSize(CRect(-20, -20, 600, 600));
}

POINT oparator(bg_point& p) { return CPoint((int)p.get_x(), (int)p.get_y()); }
inline POINT bg_point_to_POINT(const bg_point& pt) { return CPoint((int)pt.get_x(), (int)pt.get_y()); }
// ..................................................................
void TestCADView::OnDraw(CDC* poDC)
{
	CADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	sp_layer_set_type layers;
	cad_layer layer(0,L"layer");

	layers.push_back(layer);

	offsetx = -scroller.GetScrollPos().cx - d_ext.rect_target.left;
	offsety = -scroller.GetScrollPos().cy - d_ext.rect_target.top;
	CNewMemDC pDC(poDC);
	SetBkMode(pDC, TRANSPARENT);
	CRect crect;
	GetClientRect(crect);

	//
#if 0
	ItemSet items;
	items.push_back(LineItem(bg_point(100, 400), bg_point(400, 100)));
	items.push_back(LineItem(bg_point(400,100), bg_point(400, 400)));
	items.push_back(LineItem(bg_point(400, 400), bg_point(100, 400)));


	//as this is a polygon...........
	ItemSet poly;
	poly.push_back(items);
	PointItem pi(bg_point(200, 200));
	poly.push_back(pi);
#endif
	ItemSet poly;
	BezierCubeItem b1(bg_point(100, 100), bg_point(100, 150), bg_point(150, 200), bg_point(200, 200));
	BezierCubeItem b2(bg_point(400, 400), bg_point(350, 300), bg_point(400, 350), bg_point(300, 300));
	poly.push_back( LineItem(bg_point(0, 0), bg_point(450, 450)));
	poly.push_back(b1);
	ItemSet sub;
	sub.push_back(b2);
	poly.push_back(sub);

	//the drawer....................
	DrawingObect objects;
	objects.push_back(poly);

	DrawArea drawer(pDC, d_ext);//, layers);
	//CBrush brush;
	//brush.CreateSolidBrush(RGB(20, 122, 255));
	//pDC->SelectObject(brush);
	//pDC->BeginPath();
	drawer.DrawObjects(objects);
	//auto j = dynamic_cast<LineItem*>(items.front().get());
	//pDC->MoveTo(bg_point_to_POINT(j->get_bg_line().first));
	//for (auto & item : items)
	//{
	//	auto i = dynamic_cast<LineItem*>(item.get());
	//	pDC->LineTo(bg_point_to_POINT(i->get_bg_line().second));
	//}
	//pDC->EndPath();
	//pDC->StrokeAndFillPath();

	return;

	/****************************************************************************************/
	/*
	//prototype, assemble PolyDraw object
	//temp try it this way

	struct PolyDraw_types
	{
		enum ID {
			eidBase,
			eidLine,
			eidPoly,
			eidBzier,
		};
		int id;
		std::string name;
		size_t poly_size;
		DWORD PTTYPE;
	};
	std::vector<PolyDraw_types> the_types =
	{
		{ PolyDraw_types::eidBase,"class BaseItem",0,PT_MOVETO},//cause...
		{ PolyDraw_types::eidLine,"class LineItem",1,PT_LINETO},
		{ PolyDraw_types::eidPoly, "PolyItem",1,PT_LINETO },
		{ PolyDraw_types::eidBzier, "BezierCubeItem",3,PT_BEZIERTO },
	};

	//count the points, supported: Line,...
	// PT_MOVETO PT_LINETO PT_BEZIERTO
	size_t cnt = 1;//start with move_to;
	for (auto& item : items)
	{
		auto found = std::find_if(the_types.begin(), the_types.end(),
			[&](PolyDraw_types& t) { return t.name == typeid(*item).name(); } );
		if (found == the_types.end())
			assert(false);
		switch (found->id)
		{
		case PolyDraw_types::eidLine:
			++cnt;
			break;
		case PolyDraw_types::eidPoly:
			cnt += 3;
			break;
		default:
			assert(false);//only Lines and Bziers here
		}
	}
	CBrush brush;
	brush.CreateSolidBrush(RGB(20, 122, 255));
	//pDC->BeginPath();
		poly_arrays arrays(cnt);
	auto ppoints = arrays.get_points();
	auto pbyte = arrays.bytes.get();
	items.front();
	*ppoints++ = bg_point_to_POINT(dynamic_cast<LineItem*>(items.front().get())->get_bg_line().first);
	*pbyte++ = PT_MOVETO;
	for (auto& item : items)
	{
		typecase(*item,
			[&](const LineItem& pa) {*pbyte++ = PT_LINETO; *ppoints++ = bg_point_to_POINT(pa.get_bg_line().second); },
			[&](const BezierCubeItem& pa) {PolyBezierTo },
			[&](const PolyItem& pa) {}
			);
	}
	pDC->BeginPath();
	pDC->SelectObject(brush);
	pDC->PolyDraw(arrays.get_points(), arrays.get_bytes(),arrays.size());
	pDC->EndPath();
	pDC->StrokeAndFillPath();
	*/
}

