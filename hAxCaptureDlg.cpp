// hAxCaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hAxCapture.h"
#include "hAxCaptureDlg.h"
#include "Registry.h"
#include "DIBSectionLite.h"
#include "SystemTray.h"

//** <3 GDI! =P
#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	#define     DISPLAY_TIMER_ID	150		//** Credits Timer ID

 	RECT        m_ScrollRect, r;			//** rect of Static Text frame
	int         nArrIndex, nCounter;		//** Work ints
	CString     m_szWork;					//** Holds display line
	BOOL        m_bFirstTime;
	BOOL        m_bDrawText;
	int         nClip;
	int         nCurrentFontHeight;

	CWnd*       m_pDisplayFrame;

	CBitmap     m_bmpWork;                  //** Bitmap holder
	CBitmap* 	pBmpOld;                    //** Other bitmap work members
	CBitmap*    m_bmpCurrent;
	HBITMAP 	m_hBmpOld;

	CSize 		m_size;                     //** Drawing helpers
	CPoint 		m_pt;
	BITMAP 		m_bmpInfo;
	CDC 		m_dcMem;
	BOOL 		m_bProcessingBitmap;

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnVisithax();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_VISITHAX, OnVisithax)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHAxCaptureDlg dialog

CHAxCaptureDlg::CHAxCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHAxCaptureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHAxCaptureDlg)
	m_strSavePath = _T("");
	m_strHotKey = _T("");
	m_intJPEGQuality = 0;
	m_strJPEGQualityLabel = _T("");
	m_boolBalloons = FALSE;
	m_boolSounds = FALSE;
	m_boolCloseToTray = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHAxCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHAxCaptureDlg)
	DDX_Text(pDX, IDC_SAVEPATH, m_strSavePath);
	DDX_CBString(pDX, IDC_HOTKEY, m_strHotKey);
	DDX_Slider(pDX, IDC_JPEGQUALITY, m_intJPEGQuality);
	DDX_Text(pDX, IDC_JPEGQUALITYLABEL, m_strJPEGQualityLabel);
	DDX_Check(pDX, IDC_BALLOONS, m_boolBalloons);
	DDX_Check(pDX, IDC_SOUNDS, m_boolSounds);
	DDX_Check(pDX, IDC_CLOSETOTRAY, m_boolCloseToTray);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHAxCaptureDlg, CDialog)
	//{{AFX_MSG_MAP(CHAxCaptureDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_SAVEPATH, OnChangeSavepath)
	ON_BN_CLICKED(IDC_APPLYHOTKEY, OnApplyhotkey)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_JPEGQUALITY, OnReleasedcaptureJpegquality)
	ON_BN_CLICKED(IDC_BMP, OnBmp)
	ON_BN_CLICKED(IDC_JPEG, OnJpeg)
	ON_BN_CLICKED(IDC_GIF, OnGif)
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(ID_APP_QUIT, OnQuit)
	ON_COMMAND(ID_APP_VISITHAXSTUDIOS, OnAppVisit_hAx)
	ON_BN_CLICKED(IDC_SOUNDS, OnSounds)
	ON_BN_CLICKED(IDC_BALLOONS, OnBalloons)
	ON_BN_CLICKED(IDC_CLOSETOTRAY, OnClosetotray)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_BN_CLICKED(IDC_QUIT, OnQuit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHAxCaptureDlg message handlers

BOOL CHAxCaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//** Let's get rid MFC's SetIcon crap and use this
	SendMessage(WM_SETICON, IMAGE_ICON, (LPARAM)LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)));
	

	// TODO: Add extra initialization here

	
	CRegistry Registry;											//** Gotta love this class
	CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";	//** The path to our keys

	if(Registry.VerifyKey(HKEY_CURRENT_USER, Path) == NULL)		//** If our key does not exist, lets make it
	{

		DWORD mBox = NULL;

		{mBox = MessageBox("We cannot find the registry keys for this program.  If you would like to save settings for this application "
			 "you need to create it.  Also, if you don't create these keys, the program may result in error. Create keys??\n\n(Note: Creating these key will not harm your computer in any way.)", "hAx Capture :: Information", MB_YESNO + MB_ICONINFORMATION);
			
		switch (mBox)
		{
			case IDYES:
				//** User wants to create the keys
				if(Registry.CreateKey(HKEY_CURRENT_USER, Path)) //** If key was created, let the user know
				{
					MessageBox("Key(s) have been successfully created.\r\n\r\nNow select where you would like your screen shots to save by default.  (You can change this later.)", "hAx Capture:: Information", MB_OK + MB_ICONINFORMATION);
					m_boolSaveSettings = TRUE;

					m_intHotKey = VK_F12;
					m_strHotKey.Format("F12");
					Registry.Write("HotKey", m_intHotKey);

					m_intShotCount = 0;
					Registry.Write("ShotCount", m_intShotCount);

					m_intImageFormat = IDC_JPEG;
					Registry.Write("ImageFormat", m_intImageFormat);

					CheckRadioButton(IDC_BMP, IDC_GIF, IDC_JPEG);	//** Check JPEG image format radio button
					
					m_intJPEGQuality = 70;
					Registry.Write("JPEGQuality", m_intJPEGQuality);

					m_strJPEGQualityLabel.Format("JPEG Quality: %d%%", m_intJPEGQuality);
					
					//** Enable the JPEG Quality slider and label
					GetDlgItem(IDC_JPEGQUALITY)->EnableWindow(TRUE);
					GetDlgItem(IDC_JPEGQUALITYLABEL)->EnableWindow(TRUE);

					Registry.Write("Sounds", 1);
					m_boolSounds = TRUE;

					Registry.Write("Balloons", 1);
					m_boolBalloons = TRUE;

					Registry.Write("CloseToTray", 1);
					m_boolCloseToTray = TRUE;

					

					m_strSavePath.Format("C:\\");
					OnBrowse();	//** Pop up the browse for folder dialog
				}
			break;

			default:
				MessageBox("Key(s) have NOT been created.\r\n\r\nHot key will be F12.  Screen count will begin at 1, and screens will automatically overwrite.  Settings will not be saved.\r\n\r\nNow select where you would like your screen shots to save by default.  (You can change this later.) ","hAx Capture :: Information", MB_OK + MB_ICONINFORMATION);
				m_boolSaveSettings = FALSE;
				m_intShotCount = 0;
				m_intHotKey = VK_F12;
				m_strHotKey.Format("F12");
				m_strSavePath.Format("C:\\");
				m_intImageFormat = IDC_JPEG;
				CheckRadioButton(IDC_BMP, IDC_GIF, IDC_JPEG);
				m_intJPEGQuality = 70;
				m_strJPEGQualityLabel.Format("JPEG Quality: %d%%", m_intJPEGQuality);
				//** Enable the JPEG Quality slider and label
				GetDlgItem(IDC_JPEGQUALITY)->EnableWindow(TRUE);
				GetDlgItem(IDC_JPEGQUALITYLABEL)->EnableWindow(TRUE);

				m_boolSounds = TRUE;
				m_boolBalloons = TRUE;
				m_boolCloseToTray = TRUE;

				m_strSavePath.Format("C:\\");
				OnBrowse(); //** Pop up the browse for folder dialog

			break;
		};
		}
	}
	else //** Keys already exist
	{
		CString WeCanUseThis;													//** Just a buffer with a funneh name

		m_boolSaveSettings = TRUE;
		Registry.Read("SavePath", m_strSavePath);								//** Read save path from Registry
		Registry.Read("ShotCount", m_intShotCount);								//** Read shot count from Registry
		WeCanUseThis.Format("[:: hAx Capture ::] [:: %d screens captured ::]", m_intShotCount);
		SetWindowText(WeCanUseThis);
		
		Registry.Read("ImageFormat", m_intImageFormat);

		CheckRadioButton(IDC_BMP, IDC_GIF, m_intImageFormat);					//** Check the appropriate Image Format Radio button

		if(m_intImageFormat == IDC_JPEG)										//** If saved setting for Image Format is JPEG
		{
			//** Enable the JPEG Quality slider and label
			GetDlgItem(IDC_JPEGQUALITY)->EnableWindow(TRUE);
			GetDlgItem(IDC_JPEGQUALITYLABEL)->EnableWindow(TRUE);
		}
		
		Registry.Read("JPEGQuality", m_intJPEGQuality);							//** Read JPEG Quality from Register
		m_strJPEGQualityLabel.Format("JPEG Quality: %d%%", m_intJPEGQuality);	//** Set value for JPEG Quality label

		Registry.Read("Sounds", m_boolSounds);
		Registry.Read("Balloons", m_boolBalloons);
		Registry.Read("CloseToTray", m_boolCloseToTray);

		Registry.Read("HotKey", m_intHotKey);									//** Read Hotkey from Registry

		switch(m_intHotKey)
		{
			case(VK_F1):
				WeCanUseThis.Format("F1");
			break;
			
			case(VK_F2):
				WeCanUseThis.Format("F2");
			break;

			case(VK_F3):
				WeCanUseThis.Format("F3");
			break;

			case(VK_F4):
				WeCanUseThis.Format("F4");
			break;
			
			case(VK_F5):
				WeCanUseThis.Format("F5");
			break;

			case(VK_F6):
				WeCanUseThis.Format("F6");
			break;

			case(VK_F7):
				WeCanUseThis.Format("F7");
			break;
			
			case(VK_F8):
				WeCanUseThis.Format("F8");
			break;

			case(VK_F9):
				WeCanUseThis.Format("F9");
			break;
					
			case(VK_F10):
				WeCanUseThis.Format("F10");
			break;
			
			case(VK_F11):
				WeCanUseThis.Format("F11");
			break;

			case(VK_F12):
				WeCanUseThis.Format("F12");
			break;

			case(VK_PAUSE):
				WeCanUseThis.Format("Pause/Break");
			break;
			
			case(VK_INSERT):
				WeCanUseThis.Format("Insert");
			break;

			case(VK_DELETE):
				WeCanUseThis.Format("Delete");
			break;
			
			case(VK_HOME):
				WeCanUseThis.Format("Home");
			break;
			
			case(VK_END):
				WeCanUseThis.Format("End");
			break;

			default:
				WeCanUseThis.Format("F12");
			break;
		}
		m_strHotKey.Format(WeCanUseThis);
	}

	Registry.Close();


	SetTimer(ID_TIMER_HOTKEY, 300, NULL);	//** Set a timer that will check the key state of our hotkey every 300 milliseconds
	
	UpdateData(FALSE);



	m_TrayIcon.Create(this, WM_ICON_NOTIFY, "[:: hAx Capture ::]", LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_TRAY)), IDR_POPUP_MENU);
	m_TrayIcon.ShowBalloon("Thank you for using hAx Capture!", "Welcome");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHAxCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHAxCaptureDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}	
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHAxCaptureDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHAxCaptureDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == ID_TIMER_HOTKEY)
	{
		if(GetAsyncKeyState(m_intHotKey)) //** m_intHotKey is our variable to hold the hotkey the user wants to use, since its user defined
		{
			Capture();
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

LRESULT CHAxCaptureDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if(m_boolCloseToTray)
	{
		switch(lParam)
		{
			case WM_LBUTTONDOWN:
				if(IsWindowVisible() == FALSE)
				{
					if(m_boolSounds)
					{
						PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
					}
					CSystemTray::MaximiseFromTray(this, TRUE);
				}
			break;
		}
	}

	return m_TrayIcon.OnTrayNotification(wParam, lParam);
}

void CHAxCaptureDlg::OnClose() 
{
	if(m_boolCloseToTray)
	{
		if(m_boolBalloons)
		{
			m_TrayIcon.ShowBalloon("hAx Capture has been sent to the system tray.", "hAx Capture :: Notification");
		}

		CSystemTray::MinimiseToTray(this, TRUE);
		ShowWindow(SW_HIDE);
	}
	else
	{
		if(m_boolBalloons)
		{
			m_TrayIcon.ShowBalloon("Thank you for using hAx Capture!", "Good bye!"); //** Show nice balloon popup

			Sleep(2000); //** Give some time for the user to read the balloon window
		}

		KillTimer(ID_TIMER_HOTKEY);
		CDialog::OnClose();
	}
}

void CHAxCaptureDlg::OnAppVisit_hAx() 
{	
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}
	
	//** code to launch hAx Studios Ltd. in the DEFAULT browser
	char lpBrowser[512];
	char holder[512];
	char addytype[512];
	DWORD dwLen = 500;
	DWORD dwType;
	HKEY hKey;
	LPSTR lpPathValue;
	ZeroMemory(lpBrowser, 512);
	
	ZeroMemory(holder, 512);
	strncpy(holder, "http://hax-studios.net", 4);

	lpPathValue = (LPSTR)GlobalAlloc(GMEM_FIXED, 500);
	
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,addytype, 0,KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey,"",0,&dwType,(BYTE *)lpPathValue, &dwLen) ==  ERROR_SUCCESS)
		{
			ZeroMemory(holder, 512);
			strcpy(holder, lpPathValue+1);
			strncpy(lpBrowser, holder, strrchr(holder, '\"') - holder);
		}
		else
		{
			strcpy(lpBrowser, "iexplore");
		}
	}
	else
	{
		strcpy(lpBrowser, "iexplore");
	}

	RegCloseKey(hKey);
	GlobalFree(lpPathValue);

	ShellExecute(NULL, "open", lpBrowser, "http://hax-studios.net", NULL, SW_SHOWMAXIMIZED);
}

void CHAxCaptureDlg::OnAbout() 
{
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}

	CAboutDlg aboutDlg;

	aboutDlg.DoModal();
}

void CHAxCaptureDlg::OnQuit() 
{
	if(m_boolBalloons)
	{
		m_TrayIcon.ShowBalloon("Thank you for using hAx Capture!", "Good bye!"); //** Show nice balloon popup

		Sleep(2000); //** Give some time for the user to read the balloon window
	}

	KillTimer(ID_TIMER_HOTKEY);

	EndDialog(0);
}

void CHAxCaptureDlg::OnBrowse() 
{
	CString szDirectory;
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC); //** Play a elite sound
	}
	LPITEMIDLIST lpItemList;
	BROWSEINFO browseInfo;
	ZeroMemory((PVOID)&browseInfo, sizeof(BROWSEINFO));
	
	browseInfo.hwndOwner = GetWindow(NULL)->m_hWnd;
	browseInfo.lpszTitle = "Please select the folder where the screen shots will save to.";

	if((lpItemList = ::SHBrowseForFolder(&browseInfo)) != NULL)
	{
		if(::SHGetPathFromIDList(lpItemList,szDirectory.GetBuffer(MAX_PATH)))
		{
			m_strSavePath.Format(szDirectory);
			UpdateData(FALSE);
			
			if(m_boolSaveSettings == TRUE)
			{
				CRegistry Registry;
				CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

				Registry.Open(HKEY_CURRENT_USER, Path);

				if(Registry.Write("SavePath", m_strSavePath) == NULL)
				{
					MessageBox("Error: Could not write m_strSavePath to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
				}

				Registry.Close();
			}

			if(m_boolSounds)
			{
				PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
			}
		}
	}	
}

void CHAxCaptureDlg::OnChangeSavepath() 
{
	UpdateData(TRUE);
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}

	if(m_boolSaveSettings == TRUE)
	{

		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("SavePath", m_strSavePath) == NULL)
		{
			MessageBox("Error: Could not write m_strSavePath to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}

		Registry.Close();
	}	
}

void CHAxCaptureDlg::OnApplyhotkey() 
{
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}
	UpdateData(TRUE);

	if(stricmp(m_strHotKey, "F1") == 0)
	{
		m_intHotKey = VK_F1;
	}
	else if(stricmp(m_strHotKey, "F2") == 0)
	{
		m_intHotKey = VK_F2;
	}
	else if(stricmp(m_strHotKey, "F3") == 0)
	{
		m_intHotKey = VK_F3;
	}
	else if(stricmp(m_strHotKey, "F4") == 0)
	{
		m_intHotKey = VK_F4;
	}
	else if(stricmp(m_strHotKey, "F5") == 0)
	{
		m_intHotKey = VK_F5;
	}
	else if(stricmp(m_strHotKey, "F6") == 0)
	{
		m_intHotKey = VK_F6;
	}
	else if(stricmp(m_strHotKey, "F7") == 0)
	{
		m_intHotKey = VK_F7;
	}
	else if(stricmp(m_strHotKey, "F8") == 0)
	{
		m_intHotKey = VK_F8;
	}
	else if(stricmp(m_strHotKey, "F9") == 0)
	{
		m_intHotKey = VK_F9;
	}
	else if(stricmp(m_strHotKey, "F10") == 0)
	{
		m_intHotKey = VK_F10;
	}
	else if(stricmp(m_strHotKey, "F11") == 0)
	{
		m_intHotKey = VK_F11;
	}
	else if(stricmp(m_strHotKey, "F12") == 0)
	{
		m_intHotKey = VK_F12;
	}
	else if(stricmp(m_strHotKey, "Pause/Break") == 0)
	{
		m_intHotKey = VK_PAUSE;
	}
	else if(stricmp(m_strHotKey, "Insert") == 0)
	{
		m_intHotKey = VK_INSERT;
	}
	else if(stricmp(m_strHotKey, "Delete") == 0)
	{
		m_intHotKey = VK_DELETE;
	}
	else if(stricmp(m_strHotKey, "Home") == 0)
	{
		m_intHotKey = VK_HOME;
	}
	else if(stricmp(m_strHotKey, "End") == 0)
	{
		m_intHotKey = VK_END;
	}
	else
	{
		m_intHotKey = VK_F12;
	}

	if(m_boolSaveSettings == TRUE)
	{
		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("HotKey", m_intHotKey) == NULL)
		{
			MessageBox("Error: Could not write m_intHotKey to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}

		Registry.Close();
	}	
}

//** This event checks when our JPEG Quality Slider has been modified
void CHAxCaptureDlg::OnReleasedcaptureJpegquality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);

	m_strJPEGQualityLabel.Format("JPEG Quality: %d%%", m_intJPEGQuality);

	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}

	UpdateData(FALSE);

	//** Save JPEG Quality Settings
	if(m_boolSaveSettings == TRUE)
	{
		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("JPEGQuality", m_intJPEGQuality) == NULL)
		{
			MessageBox("Error: Could not write m_intJPEGQuality to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}

		Registry.Close();
	}

	*pResult = 0;
}

void CHAxCaptureDlg::OnBmp() 
{
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}

	//** Disable JPEG Quality Slider and Label since the user is NOT using JPEG as the image format
	GetDlgItem(IDC_JPEGQUALITY)->EnableWindow(FALSE);
	GetDlgItem(IDC_JPEGQUALITYLABEL)->EnableWindow(FALSE);

	//** Save Image Format
	if(m_boolSaveSettings == TRUE)
	{
		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("ImageFormat", IDC_BMP) == NULL)
		{
			MessageBox("Error: Could not write m_intImageFormat to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}

		Registry.Close();
	}
}

void CHAxCaptureDlg::OnJpeg() 
{
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}
	//** Enable Quality Slider and Label since the user is using JPEG as the image format
	GetDlgItem(IDC_JPEGQUALITY)->EnableWindow(TRUE);
	GetDlgItem(IDC_JPEGQUALITYLABEL)->EnableWindow(TRUE);

	//** Save Image Format
	if(m_boolSaveSettings == TRUE)
	{
		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("ImageFormat", IDC_JPEG) == NULL)
		{
			MessageBox("Error: Could not write m_intImageFormat to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}

		Registry.Close();
	}
}

void CHAxCaptureDlg::OnGif() 
{
	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}
	//** Disable JPEG Quality Slider and Label since the user is NOT using JPEG as the image format
	GetDlgItem(IDC_JPEGQUALITY)->EnableWindow(FALSE);
	GetDlgItem(IDC_JPEGQUALITYLABEL)->EnableWindow(FALSE);

	//** Save Image Format
	if(m_boolSaveSettings == TRUE)
	{
		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("ImageFormat", IDC_GIF) == NULL)
		{
			MessageBox("Error: Could not write m_intImageFormat to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}

		Registry.Close();
	}	
}

void CHAxCaptureDlg::OnSounds() 
{
	UpdateData(TRUE);

	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}

	if(m_boolSaveSettings == TRUE)
	{
		if(m_boolSounds == TRUE)
		{
			CRegistry Registry;
			CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

			Registry.Open(HKEY_CURRENT_USER, Path);

			if(Registry.Write("Sounds", 1) == NULL)
			{
				MessageBox("Error: Could not write m_boolSounds to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			}
	
			Registry.Close();
		}
		else
		{
			CRegistry Registry;
			CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

			Registry.Open(HKEY_CURRENT_USER, Path);

			if(Registry.Write("Sounds", 0) == NULL)
			{
				MessageBox("Error: Could not write m_boolSounds to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			}
	
			Registry.Close();
		}
	}
}

void CHAxCaptureDlg::OnBalloons() 
{
	UpdateData(TRUE);

	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}
	
	if(m_boolSaveSettings == TRUE)
	{
		if(m_boolBalloons == TRUE)
		{
			CRegistry Registry;
			CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

			Registry.Open(HKEY_CURRENT_USER, Path);

			if(Registry.Write("Balloons", 1) == NULL)
			{
				MessageBox("Error: Could not write m_boolBallons to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			}
	
			Registry.Close();
		}
		else if(m_boolBalloons == FALSE)
		{
			CRegistry Registry;
			CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

			Registry.Open(HKEY_CURRENT_USER, Path);

			if(Registry.Write("Balloons", 0) == NULL)
			{
				MessageBox("Error: Could not write m_boolBallons to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			}
	
			Registry.Close();
		}
	}
}

void CHAxCaptureDlg::OnClosetotray() 
{
	UpdateData(TRUE);

	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	}
	
	if(m_boolSaveSettings == TRUE)
	{
		if(m_boolCloseToTray == TRUE)
		{
			CRegistry Registry;
			CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

			Registry.Open(HKEY_CURRENT_USER, Path);

			if(Registry.Write("CloseToTray", 1) == NULL)
			{
				MessageBox("Error: Could not write m_boolCloseToTray to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			}
	
			Registry.Close();
		}
		else
		{
			CRegistry Registry;
			CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

			Registry.Open(HKEY_CURRENT_USER, Path);

			if(Registry.Write("CloseToTray", 0) == NULL)
			{
				MessageBox("Error: Could not write m_boolCloseToTray to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			}
	
			Registry.Close();
		}
	}	
}


int CHAxCaptureDlg::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT  num = 0;          //** number of image encoders
	UINT  size = 0;         //** size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);

	if(size == 0)
	{
		return -1;  //** Failure
	}

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	
	if(pImageCodecInfo == NULL)
	{
		return -1;  //** Failure
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  //** Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  //** Failure
}

BOOL CHAxCaptureDlg::Capture()
{
	UpdateData(TRUE);
	m_intShotCount++;

	if(m_strSavePath.IsEmpty())
	{
		MessageBox("Error: You need to select the folder destination for your screen shots!", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		return FALSE;
	}

	CreateDirectory(m_strSavePath, NULL);

	if(m_boolSaveSettings == TRUE)
	{
		CRegistry Registry;
		CString Path = "Software\\hAx Studios Ltd.\\hAx Capture";

		Registry.Open(HKEY_CURRENT_USER, Path);

		if(Registry.Write("ShotCount", m_intShotCount) == NULL)
		{
			MessageBox("Error: Could not write m_intShotCount to the registry.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
			return FALSE;
		}

		CString WindowTitle;

		WindowTitle.Format("[:: hAx Capture ::] [:: %d screens captured ::]", m_intShotCount);
		SetWindowText(WindowTitle);

		Registry.Close();
	}

	
	HBITMAP hBitmap;
	CDIBSectionLite dib;
	CString m_strSaveScreen;

	hBitmap = CopyWindowToBitmap(this, GetDesktopWindow()->m_hWnd, PW_WINDOW);

	dib.SetBitmap(hBitmap);
	
	if(IsDlgButtonChecked(IDC_JPEG) != 0)		//** If user wants to save as JPEG
	{
		
		dib.Save("screen.tmp");	//** Save temporary bitmap which we will convert via GDI+

		//** Initialize GDI+.
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		CLSID             encoderClsid;
		EncoderParameters encoderParameters;
		ULONG             quality;
		Status            stat;

		//** Get an image from the disk, in this case since we've already saved a BMP, use it
		Image* image = new Image(L"screen.tmp");

		//** Get the CLSID of the systems JPEG encoder
		GetEncoderClsid(L"image/jpeg", &encoderClsid);

		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;


		//** Save the image as a JPEG with quality level 100... need to make this user defined later
		quality = m_intJPEGQuality;
		encoderParameters.Parameter[0].Value = &quality;

		//** The below is very ghetto... but you know me...
		//** self taught C++ leaves me with so many holes in my knowledge D: ;-;
		if(m_intShotCount < 10)
		{
			m_strSaveScreen.Format(m_strSavePath + "\\%s00%d.jpg", "screenShot", m_intShotCount);
		}
		else if(m_intShotCount > 10 && m_intShotCount < 100)
		{
			m_strSaveScreen.Format(m_strSavePath + "\\%s0%d.jpg", "screenShot", m_intShotCount);
		}
		else
		{
			m_strSaveScreen.Format(m_strSavePath + "\\%s%d.jpg", "screenShot", m_intShotCount);
		}

		//** Some LPCSTR to LPCWSTR conversion
		WCHAR tmp_strSaveScreen[256];
		MultiByteToWideChar(CP_ACP, 0, m_strSaveScreen, -1, tmp_strSaveScreen, 256);

		stat = image->Save(tmp_strSaveScreen, &encoderClsid, &encoderParameters);

		if(stat != Ok)
		{
			MessageBox("Error: HBITMAP to JPEG conversion via GDI+ has failed!", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}
		
		delete image;
		GdiplusShutdown(gdiplusToken);

		if(DeleteFile("screen.tmp") == FALSE)
		{
			MessageBox("Error: DeleteFile(); has failed.  Temp screen \"screen.tmp\" has NOT been deleted.", "hAx Capture :: Error", MB_OK + MB_ICONERROR);
		}
	}
	else	//** User wants BMP... (NO GIF SUPPORT YET!)
	{

		if(m_intShotCount < 10)
		{
			m_strSaveScreen.Format(m_strSavePath + "\\%s00%d.bmp", "screenShot", m_intShotCount);
		}
		else if(m_intShotCount > 10 && m_intShotCount < 100)
		{
			m_strSaveScreen.Format(m_strSavePath + "\\%s0%d.bmp", "screenShot", m_intShotCount);
		}
		else
		{
			m_strSaveScreen.Format(m_strSavePath + "\\%s%d.bmp", "screenShot", m_intShotCount);
		}

		dib.Save(m_strSaveScreen);
	}


	dib.DeleteObject(); //** We don't need it anymore

	if(m_boolSounds)
	{
		PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC); //** Play a elite sound
	}
	return TRUE;
}

HBITMAP CHAxCaptureDlg::CopyScreenToBitmap(LPRECT lpRect)
{
	HDC         hScrDC, hMemDC;         //** screen DC and memory DC     
	int         nX, nY, nX2, nY2;       //** coordinates of rectangle to grab     
	int         nWidth, nHeight;        //** DIB width and height     
	int         xScrn, yScrn;           //** screen resolution      

	HGDIOBJ     hOldBitmap , hBitmap;
		
	//** Ccheck for an empty rectangle 
    if(IsRectEmpty(lpRect))
	{
		return NULL;
	}

	//** create a DC for the screen and create     
	//** a memory DC compatible to screen DC          

	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);     
	hMemDC = CreateCompatibleDC(hScrDC);	//** get points of rectangle to grab  
   
	nX = lpRect->left;     
	nY = lpRect->top;     
	nX2 = lpRect->right;     
	nY2 = lpRect->bottom;					//** get screen resolution      
   
	xScrn = GetDeviceCaps(hScrDC, HORZRES);     
	yScrn = GetDeviceCaps(hScrDC, VERTRES);      
   
	//make sure bitmap rectangle is visible      
   
	if(nX < 0)         
		nX = 0;     
   
	if(nY < 0)         
		nY = 0;     
   
	if(nX2 > xScrn)         
		nX2 = xScrn;     
   
	if(nY2 > yScrn)         
		nY2 = yScrn;      

	nWidth = nX2 - nX;     
	nHeight = nY2 - nY;      
   
	//** create a bitmap compatible with the screen DC     
   
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      
   
	//** select new bitmap into memory DC     
   
	hOldBitmap =   SelectObject (hMemDC, hBitmap);      
   
	//** bitblt screen DC to memory DC     
   
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);     
   
	//** select old bitmap back into memory DC and get handle to     
	//** bitmap of the screen          
   
	hBitmap = SelectObject(hMemDC, hOldBitmap);      

	//** clean up      
   
	DeleteDC(hScrDC);     
	DeleteDC(hMemDC);      
  
	//** return handle to the bitmap      
   
	return (HBITMAP)hBitmap;
}

HBITMAP CHAxCaptureDlg::CopyWindowToBitmap(CWnd *wnd, HWND hWnd, WORD fPrintArea)
{
	HBITMAP     hBitmap = NULL;  //** handle to device-dependent bitmap

	//** check for a valid window handle      
	if(!hWnd)         
		return NULL; 

    RECT rectWnd; 
    ::GetWindowRect(hWnd, &rectWnd);  

	switch(fPrintArea)     
	{         
		case PW_WINDOW: //** copy entire window         
		{             
          //** get the window rectangle              
          //** get the bitmap of that window by calling             
		  //** CopyScreenToBitmap and passing it the window rect                     
		  hBitmap = CopyScreenToBitmap(&rectWnd);             
		  break;         
		}   
       case PW_CLIENT: //** copy client area         
	   {             
			RECT rectClient;             
			POINT pt1, pt2;								//** get client dimensions              
			wnd->GetClientRect(&rectClient);			//** convert client coords to screen coords              
			pt1.x = rectClient.left;             
			pt1.y = rectClient.top;             
			pt2.x = rectClient.right;             
			pt2.y = rectClient.bottom;             
			wnd->ClientToScreen(&pt1);             
			wnd->ClientToScreen(&pt2);             
			rectClient.left = pt1.x;             
			rectClient.top = pt1.y;             
			rectClient.right = pt2.x;             
			rectClient.bottom = pt2.y;  

			//** get the bitmap of the client area by calling             
			//** CopyScreenToBitmap and passing it the client rect                          
			hBitmap = CopyScreenToBitmap(&rectClient); 
            
	     break;         
	   }              
	   default:    // invalid print area             
	        return NULL;     
	}      // return handle to the bitmap     
	
	return hBitmap;
}
//** --------------------------------------------- **//

//** START: Leet About Dialog stuff
#define SCROLLAMOUNT			-1
#define DISPLAY_SLOW			40
#define DISPLAY_MEDIUM			15
#define DISPLAY_FAST			10
#define DISPLAY_SPEED			DISPLAY_MEDIUM

#define RED						RGB(255,0,0)
#define GREEN					RGB(0,255,0)
#define BLUE					RGB(0,0,255)
#define WHITE   				RGB(255,255,255)
#define YELLOW  				RGB(255,255,0)
#define TURQUOISE 				RGB(0,255,255)
#define PURPLE  				RGB(255,0,255)
#define BLACK       			RGB(0,0,0)
#define GRAY					RGB(128,128,128)
#define LIGHT_GRAY				RGB(192,192,192)
#define DARK_GRAY				RGB(64,64,64)
#define DARK_BLUE				RGB(0,0,139)

#define BACKGROUND_COLOR        GRAY
#define TOP_LEVEL_TITLE_COLOR	DARK_GRAY
#define TOP_LEVEL_GROUP_COLOR   DARK_BLUE
#define GROUP_TITLE_COLOR       LIGHT_GRAY
#define NORMAL_TEXT_COLOR		WHITE

//** Font sizes
#define TOP_LEVEL_TITLE_HEIGHT	21		
#define TOP_LEVEL_GROUP_HEIGHT  19     
#define GROUP_TITLE_HEIGHT    	16     
#define	NORMAL_TEXT_HEIGHT		15

#define TOP_LEVEL_TITLE			'\n'
#define TOP_LEVEL_GROUP         '\r'
#define GROUP_TITLE           	'\t'
#define NORMAL_TEXT				'\f' 
#define DISPLAY_BITMAP			'\b'

#define ARRAYCOUNT		51
char *pArrCredit[] = { 	"\n",
						"hAx Capture\n",
						"Copyright (C) 2005 hAx Studios Ltd.\f",
						"All rights reserved.\f",
						"",
						"",
						"Main:\r",
						"Project Lead\t",
						"fritz\f",
						"Interface Design\t",
						"fritz\f",
						"Graphics\t",
						"fritz\f",
						"Code\t",
						"fritz\f",
						"",
						"",
						"hAx Studios Ltd. Is:\r",
						"fritz\f",
						"(Working on the roster <.<)\f",
						"",
						"",
						"Special Thanks:\r",
						"IDB_HAXLOGO\b",
						"hAx Studios Ltd.\f",
						"",
						"Allowei\f",
						"PyRo\f",
						"MooreR\f",
						"Shane Martin\f",
						"Chris Maunder\f",
						"Mark Findlay\f",
						"The eAthena Development Team\f",
						"and\f",
						"The Synthetic-Reality Community\f",
						"",
						"",
						"",
						"",
						"",
						};

void CAboutDlg::OnOK() 
{
	PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);

	CDialog::OnOK();
}


void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent != DISPLAY_TIMER_ID)
	{
		CDialog::OnTimer(nIDEvent);
		return;
	}

	if(!m_bProcessingBitmap)
	if(nCounter++ % nCurrentFontHeight == 0)	 //** Every x timer events, show new line
	{
		nCounter=1;
		m_szWork = pArrCredit[nArrIndex++];
		
		if(nArrIndex > ARRAYCOUNT-1)
		{
			nArrIndex = 0;
		}
		nClip = 0;
		m_bDrawText=TRUE;
	}
	
	m_pDisplayFrame->ScrollWindow(0, SCROLLAMOUNT, &m_ScrollRect, &m_ScrollRect);
	nClip = nClip + abs(SCROLLAMOUNT);	
	
    CRect r;
    CWnd* pWnd = GetDlgItem(IDC_DISPLAY_STATIC);
    ASSERT_VALID(pWnd);
    pWnd->GetClientRect(&r);
    pWnd->ClientToScreen(r);
    ScreenToClient(&r);
    InvalidateRect(r, FALSE); //** FALSE does not erase background

	CDialog::OnTimer(nIDEvent);	
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	BOOL bRet;
	UINT nRet;
	
	nCurrentFontHeight = NORMAL_TEXT_HEIGHT;
	
	CClientDC dc(this);
	bRet = m_dcMem.CreateCompatibleDC(&dc);
	
	
	m_bProcessingBitmap = FALSE;
	
	nArrIndex = 0;
	nCounter = 1;
	nClip = 0;
	m_bFirstTime = TRUE;
	m_bDrawText = FALSE;
	m_hBmpOld = 0;
	
	m_pDisplayFrame = (CWnd*)GetDlgItem(IDC_DISPLAY_STATIC);	

	_ASSERTE(m_pDisplayFrame);
				 
	m_pDisplayFrame->GetClientRect(&m_ScrollRect);


	nRet = SetTimer(DISPLAY_TIMER_ID, DISPLAY_SPEED, NULL);
    _ASSERTE(nRet != 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	

	KillTimer(DISPLAY_TIMER_ID);

	m_dcMem.SelectObject(CBitmap::FromHandle(m_hBmpOld));
    m_bmpWork.DeleteObject();	
}

void CAboutDlg::OnPaint() 
{
	CPaintDC dc(this); //** Device context for painting
	
	PAINTSTRUCT ps;
	CDC* pDc = m_pDisplayFrame->BeginPaint(&ps);
	
	pDc->SetBkMode(TRANSPARENT);

    	CFont m_fntTahoma;
	CFont* pOldFont;
	BOOL bSuccess;
	
	BOOL bUnderline;
	BOOL bItalic;


	if(!m_szWork.IsEmpty())
	switch (m_szWork[m_szWork.GetLength()-1])
	{
		case NORMAL_TEXT:
		default:
			bItalic = FALSE;
			bUnderline = FALSE;
			nCurrentFontHeight = NORMAL_TEXT_HEIGHT;
   			bSuccess = m_fntTahoma.CreateFont(NORMAL_TEXT_HEIGHT, 0, 0, 0, 
   								FW_THIN, bItalic, bUnderline, 0, 
   								ANSI_CHARSET,
                               	OUT_DEFAULT_PRECIS,
                               	CLIP_DEFAULT_PRECIS,
                               	PROOF_QUALITY,
                               	VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                               	(LPSTR)"Tahoma");
			pDc->SetTextColor(NORMAL_TEXT_COLOR);
			pOldFont  = pDc->SelectObject(&m_fntTahoma);
		break;

		case TOP_LEVEL_GROUP:
			bItalic = FALSE;
			bUnderline = FALSE;
			nCurrentFontHeight = TOP_LEVEL_GROUP_HEIGHT;
   			bSuccess = m_fntTahoma.CreateFont(TOP_LEVEL_GROUP_HEIGHT, 0, 0, 0, 
   								FW_BOLD, bItalic, bUnderline, 0, 
   								ANSI_CHARSET,
                               	OUT_DEFAULT_PRECIS,
                               	CLIP_DEFAULT_PRECIS,
                               	PROOF_QUALITY,
                               	VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                               	(LPSTR)"Tahoma");
			pDc->SetTextColor(TOP_LEVEL_GROUP_COLOR);
			pOldFont  = pDc->SelectObject(&m_fntTahoma);
		break;
		
		
		
		case GROUP_TITLE:
			bItalic = FALSE;
			bUnderline = FALSE;
			nCurrentFontHeight = GROUP_TITLE_HEIGHT;
   			bSuccess = m_fntTahoma.CreateFont(GROUP_TITLE_HEIGHT, 0, 0, 0, 
   								FW_BOLD, bItalic, bUnderline, 0, 
   								ANSI_CHARSET,
                               	OUT_DEFAULT_PRECIS,
                               	CLIP_DEFAULT_PRECIS,
                               	PROOF_QUALITY,
                               	VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                               	(LPSTR)"Tahoma");
			pDc->SetTextColor(GROUP_TITLE_COLOR);
			pOldFont  = pDc->SelectObject(&m_fntTahoma);
		break;
		
		
		case TOP_LEVEL_TITLE:
			bItalic = FALSE;
			bUnderline = TRUE;
			nCurrentFontHeight = TOP_LEVEL_TITLE_HEIGHT;
			bSuccess = m_fntTahoma.CreateFont(TOP_LEVEL_TITLE_HEIGHT, 0, 0, 0, 
								FW_BOLD, bItalic, bUnderline, 0, 
								ANSI_CHARSET,
	                           	OUT_DEFAULT_PRECIS,
	                           	CLIP_DEFAULT_PRECIS,
	                           	PROOF_QUALITY,
	                           	VARIABLE_PITCH | 0x04 | FF_DONTCARE,
	                           	(LPSTR)"Tahoma");
			pDc->SetTextColor(TOP_LEVEL_TITLE_COLOR);
			pOldFont  = pDc->SelectObject(&m_fntTahoma);
		break;
		
		case DISPLAY_BITMAP:
			if(!m_bProcessingBitmap)
			{
				CString szBitmap = m_szWork.Left(m_szWork.GetLength()-1);
	   			if (!m_bmpWork.LoadBitmap((const char *)szBitmap))
				{
					CString str; 
					str.Format("Could not find bitmap resource \"%s\"", szBitmap); 
					KillTimer(DISPLAY_TIMER_ID); 
					MessageBox(str); 
					return; 
				}
				m_bmpCurrent = &m_bmpWork;
	   			m_bmpCurrent->GetObject(sizeof(BITMAP), &m_bmpInfo);
			
				m_size.cx = m_bmpInfo.bmWidth;	// width  of dest rect
				RECT workRect;
				m_pDisplayFrame->GetClientRect(&workRect);
				m_pDisplayFrame->ClientToScreen(&workRect);
				ScreenToClient(&workRect);
				//** Upper left point of dest
				m_pt.x = (workRect.right - 
							((workRect.right-workRect.left)/2) - (m_bmpInfo.bmWidth/2));
				m_pt.y = workRect.bottom;
				
				
				pBmpOld = m_dcMem.SelectObject(m_bmpCurrent);
				if(m_hBmpOld == 0)
					m_hBmpOld = (HBITMAP) pBmpOld->GetSafeHandle();
				m_bProcessingBitmap = TRUE;
			}
		break;

		}
	
	
	
	
	CBrush bBrush(BACKGROUND_COLOR);
	CBrush* pOldBrush;
	pOldBrush  = pDc->SelectObject(&bBrush);
	//** Only fill rect comprised of gap left by bottom of scrolling window
	r = m_ScrollRect;
	r.top = r.bottom-abs(SCROLLAMOUNT); 
	pDc->DPtoLP(&r);
	
	if(m_bFirstTime)
	{
		m_bFirstTime=FALSE;
		pDc->FillRect(&m_ScrollRect, &bBrush);
	}
	else
	{
		pDc->FillRect(&r, &bBrush);
	}
	r = m_ScrollRect;
	r.top = r.bottom-nClip;
	
	
	if(!m_bProcessingBitmap)
	{
		int x = pDc->DrawText((const char *)m_szWork, m_szWork.GetLength()-1, &r, DT_TOP |DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);	
		m_bDrawText = FALSE;
	}
	else
	{
    	dc.StretchBlt( m_pt.x, m_pt.y-nClip, m_size.cx, nClip, &m_dcMem, 0, 0, m_bmpInfo.bmWidth-1, nClip, SRCCOPY);
		if(nClip > m_bmpInfo.bmHeight)
		{
			m_bmpWork.DeleteObject();
			m_bProcessingBitmap = FALSE;
			nClip = 0;
			m_szWork.Empty();
			nCounter = 1;
		}
		pDc->SelectObject(pOldBrush);
		bBrush.DeleteObject();
		m_pDisplayFrame->EndPaint(&ps);
		return;
	}
	
	
	pDc->SelectObject(pOldBrush);
	bBrush.DeleteObject();
	
	if(!m_szWork.IsEmpty())
	{
		pDc->SelectObject(pOldFont);
		m_fntTahoma.DeleteObject();
	}

	m_pDisplayFrame->EndPaint(&ps);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CAboutDlg::OnVisithax() 
{
	PlaySound("IDR_CHIMES",0,SND_RESOURCE | SND_ASYNC);
	
	//** code to launch hAx Studios Ltd. in the DEFAULT browser
	char lpBrowser[512];
	char holder[512];
	char addytype[512];
	DWORD dwLen = 500;
	DWORD dwType;
	HKEY hKey;
	LPSTR lpPathValue;
	ZeroMemory(lpBrowser, 512);
	
	ZeroMemory(holder, 512);
	strncpy(holder, "http://hax-studios.net", 4);

	lpPathValue = (LPSTR)GlobalAlloc(GMEM_FIXED, 500);
	
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,addytype, 0,KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey,"",0,&dwType,(BYTE *)lpPathValue, &dwLen) ==  ERROR_SUCCESS)
		{
			ZeroMemory(holder, 512);
			strcpy(holder, lpPathValue+1);
			strncpy(lpBrowser, holder, strrchr(holder, '\"') - holder);
		}
		else
		{
			strcpy(lpBrowser, "iexplore");
		}
	}
	else
	{
		strcpy(lpBrowser, "iexplore");
	}

	RegCloseKey(hKey);
	GlobalFree(lpPathValue);

	ShellExecute(NULL, "open", lpBrowser, "http://hax-studios.net", NULL, SW_SHOWMAXIMIZED);
	
}

void CHAxCaptureDlg::OnSize(UINT nType, int cx, int cy) 
{
	/* - Commented out... has some blinking problems? 
	if(m_boolCloseToTray == TRUE)
	{
		if(nType == SIZE_MINIMIZED)
		{
			if(m_boolBalloons == TRUE)
			{
				m_TrayIcon.ShowBalloon("hAx Capture has been sent to the system tray.", "hAx Capture :: Notification");
			}

			CSystemTray::MinimiseToTray(this, TRUE);
			ShowWindow(SW_HIDE);
		}
	}
	else
	{
		CDialog::OnSize(nType, cx, cy);
	}
	*/

	CDialog:: OnSize(nType, cx, cy);
}
