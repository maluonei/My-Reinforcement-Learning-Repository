#include<iostream>
#include<algorithm>
#include<random>
#include<iomanip>

using namespace std;

const int gridX = 10;
const int gridY = 10;
const int finalx = 5;
const int finaly = 5;

int epoch = 20;//�趨ѵ������
int maxTry = 1000;//��maxTry��û�н���һ�������¿�ʼ
int randomSeed = 100;//���������
int startx = 2; //��ʼ��x��y
int starty = 2;
int endx = 8;   //�յ�x��y
int endy = 9;
int maxRandomNum = 1000000;  //���������
double chance = 0.9;         //1-chanceΪѡ��������Եĸ���
double lamda = 0.8;          //˥��ֵ
double obstacleChance = 0.3; //��ͼ���ϰ����ɸ���

int M[gridX][gridY];         //Map: -1Ϊ�ϰ��� 0Ϊ������, 1Ϊ�յ�
int path[gridX][gridY];      //path����������������·��
long R[gridX][gridY][4];     //rewardֵ���� 
double Q[gridX][gridY][4];   //ѧϰ����֪ʶ,Q����
//int previous[gridX][gridY];
int nextStep[4][2] = { -1,  0,
						0, -1,
						1,  0,
						0,  1 };  //�����ж����ĸ�����
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

void initM() {
	for (int i = 0; i < gridX; i++) {
		for (int j = 0; j < gridY; j++) {
			if (isSmaller(0.3)) {   //�ٷ�֮30�ĵ�ͼ�������ϰ�
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

void chooseRandomAction(int& x, int& y, int& _x, int& _y, int& direction) {  //�������
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

void chooseMaxAction(int& x, int& y, int& _x, int& _y, int& direction) {   //���ѡ�����
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

void chooseNextAction(int& x, int& y, int& _x, int& _y, int& direction, double chance) {  //���ݸ������ѡ���������ֲ��Ե�һ��
	if (isSmaller(chance)) {
		chooseMaxAction(x, y, _x, _y, direction);
	}
	else {
		chooseRandomAction(x, y, _x, _y, direction);
	}
}

double getMaxS(int x, int y) {           //��õ�ǰ��x,y��״̬�µ����Qֵ
	double maxnum = 0;
	for (int i = 0; i < 4; i++) {
		if (R[x][y][i] != -1 && Q[x][y][i] > maxnum) maxnum = Q[x][y][i];
	}
	return maxnum;
}

void getPath() {  //��ӡ����·��
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

void train2(int epoch) { //ѵ��epoch�ִ�
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
	initRandomGenerator(); //��ʼ�������������
	initM(); //��ʼ����ͼ
	initR(); //��ʼ��reward����
	initQ(); //��ʼ��Q����

	train2(epoch);//ѵ��
	cout << "***************************************\n";

	printM();//��ӡ��ͼ
	cout << "***************************************\n";

	getPath();//�������,�滭·��
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