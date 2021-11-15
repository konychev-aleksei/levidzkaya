#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <utility> 
#include <stdio.h>
#include <stdlib.h>

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

double getTriangleArea(int x0, int y0, int x1, int y1, int x2, int y2) {
	return .5 * fabs(double(x0) * (y1 - y2) + double(x1) * (y2 - y0) + double(x2) * (y0 - y1));
}

vector<int> getBaricentricCoordinatesColor(vector<vector<int>> triangle, int x, int y) {
	int ABC = getTriangleArea(triangle[0][0], triangle[0][1], triangle[1][0], triangle[1][1], triangle[2][0], triangle[2][1]);
	
	int ABX = getTriangleArea(triangle[0][0], triangle[0][1], triangle[1][0], triangle[1][1], x * GRID_SIZE, y * GRID_SIZE);
	int ACX = getTriangleArea(x * GRID_SIZE, y * GRID_SIZE, triangle[1][0], triangle[1][1], triangle[2][0], triangle[2][1]);
	int BCX = getTriangleArea(triangle[0][0], triangle[0][1], x * GRID_SIZE, y * GRID_SIZE, triangle[2][0], triangle[2][1]);

	int R = (int)(255 * ABX / ABC);
	int G = (int)(255 * ACX / ABC);
	int B = (int)(255 * BCX / ABC);

	vector<int> colors = { R, G, B };

	return colors;
}

BOOL DrawLine(HDC hdc, int x0, int y0, int x, int y) {
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

void DrawPixel(HDC hdc, vector<int> V) {
	SetDCBrushColor(hdc, RGB(V[2], V[3], V[4]));
	Rectangle(hdc, V[0] * GRID_SIZE + 1, V[1] * GRID_SIZE + 1, (V[0] + 1) * GRID_SIZE, (V[1] + 1) * GRID_SIZE);
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


void drawTriangle_BaricentricCoordinates(HDC memDC, vector<vector<int>> triangle, int offset) {
	SelectObject(memDC, GetStockObject(DC_BRUSH));


	int leftX = min(min(triangle[0][0], triangle[1][0]), triangle[2][0]) / GRID_SIZE;
	int rightX = max(max(triangle[0][0], triangle[1][0]), triangle[2][0]) / GRID_SIZE;

	int upY = min(min(triangle[0][1], triangle[1][1]), triangle[2][1]) / GRID_SIZE;
	int downY = max(max(triangle[0][1], triangle[1][1]), triangle[2][1]) / GRID_SIZE;

	set<pair<int, int>> AB = Bresenham(triangle[0][0], triangle[0][1], triangle[1][0], triangle[1][1]);
	set<pair<int, int>> BC = Bresenham(triangle[1][0], triangle[1][1], triangle[2][0], triangle[2][1]);
	set<pair<int, int>> AC = Bresenham(triangle[0][0], triangle[0][1], triangle[2][0], triangle[2][1]);

	set<pair<int, int>> plots;
	for (auto i : AB) {
		plots.insert({ i.first, i.second });
	}
	for (auto i : BC) {
		plots.insert({ i.first, i.second });
	}
	for (auto i : AC) {
		plots.insert({ i.first, i.second });
	}


	for (auto i : plots) {
		vector<int> color = getBaricentricCoordinatesColor(triangle, i.first, i.second);
		vector<int> pixelInfo = { i.first + offset, i.second, color[0], color[1], color[2] };
		DrawPixel(memDC, pixelInfo);
	}

	for (int i = upY; i <= downY; i++) {
		bool painted = false;
		set<int> hor;
		for (int j = leftX; j <= rightX; j++) {
			if (plots.count({ j, i }) && painted) {
				break;
			}

			if (plots.count({ j - 1, i }) && !plots.count({ j, i })) {
				painted = true;
			}

			if (painted) {
				hor.insert(j);
			}

			if (j == rightX) {
				hor.clear();
			}
		}

		for (auto j : hor) {
			vector<int> color = getBaricentricCoordinatesColor(triangle, j, i);
			vector<int> pixelInfo = { j + offset, i, color[0], color[1], color[2] };
			DrawPixel(memDC, pixelInfo);
		}

	}

}

void drawTriangle_Gourad(HDC memDC, vector<vector<int>> triangle, int offset) {
	SelectObject(memDC, GetStockObject(DC_BRUSH));

	int leftX = min(min(triangle[0][0], triangle[1][0]), triangle[2][0]) / GRID_SIZE;
	int rightX = max(max(triangle[0][0], triangle[1][0]), triangle[2][0]) / GRID_SIZE;

	int upY = min(min(triangle[0][1], triangle[1][1]), triangle[2][1]) / GRID_SIZE;
	int downY = max(max(triangle[0][1], triangle[1][1]), triangle[2][1]) / GRID_SIZE;

	map<pair<int, int>, vector<int>> colors;

	set<pair<int, int>> AB = Bresenham(triangle[0][0], triangle[0][1], triangle[1][0], triangle[1][1]);
	set<pair<int, int>> BC = Bresenham(triangle[1][0], triangle[1][1], triangle[2][0], triangle[2][1]);
	set<pair<int, int>> AC = Bresenham(triangle[0][0], triangle[0][1], triangle[2][0], triangle[2][1]);

	vector<set<pair<int, int>>> lines = { AB, BC, AC };


	int counter = 0;
	for (auto line : lines) {
		int gradient = 255 / (int)line.size();
		int R, G, B;

		if (counter == 0) {
			R = 255, G = 0, B = 0;
		}
		else if (counter == 1) {
			R = 0, G = 255, B = 0;
		}
		else {
			R = 255, G = 0, B = 0;
		}

		for (auto i : line) {
			vector<int> color = { R, G, B };
			vector<int> pixelInfo = { i.first, i.second, color[0], color[1], color[2] };
			DrawPixel(memDC, pixelInfo);

			colors.insert({ { i.first, i.second }, color });

			if (counter == 0) {
				R -= gradient;
				G += gradient;
			}
			else if (counter == 1) {
				B += gradient;
				G -= gradient;
			}
			else {
				R -= gradient;
				B += gradient;
			}
		}
		counter++;
	}

	for (int i = upY; i <= downY; i++) {
		bool painted = false;
		set<int> hor;
		for (int j = leftX; j <= rightX; j++) {
			if (colors.count({ j, i }) && painted) {
				break;
			}

			if (colors.count({ j - 1, i }) && !colors.count({ j, i })) {
				painted = true;
			}

			if (painted) {
				hor.insert(j);
			}

			if (j == rightX) {
				hor.clear();
			}
		}
		
		if ((int)hor.size()) {
			int left = *hor.begin() - 1, right = *hor.rbegin() + 1;
			vector<int> p = colors[{ left, i }],
						q = colors[{ right, i }];

			int R = (q[0] - p[0]) / (int)hor.size();
			int G = (q[1] - p[1]) / (int)hor.size();
			int B = (q[2] - p[2]) / (int)hor.size();

			for (auto j : hor) {
				vector<int> pixelInfo = { j, i, p[0], p[1], p[2] };
				DrawPixel(memDC, pixelInfo);

				p[0] += R;
				p[1] += G;
				p[2] += B;
			}
		}
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

	vector<vector<int>> triangle = {
		{ 110, 200, 0, 255, 0 },
		{ 110, 250, 255, 0, 0 },
		{ 280, 300, 0, 0, 255 }
	};

	drawTriangle_BaricentricCoordinates(memDC, triangle, 100);
	drawTriangle_Gourad(memDC, triangle, 0);


	//unsigned char* data = readBMP("snail.bmp");

}