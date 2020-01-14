/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Window base class
*/

#define STRICT
#include <Windows.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "Utils.h"
#include "WindowBase.h"

WindowBase::WindowBase()
{

}

WindowBase::~WindowBase()
{
}

bool WindowBase::Create(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
  m_strClassName = lpClassName;
  return CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, MoveYourApp::GetApp()->GetInstance(), reinterpret_cast<LPVOID>(this)) != nullptr;
}

LRESULT WindowBase::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI WindowBase::s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  WindowBase* pWnd = nullptr;
  if (WM_NCCREATE == uMsg)
  {
    LPCREATESTRUCTA lpCS = reinterpret_cast<LPCREATESTRUCTA>(lParam);
    pWnd = reinterpret_cast<WindowBase*>(lpCS->lpCreateParams);
    _ASSERTE(pWnd);
    if (pWnd)
      pWnd->m_hWnd = hWnd;
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
  }
  else
  {
    pWnd = reinterpret_cast<WindowBase*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
  }

  if (pWnd)
    return pWnd->WndProc(hWnd, uMsg, wParam, lParam);

  return -1;
}
