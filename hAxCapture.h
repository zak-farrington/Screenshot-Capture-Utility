// hAxCapture.h : main header file for the HAXCAPTURE application
//

#if !defined(AFX_HAXCAPTURE_H__A027039C_2466_454F_86AE_AE1BD2DF9B2B__INCLUDED_)
#define AFX_HAXCAPTURE_H__A027039C_2466_454F_86AE_AE1BD2DF9B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHAxCaptureApp:
// See hAxCapture.cpp for the implementation of this class
//

class CHAxCaptureApp : public CWinApp
{
public:
	CHAxCaptureApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHAxCaptureApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHAxCaptureApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HAXCAPTURE_H__A027039C_2466_454F_86AE_AE1BD2DF9B2B__INCLUDED_)
