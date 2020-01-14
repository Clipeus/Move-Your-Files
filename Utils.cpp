/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Utils, helper
*/

#define STRICT
#include <Windows.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "Utils.h"
#include <new.h>
#include <eh.h>

DWORD GetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwDefault /*= 0*/, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hRawKey;

  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);
    DWORD dwTemp2, dwTemp = sizeof(DWORD);
    DWORD dwRealType;

    if (RegQueryValueEx(hKey.get(), lpszKey, 0, &dwRealType, (LPBYTE)&dwTemp2, &dwTemp) == ERROR_SUCCESS)
    {
      if (REG_DWORD == dwRealType)
      {
        dwDefault = dwTemp2;
      }
    }
  }
  return dwDefault;
}

bool GetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszDefault, LPTSTR lpszReturnedString, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hRawKey;

  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);
    DWORD dwRealType;

    if (RegQueryValueEx(hKey.get(), lpszKey, 0, &dwRealType, (LPBYTE)lpszReturnedString, &dwSize) != ERROR_SUCCESS)
    {
      _tcscpy_s(lpszReturnedString, dwSize / sizeof(TCHAR), lpszDefault);
      lpszReturnedString[dwSize - 1] = 0;
      return false;
    }

    if (REG_SZ == dwRealType)
      return true;
  }
  return false;
}

bool GetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  bool bRet = false;
  HKEY hRawKey;

  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);
    DWORD dwRealType;

    if (RegQueryValueEx(hKey.get(), lpszKey, 0, &dwRealType, (LPBYTE)lpBuf, &dwSize) == ERROR_SUCCESS)
    {
      if (REG_BINARY == dwRealType)
      {
        bRet = true;
      }
    }
  }
  return bRet;
}

bool GetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpDefault, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hRawKey;

  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);
    DWORD dwRealType;

    if (RegQueryValueEx(hKey.get(), lpszKey, 0, &dwRealType, (LPBYTE)lpBuf, &dwSize) != ERROR_SUCCESS)
    {
      memmove(lpBuf, lpDefault, dwSize);
      return false;
    }

    if (REG_MULTI_SZ == dwRealType)
      return true;
  }
  return false;
}

bool SetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwData, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  bool bRet = false;
  HKEY hRawKey;
  DWORD dwDisp;

  if (RegCreateKeyEx(hRoot, lpszFolder, 0, nullptr,
    REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hRawKey, &dwDisp) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);

    if (RegSetValueEx(hKey.get(), lpszKey, 0, REG_DWORD, (CONST BYTE*) & dwData, sizeof(DWORD)) == ERROR_SUCCESS)
    {
      bRet = true;
    }
  }
  return bRet;
}

bool SetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszString, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  bool bRet = false;
  HKEY hRawKey;
  DWORD dwDisp;

  if (RegCreateKeyEx(hRoot, lpszFolder, 0, nullptr,
    REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hRawKey, &dwDisp) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);

    if (RegSetValueEx(hKey.get(), lpszKey, 0, REG_SZ, (CONST BYTE*)lpszString, _tcslen(lpszString) * sizeof(TCHAR) + sizeof(TCHAR)) == ERROR_SUCCESS)
    {
      bRet = true;
    }
  }
  return bRet;
}

bool SetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  bool bRet = false;
  HKEY hRawKey;
  DWORD dwDisp;

  if (RegCreateKeyEx(hRoot, lpszFolder, 0, nullptr,
    REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hRawKey, &dwDisp) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);

    if (RegSetValueEx(hKey.get(), lpszKey, 0, REG_BINARY, (CONST BYTE*)lpBuf, dwSize) == ERROR_SUCCESS)
    {
      bRet = true;
    }
  }
  return bRet;
}

bool DeleteRegKey(LPCTSTR lpszFolder, LPCTSTR lpszKey, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hRawKey;

  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_WRITE, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);

    if (RegDeleteValue(hKey.get(), lpszKey) == ERROR_SUCCESS)
    {
      return true;
    }
  }
  return false;
}

bool SetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszBuf, DWORD dwSize /*= 0*/, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hRawKey;

  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_WRITE, &hRawKey) == ERROR_SUCCESS)
  {
    unique_regkey hKey = make_unique_regkey(hRawKey);

    if (dwSize == 0)
      dwSize = _tcslen(lpszBuf) + sizeof(TCHAR);

    if (RegSetValueEx(hKey.get(), lpszKey, 0, REG_MULTI_SZ, (LPBYTE)lpszBuf, dwSize * sizeof(TCHAR)) == ERROR_SUCCESS)
    {
      return true;
    }
  }
  return false;
}

int ReportBox(LPCTSTR lpszMessage, int nMode /*= MB_OK*/)
{
  return MessageBox(MoveYourApp::GetApp()->GetMainWnd()->GetHWND(), lpszMessage, MoveYourApp::GetApp()->GetAppName(), nMode);
}

int ReportBox(UINT uMesID, int nMode /*= MB_OK*/)
{
  return ReportBox(LoadString(uMesID).c_str(), nMode);
}

int ShowOSError(UINT uMesID /*= IDS_SYSTEM_ERROR*/, DWORD dwError /*= 0*/, int nMode /*= MB_OK|MB_ICONERROR*/)
{
  LPTSTR szErrorText = nullptr;

  if (!dwError)
    dwError = ::GetLastError();
  
  if (dwError)
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&szErrorText), 0, nullptr);

  tstring strText = LoadString(uMesID);

  if (szErrorText)
  {
    strText += szErrorText;
    LocalFree(szErrorText);
  }

  return ReportBox(strText.c_str(), nMode);
}

tstring LoadString(UINT uID)
{
  tstring result;
#ifdef UNICODE
  LPCTSTR lpszText = nullptr;
  
  int nLen = ::LoadString(MoveYourApp::GetApp()->GetInstance(), uID, reinterpret_cast<LPTSTR>(&lpszText), 0);

  if (lpszText && nLen > 0)
    result.assign(lpszText, nLen);

  return result;
#else
  // LoadStringA has a bug when cchBufferMax is zero
  TCHAR szBuffer[256] = { 0 };

  int nLen = ::LoadString(MoveYourApp::GetApp()->GetInstance(), uID, szBuffer, sizeof(szBuffer) / sizeof(TCHAR));

  if (nLen > 0)
    result.assign(szBuffer, nLen);

#endif // !UNICODE
  return result;
}

tstring StrFormat(LPCTSTR lpszFormat, ...)
{
  va_list args;
  va_start(args, lpszFormat);

  size_t size = _vsctprintf(lpszFormat, args);

  tstring str;
  str.resize(size);

  _vstprintf_s(&str.front(), size + 1, lpszFormat, args);

  va_end(args);

  return str;
}

bool GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
  // get the filename of the executable containing the version resource
  TCHAR szFilename[MAX_PATH + 1] = { 0 };
  if (GetModuleFileName(nullptr, szFilename, MAX_PATH) == 0)
  {
    _RPTFN(_CRT_ERROR, "GetModuleFileName failed with error %d\n", GetLastError());
    return false;
  }

  // allocate a block of memory for the version info
  DWORD dummy;
  DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
  if (dwSize == 0)
  {
    _RPTFN(_CRT_ERROR, "GetFileVersionInfoSize failed with error %d\n", GetLastError());
    return false;
  }
  std::vector<BYTE> data;
  data.resize(dwSize);

  // load the version info
  if (!GetFileVersionInfo(szFilename, 0, dwSize, data.data()))
  {
    _RPTFN(_CRT_ERROR, "GetFileVersionInfo failed with error %d\n", GetLastError());
    return false;
  }

  VS_FIXEDFILEINFO* pFixedInfo;
  UINT uiVerLen = 0;
  if (VerQueryValue(data.data(), TEXT("\\"), (void**)&pFixedInfo, (UINT*)&uiVerLen) == 0)
  {
    _RPTFN(_CRT_ERROR, "VerQueryValue failed with error %d\n", GetLastError());
    return false;
  }

  rFixedInfo = *pFixedInfo;
  return true;
}

WaitCursor::WaitCursor()
{
  m_hPrevCursor = SetCursor(MoveYourApp::GetApp()->GetWaitCursor());
  MoveYourApp::GetApp()->SetWaitCursor(true);
}

WaitCursor::~WaitCursor()
{
  SetCursor(m_hPrevCursor ? m_hPrevCursor : MoveYourApp::GetApp()->GetStdCursor());
  MoveYourApp::GetApp()->SetWaitCursor(false);
}

LONG WINAPI UnExFilter(EXCEPTION_POINTERS* pExceptionInfo)
{
  FatalAppExit(0, LoadString(IDS_UNEXCEPTION).c_str());
  return EXCEPTION_CONTINUE_SEARCH;
}

void _thread_se_translator(unsigned int nCode, EXCEPTION_POINTERS* pException)
{
  _RPTF1(_CRT_ERROR, "SEH -> Exception code : %X\n", pException->ExceptionRecord->ExceptionCode);
  throw(pException);
}

int _handle_out_of_memory(size_t nSize)
{
  _RPTF1(_CRT_ERROR, "NEW -> Out of memory for %d bytes\n", nSize);
  if (IDCANCEL == ReportBox(IDS_OUT_OF_MEM, MB_RETRYCANCEL | MB_TASKMODAL | MB_ICONSTOP))
    return 0;
  return 1;
}

void SetGlobalHandlers()
{
  SetUnhandledExceptionFilter(UnExFilter);	// For Win32 SEH
  _set_se_translator(_thread_se_translator);	// For C++ try/catch
  _set_new_handler(_handle_out_of_memory);
  _set_new_mode(1);
}
