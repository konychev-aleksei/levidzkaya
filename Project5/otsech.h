#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <utility> 

#define pT 8

using namespace std;

const int SCREEN_HEIGHT = 515;
const int GRID_SIZE = 5;

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

vector<int> region = { 15, 15, 75, 75 };


BOOL DrawLine(HDC hdc, int x0, int y0, int x, int y) {
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

void DrawPixel(HDC hdc, int x0, int y0) {
	Rectangle(hdc, x0 * GRID_SIZE + 1, y0 * GRID_SIZE + 1, (x0 + 1) * GRID_SIZE, (y0 + 1) * GRID_SIZE);
}

set<pair<int, int>> Bresenham(int x1, int y1, int x2, int y2) {
	set<pair<int, int>> out;

	if (x1 == x2) {
		for (int y = y1; y < y2; y++) {
			out.insert({ (int)floor(x1 / GRID_SIZE), (int)floor(y / GRID_SIZE) });
		}
	}
	else {
		int heightDiff = 2 * abs(y2 - y1);
		int slopeError = heightDiff - (x2 - x1);
		for (int x = x1, y = y1; x <= x2; x++) {
			out.insert({ (int)floor(x / GRID_SIZE), (int)floor(y / GRID_SIZE) });
			slopeError += heightDiff;

			if (slopeError >= 0) {
				y += 2 * (y2 > y1) - 1;
				slopeError -= 2 * (x2 - x1);
			}
		}
	}

	return out;
}

const int INSIDE = 0;
const int LEFT = 1;
const int RIGHT = 2; 
const int BOTTOM = 4;
const int TOP = 8; 

const int x_max = region[3] * GRID_SIZE;
const int y_max = region[2] * GRID_SIZE;
const int x_min = region[0] * GRID_SIZE;
const int y_min = region[1] * GRID_SIZE;


int computeCode(double x, double y) {
	int code = INSIDE;

	if (x < x_min) 
		code |= LEFT;
	else if (x > x_max) 
		code |= RIGHT;
	if (y < y_min) 
		code |= BOTTOM;
	else if (y > y_max)
		code |= TOP;

	return code;
}

vector<int> cohenSutherlandClip(int x1, int y1, int x2, int y2) {
	int code1 = computeCode(x1, y1);
	int code2 = computeCode(x2, y2);

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
				code1 = computeCode(x1, y1);
			}
			else {
				x2 = x;
				y2 = y;
				code2 = computeCode(x2, y2);
			}
		}
	}

	vector<int> resolve = { x1, y1, x2, y2 };
	vector<int> reject = { };

	return accept ? resolve : reject;
}

void drawFigure(HDC memDC, int offset) {
	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(0, 0, 0));

	SelectObject(memDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(memDC, RGB(0, 200, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		vector<int> p = cohenSutherlandClip(figure[i][0], figure[i][1], figure[i][2], figure[i][3]);

		//cout << figure[i][0] << ' ' << figure[i][1] << ' ' << figure[i][2] << ' ' << figure[i][3] << endl;

		//if (p.size())
		//	cout << p[0] << ' ' << p[1] << ' ' << p[2] << ' ' << p[3] << endl;

		
		if ((int)p.size()) {
			set<pair<int, int>> plots = Bresenham(p[0], p[1], p[2], p[3]);

			for (auto i : plots) {
				DrawPixel(memDC, i.first, i.second);
			}
		}
		
	}

	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(200, 0, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		DrawLine(memDC, figure[i][0] + offset, figure[i][1], figure[i][2] + offset, figure[i][3]);
	}
}


void draw(HDC memDC, RECT rct) {
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
	for (int i = region[0]; i < region[2]; ++i)
		for (int j = region[1]; j < region[3]; ++j) {
			DrawPixel(memDC, i, j);
		}

	drawFigure(memDC, 0);
}
