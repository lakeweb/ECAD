
// Gears.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "app_share.h"
#include "HEWinApp.h"

#include <ctrlext.h>
#include "Scroller.h"

//#include "cad_full.h"
#include "Geometry.h"
#include "objects.h"
#include "shared.h"
#include "Drawing.h"

#include "app_share.h"
#include "CAD.h"
#include "MainFrm.h"
#include "CADDoc.h"
#include "CADView.h"
#include "CADFrame.h"

#include "TestCADDoc.h"
#include "GearsDoc.h"
#include "TestCADView.h"
#include "GearsView.h"

#include "TinyLIB.h"
#include "view_share.h"
#include "BXLDoc.h"
#include "TinyCADDoc.h"

#include "KiCADDoc.h"
#include "About.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Create_STD_OUT_Console( );

CADApp theApp;

BEGIN_MESSAGE_MAP( CADApp, HEWinApp )
	ON_COMMAND(ID_APP_ABOUT, &CADApp::OnAppAbout)
	// Standard file based document commands

	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, &CADApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPENBXLFILE, &CADApp::OnFileOpenbxlfile)
END_MESSAGE_MAP()

// ............................................................................
CADApp::CADApp( )
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Gears.AppID.NoVersion"));
}

BOOL CADApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	appdata_path = get_user_folder();
	HEWinApp::InitInstance();

	//for now, keep another settings xml
	{
		bfs::path other_user_data_file(appdata_path / "other_user_data.xml");
		CXML xml;
		auto node = xml.Open(other_user_data_file);
		node.SetElement("test", "the test value");
		xml.Close();
	}
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	AfxInitRichEdit2();

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(TestCADDoc),
		RUNTIME_CLASS(CADFrame),
		RUNTIME_CLASS(TestCADView) //changed from CADView to TestCADView for testing
	);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_GEARSTYPE,
		RUNTIME_CLASS(CGearsDoc),
		RUNTIME_CLASS(CADFrame),
		RUNTIME_CLASS(CGearsView)
	);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_BXLTYPE,
		RUNTIME_CLASS(CBXLDoc),
		RUNTIME_CLASS(CADFrame),
		RUNTIME_CLASS(CADView)
	);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_TINYCAD_TYPE,
		RUNTIME_CLASS(TinyCADDoc),
		RUNTIME_CLASS(CADFrame),
		RUNTIME_CLASS(CADView)
	);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_KICAD_PCB,
		RUNTIME_CLASS(KiCADDoc),
		RUNTIME_CLASS(CADFrame),
		RUNTIME_CLASS(CADView)
	);
	AddDocTemplate(pDocTemplate);

#ifdef _DEBUG
	Create_STD_OUT_Console();
#endif
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	switch (3)
	{
	case 0:
		//the testing doc, will open TestCADView....
		OpenDocumentFile(_T("test_files/test.hcd"));
		break;
	case 1:
		//Open a BXL file.
		OpenDocumentFile(L"C:\\cpp\\TinyBXL\\project\\test_files\\MSP430G2253_PW_20.bxl");
		break;
	case 2:
		//Open a TinyCAD lib symbol
		OpenDocumentFile(LR"("C:\cpp\TinyBXL\project\test_files\TinyCAD\DISCRETE.TCLib")");
		break;
	case 3:
		//Open a TinyCAD lib symbol
//		OpenDocumentFile(LR"("..\KiCAD_project\test.kicad_pcb")");
//		OpenDocumentFile(LR"(".\test_files\sonde xilinx.kicad_pcb")");
		OpenDocumentFile(LR"(".\test_files\ecc83-pp.kicad_pcb")");
		break;
	}//switch(file)

	//OnFileNew( );

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->DragAcceptFiles();
	//testing..........................
	//OnFileOpenbxlfile();
	return TRUE;
}

// ............................................................................
int CADApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return HEWinApp::ExitInstance();
}

// ............................................................................
void CADApp::OnFileNew( )
{
	POSITION pos= m_pDocManager->GetFirstDocTemplatePosition( );
	m_pDocManager->GetNextDocTemplate( pos );
	CDocTemplate* pTemplate= m_pDocManager->GetNextDocTemplate( pos );
	pTemplate->OpenDocumentFile(NULL);

	//CDocTemplate* pTemplate = (CDocTemplate*)m_pDocManager->m_templateList.GetHead();
//	CWinAppEx::OnFileNew( );
}

// ............................................................................
void CADApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

// ............................................................................
void CADApp::LoadCustomState()
{
}

// ............................................................................
void CADApp::SaveCustomState()
{
}

// .......................................................................
void CADApp::OnAppAbout( )
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal( );
}


// .......................................................................
// .......................................................................
// .......................................................................

void CADApp::OnFileOpenbxlfile( )
{
	bfs::path path( "C:\\cpp\\TinyBXL\\project\\test_files\\MSP430G2253_PW_20.bxl" );
	auto the_parser = Bridge::bxl;

	std::ifstream in(path.wstring(), std::ios::binary);
	std::string bxl_text;

	//decompress the bxl file
	{
		BXLReader::Buffer reader( in );
		std::string result = reader.decode_stream( );

		//visual dump
		std::ofstream os("./bxl_output.txt");
		os.write(result.c_str(), result.size());

		bxl_text = result;
	}

	//std::cout << "\n\n" << "parse the BXL\n\n";

	using namespace boost::spirit::x3;
	typedef std::string::const_iterator iterator_type;

	Component::ComponentStore comp;
	iterator_type begin = bxl_text.begin();
	iterator_type end = bxl_text.end();
	bool r = phrase_parse(begin, end, the_parser, space, comp);
	//std::cout << std::boolalpha << "pass: " << r << std::endl;

	//POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	//CDocTemplate* pTemplate;

	CDocTemplate* pTemplate = m_pDocManager->GetBestTemplate(L"dummy.bxl");
	pTemplate->OpenDocumentFile(path.wstring().c_str());
	//for (;;)
	//{
	//	POSITION pos;
	//	pos = m_pDocManager->GetFirstDocTemplatePosition();
	//	pTemplate = m_pDocManager->GetNextDocTemplate(pos);
	//	if (pTemplate->
	//}
	//CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(pos);
	//pTemplate->OpenDocumentFile(NULL);
}

// .......................................................................
// .......................................................................
// .......................................................................
