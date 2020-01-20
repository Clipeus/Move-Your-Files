/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Main application window
*/

#pragma once

#include <memory>
#include "WindowBase.h"

class DataHandler;
class FindDlg;
class RegMonitor;

class MoveYourWnd : public WindowBase
{
public:
  MoveYourWnd();
  ~MoveYourWnd();

public:
  bool Init();
  bool Create(int nCmdShow);

private:
  void EnableCommands(long nCount, long nSelCount, HMENU hContextMenu = nullptr);
  void SetStdStatusText(long nCount = -1, long nSelCount = -1);
  void SetStdStatusAndMenu();
  void ChangeMenuItems(HMENU hContextMenu = nullptr);
  void ReadWindowPos(LPCTSTR lpszFolder, LPCTSTR lpszKey);
  void WriteWindowPos(LPCTSTR lpszFolder, LPCTSTR lpszKey) const;
  void AdjustRect(int x, int y, int cx, int cy, bool bMove = true);
  void InitList();
  void InsertItem(LPITEMDATA lpFileData, long nIndex);
  void AddToList(LPITEMDATA lpFileData);
  bool SetListItemData(LPITEMDATA lpFileData, long nIndex = -1);
  bool GetListItemData(LPITEMDATA lpFileData, long nIndex = -1);
  bool GetListItemData(LPITEMDATA lpFileData, bool bSelection, int& nIndex);
  void DrawColumnImage(HWND hHeader, int nColumn, short nOrder, bool bBitmap);
  void SortItems(short nColumn, bool bBitmap);
  int GetStartItem();
  bool OnEndFind(int nIndex);
  bool OnFind(bool bNext = false);

  static int CALLBACK CompareListItem(LPITEMDATA lpFileData1, LPITEMDATA lpFileData2, long nSortData);

private:
  LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnDestroy(HWND hWnd);
  bool OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
  void OnSize(HWND hWnd, UINT state, int cx, int cy);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
  void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags);
  void OnContextMenu(HWND hWnd, HWND hwndContext, UINT x, UINT y);
  LRESULT OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr);
  bool OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
  void OnActivate(HWND hWnd, UINT nState, HWND hWndActDeact, bool fMinimized);
  void OnMeasureItem(HWND hWnd, MEASUREITEMSTRUCT* lpMeasureItem);
  void OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem);
  void OnInitMenuPopup(HWND hWnd, HMENU hMenu, UINT item, bool fSystemMenu);
  void OnNCDestroy(HWND hWnd);
  void OnChangeCBChain(HWND hWnd, HWND hWndRemove, HWND hWndNext);
  void OnDrawClipboard(HWND hWnd);
  void OnDropFiles(HWND hWnd, HDROP hDrop);

public:
  static LPCTSTR s_szWindowClass;

private:
  HWND m_hToolBar = nullptr;
  HWND m_hListView = nullptr;
  HWND m_hStatusBar = nullptr;
  HWND m_hClipNextView = nullptr;
  HBITMAP m_hBitmap = nullptr;
  int m_ptWidth[2] = { 0, 0 };
  std::unique_ptr<DataHandler> m_pDataHandler;
  std::unique_ptr<FindDlg> m_pFindDlg;
  std::unique_ptr<RegMonitor> m_pRegMonitor;

  friend class DataHandler;
};

