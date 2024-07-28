#include <wincodec.h>
#include <wchar.h>
#ifdef DEBUG
#include <stdio.h>
#endif

int load_img(wchar_t *fn,int coinited)
{
	wchar_t buf[260];
	HRESULT hr;
	IWICImagingFactory *iif;
	IWICBitmapDecoder *iwbdec;
	IWICBitmapFrameDecode *iwbframe;
	IWICComponentInfo *tci;		/* "temp" */
	IWICPixelFormatInfo *pfi;	/* pixel format info interface */
	WICPixelFormatGUID pfuid;
	UINT w,h;
	UINT chcount,bpp;

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
	fprintf(stderr,"frame count: %d\n",fc);
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
	fprintf(stderr,"size: %u x %u\n",w,h);
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
	fprintf(stderr,"bpp: %u cc: %u\n",bpp,chcount);
#endif



	return 0;
}
