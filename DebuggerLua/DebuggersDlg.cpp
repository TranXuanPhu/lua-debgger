// DebuggersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DebuggerLua.h"
#include "DebuggersDlg.h"
#include "afxdialogex.h"


// CDebuggersDlg dialog

IMPLEMENT_DYNAMIC(CDebuggersDlg, CAIGDialog)

CDebuggersDlg::CDebuggersDlg(CWnd* pParent /*=nullptr*/)
	: CAIGDialog(IDD_DIALOG_DEBGGERS, pParent)
{

}

CDebuggersDlg::~CDebuggersDlg()
{
	while (!m_lstTreeRoot.IsEmpty())
	{
		TREEROOT* p =(TREEROOT*)m_lstTreeRoot.GetHead();
		m_lstTreeRoot.RemoveHead();
		delete p;
	}
}

void CDebuggersDlg::DoDataExchange(CDataExchange* pDX)
{
	CAIGDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_LUA, m_lstTreeCtrlLua);
	DDX_Control(pDX, IDC_EDIT_TAB, m_edtGetTab);
	DDX_Control(pDX, IDC_BUTTON_RUN_COMMAND, m_btnRunCommand);
	DDX_Control(pDX, IDC_BUTTON_GET_ADDR_BASE, m_btnGetAddrBase);
	DDX_Control(pDX, IDC_COMBO_VERSION_DLL, m_cbxVersionDll);
	DDX_Control(pDX, IDC_COMBO_DLL, m_cbxNameDll);
	DDX_Control(pDX, IDC_CHECK_Test, m_btnTest);
	DDX_Control(pDX, IDC_COMBO_MODE_RUN_COMMAND, m_cbxModeRunCommand);
	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_edtScript);
	DDX_Control(pDX, IDC_BUTTON_DO_SCRIPT, m_btnDoScript);
	//DDX_Control(pDX, IDC_TREE_TRANSFORM, m_lstTreeCtrlTransform);
	DDX_Control(pDX, IDC_CHECK_SHOW_TREE_TRANSFORM, m_chkShowTreeTransform);
}


BEGIN_MESSAGE_MAP(CDebuggersDlg, CAIGDialog)
	ON_BN_CLICKED(IDC_BUTTON_GET_ADDR_BASE, &CDebuggersDlg::OnBnClickedButtonGetAddrBase)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_LUA, &CDebuggersDlg::OnNMDblclkTreeLua)
	ON_BN_CLICKED(IDC_BUTTON_RUN_COMMAND, &CDebuggersDlg::OnBnClickedButtonRunCommand)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_LUA, &CDebuggersDlg::OnNMRClickTreeLua)
	ON_BN_CLICKED(IDC_CHECK_Test, &CDebuggersDlg::OnBnClickedCheckTest)
	ON_BN_CLICKED(IDC_BUTTON_DO_SCRIPT, &CDebuggersDlg::OnBnClickedButtonDoScript)
	ON_NOTIFY(NM_CLICK, IDC_TREE_LUA, &CDebuggersDlg::OnNMClickTreeLua)
	//ON_NOTIFY(NM_DBLCLK, IDC_TREE_TRANSFORM, &CDebuggersDlg::OnNMDblclkTreeTransform)
	//ON_NOTIFY(NM_RCLICK, IDC_TREE_TRANSFORM, &CDebuggersDlg::OnNMRClickTreeTransform)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TREE_TRANSFORM, &CDebuggersDlg::OnBnClickedCheckShowTreeTransform)
END_MESSAGE_MAP()


// CDebuggersDlg message handlers


BOOL CDebuggersDlg::OnInitDialog()
{
	CAIGDialog::OnInitDialog();
	CString czText;
	czText.Format(L"LUA DEBGGER - PID[%d]", GetCurrentProcessId());
	SetWindowText(czText);


	m_lstTreeCtrlLua.InsertColumn(CLOUMN_TABLE, L"Table", LVCFMT_LEFT, 400);
	m_lstTreeCtrlLua.InsertColumn(CLOUMN_VALUES, L"Values", LVCFMT_LEFT, 150);
	m_lstTreeCtrlLua.InsertColumn(CLOUMN_TYPE, L"Type", LVCFMT_CENTER, 100);

	m_cbxNameDll.AddString(L"ulua.dll");
	m_cbxNameDll.AddString(L"tolua.dll");
	m_cbxNameDll.SetCurSel(0);

	m_cbxVersionDll.AddString(L"5.1");
	m_cbxVersionDll.SetCurSel(0);

	m_cbxModeRunCommand.AddString(L"Add Table,Userdata By Path");
	m_cbxModeRunCommand.AddString(L"Add Table,Userdata By Ref");
	m_cbxModeRunCommand.AddString(L"Find Field By Name");
	m_cbxModeRunCommand.AddString(L"Export All Lua Script");
	m_cbxModeRunCommand.AddString(L"Export Script Field Select Table loaded");
	m_cbxModeRunCommand.SetCurSel(0);
	
	m_edtScript.SetFont(L"MS Shell Dlg 2", 11);


	//m_lstTreeCtrlTransform.InsertColumn(CLOUMN_TABLE, L"Transforms", LVCFMT_LEFT, 400);
	//m_lstTreeCtrlTransform.InsertColumn(CLOUMN_VALUES, L"Values", LVCFMT_LEFT, 150);
	//m_lstTreeCtrlTransform.InsertColumn(CLOUMN_TYPE, L"Information", LVCFMT_CENTER, 100);
	//m_lstTreeCtrlTransform.ShowWindow(SW_HIDE);
	//m_lstTreeCtrlTransform.SetImageList

	return TRUE;
}

void CDebuggersDlg::OnPaint()
{
	//if (IsIconic())
	//{
	//	CPaintDC dc(this); // device context for painting

	//	SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	//	// Center icon in client rectangle
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

	//	// Draw the icon
	//	dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CVIEDialog::OnPaint();
	//}
}


HTREEITEM CDebuggersDlg::FindItem(CString name, HTREEITEM hRoot)
{
	// check whether the current item is the searched one
	CString text = m_lstTreeCtrlLua.GetItemText(hRoot);
	if (text.Compare(name) == 0)
		return hRoot;

	// get a handle to the first child item
	HTREEITEM hSub = m_lstTreeCtrlLua.GetChildItem(hRoot);
	// iterate as long a new item is found
	while (hSub)
	{
		// check the children of the current item
		HTREEITEM hFound = FindItem(name, hSub);
		if (hFound)
			return hFound;

		// get the next sibling of the current item
		hSub = m_lstTreeCtrlLua.GetNextSiblingItem(hSub);
	}

	// return NULL if nothing was found
	return NULL;
}

void CDebuggersDlg::InsertItem(LPCSTR lpName, int type, LPCSTR values, LPCSTR lpfullPath)
{
	//CString czPath(CUtils::_A2W(lpfullPath));
	//CStringArray czArr;
	//CUtils::SplitString(czPath, L";", czArr);
	//m_lstTreeCtrlLua.GetParent
	HTREEITEM hParent = GetHTreeTable(lpfullPath);
	if (!hParent) return;
	HTREEITEM hItem;
	if (lpName && lpName[0] != 0)hItem = m_lstTreeCtrlLua.InsertItem(CUtils::_A2W(lpName), hParent, TVI_LAST);
	else hItem = m_lstTreeCtrlLua.InsertItem(L"<<No Name>>", hParent, TVI_LAST);
	m_lstTreeCtrlLua.SetItemText(hItem, CLOUMN_TYPE,g_pApiLua->GetTypeW(type));
	m_lstTreeCtrlLua.SetItemText(hItem, CLOUMN_VALUES,CUtils::_A2W(values));
		
}

void CDebuggersDlg::InsertItem(LPCSTR lpName, int type, LPCSTR values, HTREEITEM hParent)
{
	if (!hParent) return;
	HTREEITEM hItem;
	hItem = m_lstTreeCtrlLua.InsertItem(CUtils::_A2W(lpName), hParent, TVI_LAST);

	m_lstTreeCtrlLua.SetItemText(hItem, CLOUMN_TYPE, g_pApiLua->GetTypeW(type));
	m_lstTreeCtrlLua.SetItemText(hItem, CLOUMN_VALUES, CUtils::_A2W(values));
}

void CDebuggersDlg::InsertRootInfo(int type, LPCSTR values, HTREEITEM hRoot)
{
	if (!hRoot) return;
	m_lstTreeCtrlLua.SetItemText(hRoot, CLOUMN_TYPE, g_pApiLua->GetTypeW(type));
	m_lstTreeCtrlLua.SetItemText(hRoot, CLOUMN_VALUES, CUtils::_A2W(values));
}



void CDebuggersDlg::SortChildren(CString czRoot)
{
	//m_lstTreeCtrlLua.GetRootItem
}

void CDebuggersDlg::ShowBtnRunCommand(BOOL bShow)
{
	m_btnRunCommand.EnableWindow(bShow);
}

CString CDebuggersDlg::GetPathByItem(HTREEITEM hItem)
{
	if (!hItem) return CString(L"");
	CStringArray czArrTextItem;
	CString czPath;
	while (hItem)
	{
		czArrTextItem.Add(m_lstTreeCtrlLua.GetItemText(hItem));
		hItem = m_lstTreeCtrlLua.GetParentItem(hItem);
	}
	for (int i = czArrTextItem.GetCount() - 1; i >= 0; i--)
	{
		czPath += czArrTextItem.GetAt(i);
		if (i != 0) czPath += L";";
	}
	return czPath;
}

BOOL CDebuggersDlg::IsTypeInPath(HTREEITEM hItem,int type)
{
	if (!hItem) return FALSE;
	while (hItem)
	{
		int typelua = g_pApiLua->GetTypeByName(CUtils::_W2A(m_lstTreeCtrlLua.GetItemText(hItem, CLOUMN_TYPE)));
		if (type == typelua) return TRUE;
		hItem = m_lstTreeCtrlLua.GetParentItem(hItem);
	}
	return FALSE;
}

BOOL CDebuggersDlg::IsMetatableInPath(HTREEITEM hItem)
{
	if (!hItem) return FALSE;
	while (hItem)
	{
		CString czTable = m_lstTreeCtrlLua.GetItemText(hItem, CLOUMN_TABLE);
		if (czTable.Compare(L"__<<metatable>>") == 0) return TRUE;
		hItem = m_lstTreeCtrlLua.GetParentItem(hItem);
	}
	return FALSE;
}

void CDebuggersDlg::DelAllChildren(HTREEITEM hParent)
{
	if (!hParent) return;
	HTREEITEM hItem = m_lstTreeCtrlLua.GetChildItem(hParent);
	while (hItem)
	{
		
		HTREEITEM hItemPrev = hItem;
		hItem = m_lstTreeCtrlLua.GetNextSiblingItem(hItem);
		m_lstTreeCtrlLua.DeleteItem(hItemPrev);
	}
}

BOOL CDebuggersDlg::ShowTable(CString czFullPath, HTREEITEM hParent)
{
	BOOL bShow = theApp.ShowLuaTable(czFullPath, hParent);
	if (!bShow) return FALSE;
	m_lstTreeCtrlLua.SortChildren(hParent);
	return TRUE;
}

BOOL CDebuggersDlg::ShowUserData(CString czFullPath, HTREEITEM hParent)
{
	BOOL bShow = theApp.ShowUserData(czFullPath, hParent);
	if (!bShow) return FALSE;
	m_lstTreeCtrlLua.SortChildren(hParent);
	return TRUE;
}

void CDebuggersDlg::AddTreeRoot(LPCSTR lpNameRoot, HTREEITEM hRoot)
{
	TREEROOT* p = new TREEROOT;
	strcpy_s(p->czNameRoot,lpNameRoot);
	p->hRoot = hRoot;
	m_lstTreeRoot.AddHead(p);
}

HTREEITEM CDebuggersDlg::AddTreeRoot(LPCSTR lpNameRoot)
{
	HTREEITEM hRoot = m_lstTreeCtrlLua.InsertItem(CUtils::_A2W(lpNameRoot), TVI_ROOT);
	TREEROOT* p = new TREEROOT;
	strcpy_s(p->czNameRoot, lpNameRoot);
	p->hRoot = hRoot;
	m_lstTreeRoot.AddHead(p);

	return hRoot;
}

HTREEITEM CDebuggersDlg::GetHTreeTable(LPCSTR lpfullPath)
{
	CString czPath(CUtils::_A2W(lpfullPath));
	CStringArray czArr;
	CUtils::SplitString(czPath, L";", czArr);
	for (POSITION pos = m_lstTreeRoot.GetHeadPosition(); pos != NULL;)
	{
		TREEROOT* pRoot = (TREEROOT*)m_lstTreeRoot.GetNext(pos);
		if (strcmp(pRoot->czNameRoot, CUtils::_W2A(czArr.GetAt(0))) != 0)
			continue;
		
		if (czArr.GetCount() == 1) return pRoot->hRoot;

		HTREEITEM hParent = pRoot->hRoot;
		for (int i = 1; i < czArr.GetCount(); i++)
		{
			BOOL bIsParent = FALSE;
			// get a handle to the first child item
			HTREEITEM hSub = m_lstTreeCtrlLua.GetChildItem(hParent);
			// iterate as long a new item is found
			while (hSub)
			{
				CString text = m_lstTreeCtrlLua.GetItemText(hSub);
				if (text.Compare(czArr.GetAt(i)) == 0) {

					// see last tab in root
					if (i == czArr.GetCount() - 1) return hSub;
					bIsParent = TRUE;
					hParent = hSub;
					break;
				}
				
				// get the next sibling of the current item
				hSub = m_lstTreeCtrlLua.GetNextSiblingItem(hSub);
			}

			if(!bIsParent) break;
			bIsParent = TRUE;
		}

	}
	return NULL;
}

BOOL CDebuggersDlg::IsTreeRoot(HTREEITEM hRoot)
{
	return hRoot == TVI_ROOT;
}


void CDebuggersDlg::OnBnClickedButtonGetAddrBase()
{
	CString czText;
	m_cbxNameDll.GetWindowTextW(czText);
	wcscpy_s(g_pGame->GetOptionTab()->czNameDll, czText.GetBuffer());
	g_pGame->GetOptionTab()->bRunGetAddrApiLua = TRUE;
}

	
void CDebuggersDlg::OnNMDblclkTreeLua(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	HTREEITEM hItem = m_lstTreeCtrlLua.GetSelectedItem();
	CString czPath = GetPathByItem(hItem);
	LOGW_I(L"Choose::[%s]   values:[%s] type:[%s]", czPath,m_lstTreeCtrlLua.GetItemText(hItem, CLOUMN_VALUES),m_lstTreeCtrlLua.GetItemText(hItem, CLOUMN_TYPE));

	CString czType = m_lstTreeCtrlLua.GetItemText(hItem, CLOUMN_TYPE);
	
	if (m_lstTreeCtrlLua.ItemHasChildren(hItem)) return;
	
	if (/*!IsTypeInPath(hItem, LUA_TUSERDATA) &&*/ czType.Compare(L"Table") == 0) {
		ShowTable(czPath, hItem);
		return;
	}
	else if (czType.Compare(L"UserData") == 0 || czType.Compare(L"LightUserData") == 0) {
		ShowUserData(czPath, hItem);
		return;
	}
	
}




void CDebuggersDlg::OnBnClickedButtonRunCommand()
{
	CString czTextInput;
	m_edtGetTab.GetWindowTextW(czTextInput);
	if (czTextInput.IsEmpty()) {
		AfxMessageBox(L"Text Input Run Commnad Rỗng!!!");
		return;
	}

	// add table by path
	CString czModeRun = m_cbxModeRunCommand.GetSelectedText();
	if (czModeRun.Compare(L"Add Table,Userdata By Path") == 0) {
		CStringArray czArr;
		CUtils::SplitString(czTextInput, L";", czArr);
		CStringA czRoot(CUtils::_W2A(czArr.GetAt(czArr.GetCount() - 1)));
		if (!theApp.IsLuaTableOrUserData(czTextInput)) {
			LOGW_ET(L"CDebuggersDlg :: NOT A TABLE [%s]", czTextInput);
			return;
		}
		HTREEITEM hRoot = m_lstTreeCtrlLua.InsertItem(czTextInput, TVI_ROOT);
		AddTreeRoot(czRoot, hRoot);
		if (!ShowTable(czTextInput, hRoot))
			ShowUserData(czTextInput, hRoot);
	}
	else if (czModeRun.Compare(L"Add Table,Userdata By Ref") == 0) {
		CString czRoot;
		czRoot.Format(L"Ref:%s", czTextInput);
		HTREEITEM hRoot = m_lstTreeCtrlLua.InsertItem(czRoot, TVI_ROOT);
		AddTreeRoot(CUtils::_W2A(czRoot), hRoot);
		theApp.ShowLuaTableByRef(_wtoi(czTextInput), hRoot);
		m_lstTreeCtrlLua.SortChildren(hRoot);
	}
	else if (czModeRun.Compare(L"Find Field By Name") == 0)
	{
		HTREEITEM hItem = m_lstTreeCtrlLua.GetSelectedItem();
		while (hItem)
		{
			hItem = m_lstTreeCtrlLua.GetParentItem(hItem);
			CString szType = m_lstTreeCtrlLua.GetItemText(hItem, CLOUMN_TYPE);
			BOOL bIsTable = (szType.Compare(L"Table") == 0) || (szType.Compare(L"UserData") == 0);
			if (bIsTable)break;

		}
		if (!hItem) {
			AfxMessageBox(L"Chưa chọn table để tìm kiếm (chọn ở tree)!!!");
			return;
		}
		CString szPath = GetPathByItem(hItem);
		theApp.FindFieldByName(czTextInput, szPath);
	}
	else if(czModeRun.Compare(L"Export All Lua Script") == 0)
	{
		WCHAR czLast = czTextInput.GetAt(czTextInput.GetAllocLength() - 1);
		if (wcscmp(&czLast, L"/") != 0)
			czTextInput += L"/";
		g_pExportScript->SetFolderExport(CUtils::_W2A(czTextInput));
		theApp.GetAllNameModuleInLoaded();
		theApp.ReloadAllModuleInLoaded();
		g_pExportScript->ShowError();
		g_pExportScript->Clear();
	}
	else if (czModeRun.Compare(L"Export Script Field Select Table loaded") == 0)
	{
		HTREEITEM hItem = m_lstTreeCtrlLua.GetSelectedItem();
		CString czPath = GetPathByItem(hItem);
		if (!hItem || czPath.Find(L"loaded") < 0) {
			AfxMessageBox(L"phải chọn ở trong table loaded!!");
			return;
		}
		WCHAR czLast = czTextInput.GetAt(czTextInput.GetAllocLength() - 1);
		if (wcscmp(&czLast, L"/") != 0)
			czTextInput += L"/";
		g_pExportScript->SetFolderExport(CUtils::_W2A(czTextInput));
		CString szTable = m_lstTreeCtrlLua.GetItemText(hItem);
		theApp.ReloadModule(CUtils::_W2A(szTable));
	}
	
	

}


void CDebuggersDlg::OnNMRClickTreeLua(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_lstTreeCtrlLua.GetSelectedItem();
	if (!hItem) return;
	CString czPath = GetPathByItem(hItem);
	CString czType = m_lstTreeCtrlLua.GetItemText(hItem,CLOUMN_TYPE);
	
	if (czType.Compare(L"Table") == 0) {
		if (MessageBox(L"Reloaded Table?", L"", MB_YESNO) == IDYES) {
			DelAllChildren(hItem);
			ShowTable(czPath, hItem);
		}
			
	}
	else if (czType.Compare(L"UserData") == 0 || czType.Compare(L"LightUserData") == 0) {
		if (MessageBox(L"Reloaded UserData?", L"", MB_YESNO) == IDYES) {
			DelAllChildren(hItem);
			ShowUserData(czPath, hItem);
		}
	}
	else if (czType.Compare(L"Function") == 0) {

		int type = (IsTypeInPath(hItem,LUA_TUSERDATA) || IsMetatableInPath(hItem)) ? LUA_TUSERDATA: LUA_TTABLE;
		
		BOOL bUserData = m_szPrevNameCallFunc.Compare(czPath) == 0;
		m_szPrevNameCallFunc = czPath;
		if (!bUserData) g_pGame->DefaultArgs();
		CCallFuntionDlg* pCallDlg = new CCallFuntionDlg(this, czPath,type,bUserData);
		//pCallDlg->DoModal();
		//pCallDlg.m_czTitle = m_lstTreeCtrlLua.GetItemText(hItem);
		pCallDlg->Create(IDD_DIALOG_CALL_FUC);
		pCallDlg->ShowWindow(TRUE);
		//delete pCallDlg;
		
	}
	

}

/*----------transform-------*/

void CDebuggersDlg::OnBnClickedCheckTest()
{
	g_pGame->GetOptionTab()->bTest = m_btnTest.GetCheck();
}




void CDebuggersDlg::OnBnClickedButtonDoScript()
{
	CString CzScript = m_edtScript.GetText();
	if (CzScript.IsEmpty()) return;
	theApp.LoadLuaString(CzScript);

}


void CDebuggersDlg::OnNMClickTreeLua(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

}


//void CDebuggersDlg::OnNMDblclkTreeTransform(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
//
//
//void CDebuggersDlg::OnNMRClickTreeTransform(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}


void CDebuggersDlg::OnBnClickedCheckShowTreeTransform()
{
	BOOL bShowTransform = (m_chkShowTreeTransform.GetCheck() == BST_CHECKED);
	if (bShowTransform) {
		//m_lstTreeCtrlTransform.ShowWindow(SW_SHOW);
		m_lstTreeCtrlLua.ShowWindow(SW_HIDE);
	}
	else {
		m_lstTreeCtrlLua.ShowWindow(SW_SHOW);
		//m_lstTreeCtrlTransform.ShowWindow(SW_HIDE);
	}
}
