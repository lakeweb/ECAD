
//#include <Windows.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <afxrich.h>        // MFC Rich Edit Control used in most.
#include <Richedit.h>
#include <map>

#define USE_WINDOWS_EX
#include <globals.h>

// ...........................................................................
#ifdef _DEBUG
std::ofstream bug_os("bug_os.txt");
extern std::ofstream testos;// if used in other 
#define BUG_OS( x ) {  bug_os << x; }
#else
#define BUG_OS( x ) __noop;
#endif


// ............................................................................
DWORD CALLBACK EditStreamCallBack(
		DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb )
{
	_afxRichEditStreamCookie* pCookie = (_afxRichEditStreamCookie*)dwCookie;
	CArchive& ar = pCookie->m_ar;
	DWORD dw = 0;
	*pcb = cb;
	TRY
	{
		if ( ar.IsStoring( ) )
			ar.GetFile( )->Write( pbBuff, cb );
		else
			*pcb = ar.GetFile( )->Read( pbBuff, cb );
	}
	CATCH( CFileException, e )
	{
		*pcb = 0;
		pCookie->m_dwError = (DWORD)e->m_cause;
		dw = 1;
		e->Delete( );
	}
	AND_CATCH_ALL( e )
	{
		*pcb = 0;
		pCookie->m_dwError = -1;
		dw = 1;
		e->Delete( );
	}
	END_CATCH_ALL
	return dw;
}

// ............................................................................
BOOL SetRichText( CRichEditCtrl& re, char* buf, int format )//| SFF_SELECTION
{
	CMemFile oFile( (BYTE*)buf, (UINT)strlen( buf ) );
 	CArchive oar( &oFile, CArchive::load );

	EDITSTREAM es= {0, 0, EditStreamCallBack};
	_afxRichEditStreamCookie cookie( oar );
	es.dwCookie = (DWORD_PTR)&cookie;

	oFile.SeekToBegin( );
	re.StreamIn( format, es ); //| SFF_SELECTION
	if( cookie.m_dwError != 0 )
	{
		AfxThrowFileException( cookie.m_dwError );
		return FALSE;
	}

	oar.Close( );
	oFile.Close( );
	return TRUE;
}

// ............................................................................
// will depreciate as every thing should go through boost::filesystem (bfs)
BOOL FileExceptionMessageBox( CFileException* exception, LPCTSTR addInfo )
{
	CString message;
	if( addInfo != NULL )
	{
		message= addInfo;
		message+= _T("\n");
	}
	switch( exception->m_cause )
	{
	case CFileException::none:
		return TRUE;
	case CFileException::fileNotFound:
		message+= _T("File Not Found\n");
		break;
	case CFileException::badPath:
		message+= _T("Improper Filename Path\n");
		break;
	case CFileException::accessDenied:
		message+= _T("Access Denied or Read Only\n");
		break;
	case CFileException::sharingViolation:
		message+= _T("Sharing Violation\n");
		break;
	default:
		message+= _T("Unknown Error???\n");

	}
	message+= _T("Filename: ");
	if( !exception->m_strFileName.IsEmpty( ) )
		message+= exception->m_strFileName;
	else
		message+= _T("filename is null!");
	MessageBox( AfxGetMainWnd( )->GetSafeHwnd( ), message, _T("FileException Handler"), MB_OK | MB_ICONEXCLAMATION );
	return FALSE;
}

