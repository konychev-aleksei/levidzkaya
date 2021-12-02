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

const int SCREEN_HEIGHT = 800;
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


BOOL DrawLine(HDC hdc, int x0, int y0, int x, int y) {
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

void DrawPixel(HDC hdc, int x0, int y0) {
	Rectangle(hdc, x0 * GRID_SIZE + 1, y0 * GRID_SIZE + 1, x0 * GRID_SIZE + GRID_SIZE + 1, y0 * GRID_SIZE + GRID_SIZE + 1);
}


set<pair<int, int>> CDA(int x1, int y1, int x2, int y2) {
	set<pair<int, int>> out;

	float dx = x1 - x2;
	float dy = y1 - y2;

	if (dx * dy != 0) {
		float df;

		if (abs(dy) > abs(dx)) {
			df = dx / dy;
			float x;

			if (y1 > y2)
			{
				x = y1;
				y1 = y2;
				y2 = (int)x;
				x = x1;
				x1 = x2;
				x2 = (int)x;
			}

			x = x1;

			for (int i = y1; i < y2; i++) {
				x += df;
				out.insert({ (int)trunc(x), i });
			}
		}
		else
		{
			df = dy / dx;
			float y;

			if (x1 > x2)
			{
				y = x1;
				x1 = x2;
				x2 = (int)y;
				y = y1;
				y1 = y2;
				y2 = (int)y;
			}

			y = y1;

			for (int i = x1; i < x2; i++) {
				y += df;
				out.insert({ i, (int)trunc(y) });
			}
		}
	}
	else if (dx == 0)
		for (int i = min(y1, y2); i < max(y1, y2); i++) {
			out.insert({ x1, i });
		}
	else
		for (int i = min(x1, x2); i < max(x1, x2); i++) {
			out.insert({ i, y1 });
		}

	return out;
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


void drawFigure(HDC memDC, int offset) {
	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(0, 0, 0));

	SelectObject(memDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(memDC, RGB(0, 200, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		set<pair<int, int>> plots =
			offset ?
			Bresenham(figure[i][0] / GRID_SIZE + offset, figure[i][1] / GRID_SIZE, figure[i][2] / GRID_SIZE + offset, figure[i][3] / GRID_SIZE)
			:
			CDA(figure[i][0] / GRID_SIZE + offset, figure[i][1] / GRID_SIZE, figure[i][2] / GRID_SIZE + offset, figure[i][3] / GRID_SIZE)
			;

		for (auto i : plots) {
			DrawPixel(memDC, i.first, i.second);
		}
	}

	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(200, 0, 0));

	for (int i = 0; i < (int)figure.size(); ++i) {
		//DrawLine(memDC, figure[i][0] + offset * 10, figure[i][1] + offset / 10, figure[i][2] + offset * 10, figure[i][3] + offset / 10);
	}
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

	drawFigure(memDC, 0);
	drawFigure(memDC, GRID_SIZE * 5);

	SelectObject(memDC, GetStockObject(DC_PEN));
	SetDCPenColor(memDC, RGB(0, 0, 0));

	SelectObject(memDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(memDC, RGB(0, 200, 0));
	set<pair<int, int>> plots = Bresenham(x1, y1 + 30, x2, y2 + 30);

	for (auto i : plots) {
		DrawPixel(memDC, i.first, i.second);
	}
}
