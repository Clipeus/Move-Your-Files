/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Registry monitor
*/

#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <stdio.h>
#include <tchar.h>
#include <thread>
#include <array>
#include "MoveYour.h"
#include "Utils.h"
#include "DataHandler.h"
#include "RegMonitor.h"

RegMonitor::RegMonitor()
{
}

RegMonitor::~RegMonitor()
{
}

bool RegMonitor::Install(HWND hWnd)
{
  Uninstall();

  m_hExitEvent = make_unique_handle(::CreateEvent(nullptr, true, false, nullptr));
  if (!m_hExitEvent)
    return false;

  m_hSuspendEvent = make_unique_handle(::CreateEvent(nullptr, false, false, nullptr));
  if (!m_hSuspendEvent)
    return false;

  m_hResumeEvent = make_unique_handle(::CreateEvent(nullptr, false, false, nullptr));
  if (!m_hResumeEvent)
    return false;

  m_hWnd = hWnd;
  m_pMonitorThread = std::make_unique<std::thread>(&RegMonitor::Process, this);

  return true;
}

bool RegMonitor::Uninstall()
{
  if (m_hExitEvent)
  {
    ::SetEvent(m_hExitEvent.get());
    m_pMonitorThread->join();
    m_pMonitorThread.reset();

    m_hExitEvent.reset();
    m_hSuspendEvent.reset();
    m_hResumeEvent.reset();
  }

  return true;
}

void RegMonitor::Suspend()
{
  _ASSERTE(m_hSuspendEvent.get());

  if (m_hSuspendEvent)
    ::SetEvent(m_hSuspendEvent.get());
}

void RegMonitor::Resume()
{
  _ASSERTE(m_hResumeEvent.get());

  if (m_hResumeEvent)
    ::SetEvent(m_hResumeEvent.get());
}

void RegMonitor::Process()
{
  unique_handle m_hRegEvent = make_unique_handle(::CreateEvent(nullptr, false, false, nullptr));
  if (!m_hRegEvent)
    return;

  std::array<HANDLE, 3> hEvents;
  hEvents[0] = m_hExitEvent.get();
  hEvents[1] = m_hSuspendEvent.get();
  hEvents[2] = m_hRegEvent.get();

  while (true)
  {
    long nRetCode = ::WaitForSingleObject(m_hExitEvent.get(), 0);
    if (nRetCode == WAIT_OBJECT_0)
      break;

    HKEY hRawKey;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, DataHandler::s_szSysKey, 0, KEY_ALL_ACCESS, &hRawKey) != ERROR_SUCCESS)
    {
      _RPTF0(_CRT_WARN, "Cannot open reg key\n");
      ::Sleep(5000);
      continue;
    }

    unique_regkey hKey = make_unique_regkey(hRawKey);

    if (ERROR_SUCCESS != ::RegNotifyChangeKeyValue(hKey.get(), false, REG_NOTIFY_CHANGE_LAST_SET, m_hRegEvent.get(), true))
    {
      _RPTF0(_CRT_WARN, "Cannot set notify\n");
      ::Sleep(5000);
      continue;
    }

    nRetCode = ::WaitForMultipleObjects(hEvents.size(), hEvents.data(), false, INFINITE);
    if (nRetCode == WAIT_OBJECT_0)
    {
      break;
    }
    else if (nRetCode == WAIT_OBJECT_0 + 1)
    {
      _RPTF0(_CRT_WARN, "Suspend\n");
      ::WaitForSingleObject(m_hResumeEvent.get(), INFINITE);
      _RPTF0(_CRT_WARN, "Resume\n");
    }
    else if (nRetCode == WAIT_OBJECT_0 + 2)
    {
      _RPTF0(_CRT_WARN, "Notify refresh\n");
      ::SendMessage(m_hWnd, WM_COMMAND, MAKELONG(ID_VIEW_REFRESH, 0), 0);
    }
  }

  _RPTF0(_CRT_WARN, "Exit Monitor Process\n");
}

