#define WIN32_LEAN_AND_MEAN
#include <wincodec.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>

/* note to self: check https://learn.microsoft.com/en-us/windows/win32/api/wincodec/nf-wincodec-iwicformatconverter-initialize */

/* helper debug */
static void dbg_num(char *msg, unsigned long int n)
{
	char dbuf[1024];
	snprintf(dbuf,1024,"%s: 0x%x (%d)",msg,n,n);
	OutputDebugStringA(dbuf);
}


int load_img(wchar_t *fn, HDC *dhdc)
{
	int rv=0;
	HRESULT hr;
	/* WIM part */
	IWICImagingFactory *iif=NULL;
	IWICBitmapDecoder *iwbdec=NULL;
	IWICBitmapFrameDecode *iwbframe=NULL;
	IWICFormatConverter *ifc=NULL;
	unsigned int fc;

	wchar_t buf[260];

#ifdef DEBUG
	IWICComponentInfo *tci=NULL;		/* "temp" */
	IWICPixelFormatInfo *pfi=NULL;	/* pixel format info interface */
	WICPixelFormatGUID pfuid;
	UINT chcount,bpp;
#endif

	UINT w,h;

	/* GDI part */
	HDC hdci;
	HBITMAP hBMP;
	BITMAPINFO bi;
	void *pxdata;

	hr = CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory, (void**)&iif);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CoCreateInstance returned error: %d, 0x%x - was CoInitialize called before this?",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	
	hr=iif->lpVtbl->CreateDecoderFromFilename(iif,fn,NULL,GENERIC_READ,WICDecodeMetadataCacheOnDemand,&iwbdec);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateDecoderFromFilename returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	iwbdec->lpVtbl->GetFrameCount(iwbdec,&fc);
	if(fc<1)
	{
		swprintf(buf,256,L"No frames found");
		OutputDebugString(buf);
		return -1;
	}
#ifdef DEBUG
	dbg_num("Frame count: ",fc);
#endif

	hr=iwbdec->lpVtbl->GetFrame(iwbdec,0,&iwbframe);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"GetFrame returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	/* now iwbframe should have our pixels, but first we need dimensions */
	/* TODO: error checking here */
	iwbframe->lpVtbl->GetSize(iwbframe,&w,&h);
	iwbframe->lpVtbl->GetPixelFormat(iwbframe,&pfuid);
#ifdef DEBUG
	dbg_num("Width: ",w);
	dbg_num("Height: ",h);
	/* fucking more interfaces to get to the fucking pixel data information */
	iif->lpVtbl->CreateComponentInfo(iif,&pfuid,&tci);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateComponentInfo returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}

	/* pixel format interface */
	hr=tci->lpVtbl->QueryInterface(tci,&IID_IWICPixelFormatInfo,(void **)&pfi);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateComponentInfo returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}

	pfi->lpVtbl->GetBitsPerPixel(pfi,&bpp);
	pfi->lpVtbl->GetChannelCount(pfi,&chcount);
	dbg_num("bpp: ",bpp);
	dbg_num("channel count: ",chcount);
#endif

	/* format converter */
	hr=iif->lpVtbl->CreateFormatConverter(iif,&ifc);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateFormatConverter returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}

   hr=ifc->lpVtbl->Initialize(ifc,(IWICBitmapSource *)iwbframe,&GUID_WICPixelFormat32bppBGRA,WICBitmapDitherTypeNone,NULL,0.0f,WICBitmapPaletteTypeCustom);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateFormatConverter->Initialize returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}

	

	/* GDI */
	/* Device Context */
	hdci=CreateCompatibleDC(0);
	/* prepare for DIB generation */
	bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth=w;
	bi.bmiHeader.biHeight=0-h;
	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biBitCount=32;
	bi.bmiHeader.biCompression=BI_RGB;
	bi.bmiHeader.biSizeImage=0;
	bi.bmiHeader.biClrUsed=0;
	bi.bmiHeader.biClrImportant=0;

	hBMP=CreateDIBSection(hdci,&bi,DIB_RGB_COLORS,&pxdata,NULL,0);
	if(hBMP==NULL || pxdata==NULL)
	{
		OutputDebugString(_T("CreateDIBSection failed"));
		return -1;
	}

#ifdef DEBUG
	BITMAP bmp;

	GetObject(hBMP,sizeof(bmp),&bmp);
	dbg_num("created x: ",bmp.bmWidth);
	dbg_num("created y: ",bmp.bmHeight);
	dbg_num("created b: ",bmp.bmWidthBytes);
	dbg_num("created bpp: ",bmp.bmBitsPixel);
#endif

	/* since the bitmap was created with 32bpp, we know that stride must be 4*width and buffer size is 4*width*height */
	hr=ifc->lpVtbl->CopyPixels(ifc,NULL,4*w,4*w*h,pxdata);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CopyPixels returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	
	SelectObject(hdci,hBMP);
	*dhdc=hdci;
	return rv;
}
