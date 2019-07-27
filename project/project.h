#pragma once

//this does not fit into the doc/view paradigm. It is management for a group of documents that comprise a project.
//testing//There may be no way out of it, like CWinApp, there will be a global project(s) object

struct project_file
{
	bfs::path file_path;
	HE_UUID id;
};
using project_files_type = std::vector<project_file>;

struct project
{
	project_files_type files;
};