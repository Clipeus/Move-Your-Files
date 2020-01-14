/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Application class
*/

#pragma once

#include <memory>
#include "MoveYour.h"

class MoveYourWnd;
struct AppOptions;

class MoveYourApp
{
  MoveYourApp();

public:
  ~MoveYourApp();
  static MoveYourApp* GetApp();

public:
  bool Init(HINSTANCE hinstExe, LPTSTR pszCmdLine);
  int Run(int nCmdShow);

public:
  HINSTANCE GetInstance() const
  {
    return m_hInstance;
  }
  MoveYourWnd* GetMainWnd() const
  {
    return m_pMainWnd.get();
  }
  AppOptions* GetOptions() const
  {
    return m_pAppOptions.get();
  }
  LPCTSTR GetAppName() const
  {
    return m_strAppName.c_str();
  }
  bool IsWaitCursor() const
  {
    return m_bWaitCursor;
  }
  void SetWaitCursor(bool bWait)
  {
    m_bWaitCursor = bWait;
  }
  HCURSOR GetWaitCursor() const
  {
    return m_hWaitCursor;
  }
  HCURSOR GetStdCursor() const
  {
    return m_hStdCursor;
  }
  const VS_FIXEDFILEINFO& GetFixedInfo() const
  {
    return m_FixedInfo;
  }

public:
  static LPCTSTR s_szMainKey;

private:
  static MoveYourApp* s_pMoveYourApp;

  tstring m_strAppName;
  HINSTANCE m_hInstance = nullptr;
  std::unique_ptr<MoveYourWnd> m_pMainWnd;
  std::unique_ptr<AppOptions> m_pAppOptions;
  bool m_bWaitCursor = false;
  HCURSOR m_hWaitCursor = nullptr;
  HCURSOR m_hStdCursor = nullptr;
  VS_FIXEDFILEINFO m_FixedInfo = { 0 };
};

