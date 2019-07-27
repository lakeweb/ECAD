

now using pugi xml...

MIlling and screen drawing................................

Geometry.h
	bases of 2D Cartesian points using
	boost geometry.
	support for transformations, rotations, mirroring, etc...

object.h
	primitive objects for milling and drawing
	a base class: BaseItem and containers use SP_BaseItem
		shared pointers..

Drawing.h
	support for drawing objects.



DrawingObect : top level container for drawing
	wrapper for object_set_t called only ItemSet for now.

ItemSet
	owns asp_obj_vect_t
	which is a vector of shared pointers SP_BaseItem

11/27/2018
	polygons are supported. They can be open, and/or, they can be filled.
	An ItemSet stored as a base object will not be considered a polygon. This may change.
	But if an ItemSet is stored in an ItemSet, it will be treated as a polygon.

	So far, the fill is a 

...........................................................................
The Tiny Parse
...........................................................................
boost::geometry does not support curved lines much less in a line string.
So each polygon is stored on a ItemSet.

			//<POLYGON pos = '15.00000,15.00000' style = '4' fill = '1'>
			//	<POINT pos = '0.00000,0.00000' arc = '0' / >
			//create an object for each polygon to preserve tiny objects.
			//geometry does not support curves in a linestring so....


Note that CppSQLite3U is rather unusable. The only reason it is in here is for tinylib
	it will be updated.....

On the local system, symlinks are set for:
folder: <include/xml.h>
file: <BXLReader/BXL_Component.h>
