#include <iostream>
#include <random>
#include <iomanip>
#include <Windows.h>
#include <queue>
#include <unordered_map>

using namespace std;

int maxRandomNum = 1000000;   //���������
int randomSeed = 10000;//���������

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//������ʾ���������,�������ֻ����ÿ��ͷ�ļ��е����������ͺ�����������Ч
void gotoxy(HANDLE hOut, int x, int y)        //����x��y������������෴�ģ�ע������
{
	COORD pos;
	pos.X = x;             //������
	pos.Y = y;             //������
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

void initRandomGenerator() {       //��ʼ�������������
	for (int i = 0; i < randomSeed; i++) {
		getRandomNumber(0);
	}
}

const int n = 3;//����ʿ/Ұ����Ŀ
const int k = 2;//�����ؿ���

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
	int leftM;  //�󰶴���ʿ
	int leftC;  //��Ұ��
	int rightM; //�Ұ�����ʿ
	int rightC; //�Ұ�Ұ��
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

struct hash_state {  //unordered_map�����ϣֵ
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

		for (int i = 0; i <= k; i++) {           //�����صĴ���ʿ����
			for (int j = 0; j <= k - i; j++) {   //�����ص�Ұ������
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
						nextState->previous = current->self;//current��ջ�ϵı��ر�������Ҫ��ȡcurrent�ĵ�ַ��Ҫ��current->self
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



