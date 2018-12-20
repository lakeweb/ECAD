#pragma once


typedef struct _profile
{
	_profile( )
		:export_type( 2 )
		,path_bxl_folder("../test_files")// TODO
	{ }
	size_t export_type;

	bfs::path path_bxl_folder;

}profile_t;

//TODO this stuff will point properly in release

inline bfs::path get_program_folder( ) { return bfs::path( L".\\ProgramFiles\\" ); } // use 'program' in debug;
inline bfs::path get_user_folder( ) { return bfs::path( L".\\AppData\\" ); }
inline bfs::path get_bxl_folder() { return bfs::path(L".\\project\\test_files"); }
inline bfs::path get_tinylib_folder() { return bfs::path(LR"(.\test_files\TinyCAD)"); }
profile_t& get_user_profile( );

//Tiny LIb Stuff
// .............................................................................................
using tinylib_pathvect_type = std::vector<bfs::path>;
tinylib_pathvect_type load_tinylib_paths(const bfs::path& dir= get_tinylib_folder());

struct tinycad_repo_struct {
	tinycad_repo_struct() { tinylib_path_vect = load_tinylib_paths(); }
	tinylib_pathvect_type tinylib_path_vect;
};

extern tinycad_repo_struct tinycad_repo;

//using sp_tinycad_repo = boost::shared_ptr<tinycad_repo>;
