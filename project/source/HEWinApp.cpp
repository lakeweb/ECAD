

//HeWinApp.cpp

#include "stdafx.h"
#include "../stdafx.h"
#include <debug.h>
#include "HEWinApp.h"


SP_XML sp_xml_settings_store_global;
SP_XMLNODE sp_currentNode_settings_store_global;
CString strSysRegPath;

#ifdef _DEBUG
std::ofstream sros( "store_read.log" );
#define SRLOG( x ) sros << x << std::endl;
std::wofstream swos( "store_write.log" );
#define SWLOG( x ) swos << x << std::endl;
#else
#define SRLOG( x ) __noop;
#define SWLOG( x ) __noop;
#endif
// This (was*) a part of the Microsoft Foundation Classes C++ library.
// Hacked for XML storage
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


IMPLEMENT_DYNCREATE( XMLSettingsStore, CSettingsStore )

static CString __stdcall PreparePath(LPCTSTR lpszPath)
{
	ENSURE(lpszPath != NULL);
	CString strPath = lpszPath;

	int iPathLen = strPath.GetLength();
	if (iPathLen > 0 && strPath [iPathLen - 1] == _T('\\'))
	{
		strPath = strPath.Left(iPathLen - 1);
	}
	return strPath;
}

// .................................................................
bool XMLSettingsStore::OpenXML( LPCTSTR psPathFile )
{
	ASSERT( sp_xml.use_count( ) );
	if (!sp_xml->Open(psPathFile, 0, "Root"))// , CXML::modeProgFile) )
		return false;

	*sp_currentNode= sp_xml->get_current_node( );
	SWLOG(L"OpenXML: " << psPathFile << " " << sp_currentNode->name( ) )
	SRLOG("OpenXML: " << to_utf8(psPathFile) << " " << sp_currentNode->name())
	return *sp_currentNode;// ->IsValid();
}

// .................................................................
XMLSettingsStore::XMLSettingsStore( )
	:sp_xml( sp_xml_settings_store_global )
	,sp_currentNode( sp_currentNode_settings_store_global )
	,psRegPath( strSysRegPath )
{}

// .................................................................
XMLSettingsStore::XMLSettingsStore(BOOL bAdmin, BOOL bReadOnly)
	:CSettingsStore( bAdmin, bReadOnly )
	,sp_xml( sp_xml_settings_store_global )
	,sp_currentNode( sp_currentNode_settings_store_global )
	,psRegPath( strSysRegPath )
{
	m_reg.m_hKey = bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
}

// .................................................................
void XMLSettingsStore::SetCurrentNode( LPCTSTR psNode )
{
	*sp_currentNode = sp_xml->GetNode(to_utf8(psNode),m_bReadOnly);
}

// .................................................................
XMLSettingsStore::~XMLSettingsStore( )
{
	if( ! m_bReadOnly )
		Close( );//if a valid file system path, will be saved
}

// .................................................................
bool XMLSettingsStore::MakePath( LPCTSTR psIn, CString& strIn )
{
	bool bNoDummy = false;
	CString strT(psIn);
	CString strR(psRegPath);
	if (strT.Left(strR.GetLength()) == strR)
	{
		strT.Delete(0, strR.GetLength());
		if (strT.GetLength())
			strT.Insert(0, _T("/Root/Workspace/"));
		else
			strT = _T("/Root/Workspace");
	}
	else
	{
		bNoDummy = true;
		XMLSTRACE(_T("NO DUMMY: %s\n"), psIn);
	}
	if (!strT.IsEmpty() && strT[strT.GetLength() - 1] == _T('\\'))
	{
		strT.Delete(strT.GetLength() - 1);
		//		strT+= _T('/');
	}
	strIn = strT;
	SWLOG( L"makepath: " << (LPCTSTR)strT )
	SRLOG( "makepath: " << to_utf8( (LPCTSTR)strT ) )
	return bNoDummy;
}

// .................................................................
BOOL XMLSettingsStore::Open( LPCTSTR lpszPath )
{
	CString strT;
	if (MakePath(lpszPath, strT))
		return FALSE;

	SWLOG(L" REG Open " << (LPCTSTR)strT << " ro: " << (m_bReadOnly ? _T("true") : _T("false")));
	SRLOG( " REG Open " << to_utf8(strT) << " ro: " << (m_bReadOnly ? "true" : "false"));
	auto check= *sp_currentNode = sp_xml->GetNode(to_utf8(strT), ! m_bReadOnly);
	SRLOG( "        found: " << check.name() << std::endl)
	return !! *sp_currentNode;
}

// .................................................................
BOOL XMLSettingsStore::CreateKey( LPCTSTR lpszPath )
{
	CString strT;
	if( MakePath( lpszPath, strT ) )
		return FALSE;

	if( m_bReadOnly )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	*sp_currentNode = sp_xml->GetNode(to_utf8(strT), ! m_bReadOnly);//testing
	SWLOG( L" makekey: " << (LPCTSTR)strT );
	return !! *sp_currentNode;
}

// .................................................................
void XMLSettingsStore::Close( )
{
	m_reg.Close( );
	SWLOG(pugi::as_wide(sp_currentNode->name()) << L" ---close\n");
	SRLOG(sp_currentNode->name() << " ---close\n");
}

// .................................................................
// read from XML
// .................................................................
BOOL XMLSettingsStore::Read(LPCTSTR lpszValueName, int& nValue)
{
	SRLOG("  int name: " << to_utf8(lpszValueName) << " \tval: " << nValue)
		return Read(lpszValueName, (DWORD&)nValue);
}

// .................................................................
BOOL XMLSettingsStore::Read(LPCTSTR lpszValueName, DWORD& dwValue)
{
	auto node= sp_currentNode->GetElement(to_utf8(lpszValueName), dwValue, ! m_bReadOnly);
	SRLOG( "  DWORD name: " << to_utf8(lpszValueName) << " \tval: " << dwValue )
	return node ? TRUE : FALSE;
}

// .................................................................
BOOL XMLSettingsStore::Read(LPCTSTR lpszValueName, CString& strValue)
{
	ENSURE(lpszValueName != NULL);

	strValue.Empty( );

	std::string sval;
	auto node= sp_currentNode->GetElement(to_utf8(lpszValueName), sval, ! m_bReadOnly);

	SRLOG( "  string name: " << to_utf8(lpszValueName)
		<< " \tval: " << sval 
		<< " tf: " << (node ? "true" : "false") )

	return TRUE;
}

// .................................................................
BOOL XMLSettingsStore::Read(LPCTSTR lpszValueName, BYTE** ppData, UINT* pcbData)
{
	ENSURE(lpszValueName != NULL);
	ENSURE(ppData != NULL);
	ENSURE(pcbData != NULL);

	*ppData = NULL;
	*pcbData = 0;

	if( m_bReadOnly && ! sp_currentNode->GetElement(to_utf8(lpszValueName).c_str()))
		return FALSE;

	auto node = sp_currentNode->GetElementArray(to_utf8(lpszValueName).c_str(), ppData, pcbData);

	SRLOG("  BYTE name: " << to_utf8(lpszValueName) << " \tCNT: " << *pcbData
		<< " tf: " << (node ? "true" : "false") << " loc: " << std::hex << "0x" << (unsigned long)*ppData );

	return node ? TRUE : FALSE;
}

// .................................................................
BOOL XMLSettingsStore::Read( LPCTSTR lpszValueName, CObject& obj )
{
	BOOL bSucess = FALSE;
	BYTE* pData = NULL;
	UINT uDataSize;

	if (!Read(lpszValueName, &pData, &uDataSize))
	{
		ENSURE(pData == NULL);
		SRLOG("read object, return false");
		return FALSE;
	}

	ENSURE(pData != NULL);

	try
	{
		CMemFile file(pData, uDataSize);
		CArchive ar(&file, CArchive::load);

		obj.Serialize(ar);
		bSucess = TRUE;
	}
	catch(CMemoryException* pEx)
	{
		pEx->Delete();
		SRLOG("Memory exception in XMLSettingsStore::Read()!\n");
	}
	catch(CArchiveException* pEx)
	{
		pEx->Delete();
		SRLOG("CArchiveException exception in XMLSettingsStore::Read()!\n");
	}

	SRLOG( "  Object name: " << to_utf8( lpszValueName ) << " \tSIZE: " << uDataSize )
	delete [] pData;
	return bSucess;
}

// .................................................................
BOOL XMLSettingsStore::Read( LPCTSTR lpszValueName, CObject*& pObj )
{
	BOOL bSucess = FALSE;
	BYTE* pData = NULL;
	UINT uDataSize;

	if (!Read(lpszValueName, &pData, &uDataSize))
	{
		ENSURE(pData == NULL);
		SRLOG("read object pointer, return false");
		return FALSE;
	}

	ENSURE(pData != NULL);

	try
	{
		CMemFile file(pData, uDataSize);
		CArchive ar(&file, CArchive::load);
		ar >> pObj;

		bSucess = TRUE;
	}
	catch(CMemoryException* pEx)
	{
		pEx->Delete();
		SRLOG("Memory exception in XMLSettingsStore::Read()!\n");
	}
	catch(CArchiveException* pEx)
	{
		pEx->Delete();
		SRLOG("CArchiveException exception in XMLSettingsStore::Read()!\n");
	}

	SRLOG( "  Object name: " << to_utf8( lpszValueName ) << " \tPTR SIZE: " << uDataSize )
	delete [] pData;
	return bSucess;
}

// .................................................................
BOOL XMLSettingsStore::Read( LPCTSTR lpszValueName, CRect& rect )
{
	assert( *sp_currentNode );

	size_t check;
	DWORD* ptr;
	XMLNODE child = sp_currentNode->GetElementArray(to_utf8(lpszValueName).c_str(), &ptr, &check);
	if (!child)
	{
		SRLOG(" read rect return FALSE");
		return FALSE;
	}
	assert(check == 4);
	memcpy((void*)((LPCRECT)rect), ptr, sizeof(RECT));
	delete[]ptr;
	SRLOG( "Rect name: " << to_utf8( lpszValueName )\
		<< " \t val: " << rect.top << " " << rect.left << " " << rect.bottom << " " << rect.right )
	return TRUE;
}

// .................................................................
//writing methods
// .................................................................
BOOL XMLSettingsStore::Write(LPCTSTR lpszValueName, int nValue)
{
	SWLOG(L"  int_val: " << lpszValueName << " v: " << nValue);
	return Write(lpszValueName, (DWORD) nValue);
}

// .................................................................
BOOL XMLSettingsStore::Write(LPCTSTR lpszValueName, DWORD dwValue)
{
	if( m_bReadOnly )
	{
		ASSERT(FALSE);
		return FALSE;
	}
	sp_currentNode->SetElement(lpszValueName, dwValue );
	SWLOG( L"  dword: " << lpszValueName << " v: " << dwValue );
	return TRUE;
}

// .................................................................
BOOL XMLSettingsStore::Write(LPCTSTR lpszValueName, LPCTSTR lpszData)
{
	if( m_bReadOnly )
	{
		ASSERT(FALSE);
		return FALSE;
	}
	SWLOG( L"  str: " << lpszValueName << L" d: " << lpszData );
	sp_currentNode->SetElement(to_utf8(lpszValueName), to_utf8(lpszData));
	return TRUE;
}

// .................................................................
BOOL XMLSettingsStore::Write( LPCTSTR lpszValueName, CObject& obj )
{
	if( m_bReadOnly )
	{
		ASSERT(FALSE);
		return FALSE;
	}
	BOOL bRes = FALSE;
	try
	{
		CMemFile file;
		{
			CArchive ar(&file, CArchive::store);
			obj.Serialize(ar);
			ar.Flush();
		}
		DWORD dwDataSize = (DWORD) file.GetLength();
		LPBYTE lpbData = file.Detach();

		if (lpbData == NULL)
		{
			SWLOG(L"  : " << lpszValueName << L" obj: " << typeid(obj).name());
			return FALSE;
		}
		bRes = Write(lpszValueName, lpbData, (UINT) dwDataSize);
		free(lpbData);
	}
	catch(CMemoryException* pEx)
	{
		pEx->Delete();
		TRACE(_T("Memory exception in XMLSettingsStore::Write()!\n"));
	}
	SWLOG( L"  obj: " << lpszValueName << typeid(obj).name( ) );
	return bRes;
}

// .................................................................
BOOL XMLSettingsStore::Write( LPCTSTR lpszValueName, CObject* pObj )
{
	ASSERT_VALID( pObj );
	return Write( lpszValueName, *pObj );
}

// .................................................................
BOOL XMLSettingsStore::Write(LPCTSTR lpszValueName, const CRect& rect)
{
	assert( *sp_currentNode );

	XMLNODE child= sp_currentNode->SetElementArray(lpszValueName, (DWORD*)(LPCRECT)rect, 4 );
	if( ! child )
		return FALSE;
	SWLOG(L"  rect: " << lpszValueName << " t " << rect.top	<< " r " << rect.right << " b " << rect.bottom << " l " << rect.left );
	return TRUE;
}

// .................................................................
BOOL XMLSettingsStore::Write( LPCTSTR lpszValueName, LPBYTE pData, UINT nBytes )
{
	if( m_bReadOnly )
	{
		ASSERT(FALSE);
		return FALSE;
	}
	sp_currentNode->SetElementArray(lpszValueName, pData, nBytes );
	SWLOG( L"  dword: " << lpszValueName << L" l: " << nBytes );
	return TRUE;
}

// .................................................................
BOOL XMLSettingsStore::DeleteValue(LPCTSTR lpszValue)
{
	return m_reg.DeleteValue(lpszValue) == ERROR_SUCCESS;
}

// .................................................................
BOOL XMLSettingsStore::DeleteKey(LPCTSTR lpszPath, BOOL bAdmin)
{
	if( m_bReadOnly )
	{
		return FALSE;
	}

	m_reg.Close( );
	m_reg.m_hKey = bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

	return m_reg.RecurseDeleteKey(PreparePath(lpszPath)) == ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// HEWinApp members
BEGIN_MESSAGE_MAP(HEWinApp, CWinAppEx)
END_MESSAGE_MAP()

HEWinApp::HEWinApp( )
	:mruCount( 0 )
{
	m_bHiColorIcons = TRUE;
}

BOOL HEWinApp::InitInstance()
{
	//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof( InitCtrls );
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);


	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	EnableTaskbarInteraction( FALSE );

	//Command Line Handleing
	// was: LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	/*/////////////////////////////////////////////////////////////////////

	Override to xml instead of regestry on app settings

	C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\include
	afxsettingsstore.h
	C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\src\mfc
	afxsettingsstore.cpp

	XMLSettingsstore simply started as exact copies of the above.

	The settings store is not call upon until the mainframe is created
	But we will initialize it here to set the xml path/file

	this was helpful; The:  CSettingsStoreSP::SetRuntimeClass(RUNTIME_CLASS(CLocalSettingsStore));
	came from there. I voted 5 for the time saver
	http://www.codeproject.com/Articles/33089/Replacing-the-CSettingsStore-Class-in-MFC-to-Store

	/////////////////////////////////////////////////////////////////////*/

	SetRegistryKey( DUMMY_REGISTRY_PATH );

	CSettingsStoreSP::SetRuntimeClass( RUNTIME_CLASS( XMLSettingsStore ) );
	sp_xml_settings_store_global= SP_XML( new CXML );
	sp_currentNode_settings_store_global= SP_XMLNODE( new XMLNODE( *sp_xml_settings_store_global ) );
	strSysRegPath= GetRegSectionPath( );
	CSettingsStoreSP regSP;
	{
		//need to open 'not' read only to create valid m_pRecentFileList
		XMLSettingsStore* pReg = dynamic_cast< XMLSettingsStore* >( &regSP.Create( FALSE, FALSE ) );
		pReg->SetRegPath( GetRegSectionPath( ) );
		ASSERT_VALID( pReg );

		if( ! appdata_path.size( ) )//we will try to load here
		{
			TCHAR szPath[ MAX_PATH ];
			if( SUCCEEDED( SHGetFolderPath(	NULL, CSIDL_APPDATA, NULL, 0, szPath ) ) )
				appdata_path= szPath;
			//else
			//	;//fail? Should never.....
		}
		//in case -u also had a file name
		bfs::path setting_path(appdata_path / USER_SETTINGS_FILENAME);

		if( ! pReg->OpenXML(setting_path.wstring().c_str()))
			;//TODO: don't need it to continue... but warn?

		pReg->SetCurrentNode(pugi::as_wide("/Root/RecentFileList").c_str());
		if( pReg->GetCurrentNode( ) )
			MRU_ReadXML( *pReg->GetCurrentNode( ) );
		else
			;//error
	}

	//these are done in derived.....
	//InitContextMenuManager( );
	//InitKeyboardManager( );
	//InitTooltipManager( );
	//CMFCToolTipInfo ttParams;
	//ttParams.m_bVislManagerTheme = TRUE;
	//GetTooltipManager( )->SetTooltipParams( AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS( CMFCToolTipCtrl ), &ttParams );

	return CWinAppEx::InitInstance( );
}

int HEWinApp::ExitInstance( )
{
	//Save the MRU
	XMLNODE node= sp_xml_settings_store_global->GetNode("/Root/RecentFileList");
	MRU_WriteXML( node );

	//Write user XML
	sp_xml_settings_store_global->Close( );

	AfxOleTerm( FALSE );

	//inhibit write of MRU to registry
	m_pCmdInfo= new CCommandLineInfo;
	m_pCmdInfo->m_nShellCommand= CCommandLineInfo::AppUnregister;

	return CWinAppEx::ExitInstance( );
}

AFX_STATIC_DATA const TCHAR _afxFileSection[] = _T("Recent File List");
AFX_STATIC_DATA const TCHAR _afxFileEntry[] = _T("File%d");
// .......................................................................
void HEWinApp::MRU_ReadXML( XMLNODE& node )
{
	ASSERT( ! m_pRecentFileList );
	mruCount= 4;
	node.GetElement( "maxMRU", mruCount );
	m_pRecentFileList = new CRecentFileList( 0, _afxFileSection, _afxFileEntry, mruCount );

	size_t j = 0;
	for( auto& item : node )
		if ( std::string("projMRU") == item.name())
			//12/21/18 till upgrade of recent file list? check if exist...
			if(bfs::exists(item.first_child().value()))
				m_pRecentFileList->m_arrNames[ j++ ]= item.first_child().value();
}

// .......................................................................
void HEWinApp::MRU_WriteXML( XMLNODE& node )
{
	ASSERT( m_pRecentFileList );
	auto ns = node.select_nodes("//projMRU");
	for (auto& child : ns)
		node.remove_child(child.node());

	node.SetElement( "maxMRU", mruCount );
	for( int iMRU= 0; iMRU < m_pRecentFileList->m_nSize; iMRU++ )
	{
		if( ! m_pRecentFileList->m_arrNames[ iMRU ].IsEmpty( ) )
		{
			XMLNODE child= node.AddChild("projMRU");
			child.append_child(pugi::node_pcdata).set_value(pugi::as_utf8(m_pRecentFileList->m_arrNames[iMRU]).c_str());
		}
	}
}

// .......................................................................
UINT HEWinApp::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	CString strT( _T("/Root/") );
	strT+= lpszSection;
	strT.Replace( _T('\\'), _T('/') );
	CSettingsStoreSP regSP;
	XMLSettingsStore* pReg = dynamic_cast< XMLSettingsStore* >( &regSP.Create( FALSE, TRUE ) );
	XMLNODE node= pReg->sp_xml->GetNode(to_utf8(strT));
	if( ! node )
		return nDefault;
	node.GetElement( lpszEntry, nDefault );
	return nDefault;
}

// .......................................................................
BOOL HEWinApp::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	CString strT( _T("/Root/") );
	strT+= lpszSection;
	strT.Replace( _T('\\'), _T('/') );
	CSettingsStoreSP regSP;
	XMLSettingsStore* pReg = dynamic_cast< XMLSettingsStore* >( &regSP.Create( FALSE, FALSE ) );
	XMLNODE node= pReg->sp_xml->GetNode(to_utf8(strT));
	if( ! node )
		return FALSE;
	return !! node.SetElement( lpszEntry, nValue );
}

// .......................................................................

// .......................................................................
void HEWinApp::PreLoadState( )
{
}

// .......................................................................
void HEWinApp::LoadCustomState( )
{
}

// .......................................................................
void HEWinApp::SaveCustomState( )
{
}

// .......................................................................
void HEWinApp::XMLSaveState( CFrameImpl* pFrame )
{
}

