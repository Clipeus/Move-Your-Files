/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	New and Edit dialog
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
#include "BrowserCtl.h"

NewEditDlg::NewEditDlg() : DialogBase(MAKEINTRESOURCE(IDD_NEW_EDIT))
{

}

void NewEditDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case IDOK:
    {
      if (m_pItemDataEx)
      {
        if (!FillListItems(hWnd, m_pItemDataEx))
          return;
      }
    }
    case IDCANCEL:
    {
      EndDialog(hWnd, id);
      return;
    }
    case IDC_DELETE:
    {
      HWND hControl;
      bool bEnable = SendMessage(hWndCtl, BM_GETCHECK, 0, 0) == BST_UNCHECKED;
      if (hControl = GetDlgItem(hWnd, IDC_EDIT2))
      {
        EnableWindow(hControl, bEnable);
      }
    }
  }
}

void NewEditDlg::FillDlgItems(HWND hWnd, LPITEMDATAEX lpItemDataEx)
{
  HWND hControl = GetDlgItem(hWnd, IDC_EDIT1);
  if (hControl)
  {
    SendMessage(hControl, FBM_SETHOTKEY, (WPARAM)VK_F4, 0);
    SendMessage(hControl, FBM_SETFILENAME, (WPARAM)lpItemDataEx->szSource, 0);
  }
  if (hControl = GetDlgItem(hWnd, IDC_EDIT2))
  {
    SendMessage(hControl, FBM_SETHOTKEY, (WPARAM)VK_F4, 0);
    if (lpItemDataEx->szDestination[0])
    {
      SendMessage(hControl, FBM_SETFILENAME, (WPARAM)lpItemDataEx->szDestination, 0);
      hControl = GetDlgItem(hWnd, IDC_DELETE);
      if (hControl)
        SendMessage(hControl, BM_SETCHECK, BST_UNCHECKED, 0);
    }
    else
    {
      SendMessage(hControl, FBM_SETFILENAME, 0, 0);
      EnableWindow(hControl, false);
      hControl = GetDlgItem(hWnd, IDC_DELETE);
      if (hControl)
        SendMessage(hControl, BM_SETCHECK, BST_CHECKED, 0);
    }
  }
}

bool NewEditDlg::FillListItems(HWND hWnd, LPITEMDATAEX lpItemDataEx)
{
  HWND hControl = GetDlgItem(hWnd, IDC_EDIT1);
  if (hControl)
  {
    SendMessage(hControl, FBM_GETFILENAME, (WPARAM)lpItemDataEx->szSource, 255);
    if (!lpItemDataEx->szSource || !lpItemDataEx->szSource[0])
    {
      if (IDCANCEL == ReportBox(IDS_EDIT_NEW_MES1, MB_OKCANCEL | MB_ICONWARNING))
      {
        EndDialog(hWnd, IDCANCEL);
        return false;
      }
      else
      {
        SetFocus(hControl);
        return false;
      }
    }
  }

  hControl = GetDlgItem(hWnd, IDC_DELETE);
  if (hControl)
  {
    if (SendMessage(hControl, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
    {
      hControl = GetDlgItem(hWnd, IDC_EDIT2);
      SendMessage(hControl, FBM_GETFILENAME, (WPARAM)lpItemDataEx->szDestination, 255);
      if (!lpItemDataEx->szDestination || !lpItemDataEx->szDestination[0])
      {
        if (IDCANCEL == ReportBox(StrFormat(LoadString(IDS_EDIT_NEW_MES2).c_str(), lpItemDataEx->szSource).c_str(), MB_OKCANCEL | MB_ICONWARNING))
        {
          SetFocus(hControl);
          return false;
        }
      }
    }
    else
    {
      lpItemDataEx->szDestination[0] = 0;
    }
  }
  return true;
}

bool NewEditDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  if (m_pItemDataEx->uAction == ID_EDIT_EDIT)
    SetWindowText(hWnd, LoadString(IDS_EDIT_CAPTION).c_str());
  else
    SetWindowText(hWnd, LoadString(IDS_NEW_CAPTION).c_str());

  FillDlgItems(hWnd, m_pItemDataEx);
  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool NewEditDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}
