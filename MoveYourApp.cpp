/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Application class
*/

#define STRICT
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "BrowserCtl.h"
#include "Utils.h"
#include "AppOptions.h"

LPCTSTR MoveYourApp::s_szMainKey = _T("SOFTWARE\\LMSoft\\Move Your Files");
MoveYourApp* MoveYourApp::s_pMoveYourApp = nullptr;

MoveYourApp::MoveYourApp()
{

}

MoveYourApp::~MoveYourApp()
{
  if (m_pAppOptions)
    m_pAppOptions->WriteOptions();
}

MoveYourApp* MoveYourApp::GetApp()
{
  if (s_pMoveYourApp == nullptr)
    s_pMoveYourApp = new MoveYourApp;

  return s_pMoveYourApp;
}

bool MoveYourApp::Init(HINSTANCE hinstExe, LPTSTR pszCmdLine)
{
  SetGlobalHandlers();

  HWND hWnd = FindWindow(MoveYourWnd::s_szWindowClass, nullptr);
  if (hWnd)
  {
    if (IsIconic(hWnd))
      ShowWindow(hWnd, SW_RESTORE);
    SetForegroundWindow(hWnd);
    return false;
  }

  m_hInstance = hinstExe;

  INITCOMMONCONTROLSEX iccs = { 0 };
  iccs.dwSize = sizeof(iccs);
  iccs.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&iccs);
  InitBrowserCtl();

  m_pAppOptions = std::make_unique<AppOptions>();
  m_pAppOptions->ReadOptions();

  m_strAppName = LoadString(IDS_TITLE);

  ::GetFixedInfo(m_FixedInfo);

  m_hWaitCursor = LoadCursor(nullptr, IDC_WAIT);
  m_hStdCursor = LoadCursor(nullptr, IDC_ARROW);

  m_pMainWnd = std::make_unique<MoveYourWnd>();
  return m_pMainWnd->Init();
}

int MoveYourApp::Run(int nCmdShow)
{
  if (!m_pMainWnd->Create(nCmdShow))
    return 1;

  HACCEL hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
  if (!hAccel)
  {
    ShowOSError();
    return 1;
  }

  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(m_pMainWnd->GetHWND(), hAccel, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}
