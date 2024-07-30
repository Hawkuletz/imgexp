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

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <combaseapi.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>
#include "imgexp.h"
#include "img2dc.h"

/* often used */
#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
#define BUF_SIZE 4096

/** Prototypes **************************************************************/

static INT_PTR CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK ImgExpProc(HWND, UINT, WPARAM, LPARAM);

/** Global variables ********************************************************/

static HANDLE ghInstance;
HDC img_dc=NULL;

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

	if(CoInitialize(NULL)!=S_OK) return 1;
	
    /* The user interface is a modal dialog box */
    return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)MainDlgProc);
}

/* helper functions */
void dbg_num(char *msg, unsigned long int n)
{
	char dbuf[1024];
	snprintf(dbuf,1024,"%s: 0x%x (%d)",msg,n,n);
	OutputDebugStringA(dbuf);
}

void dbg_dbl(char *msg, double n)
{
	char dbuf[1024];
	snprintf(dbuf,1024,"%s: %.4f",msg,n);
	OutputDebugStringA(dbuf);
}



/**********************************
 * application specific functions *
 **********************************/
int init_idc(HWND hDlg)
{
	return 0;
}

int do_something(HWND hDlg)
{
	HBITMAP hbmp;
	wchar_t fn[256];
	if(img_dc!=NULL)
	{
		hbmp=GetCurrentObject(img_dc,OBJ_BITMAP);
		DeleteDC(img_dc);
		DeleteObject(hbmp);
	}
	GetDlgItemText(hDlg,ID_EDH,fn,255);
	fn[255]=0;
	load_img(fn,&img_dc);
	if(img_dc==NULL) OutputDebugStringA("no img_dc");
	InvalidateRect(GetDlgItem(hDlg,ID_IMG),NULL,FALSE);
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
			init_idc(hwndDlg);
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
	PAINTSTRUCT ps;
	HDC wdc;
	BITMAP bmp;
	HBITMAP hbmp;
	RECT rct;

;
    switch (uMsg)
    {
		case WM_PAINT:
			if(!img_dc) return DefWindowProc(hWnd,uMsg,wParam,lParam);
			if(GetClientRect(hWnd,&rct)==0) return FALSE;
			wdc=BeginPaint(hWnd,&ps);
			hbmp=GetCurrentObject(img_dc,OBJ_BITMAP);
			GetObject(hbmp,sizeof(bmp),&bmp);
			SetStretchBltMode(wdc,HALFTONE);
			StretchBlt(wdc,0,0,rct.right,rct.bottom,img_dc,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
			EndPaint(hWnd,&ps);
			return TRUE;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
