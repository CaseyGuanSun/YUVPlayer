#pragma once

class CYUVPlayerDlg;

// CSettingDlg �Ի���

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSettingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SETTING };

public:
    void SetParentWnd(CYUVPlayerDlg* pWnd) {m_pParentWnd = pWnd;}

    void SetParameters(CString& strSize, int width, int height, int fpsidx, int fmt, BOOL loop)
    {
        m_strAddedSize = strSize;
        m_nWidth = width;
        m_nHeight = height;
        m_nFpsIndex = fpsidx;
        m_nYuvFormat = fmt;
        m_fLoop = loop;
        
        //UpdateWindow(FALSE);
    }

    void SetRegistration(CString& strSize, int width, int height, int fpsidx, int fmt, int loop);
    void GetRegistration(CString& strSize, int& width, int& height, int& fpsidx, int& fmt, int& loop);
    BOOL ExistRegistration();

private:
    CYUVPlayerDlg *m_pParentWnd; // ���ڲ�������
    CString m_strAddedSize;   // �û��Զ���ķֱ���(��ϵͳ���е�)
    int m_nFpsIndex;
    CStringArray m_strArrAddedSize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedApply();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    CComboBox m_cbFps;
    CComboBox m_cbYuvFormat;
    CComboBox m_cbResolution;
    int m_nHeight;
    int m_nWidth;
    int m_nFps;
    int m_nYuvFormat;
    BOOL m_fLoop;
    afx_msg void OnSelchangeCbSize();
    afx_msg void OnSelchangeCbYuvFmt();
    afx_msg void OnSelchangeCbFps();
    afx_msg void OnClickedCkLoop();
    afx_msg void OnChangeEWidth();
    afx_msg void OnChangeEHeight();
    afx_msg void OnBnClickedBtAdd();
    afx_msg void OnBnClickedBtDel();
};
