/*
		Author: Mikhail Lyubushkin

		Project:		Move Your Files 2.0
		Description:	Utils, helper
*/

#pragma once
#include <crtdbg.h>
#include <memory>

// The normal HANDLE_MSG macro in WINDOWSX.H does not work properly for dialog
// boxes because DlgProc's return a bool instead of an LRESULT (like
// WndProcs). This HANDLE_DLGMSG macro corrects the problem:
#define HANDLE_DLGMSG(hWnd, message, fn)                          \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,               \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

#define UNIQUE_VALUE(NAME, TYPE, DELETER) \
using unique_##NAME = std::unique_ptr<std::remove_pointer<TYPE>::type, decltype(&DELETER)>; \
inline unique_##NAME make_unique_##NAME(TYPE value = nullptr) \
{ \
    return unique_##NAME(value, DELETER); \
}

#define UNIQUE_PTR(NAME, PTR, DELETER) \
using unique_##NAME = std::unique_ptr<PTR, decltype(&DELETER)>; \
inline unique_##NAME make_unique_##NAME(PTR* ptr = nullptr) \
{ \
    return unique_##NAME(ptr, DELETER); \
}

inline void CloseHandleAndSaveLastError(HANDLE hObject)
{
	DWORD le = ::GetLastError(); 
	::CloseHandle(hObject);	
	::SetLastError(le);
}

UNIQUE_VALUE(handle, HANDLE, CloseHandleAndSaveLastError)

inline void RegCloseKeyAndSaveLastError(HKEY hKey)
{
	DWORD le = ::GetLastError();
	::RegCloseKey(hKey);
	::SetLastError(le);
}

UNIQUE_VALUE(regkey, HKEY, RegCloseKeyAndSaveLastError)

void SetGlobalHandlers();

DWORD GetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwDefault = 0, HKEY hRoot = HKEY_CURRENT_USER);
bool GetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszDefault, LPTSTR lpszReturnedString, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);
bool GetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);
bool GetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpDefault, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);

bool SetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwData, HKEY hRoot = HKEY_CURRENT_USER);
bool SetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszString, HKEY hRoot = HKEY_CURRENT_USER);
bool SetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);
bool SetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszBuf, DWORD dwSize = 0, HKEY hRoot = HKEY_CURRENT_USER);

bool DeleteRegKey(LPCTSTR lpszFolder, LPCTSTR lpszKey, HKEY hRoot = HKEY_CURRENT_USER);

bool GetFixedInfo(VS_FIXEDFILEINFO & rFixedInfo);
tstring LoadString(UINT uID);
tstring StrFormat(LPCTSTR lpszFormat, ...);

int ShowOSError(UINT uMesID = IDS_SYSTEM_ERROR, DWORD dwError = 0, int nMode = MB_OK | MB_ICONERROR);
int ReportBox(LPCTSTR lpszMessage, int nMode = MB_OK);
int ReportBox(UINT uMesID, int nMode = MB_OK);

inline void ScreenToClient(HWND hParentWnd, LPRECT lpRect)
{
	ScreenToClient(hParentWnd, (LPPOINT)lpRect);
	ScreenToClient(hParentWnd, ((LPPOINT)lpRect) + 1);
}

class WaitCursor
{
public:
	WaitCursor();
	~WaitCursor();

protected:
	HCURSOR m_hPrevCursor = nullptr;
};
