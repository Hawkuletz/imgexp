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
#include "limg2.h"

/* often used */
#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
#define BUF_SIZE 4096

/** Prototypes **************************************************************/

static INT_PTR CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK ImgExpProc(HWND, UINT, WPARAM, LPARAM);

/** Global variables ********************************************************/

static HANDLE ghInstance;
HDC idc=NULL;
HBITMAP ibmp;
HBRUSH hbrush;
RECT irct;

HDC img_dc=NULL;
HBITMAP img_bmp=NULL;

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
	HDC cdc;
	HWND hic=GetDlgItem(hDlg,ID_IMG);
	if(GetClientRect(hic,&irct)==0) dbg_num("GetClientRect failed, lasterror is",GetLastError());
	cdc=GetDC(hic);
	idc=CreateCompatibleDC(cdc);
	ibmp=CreateCompatibleBitmap(cdc,irct.right,irct.bottom);
	SelectObject(idc,ibmp);
	hbrush=CreateSolidBrush(0x0000ff);
	SelectObject(idc,hbrush);
	Rectangle(idc,irct.left,irct.top,irct.right,irct.bottom);
	ReleaseDC(hic,cdc);
	return 0;
}

int do_something(HWND hDlg)
{
	wchar_t fn[256];
	OutputDebugString(_T("something"));
	GetDlgItemText(hDlg,ID_EDH,fn,255);
	fn[255]=0;
	load_img(fn,&img_dc,&img_bmp);
//	load_img(L"retro_t.png",0,&img_dc,&img_bmp);
	if(img_dc==NULL) OutputDebugStringA("no img_dc");
	InvalidateRect(GetDlgItem(hDlg,ID_IMG),&irct,FALSE);
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

;
    switch (uMsg)
    {
		case WM_PAINT:
			if(!idc && !img_dc) return DefWindowProc(hWnd,uMsg,wParam,lParam);
			wdc=BeginPaint(hWnd,&ps);
			if(img_dc)
			{
				hbmp=GetCurrentObject(img_dc,OBJ_BITMAP);
				GetObject(hbmp,sizeof(bmp),&bmp);
				SetStretchBltMode(wdc,HALFTONE);
				StretchBlt(wdc,0,0,irct.right,irct.bottom,img_dc,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
			}
			else
			{
				BitBlt(wdc,0,0,irct.right,irct.bottom,idc,0,0,SRCCOPY);
			}
			EndPaint(hWnd,&ps);
			return TRUE;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
