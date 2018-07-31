
#include "stdafx.h"

VOID  OnPaint(HDC hdc)
{
	// get screen/window information
	BITMAP structBitmapHeader;
	memset(&structBitmapHeader, 0, sizeof(BITMAP));

	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);

	// windows width and height
	LONG width = structBitmapHeader.bmWidth;
	LONG height = structBitmapHeader.bmHeight;

	// bits per pixel, normally it is 24 (3 bytes) or 32 (4 bytes)
	WORD bytesPerPixel = structBitmapHeader.bmBitsPixel / 8;

	//
	// allocate pixel buffer for drawing
	//
	// NOTE: this is BAD practice that should only exist in 
	//       demo purpose, in real case, programmer should
	//       allocate buffer only once if the window size
	//       is same as before
	LONG bufferSize = width * height * bytesPerPixel;
	BYTE* buffer = new BYTE[bufferSize];

	// set all pixels to black 
	for (int i = 0; i < bufferSize; i = i + bytesPerPixel)
	{
		buffer[i] = 0;     // BLUE
		buffer[i + 1] = 0; // GREEN
		buffer[i + 2] = 0; // RED
	}

	// update the screen with buffer
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biBitCount = structBitmapHeader.bmBitsPixel;
	bih.biClrImportant = 0;
	bih.biClrUsed = 0;
	bih.biCompression = BI_RGB;
	bih.biWidth = structBitmapHeader.bmWidth;
	bih.biHeight = structBitmapHeader.bmHeight;
	bih.biPlanes = 1;
	bih.biSizeImage = bufferSize;

	BITMAPINFO bi;
	bi.bmiHeader = bih;
	SetDIBitsToDevice(
		hdc, 0, 0, 
		width, height, 
		0, 0, 0, height,
		buffer, &bi, DIB_RGB_COLORS);

	// release buffer
	delete[] buffer;
}