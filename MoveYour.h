/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Global application defs
*/

#pragma once

#include <string>
#include "afxres.h"
#include "Resource.h"

#if defined UNICODE || defined _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //UNICODE

typedef struct tagITEMDATA
{
  TCHAR szSource[MAX_PATH] = { 0 };
  TCHAR szDestination[MAX_PATH] = { 0 };
} ITEMDATA, * LPITEMDATA;

typedef struct tagITEMDATAEX : tagITEMDATA
{
  UINT uAction = 0;
} ITEMDATAEX, * LPITEMDATAEX;
