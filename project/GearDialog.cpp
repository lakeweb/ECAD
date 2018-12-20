
#include "stdafx.h"

#include "gear_gen.h"
#include "Resource.h"
#include <ctrlext.h>
#include "shared.h"
#include "MainFrm.h"
#include "GearDialog.h"

// ................................................................
CGearsDialog::CGearsDialog( )
	: valGearPitch( 8 )
	, valToothCount( 12 )
	, valPitchAngle( 20 * PI / 180 )
	, valCenterBore( .25 )
	, bInInit( true )
{
}

// ................................................................
CGearsDialog::~CGearsDialog( )
{
}

// ................................................................
BEGIN_MESSAGE_MAP( CGearsDialog, CPaneDialog )
	ON_WM_CREATE( )
	ON_WM_SIZE( )
	ON_CBN_SELCHANGE( IDC_GEAR_PTICH_ANG, &CGearsDialog::OnCbnSelchangeGearPtichAng )
	ON_EN_CHANGE( IDC_GEAR_TOOTH_CNT, &CGearsDialog::OnCbnSelchangeGearPtichAng )
	ON_EN_CHANGE( IDC_GEAR_PITCH, &CGearsDialog::OnCbnSelchangeGearPtichAng )
	ON_EN_CHANGE( IDC_GEAR_BORE, &CGearsDialog::OnCbnSelchangeGearPtichAng )
	ON_MESSAGE( WM_INITDIALOG, HandleInitDialog )
END_MESSAGE_MAP( )

// ................................................................
void CGearsDialog::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_GEAR_PTICH_ANG, cGeardlgPitchCombo );
	DDX_Control( pDX, IDC_STATIC_INFO_GEAR, cGeardlgInfo );
	DDX_Control( pDX, IDC_GEAR_BORE, cGeardlgCenterBoreEdit );

	DDX_Text( pDX, IDC_GEAR_PITCH, valGearPitch );
	DDX_Text( pDX, IDC_GEAR_TOOTH_CNT, valToothCount );
	DDX_Text( pDX, IDC_GEAR_BORE, valCenterBore );
}

// ................................................................
LRESULT CGearsDialog::HandleInitDialog( WPARAM wParam, LPARAM lParam )
{
	CPaneDialog::HandleInitDialog( wParam, lParam );
	cGeardlgPitchCombo.InsertString( 0, _T("14.5") );
	cGeardlgPitchCombo.InsertString( 1, _T("20.0") );
	cGeardlgPitchCombo.SetCurSel( 0 );
	OnCbnSelchangeGearPtichAng( );
	CString strT;
	strT.Format( _T("%2.2f"), valCenterBore );
	cGeardlgCenterBoreEdit.SetWindowText( strT );
	bInInit= false;
	OnCbnSelchangeGearPtichAng( );
	return 0;
}

// ................................................................
int CGearsDialog::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CPaneDialog::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	return 0;
}

// ................................................................
void CGearsDialog::OnSize( UINT nType, int cx, int cy )
{
	CPaneDialog::OnSize( nType, cx, cy );
	AdjustLayout( );
}

// ................................................................
void CGearsDialog::OnCbnSelchangeGearPtichAng( )
{
	if( bInInit )
		return;

	sp_gear= boost::make_shared< gear_params_t >( 18, 14.5, 12, .25 );

	static NMHDROBJECT nm;
	nm.nmHdr.hwndFrom= *this;
	nm.nmHdr.code= ID_GEAR_PARAM_CHANGED;
	nm.nmHdr.idFrom= 0;
	CDataExchange DX( this, TRUE );
	DoDataExchange( &DX );
	CString strT;
	cGeardlgPitchCombo.GetLBText( cGeardlgPitchCombo.GetCurSel( ), strT );
	double pitch= _wtof( strT );
	cGeardlgCenterBoreEdit.GetWindowText( strT );
	double db= _wtof( strT );
	
	sp_gear= boost::make_shared< gear_params_t >( valToothCount, TO_RAD( pitch ), valGearPitch, db );
	nm.pObject= sp_gear.get( );

	//the gear document will handle this message
	::PostMessage( *AfxGetMainWnd( ), WM_NOTIFY, ID_GEAR_PARAM_CHANGED, (LPARAM)&nm );

	strT.Format( _T("Pitch Diameter: %2.2f\r\ntest"), sp_gear->pd );
	cGeardlgInfo.SetWindowText( strT );
}

// ................................................................
//TODO delete....
void CGearsDialog::OnEnChangeGearToothCnt( )
{
}


void CGearsDialog::OnEnChangeGearPitch( )
{
}

void CGearsDialog::OnEnChangeGearBore( )
{
}
