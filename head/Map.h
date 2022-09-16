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

int startx = 6; //��ʼ��x��y
int starty = 3;
int endx = 48;   //�յ�x��y
int endy = 38;
double obstacleChance = 0.33; //��ͼ���ϰ����ɸ���
int maxRandomNum = 1000000;   //���������
int randomSeed = 500;//���������

int M[gridX][gridY];         //Map: -1Ϊ�ϰ��� 0Ϊ������, 1Ϊ�յ�
int path[gridX][gridY];      //path����������������·��
int previous[gridX][gridY];
int nextStep[4][2] = {
	-1,  0,
	 0, -1,
	 1,  0,
	 0,  1 };  //�����ж����ĸ�����

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//������ʾ���������,�������ֻ����ÿ��ͷ�ļ��е����������ͺ�����������Ч
void gotoxy(HANDLE hOut, int x, int y)//����x��y������������෴�ģ�ע������
{
	COORD pos;
	pos.X = x;             //������
	pos.Y = y;            //������
	SetConsoleCursorPosition(hOut, pos);
}


int getRandomChance() {    //����[0��maxRandomNum-1]�����
	static uniform_int_distribution<unsigned >u1(0, maxRandomNum - 1);
	static default_random_engine e1;
	return u1(e1);
}

int getRandomNumber(int number) {    //����[0:number-1]�����
	return getRandomChance() * (number + 1) / maxRandomNum;
}

bool isSmaller(double chance) {      //����һ��[0,1]��������ֵС��chance�ĸ���
	return getRandomChance() < maxRandomNum * chance;
}

void initRandomGenerator() {       //��ʼ�������������
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
			if (isSmaller(obstacleChance)) {   //�ٷ�֮30�ĵ�ͼ�������ϰ�
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
