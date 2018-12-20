
// sqlite_3_2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Csqlite_3_2App:
// See sqlite_3_2.cpp for the implementation of this class
//

class Csqlite_3_2App : public CWinApp
{
public:
	Csqlite_3_2App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Csqlite_3_2App theApp;