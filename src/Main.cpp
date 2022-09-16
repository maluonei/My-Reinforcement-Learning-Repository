//#include "head/Search.h"
#include "Search.h"
#include "EnforcementLearning.h"
using namespace std;

//const int gridX = 15; //��ͼ����
//const int gridY = 15; //��ͼ����

int epoch = 300;//�趨ѵ������ sarsa(lambda)�㷨�����ִθ���
int maxTry = 50000;//��maxTry��û�н���һ�������¿�ʼ
//int startx = 0; //��ʼ��x��y
//int starty = 1;
//int endx = 13;   //�յ�x��y
//int endy = 14;
int screenFlashTimeSlice = 0; //������ӻ���ÿһ������ʱ�䣨���룩
double chance = 0.9;          //1-chanceΪѡ��������Եĸ���
double alpha = 0.8;           //sasra�㷨�еĵ�������
double gamma = 0.8;           //˥��ֵ
double lambda = 0.5;          //sarsa(lambda)�е�lambda
bool isAddRestriction = false;//�Ƿ������ϰ����ͣ��������ϴ��һ��
bool isVisuable = false;       //�Ƿ���ӻ�ѵ�����̣��������
//left--0--i-1
//down--1--j-1
//right--2--i+1
//up--3--j+1

int main() {
	initRandomGenerator(); //��ʼ�������������
	initM(); //��ʼ����ͼ
	//initR(); //��ʼ��reward����
	//initQ(); //��ʼ��Q����

	//train_QLearning();//ѵ��
	//train();
	//train_Sarsa();
	//train_SarsaLambda();
	policy* p = new QLearningPolicy();
	p->train();
	cout << "train done\n";
	cout << "***************************************\n";

	printM();//��ӡ��ͼ
	cout << "print1 done\n";
	cout << "***************************************\n";
	//printQ();

	p->printPath();//�������,�滭·��
	cout << "print2 done\n";
	cout << "***************************************\n";
	return 0;
}
