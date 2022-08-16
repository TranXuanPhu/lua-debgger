#pragma once
#include "CallFuntionDlg.h"

#define CLOUMN_TABLE	0
#define CLOUMN_VALUES	1
#define CLOUMN_TYPE		2


typedef struct _TREEROOT{
	CHAR czNameRoot[MAX_PATH];
	HTREEITEM hRoot;

}TREEROOT;


// CDebuggersDlg dialog

class  CDebuggersDlg : public CAIGDialog
{
	DECLARE_DYNAMIC(CDebuggersDlg)

public:
	CDebuggersDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDebuggersDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEBGGERS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CString m_szPrevNameCallFunc;

public:
	HTREEITEM FindItem(CString name, HTREEITEM hRoot);
	void InsertItem(LPCSTR lpName, int type, LPCSTR values, LPCSTR lpfullPath);
	void InsertItem(LPCSTR lpName, int type, LPCSTR values, HTREEITEM hParent);
	void InsertRootInfo(int type, LPCSTR values, HTREEITEM hRoot);
	void SortChildren(CString czRoot);
	void ShowBtnRunCommand(BOOL bShow = TRUE);
	CString GetPathByItem(HTREEITEM hItem);
	BOOL	IsTypeInPath(HTREEITEM hItem,int type = 7);//LUA_TUSERDATA
	BOOL	IsMetatableInPath(HTREEITEM hItem);
	void DelAllChildren(HTREEITEM hParent);
	BOOL ShowTable(CString czFullPath, HTREEITEM hParent);
	BOOL ShowUserData(CString czFullPath, HTREEITEM hParent);

	void AddTreeRoot(LPCSTR lpNameRoot, HTREEITEM hRoot);
	HTREEITEM AddTreeRoot(LPCSTR lpNameRoot);
	HTREEITEM GetHTreeTable(LPCSTR lpfullPath);
	BOOL	 IsTreeRoot(HTREEITEM hRoot);
	CPtrList m_lstTreeRoot;


	CAIGTreeListCtrl m_lstTreeCtrlLua;
	CAIGEdit m_edtGetTab;
	CAIGButton m_btnRunCommand;
	CAIGButton m_btnGetAddrBase;
	CAIGComboBox m_cbxVersionDll;
	CAIGComboBox m_cbxNameDll;
	afx_msg void OnBnClickedButtonGetAddrBase();
	afx_msg void OnNMDblclkTreeLua(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRunCommand();
	afx_msg void OnNMRClickTreeLua(NMHDR *pNMHDR, LRESULT *pResult);
	CAIGCheckBox m_btnTest;
	afx_msg void OnBnClickedCheckTest();
	CAIGComboBox m_cbxModeRunCommand;
	CAIGEdit m_edtScript;
	CAIGButton m_btnDoScript;
	afx_msg void OnBnClickedButtonDoScript();
	afx_msg void OnNMClickTreeLua(NMHDR *pNMHDR, LRESULT *pResult);


	/*----------transform-------*/
	/*CAIGTreeListCtrl m_lstTreeCtrlTransform;
	afx_msg void OnNMDblclkTreeTransform(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTreeTransform(NMHDR *pNMHDR, LRESULT *pResult);*/
	CAIGCheckBox m_chkShowTreeTransform;
	afx_msg void OnBnClickedCheckShowTreeTransform();
};
