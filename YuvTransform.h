#pragma once
#include "afxwin.h"


// CYuvTransform dialog

class CYuvTransform : public CDialogEx
{
	DECLARE_DYNAMIC(CYuvTransform)

public:
	CYuvTransform(CWnd* pParent = NULL);   // standard constructor
	virtual ~CYuvTransform();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRANSFORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
private:
    int m_nYuvFormat;
    char*   m_pbYuvData;    // YUV����
    char*   m_pbOutputData;    // RGB����
    UINT    m_iYuvSize; // һ��ͼ���С
    UINT    m_iOutputSize; // BMPͼ�Ĵ�С
    INT     m_nTotalFrame;
    INT     m_nCurrentFrame;
    CFile m_cFile;

public:
    void ParseFilename(const char* pFilename);
    void Open();
    BOOL IsOpen();
    void Malloc();
    void Read(INT nCurrentFrame);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBOpen();
    afx_msg void OnBnClickedBCombine();
    afx_msg void OnBnClickedBSplit();
    afx_msg void OnBnClickedBTransform();
    CString m_strPathName;
    afx_msg void OnDropFiles(HDROP hDropInfo);
    UINT m_nWidth;
    UINT m_nHeight;
    CComboBox m_cbInput;
    CComboBox m_cbOutput;
};
