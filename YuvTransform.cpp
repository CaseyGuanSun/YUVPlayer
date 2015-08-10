// YuvTransform.cpp : implementation file
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "YuvTransform.h"
#include "afxdialogex.h"

#include "SettingDlg.h"

#include "yuv2rgb.h"

#ifndef MAX_URL_LENGTH
#define MAX_URL_LENGTH 256
#endif
// CYuvTransform dialog

IMPLEMENT_DYNAMIC(CYuvTransform, CDialogEx)

CYuvTransform::CYuvTransform(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYuvTransform::IDD, pParent)
    , m_strPathName(_T(""))
    , m_nWidth(0)
    , m_nHeight(0)
{
    m_pbYuvData = NULL;
    m_pbOutputData = NULL;
}

CYuvTransform::~CYuvTransform()
{
}

void CYuvTransform::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_E_INPUT, m_strPathName);
    DDX_Text(pDX, IDC_E_WIDTH, m_nWidth);
    DDX_Text(pDX, IDC_E_HEIGHT, m_nHeight);
    DDX_Control(pDX, IDC_CB_INFMT, m_cbInput);
    DDX_Control(pDX, IDC_CB_OUTFMT, m_cbOutput);
}


BEGIN_MESSAGE_MAP(CYuvTransform, CDialogEx)
    ON_BN_CLICKED(IDC_B_OPEN, &CYuvTransform::OnBnClickedBOpen)
    ON_BN_CLICKED(IDC_B_COMBINE, &CYuvTransform::OnBnClickedBCombine)
    ON_BN_CLICKED(IDC_B_SPLIT, &CYuvTransform::OnBnClickedBSplit)
    ON_BN_CLICKED(IDC_B_TRANSFORM, &CYuvTransform::OnBnClickedBTransform)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()


void CYuvTransform::OnBnClickedBOpen()
{
    wchar_t szFilter[] = _T("YUV Files(*.yuv;*.raw)|*.yuv;*.raw|All Files(*.*)|*.*||");
    CFileDialog fileDlg(TRUE, _T("YUV"), NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("Open YUV File");   // ����
    if (fileDlg.DoModal() != IDOK)
        return;

    m_strPathName = fileDlg.GetPathName();
    // ���ļ���
    wchar_t* tmp = wcsrchr(m_strPathName.GetBuffer(), '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);
    ParseFilename(szFilename);

    UpdateData(FALSE);
}


void CYuvTransform::OnBnClickedBCombine()
{
    // TODO: Add your control notification handler code here
}


void CYuvTransform::OnBnClickedBSplit()
{
    // TODO: Add your control notification handler code here
}


void CYuvTransform::OnBnClickedBTransform()
{
    UpdateData();
    


    Open();
    Malloc();

    Read(1);
    Transform();
    m_strOutputFile = _T("aaa_nv16.yuv");
    m_cOutputFile.Open(m_strOutputFile.GetBuffer(), CFile::modeWrite|CFile::modeCreate);
    m_cOutputFile.Write(m_pbOutputData, m_iOutputSize);
    m_cOutputFile.Close();
}


void CYuvTransform::OnDropFiles(HDROP hDropInfo)
{
    CDialogEx::OnDropFiles(hDropInfo);

    wchar_t* pFilePathName =(wchar_t *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName, MAX_URL_LENGTH);
    m_strPathName.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);

    // ���ļ���
    wchar_t* tmp = wcsrchr(pFilePathName, '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);
    free(pFilePathName);

    ParseFilename(szFilename);

    UpdateData(FALSE);
}

void CYuvTransform::ParseFilename(const char* pFilename)
{
    int nYuvFormat = 0;
    int nWidth = 0;
    int nHeight = 0;

    find_resolution((char*)pFilename, nYuvFormat, nWidth, nHeight);

    // ����ԭ����ֵ
    if (nYuvFormat != -1)
    {
        m_nYuvFormat = nYuvFormat;
    }
    if (nWidth > 0 && nHeight > 0)
    {
        m_nWidth = nWidth;
        m_nHeight = nHeight;
    }

    m_cbInput.SetCurSel(m_nYuvFormat);
    UpdateData(FALSE);
}

// �ڲ�ʵ��
void CYuvTransform::Open()
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

BOOL CYuvTransform::IsOpen()
{
    if (CFile::hFileNull != m_cFile.m_hFile)
        return TRUE;
    else
        return FALSE;
}

void CYuvTransform::Malloc()
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

    m_iOutputSize = m_iYuvSize;

    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbOutputData != NULL)
    {
        delete[] m_pbOutputData;
        m_pbOutputData = NULL;
    }

    m_pbYuvData = new char[m_iYuvSize];
    m_pbOutputData  = new char[m_iOutputSize];

    m_nTotalFrame = (UINT)(m_cFile.GetLength() / m_iYuvSize);

}

void CYuvTransform::Read(INT nCurrentFrame)
{
    m_cFile.Seek(m_iYuvSize * (nCurrentFrame - 1), SEEK_SET);
    m_cFile.Read(m_pbYuvData, m_iYuvSize);
}

void CYuvTransform::Transform()
{
    int nOutput = -1;
    nOutput = m_cbOutput.GetCurSel();

    if (m_nYuvFormat == FMT_YUV422 && nOutput == FMT_NV16)
    {
        yuv422p_to_yuv422sp((unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else
    {
        MessageBox(_T("Sorry, can not do it."));
    }
}