
// sqlite_3_2Dlg.cpp : implementation file
//

#include "stdafx.h"
#define USING_UNI_CONVERTER
#include "string_types.h"
#include "sqlite3.h"
//#include "SQLite/CppSQLite3U.h"
#include "Symbol.h"
#include "sqlite_3_2.h"
#include "sqlite_3_2Dlg.h"
#include "afxdialogex.h"

#include "Stream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef _DEBUG
std::ofstream testos( "C:\\cpp\\sqlite_3200100\\test.txt" );
#define TEST( x ) {  testos << x; }
#else
#define TEST( x ) __noop;
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Csqlite_3_2Dlg dialog



Csqlite_3_2Dlg::Csqlite_3_2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SQLITE_3_2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Csqlite_3_2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MODE, cModeCombo);
}

BEGIN_MESSAGE_MAP(Csqlite_3_2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// Csqlite_3_2Dlg message handlers
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++){
	      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

#include <boost/lexical_cast.hpp>
#include <codecvt>
#include <locale>
#include <string>
#include <cassert>

// ................................................................
struct uni_to_utf
{
	std::string str;
	uni_to_utf( const wchar_t* in )
	{
		str= boost::locale::conv::utf_to_utf<char>( in );
	}
	operator const char* ( ) { return str.c_str( ); }
};

// ................................................................
struct item_type : CSymbolRecord
{
	std::string symbol;
};
typedef std::vector< item_type > item_vect_type;

// ................................................................
BOOL Csqlite_3_2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//sql testing
#if 1
#endif

	CFile cfile( L"C:\\cpp\\TinyCAD_2010\\2017-09-28_10-12-14.xml", CFile::modeRead );
	CArchive arc( &cfile, CArchive::load, 4096 );
	CStreamFile strm( &arc );
	CXMLReader xml( &strm );
	CString cmp( L"ul_xml_library" );
	xml.getChildData( cmp);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Csqlite_3_2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Csqlite_3_2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Csqlite_3_2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

