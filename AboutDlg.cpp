/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	About Dialog
*/

#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <stdio.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "AboutDlg.h"
#include "Utils.h"

AboutDlg::AboutDlg() : DialogBase(MAKEINTRESOURCE(IDD_ABOUT))
{

}

bool AboutDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  TCHAR szVerInfo[256] = { 0 };
  ::GetDlgItemText(hWnd, IDC_VERSION, szVerInfo, 255);

  VS_FIXEDFILEINFO FixedInfo = { 0 };
  GetFixedInfo(FixedInfo);
  _stprintf_s(szVerInfo, szVerInfo, HIWORD(FixedInfo.dwProductVersionMS), LOWORD(FixedInfo.dwProductVersionMS), HIWORD(FixedInfo.dwProductVersionLS), LOWORD(FixedInfo.dwProductVersionLS));

  ::SetDlgItemText(hWnd, IDC_VERSION, szVerInfo);

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool AboutDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}
