// SettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "YUVPlayerDlg.h"

// CSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{
    m_pParentWnd = NULL;
    m_strAddedSize = _T("160x120;176x144;320x240;352x288;640x480;704x576");
    m_nWidth = 176;
    m_nHeight = 144;
    m_nFps = 30;
    m_nFpsIndex = 7; // index 7 == 30
    m_nYuvFormat = 1;   // tmpe yuv420
    m_fLoop = FALSE;
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //  DDX_Text(pDX, IDC_E_WIDTH, m_nWidth);
    //  DDX_Control(pDX, IDC_CB_YUV_FMT, m_cbResolution);
    DDX_Control(pDX, IDC_CB_FPS, m_cbFps);
    DDX_Control(pDX, IDC_CB_YUV_FMT, m_cbYuvFormat);
    DDX_Control(pDX, IDC_CB_SIZE, m_cbResolution);
    DDX_Text(pDX, IDC_E_HEIGHT, m_nHeight);
    DDV_MinMaxInt(pDX, m_nHeight, 1, 65535);
    DDX_Text(pDX, IDC_E_WIDTH, m_nWidth);
    DDV_MinMaxInt(pDX, m_nWidth, 1, 65535);
    DDX_Check(pDX, IDC_CK_LOOP, m_fLoop);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
    ON_BN_CLICKED(IDC_APPLY, &CSettingDlg::OnBnClickedApply)
    ON_BN_CLICKED(IDCANCEL, &CSettingDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_CB_SIZE, &CSettingDlg::OnSelchangeCbSize)
    ON_CBN_SELCHANGE(IDC_CB_YUV_FMT, &CSettingDlg::OnSelchangeCbYuvFmt)
    ON_CBN_SELCHANGE(IDC_CB_FPS, &CSettingDlg::OnSelchangeCbFps)
    ON_BN_CLICKED(IDC_CK_LOOP, &CSettingDlg::OnClickedCkLoop)
    ON_EN_CHANGE(IDC_E_WIDTH, &CSettingDlg::OnChangeEWidth)
    ON_EN_CHANGE(IDC_E_HEIGHT, &CSettingDlg::OnChangeEHeight)
    ON_BN_CLICKED(IDC_BT_ADD, &CSettingDlg::OnBnClickedBtAdd)
    ON_BN_CLICKED(IDC_BT_DEL, &CSettingDlg::OnBnClickedBtDel)
END_MESSAGE_MAP()

BOOL CSettingDlg::ExistRegistration()
{
    HKEY hKey;
    int ret = 0;
    ret = RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Software\\YUVPlayer-latelee.org"), 0, KEY_ALL_ACCESS, &hKey);
    RegCloseKey(hKey);
    return !ret;
}

void CSettingDlg::SetRegistration(CString& strSize, int width, int height, int fpsidx, int fmt, int loop)
{
    HKEY hKey;

    RegCreateKey(HKEY_CURRENT_USER, _T("Software\\YUVPlayer-latelee.org\\Setting"), &hKey);
    RegSetValueEx(hKey, _T("AddedSize"), 0, REG_SZ, (BYTE*)(strSize.GetBuffer()), strSize.GetLength()*sizeof(TCHAR));
    RegSetValueEx(hKey, _T("FrameWidth"), 0, REG_DWORD, (BYTE*)&width, 4);
    RegSetValueEx(hKey, _T("FrameHeight"), 0, REG_DWORD, (BYTE*)&height, 4);
    RegSetValueEx(hKey, _T("FrameRate"), 0, REG_DWORD, (BYTE*)&fpsidx, 4);
    RegSetValueEx(hKey, _T("CurrentPixelFormat"), 0, REG_DWORD, (BYTE*)&fmt, 4);
    RegSetValueEx(hKey, _T("Loop"), 0, REG_DWORD, (BYTE*)&loop, 4);

    RegCloseKey(hKey);
}

void CSettingDlg::GetRegistration(CString& strSize, int& width, int& height, int& fpsidx, int& fmt, int& loop)
{
    HKEY hKey;
    DWORD dwType = 0;
    DWORD dwLen = 1024; // �˴�ֵ�Ƿ��ʺϣ�CString������θ������ȣ�
    //BYTE szBuffer[1024] = {0};

    RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Software\\YUVPlayer-latelee.org\\Setting"), 0, KEY_QUERY_VALUE, &hKey);
    RegQueryValueEx(hKey, _T("AddedSize"), NULL, &dwType, (BYTE*)(strSize.GetBuffer()), &dwLen);
    RegQueryValueEx(hKey, _T("FrameWidth"), NULL, &dwType, (BYTE*)&width, &dwLen);
    RegQueryValueEx(hKey, _T("FrameHeight"), NULL, &dwType, (BYTE*)&height, &dwLen);
    RegQueryValueEx(hKey, _T("FrameRate"), NULL, &dwType, (BYTE*)&fpsidx, &dwLen);
    RegQueryValueEx(hKey, _T("CurrentPixelFormat"), NULL, &dwType, (BYTE*)&fmt, &dwLen);
    RegQueryValueEx(hKey, _T("Loop"), NULL, &dwType, (BYTE*)&loop, &dwLen);

    RegCloseKey(hKey);
}
// CSettingDlg ��Ϣ�������

BOOL CSettingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  ʹ��ע������ֵ
    // 
    CString strTemp;

    // default
    //if (!ExistRegistration())
    if (1)
    {
        SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
    }
    // read
    GetRegistration(m_strAddedSize, m_nWidth, m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
#if 0
    // debug
    CString strMsg;
    strMsg.Format(_T("%s %d %d %d %d %d"), m_strAddedSize, m_nWidth, m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
    MessageBox(strMsg);
#endif

    int pos = 0;  
    strTemp = m_strAddedSize.Tokenize(_T(";"), pos);
    m_strArrAddedSize.Add(strTemp);
    while (strTemp != _T(""))
    {
        strTemp = m_strAddedSize.Tokenize(_T(";"), pos);
        m_strArrAddedSize.Add(strTemp);
    }

    int nResolutionIdx = -1;
    for (int i = 0; i < m_strArrAddedSize.GetCount()-1; i++)
    {
        m_cbResolution.AddString(m_strArrAddedSize[i]);
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            nResolutionIdx = i;
    }

    m_cbYuvFormat.SetCurSel(m_nYuvFormat);
    m_cbFps.SetCurSel(m_nFpsIndex);
    m_cbResolution.SetCurSel(nResolutionIdx);
    m_nYuvFormat = m_cbYuvFormat.GetCurSel();
    m_cbResolution.GetWindowText(strTemp);
    //swscanf_s(strTemp.GetBuffer(), _T("%dx%d"), &m_nWidth, &m_nHeight);
    m_cbFps.GetWindowText(strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%d"), &m_nFps);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

    
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

// Ӧ�ã���Ҫ�رմ���
void CSettingDlg::OnBnClickedApply()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData();
    int nResolutionIdx = -1;
    for (int i = 0; i < m_strArrAddedSize.GetCount()-1; i++)
    {
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            nResolutionIdx = i;
    }

    m_cbResolution.SetCurSel(nResolutionIdx);

    m_pParentWnd->GetParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}


void CSettingDlg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CDialogEx::OnCancel();
}


void CSettingDlg::OnBnClickedOk()
{
    m_pParentWnd->GetParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
    CDialogEx::OnOK();
}


void CSettingDlg::OnSelchangeCbSize()
{
    CString strTemp;
    m_cbResolution.GetLBText(m_cbResolution.GetCurSel(), strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%dx%d"), &m_nWidth, &m_nHeight);

    UpdateData(FALSE);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnSelchangeCbYuvFmt()
{
    m_nYuvFormat = m_cbYuvFormat.GetCurSel();
    //UpdateData(FALSE);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnSelchangeCbFps()
{
    CString strTemp;
    m_cbFps.GetLBText(m_cbFps.GetCurSel(), strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%d"), &m_nFps);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnClickedCkLoop()
{
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CK_LOOP);
    m_fLoop = pBtn->GetCheck();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnChangeEWidth()
{
    // TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
    // ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
    // ���������� CRichEditCtrl().SetEventMask()��
    // ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

    // TODO:  �ڴ���ӿؼ�֪ͨ����������
    UpdateData();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnChangeEHeight()
{
    // TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
    // ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
    // ���������� CRichEditCtrl().SetEventMask()��
    // ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

    // TODO:  �ڴ���ӿؼ�֪ͨ����������
    UpdateData();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

// ��ע������һ�ֱַ���
void CSettingDlg::OnBnClickedBtAdd()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

// ��ע���ɾ��һ�ֱַ���
void CSettingDlg::OnBnClickedBtDel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}
