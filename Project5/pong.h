//#include "interpolation.h"
#include "rastr.h"
//#include "otsech.h"

RECT rct;

void canvas(HDC dc, HWND hwnd, int x1, int y1, int x2, int y2) {
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP memBM = CreateCompatibleBitmap(dc, rct.right - rct.left, rct.bottom - rct.top);
	SelectObject(memDC, memBM); 
	//draw(memDC, rct, x1, y1, x2, y2); 
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
	hwnd = CreateWindow("my Window", "Window", WS_OVERLAPPEDWINDOW, 100, 100, 992, SCREEN_HEIGHT, NULL, NULL, NULL, NULL);

	HDC dc = GetDC(hwnd);
	ShowWindow(hwnd, SW_SHOWNORMAL);

	srand(time(NULL));
	MSG msg;

	int timer = 0;
	int x1, y1, x2, y2;

	while (1) {
		if (timer % 120 == 0) {
			
			x1 = rand() % 90;
			y1 = rand() % 50;
			x2 = rand() % 90;
			y2 = rand() % 50;
			
			
			//x1 = rand() % 25;
			//y1 = rand() % 25;
			//x2 = rand() % 25 + x1 + 10;
			//y2 = rand() % 25 + y1 + 10;
			
			timer = 1;
		}
		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			canvas(dc, hwnd, x1, y1, x2, y2);
			Sleep(1);
			timer++;
		}
	}


}
