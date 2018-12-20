
#include "stdafx.h"
#include "shared.h"
#include "Geometry.h"
#include "Objects.h"
#include "Drawing.h"
#include "TinyLIB.h"
#include "SQLite\CppSQLite3U.h"
#pragma comment(lib, "./SQLite/sqlite3.lib")

// ...........................................................................
void get_tinylib_item_data(const CppSQLite3DB& lib)
{
}

using namespace TinyLIB;

// ...........................................................................
//this will load the whole library by file name, it does not load the item blobs
tinylib_set get_tinylib_data(const bfs::path& path)
{
	CppSQLite3DB m_database;
	m_database.open(path.wstring().c_str());

	std::tstring sql(L"SELECT * FROM [Name]");
	CppSQLite3Query q = m_database.execQuery(sql.c_str());

	tinylib_set lib;
	lib.full_path = path;
	auto& items = lib.cur_lib_set;

	for (; !q.eof(); )
	{
		tinylib_item item;
		CSymbolRecord& r = item;
		r.name = q.getStringField(_T("Name"));
		r.NameID = q.getIntField(_T("NameID"));
		r.reference = q.getStringField(_T("Reference"));
		r.description = q.getStringField(_T("Description"));
		r.name_type = static_cast<SymbolFieldType> (q.getIntField(_T("ShowName")));
		r.ref_type = static_cast<SymbolFieldType> (q.getIntField(_T("ShowRef")));

		std::tstring asql(L"SELECT * FROM [Attribute] WHERE [NameID]=");
		asql += boost::lexical_cast< std::tstring >(r.NameID);
		CppSQLite3Query aq = m_database.execQuery(asql.c_str());

		r.fields.erase(r.fields.begin(), r.fields.end());
		for (; !aq.eof(); )
		{
			CSymbolField field;
			field.field_name = aq.getStringField(L"AttName");
			field.field_value = aq.getStringField(L"AttValue");
			field.field_type = static_cast<SymbolFieldType> (aq.getIntField(L"ShowAtt"));
			aq.nextRow();
		}

		r.fields_loaded = TRUE;
		items.push_back(item);
		q.nextRow();
	}
	return lib;
}

// ...........................................................................
//load the blob of one lib item
std::wstring get_tinylib_symbol(const bfs::path& path, size_t index)
{
	std::wstring data;
	CppSQLite3DB m_database;
	m_database.open(path.wstring().c_str());

	std::wstring ssql(L"SELECT * FROM [Symbol] WHERE SymbolID=");
	ssql += boost::lexical_cast< std::wstring >(index);
	CppSQLite3Query qs = m_database.execQuery(ssql.c_str());
	if (!qs.eof())
	{
		int len;
		data = pugi::as_wide((char *)qs.getBlobField(L"Data", len));
		//		if( data.size( ) )
		{
			//item.symbol= ch;
			boost::replace_all(data, "\r\n", "\n");
			BUG_OS("in: get_tinylib_symbol\n" << pugi::as_utf8(data) << std::endl);
		}
	}
	return data;
}

// ...........................................................................
std::wstring tinylib_set::load_symble(tinylib_set::iterator it)
{
	return get_tinylib_symbol(full_path, it->NameID);
}

