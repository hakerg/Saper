#pragma once
#include <Windows.h>

class ScreenCapturer
{
private:
	HDC hDesktopDC;
	HDC hMyDC;
	HBITMAP hBitmap;
	BITMAPINFO bi;

public:

	RGBQUAD* screenData;

	ScreenCapturer();
	~ScreenCapturer();
	void takeScreenshot();
	RGBQUAD& pixel(int x, int y);
};