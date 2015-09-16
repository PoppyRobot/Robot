/*
   --------------------------------------------
   Path Tracking Control of 3-DOF Manipulator
   Head File <trd3dLib.h>

   CopyRight (c) Shugen MA
   June 6, 1997
   ----------------------------------
*/
#define PI 3.1415926535897932384626433832795  /* PEI number */
#define TRUE   1
#define FAULT  0

/* Defination for Matrix calculation */
/* 定义用于矩阵计算 */
typedef struct {
	int m;          // 矩阵行数
	int n;          // 矩阵列数
	double *x;      // 矩阵首地址
} MAT;//定义m行n列的矩阵结构

#define MATwidth(mat)		((mat).n)  // 矩阵列数n
#define MATheight(mat)		((mat).m)  // 矩阵行数m

// 定义矩阵的(i,j)元素表达法  矩阵是按列的次序存储的
#define MATelem(mat, i, j)	((mat).x[((i) - 1) * MATwidth(mat) + (j) - 1])

/* Allocate the Matrix */
//内存中分配m*n个大小为sizeof(double)的存储空间，
//并把存储空间的首地址返回给mat->x
int MATalloc(MAT*, int, int);

//*mat1=*mat2
void MATassign(MAT *, MAT *);

//释放矩阵的内存空间
void MATfree(MAT *);

//求取第二个矩阵的逆矩阵，存到第一个中
int MATinvgauss(MAT *, MAT *);

void MATsquare(MAT *);////*mat=I  单位矩阵

#define N  3  /* Degrees of freedom 自由度数*/

/* Structure of PARAMETER */
struct PARA {
	double Len[N+1], LenG[N+1];   //机械臂长度，机械臂质量中心位置
	double Marm[N], Iarm[N];      //机械臂质量Marm;机械臂转动惯量Iarm
	double the0[N];               //机械臂初始角度
};  //定义结构变量，存储参数值
   
/* GLOBLE VARIER DEFINATION 定义全局变量*/
//输入重要参数
void inputData(int *, double *, double *, double *, double *, double *);

//从文件"armP.txt"读入机器人结构参数
void readArmParameters(int, struct PARA *);

//计算 机械臂末端位置
void endPosition(double *, double *, double *);

//根据*len和*the计算出雅可比矩阵的各个元素值，输出给*Jaco
void jacobianMatrix(double *, double *, MAT *);

//计算雅克比矩阵的 时间微分
void dJacobianMatrix(double *, double *, double *, MAT *);

//计算末端执行器的速度
void endVelocity(MAT *, double *, double *);

//期望的末端速度输入
int demandInput(int, double, double, double *, double *, double *,
		double, double, double *, double *, double *);

//计算转动惯量矩阵
void inertiaMatrix(double *, double *, double *, double *, double *, MAT *);

//离心力和Coriolis力产生的关节转动力矩*torC
void jointTorqueC(double *, double *, double *, double *, double *, double *, double *);

//重力产生的力矩*torG
void jointTorqueG(double *, double *, double *, double *, double *);

//计算关节力矩*torq
int jointTorque(MAT *, double *, double *, double *, double *);

//关节旋转加速度*ddThe
int jointRotateAcceleration(MAT *, double *, double *);

//机器人仿真用关节旋转变量
int jointRotateVariables(double, MAT *, double *, double *, double *,
			 double *, double *, double *);
