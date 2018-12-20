#pragma once

//instead of a default << operator this specializes for svg and looks like a named function
void svg_stream( std::ostream& os, const SP_BaseItem& in );

class svg_obj
{
public:
	svg_obj( const SP_BaseItem& in ) : obj( in ) {}
	friend std::ostream& operator<<( std::ostream& os, const svg_obj& sobj ) 
	{
		//os << "M ";
		svg_stream( os, sobj.obj );
		return os;
	}
private:
	const SP_BaseItem& obj;
};

void SVG_WriteFile( bfs::path& path, DrawingObects& draw );

