
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

    m_nTotalFrame = m_nCurrentFrame = 0;
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
    ON_COMMAND(ID_FILE_SAVEFRAME, &CYUVPlayerDlg::OnFileSave)
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

    //GetDlgItem(ID_FILE_SAVEFRAME)->EnableWindow(FALSE);
    //GetMenu()->GetSubMenu(3)->EnableMenuItem(1,MF_BYPOSITION | MF_ENABLED);
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
    OnBnClickedButtonOpen();
}


void CYUVPlayerDlg::OnFileSave()
{
    OnBnClickedButtonSave();
}


void CYUVPlayerDlg::OnFileExit()
{
    OnRExit();
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

/*
������Զ�ͨ���ļ�������YUV��ʽ����ߣ��ļ�������Ҫ��YUV��ʽ�ַ������ֱ��ʡ�YUV��ʽ�ο����öԻ����Y�������еĸ�ʽ���ֱ��ʿ���qcif��cif��720p��1080p��ֱ���ÿ�x�ߣ�ʾ����foo_yuv422_176x144.yuv��
*/
void CYUVPlayerDlg::OnHelpHelp()
{
    wchar_t* help = _T("YUV������˵����\r\n1��֧�ֳ���YUV��ʽ�ļ����ţ�\r\n2��֧�ֱ���YUV��BMP�ļ���\r\n3��ֱ����ק�ļ�����������ͨ���˵����ļ���\r\n\r\n\r\nTrick\r\n������Զ�ͨ���ļ�������YUV��ʽ����ߣ��ļ�������Ҫ��YUV��ʽ�ַ������ֱ��ʡ�YUV��ʽ�ο����öԻ����Y�������еĸ�ʽ���ֱ��ʿ���qcif��cif��720p��1080p��ֱ���ÿ�x�ߣ�ʾ����foo_yuv422_176x144.yuv��");
    MessageBox((LPCTSTR)help);
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
    OnBnClickedButtonOpen();
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
    fileDlg.GetOFN().lpstrTitle = _T("Open YUV File");   // ����
    if (fileDlg.DoModal() != IDOK)
        return;

    m_strPathName = fileDlg.GetPathName();

    // ��ʾ����
    CString strTemp;
    strTemp.Format(_T("%s-%s"), m_strPathName.GetBuffer(), APP_NAM);
    this->SetWindowText(strTemp);

    // ���ļ���
    wchar_t* tmp = wcsrchr(m_strPathName.GetBuffer(), '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);

    m_pSettingDlg->ParseFilename(szFilename);
    m_pSettingDlg->SetParametersToParentWnd(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    // ��ʾ��һ֡
    ShowOpenedFrame();

    return;
}

void CYUVPlayerDlg::OnBnClickedButtonSave()
{
    // Ĭ��yuv
    wchar_t szFilter[128] = _T("YUV Files(*.%s)|*.%s|BMP(*.bmp)|*.bmp||");

    CFile cFile;
    CString strFile;
    wchar_t szFileName[128] = {0};
    wchar_t szExt[16] = {0};
    wchar_t* pExt = _T("yuv");
    char* pData = m_pbYuvData;
    UINT nSize = m_iYuvSize;

    int nType = 0;

    // ���ļ���
    _wsplitpath(m_strPathName, NULL, NULL, szFileName, szExt);
    if (wcscmp(szExt, _T("yuv")))
    {
        swprintf_s(szFilter, _T("YUV Files(*.%s)|*.%s|BMP(*.bmp)|*.bmp||"), &szExt[1], &szExt[1]);
        pExt = &szExt[1];
    }

    strFile.Format(_T("%s_%d.%s"), szFileName, m_nCurrentFrame, pExt);

    CFileDialog fileDlg(FALSE, _T("yuv"), strFile.GetBuffer(), OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("Save Frame");
    if (fileDlg.DoModal() != IDOK)
        return;

    CString strTemp = fileDlg.GetFileExt();
    if (!strTemp.Compare(_T("bmp")))
    {
        pExt = _T("bmp");
        pData = m_pbRgbData;
        nSize = m_iRgbSize;
    }

    cFile.Open(fileDlg.GetFileName(), CFile::modeWrite|CFile::modeCreate);
    cFile.Write(pData, nSize);
    cFile.Close();
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
    OnBnClickedButtonFirst();
}


void CYUVPlayerDlg::OnBnClickedButtonPrev()
{
    m_nCurrentFrame--;
    if (m_nCurrentFrame <= 1)
    {
        m_nCurrentFrame = 1;
        return;
    }
    this->Read(m_nCurrentFrame);
    this->Show();
}


void CYUVPlayerDlg::OnBnClickedButtonNext()
{
    m_nCurrentFrame++;

    if (m_nCurrentFrame >= m_nTotalFrame)
    {
        m_nCurrentFrame = m_nTotalFrame;

    }
    this->Read(m_nCurrentFrame);
    this->Show();
}


void CYUVPlayerDlg::OnBnClickedButtonFirst()
{
    m_nCurrentFrame = 1;
    this->Read(m_nCurrentFrame);
    this->Show();
}


void CYUVPlayerDlg::OnBnClickedButtonLast()
{
    m_nCurrentFrame = m_nTotalFrame;
    this->Read(m_nCurrentFrame);
    this->Show();
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
    if (!m_fInit) return;
//    if (cx<=333 || cy <= 343) return;

    CDialogEx::OnSize(nType, cx, cy);

    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, cx, cy-26-20-21-4-4);
        pWnd->Invalidate();
        pWnd->UpdateData();
    }
    // ���ұߵİ�ť
    pWnd = GetDlgItem(m_nStartX[0][10]);
    if (pWnd)
    {
       pWnd->SetWindowPos(NULL,cx-30-2,cy-26-4,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }

    int startx = 2;
    // ֡��ͳ��
    pWnd = GetDlgItem(m_nStartX[0][0]);
    if (pWnd)
    {
        pWnd->SetWindowPos(NULL,startx+m_nStartX[1][0],cy-26-20-21-4,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }
    // ������
    pWnd = GetDlgItem(m_nStartX[0][1]);
    if (pWnd)
    {
        //pWnd->MoveWindow(startx+0, cy-26-26, cx, cy-26-20-21);
        //pWnd->Invalidate();
        pWnd->SetWindowPos( NULL,startx+m_nStartX[1][1],cy-26-26-4,cx,26,SWP_NOZORDER);
    }
    // ˮƽλ����ͬ�İ�ť
    for (int i = 2; i < 10; i++)
    {
        pWnd = GetDlgItem(m_nStartX[0][i]);
        if (pWnd)
        {
            pWnd->SetWindowPos(NULL,startx+m_nStartX[1][i],cy-26-4,0,0,SWP_NOZORDER|SWP_NOSIZE);
        }
    }
}


void CYUVPlayerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialogEx::OnSizing(fwSide, pRect);
}

void CYUVPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
    // note ��m_strPathName�Ļ����رմ��ں󣬽��̻���
    CDialogEx::OnDropFiles(hDropInfo);
    wchar_t* pFilePathName =(wchar_t *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName, MAX_URL_LENGTH);
    m_strPathName.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);

    // ��ʾ����
    CString strTemp;
    strTemp.Format(_T("%s-%s"), pFilePathName, APP_NAM);
    this->SetWindowText(strTemp);

    // ���ļ���
    wchar_t* tmp = wcsrchr(pFilePathName, '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);
    free(pFilePathName);

    m_pSettingDlg->ParseFilename(szFilename);
    m_pSettingDlg->SetParametersToParentWnd(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    // ��ʾ��һ֡
    ShowOpenedFrame();
#if 0
    save_yuv_file("rainbow_176x144_yuyv.yuv", 176, 144, FMT_YUYV);
    save_yuv_file("rainbow_176x144_yvyu.yuv", 176, 144, FMT_YVYU);
    save_yuv_file("rainbow_176x144_uyvy.yuv", 176, 144, FMT_UYVY);
    save_yuv_file("rainbow_176x144_vyuy.yuv", 176, 144, FMT_VYUY);
    save_yuv_file("rainbow_176x144_yuv444.yuv", 176, 144, FMT_YUV444);
#endif

    //change_yuv_file("suzie_qcif_yuv420p_00.yuv", "suzie_qcif_nv21.yuv", 176, 144, 0);
    //change_yuv_file("suzie_qcif_sp.yuv", "suzie_qcif_p.yuv", 176, 144, 1);
    //change_yuv_file("suzie_qcif_yuv422p.yuv", "suzie_qcif_nv61.yuv", 176, 144, 2);
}

/////////////////////////////
// �ڲ�ʵ��
void CYUVPlayerDlg::Open()
{
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
}

BOOL CYUVPlayerDlg::IsOpen()
{
    if (CFile::hFileNull != m_cFile.m_hFile)
        return TRUE;
    else
        return FALSE;
}

void CYUVPlayerDlg::Malloc()
{
    // ����YUV��ʽ�����ڴ�
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
    case FMT_YUV444:
        m_iYuvSize = m_nWidth * m_nHeight * 3;
        break;
    case FMT_Y:
        m_iYuvSize = m_nWidth * m_nHeight;
        break;
    default:
        break;
    }

    m_iRgbSize = m_nWidth * m_nHeight * 3 + 54;

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

    m_pbYuvData = new char[m_iYuvSize];
    m_pbRgbData  = new char[m_iRgbSize];

    m_nTotalFrame = (UINT)(m_cFile.GetLength() / m_iYuvSize);
    this->ShowFrameCount(m_nCurrentFrame);
}

void CYUVPlayerDlg::Read(INT nCurrentFrame)
{
    this->ShowFrameCount(nCurrentFrame); // ��һ֡ʱ��˳����ʾ����ǰ֡
    m_cFile.Seek(m_iYuvSize * (nCurrentFrame - 1), SEEK_SET);
    m_cFile.Read(m_pbYuvData, m_iYuvSize);
}

void CYUVPlayerDlg::Show()
{
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
    // BMP��BGR��ʽ�ģ�rgb->bgr
    swargb((BYTE*)m_pbRgbData+54, m_iRgbSize-54);
    // ��ʾ
    ShowPicture((BYTE *)m_pbRgbData, m_iRgbSize);
}

void CYUVPlayerDlg::ShowFrameCount(int nCurrentFrame)
{
    // ��ʾ��ǰ֡/��֡��
    CString strTemp;
    strTemp.Format(_T("%d/%d"), nCurrentFrame, m_nTotalFrame);
    GetDlgItem(IDC_STATIC_FRAMECNT)->SetWindowText(strTemp);
}

// ���ļ� ʱ��ʾ��һ֡
void CYUVPlayerDlg::ShowOpenedFrame()
{
    m_nCurrentFrame = 1;

    this->Open();
    this->Malloc();
    
    this->Read(m_nCurrentFrame);
    this->Show();

    m_bSaveFrame.EnableWindow(TRUE);
    m_bPlay.EnableWindow(TRUE);
    //m_bStop.EnableWindow(TRUE);
    m_bPrevFrame.EnableWindow(TRUE);
    m_bNextFrame.EnableWindow(TRUE);
    m_bFirstFrame.EnableWindow(TRUE);
    m_bLastFrame.EnableWindow(TRUE);
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

void CYUVPlayerDlg::SetParentParameters(int width, int height, int fps, int fmt, BOOL loop)
{
    m_nWidth = width;
    m_nHeight = height;
    m_nFps = fps;
    m_nYuvFormat = fmt;
    m_fLoop = loop;

    // �ָ�ԭʼ״̬
    if (!IsOpen()) return;

    ShowOpenedFrame();
}

// �����߳�
UINT Play(LPVOID pParam)
{
    CYUVPlayerDlg* pWin = (CYUVPlayerDlg *)pParam;  // �Ի�����

    int iTimeSpan = 1000 / pWin->m_nFps;

    HANDLE hPlay = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Play"));

    if (!hPlay)
    {
        CString msg;
        //printErrorMessage("Open Mutex failed.", msg);
        MessageBox(pWin->m_hWnd, msg, NULL, MB_OK);
    }

    while (pWin->m_nCurrentFrame <= pWin->m_nTotalFrame)
    {
        DWORD t1 = GetTickCount();

        if (WAIT_OBJECT_0 == WaitForSingleObject(hPlay,INFINITE))
        {
            ReleaseMutex(hPlay);
        }

        pWin->Read(pWin->m_nCurrentFrame);
        pWin->Show();

        DWORD t2 = GetTickCount();
        int t = t2 - t1;
        if (t < iTimeSpan)
            Sleep(iTimeSpan - t);

        pWin->m_nCurrentFrame++;
    }
    pWin->m_nCurrentFrame--; // ����ѭ��������m_nCurrentFrame���һ��
    pWin->m_bStop.EnableWindow(FALSE);
    pWin->m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));

    pWin->m_pWinThread = NULL;
    AfxEndThread(0);

    return 0;
}
