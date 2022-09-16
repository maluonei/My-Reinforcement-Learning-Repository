#pragma once
#ifndef ENFORCEMENTLEARNING_H
#define ENFORCEMENTLEARNING_H
#include "policy.h"
#include<algorithm>

using namespace std;

extern int epoch;//设定训练次数 sarsa(lambda)算法所需轮次更多
extern int maxTry;//若maxTry次没有结束一轮则重新开始
//extern int randomSeed;//随机数种子
//extern int startx; //起始点x与y
//extern int starty;
//extern int endx;   //终点x与y
//extern int endy;
extern int screenFlashTimeSlice; //如果可视化，每一步进行时间（毫秒）
extern double chance;          //1-chance为选择随机策略的概率
extern double alpha;           //sasra算法中的迭代步长
extern double gamma;           //衰减值
extern double lambda;          //sarsa(lambda)中的lambda
//extern double obstacleChance; //地图上障碍生成概率
extern bool isAddRestriction;//是否碰到障碍物就停下来进行洗下一轮
extern bool isVisuable;       //是否可视化训练过程（会很慢）

int R[gridX][gridY][4];     //reward值矩阵 
double Q[gridX][gridY][4];   //学习到的知识,Q矩阵
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

void chooseRandomAction(int x, int y, int& direction) {  //随机策略
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

void chooseMaxAction(int x, int y, int& direction) {   //最大化选择策略
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

void chooseNextAction(int x, int y, int& direction, double chance) {  //根据概率随机选择上列两种策略的一种
	if (isSmaller(chance)) {
		chooseMaxAction(x, y, direction);
	}
	else {
		chooseRandomAction(x, y, direction);
	}
}

double getmaxQ(int x, int y) {           //获得当前（x,y）状态下的最大Q值
	double maxnum = 0;
	for (int i = 0; i < 4; i++) {
		if (R[x][y][i] != -1 && Q[x][y][i] > maxnum) maxnum = Q[x][y][i];
	}
	return maxnum;
}

void _printPath() {  //打印最终路径
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

void train_QLearning() { //训练epoch轮次, 说到不做到，观看下一步的最高期望，却不一定执行那个动作
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

void train_Sarsa() {//说到做到，观看下一步的最高期望，也一定执行那个动作，和qleaen的更新方程不同
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

void train_SarsaLambda() {//和sarsa总的思想类似，不过sarsa只有当走到有奖励的一步时，才会更新前一步，而sarsa（lambda）在走到有奖励的一步时，会更新之前走的每一步
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
