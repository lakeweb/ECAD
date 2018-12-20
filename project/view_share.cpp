
#include "stdafx.h"
#include "shared.h"
#include "Geometry.h"
#include "Objects.h"
#include "TinyLIB.h"
#include "view_share.h"

//open a TinyLib by interface id.

tinylib_set get_tinylib_by_id(const doc_lib_list& list, uint64_t id)
{
	auto it = std::find_if(list.begin(), list.end(),
		[id](const doc_lib_item& dl) {return dl.id == id; });

	if (it != list.end() && it->parent)
		return get_tinylib_data(it->parent->pathname);
	//else
	return tinylib_set();
}

//tinylib_item get_tinylib_item_by_id(uint64_t id);

