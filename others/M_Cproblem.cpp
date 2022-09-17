#include <iostream>
#include <random>
#include <iomanip>
#include <Windows.h>
#include <queue>
#include <unordered_map>

using namespace std;

int maxRandomNum = 1000000;   //随机数设置
int randomSeed = 10000;//随机数种子

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//定义显示器句柄变量,并且这个只能在每个头文件中单独定义句柄和函数，否则无效
void gotoxy(HANDLE hOut, int x, int y)        //其中x，y是与正常理解相反的，注意区分
{
	COORD pos;
	pos.X = x;             //横坐标
	pos.Y = y;             //纵坐标
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

void initRandomGenerator() {       //初始化随机数生成器
	for (int i = 0; i < randomSeed; i++) {
		getRandomNumber(0);
	}
}

const int n = 3;//传教士/野人数目
const int k = 2;//船的载客量

class state {
public:
	state* self = NULL;
	state* previous = NULL;
	state* next = NULL;
	int index = 0;
	void setself() {
		self = this;
	}

	bool boat;  //0:left, 1:right
	int leftM;  //左岸传教士
	int leftC;  //左岸野人
	int rightM; //右岸传教士
	int rightC; //右岸野人
	state() {
		boat = 0;
		leftM = n;
		rightM = 0;
		leftC = n;
		rightC = 0;
	}
	state(bool _boat, int _leftM, int _leftC, int _rightM, int _rightC) :
		boat(_boat), leftM(_leftM), leftC(_leftC), rightM(_rightM), rightC(_rightC) {}
	bool operator<(const state& s) const;
	bool operator==(const state& s) const {
		return this->boat == s.boat &&
			this->leftC == s.leftC &&
			this->leftM == s.leftM &&
			this->rightC == s.rightC &&
			this->rightM == s.rightM;
	}
	bool operator!=(const state& s) const {
		return this->boat != s.boat ||
			this->leftC != s.leftC ||
			this->leftM != s.leftM ||
			this->rightC != s.rightC ||
			this->rightM != s.rightM;
	}
	void setindex(int index) {
		this->index = index;
	}
};

ostream& operator<<(ostream& cout, state& s) {
	cout << "(" << s.leftM << "," << s.leftC << "," << s.rightM << "," << s.rightC << "," << s.boat << ")";
	return cout;
}

state* start = new state(0, n, n, 0, 0);
state* final = new state(1, 0, 0, n, n);

struct hash_state {  //unordered_map计算哈希值
	size_t operator()(const state& _state) const {
		return 1 * hash<bool>()(_state.boat) +
			2 * hash<int>()(_state.leftM) +
			3 * hash<int>()(_state.leftC) +
			4 * hash<int>()(_state.rightM) +
			5 * hash<int>()(_state.rightC);
	}
};

int h(state s) {
	return abs(s.boat - final->boat) + abs(s.rightC - final->rightC) + abs(s.rightM - final->rightM);
}

bool state::operator<(const state& s)const {
	return h(*this) < h(s);
}

bool validOnBoat(int M, int C, int currentM, int currentC) {
	if ((M == 0 && C == 0) || M > currentM || C > currentC || (M != 0 && M < C)) return false;
	return true;
}

void search() {
	state temp;
	state* current = start;
	state* nextState;
	current->setself();
	current->setindex(1);

	int index = 0;
	bool next_boat;
	int next_lM, next_lC, next_rM, next_rC;

	int currentM, currentC;

	priority_queue<state> pqueue;
	pqueue.push(*current);

	unordered_map<state, int, hash_state> hashmap;
	hashmap[*current] = 1;

	while (!pqueue.empty()) {
		temp = pqueue.top();
		current = &temp;
		pqueue.pop();

		if (*current == *final) {
			break;
		}

		if (hashmap[*current] != current->index) {
			continue;
		}

		if (current->boat == false) { //left
			currentM = current->leftM;
			currentC = current->leftC;
		}
		else {
			currentM = current->rightM;
			currentC = current->rightC;
		}

		for (int i = 0; i <= k; i++) {           //船上载的传道士数量
			for (int j = 0; j <= k - i; j++) {   //船上载的野人数量
				if (validOnBoat(i, j, currentM, currentC)) {
					next_boat = !(current->boat);
					if (current->boat == 0) {
						next_lM = currentM - i;
						next_lC = currentC - j;
						next_rM = current->rightM + i;
						next_rC = current->rightC + j;
					}
					else {
						next_rM = currentM - i;
						next_rC = currentC - j;
						next_lM = current->leftM + i;
						next_lC = current->leftC + j;
					}
					if ((next_lM < next_lC && next_lM != 0) || (next_rM < next_rC && next_rM != 0)) continue;
					nextState = new state(next_boat, next_lM, next_lC, next_rM, next_rC);
					if (hashmap.count(*nextState) == 0 || (hashmap.count(*nextState) == 1 && hashmap[*nextState] > index + 1)) {
						nextState->previous = current->self;//current是栈上的本地变量，想要获取current的地址需要用current->self
						nextState->setindex(index + 1);
						nextState->setself();
						hashmap[*nextState] = index + 1;
						pqueue.push(*nextState);
					}
				}
			}
		}
	}

	while (*current != *start) {
		current->previous->next = current;
		current = current->previous;
	}

	while (*current != *final) {
		cout << *current << endl;
		current = current->next;
	}
	cout << *current << endl;
}

int main() {
	initRandomGenerator();
	search();
	return 0;
}



