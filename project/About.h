

// .......................................................................
#include "afxcmn.h"

//......................................................................//
class CRichBox : public CRichEditCtrl
{
	//DECLARE_DYNAMIC(CStatic)

public:
	virtual BOOL Create( LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff );

	CDC dcTarget;
	//CRichEditCtrl cEdit;
public:
	//operator CRichEditCtrl& ( ) { return cEdit; }
	DECLARE_MESSAGE_MAP( )
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
};

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	CRichBox cRichAck;

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	double valDouble;
};
