
// YUVPlayerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "YUVPlayerDlg.h"
#include "afxdialogex.h"

#include "yuv2rgb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static BOOL g_iYuv420Init = FALSE;
static BOOL g_iYuv422Init = FALSE;
static BOOL g_iEndFile = FALSE;

UINT Play(LPVOID pParam);

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CYUVPlayerDlg �Ի���




CYUVPlayerDlg::CYUVPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYUVPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pSettingDlg = NULL;
    m_fInit = FALSE;

    // just in case
    m_nWidth = 176;
    m_nHeight = 144;
    m_nFps = 30;
    m_nYuvFormat = 0;
    m_fLoop = FALSE;

    m_pbYuvData = NULL;
    m_pbRgbData = NULL;
    m_pWinThread = NULL;
}

CYUVPlayerDlg::~CYUVPlayerDlg()
{
    if (m_pSettingDlg != NULL)
    {
        delete m_pSettingDlg;
        m_pSettingDlg = NULL;
    }
}

void CYUVPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_FIRST, m_bFirstFrame);
    DDX_Control(pDX, IDC_BUTTON_LAST, m_bLastFrame);
    DDX_Control(pDX, IDC_BUTTON_NEXT, m_bNextFrame);
    DDX_Control(pDX, IDC_BUTTON_OPEN, m_bOpenFile);
    DDX_Control(pDX, IDC_BUTTON_PLAY, m_bPlay);
    DDX_Control(pDX, IDC_BUTTON_PREV, m_bPrevFrame);
    DDX_Control(pDX, IDC_BUTTON_SAVE, m_bSaveFrame);
    DDX_Control(pDX, IDC_BUTTON_SET, m_bSetting);
    DDX_Control(pDX, IDC_BUTTON_STOP, m_bStop);
}

BEGIN_MESSAGE_MAP(CYUVPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BUTTON_SET, &CYUVPlayerDlg::OnBnClickedButtonSet)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &CYUVPlayerDlg::OnBnClickedButtonOpen)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CYUVPlayerDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_PLAY, &CYUVPlayerDlg::OnBnClickedButtonPlay)
    ON_WM_NCHITTEST()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_FILE_OPEN, &CYUVPlayerDlg::OnFileOpen)
    ON_COMMAND(ID_FILE_CLOSE, &CYUVPlayerDlg::OnFileClose)
    ON_COMMAND(ID_FILE_SAVE, &CYUVPlayerDlg::OnFileSave)
    ON_COMMAND(ID_FILE_EXIT, &CYUVPlayerDlg::OnFileExit)
    ON_COMMAND(ID_PLAYING_PLAY, &CYUVPlayerDlg::OnPlayingPlay)
    ON_COMMAND(ID_PLAYING_SETTING, &CYUVPlayerDlg::OnPlayingSetting)
    ON_COMMAND(ID_PLAYING_STOP, &CYUVPlayerDlg::OnPlayingStop)
    ON_COMMAND(ID_HELP_HELP, &CYUVPlayerDlg::OnHelpHelp)
    ON_COMMAND(ID_HELP_ABOUT, &CYUVPlayerDlg::OnHelpAbout)
    ON_COMMAND(ID_R_OPEN, &CYUVPlayerDlg::OnROpen)
    ON_COMMAND(ID_R_SETTING, &CYUVPlayerDlg::OnRSetting)
    ON_COMMAND(ID_R_EXIT, &CYUVPlayerDlg::OnRExit)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CYUVPlayerDlg::OnBnClickedButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_PREV, &CYUVPlayerDlg::OnBnClickedButtonPrev)
    ON_BN_CLICKED(IDC_BUTTON_NEXT, &CYUVPlayerDlg::OnBnClickedButtonNext)
    ON_BN_CLICKED(IDC_BUTTON_FIRST, &CYUVPlayerDlg::OnBnClickedButtonFirst)
    ON_BN_CLICKED(IDC_BUTTON_LAST, &CYUVPlayerDlg::OnBnClickedButtonLast)
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CYUVPlayerDlg ��Ϣ�������

BOOL CYUVPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	//ShowWindow(SW_MINIMIZE); // ��Ҫ��С������

	// ��ʼ�� ����  �Ի���
    if (m_pSettingDlg == NULL)
    {
        m_pSettingDlg = new CSettingDlg();
        m_pSettingDlg->Create(IDD_DIALOG_SETTING, this);
        m_pSettingDlg->SetParentWnd(this);
        // ���Ӵ��ڵ�ֵ���ݸ�������
        m_pSettingDlg->SetParametersToParentWnd(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
    }
    
    // ���ఴť
    m_nStartX[0][0] = IDC_STATIC_FRAMECNT;
    m_nStartX[0][1] = IDC_SLIDER1;
    m_nStartX[0][2] = IDC_BUTTON_OPEN;
    m_nStartX[0][3] = IDC_BUTTON_SAVE;
    m_nStartX[0][4] = IDC_BUTTON_PLAY;
    m_nStartX[0][5] = IDC_BUTTON_STOP;
    m_nStartX[0][6] = IDC_BUTTON_PREV;
    m_nStartX[0][7] = IDC_BUTTON_NEXT;
    m_nStartX[0][8] = IDC_BUTTON_FIRST;
    m_nStartX[0][9] = IDC_BUTTON_LAST;
    m_nStartX[0][10] = IDC_BUTTON_SET;

    CRect rect;
    for (int i = 0; i < 10; i++)
    {
        GetDlgItem(m_nStartX[0][i])->GetWindowRect(rect);
        ScreenToClient(rect);
        m_nStartX[1][i] = rect.left;
    }
    // ��ͼ
    m_bOpenFile.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_OPEN)));
    m_bSaveFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SAVE)));
    m_bSaveFrame.EnableWindow(FALSE);
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_bPlay.EnableWindow(FALSE);
    m_bStop.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_STOP)));
    m_bStop.EnableWindow(FALSE);
    m_bPrevFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PREV)));
    m_bPrevFrame.EnableWindow(FALSE);
    m_bNextFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_NEXT)));
    m_bNextFrame.EnableWindow(FALSE);
    m_bFirstFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_FIRST)));
    m_bFirstFrame.EnableWindow(FALSE);
    m_bLastFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_LAST)));
    m_bLastFrame.EnableWindow(FALSE);
    m_bSetting.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SETTING)));

    // ����������
    HANDLE hPlay = NULL;
    if ((hPlay = OpenMutex(MUTEX_ALL_ACCESS,FALSE,_T("Play"))) == NULL)
    {
        hPlay = CreateMutex(NULL, FALSE, _T("Play"));
    }

    m_fInit = TRUE;
    m_fShowBlack = TRUE;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CYUVPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CYUVPlayerDlg::OnPaint()
{
#if 01
    //picture�ؼ�����ɫΪ��ɫ
    if (m_fShowBlack)
    {
        CRect rtTop;
        CStatic *pWnd = (CStatic*)GetDlgItem(IDC_VIDEO);
        CDC *cDc = pWnd->GetDC();
        pWnd->GetClientRect(&rtTop);
        cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(),RGB(0,0,0));
        Invalidate(FALSE);
    }
#endif

    ShowPicture((BYTE *)m_pbRgbData, m_iRgbSize);

	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CYUVPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CYUVPlayerDlg::ShowSettingWindow()
{
    // ��ģ̬�Ի���
    if (m_pSettingDlg == NULL)
    {
        m_pSettingDlg = new CSettingDlg();
        m_pSettingDlg->Create(IDD_DIALOG_SETTING, this);
    }

    m_pSettingDlg->ShowWindow(SW_SHOW);
    //m_pSettingDlg->CenterWindow(this);
    //m_pParentWnd->CenterWindow(this);

    /*
    CWnd *pWnd = GetDlgItem(IDD_DIALOG_SETTING);
    if (pWnd)
    {
        pWnd->SetWindowPos( NULL,4,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }
    */
}

// -------------------------------------------------------

// ʵ�ַǱ���Ҳ��������϶�
LRESULT CYUVPlayerDlg::OnNcHitTest(CPoint point)
{
     UINT nHitTest = CDialogEx::OnNcHitTest(point);
     if ((nHitTest == HTCLIENT) && (::GetAsyncKeyState (MK_LBUTTON) < 0))
         nHitTest = HTCAPTION;
     return nHitTest;
    //return CDialogEx::OnNcHitTest(point); // not this....
}

// �Ҽ��˵�
void CYUVPlayerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    // TODO: Add your message handler code here
    CMenu popMenu;
    CMenu *pPopup;
    popMenu.LoadMenu(IDR_MENU2);

    pPopup=popMenu.GetSubMenu(0);

    pPopup->EnableMenuItem(ID_R_OPEN, MF_BYCOMMAND|MF_ENABLED);
    pPopup->EnableMenuItem(ID_R_SETTING, MF_BYCOMMAND|MF_ENABLED);
    pPopup->EnableMenuItem(ID_R_EXIT, MF_BYCOMMAND|MF_ENABLED);

    //pPopup->EnableMenuItem(ID_R_SETTING,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
    pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
    pPopup->Detach();
    popMenu.DestroyMenu();
}

/////////////////////////////////////////// ------> ����˵��¼�
void CYUVPlayerDlg::OnFileOpen()
{

}


void CYUVPlayerDlg::OnFileClose()
{

}


void CYUVPlayerDlg::OnFileSave()
{

}


void CYUVPlayerDlg::OnFileExit()
{

}


void CYUVPlayerDlg::OnPlayingPlay()
{

}


void CYUVPlayerDlg::OnPlayingSetting()
{
    ShowSettingWindow();
}


void CYUVPlayerDlg::OnPlayingStop()
{

}


void CYUVPlayerDlg::OnHelpHelp()
{

    MessageBox(_T("Help me"));
}


void CYUVPlayerDlg::OnHelpAbout()
{

    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}
/////////////////////////////////////////// <------ ����˵��¼�

/////////////////////////////////////////// ------> �Ҽ��˵��¼�
void CYUVPlayerDlg::OnROpen()
{

}


void CYUVPlayerDlg::OnRSetting()
{
    ShowSettingWindow();
}


void CYUVPlayerDlg::OnRExit()
{
    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbRgbData != NULL)
    {
        delete[] m_pbRgbData;
        m_pbRgbData = NULL;
    }
    OnCancel();
}

/////////////////////////////////////////// <------ �Ҽ��˵��¼�

/////////////////////////////////////////// ------> ������ť�¼�
void CYUVPlayerDlg::OnBnClickedButtonOpen()
{
    wchar_t szFilter[] = _T("YUV Files(*.yuv;*.raw)|*.yuv;*.raw|All Files(*.*)|*.*||");
    CFileDialog fileDlg(TRUE, _T("YUV"), NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("ѡ��YUV�ļ�");   // ����
    if (fileDlg.DoModal() != IDOK)
        return;

    m_strPathName = fileDlg.GetPathName();
    CString strTemp;
    strTemp.Format(_T("%s-%s"), m_strPathName.GetBuffer(), APP_NAM);
    this->SetWindowText(strTemp);

    ShowOpenedFrame();

    m_bSaveFrame.EnableWindow(TRUE);
    m_bPlay.EnableWindow(TRUE);
    //m_bStop.EnableWindow(TRUE);
    m_bPrevFrame.EnableWindow(TRUE);
    m_bNextFrame.EnableWindow(TRUE);
    m_bFirstFrame.EnableWindow(TRUE);
    m_bLastFrame.EnableWindow(TRUE);

    return;
}


void CYUVPlayerDlg::OnBnClickedButtonSave()
{

}


void CYUVPlayerDlg::OnBnClickedButtonPlay()
{
    static BOOL bPlay = TRUE;

    //UpdateData();
    m_bStop.EnableWindow(TRUE);

    if (bPlay)
    {
        //GetDlgItem(IDC_BN_PLAY)->SetWindowText(_T("��ͣ"));
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PAUSE)));
        bPlay = FALSE;
    }
    else
    {
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
        bPlay = TRUE;
    }

    HANDLE hPlay = NULL;
    hPlay = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Play"));

    if (!hPlay)
    {
        CString msg;
        //printErrorMessage("Open Mutex failed.", msg);
        MessageBox(msg);
    }

    if (bPlay)
    {
        WaitForSingleObject(hPlay, 0);
        //MessageBox("Play...");
    }
    else
    {
        ReleaseMutex(hPlay);
        //MessageBox("Pause...");
    }

    if (m_pWinThread == NULL)
        m_pWinThread = AfxBeginThread(Play, this);
}


void CYUVPlayerDlg::OnBnClickedButtonStop()
{

}


void CYUVPlayerDlg::OnBnClickedButtonPrev()
{

}


void CYUVPlayerDlg::OnBnClickedButtonNext()
{

}


void CYUVPlayerDlg::OnBnClickedButtonFirst()
{

}


void CYUVPlayerDlg::OnBnClickedButtonLast()
{

}


void CYUVPlayerDlg::OnBnClickedButtonSet()
{
    ShowSettingWindow();
}

/////////////////////////////////////////// <------ ������ť�¼�

/////////////////////////////////////////// ------> ���̿�ݼ��¼�



// ��������
// todo:ֻ�ܿ���ͬʱ����
void CYUVPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if (!m_fInit) return;
    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, cx, cy-26-20-21);
        pWnd->Invalidate();
        pWnd->UpdateData();
    }
    // ���ұߵİ�ť
    pWnd = GetDlgItem(m_nStartX[0][10]);
    if (pWnd)
    {
       pWnd->SetWindowPos(NULL,cx-30,cy-26,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }

    int startx = 2;
    // ֡��ͳ��
    pWnd = GetDlgItem(m_nStartX[0][0]);
    if (pWnd)
    {
        pWnd->SetWindowPos(NULL,startx+m_nStartX[1][0],cy-26-20-21,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }
    // ������
    pWnd = GetDlgItem(m_nStartX[0][1]);
    if (pWnd)
    {
        //pWnd->MoveWindow(startx+0, cy-26-26, cx, cy-26-20-21);
        //pWnd->Invalidate();
        pWnd->SetWindowPos( NULL,startx+m_nStartX[1][1],cy-26-26,cx,26,SWP_NOZORDER);
    }
    // ˮƽλ����ͬ�İ�ť
    for (int i = 2; i < 10; i++)
    {
        pWnd = GetDlgItem(m_nStartX[0][i]);
        if (pWnd)
        {
            pWnd->SetWindowPos(NULL,startx+m_nStartX[1][i],cy-26,0,0,SWP_NOZORDER|SWP_NOSIZE);
        }
    }
}


void CYUVPlayerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialogEx::OnSizing(fwSide, pRect);

#if 0
    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, pRect->right - pRect->left, pRect->bottom - pRect->top);
        pWnd->Invalidate(FALSE);
        pWnd->UpdateData();
    }
#endif
}

void CYUVPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
    // note ��m_strPathName�Ļ����رմ��ں󣬽��̻���
    CDialogEx::OnDropFiles(hDropInfo);
    wchar_t* pFilePathName =(wchar_t *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName, MAX_URL_LENGTH);  // ��ȡ�Ϸ��ļ��������ļ�������ؼ���
    m_strPathName.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);   // ע����������٣��������ͷ�Windows Ϊ�����ļ��ϷŶ�������ڴ�

    CString strTemp;
    strTemp.Format(_T("%s-%s"), pFilePathName, APP_NAM);
    this->SetWindowText(strTemp);

    // ��ʾ��һ֡
    ShowOpenedFrame();

    m_bSaveFrame.EnableWindow(TRUE);
    m_bPlay.EnableWindow(TRUE);
    //m_bStop.EnableWindow(TRUE);
    m_bPrevFrame.EnableWindow(TRUE);
    m_bNextFrame.EnableWindow(TRUE);
    m_bFirstFrame.EnableWindow(TRUE);
    m_bLastFrame.EnableWindow(TRUE);


#if 0
    save_yuv_file("YUV_yuyv.yuv", 320, 240, FMT_YUYV);
    save_yuv_file("YUV_yvyu.yuv", 320, 240, FMT_YVYU);
    save_yuv_file("YUV_uyvy.yuv", 320, 240, FMT_UYVY);
    save_yuv_file("YUV_vyuy.yuv", 320, 240, FMT_VYUY);
#endif
}

/////////////////////////////
// �ڲ�ʵ��

// ���ļ� ʱ��ʾ��һ֡
void CYUVPlayerDlg::ShowOpenedFrame()
{
    // ����YUV��ʽ
    switch (m_nYuvFormat)
    {
    case FMT_YUV420:
    case FMT_YV12:
    case FMT_NV12:
    case FMT_NV21:
        m_iYuvSize = m_nWidth * m_nHeight * 3 / 2;
        break;
    case FMT_YUV422:
    case FMT_YV16:
    case FMT_YUYV:
    case FMT_YVYU:
    case FMT_UYVY:
    case FMT_VYUY:
    case FMT_NV16:
    case FMT_NV61:
        m_iYuvSize = m_nWidth * m_nHeight * 2;
        break;
    default:
        break;
    }

    m_iRgbSize = m_nWidth * m_nHeight * 3 + 54;

    m_pbYuvData = new char[m_iYuvSize];
    m_pbRgbData  = new char[m_iRgbSize];

    // ���ļ�
    // ��ʱֻ֧��һ���ļ�������Ѿ��򿪣��͹ر�
    // TODO���˴�Ӧ�����Ż�
    if (CFile::hFileNull != m_cFile.m_hFile)
    {
        m_cFile.Close();
    }
    if (!m_cFile.Open(m_strPathName.GetBuffer(), CFile::modeRead))
    {
        MessageBox(_T("��YUV�ļ�ʧ��!"));
        return;
    }
    m_cFile.Read(m_pbYuvData, m_iYuvSize);

    // �����BMPͷ
    m_bmHeader.bfType = 'MB';
    m_bmHeader.bfSize = m_iRgbSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    m_bmHeader.bfReserved1 = 0;
    m_bmHeader.bfReserved2 = 0;
    m_bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    m_bmInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
    m_bmInfo.bmiHeader.biWidth  = m_nWidth;
    // note BMPͼ���ǵ�������
    m_bmInfo.bmiHeader.biHeight = -m_nHeight;
    m_bmInfo.bmiHeader.biPlanes = 1;
    m_bmInfo.bmiHeader.biBitCount = 24;
    m_bmInfo.bmiHeader.biCompression = BI_RGB;
    m_bmInfo.bmiHeader.biSizeImage   = m_iRgbSize;
    m_bmInfo.bmiHeader.biXPelsPerMeter = 0;
    m_bmInfo.bmiHeader.biYPelsPerMeter = 0;
    m_bmInfo.bmiHeader.biClrUsed = 0;
    m_bmInfo.bmiHeader.biClrImportant = 0;

    memcpy(m_pbRgbData, &m_bmHeader, sizeof(BITMAPFILEHEADER));
    memcpy(m_pbRgbData+sizeof(BITMAPFILEHEADER), &m_bmInfo, sizeof(BITMAPINFOHEADER));

    // ��ת����ʽ
    yuv_to_rgb24((YUV_TYPE)m_nYuvFormat, (unsigned char *)m_pbYuvData, (unsigned char *)m_pbRgbData+54, m_nWidth, m_nHeight);
    // rgb->bgr
    swargb((BYTE*)m_pbRgbData+54, m_iRgbSize-54);
    //yuv420_to_rgb24((unsigned char *)m_pbYuvData, (unsigned char *)m_pbRgbData+54, m_nWidth, m_nHeight);
    // ��ʾ
    ShowPicture((BYTE *)m_pbRgbData, m_iRgbSize);
}

inline void RenderBitmap(CWnd *pWnd, Bitmap* pbmp)
{
    RECT rect;
    pWnd->GetClientRect( &rect );

    Graphics grf( pWnd->m_hWnd );
    if ( grf.GetLastStatus() == Ok )
    {
        Rect rc( rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top );

        Status st = grf.DrawImage(pbmp, rc);
        if (st != Ok)
        {
            //OutputDebugString("DRAWIMAGE ERROR\n");
        }
    }
}

// ��ʾͼƬ
void CYUVPlayerDlg::ShowPicture(BYTE* pbData, int iSize)
{
    if (pbData == NULL) return;
    CWnd* pWnd=GetDlgItem(IDC_VIDEO);   // IDC_VIDEO��picture contral �ؼ�ID
    IStream* pPicture = NULL;
    CreateStreamOnHGlobal(NULL,TRUE,&pPicture);
    if( pPicture != NULL )
    {
        pPicture->Write(pbData,  iSize, NULL);
        LARGE_INTEGER liTemp = { 0 };
        pPicture->Seek(liTemp, STREAM_SEEK_SET, NULL);
        Bitmap TempBmp(pPicture);
        RenderBitmap(pWnd, &TempBmp);
    }
    if(pPicture != NULL)
    {
        pPicture->Release();
        pPicture = NULL;
    }

    m_fShowBlack = FALSE;
}

// �����߳�
UINT Play(LPVOID pParam)
{
    int i = 0;
    CYUVPlayerDlg* pWin = (CYUVPlayerDlg *)pParam;  // �Ի�����

    int iTimeSpan = 1000 / pWin->m_nFps;

    if (CFile::hFileNull == pWin->m_cFile.m_hFile)
    {
        //MessageBox("���ȴ��ļ�");
        return -1;
    }

    HANDLE hPlay = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Play"));

    if (!hPlay)
    {
        CString msg;
        //printErrorMessage("Open Mutex failed.", msg);
        MessageBox(pWin->m_hWnd, msg, NULL, MB_OK);
    }

    CString strTemp;
    BOOL bEof = FALSE;

    //if (m_cLoop.GetCheck())
    //    MessageBox("Check");
    while (!bEof)
    {
        DWORD t1 = GetTickCount();

        if (WAIT_OBJECT_0 == WaitForSingleObject(hPlay,INFINITE))
        {
            ReleaseMutex(hPlay);
        }
        if (pWin->m_iYuvSize != pWin->m_cFile.Read(pWin->m_pbYuvData, pWin->m_iYuvSize))
        {
            bEof = TRUE;
            g_iEndFile = TRUE;
            //m_cFile.SeekToBegin();
            //i = 0;
            break;
        }
        i++;
        pWin->m_cFile.Seek(pWin->m_iYuvSize * i, SEEK_SET);
        // �����BMPͷ
        pWin->m_bmHeader.bfType = 'MB';
        pWin->m_bmHeader.bfSize = pWin->m_iRgbSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        pWin->m_bmHeader.bfReserved1 = 0;
        pWin->m_bmHeader.bfReserved2 = 0;
        pWin->m_bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        pWin->m_bmInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
        pWin->m_bmInfo.bmiHeader.biWidth  = pWin->m_nWidth;
        // note BMPͼ���ǵ�������
        pWin->m_bmInfo.bmiHeader.biHeight = -pWin->m_nHeight;;
        pWin->m_bmInfo.bmiHeader.biPlanes = 1;
        pWin->m_bmInfo.bmiHeader.biBitCount = 24;
        pWin->m_bmInfo.bmiHeader.biCompression = BI_RGB;
        pWin->m_bmInfo.bmiHeader.biSizeImage   = pWin->m_iRgbSize;
        pWin->m_bmInfo.bmiHeader.biXPelsPerMeter = 0;
        pWin->m_bmInfo.bmiHeader.biYPelsPerMeter = 0;
        pWin->m_bmInfo.bmiHeader.biClrUsed = 0;
        pWin->m_bmInfo.bmiHeader.biClrImportant = 0;

        memcpy(pWin->m_pbRgbData, &(pWin->m_bmHeader), sizeof(BITMAPFILEHEADER));
        memcpy(pWin->m_pbRgbData+sizeof(BITMAPFILEHEADER), &(pWin->m_bmInfo), sizeof(BITMAPINFOHEADER));

        // ��ת����ʽ
        yuv_to_rgb24((YUV_TYPE)pWin->m_nYuvFormat, (unsigned char *)pWin->m_pbYuvData, (unsigned char *)pWin->m_pbRgbData+54, pWin->m_nWidth, pWin->m_nHeight);
        //yuv420_to_rgb24((unsigned char *)pWin->m_pbYuvData, (unsigned char *)pWin->m_pbRgbData+54, pWin->m_nWidth, pWin->m_nHeight);
        // rgb->bgr
        swargb((BYTE*)pWin->m_pbRgbData+54, pWin->m_iRgbSize-54);
        // ��ʾ
        pWin->ShowPicture((BYTE *)pWin->m_pbRgbData, pWin->m_iRgbSize);

        DWORD t2 = GetTickCount();
        int t = t2 - t1;
        if (t < iTimeSpan)
            Sleep(iTimeSpan - t);

        //strTemp.Format("���ڲ���: %d %d %d %d", iTimeSpan, t1, t2, t);
    }

    pWin->m_bStop.EnableWindow(FALSE);
    pWin->m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));

    pWin->m_pWinThread = NULL;
    AfxEndThread(0);

    return 0;
}
