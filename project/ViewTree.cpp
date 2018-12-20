
#include "stdafx.h"
#include <ctrlext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

//IMPLEMENT_DYNCREATE( CViewTreeEx, CTreeCtrlEx )

CViewTreeEx::CViewTreeEx( )
{
}

CViewTreeEx::~CViewTreeEx( )
{
}

BEGIN_MESSAGE_MAP( CViewTreeEx, CTreeCtrlEx )
END_MESSAGE_MAP( )

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTreeEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

