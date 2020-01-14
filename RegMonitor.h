/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Registry monitor
*/

#pragma once

#include <memory>

namespace std {
  class thread;
}

class RegMonitor
{
public:
  RegMonitor();
  ~RegMonitor();

public:
  bool Install(HWND hWnd);
  bool Uninstall();

  void Suspend();
  void Resume();

private:
  void Process();

private:
  HWND m_hWnd = nullptr;
  std::unique_ptr<std::thread> m_pMonitorThread;
  unique_handle m_hExitEvent = make_unique_handle();
  unique_handle m_hSuspendEvent = make_unique_handle();
  unique_handle m_hResumeEvent = make_unique_handle();
};

