#pragma once

// ..........................................................................
//MFC specific, like to clean up a thread exit....
#define CLEARMESSAGEPUMP \
{\
	MSG msg;\
	while( ::PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ) )\
		AfxPumpMessage( );\
}\

// ..........................................................................
//MFC specific for the actual view message
struct DocNotifyObject
{
	NMHDR nmHdr;
	uint64_t id;
	bool bVal;
};

// ..........................................................................
//UI Genaric
//a communication structure
struct tree_view_struct
{
	//in:
	tree_view_struct() :id(0) {}
	const wchar_t* label;
	size_t icon_pos;
	//out:
	//the id is assigned by the UI tree as it does not know about our internals.
	//for MFC, it will be HTREEITEM, doesn't matter to us, just a number.
	uint64_t id;
};

//this is out there, UI specific
//For tree view interface passed as pointer through LPARAM when MFC
bool AddTreeViewNode(tree_view_struct* data = nullptr);

//Send a APP_INFO_NOTIFY message to main frame family of output windows
void SetInfoWnd(const info_notify& info, void* pinfo= nullptr);

// ..........................................................................
//UI Genaric
//a container of data that is linked by some id to a tree or list view.
//  'pathname' can be any text data used by the remote view.
struct doc_list_item
{
	size_t lib_index;
	std::wstring listname;
	uint64_t id;
	//for lib items
	doc_list_item* parent;//if a tree view

	doc_list_item(const std::wstring& pn, uint64_t ti) :listname(pn), id(ti), parent(nullptr) {}
	doc_list_item(const wchar_t* pn, const uint64_t ti, doc_list_item* pid = nullptr) :listname(pn), id(ti), parent(pid) {}
};
//gota use list so items do not move
using doc_lib_list = std::list<doc_list_item>;


void Create_STD_OUT_Console();
