#include <wincodec.h>
#include <tchar.h>
#include <stdio.h>

/* helper debug */
static void dbg_num(char *msg, unsigned long int n)
{
	char dbuf[1024];
	snprintf(dbuf,1024,"%s: 0x%x (%d)",msg,n,n);
	OutputDebugStringA(dbuf);
}



int load_img(wchar_t *fn,int coinited, HDC *dhdc, HBITMAP *dhbmp)
{
	wchar_t buf[260];
	HRESULT hr;
	/* WIM part */
	IWICImagingFactory *iif;
	IWICBitmapDecoder *iwbdec;
	IWICBitmapFrameDecode *iwbframe;
	IWICComponentInfo *tci;		/* "temp" */
	IWICPixelFormatInfo *pfi;	/* pixel format info interface */
	WICPixelFormatGUID pfuid;

    IWICColorTransform *pcoltr = NULL;
    IWICColorContext *pcsrc = NULL;
    IWICColorContext *pcdst = NULL;



	UINT w,h;
	UINT chcount,bpp;

	/* GDI part */
	HDC hdci;
	HBITMAP hBMP;
	BITMAPINFO bi;
	int xs,ys;
	void *pxdata;

	if(!coinited)
	{
		hr = CoInitialize(NULL);
		if(hr!=S_OK)
		{
			swprintf(buf,256,L"CoInitialize returned error: %d, 0x%x",hr,hr);
			OutputDebugString(buf);
			return -2;
		}
	}

	hr = CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory, (void**)&iif);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CoCreateInstance returned error: %d, 0x%x",hr,hr);
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
#ifdef DEBUG
	int fc;
	iwbdec->lpVtbl->GetFrameCount(iwbdec,&fc);
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
#endif
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
#ifdef DEBUG
	dbg_num("bpp: ",bpp);
	dbg_num("channel count: ",chcount);
#endif

	/* color transform */
	/* create objects */
	hr=iif->lpVtbl->CreateColorTransformer(iif,&pcoltr);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateColorTransformer returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	hr=iif->lpVtbl->CreateColorContext(iif,&pcsrc);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateColorContext returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	hr=iif->lpVtbl->CreateColorContext(iif,&pcdst);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CreateColorContext returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}

	/* initialize ColorContexts */
	hr=pcsrc->lpVtbl->InitializeFromExifColorSpace(pcsrc,1);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"ColorContext init returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	hr=pcdst->lpVtbl->InitializeFromExifColorSpace(pcdst,1);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"ColorContext init returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}

	hr=pcoltr->lpVtbl->Initialize(pcoltr,(IWICBitmapSource *)iwbframe,pcsrc,pcdst,&GUID_WICPixelFormat32bppBGRA);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"Initialize color transform returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	/* GDI */
	/* Device Context */
	hdci=CreateCompatibleDC(0);
	xs=w;
	ys=h;
	/* prepare for DIB generation */
	bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth=xs;
	bi.bmiHeader.biHeight=-ys;
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

	BITMAP bmp;

	GetObject(hBMP,sizeof(bmp),&bmp);
	dbg_num("created x: ",bmp.bmWidth);
	dbg_num("created y: ",bmp.bmHeight);
	dbg_num("created b: ",bmp.bmWidthBytes);
	dbg_num("created bpp: ",bmp.bmBitsPixel);

/*	void *pxbuf2=calloc(w*h,4);
	if(pxbuf2==NULL)
	{
		OutputDebugStringA("alloc failed");
		return -1;
	}
	unsigned int *dbuf2=pxdata;
	dbuf2[0]=0x808080;

	hr=pcoltr->lpVtbl->CopyPixels(pcoltr,NULL,4*w,4*w*h,pxbuf2);
	OutputDebugStringA("done pxbuf2"); */

	/* since we forced bpp to 32 we know that stride must be 4*width */
	hr=pcoltr->lpVtbl->CopyPixels(pcoltr,NULL,4*w,4*w*h,pxdata);
//	hr=iwbframe->lpVtbl->CopyPixels(iwbframe,NULL,4*w,4*w*h,pxdata);
	if(hr!=S_OK)
	{
		swprintf(buf,256,L"CopyPixels returned error: %d, 0x%x",hr,hr);
		OutputDebugString(buf);
		return -1;
	}
	OutputDebugStringA("done pxdata");

	
	SelectObject(hdci,hBMP);
	*dhdc=hdci;
	*dhbmp=hBMP;
	return 0;
}
