#include<iostream>
#include<algorithm>
#include<random>
#include<iomanip>

using namespace std;

const int gridX = 10;
const int gridY = 10;
const int finalx = 5;
const int finaly = 5;

int epoch = 20;//设定训练次数
int maxTry = 1000;//若maxTry次没有结束一轮则重新开始
int randomSeed = 100;//随机数种子
int startx = 2; //起始点x与y
int starty = 2;
int endx = 8;   //终点x与y
int endy = 9;
int maxRandomNum = 1000000;  //随机数设置
double chance = 0.9;         //1-chance为选择随机策略的概率
double lamda = 0.8;          //衰减值
double obstacleChance = 0.3; //地图上障碍生成概率

int M[gridX][gridY];         //Map: -1为障碍， 0为可以走, 1为终点
int path[gridX][gridY];      //path矩阵，用来绘制最终路径
long R[gridX][gridY][4];     //reward值矩阵 
double Q[gridX][gridY][4];   //学习到的知识,Q矩阵
//int previous[gridX][gridY];
int nextStep[4][2] = { -1,  0,
						0, -1,
						1,  0,
						0,  1 };  //可以行动的四个方向
//left--0--i-1
//down--1--j-1
//right--2--i+1
//up--3--j+1

struct Point {
	int x;
	int y;
	int direction;
	Point(int _x, int _y, int _direction) :x(_x), y(_y), direction(_direction) {}
};


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

void initM() {
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (isSmaller(0.3)) {   //百分之30的地图格子是障碍
				M[i][j] = -1;
			}
			else {
				M[i][j] = 0;
			}
		}
	}
	M[endx][endy] = 1;
}

void initQ() {
	fill(Q[0][0], Q[0][0] + gridX * gridY * 4, 0);
}

void printM() {
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			cout << setw(3) << M[i][j] << " ";
		}
		cout << endl;
	}
}

void initR() {
	fill(R[0][0], R[0][0] + gridX * gridY * 4, -1);
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (i != 0 && M[i - 1][j] != -1) R[i][j][0] = 0;
			if (j != 0 && M[i][j - 1] != -1) R[i][j][1] = 0;
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

void chooseRandomAction(int& x, int& y, int& _x, int& _y, int& direction) {  //随机策略
	//cout << "Random\n";
	vector<Point> nexts;
	for (int j = 0; j < 4; j++) {
		if (R[x][y][j] != -1) {
			nexts.push_back(Point(x + nextStep[j][0], y + nextStep[j][1], j));
		}
	}
	int randomNum = 0;
	if (nexts.size() == 0) {
		_x = -1;
		_y = -1;
		direction = -1;
		return;
	}

	while (true) {
		randomNum = getRandomNumber(nexts.size() - 1);
		_x = nexts[randomNum].x;
		_y = nexts[randomNum].y;
		direction = nexts[randomNum].direction;
		return;
	}
}

void chooseMaxAction(int& x, int& y, int& _x, int& _y, int& direction) {   //最大化选择策略
	//cout << "Max\n";
	int _direction = -1;
	double maxS = 0;
	for (int i = 0; i < 4; i++) {
		//if (R[x][y][i] != -1 && previous[x + nextStep[i][0]][y + nextStep[i][1]] == 0) {
		if (R[x][y][i] != -1) {
			if (Q[x][y][i] > maxS) {
				maxS = Q[x][y][i];
				_direction = i;
			}
		}
	}
	if (_direction == -1) {
		chooseRandomAction(x, y, _x, _y, direction);
	}
	else {
		direction = _direction;
		_x = x + nextStep[direction][0];
		_y = y + nextStep[direction][1];
	}
}

void chooseNextAction(int& x, int& y, int& _x, int& _y, int& direction, double chance) {  //根据概率随机选择上列两种策略的一种
	if (isSmaller(chance)) {
		chooseMaxAction(x, y, _x, _y, direction);
	}
	else {
		chooseRandomAction(x, y, _x, _y, direction);
	}
}

double getMaxS(int x, int y) {           //获得当前（x,y）状态下的最大Q值
	double maxnum = 0;
	for (int i = 0; i < 4; i++) {
		if (R[x][y][i] != -1 && Q[x][y][i] > maxnum) maxnum = Q[x][y][i];
	}
	return maxnum;
}

void getPath() {  //打印最终路径
	int x = startx;
	int y = starty;
	int direction = 0;
	double maxS = 0;
	fill(path[0], path[0] + gridX * gridY, 0);
	while (true) {
		path[x][y] = 2;
		if (M[x][y] == 1) break;

		direction = 0;
		maxS = 0;
		for (int i = 0; i < 4; i++) {
			if (Q[x][y][i] > maxS) {
				direction = i;
				maxS = Q[x][y][i];
			}
		}
		if (maxS == 0) {
			cout << "can`t reach destination from (" << startx << "," << starty << ")" << endl;
			return;
		}
		x += nextStep[direction][0];
		y += nextStep[direction][1];
	}
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (path[i][j] != 2 || M[i][j] == 1) path[i][j] = M[i][j];
			cout << setw(3) << path[i][j] << " ";
		}
		cout << endl;
	}
}

void updateQ(int x, int y, int direction, double maxS) {
	Q[x][y][direction] = double(R[x][y][direction]) + lamda * maxS;
}

void train2(int epoch) { //训练epoch轮次
	int successNum = 0;
	for (int i = 0; i < epoch; i++) {
		cerr << "epoch:" << i << endl << std::flush;
		int start_x = startx, start_y = starty;
		int action_x = -1, action_y = -1;
		int direction = -1;
		int try_time = 0;
		double maxS = 0.0;
		while (true) {
			//cout << start_x << " " << start_y << endl;
			if (try_time > maxTry) break;
			try_time++;
			chooseNextAction(start_x, start_y, action_x, action_y, direction, chance);
			if (direction == -1) {
				break;
			}

			maxS = getMaxS(action_x, action_y);
			updateQ(start_x, start_y, direction, maxS);

			start_x = action_x;
			start_y = action_y;

			if (M[start_x][start_y] == 1 || M[start_x][start_y] == -1) {
				break;
			}
		}
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

int main() {
	initRandomGenerator(); //初始化随机数生成器
	initM(); //初始化地图
	initR(); //初始化reward矩阵
	initQ(); //初始化Q矩阵

	train2(epoch);//训练
	cout << "***************************************\n";

	printM();//打印地图
	cout << "***************************************\n";

	getPath();//设置起点,绘画路径
	cout << "***************************************\n";
	return 0;
}



/*
void train(int epoch, int type) {
	if (type == 0) {
		int successNum = 0;
		for (int i = 0; i < epoch; i++) {
			cerr << "epoch:" << i << endl << std::flush;
			//fill(previous[0], previous[0] + gridX * gridY, 0);
			int start_x = -1, start_y = -1;
			getRandomNumber(start_x, start_y);
			int action_x = -1, action_y = -1;
			int direction = -1;
			int try_time = 0;
			while (true) {
				if (try_time > 300) break;
				try_time++;
				//cout << start_x << " " << start_y << endl << flush;
				chooseNextAction(start_x, start_y, action_x, action_y, direction, chance);
				//cout << direction << endl;
				if (direction == -1) {
					break;
				}

				//if (M[action_x][action_y] == 1 || M[action_x][action_y] == -1) {
				//	break;
				//}

				//previous[start_x][start_y] = 1;
				double maxS = getMaxS(action_x, action_y);
				Q[start_x][start_y][direction] = double(R[start_x][start_y][direction]) + lamda * maxS;

				start_x = action_x;
				start_y = action_y;

				if (M[start_x][start_y] == 1 || M[start_x][start_y] == -1) {
					break;
				}
			}
		}
	}
	else if (type == 1) {
		int successNum = 0;
		int index = 0;
		int testnum = 0;
		int i = 0;
		while (index < gridX * gridY) {
			cerr << "epoch:" << i << endl << std::flush;
			i++;
			//fill(previous[0], previous[0] + gridX * gridY, 0);
			int start_x = -1, start_y = -1;
			while (index < gridX * gridY) {
				start_x = index / gridX;
				start_y = index % gridX;
				if (M[start_x][start_y] != -1) {
					if (testnum < 3) testnum++;
					else {
						testnum = 0;
						index++;
					}
					break;
				}
				else {
					index++;
					break;
				}
			}
			int action_x = -1, action_y = -1;
			int direction = -1;
			while (true) {
				//cout << start_x << " " << start_y << endl << flush;
				chooseNextAction(start_x, start_y, action_x, action_y, direction, chance);
				//cout << direction << endl;
				if (direction == -1) {
					break;
				}
				//previous[start_x][start_y] = 1;
				double maxS = getMaxS(action_x, action_y);
				Q[start_x][start_y][direction] = double(R[start_x][start_y][direction]) + lamda * maxS;

				start_x = action_x;
				start_y = action_y;

				if (M[start_x][start_y] == 1) {
					//cout << start_x << " " << start_y << endl;
					successNum++;
					break;
				}
			}
		}
	}
}
*/