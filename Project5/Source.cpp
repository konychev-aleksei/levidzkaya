#include "interpolation.h"

RECT rct;

void canvas(HDC dc, HWND hwnd) {
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP memBM = CreateCompatibleBitmap(dc, rct.right - rct.left, rct.bottom - rct.top);
	SelectObject(memDC, memBM);
	draw(memDC, rct);
	BitBlt(dc, 0, 0, rct.right - rct.left, rct.bottom - rct.top, memDC, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, memDC);
	DeleteDC(memDC);
	DeleteObject(memBM);
}

LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	if (message == WM_DESTROY)
		PostQuitMessage(0);
	else if (message == WM_SIZE) 
		GetClientRect(hwnd, &rct);
	else 
		return DefWindowProcA(hwnd, message, wparam, lparam);
}

int main() {
	WNDCLASSA wcl;
	memset(&wcl, 0, sizeof(WNDCLASSA));
	wcl.lpszClassName = "my Window";
	wcl.lpfnWndProc = WndProc;
	RegisterClassA(&wcl);

	HWND hwnd;
	hwnd = CreateWindow("my Window", "Lab1", WS_OVERLAPPEDWINDOW, 100, 100, 992, 515, NULL, NULL, NULL, NULL);

	HDC dc = GetDC(hwnd);
	ShowWindow(hwnd, SW_SHOWNORMAL);

	MSG msg;
	while (1) {
		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			canvas(dc, hwnd);
			Sleep(1);
		}
	}


}


