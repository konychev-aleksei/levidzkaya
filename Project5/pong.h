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

int const GRID_SIZE = 5;

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


BOOL DrawLine(HDC hdc, int x0, int y0, int x, int y) {
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

void DrawPixel(HDC hdc, int x0, int y0) {
	Rectangle(hdc, x0 * GRID_SIZE + 1, y0 * GRID_SIZE + 1, (x0 + 1) * GRID_SIZE, (y0 + 1) * GRID_SIZE);
}


set<pair<int, int>> CDA(int x0, int y0, int x1, int y1) {
	set<pair<int, int>> out;
	int dx = x1 - x0,
		dy = y1 - y0,
		s = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	double xi = float(dx) / s,
		yi = float(dy) / s;

	double x = x0, y = y0;
	out.insert({ x0, y0 });

	for (int i = 0; i < s; i++) {
		x += xi;
		y += yi;
		out.insert({ (int)floor(x / GRID_SIZE), (int)floor(y / GRID_SIZE) });
	}

	return out;
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


void drawFigure(HDC memDC, int offset) {
	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(0, 0, 0));

	SelectObject(memDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(memDC, RGB(0, 200, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		set<pair<int, int>> plots = 
			offset ? 
					Bresenham(figure[i][0] + offset, figure[i][1], figure[i][2] + offset, figure[i][3])
				:
					CDA(figure[i][0] + offset, figure[i][1], figure[i][2] + offset, figure[i][3])
			;

		for (auto i : plots) {
			DrawPixel(memDC, i.first, i.second);
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

	//Отрисовка сетки
	for (int i = 0; i < 100; ++i) {
		DrawLine(memDC, 0, i * GRID_SIZE, width, i * GRID_SIZE);
	}

	for (int i = 0; i < 200; ++i) {
		DrawLine(memDC, i * GRID_SIZE, 0, i * GRID_SIZE, height);
	}

	drawFigure(memDC, 0);
	drawFigure(memDC, 400);
}