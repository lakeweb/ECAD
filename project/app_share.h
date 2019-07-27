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

//UUID object management

#pragma once

//.................................
//.................................
class HE_UUID
{
	UUID id;
	bool bValid;

public:
	HE_UUID(UUID& in)
		:id(in)
		, bValid(true)//assume as is assighnment constructor
	{ }
	HE_UUID(std::string& in)
	{
		bValid = SetID(in.c_str());
	}
	HE_UUID(const char* in)
	{
		bValid = SetID(in);
	}
	HE_UUID() :bValid(false) { ZeroMemory(&id, sizeof(UUID)); }

	bool IsValid()const { return bValid; }
	void Generate() { UuidCreate(&id); bValid = true; }
	void SetID(UUID* inId) { id = *inId; }
	bool SetID(const char* psId)
	{
		UUID tid;
		if (UuidFromStringA((RPC_CSTR)psId, &tid) == RPC_S_OK)
		{
			id = tid;
			return bValid = true;
		}
		else
		{
			id = { 0,0,0,0 };
			return bValid = false;
		}
	}
	operator std::string() const
	{
		//ASSERT( id.Data1 && id.Data2 );
		std::string strT;
		RPC_CSTR us = NULL;
		if (RPC_S_OK == UuidToStringA(&id, &us) && us && *us)
		{
			if (id == GUID_NULL)
				strT = "GUID_NULL";
			else
				strT = (char*)us;
			RpcStringFreeA(&us);
		}
		return strT;
	}
	std::string string() const { return this->operator std::string(); }
	operator bool() const //return false if uuid is null
	{
		RPC_STATUS stat;
		UUID temp = id;
		return !UuidIsNil(&temp, &stat);
	}

	bool IsNull() const { return !*this; }

	bool operator < (const HE_UUID& inuuid) const
	{
		// comparison logic goes here
		return memcmp(&inuuid.id, &id, sizeof(UUID)) < 0;
	}

	bool operator == (const HE_UUID& inuuid) const
	{
		RPC_STATUS stat;
		UUID temp = id;
		UUID temp2 = inuuid.id;
		return !!UuidEqual(&temp2, &temp, &stat);
	}
	operator UUID () { return id; }
};

inline std::ostream& operator << (std::ostream& s, const HE_UUID& y)
{
	s << std::string(y);
	return s;
}

//inline bg_box CREctTo_bg_box(const CRect& r) { return bg_box(bg_point(r.left, r.top), bg_point(r.right, r.bottom)); }
//  ---------------------------------------------------------------------------
#pragma comment(lib, "rpcrt4")

/*
HE_UUID testid;
testid.Generate( );
CString strTest( testid );
HE_UUID testid2;
testid2.SetID( strTest );
CString strTest2( testid2 );
*/

//using sp_tinycad_repo = boost::shared_ptr<tinycad_repo>;
