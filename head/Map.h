#pragma once
#ifndef MAP_H
#define MAP_H
#include <iostream>
#include<Windows.h>
#include<ctime>
#include<random>
#include<iomanip>
using namespace std;

const int gridX = 50;
const int gridY = 50;

int startx = 6; //起始点x与y
int starty = 3;
int endx = 48;   //终点x与y
int endy = 38;
double obstacleChance = 0.33; //地图上障碍生成概率
int maxRandomNum = 1000000;   //随机数设置
int randomSeed = 500;//随机数种子

int M[gridX][gridY];         //Map: -1为障碍， 0为可以走, 1为终点
int path[gridX][gridY];      //path矩阵，用来绘制最终路径
int previous[gridX][gridY];
int nextStep[4][2] = {
	-1,  0,
	 0, -1,
	 1,  0,
	 0,  1 };  //可以行动的四个方向

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//定义显示器句柄变量,并且这个只能在每个头文件中单独定义句柄和函数，否则无效
void gotoxy(HANDLE hOut, int x, int y)//其中x，y是与正常理解相反的，注意区分
{
	COORD pos;
	pos.X = x;             //横坐标
	pos.Y = y;            //纵坐标
	SetConsoleCursorPosition(hOut, pos);
}


int getRandomChance() {    //生成[0，maxRandomNum-1]随机数
	static uniform_int_distribution<unsigned >u1(0, maxRandomNum - 1);
	static default_random_engine e1;
	return u1(e1);
}

int getRandomNumber(int number) {    //生成[0:number-1]随机数
	return getRandomChance() * (number + 1) / maxRandomNum;
}

bool isSmaller(double chance) {      //进行一次[0,1]随机，随机值小于chance的概率
	return getRandomChance() < maxRandomNum * chance;
}

void initRandomGenerator() {       //初始化随机数生成器
	for (int i = 0; i < randomSeed; i++) {
		getRandomNumber(0);
	}
}

void initPath() {
	fill(path[0], path[0] + gridX * gridY, 0);
}

void initPrevious() {
	fill(previous[0], previous[0] + gridX * gridY, 0);
}
void initM() {
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (isSmaller(obstacleChance)) {   //百分之30的地图格子是障碍
				M[i][j] = -1;
			}
			else {
				M[i][j] = 0;
			}
		}
	}
	M[endx][endy] = 1;
}

void printM() {
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (i == startx && j == starty) {
				cout << "S";
			}
			else if (M[i][j] == -1) {
				cout << "#";
			}
			else if (M[i][j] == 0) {
				cout << " ";
			}
			else if (M[i][j] == 1) {
				cout << "D";
			}
		}
		cout << endl;
	}
}


#endif
