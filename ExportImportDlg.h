/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Export and Import dialog
*/

#pragma once
#include "DialogBase.h"

class ExportImportDlg : public DialogBase
{
public:
  ExportImportDlg();

public:
  INT_PTR DoExportModal(HWND hWnd);
  INT_PTR DoImportModal(HWND hWnd);

private:
  bool DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
};

