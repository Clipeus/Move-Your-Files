/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Application option
*/

#define STRICT
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "AppOptions.h"
#include "Utils.h"

void AppOptions::ReadOptions()
{
  bURAS = GetRegDWORD(MoveYourApp::s_szMainKey, _T("URAS"), false, HKEY_CURRENT_USER);
  bDNSDWIF = GetRegDWORD(MoveYourApp::s_szMainKey, _T("DNSDWIF"), false, HKEY_CURRENT_USER);
  bMI = GetRegDWORD(MoveYourApp::s_szMainKey, _T("MI"), true, HKEY_CURRENT_USER);
  bDNSDWEF = GetRegDWORD(MoveYourApp::s_szMainKey, _T("DNSDWEF"), false, HKEY_CURRENT_USER);
  bAI = GetRegDWORD(MoveYourApp::s_szMainKey, _T("AI"), true, HKEY_CURRENT_USER);
}

void AppOptions::WriteOptions() const
{
  SetRegDWORD(MoveYourApp::s_szMainKey, _T("URAS"), bURAS, HKEY_CURRENT_USER);
  SetRegDWORD(MoveYourApp::s_szMainKey, _T("DNSDWIF"), bDNSDWIF, HKEY_CURRENT_USER);
  SetRegDWORD(MoveYourApp::s_szMainKey, _T("MI"), bMI, HKEY_CURRENT_USER);
  SetRegDWORD(MoveYourApp::s_szMainKey, _T("DNSDWEF"), bDNSDWEF, HKEY_CURRENT_USER);
  SetRegDWORD(MoveYourApp::s_szMainKey, _T("AI"), bAI, HKEY_CURRENT_USER);
}

