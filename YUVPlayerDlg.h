
// YUVPlayerDlg.h : ͷ�ļ�
//

#pragma once

#include "SettingDlg.h"

// CYUVPlayerDlg �Ի���
class CYUVPlayerDlg : public CDialogEx
{
// ����
public:
	CYUVPlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��
    ~CYUVPlayerDlg();
// �Ի�������
	enum { IDD = IDD_YUVPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
    BOOL m_fInit;

    CSettingDlg* m_pSettingDlg;
    int m_nWidth;
    int m_nHeight;
    int m_nFps;
    int m_nYuvFormat;
    BOOL m_fLoop;

    int m_nStartX[2][11];

public:
    void GetParameters(int width, int height, int fps, int fmt, BOOL loop)
    {
        m_nWidth = width;
        m_nHeight = height;
        m_nFps = fps;
        m_nYuvFormat = fmt;
        m_fLoop = loop;
    }

    void ShowSettingWindow();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSet();
    afx_msg void OnBnClickedButtonOpen();
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonPlay();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

    afx_msg void OnFileOpen();
    afx_msg void OnFileClose();
    afx_msg void OnFileSave();
    afx_msg void OnFileExit();
    afx_msg void OnPlayingPlay();
    afx_msg void OnPlayingSetting();
    afx_msg void OnPlayingStop();
    afx_msg void OnHelpHelp();
    afx_msg void OnHelpAbout();
    afx_msg void OnROpen();
    afx_msg void OnRSetting();
    afx_msg void OnRExit();
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedButtonPrev();
    afx_msg void OnBnClickedButtonNext();
    afx_msg void OnBnClickedButtonFirst();
    afx_msg void OnBnClickedButtonLast();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};
