

#include "stdafx.h"
#include "CAD.h"
#include "std_share.h"

profile_t& get_user_profile( ) { return theApp.profile; }

tinylib_pathvect_type load_tinylib_paths(const bfs::path& dir)
{
	bfs::directory_iterator plib(dir);
	tinylib_pathvect_type vect;
	for (; plib != bfs::directory_iterator(); ++plib)
	{
		if (plib->path().extension() == L".TCLib")
			//keeping full path so multiple folders are valid
			vect.push_back(plib->path());
	}
	return vect;
}

//concrete objects
tinycad_repo_struct tinycad_repo;
