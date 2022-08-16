
// DbgUIDlg.h : header file
//

#pragma once
#define		GAMELIST_INDEX_CONTROL		0
#define		GAMELIST_INDEX_NAME			1
#define		GAMELIST_INDEX_STATUS		2

// CDbgUIDlg dialog
class CDbgUIDlg : public CAIGDialog
{
// Construction
public:
	CDbgUIDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DBGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support



// Implementation
protected:
	HICON m_hIcon;
	CWinThread*	m_pThreadFindGame;
	CEvent		m_evThreadFindGame;
	static UINT _ThreadFindGame(LPVOID pData);
	void	OnThreadFindGame();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
public:
	afx_msg LRESULT OnGameEvents(WPARAM wParam, LPARAM lParam);
	void			OnEventGameInit(CGameInfo* pGame);
	void			AddNewGame(CGameInfo* pGame);
	static VOID CALLBACK _TimeGameUpdate(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void				 OnTimeGameUpdate(CGameInfo* pGame);
	afx_msg void		OnLvnItemCheckListGame(NMHDR *pNMHDR, LRESULT *pResult);

public:
	CAIGListCtrl m_lstCtrlGame;
	afx_msg void OnNMClickListGame(NMHDR *pNMHDR, LRESULT *pResult);
};
