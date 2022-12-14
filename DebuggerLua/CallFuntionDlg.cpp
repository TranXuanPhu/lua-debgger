// CallFuntionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DebuggerLua.h"
#include "CallFuntionDlg.h"
#include "afxdialogex.h"


// CCallFuntionDlg dialog

IMPLEMENT_DYNAMIC(CCallFuntionDlg, CAIGDialog)

CCallFuntionDlg::CCallFuntionDlg(CWnd* pParent /*=nullptr*/, CString czPathFunc, int ParentType /*= LUA_TTABLE*/, BOOL bUserDataPrev /*= FALSE*/)
	: CAIGDialog(IDD_DIALOG_CALL_FUC, pParent)
{
	CStringArray czArr;
	CUtils::SplitString(czPathFunc, L";", czArr);
	CString czFunc = czArr.GetAt(czArr.GetCount() - 1);
	m_czNameFunc = czFunc;
	m_czPathFunc = czPathFunc;
	m_bUserDataPrev = bUserDataPrev;
	m_czPathParentFunc = czPathFunc.Left(czPathFunc.GetLength() - czFunc.GetAllocLength() - 1);
	m_ParentType = ParentType;
}

CCallFuntionDlg::~CCallFuntionDlg()
{
}

BOOL CCallFuntionDlg::OnInitDialog()
{
	CAIGDialog::OnInitDialog();
	CString czText;
	czText.Format(L"Call Function - %s", m_czNameFunc);
	SetWindowText(czText);

	m_lstCtrlArgs.InsertColumn(ARGSLIST_INDEX_INDEX, L"#", LVCFMT_CENTER, 25);
	m_lstCtrlArgs.InsertColumn(ARGSLIST_INDEX_VALUES, TEXT("Values"), LVCFMT_LEFT, 190);
	m_lstCtrlArgs.InsertColumn(ARGSLIST_INDEX_TYPE, TEXT("Type"), LVCFMT_CENTER, 65);
	m_lstCtrlArgs.InsertColumn(ARGSLIST_INDEX_NAMEARGS, TEXT("Name Args"), LVCFMT_LEFT, 68);

	m_lstCtrlArgs.SetRowAutoBkColor(RGB(255, 255, 255), RGB(250, 250, 255));
	m_lstCtrlArgs.SetItemsHeight(22);
	m_lstCtrlArgs.EnableDragDropItem(TRUE);
	//m_lstCtrlArgs.SetBorderColor(RGB(255, 255, 255));//(RGB(169,175,185));
	//m_lblNote.SetFont(L"Tahoma", 8, TRUE);
	m_lblNote.SetTextColor(RGB(255, 0, 0));

	m_cbxValueType.AddString(L"Nil");
	m_cbxValueType.AddString(L"Boolean");
	m_cbxValueType.AddString(L"Number");
	m_cbxValueType.AddString(L"String");
	m_cbxValueType.AddString(L"Field");
	m_cbxValueType.AddString(L"Self");
	m_cbxValueType.AddString(L"Ref");
	m_cbxValueType.SetCurSel(0);

	m_cbxResultType.AddString(L"show all children in Log");
	m_cbxResultType.AddString(L"Creates a reference in LUA_REGISTRY");
	m_cbxResultType.SetCurSel(0);
	
	UpdateArgs(m_bUserDataPrev);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CCallFuntionDlg, CAIGDialog)
	ON_BN_CLICKED(IDC_BUTTON_CALL, &CCallFuntionDlg::OnBnClickedButtonCall)
	ON_NOTIFY(LVN_EX_EDITCHANGED, IDC_LIST_ARGS, &CCallFuntionDlg::OnLvnExEditChanged)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CCallFuntionDlg::OnCbnSelchangeComboType)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SHOW_INFO, &CCallFuntionDlg::OnBnClickedButtonShowInfo)
	ON_STN_CLICKED(IDC_LBL_NOTE, &CCallFuntionDlg::OnStnClickedLblNote)
END_MESSAGE_MAP()

void CCallFuntionDlg::UpdateArgs(BOOL bUserDataPrev /*= FALSE*/)
{
	//get params func
	CStringArray ArrNameArgs;
	BOOL isvararg = FALSE; // ham có nhiềm tham số?
	BOOL bIsFuncLua =  theApp.GetArgsFunction(m_czPathFunc,ArrNameArgs, isvararg, m_ParentType == LUA_TUSERDATA);

	int iMax_Args = MAX_ARGUMENT;
	int iCountArgs = ArrNameArgs.GetCount();
	if (bIsFuncLua) iMax_Args = iCountArgs;
	//LOGA("bIsFuncLua:%d,iMax_Args:%d,iCountArgs:%d", bIsFuncLua, iMax_Args, iCountArgs);
	OPTIONTAB* pTab = g_pGame->GetOptionTab();
	for (int i = 0; i < iMax_Args; i++)
	{
		int iIndex = m_lstCtrlArgs.InsertItem(i, L"");
		m_lstCtrlArgs.SetItemFormatText(iIndex, ARGSLIST_INDEX_INDEX,L"%d",i + 1);
		
		m_lstCtrlArgs.SetItemEditable(iIndex, ARGSLIST_INDEX_VALUES);
		m_lstCtrlArgs.SetComboBox(iIndex, ARGSLIST_INDEX_TYPE, &m_cbxValueType);
		
		//m_lstCtrlArgs.SetItemFormatText(iIndex, ARGSLIST_INDEX_VALUES, L"");
		m_cbxValueType.SetCurSel(0);

		if (i < iCountArgs) {
			m_lstCtrlArgs.SetItemFormatText(iIndex, ARGSLIST_INDEX_NAMEARGS, L"%s", ArrNameArgs.GetAt(i));
			if (ArrNameArgs.GetAt(i).Compare(L"self") == 0) {
				m_lstCtrlArgs.SetItemText(iIndex, ARGSLIST_INDEX_VALUES, m_czPathParentFunc);
				wcscpy_s(g_pGame->GetOptionTab()->lstArgs[iIndex].czArg, m_czPathParentFunc.GetBuffer());
				g_pGame->GetOptionTab()->lstArgs[iIndex].iType = ARGUMENT_SELF;
				m_cbxValueType.SetCurSel(ARGUMENT_SELF);
				//LOGW(L"index:%d [%s]", iIndex,g_pGame->GetOptionTab()->lstArgs[iIndex].czArg);
			}
				
		}else if(bIsFuncLua && isvararg)
			m_lstCtrlArgs.SetItemText(iIndex, ARGSLIST_INDEX_NAMEARGS, L"...");
		
		if (bUserDataPrev) {
			m_lstCtrlArgs.SetItemFormatText(iIndex, ARGSLIST_INDEX_VALUES, L"%s",pTab->lstArgs[i].czArg);
			m_cbxValueType.SetCurSel(pTab->lstArgs[i].iType);
		}
			
		m_lstCtrlArgs.SetItemText(iIndex, ARGSLIST_INDEX_TYPE, m_cbxValueType.GetSelectedText());
	}

	m_edtnArgs.SetFormatText(L"%d", iCountArgs);

}

void CCallFuntionDlg::DoDataExchange(CDataExchange* pDX)
{
	CAIGDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ARGS, m_lstCtrlArgs);
	DDX_Control(pDX, IDC_EDIT_NARGS, m_edtnArgs);
	DDX_Control(pDX, IDC_BUTTON_CALL, m_btnCallFunc);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cbxValueType);
	DDX_Control(pDX, IDC_LBL_NOTE, m_lblNote);
	DDX_Control(pDX, IDC_COMBO_RESULTTYPE, m_cbxResultType);
	DDX_Control(pDX, IDC_BUTTON_SHOW_INFO, m_btnShowInfoFunc);
}




// CCallFuntionDlg message handlers


void CCallFuntionDlg::OnClose()
{
	CAIGDialog::OnClose();
}

void CCallFuntionDlg::OnBnClickedButtonCall()
{
	OPTIONTAB* pTab = g_pGame->GetOptionTab();
	int nArgs =_wtoi(m_edtnArgs.GetText());
	LPVOID L = g_pApiLua->Get_MainThread();
	int oldTop = g_pApiLua->lua_gettop(L);
	//push func
	if (m_ParentType == LUA_TUSERDATA)
		theApp.PushTopFieldInUserData(m_czPathFunc);
	else theApp.PushTopField(m_czPathFunc);

	// push args
	for (int i = 0; i < nArgs; i++)
	{
		theApp.PushArg(pTab->lstArgs[i].czArg, pTab->lstArgs[i].iType);
	}

	BOOL bRef = (m_cbxResultType.GetCurSel() == 1);
	theApp.CallFuncLua(nArgs, m_czPathFunc, bRef, bRef? FALSE: TRUE);
	g_pApiLua->lua_settop(L, oldTop);
}

void CCallFuntionDlg::OnLvnExEditChanged(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMITEMACTIVATE pNM = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString szText = m_lstCtrlArgs.GetItemText(pNM->iItem, pNM->iSubItem);
	if (pNM->iSubItem == ARGSLIST_INDEX_VALUES && pNM->iItem >= 0)
	{
		wcscpy_s(g_pGame->GetOptionTab()->lstArgs[pNM->iItem].czArg,szText);
	}
}


void CCallFuntionDlg::OnCbnSelchangeComboType()
{
	int iValCurSel = m_cbxValueType.GetCurSel();
	int iItem = m_lstCtrlArgs.GetSelectedItem();
	
	if (iValCurSel == ARGUMENT_SELF) {
		//LOGA_D("lstArgs[%d] type:%d", m_lstCtrlArgs.GetSelectedItem(), m_cbxValueType.GetCurSel());
		m_lstCtrlArgs.SetItemText(iItem, ARGSLIST_INDEX_VALUES,m_czPathParentFunc);
		wcscpy_s(g_pGame->GetOptionTab()->lstArgs[iItem].czArg, m_czPathParentFunc);
	}
	g_pGame->GetOptionTab()->lstArgs[iItem].iType = iValCurSel;
	//LOGA_D("lstArgs[%d] type:%d", m_lstCtrlArgs.GetSelectedItem(), m_cbxValueType.GetCurSel());
}



void CCallFuntionDlg::OnBnClickedButtonShowInfo()
{
	LOGW_I(L"Show info func [%s]", m_czPathFunc);
	LPVOID L = g_pApiLua->Get_MainThread();
	int oldTop = g_pApiLua->lua_gettop(L);
	theApp.PushTopField(L"debug;getinfo");
	if (m_ParentType == LUA_TUSERDATA) theApp.PushTopFieldInUserData(m_czPathFunc);
	else theApp.PushTopField(m_czPathFunc);// push arg
	if (g_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
		LOGA_E("Error call Func [debug;getinfo]:: %s", g_pApiLua->lua_tostring(L, -1));
		g_pApiLua->lua_settop(L, oldTop);
		return;
	}
	theApp.ShowAllChildrentTable(L"", NULL,TRUE);
	g_pApiLua->lua_settop(L, oldTop);
}




void CCallFuntionDlg::OnStnClickedLblNote()
{
	// TODO: Add your control notification handler code here
}
