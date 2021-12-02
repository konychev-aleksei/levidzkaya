#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <utility> 

using namespace std;

const int SCREEN_HEIGHT = 515;
const int GRID_SIZE = 10;

vector<vector<int>> figure = {
	{ 110, 200, 110, 250 },
	{ 280, 250, 280, 300 },
	{ 410, 150, 410, 200 },
	{ 110, 250, 280, 300 },
	{ 280, 300, 410, 200 },
	{ 110, 200, 280, 250 },
	{ 280, 250, 410, 150 },
	{ 280, 100, 410, 150 },
	{ 110, 200, 280, 100 }
};

vector<int> region = { 0, 0, 50, 100 };


BOOL DrawLine(HDC hdc, int x0, int y0, int x, int y) {
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

void DrawPixel(HDC hdc, int x0, int y0, int X1, int Y1, int X2, int Y2) {
	if (x0 >= X1 + 1 && x0 <= X2 - 1 && y0 >= Y1 + 1 && y0 <= Y2 - 1) {
		Rectangle(hdc, x0 * GRID_SIZE + 1, y0 * GRID_SIZE + 1, x0 * GRID_SIZE + GRID_SIZE + 1, y0 * GRID_SIZE + GRID_SIZE + 1);
	}	
}

set<pair<int, int>> Bresenham(int x1, int y1, int x2, int y2) {
	set<pair<int, int>> out;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = 2 * (x2 >= x1) - 1;
	int sy = 2 * (y2 >= y1) - 1;

	if (dy < dx)
	{
		int d = (dy << 1) - dx;
		int d1 = dy << 1;
		int d2 = (dy - dx) << 1;
		int x = x1 + sx;
		int y = y1;

		out.insert({ x1, y1 });

		for (int i = 1; i <= dx; i++)
		{
			if (d > 0)
			{
				d += d2;
				y += sy;
			}
			else
				d += d1;

			out.insert({ x, y });
			x += sx;
		}
	}
	else
	{
		int d = (dx << 1) - dy;
		int d1 = dx << 1;
		int d2 = (dx - dy) << 1;
		int x = x1;
		int y = y1 + sy;

		out.insert({ x1, y1 });

		for (int i = 1; i <= dy; i++)
		{
			if (d > 0)
			{
				d += d2;
				x += sx;
			}
			else
				d += d1;

			out.insert({ x, y });
			y += sy;
		}
	}

	return out;
}

const int INSIDE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int BOTTOM = 4;
const int TOP = 8;


int computeCode(double x, double y, int X1, int Y1, int X2, int Y2) {
	int code = INSIDE;

	if (x < X1)
		code |= LEFT;
	else if (x > X2)
		code |= RIGHT;
	if (y < Y1)
		code |= BOTTOM;
	else if (y > Y2)
		code |= TOP;

	return code;
}

vector<int> cohenSutherlandClip(int x1, int y1, int x2, int y2, int X1, int Y1, int X2, int Y2) {
	const int x_max = X2;
	const int y_max = Y2;
	const int x_min = X1;
	const int y_min = Y1;

	int code1 = computeCode(x1, y1, X1, Y1, X2, Y2);
	int code2 = computeCode(x2, y2, X1, Y1, X2, Y2);

	bool accept = false;

	while (true) {
		if ((code1 == 0) && (code2 == 0)) {
			accept = true;
			break;
		}
		else if (code1 & code2) {
			break;
		}
		else {
			int code_out, x, y;

			if (code1 != 0)
				code_out = code1;
			else
				code_out = code2;

			if (code_out & TOP) {
				x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
				y = y_max;
			}
			else if (code_out & BOTTOM) {
				x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
				y = y_min;
			}
			else if (code_out & RIGHT) {
				y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
				x = x_max;
			}
			else if (code_out & LEFT) {
				y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
				x = x_min;
			}

			if (code_out == code1) {
				x1 = x;
				y1 = y;
				code1 = computeCode(x1, y1, X1, Y1, X2, Y2);
			}
			else {
				x2 = x;
				y2 = y;
				code2 = computeCode(x2, y2, X1, Y1, X2, Y2);
			}
		}
	}

	vector<int> resolve = { x1, y1, x2, y2 };
	vector<int> reject = { };

	return accept ? resolve : reject;
}

void drawFigure(HDC memDC, int offset, int X1, int Y1, int X2, int Y2) {
	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(0, 0, 0));

	SelectObject(memDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(memDC, RGB(0, 200, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		vector<int> p = cohenSutherlandClip(figure[i][0] / GRID_SIZE, figure[i][1] / GRID_SIZE, figure[i][2] / GRID_SIZE, figure[i][3] / GRID_SIZE, X1, Y1, X2, Y2);
		//cout << figure[i][0] / GRID_SIZE << ' ' << figure[i][1] / GRID_SIZE << ' ' << figure[i][2] / GRID_SIZE << ' ' << figure[i][3] / GRID_SIZE << endl;

		if ((int)p.size()) {
			set<pair<int, int>> plots = Bresenham(p[0], p[1], p[2], p[3]);

			for (auto i : plots) {
				DrawPixel(memDC, i.first, i.second, X1, Y1, X2, Y2);
			}
		}

	}

	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(200, 0, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		DrawLine(memDC, figure[i][0] + offset, figure[i][1] + 3, figure[i][2] + offset, figure[i][3] + 3);
	}
}

void DrawPixel(HDC hdc, int x0, int y0) {
	Rectangle(hdc, x0 * GRID_SIZE + 1, y0 * GRID_SIZE + 1, x0 * GRID_SIZE + GRID_SIZE + 1, y0 * GRID_SIZE + GRID_SIZE + 1);
}

void draw(HDC memDC, RECT rct, int x1, int y1, int x2, int y2) {
	int width = rct.right - rct.left;
	int height = rct.bottom - rct.top;


	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(50, 50, 50));

	for (int i = 0; i < 100; ++i) {
		DrawLine(memDC, 0, i * GRID_SIZE, width, i * GRID_SIZE);
	}

	for (int i = 0; i < 200; ++i) {
		DrawLine(memDC, i * GRID_SIZE, 0, i * GRID_SIZE, height);
	}


	SelectObject(memDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(memDC, RGB(50, 50, 50));
	for (int i = x1; i < x2; ++i)
		for (int j = y1; j < y2; ++j) {
			DrawPixel(memDC, i, j, x1, y1, x2, y2);
		}

	drawFigure(memDC, 0, x1, y1, x2, y2);
}
