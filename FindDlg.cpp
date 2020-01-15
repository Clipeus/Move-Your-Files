/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Handles MoveFiles data
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
#include "FindDlg.h"

FindDlg::FindDlg() : DialogBase(MAKEINTRESOURCE(IDD_FIND_DLG))
{

}

void FindDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case IDOK:
    {
      m_eFlags = Flags::FD_NONE;
      if (SendDlgItemMessage(hWnd, IDC_RADIO3, BM_GETCHECK, 0, 0))
        m_eFlags |= Flags::FD_LOOKATDST;
      else if (SendDlgItemMessage(hWnd, IDC_RADIO2, BM_GETCHECK, 0, 0))
        m_eFlags |= Flags::FD_LOOKATSRC;
      else
        m_eFlags |= Flags::FD_ALLFILES;

      if (SendDlgItemMessage(hWnd, IDC_WORD_ONLY, BM_GETCHECK, 0, 0))
        m_eFlags |= Flags::FD_WORDONLY;

      m_strFindText.resize(255);
      GetDlgItemText(hWnd, IDC_FINDTEXT, &m_strFindText.front(), 255);
      if (m_strFindText.empty())
      {
        EndDialog(hWnd, IDCANCEL);
        break;
      }
    }
    case IDCANCEL:
    {
      EndDialog(hWnd, id);
      break;
    }
    case IDC_FINDTEXT:
    {
      if (codeNotify == EN_CHANGE)
      {
        TCHAR szFindText[MAX_PATH] = _T("");
        GetDlgItemText(hWnd, IDC_FINDTEXT, szFindText, MAX_PATH);
        EnableWindow(GetDlgItem(hWnd, IDOK), szFindText[0]);
      }
      break;
    }
  }
}

bool FindDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  if ((m_eFlags & Flags::FD_LOOKATDST) && (m_eFlags & Flags::FD_LOOKATSRC))
    SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
  else if (m_eFlags & Flags::FD_LOOKATSRC)
    SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
  else if (m_eFlags & Flags::FD_LOOKATDST)
    SendDlgItemMessage(hWnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);

  if (m_eFlags & Flags::FD_WORDONLY)
    SendDlgItemMessage(hWnd, IDC_WORD_ONLY, BM_SETCHECK, BST_CHECKED, 0);
  if (!m_strFindText.empty())
    SetDlgItemText(hWnd, IDC_FINDTEXT, m_strFindText.c_str());
  else
    EnableWindow(GetDlgItem(hWnd, IDOK), false);

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool FindDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}
