#pragma once

//This is a TinyLib specific unit. Mostly taekn from the TinyCAD program.

//tinylib_item -- derived from the TinyCAD CSymbolRecord
//	a partially or completly load record from a tiny lib.
//	partially means the 'blob' has not been loaded and parsed
//	derived so as to also contain the 'blob'

//tinylib_set
//	container for all of a tiny lib. Index accessable.




//assume we have the lib path from regestry
static bfs::path tiny_root_libpath(L"C:\\cpp\\Tiny_CAD\\Tiny_CAD\\TinyCAD\\trunk\\libs");

// ................................................................
namespace TinyLIB {
	// ................................................................
	enum SymbolFieldType	//Values used for 2.70.00 Beta 2 and later
							/** Note:  The values assigned to this enum are written into the schematic and symbol design files.
							*  These values must NOT be changed, or a database revision will have to be implemented!
							*/
	{
		default_show = 0,
		default_hidden = 1,
		always_hidden = 2,
		extra_parameter = 3,	//Used to indicate a parameter that was added by the user to the schematic, not in the symbol editor
		default_show_name_and_value_only_if_value_not_empty = 4,
		default_show_name_and_value = 5,
		default_show_value_only_if_value_not_empty = 6,
		last_symbol_field_type	//This value is always last, and is never written to a design file so that its value may safely change from release to release
								//It is used to determine how many enums have been defined (sequential values assumed).
	};

	// ................................................................
	class CSymbolField
	{
	public:
		std::wstring field_name;
		std::wstring field_value; // Not used!
		std::wstring field_default;
		//	CDPoint field_pos; // Not used!


		SymbolFieldType field_type;

		// default constructor to initialize field_pos to (0,0)
		CSymbolField()
		{
			//		field_pos = CDPoint(0, 0);
		}

		// Save this field
		//void Save(CStream &); // Save this symbol field into an archive
		//void SaveXML(CXMLWriter &xml, bool simple); // Save this symbol field into xml
		//void Load(CStream &); // Load this symbol field from an archive
		//void LoadXML(CXMLReader &xml); // Load this symbol field from xml
	};

	// ................................................................
	// Here is the data that is associated with
	// all symbols
	class CSymbolRecord
	{

	public:
		// The constructor
		CSymbolRecord() {}

		std::wstring name;				// The name of this symbol
		std::wstring reference;			// The default reference
		std::wstring description;		// The description of this symbol
		SymbolFieldType name_type;	// Do we show the name by default?
		SymbolFieldType ref_type;	// Do we show the reference by default?
		DWORD NameID;				// The ID of this name when stored in a database

		//void SaveXML(CXMLWriter &xml); // Save this symbol into an archive
		//void LoadXML(CXMLReader &xml); // Load this symbol from an archive

		// Here are the other fields we can have
		std::vector<CSymbolField> fields;
		bool fields_loaded;
	};
}// namespace TinyLIB

// ................................................................
struct tinylib_item : TinyLIB::CSymbolRecord
{
	std::wstring symbol;
};

// ................................................................
struct tinylib_set
{
	bfs::path full_path;
	using vect_type =  std::vector< tinylib_item >;
	using iterator = std::vector< tinylib_item >::iterator;

	vect_type cur_lib_set;

	size_t size() const { return cur_lib_set.size(); }
	iterator begin() { return cur_lib_set.begin(); }
	iterator end() { return cur_lib_set.end(); }
	tinylib_item& at(size_t index) { return cur_lib_set.at(index); }
	iterator get_by_name(const std::wstring& name) {
		return std::find_if(cur_lib_set.begin(), cur_lib_set.end(),
			[&name](const tinylib_item& item) {return item.name == name; });
	}
	std::wstring load_symble(iterator it);
};

// ................................................................
//loads a library from path
tinylib_set get_tinylib_data(const bfs::path& path);
//Get the library item symbol blob, is an XML string; by path_name
std::wstring get_tinylib_symbol(const bfs::path& path, size_t index);
//convert the tinyCAD arc to a BezierCubeItem
SP_BaseItem GetTinyArc(const bg_point& a, const bg_point& b, size_t arc);

//Draw Object for electrical pins....

