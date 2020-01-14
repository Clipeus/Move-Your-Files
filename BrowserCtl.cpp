/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Browser Control
*/

#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <tchar.h>
#include <crtdbg.h>
#include "BrowserCtl.h"
#include "Resource.h"

#define BUTTONWIDTH		21

#define EDIT_ID			0x0001
#define BUTTON_ID		0x0002

LRESULT CALLBACK EditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK BrCtlWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct tagBRWSDATA
{
  UINT uHotKey;
  bool bPressed;
  WNDPROC oldWndPros;
} BRWSDATA, * LPBRWSDATA;

bool InitBrowserCtl()
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = 0;
  wcex.lpfnWndProc = (WNDPROC)BrCtlWinProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = GetModuleHandle(nullptr);
  wcex.hIcon = nullptr;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = nullptr;
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = _T("FileBrowser");
  wcex.hIconSm = nullptr;

  return RegisterClassEx(&wcex);
}

bool BrowserCtl_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
  HWND hEdit = nullptr, hButton = nullptr;
  RECT Rect;
  GetClientRect(hWnd, &Rect);
  int x = Rect.left, y = Rect.top, cx = Rect.right - Rect.left, cy = Rect.bottom - Rect.top;
  if (lpCreateStruct->style & FBS_RIGHTBUTTON)
  {
    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), nullptr, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP, x + BUTTONWIDTH + 3, y,
      cx, cy, hWnd, (HMENU)EDIT_ID, lpCreateStruct->hInstance, nullptr);
    if (!hEdit)
      return false;

    if (lpCreateStruct->style & FBS_TABSTOPBUTTON)
    {
      hButton = CreateWindow(_T("BUTTON"), nullptr, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW | WS_TABSTOP,
        x, y, BUTTONWIDTH, cy, hWnd, (HMENU)BUTTON_ID, lpCreateStruct->hInstance, nullptr);
      if (!hButton)
        return false;
    }
  }
  else
  {
    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), nullptr, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP, x, y, cx - BUTTONWIDTH - 3, cy,
      hWnd, (HMENU)EDIT_ID, lpCreateStruct->hInstance, nullptr);
    if (!hEdit)
      return false;

    if (lpCreateStruct->style & FBS_TABSTOPBUTTON)
    {
      hButton = CreateWindow(_T("BUTTON"), nullptr, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW | WS_TABSTOP,
        cx - BUTTONWIDTH, y, BUTTONWIDTH, cy, hWnd, (HMENU)BUTTON_ID, lpCreateStruct->hInstance, nullptr);
      if (!hButton)
        return false;
    }
  }

  LPBRWSDATA lpData = new BRWSDATA;
  lpData->uHotKey = 0;
  lpData->bPressed = false;
  if (hEdit)
  {
    lpData->oldWndPros = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);
    SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)lpData);
    SendMessage(hEdit, WM_SETFONT, SendMessage(GetParent(hWnd), WM_GETFONT, 0, 0), -1);
  }
  else
  {
    lpData->oldWndPros = nullptr;
  }

  if (hButton)
    SendMessage(hButton, WM_SETFONT, SendMessage(GetParent(hWnd), WM_GETFONT, 0, 0), -1);

  SetWindowLongPtr(hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | lpCreateStruct->style);
  SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_CONTROLPARENT | lpCreateStruct->dwExStyle);
  return true;
}

bool OnSelectFile(HWND hWnd)
{
  OPENFILENAME ofn;
  TCHAR szFileName[MAX_PATH] = _T("");
  TCHAR szFilter[256];

  GetWindowText(hWnd, szFilter, 255);
  LPTSTR lpszTemp = &szFilter[-1];
  while (true)
  {
    lpszTemp = _tcschr(&lpszTemp[1], _T('#'));
    if (lpszTemp)
      lpszTemp[0] = 0;
    else
      break;
  }
  GetDlgItemText(hWnd, EDIT_ID, szFileName, 255);

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd;
  ofn.hInstance = GetModuleHandle(nullptr);
  ofn.lpstrFilter = szFilter;
  ofn.lpstrCustomFilter = nullptr;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 1;;
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFileTitle = nullptr;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = nullptr;
  ofn.lpstrTitle = nullptr;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = nullptr;
  ofn.lCustData = 0;
  ofn.lpfnHook = nullptr;
  ofn.lpTemplateName = nullptr;
  if (GetOpenFileName(&ofn))
  {
    SetDlgItemText(hWnd, EDIT_ID, szFileName);
    return true;
  }
  return false;
}

bool BrowserCtl_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  if ((id == BUTTON_ID) && (codeNotify == BN_CLICKED))
  {
    if (!SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hWnd), FBN_BROWSING), (LPARAM)hWnd))
    {
      if (OnSelectFile(hWnd))
      {
        SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hWnd), FBN_BROWSED), (LPARAM)hWnd);
        return true;
      }
    }
  }
  return false;
}

bool BrowserCtl_OnEraseBkgnd(HWND hWnd, HDC hDC)
{
  bool fEnable = IsWindowEnabled(hWnd);
  HWND hControl = GetDlgItem(hWnd, EDIT_ID);
  if (hControl)
    EnableWindow(hControl, fEnable);
  hControl = GetDlgItem(hWnd, BUTTON_ID);
  if (hControl)
    EnableWindow(hControl, fEnable);
  HBRUSH hBrush;
  if (fEnable)
    hBrush = (HBRUSH)DefWindowProc(hWnd, WM_CTLCOLOREDIT, (WPARAM)hDC, (LPARAM)hWnd);
  else
    hBrush = (HBRUSH)DefWindowProc(hWnd, WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)hWnd);
  RECT Rect;
  GetClientRect(hWnd, &Rect);
  FillRect(hDC, &Rect, hBrush);
  return false;
}

void BrowserCtl_OnEnable(HWND hWnd, bool fEnable)
{
  InvalidateRect(hWnd, nullptr, true);
  UpdateWindow(hWnd);
}

void DrawPicture(HDC hDC, LPRECT lpRect, UINT nCtlID, HWND hWndItem, bool bDisable)
{
  HBITMAP hBitmap = nullptr;
  COLORMAP ColorMap[1];
  ColorMap[0].from = RGB(255, 255, 255);
  ColorMap[0].to = bDisable ? RGB(255, 255, 255) : GetSysColor(COLOR_3DFACE);

  hBitmap = CreateMappedBitmap(GetModuleHandle(nullptr), IDB_OPEN, 0, ColorMap, 1);

  BITMAP bm;
  GetObject(hBitmap, sizeof(bm), &bm);

  DrawState(hDC, nullptr, nullptr, (LPARAM)hBitmap, (WPARAM)0, lpRect->left + ((lpRect->right - lpRect->left) >> 1) - (bm.bmWidth >> 1),
    lpRect->top + ((lpRect->bottom - lpRect->top) >> 1) - (bm.bmHeight >> 1), bm.bmWidth, bm.bmHeight,
    bDisable ? DSS_DISABLED | DST_BITMAP : DST_BITMAP);
  DeleteObject(hBitmap);
}

void BrowserCtl_OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem)
{
  RECT Rect;
  CopyRect(&Rect, &lpDrawItem->rcItem);

  if (lpDrawItem->itemState & ODS_SELECTED)
  {
    Rect.right += 1;
    Rect.bottom += 2;
    DrawFrameControl(lpDrawItem->hDC, (LPRECT)&lpDrawItem->rcItem, DFC_BUTTON, DFCS_PUSHED | DFCS_BUTTONPUSH);
  }
  else if (lpDrawItem->itemState & ODS_DISABLED)
    DrawFrameControl(lpDrawItem->hDC, (LPRECT)&lpDrawItem->rcItem, DFC_BUTTON, DFCS_INACTIVE | DFCS_BUTTONPUSH);
  else
    DrawFrameControl(lpDrawItem->hDC, (LPRECT)&lpDrawItem->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);

  DrawPicture(lpDrawItem->hDC, &Rect, lpDrawItem->CtlID, lpDrawItem->hwndItem, lpDrawItem->itemState & ODS_DISABLED);

  if (lpDrawItem->itemState & ODS_FOCUS)
  {
    RECT Rect;
    CopyRect(&Rect, &lpDrawItem->rcItem);
    InflateRect(&Rect, -2, -2);
    DrawFocusRect(lpDrawItem->hDC, &Rect);
  }
}

void SetButtonPressed(HWND hWnd, bool bPressed)
{
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
  {
    LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hEdit, GWLP_USERDATA);
    lpData->bPressed = bPressed;
    SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)lpData);
  }
}

bool IsButtonPressed(HWND hWnd)
{
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
  {
    LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hEdit, GWLP_USERDATA);
    return lpData->bPressed;
  }
  return false;
}

void BrowserCtl_OnDrawButton(HWND hWnd, HDC hDC, int itemState = -1)
{
  DRAWITEMSTRUCT DrawItem;
  DrawItem.CtlType = ODT_BUTTON;
  DrawItem.CtlID = 0;
  DrawItem.itemID = 0;
  DrawItem.itemAction = 0;
  if (itemState == -1)
  {
    if (!IsWindowEnabled(hWnd))
      DrawItem.itemState = ODS_DISABLED;
    else if (IsButtonPressed(hWnd))
      DrawItem.itemState = ODS_SELECTED;
    else
      DrawItem.itemState = 0;
  }
  else
  {
    DrawItem.itemState = itemState;
  }
  DrawItem.hwndItem = 0;
  DrawItem.hDC = hDC;
  GetClientRect(hWnd, &DrawItem.rcItem);
  DrawItem.rcItem.left = DrawItem.rcItem.right - BUTTONWIDTH;
  DrawItem.itemData = 0;
  BrowserCtl_OnDrawItem(hWnd, &DrawItem);
}

void BrowserCtl_OnPaint(HWND hWnd)
{
  DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
  if (FBS_TABSTOPBUTTON & dwStyle)
  {
    DefWindowProc(hWnd, WM_PAINT, 0, 0);
    return;
  }
  PAINTSTRUCT ps;
  HDC hDC = BeginPaint(hWnd, &ps);
  BrowserCtl_OnDrawButton(hWnd, hDC);
  EndPaint(hWnd, &ps);
}

void BrowserCtl_OnLButtonUp(HWND hWnd, int nX, int nY, UINT uKeyFlags)
{
  if (hWnd == GetCapture())
    ReleaseCapture();
  DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
  if (FBS_TABSTOPBUTTON & dwStyle)
    return;
  if (!IsWindowEnabled(hWnd))
    return;

  RECT Rect;
  POINT Point = { nX, nY };
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
  {
    GetClientRect(hEdit, &Rect);
    if (!PtInRect(&Rect, Point))
    {
      GetClientRect(hWnd, &Rect);
      if (PtInRect(&Rect, Point) && IsButtonPressed(hWnd))
      {
        if (IsButtonPressed(hWnd))
          BrowserCtl_OnCommand(hWnd, BUTTON_ID, (HWND)-1, BN_CLICKED);
      }
    }
  }

  HDC hDC = GetDC(hWnd);
  SetButtonPressed(hWnd, false);
  BrowserCtl_OnDrawButton(hWnd, hDC, 0);
  ReleaseDC(hWnd, hDC);
}

void BrowserCtl_OnLButtonDown(HWND hWnd, bool fDoubleClick, int nX, int nY, UINT uKeyFlags)
{
  DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
  if (FBS_TABSTOPBUTTON & dwStyle)
    return;
  if (!IsWindowEnabled(hWnd))
    return;

  SetCapture(hWnd);

  HDC hDC = GetDC(hWnd);
  SetButtonPressed(hWnd, true);
  BrowserCtl_OnDrawButton(hWnd, hDC, ODS_SELECTED);
  ReleaseDC(hWnd, hDC);
}

void BrowserCtl_OnMouseMove(HWND hWnd, int nX, int nY, UINT uKeyFlags)
{
  DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
  if (FBS_TABSTOPBUTTON & dwStyle)
    return;
  if (!IsWindowEnabled(hWnd))
    return;

  HDC hDC = GetDC(hWnd);
  RECT Rect;
  POINT Point = { nX, nY };
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
  {
    GetClientRect(hEdit, &Rect);
    if (!PtInRect(&Rect, Point))
    {
      GetClientRect(hWnd, &Rect);
      if (PtInRect(&Rect, Point) && IsButtonPressed(hWnd))
      {
        BrowserCtl_OnDrawButton(hWnd, hDC, ODS_SELECTED);
      }
      else
      {
        BrowserCtl_OnDrawButton(hWnd, hDC, 0);
      }
    }
    else
    {
      BrowserCtl_OnDrawButton(hWnd, hDC, 0);
    }
  }
  else
  {
    BrowserCtl_OnDrawButton(hWnd, hDC, 0);
  }
  ReleaseDC(hWnd, hDC);
}

void BrowserCtl_OnSetFocus(HWND hWnd, HWND hwndOldFocus)
{
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
    SetFocus(hEdit);
}

void BrowserCtl_OnSysColorChange(HWND hWnd)
{
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
    SendMessage(hEdit, WM_SYSCOLORCHANGE, 0, 0);
}

void BrowserCtl_OnDestroy(HWND hWnd)
{
  HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
  if (hEdit)
  {
    LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hEdit, GWLP_USERDATA);
    if (lpData)
    {
      SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)lpData->oldWndPros);
      delete lpData;
    }
  }
}

LRESULT CALLBACK BrCtlWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case FBM_GETFILENAME:
    {
      return GetDlgItemText(hWnd, EDIT_ID, (LPTSTR)wParam, static_cast<int>(lParam));
    }
    case FBM_SETFILENAME:
    {
      return SetDlgItemText(hWnd, EDIT_ID, (LPTSTR)wParam);
    }
    case FBM_BROWSERFILES:
    {
      return BrowserCtl_OnCommand(hWnd, BUTTON_ID, (HWND)-1, BN_CLICKED);
    }
    case FBM_GETEDITCTL:
    {
      return (LRESULT)GetDlgItem(hWnd, EDIT_ID);
    }
    case FBM_GETBUTTON:
    {
      return (LRESULT)GetDlgItem(hWnd, BUTTON_ID);
    }
    case FBM_GETHOTKEY:
    {
      HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
      if (hEdit)
      {
        LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hEdit, GWLP_USERDATA);
        return lpData->uHotKey;
      }
      return 0;
    }
    case FBM_SETHOTKEY:
    {
      HWND hEdit = GetDlgItem(hWnd, EDIT_ID);
      if (hEdit)
      {
        LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hEdit, GWLP_USERDATA);
        lpData->uHotKey = (UINT)MAKELONG((WORD)wParam, (WORD)lParam);
        SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)lpData);
        return 1;
      }
      return 0;
    }
    HANDLE_MSG(hWnd, WM_LBUTTONDOWN, BrowserCtl_OnLButtonDown);
    HANDLE_MSG(hWnd, WM_LBUTTONUP, BrowserCtl_OnLButtonUp);
    HANDLE_MSG(hWnd, WM_MOUSEMOVE, BrowserCtl_OnMouseMove);
    HANDLE_MSG(hWnd, WM_ENABLE, BrowserCtl_OnEnable);
    HANDLE_MSG(hWnd, WM_ERASEBKGND, BrowserCtl_OnEraseBkgnd);
    HANDLE_MSG(hWnd, WM_CREATE, BrowserCtl_OnCreate);
    HANDLE_MSG(hWnd, WM_DRAWITEM, BrowserCtl_OnDrawItem);
    HANDLE_MSG(hWnd, WM_PAINT, BrowserCtl_OnPaint);
    HANDLE_MSG(hWnd, WM_COMMAND, BrowserCtl_OnCommand);
    HANDLE_MSG(hWnd, WM_DESTROY, BrowserCtl_OnDestroy);
    HANDLE_MSG(hWnd, WM_SETFOCUS, BrowserCtl_OnSetFocus);
    HANDLE_MSG(hWnd, WM_SYSCOLORCHANGE, BrowserCtl_OnSysColorChange);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK EditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_KEYDOWN)
  {
    LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (lpData->uHotKey)
    {
      if (LOWORD(lpData->uHotKey) == wParam)
        BrowserCtl_OnCommand(GetParent(hWnd), BUTTON_ID, (HWND)-1, BN_CLICKED);
    }
  }

  LPBRWSDATA lpData = (LPBRWSDATA)GetWindowLongPtr(hWnd, GWLP_USERDATA);
  _ASSERTE(lpData->oldWndPros);
  return CallWindowProc(lpData->oldWndPros, hWnd, uMsg, wParam, lParam);
}
