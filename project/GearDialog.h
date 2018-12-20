#pragma once


class CGearsDialog : public CPaneDialog
{
public:
	enum { IDD = IDS_GEAR_DLG };
	CGearsDialog( );
	virtual ~CGearsDialog( );

	CComboBox cGeardlgPitchCombo;
	CStatic cGeardlgInfo;
	CEdit	cGeardlgCenterBoreEdit;

	bool bInInit;

	int valGearPitch;
	int valToothCount;
	double valPitchAngle;
	double valCenterBore;

	SP_gear_params_t sp_gear;

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

protected:
DECLARE_MESSAGE_MAP()
	afx_msg LRESULT HandleInitDialog( WPARAM wParam, LPARAM lParam );
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnCbnSelchangeGearPtichAng( );
	afx_msg void OnEnChangeGearToothCnt( );
	afx_msg void OnEnChangeGearPitch( );
	afx_msg void OnEnChangeGearBore( );
};
