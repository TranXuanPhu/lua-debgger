#pragma once
#define ARGSLIST_INDEX_INDEX		0
#define ARGSLIST_INDEX_VALUES		1
#define ARGSLIST_INDEX_TYPE			2
#define ARGSLIST_INDEX_NAMEARGS		3

// CCallFuntionDlg dialog

class CCallFuntionDlg : public CAIGDialog
{
	DECLARE_DYNAMIC(CCallFuntionDlg)

public:																	//LUA_TTABLE
	CCallFuntionDlg(CWnd* pParent = nullptr,CString czPathFunc = L"",int ParentType = 5 ,BOOL bUserDataPrev = FALSE);   // standard constructor
	virtual ~CCallFuntionDlg();
	virtual BOOL OnInitDialog();
	CString m_czNameFunc;
	CString m_czPathFunc;
	CString m_czPathParentFunc;
	BOOL m_bUserDataPrev;
	int	m_ParentType;

	void UpdateArgs(BOOL bUserDataPrev = FALSE);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALL_FUC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CAIGListCtrl m_lstCtrlArgs;
	CAIGEdit m_edtnArgs;
	CAIGButton m_btnCallFunc;
	CAIGComboBox m_cbxValueType;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonCall();
	afx_msg void OnLvnExEditChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboType();
	CAIGLabel m_lblNote;
	CAIGComboBox m_cbxResultType;
	CAIGButton m_btnShowInfoFunc;
	afx_msg void OnBnClickedButtonShowInfo();
	afx_msg void OnStnClickedLblNote();
};
