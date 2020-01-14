/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Browser Control
*/

#pragma once

/* File Browser Styles */
#define FBS_NOTABSTOPBUTTON			0x0000
#define FBS_TABSTOPBUTTON			0x0001
#define FBS_RIGHTBUTTON				0x0002

/* File Browser Message */
#define	FBM_GETFILENAME				WM_USER + 0x0001
#define	FBM_SETFILENAME				WM_USER + 0x0002
#define	FBM_BROWSERFILES			WM_USER + 0x0003
#define	FBM_GETEDITCTL				WM_USER + 0x0004
#define	FBM_GETBUTTON				WM_USER + 0x0005
#define	FBM_SETHOTKEY				WM_USER + 0x0006
#define	FBM_GETHOTKEY				WM_USER + 0x0007

/*
#define FBM_SETBUTTONIMAGE			WM_USER + 0x0006
#define SBI_TEXT					0x0000
#define SBI_ICON					0x0001
#define SBI_BITMAP					0x0002
*/

/* File Browser Notification Codes */

/* Includes Edit Control Notification Codes */

#define FBN_BROWSING				WM_USER + 0x0001
#define FBN_BROWSED					WM_USER + 0x0002

bool InitBrowserCtl();
