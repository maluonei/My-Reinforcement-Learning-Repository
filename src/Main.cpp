//#include "head/Search.h"
#include "Search.h"
#include "EnforcementLearning.h"
using namespace std;

//const int gridX = 15; //地图行数
//const int gridY = 15; //地图列数

int epoch = 300;//设定训练次数 sarsa(lambda)算法所需轮次更多
int maxTry = 50000;//若maxTry次没有结束一轮则重新开始
//int startx = 0; //起始点x与y
//int starty = 1;
//int endx = 13;   //终点x与y
//int endy = 14;
int screenFlashTimeSlice = 0; //如果可视化，每一步进行时间（毫秒）
double chance = 0.9;          //1-chance为选择随机策略的概率
double alpha = 0.8;           //sasra算法中的迭代步长
double gamma = 0.8;           //衰减值
double lambda = 0.5;          //sarsa(lambda)中的lambda
bool isAddRestriction = false;//是否碰到障碍物就停下来进行洗下一轮
bool isVisuable = false;       //是否可视化训练过程（会很慢）
//left--0--i-1
//down--1--j-1
//right--2--i+1
//up--3--j+1

int main() {
	initRandomGenerator(); //初始化随机数生成器
	initM(); //初始化地图
	//initR(); //初始化reward矩阵
	//initQ(); //初始化Q矩阵

	//train_QLearning();//训练
	//train();
	//train_Sarsa();
	//train_SarsaLambda();
	policy* p = new QLearningPolicy();
	p->train();
	cout << "train done\n";
	cout << "***************************************\n";

	printM();//打印地图
	cout << "print1 done\n";
	cout << "***************************************\n";
	//printQ();

	p->printPath();//设置起点,绘画路径
	cout << "print2 done\n";
	cout << "***************************************\n";
	return 0;
}
