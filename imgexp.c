/****************************************************************************
 *                                                                          *
 * File    : main.c                                                         *
 *                                                                          *
 * Purpose : Win32 (dialog) Skeleton                                        *
 *                                                                          *
 ****************************************************************************/

/* 
 * Either define WIN32_LEAN_AND_MEAN, or one or more of NOCRYPT,
 * NOSERVICE, NOMCX and NOIME, to decrease compile time (if you
 * don't need these defines -- see windows.h).
 */

#define WIN32_LEAN_AND_MEAN
/* #define NOCRYPT 
#define NOSERVICE
#define NOMCX
#define NOIME */
#define __STDC_WANT_LIB_EXT2__ 1

#define USE_OWN_SHA1

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdio.h>
#include "imgexp.h"

/* often used */
#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
#define BUF_SIZE 4096

/** Prototypes **************************************************************/

static INT_PTR CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK ImgExpProc(HWND, UINT, WPARAM, LPARAM);

/** Global variables ********************************************************/

static HANDLE ghInstance;


/****************************************************************************
 *                                                                          *
 * Function: WinMain                                                        *
 *                                                                          *
 * Purpose : Initialize the application.  Register a window class,          *
 *           create and display the main window and enter the               *
 *           message loop.                                                  *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icc;
    WNDCLASSEX wcx;
    WNDCLASSEX ccx;

    ghInstance = hInstance;

    /* Initialize common controls. Also needed for MANIFEST's */
    /*
     * TODO: set the ICC_???_CLASSES that you need.
     */
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    /* Get system dialog information */
    wcx.cbSize = sizeof(wcx);
    if (!GetClassInfoEx(NULL, MAKEINTRESOURCE(32770), &wcx))
        return 0;

    /* Add our own stuff */
    wcx.hInstance = hInstance;
    wcx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    wcx.lpszClassName = _T("hwkimgClass");
    if (!RegisterClassEx(&wcx))
        return 0;

	/* custom control */
	ccx.cbSize = sizeof(ccx);
	ccx.style = CS_HREDRAW | CS_VREDRAW;
	ccx.lpfnWndProc = (WNDPROC) ImgExpProc;
	ccx.cbClsExtra = 0;
	ccx.cbWndExtra = 0;
	ccx.hInstance = hInstance;
	ccx.hbrBackground = NULL;
	ccx.lpszMenuName = NULL;
	ccx.lpszClassName = _T("hwkimgexp");
    ccx.hIcon = NULL;
	ccx.hIconSm = NULL;
	ccx.hCursor = LoadCursor(NULL,IDC_CROSS);
    if (!RegisterClassEx(&ccx))
        return 1;
	OutputDebugString("5");
	
    /* The user interface is a modal dialog box */
    return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)MainDlgProc);
}

/**********************************
 * application specific functions *
 **********************************/
int do_something(HWND hDlg)
{
	OutputDebugString("something");
	SetDlgItemText(hDlg,ID_EDH,"Did something");
	return 0;
}

/****************************************************************************
 *                                                                          *
 * Function: MainDlgProc                                                    *
 *                                                                          *
 * Purpose : Process messages for the Main dialog.                          *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/


static INT_PTR CALLBACK MainDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
			/* code to initialize the dialog. */
            
            return TRUE;

        case WM_SIZE:
            /*
             * TODO: Add code to process resizing, when/if needed.
             */
            return TRUE;

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam))
            {
                case ID_DOIT:
                    do_something(hwndDlg);
                    return TRUE;
            }
            break;

        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            return TRUE;

        /*
         * TODO: Add more messages, when needed.
         */
    }

    return FALSE;
}

static INT_PTR CALLBACK ImgExpProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
