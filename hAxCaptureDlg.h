// hAxCaptureDlg.h : header file
//

#if !defined(AFX_HAXCAPTUREDLG_H__A55D78BB_0A23_4ECA_A1AC_21A768E5F5F9__INCLUDED_)
#define AFX_HAXCAPTUREDLG_H__A55D78BB_0A23_4ECA_A1AC_21A768E5F5F9__INCLUDED_

#include "SystemTray.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//** START: Some Defines for hAx Capture **//
#define	ID_TIMER_HOTKEY		100
#define	PW_WINDOW			9001
#define	PW_CLIENT			9002
#define	WM_ICON_NOTIFY		WM_APP+10
//** END:	Some Defines for hAx Capture **//

/////////////////////////////////////////////////////////////////////////////
// CHAxCaptureDlg dialog

class CHAxCaptureDlg : public CDialog
{
// Construction
public:
	CSystemTray m_TrayIcon;
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	int m_intImageFormat;
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	HBITMAP CopyWindowToBitmap(CWnd *wnd, HWND hWnd, WORD fPrintArea);
	HBITMAP CopyScreenToBitmap(LPRECT lpRect);
	BOOL Capture();
	int m_intHotKey;
	BOOL m_boolSaveSettings;
	int m_intShotCount;
	CHAxCaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHAxCaptureDlg)
	enum { IDD = IDD_HAXCAPTURE_DIALOG };
	CString	m_strSavePath;
	CString	m_strHotKey;
	int		m_intJPEGQuality;
	CString	m_strJPEGQualityLabel;
	BOOL	m_boolBalloons;
	BOOL	m_boolSounds;
	BOOL	m_boolCloseToTray;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHAxCaptureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHAxCaptureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBrowse();
	afx_msg void OnChangeSavepath();
	afx_msg void OnApplyhotkey();
	afx_msg void OnReleasedcaptureJpegquality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBmp();
	afx_msg void OnJpeg();
	afx_msg void OnGif();
	afx_msg void OnClose();
	afx_msg void OnAbout();
	afx_msg void OnQuit();
	afx_msg void OnAppVisit_hAx();
	afx_msg void OnSounds();
	afx_msg void OnBalloons();
	afx_msg void OnClosetotray();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HAXCAPTUREDLG_H__A55D78BB_0A23_4ECA_A1AC_21A768E5F5F9__INCLUDED_)
