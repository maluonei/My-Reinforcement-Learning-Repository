#pragma once
#ifndef ENFORCEMENTLEARNING_H
#define ENFORCEMENTLEARNING_H
#include "policy.h"
#include<algorithm>

using namespace std;

extern int epoch;//�趨ѵ������ sarsa(lambda)�㷨�����ִθ���
extern int maxTry;//��maxTry��û�н���һ�������¿�ʼ
//extern int randomSeed;//���������
//extern int startx; //��ʼ��x��y
//extern int starty;
//extern int endx;   //�յ�x��y
//extern int endy;
extern int screenFlashTimeSlice; //������ӻ���ÿһ������ʱ�䣨���룩
extern double chance;          //1-chanceΪѡ��������Եĸ���
extern double alpha;           //sasra�㷨�еĵ�������
extern double gamma;           //˥��ֵ
extern double lambda;          //sarsa(lambda)�е�lambda
//extern double obstacleChance; //��ͼ���ϰ����ɸ���
extern bool isAddRestriction;//�Ƿ������ϰ����ͣ��������ϴ��һ��
extern bool isVisuable;       //�Ƿ���ӻ�ѵ�����̣��������

int R[gridX][gridY][4];     //rewardֵ���� 
double Q[gridX][gridY][4];   //ѧϰ����֪ʶ,Q����
double E[gridX][gridY][4];

struct Point {
	int x;
	int y;
	int direction;
	Point(int _x, int _y, int _direction) :x(_x), y(_y), direction(_direction) {}
};

void initQ() {
	fill(Q[0][0], Q[0][0] + gridX * gridY * 4, 0);
}

void initE() {
	fill(E[0][0], E[0][0] + gridX * gridY * 4, 0);
}


void initR() {
	fill(R[0][0], R[0][0] + gridX * gridY * 4, -1);
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (i != 0 && i - 1 != 0 && M[i - 1][j] != -1) R[i][j][0] = 0;
			if (j != 0 && j - 1 != 0 && M[i][j - 1] != -1) R[i][j][1] = 0;
			if (i != gridX - 1 && M[i + 1][j] != -1) R[i][j][2] = 0;
			if (j != gridY - 1 && M[i][j + 1] != -1) R[i][j][3] = 0;

			if (i - 1 != 0 && M[i - 1][j] == 1) {
				R[i][j][0] = 1;
			}
			if (i + 1 != gridX && M[i + 1][j] == 1) {
				R[i][j][2] = 1;
			}
			if (j - 1 != 0 && M[i][j - 1] == 1) {
				R[i][j][1] = 1;
			}
			if (j + 1 != gridY && M[i][j + 1] == 1) {
				R[i][j][3] = 1;
			}
		}
	}
}

void chooseRandomAction(int x, int y, int& direction) {  //�������
	//cout << "Random\n";
	vector<int> nexts;
	for (int j = 0; j < 4; j++) {
		if (R[x][y][j] != -1) {
			if (!isAddRestriction) {
				nexts.push_back(j);
			}
			else {
				if (previous[x + nextStep[j][0]][y + nextStep[j][1]] == 0) {
					nexts.push_back(j);
				}
			}
		}
	}
	int randomNum = 0;
	if (nexts.size() == 0) {
		direction = -1;
		return;
	}

	while (true) {
		randomNum = getRandomNumber(nexts.size() - 1);
		direction = nexts[randomNum];
		return;
	}
}

void chooseMaxAction(int x, int y, int& direction) {   //���ѡ�����
	//cout << "Max\n";
	int _direction = -1;
	double maxQ = 0;
	for (int i = 0; i < 4; i++) {
		//if (R[x][y][i] != -1 && previous[x + nextStep[i][0]][y + nextStep[i][1]] == 0) {
		if (R[x][y][i] != -1) {
			if (!isAddRestriction) {
				if (Q[x][y][i] > maxQ) {
					maxQ = Q[x][y][i];
					_direction = i;
				}
			}
			else {
				if (Q[x][y][i] > maxQ && previous[x + nextStep[i][0]][y + nextStep[i][1]] == 0) {
					maxQ = Q[x][y][i];
					_direction = i;
				}
			}
		}
	}
	if (_direction == -1) {
		chooseRandomAction(x, y, direction);
	}
	else {
		direction = _direction;
	}
}

void chooseNextAction(int x, int y, int& direction, double chance) {  //���ݸ������ѡ���������ֲ��Ե�һ��
	if (isSmaller(chance)) {
		chooseMaxAction(x, y, direction);
	}
	else {
		chooseRandomAction(x, y, direction);
	}
}

double getmaxQ(int x, int y) {           //��õ�ǰ��x,y��״̬�µ����Qֵ
	double maxnum = 0;
	for (int i = 0; i < 4; i++) {
		if (R[x][y][i] != -1 && Q[x][y][i] > maxnum) maxnum = Q[x][y][i];
	}
	return maxnum;
}

void _printPath() {  //��ӡ����·��
	int x = startx;
	int y = starty;
	int direction = 0;
	double maxQ = 0;
	fill(path[0], path[0] + gridX * gridY, 0);
	while (true) {
		//cout << x << " " << y << endl;
		path[x][y] = 2;
		if (M[x][y] == 1) break;

		direction = 0;
		maxQ = 0;
		for (int i = 0; i < 4; i++) {
			if (Q[x][y][i] >= maxQ && x + nextStep[i][0] >= 0 && x + nextStep[i][0] < gridX && y + nextStep[i][1] >= 0 && y + nextStep[i][1] < gridY && path[x + nextStep[i][0]][y + nextStep[i][1]] != 2) {
				direction = i;
				maxQ = Q[x][y][i];
			}
		}
		if (maxQ == 0) {
			cout << "can`t reach destination from (" << startx << "," << starty << ")" << endl;
			return;
		}
		x += nextStep[direction][0];
		y += nextStep[direction][1];
	}
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (path[i][j] != 2 || M[i][j] == 1) path[i][j] = M[i][j];

			if (i == startx && j == starty) {
				cout << "S";
			}
			else if (path[i][j] == -1) {
				cout << "#";
			}
			else if (path[i][j] == 0) {
				cout << " ";
			}
			else if (path[i][j] == 1) {
				cout << "D";
			}
			else if (path[i][j] == 2) {
				cout << "*";
			}
		}
		cout << endl;
	}
}

void updateQ_QLearning(int x, int y, int direction, double maxQ) {
	Q[x][y][direction] = double(R[x][y][direction]) + gamma * maxQ;
}

void updateQ_Sasra(int state_x, int state_y, int direction, int next_state_x, int next_state_y, int next_direction) {
	Q[state_x][state_y][direction] += alpha * (R[state_x][state_y][direction] + gamma * Q[next_state_x][next_state_y][next_direction] - Q[state_x][state_y][direction]);
}

void updateQandE_SarsaLambda(double delta) {
	for (int j = 0; j < gridX; j++) {
		for (int k = 0; k < gridY; k++) {
			for (int l = 0; l < 4; l++) {
				if (R[j][k][l] != -1) {
					Q[j][k][l] += alpha * delta * E[j][k][l];
					E[j][k][l] = gamma * lambda * E[j][k][l];
				}
			}
		}
	}
}

void printMap(int epoch, int state_x, int state_y) {
	//system("cls");
	gotoxy(hOut, 0, 0);
	cout << "training epoch:" << epoch << endl;
	for (int i = 0; i < gridX; i++) {
		//cout << "\r";
		for (int j = 0; j < gridY; j++) {
			if (i == state_x && j == state_y) {
				cout << "*";
			}
			else {
				if (i == startx && j == starty) cout << "S";
				else if (M[i][j] == 0) cout << " ";
				else if (M[i][j] == -1) cout << "#";
				else if (M[i][j] == 1) cout << "D";
			}
		}
		cout << endl;
	}
}

void Delay(int time)
{
	clock_t  now = clock();
	while (clock() - now < time);
}

void train_QLearning() { //ѵ��epoch�ִ�, ˵�����������ۿ���һ�������������ȴ��һ��ִ���Ǹ�����
	int successNum = 0;
	for (int i = 0; i < epoch; i++) {
		if (!isVisuable) {
			cerr << "\repoch:" << i << std::flush;
		}
		if (isAddRestriction) initPrevious();
		int state_x = startx, state_y = starty;
		int next_state_x = -1, next_state_y = -1;
		int direction = -1;
		int try_time = 0;
		double maxQ = 0.0;
		while (true) {
			if (isVisuable) {
				printMap(i, state_x, state_y);
				Sleep(screenFlashTimeSlice);
			}
			if (isAddRestriction) previous[state_x][state_y] = 1;
			//cout << start_x << " " << start_y << endl;
			if (try_time > maxTry) break;
			try_time++;
			chooseNextAction(state_x, state_y, direction, chance);
			if (direction == -1) {
				break;
			}

			next_state_x = state_x + nextStep[direction][0];
			next_state_y = state_y + nextStep[direction][1];

			maxQ = getmaxQ(next_state_x, next_state_y);
			updateQ_QLearning(state_x, state_y, direction, maxQ);

			state_x = next_state_x;
			state_y = next_state_y;

			if (M[state_x][state_y] == 1) {
				successNum++;
				break;
			}
		}
	}
	cout << "successNum:" << successNum << endl;
}

void train_Sarsa() {//˵���������ۿ���һ�������������Ҳһ��ִ���Ǹ���������qleaen�ĸ��·��̲�ͬ
	int successNum = 0;
	for (int i = 0; i < epoch; i++) {
		if (!isVisuable) {
			cerr << "\repoch:" << i << std::flush;
		}
		if (isAddRestriction) initPrevious();
		int state_x = startx, state_y = starty;
		int next_state_x = -1, next_state_y = -1;
		int direction = -1;
		int next_direction = -1;
		int try_time = 0;
		double maxQ = 0.0;
		while (true) {
			if (isVisuable) {
				printMap(i, state_x, state_y);
				Sleep(screenFlashTimeSlice);
			}
			if (isAddRestriction) previous[state_x][state_y] = 1;
			//cout << start_x << " " << start_y << endl;
			if (try_time > maxTry) break;
			try_time++;
			chooseNextAction(state_x, state_y, direction, chance);
			if (direction == -1) {
				break;
			}
			next_state_x = state_x + nextStep[direction][0];
			next_state_y = state_y + nextStep[direction][1];

			chooseNextAction(next_state_x, next_state_y, next_direction, chance);
			if (next_direction == -1) {
				break;
			}

			updateQ_Sasra(state_x, state_y, direction, next_state_x, next_state_y, next_direction);

			//maxQ = getmaxQ(action_x, action_y);
			//updateQ(start_x, start_y, direction, maxQ);

			state_x = next_state_x;
			state_y = next_state_y;

			if (M[state_x][state_y] == 1) {// || M[state_x][state_y] == -1) {
				successNum++;
				break;
			}
		}
	}
	cout << "successNum:" << successNum << endl;
}

void train_SarsaLambda() {//��sarsa�ܵ�˼�����ƣ�����sarsaֻ�е��ߵ��н�����һ��ʱ���Ż����ǰһ������sarsa��lambda�����ߵ��н�����һ��ʱ�������֮ǰ�ߵ�ÿһ��
	int successNum = 0;
	for (int i = 0; i < epoch; i++) {
		initE();
		if (!isVisuable) {
			cerr << "\repoch:" << i << std::flush;
		}
		if (isAddRestriction) initPrevious();
		int state_x = startx, state_y = starty;
		int next_state_x = -1, next_state_y = -1;
		int direction = -1;
		int next_direction = -1;
		int try_time = 0;
		double maxQ = 0.0;
		double delta = 0;

		while (true) {
			if (isVisuable) {
				printMap(i, state_x, state_y);
				Sleep(screenFlashTimeSlice);
			}
			if (isAddRestriction) previous[state_x][state_y] = 1;
			//cout << start_x << " " << start_y << endl;
			if (try_time > maxTry) break;
			try_time++;
			chooseNextAction(state_x, state_y, direction, chance);
			if (direction == -1) {
				break;
			}
			next_state_x = state_x + nextStep[direction][0];
			next_state_y = state_y + nextStep[direction][1];
			chooseNextAction(next_state_x, next_state_y, next_direction, chance);
			if (next_direction == -1) {
				break;
			}

			delta = R[state_x][state_y][direction] + gamma * Q[next_state_x][next_state_y][next_direction] - Q[state_x][state_y][direction];
			E[state_x][state_y][direction] += 1;

			updateQandE_SarsaLambda(delta);
			//updateQ_Sasra(state_x, state_y, direction, next_state_x, next_state_y, next_direction);
			//maxQ = getmaxQ(action_x, action_y);
			//updateQ(start_x, start_y, direction, maxQ);

			state_x = next_state_x;
			state_y = next_state_y;

			if (M[state_x][state_y] == 1) {// || M[state_x][state_y] == -1) {
				break;
			}
		}

		//for (int j = 0; j < gridX; j++) {
		//	for (int k = 0; k < gridY; k++) {
		//		for (int l = 0; l < 4; l++) {
		//			cout << E[j][k][l] << " ";
		//		}
		//		cout << "|";
		//	}
		//	cout << endl;
		//}
	}
}

void printQ() {
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			for (int k = 0; k < 4; k++) {
				cout << setw(8) << Q[i][j][k] << " ";
			}
			cout << "|";
		}
		cout << endl;
	}
}

class QLearningPolicy :public  policy {
public:
	QLearningPolicy() {}
	void train() {
		initR();
		initQ();
		train_QLearning();
	}
	void printPath() {
		_printPath();
	}
};

class SarsaPolicy :public policy {
public:
	SarsaPolicy() {}
	void train() {
		initR();
		initQ();
		train_Sarsa();
	}
	void printPath() {
		_printPath();
	}
};

class SarsaLambdaPolicy :public policy {
public:
	SarsaLambdaPolicy() {}
	void train() {
		initR();
		initQ();
		train_SarsaLambda();
	}
	void printPath() {
		_printPath();
	}
};


#endif
