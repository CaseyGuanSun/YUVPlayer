
// YUVPlayerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "YUVPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    if (m_pSettingDlg == NULL)
    {
        m_pSettingDlg = new CSettingDlg();
        m_pSettingDlg->Create(IDD_DIALOG_SETTING, this);
        m_pSettingDlg->SetParentWnd(this);
    }

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
    CRect rtTop;
    CStatic *pWnd = (CStatic*)GetDlgItem(IDC_VIDEO);
    CDC *cDc = pWnd->GetDC();
    pWnd->GetClientRect(&rtTop);
    cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(),RGB(0,0,0));
    Invalidate(FALSE);
#endif

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
    //m_pParentWnd->CenterWindow(this);
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

    pPopup=popMenu.GetSubMenu(0); //����Ӳ˵�ָ��

    pPopup->EnableMenuItem(ID_R_OPEN, MF_BYCOMMAND|MF_ENABLED); //����˵���ʹ��
    pPopup->EnableMenuItem(ID_R_SETTING, MF_BYCOMMAND|MF_ENABLED); //����˵���ʹ��
    pPopup->EnableMenuItem(ID_R_EXIT, MF_BYCOMMAND|MF_ENABLED); //����˵���ʹ��

    //pPopup->EnableMenuItem(ID_R_SETTING,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED); //������˵���ʹ��
    pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this); //��ʾ�����˵�
    pPopup->Detach();
    popMenu.DestroyMenu();
}

/////////////////////////////////////////// ------> ����˵��¼�
void CYUVPlayerDlg::OnFileOpen()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnFileClose()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnFileSave()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnFileExit()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnPlayingPlay()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnPlayingSetting()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnPlayingStop()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnHelpHelp()
{
    // TODO: Add your command handler code here
    MessageBox(_T("Help me"));
}


void CYUVPlayerDlg::OnHelpAbout()
{
    // TODO: �ڴ���������������
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}
/////////////////////////////////////////// <------ ����˵��¼�

/////////////////////////////////////////// ------> �Ҽ��˵��¼�
void CYUVPlayerDlg::OnROpen()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnRSetting()
{
    // TODO: Add your command handler code here
}


void CYUVPlayerDlg::OnRExit()
{
    // TODO: Add your command handler code here
}

/////////////////////////////////////////// <------ �Ҽ��˵��¼�

/////////////////////////////////////////// ------> ������ť�¼�
void CYUVPlayerDlg::OnBnClickedButtonOpen()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������

    int aa =  m_nWidth;

    return;
}


void CYUVPlayerDlg::OnBnClickedButtonSave()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CYUVPlayerDlg::OnBnClickedButtonPlay()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CYUVPlayerDlg::OnBnClickedButtonStop()
{
    // TODO: Add your control notification handler code here
}


void CYUVPlayerDlg::OnBnClickedButtonPrev()
{
    // TODO: Add your control notification handler code here
}


void CYUVPlayerDlg::OnBnClickedButtonNext()
{
    // TODO: Add your control notification handler code here
}


void CYUVPlayerDlg::OnBnClickedButtonFirst()
{
    // TODO: Add your control notification handler code here
}


void CYUVPlayerDlg::OnBnClickedButtonLast()
{
    // TODO: Add your control notification handler code here
}


void CYUVPlayerDlg::OnBnClickedButtonSet()
{
    ShowSettingWindow();
}

/////////////////////////////////////////// <------ ������ť�¼�

/////////////////////////////////////////// ------> ���̿�ݼ��¼�


