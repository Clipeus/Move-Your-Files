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
#include <memory>
#include "MoveYour.h"
#include "AppOptions.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "DataHandler.h"
#include "Utils.h"
#include "ExportImportDlg.h"
#include "RegMonitor.h"

LPCTSTR DataHandler::s_szSysKey = _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager");
static TCHAR s_szSysVal[] = _T("PendingFileRenameOperations");
static TCHAR s_szSignature[] = _T("MOVEYOURFILES");
static TCHAR s_szPointer[] = _T(" > ");
static TCHAR s_szPointer2[] = _T("=");
static TCHAR s_szEOL[] = _T("\r\n");
static TCHAR s_szEO2L[] = _T("\r\n\r\n");
static TCHAR s_szNULL[] = _T("NUL");
static TCHAR s_szPrefix[] = _T("\\??\\");
static TCHAR s_szPrefix2[] = _T("!\\??\\");

DataHandler::DataHandler()
{

}

DataHandler::~DataHandler()
{

}

DWORD DataHandler::GetDataLen()
{
  DWORD dwSize = 0;
  HKEY hRawKey;

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, s_szSysKey, 0, KEY_ALL_ACCESS, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);
    DWORD dwRealType;

    if ((RegQueryValueEx(hKey.get(), s_szSysVal, 0, &dwRealType, nullptr, &dwSize) != ERROR_SUCCESS)
      && (REG_MULTI_SZ == dwRealType))
    {
      dwSize = 0;
    }
  }

  return dwSize;
}

bool DataHandler::DoGetFileRecord(HWND hListhWnd)
{
  DWORD dwSize = GetDataLen();
  if (!dwSize)
    return false;

  m_vecRegBuf.assign(dwSize / sizeof(TCHAR), 0);

  if (!GetRegMString(s_szSysKey, s_szSysVal, nullptr, &m_vecRegBuf[0], dwSize, HKEY_LOCAL_MACHINE))
  {
    ShowOSError();
    return false;
  }

  LPTSTR sz = m_vecRegBuf.data();
  for (int i = 0; *sz; i++)
  {
    bool bDelete = true;
    std::unique_ptr<ITEMDATA> spItemData = std::make_unique<ITEMDATA>();
    _tcscpy_s(spItemData->szSource, sz + _tcslen(s_szPrefix));
    _tcscpy_s(spItemData->szDestination, sz + _tcslen(sz) + 1);
    size_t lStep = _tcslen(s_szPrefix);

    if (spItemData->szDestination && spItemData->szDestination[0])
    {
      if (0 == _tcsncmp(spItemData->szDestination, s_szPrefix2, _tcslen(s_szPrefix2)))
        lStep = _tcslen(s_szPrefix2);

      memmove(spItemData->szDestination, spItemData->szDestination + lStep, (_tcslen(spItemData->szDestination) - lStep + 1) * sizeof(TCHAR));
      bDelete = false;
    }

    sz = sz + _tcslen(s_szPrefix) + _tcslen(spItemData->szDestination) + 1 + (bDelete ? 0 : lStep) + _tcslen(spItemData->szSource) + 1;
    MoveYourApp::GetApp()->GetMainWnd()->InsertItem(spItemData.release(), i);
  }
  
  return true;
}

void DataHandler::TrimPath(LPTSTR lpszPath)
{
  if (!lpszPath || !lpszPath[0])
    return;

  LPTSTR sz = lpszPath + _tcslen(lpszPath);
  sz = _tcsdec(lpszPath, sz);

  while (sz && _istspace(*sz))
    sz = _tcsdec(lpszPath, sz);

  if (sz)
    *_tcsinc(sz) = 0;

  sz = lpszPath;
  int i;

  for (i = 0; *sz && _istspace(*sz); i++)
    sz = _tcsinc(sz);

  if (i)
  {
    memmove(lpszPath, lpszPath + i, _tcslen(lpszPath) - i);
    lpszPath[_tcslen(lpszPath) - i] = 0;
  }
}

LPTSTR DataHandler::CheckHeader(LPTSTR lpBuf)
{
  LPTSTR sz = lpBuf;
  while (*sz && _istspace(*sz))
    sz = _tcsinc(sz);

  TCHAR szRename[] = _T("[rename]\r\n");
  size_t nLen = sizeof(szRename);

  if (_tcsnicmp(sz, szRename, nLen) - 1)
  {
    if (_tcsnicmp(sz, szRename, nLen) - 1)
      return nullptr;

    sz += nLen - 1;
    while (*sz && _istspace(*sz) && _tccmp(sz, _T("\n")))
      sz = _tcsinc(sz);

    if ((*sz != 0) && (!_tccmp(sz, _T("\n"))))
      return sz;
    else
      return nullptr;
  }

  return sz;
}

bool DataHandler::DoUpdateRecord(HWND hListhWnd)
{
  std::vector<TCHAR> vecRegBuf;
  std::vector<TCHAR> vecData;
  size_t nLen = 0;
  DWORD dwSize = GetDataLen();

  if (dwSize)
  {
    vecRegBuf.assign(dwSize / sizeof(TCHAR), 0);
    if (!GetRegMString(s_szSysKey, s_szSysVal, nullptr, vecRegBuf.data(), dwSize, HKEY_LOCAL_MACHINE))
    {
      ShowOSError();
      return false;
    }
  }

  if (m_vecRegBuf == vecRegBuf)
  {
    vecData.assign((_tcslen(s_szPrefix) + MAX_PATH) * 2 * ListView_GetItemCount(hListhWnd), 0);
    ITEMDATA DataHandler;

    for (int i = 0; MoveYourApp::GetApp()->GetMainWnd()->GetListItemData(&DataHandler, i); i++)
    {
      _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, s_szPrefix, _tcslen(s_szPrefix));
      nLen += _tcslen(s_szPrefix);
      _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, DataHandler.szSource, _tcslen(DataHandler.szSource));
      nLen += _tcslen(DataHandler.szSource);
      _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, _T("\0"), 1);
      nLen += 1;
      if (DataHandler.szDestination[0])
      {
        _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, s_szPrefix, _tcslen(s_szPrefix));
        nLen += _tcslen(s_szPrefix);
        _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, DataHandler.szDestination, _tcslen(DataHandler.szDestination));
        nLen += _tcslen(DataHandler.szDestination);
      }
      _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, _T("\0"), 1);
      nLen += 1;
    }

    if (nLen)
    {
      _tcsncpy_s(&vecData[nLen], vecData.size() - nLen, _T("\0"), 1);
      nLen += 1;
      if (!SetRegMString(s_szSysKey, s_szSysVal, vecData.data(), nLen, HKEY_LOCAL_MACHINE))
      {
        ShowOSError();
        return false;
      }
    }
    else
    {
      DeleteRegKey(s_szSysKey, s_szSysVal, HKEY_LOCAL_MACHINE);
    }
  }
  else
  {
    return SetEQU(hListhWnd);
  }

  vecData.resize(nLen);
  m_vecRegBuf = std::move(vecData);

  return true;
}

bool DataHandler::UpdateRecord(HWND hListhWnd)
{
  WaitCursor wc;

  MoveYourApp::GetApp()->GetMainWnd()->m_pRegMonitor->Suspend();
  bool bRet = DoUpdateRecord(hListhWnd);
  if (!bRet)
    SetEQU(hListhWnd);
  MoveYourApp::GetApp()->GetMainWnd()->m_pRegMonitor->Resume();

  return bRet;
}

bool DataHandler::FillList(HWND hListhWnd)
{
  WaitCursor wc;
  ListView_DeleteAllItems(hListhWnd);
  return DoGetFileRecord(hListhWnd);
}

bool DataHandler::OnSelectFile(HWND hWnd, HINSTANCE hInstance, bool bOpen, LPTSTR lpszName)
{
  if (!lpszName)
    return false;

  OPENFILENAME ofn;
  tstring strFilter = LoadString(IDS_SCRIPT_FILTER);

  LPTSTR lpszTemp = &strFilter.front() - 1; // points before first
  while (true)
  {
    lpszTemp = _tcschr(&lpszTemp[1], _T('#'));
    if (lpszTemp)
      lpszTemp[0] = 0;
    else
      break;
  }

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd;
  ofn.hInstance = hInstance;
  ofn.lpstrFilter = strFilter.c_str();
  ofn.lpstrCustomFilter = nullptr;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 1;;
  ofn.lpstrFile = lpszName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFileTitle = nullptr;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = nullptr;
  ofn.lpstrTitle = nullptr;
  ofn.Flags = OFN_PATHMUSTEXIST | (bOpen ? OFN_FILEMUSTEXIST : 0) | OFN_HIDEREADONLY | OFN_EXPLORER;
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = _T("myf");
  ofn.lCustData = 0;
  ofn.lpfnHook = nullptr;
  ofn.lpTemplateName = nullptr;

  if ((bOpen ? GetOpenFileName(&ofn) : GetSaveFileName(&ofn)))
    return true;

  return false;
}

bool DataHandler::OnExportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance)
{
  WaitCursor wc;

  TCHAR szFileName[MAX_PATH] = _T("Script.myf");
  if (!OnSelectFile(hWnd, hInstance, false, szFileName))
    return false;

  size_t nCode = MoveYourApp::GetApp()->GetOptions()->bAI ? 1 : 2;
  if (ListView_GetSelectedCount(hListhWnd) && !MoveYourApp::GetApp()->GetOptions()->bDNSDWEF)
  {
    ExportImportDlg dlg;
    nCode = dlg.DoExportModal(hWnd);
    if (!nCode || nCode == -1)
      return false;
  }

  HANDLE hRawFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (INVALID_HANDLE_VALUE == hRawFile)
  {
    ShowOSError(IDS_FILE_CREATE_FAILED, GetLastError());
    return false;
  }

  unique_handle hFile = make_unique_handle(hRawFile);
  DWORD dwCheck = 0;

  if (!WriteFile(hFile.get(), s_szSignature, _tcslen(s_szSignature) * sizeof(TCHAR), &dwCheck, nullptr))
  {
    ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
    return false;
  }
  if (!WriteFile(hFile.get(), s_szEO2L, _tcslen(s_szEO2L) * sizeof(TCHAR), &dwCheck, nullptr))
  {
    ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
    return false;
  }

  ITEMDATA DataHandler;
  int nIndex = -1;

  for (int i = 0; MoveYourApp::GetApp()->GetMainWnd()->GetListItemData(&DataHandler, nCode == 2, (nCode == 2) ? nIndex : i); i++)
  {
    if (!WriteFile(hFile.get(), DataHandler.szSource, _tcslen(DataHandler.szSource) * sizeof(TCHAR), &dwCheck, nullptr))
    {
      ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
      return false;
    }

    if (!WriteFile(hFile.get(), s_szPointer, _tcslen(s_szPointer) * sizeof(TCHAR), &dwCheck, nullptr))
    {
      ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
      return false;
    }

    if (DataHandler.szDestination[0])
    {
      if (!WriteFile(hFile.get(), DataHandler.szDestination, _tcslen(DataHandler.szDestination) * sizeof(TCHAR), &dwCheck, nullptr))
      {
        ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
        return false;
      }
    }
    else
    {
      if (!WriteFile(hFile.get(), s_szNULL, _tcslen(s_szNULL) * sizeof(TCHAR), &dwCheck, nullptr))
      {
        ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
        return false;
      }
    }

    if (!WriteFile(hFile.get(), s_szEOL, _tcslen(s_szEOL) * sizeof(TCHAR), &dwCheck, nullptr))
    {
      ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
      return false;
    }
  }

  return true;
}

bool DataHandler::OnInportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance, LPCTSTR lpszFileName /*= nullptr*/)
{
  WaitCursor wc;

  TCHAR szFileName[MAX_PATH] = _T("");

  if (!lpszFileName)
  {
    if (!OnSelectFile(hWnd, hInstance, true, szFileName))
      return false;
  }
  else
  {
    _tcscpy_s(szFileName, lpszFileName);
  }

  size_t nCode = MoveYourApp::GetApp()->GetOptions()->bMI ? 1 : 2;
  if (ListView_GetItemCount(hListhWnd) && !MoveYourApp::GetApp()->GetOptions()->bDNSDWIF)
  {
    ExportImportDlg dlg;
    nCode = dlg.DoImportModal(hWnd);
    if (!nCode || nCode == -1)
      return false;
  }

  if (nCode == 2)
    ListView_DeleteAllItems(hListhWnd);

  HANDLE hRawFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (INVALID_HANDLE_VALUE == hRawFile)
  {
    ShowOSError(IDS_FILE_OPEN_FAILED, GetLastError());
    return false;
  }

  unique_handle hFile = make_unique_handle(hRawFile);
  DWORD dwCheck = 0;
  DWORD dwFileLen = GetFileSize(hFile.get(), &dwCheck);

  if ((dwFileLen > 0xFFFFF) || dwCheck)
  {
    ShowOSError(IDS_SO_LARGE_FILE, GetLastError());
    return false;
  }

  std::vector<TCHAR> vecBuf(dwFileLen / sizeof(TCHAR), 0);

  if (!ReadFile(hFile.get(), &vecBuf[0], dwFileLen, &dwCheck, nullptr) && (dwFileLen != dwCheck))
  {
    ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
    return false;
  }

  hFile.reset();
  LPTSTR sz = vecBuf.data();

  while (*sz && _istspace(*sz))
  {
    sz = _tcsinc(sz);
  }

  if ((dwFileLen < _tcslen(s_szSignature) + 2) || (_tcsncmp(s_szSignature, sz, _tcslen(s_szSignature)) && !_istspace(*_tcsinc(sz))))
  {
    ShowOSError(IDS_NO_SCRIPT_FILE);
    return false;
  }

  sz += _tcslen(s_szSignature) + _tcslen(s_szEOL);

  while (*sz)
  {
    while (*sz && _istspace(*sz))
      sz = _tcsinc(sz);

    if (!*sz)
      break;

    std::unique_ptr<ITEMDATA> lpItemData(new ITEMDATA);

    int i;
    for (i = 0; *sz && (*sz != 13) && (*sz != 10) && _tcsncmp(sz, s_szPointer, _tcslen(s_szPointer)) && (i < MAX_PATH); i++, sz++)
      lpItemData->szSource[i] = *sz;

    if (!*sz || ((*sz == 13) || (*sz == 10)))
      continue;

    lpItemData->szSource[i] = 0;
    TrimPath(lpItemData->szSource);
    sz = sz + _tcslen(s_szPointer);

    for (i = 0; *sz && _tcsncmp(sz, s_szEOL, _tcslen(s_szEOL)) && (i < MAX_PATH); i++, sz++)
      lpItemData->szDestination[i] = *sz;

    if (!*sz)
      continue;

    lpItemData->szDestination[i] = 0;
    sz = sz + _tcslen(s_szEOL);
    TrimPath(lpItemData->szDestination);

    if (!_tcsicmp(lpItemData->szDestination, s_szNULL))
      lpItemData->szDestination[0] = 0;

    MoveYourApp::GetApp()->GetMainWnd()->AddToList(lpItemData.release());
  }

  return true;
}

bool DataHandler::OnPaste(HWND hWnd, HWND hListView)
{
  bool bRet = false;
  if (OpenClipboard(hWnd))
  {
    HGLOBAL hClipData = (HGLOBAL)GetClipboardData(CF_TEXT);
    if (hClipData)
    {
      int nInsertBefore = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
      LPTSTR lpszClipData = (LPTSTR)GlobalLock(hClipData);
      LPTSTR sz = lpszClipData;

      while (*sz)
      {
        while (*sz && _istspace(*sz))
          sz = _tcsinc(sz);

        if (!*sz)
          break;

        std::unique_ptr<ITEMDATA> lpItemData(new ITEMDATA);

        int i;
        for (i = 0; *sz && (*sz != 13) && (*sz != 10) && _tcsncmp(sz, s_szPointer, _tcslen(s_szPointer)) && (i < MAX_PATH); i++, sz++)
          lpItemData->szSource[i] = *sz;

        if (!*sz || (*sz == 13) || (*sz == 10))
          continue;

        lpItemData->szSource[i] = 0;
        TrimPath(lpItemData->szSource);
        sz = sz + _tcslen(s_szPointer);

        for (i = 0; *sz && _tcsncmp(sz, (LPTSTR)s_szEOL, _tcslen(s_szEOL)) && (i < MAX_PATH); i++, sz++)
          lpItemData->szDestination[i] = *sz;

        if (!*sz)
          continue;

        lpItemData->szDestination[i] = 0;
        sz = sz + _tcslen(s_szEOL);
        TrimPath(lpItemData->szDestination);

        if (!_tcsicmp(lpItemData->szDestination, s_szNULL))
          lpItemData->szDestination[0] = 0;

        if (nInsertBefore != -1)
          MoveYourApp::GetApp()->GetMainWnd()->InsertItem(lpItemData.release(), nInsertBefore);
        else
          MoveYourApp::GetApp()->GetMainWnd()->AddToList(lpItemData.release());
      }

      GlobalUnlock(hClipData);
      bRet = true;
    }

    CloseClipboard();
  }
  return bRet;
}

bool DataHandler::OnCopyCut(HWND hWnd, HWND hListView, bool bCut)
{
  bool bRet = false;
  if (OpenClipboard(hWnd) && EmptyClipboard())
  {
    ITEMDATA DataHandler;
    int nIndex = -1;
    HGLOBAL hClipData = nullptr;
    LPTSTR lpszClipData = nullptr;

    while (MoveYourApp::GetApp()->GetMainWnd()->GetListItemData(&DataHandler, true, nIndex))
    {
      if (hClipData)
        hClipData = GlobalReAlloc(hClipData, 2 * (MAX_PATH + 1), GHND);
      else
        hClipData = GlobalAlloc(GHND, 2 * (MAX_PATH + 1));

      _ASSERTE(hClipData);
      if (!hClipData)
        break;

      lpszClipData = (LPTSTR)GlobalLock(hClipData);

      _tcscat_s(lpszClipData, MAX_PATH, DataHandler.szSource);
      if (_tcslen(lpszClipData) < MAX_PATH)
        _tcscat_s(lpszClipData, MAX_PATH - _tcslen(lpszClipData), s_szPointer);
      else
        return false;

      if (DataHandler.szDestination[0])
      {
        if (_tcslen(lpszClipData) < MAX_PATH)
          _tcscat_s(lpszClipData, MAX_PATH - _tcslen(lpszClipData), DataHandler.szDestination);
        else
          return false;
      }
      else
      {
        if (_tcslen(lpszClipData) < MAX_PATH)
          _tcscat_s(lpszClipData, MAX_PATH - _tcslen(lpszClipData), s_szNULL);
        else
          return false;
      }

      if (_tcslen(lpszClipData) < MAX_PATH)
        _tcsncat_s(lpszClipData, MAX_PATH - _tcslen(lpszClipData), s_szEOL, _tcslen(s_szEOL));
      else
        return false;

      GlobalUnlock(hClipData);

      if (bCut)
      {
        ListView_DeleteItem(hListView, nIndex);
        nIndex--;
      }
    }

    HANDLE hClip = SetClipboardData(CF_TEXT, hClipData);
    if (!hClip)
      GlobalFree(hClipData);
    CloseClipboard();
    bRet = bCut;
  }
  return bRet;
}

bool DataHandler::SetEQU(HWND hListhWnd)
{
  ReportBox(IDS_UPDATE_TO_EQU, MB_OK | MB_ICONWARNING);
  ListView_DeleteAllItems(hListhWnd);
  return FillList(hListhWnd);
}
