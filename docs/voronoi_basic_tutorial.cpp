// Boost.Polygon library voronoi_basic_tutorial.cpp file

//          Copyright Andrii Sydorchuk 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)

// See http://www.boost.org for updates, documentation, and revision history.

#include &lt;cstdio&gt;
#include &lt;vector&gt;

#include &lt;<a href="../../../boost/polygon/voronoi.hpp">boost/polygon/voronoi.hpp</a>&gt;
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::x;
using boost::polygon::y;
using boost::polygon::low;
using boost::polygon::high;

#include &quot;voronoi_visual_utils.hpp&quot;

struct Point {
  int a;
  int b;
  Point(int x, int y) : a(x), b(y) {}
};

struct Segment {
  Point p0;
  Point p1;
  Segment(int x1, int y1, int x2, int y2) : p0(x1, y1), p1(x2, y2) {}
};

namespace boost {
namespace polygon {

template &lt;&gt;
struct geometry_concept&lt;Point&gt; {
  typedef point_concept type;
};

template &lt;&gt;
struct point_traits&lt;Point&gt; {
  typedef int coordinate_type;

  static inline coordinate_type get(
      const Point&amp; point, orientation_2d orient) {
    return (orient == HORIZONTAL) ? point.a : point.b;
  }
};

template &lt;&gt;
struct geometry_concept&lt;Segment&gt; {
  typedef segment_concept type;
};

template &lt;&gt;
struct segment_traits&lt;Segment&gt; {
  typedef int coordinate_type;
  typedef Point point_type;

  static inline point_type get(const Segment&amp; segment, direction_1d dir) {
    return dir.to_int() ? segment.p1 : segment.p0;
  }
};
}  // polygon
}  // boost

// Traversing Voronoi edges using edge iterator.
int iterate_primary_edges1(const voronoi_diagram&lt;double&gt;&amp; vd) {
  int result = 0;
  for (voronoi_diagram&lt;double&gt;::const_edge_iterator it = vd.edges().begin();
       it != vd.edges().end(); ++it) {
    if (it-&gt;is_primary())
      ++result;
  }
  return result;
}

// Traversing Voronoi edges using cell iterator.
int iterate_primary_edges2(const voronoi_diagram&lt;double&gt; &amp;vd) {
  int result = 0;
  for (voronoi_diagram&lt;double&gt;::const_cell_iterator it = vd.cells().begin();
       it != vd.cells().end(); ++it) {
    const voronoi_diagram&lt;double&gt;::cell_type&amp; cell = *it;
    const voronoi_diagram&lt;double&gt;::edge_type* edge = cell.incident_edge();
    // This is convenient way to iterate edges around Voronoi cell.
    do {
      if (edge-&gt;is_primary())
        ++result;
      edge = edge-&gt;next();
    } while (edge != cell.incident_edge());
  }
  return result;
}

// Traversing Voronoi edges using vertex iterator.
// As opposite to the above two functions this one will not iterate through
// edges without finite endpoints and will iterate only once through edges
// with single finite endpoint.
int iterate_primary_edges3(const voronoi_diagram&lt;double&gt; &amp;vd) {
  int result = 0;
  for (voronoi_diagram&lt;double&gt;::const_vertex_iterator it =
       vd.vertices().begin(); it != vd.vertices().end(); ++it) {
    const voronoi_diagram&lt;double&gt;::vertex_type&amp; vertex = *it;
    const voronoi_diagram&lt;double&gt;::edge_type* edge = vertex.incident_edge();
    // This is convenient way to iterate edges around Voronoi vertex.
    do {
      if (edge-&gt;is_primary())
        ++result;
      edge = edge-&gt;rot_next();
    } while (edge != vertex.incident_edge());
  }
  return result;
}

int main() {
  // Preparing Input Geometries.
  std::vector&lt;Point&gt; points;
  points.push_back(Point(0, 0));
  points.push_back(Point(1, 6));
  std::vector&lt;Segment&gt; segments;
  segments.push_back(Segment(-4, 5, 5, -1));
  segments.push_back(Segment(3, -11, 13, -1));

  // Construction of the Voronoi Diagram.
  voronoi_diagram&lt;double&gt; vd;
  construct_voronoi(points.begin(), points.end(),
                    segments.begin(), segments.end(),
                    &amp;vd);

  // Traversing Voronoi Graph.
  {
    printf(&quot;Traversing Voronoi graph.\n&quot;);
    printf(&quot;Number of visited primary edges using edge iterator: %d\n&quot;,
        iterate_primary_edges1(vd));
    printf(&quot;Number of visited primary edges using cell iterator: %d\n&quot;,
        iterate_primary_edges2(vd));
    printf(&quot;Number of visited primary edges using vertex iterator: %d\n&quot;,
        iterate_primary_edges3(vd));
    printf(&quot;\n&quot;);
  }

  // Using color member of the Voronoi primitives to store the average number
  // of edges around each cell (including secondary edges).
  {
    printf(&quot;Number of edges (including secondary) around the Voronoi cells:\n&quot;);
    for (voronoi_diagram&lt;double&gt;::const_edge_iterator it = vd.edges().begin();
         it != vd.edges().end(); ++it) {
      std::size_t cnt = it-&gt;cell()-&gt;color();
      it-&gt;cell()-&gt;color(cnt + 1);
    }
    for (voronoi_diagram&lt;double&gt;::const_cell_iterator it = vd.cells().begin();
         it != vd.cells().end(); ++it) {
      printf(&quot;%lu &quot;, it-&gt;color());
    }
    printf(&quot;\n&quot;);
    printf(&quot;\n&quot;);
  }

  // Linking Voronoi cells with input geometries.
  {
    unsigned int cell_index = 0;
    for (voronoi_diagram&lt;double&gt;::const_cell_iterator it = vd.cells().begin();
         it != vd.cells().end(); ++it) {
      if (it-&gt;contains_point()) {
        std::size_t index = it-&gt;source_index();
        Point p = points[index];
        printf(&quot;Cell #%ud contains a point: (%d, %d).\n&quot;,
               cell_index, x(p), y(p));
      } else {
        std::size_t index = it-&gt;source_index() - points.size();
        Point p0 = low(segments[index]);
        Point p1 = high(segments[index]);
        if (it-&gt;source_category() ==
            boost::polygon::SOURCE_CATEGORY_SEGMENT_START_POINT) {
          printf(&quot;Cell #%ud contains segment start point: (%d, %d).\n&quot;,
                 cell_index, x(p0), y(p0));
        } else if (it-&gt;source_category() ==
                   boost::polygon::SOURCE_CATEGORY_SEGMENT_END_POINT) {
          printf(&quot;Cell #%ud contains segment end point: (%d, %d).\n&quot;,
                 cell_index, x(p0), y(p0));
        } else {
          printf(&quot;Cell #%ud contains a segment: ((%d, %d), (%d, %d)). \n&quot;,
                 cell_index, x(p0), y(p0), x(p1), y(p1));
        }
      }
      ++cell_index;
    }
  }
  return 0;
}
