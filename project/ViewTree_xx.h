
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree window

//class CViewTree : public CTreeCtrl
class CViewTree : public CTreeCtrlEx
{
// Construction
public:
	CViewTree();

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
