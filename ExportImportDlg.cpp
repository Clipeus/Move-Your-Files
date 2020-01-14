/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Export and Import dialog
*/

#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <stdio.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "NewEditDlg.h"
#include "Utils.h"
#include "ExportImportDlg.h"

ExportImportDlg::ExportImportDlg()
{

}

INT_PTR ExportImportDlg::DoExportModal(HWND hWnd)
{
  return DoModal(hWnd, MAKEINTRESOURCE(IDD_EXPORT));
}

INT_PTR ExportImportDlg::DoImportModal(HWND hWnd)
{
  return DoModal(hWnd, MAKEINTRESOURCE(IDD_IMPORT));
}

void ExportImportDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case IDOK:
    {
      EndDialog(hWnd, (BST_CHECKED == SendDlgItemMessage(hWnd, IDC_RADIO1, BM_GETCHECK, 0, 0)) ? 1 : 2);
      break;
    }
    case IDCANCEL:
    {
      EndDialog(hWnd, 0);
      break;
    }
  }
}

bool ExportImportDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool ExportImportDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_DESTROY, OnDestroy);
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}

