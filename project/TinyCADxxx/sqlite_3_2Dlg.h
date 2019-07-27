
// sqlite_3_2Dlg.h : header file
//

#pragma once
#include "afxwin.h"


// Csqlite_3_2Dlg dialog
class Csqlite_3_2Dlg : public CDialogEx
{
// Construction
public:
	Csqlite_3_2Dlg( CWnd* pParent= NULL );

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SQLITE_3_2_DIALOG };
#endif

//testing
	std::vector< std::string > test_names;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON m_hIcon;

DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
public:
	CComboBox cModeCombo;
};
