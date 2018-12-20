

//...................................................................
// this is the libreCAD way of writing a spline to DXF
#include "stdafx.h"

/**
* Writes the given spline entity to the file.
*/
//Cubic Bézier is the sum of two Quadratic Bézier curves

void /*RS_FilterDXF::*/writeSpline( DL_WriterA& dw, RS_Spline* s, const DL_Attributes& attrib )
{

	// split spline into atomic entities for DXF R12:
	if (dxf.getVersion()==VER_R12) {
		writeAtomicEntities(dw, s, attrib, RS2::ResolveNone);
		return;
	}

	if (s->getNumberOfControlPoints() < s->getDegree()+1) {
		RS_DEBUG->print(RS_Debug::D_ERROR, "RS_FilterDXF::writeSpline: "
			"Discarding spline: not enough control points given.");
		return;
	}

	// Number of control points:
	int numCtrl = s->getNumberOfControlPoints();

	// Number of knots (= number of control points + spline degree + 1)
	int numKnots = numCtrl + s->getDegree() + 1;

	int flags;
	if (s->isClosed()) {
		flags = 11;
	} else {
		flags = 8;
	}

	// write spline header:
	dxf.writeSpline(
		dw,
		DL_SplineData(s->getDegree(),
			numKnots,
			numCtrl,
			flags),
		attrib);

	// write spline knots:
	QList<RS_Vector> cp = s->getControlPoints();
	QList<RS_Vector>::iterator it;

	int k = s->getDegree()+1;
	DL_KnotData kd;
	for (int i=1; i<=numKnots; i++) {
		if (i<=k) {
			kd = DL_KnotData(0.0);
		} else if (i<=numKnots-k) {
			kd = DL_KnotData(1.0/(numKnots-2*k+1) * (i-k));
		} else {
			kd = DL_KnotData(1.0);
		}
		dxf.writeKnot(dw,
			kd);
	}

	// write spline control points:
	for (it = cp.begin(); it!=cp.end(); ++it) {
		dxf.writeControlPoint(dw,
			DL_ControlPointData((*it).x,
				(*it).y,
				0.0));
	}
}


// ..................................................................
struct poly_arrays {
	size_t the_size;
	boost::shared_array <POINT> points;
	boost::shared_array<BYTE> bytes;
	poly_arrays(size_t size) : points(new POINT[size]), bytes(new BYTE[size]), the_size(size) {}
	poly_arrays() {}
	POINT* get_points() { return points.get(); }
	BYTE* get_bytes() { return bytes.get(); }
	size_t size() const { return the_size; }
};
using poly_arrays_vect_type = std::vector<poly_arrays>;

void GetPolyArray(const ItemSet& items)
{
	typecase(items, [&](const BaseItem& pa) {});

	//[&](const PointItem& pa) {
	//DrawPoint(pa.get_bg_point());
	//},
	//[&](const LineItem& pa) {
	//	DrawLine(pa.get_bg_line());
	//	//TRACE( "DrawObject Line: %f\n", pa.get_bg_line( ).first.get_x( ) );
	//},
	//[&](const ArcItem& pa) {
	//	DrawArc(pa);
	//},
	//[&](const RectItem& pa) {
	//	DrawRect(pa.get_bg_box());
	//},
	//[&](const EllipesItem& pa) {
	//	DrawEllipes(pa.get_bg_box());
	//},
	//[&](const BezierCubeItem& pa) {
	//	DrawBezier(pa.get_point0(), pa.get_point1(), pa.get_point2(), pa.get_point3());
	//});
}


/*
The following group codes apply to spline entities. In addition to the group codes described here, see "Common Group Codes for Entities." For information about abbreviations and formatting used in this table, see "Formatting Conventions in This Reference."
Spline group codes
Group codes 	Description

100


Subclass marker (AcDbSpline)

210


Normal vector (omitted if the spline is nonplanar)
DXF: X value; APP: 3D vector

220, 230


DXF: Y and Z values of normal vector (optional)

70


Spline flag (bit coded):
1 = Closed spline
2 = Periodic spline
4 = Rational spline
8 = Planar
16 = Linear (planar bit is also set)

71


Degree of the spline curve

72


Number of knots

73


Number of control points

74


Number of fit points (if any)

42


Knot tolerance (default = 0.0000001)

43


Control-point tolerance (default = 0.0000001)

44


Fit tolerance (default = 0.0000000001)

12


Start tangent-may be omitted (in WCS)
DXF: X value; APP: 3D point

22, 32


DXF: Y and Z values of start tangent-may be omitted (in WCS)

13


End tangent-may be omitted (in WCS)
DXF: X value; APP: 3D point

23, 33


DXF: Y and Z values of end tangent-may be omitted (in WCS)

40


Knot value (one entry per knot)

41


Weight (if not 1); with multiple group pairs, are present if all are not 1

10


Control points (in WCS), one entry per control point
DXF: X value; APP: 3D point

20, 30


DXF: Y and Z values of control points (in WCS), one entry per control point

11


Fit points (in WCS), one entry per fit point
DXF: X value; APP: 3D point

21, 31


DXF: Y and Z values of fit points (in WCS), one entry per fit point
*/

