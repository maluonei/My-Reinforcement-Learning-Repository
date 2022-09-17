#include <iostream>
#include <random>
#include <iomanip>
#include <Windows.h>
#include <queue>
#include<unordered_map>

using namespace std;

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//定义显示器句柄变量,并且这个只能在每个头文件中单独定义句柄和函数，否则无效
void gotoxy(HANDLE hOut, int x, int y)        //其中x，y是与正常理解相反的，注意区分
{
	COORD pos;
	pos.X = x;             //横坐标
	pos.Y = y;             //纵坐标
	SetConsoleCursorPosition(hOut, pos);
}

const int gridsize = 4;
int maxRandomNum = 1000000;   //随机数设置
int randomSeed = 1000;//随机数种子

int nextStep[4][2] = {
	-1, 0,
	0, -1,
	1, 0,
	0, 1
};

class state {
public:
	int s[gridsize][gridsize];
	state* previous = NULL;
	state* self = NULL;
	state* next = NULL;
	void calSelf() {
		self = this;
	}
	state() {}
	state(int _s[gridsize][gridsize]) {
		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				s[i][j] = _s[i][j];
			}
		}
	}
	bool operator ==(const state& _state) const {
		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				if (s[i][j] != _state.s[i][j]) return false;
			}
		}
		return true;
	}

	bool operator !=(const state& _state) const {
		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				if (s[i][j] != _state.s[i][j]) return true;
			}
		}
		return false;
	}

	bool operator < (const state& _state) const;

	state* exchange(int x, int y, int _x, int _y) {
		state* newState = new state();
		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				newState->s[i][j] = s[i][j];
			}
		}
		newState->s[x][y] = s[_x][_y];
		newState->s[_x][_y] = s[x][y];
		return newState;
	}
};

struct hash_state {  //unordered_map计算哈希值
	size_t operator()(const state& _state) const {
		int h = 0;
		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				h += (i * gridsize + j) * hash<int>()(_state.s[i][j]);
			}
		}
		return h;
	}
};

int final[gridsize][gridsize];

void initFinal() {
	int number = 0;
	for (int i = 0; i < gridsize; i++) {
		for (int j = 0; j < gridsize; j++) {
			final[i][j] = number;
			number++;
		}
	}
}

state finalState(final);

int calDistance(int number, int x, int y) {
	int finalx = number / gridsize;
	int finaly = number % gridsize;
	return abs(finalx - x) + abs(finaly - y);
}

ostream& operator<<(ostream& cout, state& _state)
{
	for (int i = 0; i < gridsize; i++) {
		for (int j = 0; j < gridsize; j++) {
			cout << setw(3) << _state.s[i][j] << " ";
		}
		cout << endl;
	}
	return cout;
}

int getRandomChance() {    //生成[0，maxRandomNum-1]随机数
	static uniform_int_distribution<unsigned >u1(0, maxRandomNum - 1);
	static default_random_engine e1;
	return u1(e1);
}

int getRandomNumber(int number) {    //生成[0:number-1]随机数
	return getRandomChance() * (number + 1) / maxRandomNum;
}

void initRandomGenerator() {       //初始化随机数生成器
	for (int i = 0; i < randomSeed; i++) {
		getRandomNumber(0);
	}
}

int h(state current) { //启发式函数，所有棋子到目标位置的距离和
	int _h = 0;
	for (int i = 0; i < gridsize; i++) {
		for (int j = 0; j < gridsize; j++) {
			_h += calDistance(current.s[i][j], i, j);
		}
	}
	return _h;
}

bool state::operator < (const state& _state) const {
	return h(*this) > h(_state);
}

state* generateRandomState() {
	state* origin = new state(final);
	int zero_x, zero_y, next_x, next_y;
	for (int i = 0; i < randomSeed; i++) {
		for (int j = 0; j < gridsize; j++) {
			for (int k = 0; k < gridsize; k++) {
				if (origin->s[j][k] == 0) {
					zero_x = j;
					zero_y = k;
					goto flag2;
				}
			}
		}
	flag2:
		for (int j = 0; j < 4; j++) {
			next_x = zero_x + nextStep[j][0];
			next_y = zero_y + nextStep[j][1];
			if (next_x >= 0 && next_x < gridsize && next_y >= 0 && next_y < gridsize) {
				if (getRandomChance() < maxRandomNum / 2) {
					origin = origin->exchange(zero_x, zero_y, next_x, next_y);
					break;
				}
			}
		}
	}
	cout << "origin:\n" << *origin;
	return origin;
}

void train(state* _state) {
	int adjustNum = 0;
	unordered_map<state, int, hash_state> mp;
	priority_queue<state> pqueue;
	state c;
	state* current = _state;
	current->calSelf();
	state* nextstate;
	pqueue.push(*current);
	int zero_x = 0, zero_y = 0;
	int next_x, next_y;
	int index = 1;
	mp[*current] = index;

	while (!pqueue.empty()) {
		c = pqueue.top();
		current = c.self;
		pqueue.pop();
		index = mp[*current];

		if (*current == final) {
			break;
		}

		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				if (current->s[i][j] == 0) {
					zero_x = i;
					zero_y = j;
					goto flag;
				}
			}
		}
	flag:
		for (int i = 0; i < 4; i++) {
			next_x = zero_x + nextStep[i][0];
			next_y = zero_y + nextStep[i][1];
			if (next_x >= 0 && next_x < gridsize && next_y >= 0 && next_y < gridsize) {
				nextstate = current->exchange(zero_x, zero_y, next_x, next_y);
				if (mp.count(*nextstate) == 0 || (mp.count(*nextstate) == 1 && mp[*nextstate] > index + 1)) {
					mp[*nextstate] = index + 1;
					nextstate->calSelf();
					nextstate->previous = current;
					pqueue.push(*nextstate);
				}
			}
		}
	}

	cout << "move times:" << index << endl;
	while (*current != *_state) {
		current->previous->next = current;
		current = current->previous;
	}

	while (*current != final) {
		cout << *current << endl;
		current = current->next;
	}
	cout << current << endl;
}

int main()
{
	initFinal();
	initRandomGenerator();
	state* start = generateRandomState();
	train(start);
}
