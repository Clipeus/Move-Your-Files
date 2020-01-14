/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Handles MoveFiles data
*/

#pragma once

#include <vector>

class DataHandler
{
public:
  DataHandler();
  ~DataHandler();

public:
  bool UpdateRecord(HWND hListhWnd);
  bool FillList(HWND hListhWnd);
  bool OnSelectFile(HWND hWnd, HINSTANCE hInstance, bool bOpen, LPTSTR lpszName);
  bool OnExportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance);
  bool OnInportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance, LPCTSTR lpszFileName = nullptr);
  bool OnPaste(HWND hWnd, HWND hListView);
  bool OnCopyCut(HWND hWnd, HWND hListView, bool bCut);

private:
  bool DoGetFileRecord(HWND hListhWnd);
  DWORD GetDataLen();
  void TrimPath(LPTSTR lpszPath);
  LPTSTR CheckHeader(LPTSTR lpBuf);
  bool DoUpdateRecord(HWND hListhWnd);
  bool SetEQU(HWND hListhWnd);

public:
  static LPCTSTR s_szSysKey;

private:
  std::vector<TCHAR> m_vecRegBuf;
};
