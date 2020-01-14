/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Window base class
*/

#pragma once

class WindowBase
{
public:
  WindowBase();
  ~WindowBase();

public:
  bool Create(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName = _T(""), DWORD dwStyle = WS_OVERLAPPEDWINDOW,
    int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = nullptr, HMENU hMenu = nullptr);

  LPCTSTR GetClassName() const
  {
    return m_strClassName.c_str();
  }
  HWND GetHWND() const
  {
    return m_hWnd;
  }
  operator HWND() const
  {
    return GetHWND();
  }

protected:
  virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static LRESULT WINAPI s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
  HWND m_hWnd = nullptr;

private:
  tstring m_strClassName;
};

