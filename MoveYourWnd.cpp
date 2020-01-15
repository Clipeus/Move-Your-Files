/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Main application window
*/

#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <shellapi.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "Utils.h"
#include "DataHandler.h"
#include "AboutDlg.h"
#include "AppOptions.h"
#include "NewEditDlg.h"
#include "OptionsDlg.h"
#include "FindDlg.h"
#include "RegMonitor.h"

TBBUTTON tbButtons[] =
{
  {0, ID_FILE_INPORT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {1, ID_FILE_EXPORT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {2, ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {3, ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {4, ID_EDIT_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {5, ID_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {6, ID_EDIT_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {7, ID_EDIT_EDIT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {8, ID_EDIT_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {9, ID_VIEW_OPTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
};

#define TB_ALL_BUTTONS		14
#define TB_BUTTONS			10

#define IDC_MAIN_TOOLBAR	IDR_MAIN_TOOLBAR
#define IDC_MAIN_STATUSBAR	IDC_MAIN_TOOLBAR + 1
#define IDC_LIST_VIEW		IDC_MAIN_STATUSBAR + 1

#define	COL1X	40
#define	COL2X	250
#define	COL3X	250

static TCHAR s_szWindowPos[] = _T("WindowPos");
LPCTSTR MoveYourWnd::s_szWindowClass = _T("MoveYourFiles.1");

MoveYourWnd::MoveYourWnd()
{

}

MoveYourWnd::~MoveYourWnd()
{
}

void MoveYourWnd::EnableCommands(long nCount, long nSelCount, HMENU hContextMenu)
{
  if (hContextMenu == nullptr)
  {
    HMENU hMainMenu = GetMenu(m_hWnd);
    HMENU hMenu = GetSubMenu(hMainMenu, 1);

    EnableMenuItem(hMenu, ID_EDIT_EDIT, MF_BYCOMMAND | (nSelCount == 1 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_EDIT, MAKELONG(nSelCount == 1, 0));

    EnableMenuItem(hMenu, ID_EDIT_COPY, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_COPY, MAKELONG(nSelCount != 0, 0));

    EnableMenuItem(hMenu, ID_EDIT_CUT, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_CUT, MAKELONG(nSelCount != 0, 0));

    EnableMenuItem(hMenu, ID_EDIT_DELETE, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_DELETE, MAKELONG(nSelCount != 0, 0));

    EnableMenuItem(hMenu, ID_EDIT_SELECTALL, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));

    EnableMenuItem(hMenu, ID_EDIT_FIND, MF_BYCOMMAND | (nCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_FIND, MAKELONG(nCount != 0, 0));
    EnableMenuItem(hMenu, ID_EDIT_FINDNEXT, MF_BYCOMMAND | (nCount != 0 ? MF_ENABLED : MF_GRAYED));

    hMenu = GetSubMenu(hMainMenu, 0);
    EnableMenuItem(hMenu, ID_FILE_EXPORT, MF_BYCOMMAND | (nCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_FILE_EXPORT, MAKELONG(nCount != 0, 0));
  }
  else
  {
    EnableMenuItem(hContextMenu, ID_EDIT_EDIT, MF_BYCOMMAND | (nSelCount == 1 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_EDIT, MAKELONG(nSelCount == 1, 0));

    EnableMenuItem(hContextMenu, ID_EDIT_COPY, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_COPY, MAKELONG(nSelCount != 0, 0));

    EnableMenuItem(hContextMenu, ID_EDIT_CUT, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_CUT, MAKELONG(nSelCount != 0, 0));

    EnableMenuItem(hContextMenu, ID_EDIT_DELETE, MF_BYCOMMAND | (nSelCount != 0 ? MF_ENABLED : MF_GRAYED));
    SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_DELETE, MAKELONG(nSelCount != 0, 0));
  }
}

void MoveYourWnd::SetStdStatusText(long nCount, long nSelCount)
{
  if (nSelCount == -1)
    nSelCount = ListView_GetSelectedCount(m_hListView);
  if (nCount == -1)
    nCount = ListView_GetItemCount(m_hListView);

  SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)StrFormat(LoadString(IDS_IDLE_STATUS).c_str(), nCount, nSelCount).c_str());
}

void MoveYourWnd::SetStdStatusAndMenu()
{
  long nSelCount = ListView_GetSelectedCount(m_hListView);
  long nCount = ListView_GetItemCount(m_hListView);
  EnableCommands(nCount, nSelCount);
  SetStdStatusText(nCount, nSelCount);
}

void MoveYourWnd::ChangeMenuItems(HMENU hContextMenu)
{
  long nSelCount = ListView_GetSelectedCount(m_hListView);
  long nCount = ListView_GetItemCount(m_hListView);
  EnableCommands(nCount, nSelCount, hContextMenu);
}

LRESULT MoveYourWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_MSG(hWnd, WM_CHANGECBCHAIN, OnChangeCBChain);
    HANDLE_MSG(hWnd, WM_DRAWCLIPBOARD, OnDrawClipboard);
    HANDLE_MSG(hWnd, WM_NCDESTROY, OnNCDestroy);
    HANDLE_MSG(hWnd, WM_CONTEXTMENU, OnContextMenu);
    HANDLE_MSG(hWnd, WM_DROPFILES, OnDropFiles);
    HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
    HANDLE_MSG(hWnd, WM_SIZE, OnSize);
    HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
    HANDLE_MSG(hWnd, WM_MENUSELECT, OnMenuSelect);
    HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
    HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
    HANDLE_MSG(hWnd, WM_SETCURSOR, OnSetCursor);
    HANDLE_MSG(hWnd, WM_ACTIVATE, OnActivate);
    HANDLE_MSG(hWnd, WM_INITMENUPOPUP, OnInitMenuPopup);
    HANDLE_MSG(hWnd, WM_MEASUREITEM, OnMeasureItem), true; // should return true
    HANDLE_MSG(hWnd, WM_DRAWITEM, OnDrawItem), true; // should return true

    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}

void MoveYourWnd::OnDestroy(HWND hWnd)
{
  WriteWindowPos(MoveYourApp::s_szMainKey, s_szWindowPos);
  m_pRegMonitor->Uninstall();
  ChangeClipboardChain(m_hWnd, m_hClipNextView);

  if (m_hBitmap)
  ::DeleteObject(m_hBitmap);

  PostQuitMessage(0);
}

bool MoveYourWnd::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
  m_hWnd = hWnd;
  m_hToolBar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP,
    IDC_MAIN_TOOLBAR, TB_BUTTONS, MoveYourApp::GetApp()->GetInstance(), IDR_MAIN_TOOLBAR, (LPCTBBUTTON)&tbButtons, TB_ALL_BUTTONS,
    16, 16, 16, 16, sizeof(TBBUTTON));

  if (!m_hToolBar)
  {
    ShowOSError();
    return false;
  }

  m_hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER | SBARS_SIZEGRIP, nullptr, hWnd, IDC_MAIN_STATUSBAR);
  if (!m_hStatusBar)
  {
    ShowOSError();
    return false;
  }

  SendMessage(m_hStatusBar, SB_SETPARTS, 2, (LPARAM)m_ptWidth);
  SendMessage(m_hStatusBar, SB_SETTEXT, 1, 
    (LPARAM)StrFormat(LoadString(IDS_RIGHT_STATUS).c_str(), 
      HIWORD(MoveYourApp::GetApp()->GetFixedInfo().dwFileVersionMS), 
      LOWORD(MoveYourApp::GetApp()->GetFixedInfo().dwFileVersionMS)).c_str());

  m_hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, nullptr, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS,
    0, 0, lpCreateStruct->cx, lpCreateStruct->cy, hWnd, (HMENU)IDC_LIST_VIEW, MoveYourApp::GetApp()->GetInstance(), nullptr);
  if (!m_hListView)
  {
    ShowOSError();
    return false;
  }
  HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_MASK, 3, 1);
  if (!hImageList)
  {
    ShowOSError();
    return false;
  }
  
  HICON hIcon = (HICON)::LoadImage(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_DELETE_ITEM), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
  ImageList_AddIcon(hImageList, hIcon);
  DestroyIcon(hIcon);
  hIcon = (HICON)::LoadImage(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MOVE_COPY_ITEM), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
  ImageList_AddIcon(hImageList, hIcon);
  DestroyIcon(hIcon);
  ListView_SetImageList(m_hListView, hImageList, LVSIL_SMALL);

  m_hClipNextView = SetClipboardViewer(hWnd);
  InitList();
  m_pDataHandler = std::make_unique<DataHandler>();

  DragAcceptFiles(m_hWnd, true);

  m_pRegMonitor = std::make_unique<RegMonitor>();
  if (!m_pRegMonitor->Install(hWnd))
  {
    ShowOSError();
    return false;
  }

  PostMessage(hWnd, WM_COMMAND, MAKELONG(ID_VIEW_REFRESH, 0), 0);
  return true;
}

void MoveYourWnd::OnSize(HWND hWnd, UINT state, int cx, int cy)
{
  SendMessage(m_hToolBar, WM_SIZE, state, 0);
  SendMessage(m_hStatusBar, WM_SIZE, state, 0);
  m_ptWidth[0] = cx - 150;
  m_ptWidth[1] = -1;
  SendMessage(m_hStatusBar, SB_SETPARTS, 2, (LPARAM)m_ptWidth);

  RECT Rect;
  int nTop, nBottom;
  GetWindowRect(m_hToolBar, &Rect);
  nTop = Rect.bottom - Rect.top - 1;
  GetWindowRect(m_hStatusBar, &Rect);
  nBottom = Rect.bottom - Rect.top;
  AdjustRect(0, nTop, cx, cy - nBottom - nTop);
}

void MoveYourWnd::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case ID_FILE_EXIT:
    {
      PostMessage(hWnd, WM_CLOSE, 0, 0);
      break;
    }
    case ID_EDIT_SELECTALL:
    {
      for (int i = 0; i < ListView_GetItemCount(m_hListView); i++)
      {
        ListView_SetItemState(m_hListView, i, LVIS_SELECTED, LVIS_SELECTED);
      }
      break;
    }
    case ID_VIEW_REFRESH:
    {
      m_pDataHandler->FillList(m_hListView);
      ListView_SetItemState(m_hListView, 0, LVIS_SELECTED, LVIS_SELECTED);
      SetStdStatusAndMenu();
      break;
    }
    case ID_EDIT_DELETE:
    {
      int nIndex = -1, nLastIndex = -1;
      while (-1 != (nIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED)))
      {
        ListView_DeleteItem(m_hListView, nLastIndex = nIndex);
      }
      m_pDataHandler->UpdateRecord(m_hListView);
      long nCount = ListView_GetItemCount(m_hListView);
      if (nCount)
      {
        if (nLastIndex < nCount)
        {
          ListView_SetItemState(m_hListView, nLastIndex, LVIS_SELECTED, LVIS_SELECTED);
        }
        else
        {
          ListView_SetItemState(m_hListView, nLastIndex - 1, LVIS_SELECTED, LVIS_SELECTED);
        }
      }
      SetStdStatusAndMenu();
      break;
    }
    case ID_EDIT_NEW:
    case ID_EDIT_EDIT:
    {
      WaitCursor wc;

      std::unique_ptr<ITEMDATAEX> pItemDataEx(new ITEMDATAEX);
      NewEditDlg dlg;
      dlg.SetItemData(pItemDataEx.get());
      dlg.GetItemData()->uAction = id;
      if (id == ID_EDIT_EDIT)
      {
        if (!GetListItemData(pItemDataEx.get()))
          break;
      }

      if (IDOK == dlg.DoModal(m_hWnd))
      {
        if (id == ID_EDIT_EDIT)
          SetListItemData(pItemDataEx.release());
        else
          AddToList(pItemDataEx.release());

        m_pDataHandler->UpdateRecord(m_hListView);
        SetStdStatusAndMenu();
      }
      break;
    }
    case ID_HELP_ABOUT:
    {
      WaitCursor wc;

      AboutDlg dlg;
      dlg.DoModal(m_hWnd);
      break;
    }
    case ID_FILE_EXPORT:
    {
      if (ListView_GetItemCount(m_hListView))
        m_pDataHandler->OnExportScript(hWnd, m_hListView, MoveYourApp::GetApp()->GetInstance());
      break;
    }
    case ID_FILE_INPORT:
    {
      long nCount = ListView_GetItemCount(m_hListView);
      if (m_pDataHandler->OnInportScript(hWnd, m_hListView, MoveYourApp::GetApp()->GetInstance()))
      {
        if (!nCount)
          ListView_SetItemState(m_hListView, 0, LVIS_SELECTED, LVIS_SELECTED);
        m_pDataHandler->UpdateRecord(m_hListView);
        SetStdStatusAndMenu();
      }
      break;
    }
    case ID_EDIT_FIND:
    {
      OnFind();
      break;
    }
    case ID_EDIT_FINDNEXT:
    {
      OnFind(true);
      break;
    }
    case ID_EDIT_PASTE:
    {
      long nCount = ListView_GetItemCount(m_hListView);
      if (m_pDataHandler->OnPaste(hWnd, m_hListView))
      {
        if (!nCount)
          ListView_SetItemState(m_hListView, 0, LVIS_SELECTED, LVIS_SELECTED);
        m_pDataHandler->UpdateRecord(m_hListView);
        SetStdStatusAndMenu();
      }
      break;
    }
    case ID_EDIT_CUT:
    {
      int nLastIndex;
      nLastIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
      if (m_pDataHandler->OnCopyCut(hWnd, m_hListView, true))
      {
        m_pDataHandler->UpdateRecord(m_hListView);
        long nCount = ListView_GetItemCount(m_hListView);
        if (nCount)
        {
          if (nLastIndex < nCount)
          {
            ListView_SetItemState(m_hListView, nLastIndex, LVIS_SELECTED, LVIS_SELECTED);
          }
          else
          {
            ListView_SetItemState(m_hListView, nLastIndex - 1, LVIS_SELECTED, LVIS_SELECTED);
          }
        }
        SetStdStatusAndMenu();
      }
      break;
    }
    case ID_EDIT_COPY:
    {
      m_pDataHandler->OnCopyCut(hWnd, m_hListView, false);
      break;
    }
    case ID_VIEW_OPTION:
    {
      WaitCursor wc;
      OptionsDlg dlg;
      dlg.DoModal(hWnd);
      break;
    }
  }
}

void MoveYourWnd::OnMenuSelect(HWND hWnd, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags)
{
  LPTSTR lpszText = nullptr;
  tstring strTemp;
  UINT nStrID;
  DWORD dwStyle = SBT_NOBORDERS;

  if ((flags == 0xFFFFFFFF) && (hMenu == nullptr))
  {
    nStrID = IDS_IDLE_STATUS;
    SendMessage(m_hStatusBar, SB_SETPARTS, 2, (LPARAM)m_ptWidth);

    strTemp = StrFormat(LoadString(IDS_IDLE_STATUS).c_str(), ListView_GetItemCount(m_hListView), ListView_GetSelectedCount(m_hListView));
    lpszText = &strTemp.front();
    dwStyle = 0;

    SendMessage(m_hStatusBar, SB_SETTEXT, 1,
      (LPARAM)StrFormat(LoadString(IDS_RIGHT_STATUS).c_str(), 
      HIWORD(MoveYourApp::GetApp()->GetFixedInfo().dwFileVersionMS), 
      LOWORD(MoveYourApp::GetApp()->GetFixedInfo().dwFileVersionMS)).c_str());
  }
  else
  {
    nStrID = item;
    int nWidth = -1;
    SendMessage(m_hStatusBar, SB_SETPARTS, 1, (LPARAM)&nWidth);
  }

  if ((flags & MF_POPUP) || (flags & MFT_SEPARATOR))
  {
    nStrID = 0;
  }

  if (nStrID && nStrID != IDS_IDLE_STATUS)
  {
    strTemp = LoadString(nStrID);
    LPTSTR lpszNext = nullptr;
    lpszText = _tcstok_s(&strTemp.front(), _T("\n"), &lpszNext);
  }

  SendMessage(m_hStatusBar, SB_SETTEXT, 0 | dwStyle, (LPARAM)lpszText);
}

void MoveYourWnd::OnContextMenu(HWND hWnd, HWND hwndContext, UINT x, UINT y)
{
  POINT pt;
  RECT rect;

  if (x == -1 || y == -1)
  {
    int nItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    if (nItem == -1)
      ::GetClientRect(m_hListView, &rect);
    else
      ListView_GetItemRect(m_hListView, nItem, &rect, LVIR_BOUNDS);

    LVCOLUMN col = { 0 };
    col.mask = LVCF_WIDTH;
    ListView_GetColumn(m_hListView, 0, &col);
    pt.x = rect.left + col.cx;
    pt.y = rect.top;
    ::ClientToScreen(m_hListView, &pt);
  }
  else
  {
    pt.x = x;
    pt.y = y;

    if (m_hListView != ::WindowFromPoint(pt))
      return;
  }

  HMENU hMenuMain = ::LoadMenu(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDR_ITEMMENU));
  HMENU hMenuContext = ::GetSubMenu(hMenuMain, 0);

  ::SetMenuDefaultItem(hMenuContext, 0, true);
  ChangeMenuItems(hMenuContext);
  ::TrackPopupMenu(hMenuContext, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
  ::DestroyMenu(hMenuContext);
  ::DestroyMenu(hMenuMain);
}

LRESULT MoveYourWnd::OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr)
{
  switch (pNmndr->code)
  {
    case TTN_NEEDTEXTW:
    case TTN_NEEDTEXTA:
    {
      TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNmndr;
      TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNmndr;

      tstring strTemp = LoadString(idFrom);
      LPTSTR lpszText = nullptr;
      LPTSTR lpszNext = nullptr;
      _tcstok_s(&strTemp.front(), _T("\n"), &lpszNext);
      lpszText = _tcstok_s(nullptr, _T("\n"), &lpszNext);

#ifndef _UNICODE
      size_t nConverted;
      if (pNmndr->code == TTN_NEEDTEXTA)
        lstrcpyn(pTTTA->szText, lpszText, sizeof(pTTTA->szText));
      else
        mbstowcs_s(&nConverted, pTTTW->szText, lpszText, sizeof(pTTTW->szText));
#else
      size_t nc = 0;
      if (pNmndr->code == TTN_NEEDTEXTA)
        wcstombs_s(&nc, pTTTA->szText, lpszText, sizeof(pTTTA->szText));
      else
        wcsncpy_s(pTTTW->szText, lpszText, sizeof(pTTTW->szText));
#endif
      break;
    }
    case LVN_ITEMCHANGED:
    {
      SetStdStatusAndMenu();
      break;
    }
    case NM_RETURN:
    case NM_DBLCLK:
    {
      OnCommand(hWnd, ID_EDIT_EDIT, pNmndr->hwndFrom, 0);
      break;
    }
    case LVN_DELETEITEM:
    {
      NM_LISTVIEW* plvItem = (NM_LISTVIEW*)pNmndr;
      if (plvItem->lParam)
      {
        delete (LPITEMDATA)plvItem->lParam;
      }
    }
    case LVN_INSERTITEM:
    {
      SortItems(-1, false);
      break;
    }
    case LVN_COLUMNCLICK:
    {
      NM_LISTVIEW* plvItem = (NM_LISTVIEW*)pNmndr;
      SortItems(plvItem->iSubItem, true);
      if (MoveYourApp::GetApp()->GetOptions()->bURAS)
        m_pDataHandler->UpdateRecord(m_hListView);
      break;
    }
    case HDN_ITEMCHANGING:
    {
      LPNMHEADER pHeader = (LPNMHEADER)pNmndr;
      if (pHeader->iItem == 1)
      {
        RECT Rect;
        GetWindowRect(m_hListView, &Rect);
        ScreenToClient(hWnd, &Rect);
        AdjustRect(0, 0, Rect.right, Rect.bottom, false);
        break;
      }
    }
  }
  return false;
}

bool MoveYourWnd::OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
{
  if (MoveYourApp::GetApp()->IsWaitCursor())
  {
    SetCursor(MoveYourApp::GetApp()->GetWaitCursor());
    return true;
  }

  return FORWARD_WM_SETCURSOR(hWnd, hWndCursor, codeHitTest, msg, ::DefWindowProc);
  //return false;
}

void MoveYourWnd::OnActivate(HWND hWnd, UINT nState, HWND hWndActDeact, bool fMinimized)
{
  if (nState == WA_ACTIVE)
    SetFocus(m_hListView);
}

void MoveYourWnd::OnMeasureItem(HWND hWnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
  if (lpMeasureItem->CtlType == ODT_MENU)
  {
    lpMeasureItem->itemHeight = 16;
    lpMeasureItem->itemWidth = 16;
  }
}

void MoveYourWnd::OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem)
{
  if (lpDrawItem->CtlType == ODT_MENU)
  {
    HIMAGELIST hImageList = (HIMAGELIST)SendMessage(m_hToolBar, TB_GETIMAGELIST, 0, 0);
    if (hImageList)
    {
      TBBUTTONINFO tbi = { 0 };
      tbi.cbSize = sizeof(tbi);
      tbi.dwMask = TBIF_IMAGE;
      if (SendMessage(m_hToolBar, TB_GETBUTTONINFO, lpDrawItem->itemID, (LPARAM)&tbi) != -1)
        ImageList_Draw(hImageList, tbi.iImage, lpDrawItem->hDC, lpDrawItem->rcItem.left - 16, lpDrawItem->rcItem.top, ILD_NORMAL);
    }
  }
}

void MoveYourWnd::OnInitMenuPopup(HWND hWnd, HMENU hMenu, UINT item, bool fSystemMenu)
{
  if (!fSystemMenu)
  {
    HIMAGELIST hImageList = (HIMAGELIST)SendMessage(m_hToolBar, TB_GETIMAGELIST, 0, 0);
    if (hImageList)
    {
      int nCount = GetMenuItemCount(hMenu);
      for (int i = 0; i < nCount; i++)
      {
        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_ID | MIIM_BITMAP;
        GetMenuItemInfo(hMenu, i, true, &mii);

        if (mii.hbmpItem)
          continue;

        if (mii.wID == ID_HELP_ABOUT)
        {
          mii.fMask = MIIM_BITMAP;
          mii.hbmpItem = HBMMENU_SYSTEM;
        }
        else
        {
          mii.fMask = MIIM_BITMAP;
          mii.hbmpItem = HBMMENU_CALLBACK;
        }
        SetMenuItemInfo(hMenu, i, true, &mii);
      }
    }
  }
}

void MoveYourWnd::OnNCDestroy(HWND hWnd)
{
}

void MoveYourWnd::OnChangeCBChain(HWND hWnd, HWND hWndRemove, HWND hWndNext)
{
  if (m_hClipNextView = (HWND)hWndRemove)
    m_hClipNextView = (HWND)hWndNext;
  else
    SendMessage(m_hClipNextView, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndNext);
}

void MoveYourWnd::OnDrawClipboard(HWND hWnd)
{
  HMENU hMainMenu = GetMenu(hWnd);
  HMENU hMenu = GetSubMenu(hMainMenu, 1);

  EnableMenuItem(hMenu, ID_EDIT_PASTE, MF_BYCOMMAND |
    (IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED));

  SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_PASTE, MAKELONG(IsClipboardFormatAvailable(CF_TEXT), 0));
  SendMessage(m_hClipNextView, WM_DRAWCLIPBOARD, 0, 0);
}

void MoveYourWnd::OnDropFiles(HWND hWnd, HDROP hDrop)
{
  TCHAR szFileName[MAX_PATH];
  int nCount = DragQueryFile(hDrop, -1, nullptr, 0);
  for (int i = 0; i < nCount; i++)
  {
    DragQueryFile(hDrop, i, szFileName, MAX_PATH);
    long lCount = ListView_GetItemCount(m_hListView);
    if (m_pDataHandler->OnInportScript(hWnd, m_hListView, MoveYourApp::GetApp()->GetInstance(), szFileName))
    {
      if (!lCount)
        ListView_SetItemState(m_hListView, 0, LVIS_SELECTED, LVIS_SELECTED);
      m_pDataHandler->UpdateRecord(m_hListView);
      SetStdStatusAndMenu();
    }
  }
}

void MoveYourWnd::ReadWindowPos(LPCTSTR lpszFolder, LPCTSTR lpszKey)
{
  WINDOWPLACEMENT WinPos;
  if (GetRegBinary(lpszFolder, lpszKey, &WinPos, sizeof(WINDOWPLACEMENT), HKEY_CURRENT_USER))
    SetWindowPlacement(m_hWnd, &WinPos);
  else
    ShowWindow(MoveYourApp::GetApp()->GetMainWnd()->GetHWND(), SW_SHOWDEFAULT);
}

void MoveYourWnd::WriteWindowPos(LPCTSTR lpszFolder, LPCTSTR lpszKey) const
{
  WINDOWPLACEMENT WinPos = { 0 };
  WinPos.length = sizeof(WINDOWPLACEMENT);
  GetWindowPlacement(m_hWnd, &WinPos);
  SetRegBinary(lpszFolder, lpszKey, &WinPos, sizeof(WINDOWPLACEMENT), HKEY_CURRENT_USER);
}

bool MoveYourWnd::Init()
{
  WNDCLASSEX wc = { 0 };

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WindowBase::s_WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = MoveYourApp::GetApp()->GetInstance();
  wc.hIcon = (HICON)LoadImage(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 32, 32, 0);
  wc.hIconSm = (HICON)LoadImage(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 16, 16, 0);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);
  wc.lpszClassName = s_szWindowClass;

  if (!RegisterClassEx(&wc))
  {
    ShowOSError();
    return false;
  }

  return true;
}

bool MoveYourWnd::Create(int nCmdShow)
{
  if (!WindowBase::Create(WS_EX_ACCEPTFILES, s_szWindowClass, MoveYourApp::GetApp()->GetAppName()))
  {
    ShowOSError();
    return 1;
  }

  _ASSERTE(m_hWnd);

  if ((nCmdShow == SW_SHOWNORMAL) || (nCmdShow == SW_SHOWDEFAULT))
    ReadWindowPos(MoveYourApp::s_szMainKey, s_szWindowPos);
  else
    ShowWindow(m_hWnd, nCmdShow);

  UpdateWindow(m_hWnd);

  return true;
}

void MoveYourWnd::AdjustRect(int x, int y, int cx, int cy, bool bMove /*= true*/)
{
  if (bMove)
    MoveWindow(m_hListView, x, y, cx, cy, true);

  LV_COLUMN TaskList;
  int ncx;
  TaskList.mask = LVCF_WIDTH;
  ListView_GetColumn(m_hListView, 0, &TaskList);
  ncx = TaskList.cx;
  ListView_GetColumn(m_hListView, 1, &TaskList);
  ncx += TaskList.cx;
  if (cx > (ncx + COL3X))
    TaskList.cx = cx - ncx - 4;
  else
    TaskList.cx = COL3X - 4;
  ListView_SetColumn(m_hListView, 2, &TaskList);
}

void MoveYourWnd::InitList()
{
  SendMessage(m_hListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

  tstring str;
  LV_COLUMN TaskList;

  TaskList.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
  TaskList.fmt = LVCFMT_CENTER | LVCFMT_FIXED_WIDTH;
  TaskList.cx = COL1X;
  TaskList.cchTextMax = 255;
  str = LoadString(IDS_COL1);
  TaskList.pszText = &str.front();
  TaskList.iSubItem = -1;
  ListView_InsertColumn(m_hListView, 0, &TaskList);

  TaskList.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
  TaskList.fmt = LVCFMT_LEFT;
  TaskList.cchTextMax = 255;

  str = LoadString(IDS_COL2);
  TaskList.cx = COL2X;
  TaskList.pszText = &str.front();
  ListView_InsertColumn(m_hListView, 1, &TaskList);

  str = LoadString(IDS_COL3);
  TaskList.cx = COL3X;
  TaskList.pszText = &str.front();
  ListView_InsertColumn(m_hListView, 2, &TaskList);

  // Init Header column
  HWND hHeader = ListView_GetHeader(m_hListView);
  if (!hHeader)
    return;

  HD_ITEM hdItem = { 0 };
  hdItem.mask = HDI_FORMAT;
  if (!Header_GetItem(hHeader, 0, &hdItem))
    return;

  m_hBitmap = ::CreateMappedBitmap(MoveYourApp::GetApp()->GetInstance(), IDB_ICONCOLUMN, 0, nullptr, 0);
  if (!m_hBitmap)
    return;

  hdItem.mask |= HDI_BITMAP;
  hdItem.fmt |= HDF_BITMAP | HDF_FIXEDWIDTH;
  hdItem.hbm = m_hBitmap;
  Header_SetItem(hHeader, 0, &hdItem);
}

void MoveYourWnd::InsertItem(LPITEMDATA lpItemData, long nIndex)
{
  std::unique_ptr<ITEMDATA> spItemData(lpItemData);

  LV_ITEM lvItem;
  lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
  lvItem.iItem = nIndex;
  lvItem.state = 0;
  lvItem.stateMask = 0;
  lvItem.iImage = *lpItemData->szDestination ? 1 : 0;

  TCHAR szTemp[10] = { 0 };
  _ltot_s(nIndex + 1, szTemp, 10);

  lvItem.pszText = szTemp;
  lvItem.cchTextMax = _tcslen(szTemp);
  lvItem.iSubItem = 0;
  lvItem.lParam = (LONG_PTR)lpItemData;
  ListView_InsertItem(m_hListView, &lvItem);

  lvItem.mask = LVIF_TEXT;
  lvItem.pszText = lpItemData->szSource;
  lvItem.cchTextMax = _tcslen(lpItemData->szSource);
  lvItem.iSubItem = 1;
  ListView_SetItem(m_hListView, &lvItem);

  tstring str;

  if (*lpItemData->szDestination)
  {
    lvItem.pszText = lpItemData->szDestination;
    lvItem.cchTextMax = _tcslen(lpItemData->szDestination);
  }
  else
  {
    str = LoadString(IDS_NULL);
    lvItem.pszText = &str.front();
    lvItem.cchTextMax = str.size();
  }

  lvItem.iSubItem = 2;
  ListView_SetItem(m_hListView, &lvItem);

  spItemData.release();
}

void MoveYourWnd::AddToList(LPITEMDATA lpItemData)
{
  InsertItem(lpItemData, ListView_GetItemCount(m_hListView));
}

bool MoveYourWnd::SetListItemData(LPITEMDATA lpItemData, long nIndex)
{
  std::unique_ptr<ITEMDATA> spItemData(lpItemData);

  if (nIndex == -1)
  {
    nIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED | LVNI_FOCUSED);
    if (nIndex == -1)
      return false;
  }

  LV_ITEM lvItem = { 0 };
  lvItem.mask = LVIF_PARAM;
  lvItem.iItem = nIndex;
  if (ListView_GetItem(m_hListView, &lvItem) && lvItem.lParam)
  {
    delete (LPITEMDATA)lvItem.lParam;
  }

  memset(&lvItem, 0, sizeof(LV_ITEM));
  lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
  lvItem.iItem = nIndex;
  lvItem.state = 0;
  lvItem.stateMask = 0;
  lvItem.iImage = *lpItemData->szDestination ? 1 : 0;

  TCHAR szTemp[10] = { 0 };
  _ltot_s(nIndex + 1, szTemp, 10);
  lvItem.pszText = szTemp;
  lvItem.cchTextMax = _tcslen(szTemp);
  lvItem.iSubItem = 0;
  lvItem.lParam = (LONG_PTR)lpItemData;
  ListView_SetItem(m_hListView, &lvItem);

  lvItem.mask = LVIF_TEXT;
  lvItem.pszText = lpItemData->szSource;
  lvItem.cchTextMax = _tcslen(lpItemData->szSource);
  lvItem.iSubItem = 1;
  ListView_SetItem(m_hListView, &lvItem);

  tstring str;

  if (*lpItemData->szDestination)
  {
    lvItem.pszText = lpItemData->szDestination;
    lvItem.cchTextMax = _tcslen(lpItemData->szDestination);
  }
  else
  {
    str = LoadString(IDS_NULL);
    lvItem.pszText = &str.front();
    lvItem.cchTextMax = str.size();
  }

  lvItem.iSubItem = 2;
  ListView_SetItem(m_hListView, &lvItem);

  spItemData.release();
  return true;
}

bool MoveYourWnd::GetListItemData(LPITEMDATA lpItemData, long nIndex)
{
  if (nIndex == -1)
  {
    nIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED | LVNI_FOCUSED);
    if (nIndex == -1)
      return false;
  }

  LV_ITEM lvItem = { 0 };
  lvItem.mask = LVIF_PARAM;
  lvItem.iItem = nIndex;
  if (ListView_GetItem(m_hListView, &lvItem))
  {
    memcpy(lpItemData, (LPITEMDATA)lvItem.lParam, sizeof(ITEMDATA));
    return true;
  }
  return false;
}

bool MoveYourWnd::GetListItemData(LPITEMDATA lpItemData, bool bSelection, int& nIndex)
{
  if (bSelection)
  {
    nIndex = ListView_GetNextItem(m_hListView, nIndex, LVNI_SELECTED);
    if (nIndex == -1)
      return false;
  }
  return GetListItemData(lpItemData, nIndex);
}

int CALLBACK MoveYourWnd::CompareListItem(LPITEMDATA lpItemData1, LPITEMDATA lpItemData2, long nSortData)
{
  short nColumn = LOWORD(nSortData);
  short nSortOrder = HIWORD(nSortData);

  switch (nColumn)
  {
    case 1:
    {
      if (nSortOrder)
        return _tcscmp(lpItemData1->szSource, lpItemData2->szSource);
      else
        return _tcscmp(lpItemData2->szSource, lpItemData1->szSource);
    }
    case 2:
    {
      if (nSortOrder)
        return _tcscmp(lpItemData1->szDestination, lpItemData2->szDestination);
      else
        return _tcscmp(lpItemData2->szDestination, lpItemData1->szDestination);
    }
  }
  return 0;
}

void MoveYourWnd::DrawColumnImage(HWND hHeader, int nColumn, short nOrder, bool bBitmap)
{
  HD_ITEM hdItem = { 0 };
  hdItem.mask = HDI_FORMAT;
  if (!Header_GetItem(hHeader, nColumn, &hdItem))
    return;

  if (bBitmap)
  {
    hdItem.mask = HDI_FORMAT;
    hdItem.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
    hdItem.fmt |= nOrder ? HDF_SORTUP : HDF_SORTDOWN;
  }
  else
  {
    hdItem.mask = HDI_FORMAT;
    hdItem.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
  }

  Header_SetItem(hHeader, nColumn, &hdItem);
}

void MoveYourWnd::SortItems(short nColumn, bool bBitmap)
{
  if (!nColumn)
    return;

  HWND hHeader = ListView_GetHeader(m_hListView);
  if (!hHeader)
    return;

  short nSortOrder = 1;

  if (nColumn != -1)
  {
    HDITEM HdItem = { 0 };
    HdItem.mask |= HDI_LPARAM;
    if (!Header_GetItem(hHeader, nColumn, &HdItem))
      return;

    HdItem.lParam = !HdItem.lParam;
    nSortOrder = (short)HdItem.lParam;
    Header_SetItem(hHeader, nColumn, &HdItem);
  }

  for (int i = 1; i < 3; i++)
    DrawColumnImage(hHeader, i, (i != nColumn) ? 0 : nSortOrder, (i != nColumn) ? false : bBitmap);

  if (nColumn != -1)
    ListView_SortItems(m_hListView, (PFNLVCOMPARE)CompareListItem, MAKELONG(nColumn, nSortOrder));
}

int MoveYourWnd::GetStartItem()
{
  int nIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
  if (nIndex == -1)
    return 0;
  return nIndex + 1;
}

bool MoveYourWnd::OnEndFind(int nIndex)
{
  int nLastIndex = -1;
  while ((nLastIndex = ListView_GetNextItem(m_hListView, nLastIndex, LVNI_SELECTED)) != -1)
    ListView_SetItemState(m_hListView, nLastIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
  ListView_SetItemState(m_hListView, nIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
  return true;
}

bool MoveYourWnd::OnFind(bool bNext /*= false*/)
{
  WaitCursor wc;

  if (!m_pFindDlg)
    m_pFindDlg = std::make_unique<FindDlg>();

  if (!bNext || m_pFindDlg->GetFindText().empty())
  {
    if (IDOK != m_pFindDlg->DoModal(m_hWnd))
      return false;
  }

  ITEMDATA ItemData;
  TCHAR szString1[256], szString2[256];
  _tcscpy_s(szString2, m_pFindDlg->GetFindText().c_str());
  _tcsupr_s(szString2);

  int nIndex = -1;
  for (int i = GetStartItem(); GetListItemData(&ItemData, i); i++)
  {
    _tcscpy_s(szString1, ItemData.szSource);
    _tcsupr_s(szString1);
    if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_LOOKATSRC)
    {
      if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_WORDONLY)
      {
        if (!_tcscmp(szString1, szString2))
        {
          return OnEndFind(i);
        }
      }
      else
      {
        if (_tcsstr(szString1, szString2))
        {
          return OnEndFind(i);
        }
      }
    }

    _tcscpy_s(szString1, ItemData.szDestination);
    _tcsupr_s(szString1);

    if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_LOOKATDST)
    {
      if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_WORDONLY)
      {
        if (!_tcscmp(szString1, szString2))
        {
          return OnEndFind(i);
        }
      }
      else
      {
        if (_tcsstr(szString1, szString2))
        {
          return OnEndFind(i);
        }
      }
    }
  }

  if (!m_pFindDlg->GetFindText().empty())
  {
    ReportBox(StrFormat(LoadString(IDS_CANNOT_FIND).c_str(), m_pFindDlg->GetFindText().c_str()).c_str(), MB_ICONINFORMATION | MB_OK);
  }

  return false;
}
