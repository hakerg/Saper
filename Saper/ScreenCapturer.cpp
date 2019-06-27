#include "ScreenCapturer.h"
#include <iostream>


ScreenCapturer::ScreenCapturer()
{
	hDesktopDC = GetDC(NULL);
	hMyDC = CreateCompatibleDC(hDesktopDC);
	memset(&bi, 0, sizeof(&bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biWidth = 1366;
	bi.bmiHeader.biHeight = -768;
	hBitmap = CreateCompatibleBitmap(hDesktopDC, 1366, 768);
	SelectObject(hMyDC, hBitmap);
	screenData = new RGBQUAD[1366 * 768];
}


ScreenCapturer::~ScreenCapturer()
{
	DeleteObject(hBitmap);
	ReleaseDC(NULL, hDesktopDC);
	DeleteDC(hMyDC);
	delete[] screenData;
}


void ScreenCapturer::takeScreenshot()
{
	BitBlt(hMyDC, 0, 0, 1366, 768, hDesktopDC, 0, 0, SRCCOPY);
	GetDIBits(hMyDC, hBitmap, 0, 768, screenData, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
}

RGBQUAD& ScreenCapturer::pixel(int x, int y)
{
	return screenData[x + 1366 * y];
}
