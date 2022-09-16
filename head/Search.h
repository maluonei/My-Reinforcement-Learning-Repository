#pragma once
#ifndef SEARCH_H
#define SEARCH_H
#include "policy.h"
#include <queue>
using namespace std;

int f[gridX][gridY];
int g[gridX][gridY];
int pre[gridX][gridY];

struct point {
	int x;
	int y;
	int f;
	point(int _x, int _y, int _f) :x(_x), y(_y), f(_f) {}
	bool operator <(const point& p) const {
		return f > p.f;
	}
};

int h(int x, int y) {
	return abs(endx - x) + abs(endy - y);
}

void initf() {
	fill(f[0], f[0] + gridX * gridY, 0);
}

void initg() {
	fill(g[0], g[0] + gridX * gridY, -1);
	g[startx][starty] = 0;
}

void initPrev() {
	fill(pre[0], pre[0] + gridX * gridY, -1);
}


void trainAstar() {
	initf();
	initg();
	initPrev();

	priority_queue<point> pqueue;
	f[startx][starty] = h(startx, starty);
	pre[startx][starty] = -1;
	point current(startx, starty, f[startx][starty]);
	pqueue.push(current);

	while (!pqueue.empty()) {
		current = pqueue.top();
		pqueue.pop();
		//cout << current.x << " " << current.y << endl;

		if (current.x == endx && current.y == endy) break;
		if (current.f != g[current.x][current.y] + h(current.x, current.y)) {
			continue;
		}

		//previous[current.x][current.y] = 1;
		for (int i = 0; i < 4; i++) {
			int nextX = current.x + nextStep[i][0];
			int nextY = current.y + nextStep[i][1];
			if (nextX >= 0 && nextX < gridX && nextY >= 0 && nextY < gridY && M[nextX][nextY] != -1) {
				if (g[nextX][nextY] == -1 || g[nextX][nextY] > g[current.x][current.y] + 1) {
					g[nextX][nextY] = g[current.x][current.y] + 1;
					f[nextX][nextY] = g[nextX][nextY] + h(nextX, nextY);
					pre[nextX][nextY] = i;
					pqueue.push(point(nextX, nextY, f[nextX][nextY]));
				}
			}
		}
	}
	//
	//for (int i = 0; i < gridX; i++) {
	//	for (int j = 0; j < gridY; j++) {
	//		if (i == endx && j == endy) cout << setw(3) << "D";
	//		else if (i == startx && j == starty) cout << setw(3) << "S";
	//		else
	//			cout << setw(3) << pre[i][j];
	//	}
	//	cout << endl;
	//}
}

void printPathAstar() {
	int x = endx, y = endy;
	initPath();
	if (pre[endx][endy] == -1) {
		cout << "can`t find the path\n";
		return;
	}
	while (true) {
		//cout << x << " " << y << endl;
		if (x == startx && y == starty) break;
		path[x][y] = 1;

		int direction = (pre[x][y] + 2) % 4;

		if (direction == -1) {
			cout << "can`t find the path\n";
			return;
		}
		//cout << "direction:" << direction << endl;
		x = x + nextStep[direction][0];
		y = y + nextStep[direction][1];
	}

	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (i == startx && j == starty) {
				cout << "S";
			}
			else if (i == endx && j == endy) {
				cout << "D";
			}
			else if (path[i][j] == 1) {
				cout << "*";
			}
			else if (M[i][j] == -1) {
				cout << "#";
			}
			else cout << " ";
		}
		cout << endl;
	}
}

class AstarPolicy :public policy {
public:
	AstarPolicy() {}
	void train() {
		trainAstar();
	}
	void printPath() {
		printPathAstar();
	}
};

#endif
