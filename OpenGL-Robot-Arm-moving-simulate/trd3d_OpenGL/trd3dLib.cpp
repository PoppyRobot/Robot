/*
 ---------------------------------------
 	Libery for Traditional Control
		in Acceleration level
		(3-D Simulation)
	CopyRight Shugen MA
		97 / 06 / 03
 ---------------------------------------
*/

#include "stdafx.h"
#include "trd3dLib.h"
#include <math.h>

/*data input function*/
/*输入重要的参数*/
void inputData(int *num, double *endP, double *tSample,
			   double *lastTime, double *kP, double *kV)
{
	/* degree of freedom of arm*/
	/*自由度数*/
	*num = 3;

	/* target position of the front arm*/
	/*手臂前端目标位置*/
	endP[0] = -1.5; 
	endP[1] = 1.0; 
	endP[2] = 0.5;

	/*set the time period of sampling*/
	/*采样时间的 设定*/
	*tSample = 0.025;

	/*time of motion*//*运动时间*/
	*lastTime = 5.0;

	/*分解加速度控制的重要系数设定*/
	*kP = 200.0; 
	*kV = 30.0;
}

/* 读取手臂参数 */
void readArmParameters(int num, struct PARA *arm)
{
	register int i;   //定义寄存器变量
	char dummy[80];   //定义哑元数组
	FILE *fpPara;     //定义文件指针变量

	/* Open parameter File *//*打开参数文件*/
	// 从当前文件夹中，打开参数文件"armP.txt"; 如果打开失败，提示并退出
	if ((fpPara = fopen("armP.txt", "r")) == NULL) {
		fprintf(stderr, "Can not open <Arm Parameter File>!\n");
		AfxMessageBox("在当前目录下，找不到armP.txt参数文件\n程序将结束。");
		
		exit(FAULT);
	}  

	/* Read data from File */
	fscanf(fpPara, "%s", dummy);   //读入第一行
	fscanf(fpPara, "%s", dummy);   //读入第二行

	// 机械臂长度
	for (i = 0; i < num + 1; ++i)
		fscanf(fpPara, "%lf", (arm->Len + i));  

	// 机械臂质量中心位置
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num+1; ++i)
		fscanf(fpPara, "%lf", (arm->LenG + i));
	
	// 机械臂质量（千克）
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num; ++i)
		fscanf(fpPara, "%lf", (arm->Marm + i));
	
	// 机械臂转动惯量 千克平方米
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num; ++i)
		fscanf(fpPara, "%lf", (arm->Iarm + i));
	
	// 初始关节角度（度）
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num; ++i) {
		fscanf(fpPara, "%lf", (arm->the0 + i));
		arm->the0[i] *= PI / 180.0;
	}

	//关闭打开的参数文件
	fclose(fpPara);
}


/* Position of the End-effector */
//根据结构参数确定末端位置
/*
* @param: len  连杆长度
* @param: the  
*/
void endPosition(
     double *len, /* Link length 0, 1, 2, 3 */
     double *the, /* Joint length 0, 1, 2 */
     double *pos  /* End position x, y, z */ )
{
	pos[0] = (len[2] * cos(the[1]) + len[3] * cos(the[1] + the[2])) * cos(the[0]);
	pos[1] = (len[2] * cos(the[1]) + len[3] * cos(the[1] + the[2])) * sin(the[0]);
	pos[2] = len[0] + len[1] + len[2] * sin(the[1]) + len[3] * sin(the[1] + the[2]);
}

/* Jacobian Matrix */
//根据*len和*the计算出雅可比矩阵的各个元素值，输出给*Jaco
void jacobianMatrix(double *len, double *the, MAT *Jaco)
{
	double l3s23, l3c23;
	double aa, bb;

	l3s23 = len[3] * sin(the[1] + the[2]);
	l3c23 = len[3] * cos(the[1] + the[2]);
	aa = len[2] * cos(the[1]) + l3c23;
	bb = len[2] * sin(the[1]) + l3s23;

	MATelem(*Jaco, 1, 1) = -aa * sin(the[0]);
	MATelem(*Jaco, 1, 2) = -bb * cos(the[0]); 
	MATelem(*Jaco, 1, 3) = -l3s23 * cos(the[0]);
	MATelem(*Jaco, 2, 1) =  aa * cos(the[0]);
	MATelem(*Jaco, 2, 2) = -bb * sin(the[0]); 
	MATelem(*Jaco, 2, 3) = -l3s23 * sin(the[0]);
	MATelem(*Jaco, 3, 1) = 0; 
	MATelem(*Jaco, 3, 2) = aa; 
	MATelem(*Jaco, 3, 3) = l3c23;
}

/* Time-derivative of Jacobian Matrix */
//计算雅克比矩阵的 时间微分
void dJacobianMatrix(
     double *len,   // 连杆长度
     double *the,   // 关节角度
     double *dThe,  // 关节角速度
     MAT *dJaco)    // 雅克比矩阵的时间微分
{
	double l3s23, l3c23;
	double aa, bb;
	double cc, dd;

	l3s23 = len[3] * sin(the[1]+the[2]);
	l3c23 = len[3] * cos(the[1]+the[2]);
	aa = len[2] * cos(the[1]) + l3c23;
	bb = len[2] * sin(the[1]) + l3s23;
	cc = len[2] * cos(the[1]) * dThe[1] + l3c23 * (dThe[1] + dThe[2]);
	dd = len[2] * sin(the[1]) * dThe[1] + l3s23 * (dThe[1] + dThe[2]);

	MATelem(*dJaco, 1, 1) = -aa * cos(the[0]) * dThe[0] + dd * sin(the[0]);
	MATelem(*dJaco, 1, 2) = bb * sin(the[0]) * dThe[0] - cc * cos(the[0]);
	MATelem(*dJaco, 1, 3) = l3s23 * sin(the[0]) * dThe[0]-l3c23 * cos(the[0]) * (dThe[1] + dThe[2]);
	MATelem(*dJaco, 2, 1) = -aa * sin(the[0]) * dThe[0] - dd * cos(the[0]);
	MATelem(*dJaco, 2, 2) = -bb * cos(the[0]) * dThe[0] - cc * sin(the[0]);
	MATelem(*dJaco, 2, 3) = -l3s23 * cos(the[0]) * dThe[0]-l3c23 * sin(the[0]) * (dThe[1] + dThe[2]);
	MATelem(*dJaco, 3, 1) = 0;
	MATelem(*dJaco, 3, 2) = -dd;
	MATelem(*dJaco, 3, 3) = -l3s23 * (dThe[1] + dThe[2]);
}

/* Velocity of Arm End-Effector */
//计算末端执行器的速度
void endVelocity(
     MAT *Jaco,     // 雅克比矩阵
     double *dThe,  // 各关节角速度
     double *dPos)  // 末端执行器的速度
{
	register int i, j;
	int mm, nn;

	mm = MATheight(*Jaco);
	nn = MATwidth(*Jaco);
	for (i = 0; i < mm; i++) {
		dPos[i] = 0.0;
		for (j = 0; j < nn; j++)
		  dPos[i] += (MATelem(*Jaco, i + 1, j + 1) * dThe[j]);
	}
}

/* Acceleration of Arm End-Effector */
void endAcceleration(
     MAT *Jaco,
     MAT *dJaco, 
     double *dThe,
     double *ddThe,
     double *ddPos)
{
	register int i, j;
	int mm, nn;

	mm = MATheight(*Jaco);
	nn = MATwidth(*Jaco);
	for (i = 0; i < mm; i++) {
		ddPos[i] = 0.0;
		for (j = 0; j < nn; j++)
			ddPos[i] += (MATelem(*Jaco, i + 1, j + 1) * ddThe[j] + MATelem(*dJaco, i + 1, j + 1) * dThe[j]);
	}
}

/* Demanded Input of End Velocity */
//期望的末端速度输入
int demandInput(
     int num,
     double kP,
	 double kV,
     double *Pint,
	 double *Pend,
     double *len,
     double time,
	 double lastTime,
     double *the,
     double *dThe,
     double *ddP)
{
	register int i, j;
	double str;
	double cxyz[3];
	double Amp;
	double stD, veD, acD;

	double *p, *dP;
	double *q, *dQ;

	if ((p = (double *)calloc(num, sizeof(double))) == NULL)
		return FAULT;
	if ((q = (double *)calloc(num, sizeof(double))) == NULL)
		return FAULT;
	if ((dP = (double *)calloc(num, sizeof(double))) == NULL)
		return FAULT;
	if ((dQ = (double *)calloc(num, sizeof(double))) == NULL)
		return FAULT;

	str = sqrt((Pend[0] - Pint[0]) * (Pend[0] - Pint[0]) 
		+ (Pend[1] - Pint[1]) * (Pend[1] - Pint[1]) 
		+ (Pend[2] - Pint[2]) * (Pend[2] - Pint[2]));

	for (i = 0; i < num; ++i)
		cxyz[i] = (Pend[i] - Pint[i]) / str;

	Amp = 2.0 * PI * str / (lastTime * lastTime);
	acD = Amp * sin(2.0 * PI / lastTime * time);
	veD = Amp * lastTime / (2.0 * PI) *(1.0 - cos(2.0 * PI / lastTime * time));
	stD = Amp * lastTime / (2.0 * PI) *(time - lastTime / (2.0 * PI) * sin(2.0 * PI / lastTime * time));

	endPosition(len, the, p);
	{
		MAT Jaco;
		if (MATalloc(&Jaco, 3, 3) == FAULT)
			return FAULT;
		jacobianMatrix(len, the, &Jaco);
		endVelocity(&Jaco, dThe, dP);
		MATfree(&Jaco);
	}

	{
		MAT dJaco;
		if (MATalloc(&dJaco, 3, 3) == FAULT)
			return FAULT;
		dJacobianMatrix(len, the, dThe, &dJaco);
		for (i = 0; i < 3; ++i) {
			q[i] = Pint[i] + stD * cxyz[i];
			dQ[i] = veD * cxyz[i];
			ddP[i] = acD * cxyz[i] + kV * (dQ[i] - dP[i]) + kP * (q[i] - p[i]);
			for (j = 0; j < num; ++j)
				ddP[i] -= MATelem(dJaco, i + 1, j + 1) * dThe[j];
		}
		MATfree(&dJaco);
	}
	free(p);
	free(q);
	free(dP);
	free(dQ);

	return TRUE;
}

/* Inertia Matrix */
//计算转动惯量矩阵
void inertiaMatrix(
     double *lenG,
	 double *len,
     double *Marm,
	 double *Iarm,
     double *the,
     MAT *inert)
{
	MATelem(*inert, 1, 1) = Iarm[0] + (Marm[1] * lenG[2] * lenG[2] + Iarm[1]) * cos(the[1]) * cos(the[1]) 
		+ Marm[2] * (len[2] * cos(the[1]) + lenG[3] * cos(the[1] + the[2])) * (len[2] * cos(the[1]) + lenG[3] * cos(the[1] + the[2])) 
		+ Iarm[2] * cos(the[1] + the[2]) * cos(the[1] + the[2]);
	MATelem(*inert, 1, 2) = 0.0;
	MATelem(*inert, 1, 3) = 0.0;
	MATelem(*inert ,2, 1) = MATelem(*inert, 1 ,2);
	MATelem(*inert, 2, 2) = Iarm[1] + Marm[1] * lenG[2] * lenG[2] + Iarm[2] 
	                     + Marm[2] * (len[2] * len[2] + lenG[3] * lenG[3] + 2.0 * len[2] * lenG[3] * cos(the[2]));
	MATelem(*inert, 2, 3) = Iarm[2] + Marm[2] * (lenG[3] * lenG[3] + len[2] * lenG[3] * cos(the[2]));
	MATelem(*inert, 3, 1) = MATelem(*inert, 1, 3);
	MATelem(*inert, 3, 2) = MATelem(*inert, 2, 3);
	MATelem(*inert, 3, 3) = Iarm[2] + Marm[2] * lenG[3] * lenG[3];
}

/* Joint Torqus based on Centrifugal and Coriolis forces */
//离心力和Coriolis力产生的关节转动力矩*torC
void jointTorqueC(
     double *lenG,
	 double *len,
     double *Marm,
	 double *Iarm,
     double *the,
	 double *dThe,
     double *torC)
{
	torC[0] = 0.0;
	torC[1] = (Marm[1] * lenG[2] * lenG[2] + Iarm[1]) * cos(the[1]) * sin(the[1]) 
		+ Marm[2] * (len[2] * cos(the[1]) + lenG[2] * cos(the[1] + the[2])) * (len[2] * sin(the[1]) + lenG[2] * sin(the[1] + the[2])) 
		+ Iarm[2] * sin(the[1] + the[2]) * cos(the[1] + the[2]);
	torC[1] *= dThe[0] * dThe[0];
	torC[2] = Marm[2] * lenG[3] * (len[2] * cos(the[1]) + lenG[3] * cos(the[1] + the[2])) 
		+ Iarm[2] * sin(the[1] + the[2]) * cos(the[1] + the[2]);
	torC[2] *= dThe[0] * dThe[0];
	torC[2] += Marm[2] * len[1] * lenG[2] * sin(the[2]) * dThe[1] * dThe[1]
		+ Marm[2] * len[2] * lenG[3] * sin(the[1] + the[2]) * dThe[1] * dThe[2];
}

/* Joint Torqus based on Gravity force */
//重力产生的力矩*torG
void jointTorqueG(
     double *lenG,
	 double *len,
     double *Marm,
     double *the,
     double *torG)
{
	torG[0] = 0.0;
	torG[1] = Marm[1] * (len[2] + lenG[2]) * cos(the[1]) + Marm[2] * lenG[3] * cos(the[1] + the[2]);
	torG[2] = Marm[2] * lenG[3] * cos(the[1] + the[2]);
}

/* Joint Torques */
//计算关节力矩*torq
int jointTorque(
     MAT *Inert,
     double *ddThe,
     double *torC,
     double *torG,
     double *torq)
{
	register int i, j;
	int num;

	num = MATwidth(*Inert);
	if (MATheight(*Inert) != num)
		return FAULT;
	for (i = 0; i < num; ++i){
		torq[i] = torC[i] + torG[i];
		for (j = 0; j < num; ++j)
			torq[i] += MATelem(*Inert, i+1, j+1) * ddThe[j];
	}
	return TRUE;
}

/* Joint rotate aceelerations */
//关节旋转加速度*ddThe
int jointRotateAcceleration(
     MAT *Jaco,
     double *ddPos,
     double *ddThe)
{
	register int i, j;
	int num;
	MAT invJaco;

	num = MATwidth(*Jaco);
	if (MATheight(*Jaco) != num)
		return FAULT;
	if (MATalloc(&invJaco, num, num) == FAULT)
		return FAULT;
	if (MATinvgauss(&invJaco, Jaco) == FAULT){
		fprintf(stderr, "Error on inverse of Jacobian Matrix\n");
		exit(999);
	}
	for (i = 0; i < num; ++i){
		ddThe[i] = 0.0;
		for (j = 0; j < num; ++j)
		  ddThe[i] += MATelem(invJaco, i+1, j+1) * ddPos[j];
	}
	MATfree(&invJaco);

	return TRUE;
}

/* Joint rotate variables for ROBOT Simulator */
//机器人仿真用关节旋转变量
int jointRotateVariables(
     double tSample,
     MAT *Inertia,
     double *torq,
     double *torC,
     double *torG,
     double *the,
     double *dThe,
     double *ddThe)
{
	register int i, j;
	int num;
	MAT invIner;

	num = MATwidth(*Inertia);
	if (MATheight(*Inertia) != num)
		return FAULT;
	if (MATalloc(&invIner, num, num) == FAULT)
		return FAULT;
	if (MATinvgauss(&invIner, Inertia) == FAULT) {
		fprintf(stderr, "Error on inverse of Inertia Matrix\n");
		exit(999);
	}
	for (i = 0; i < num; ++i) {
		ddThe[i] = 0.0;
		for (j = 0; j < num; ++j)
			ddThe[i] += MATelem(invIner, i+1, j+1) * (torq[j] - torC[j] - torG[j]);
		the[i] += dThe[i]*tSample;
		dThe[i] += ddThe[i]*tSample;
	}
	MATfree(&invIner);

	return TRUE;
}


/* Libery for Matrix Calucation needed in this program */
#define swap(x,y)			{ double t; t = x; x = y; y = t; }

double MATeps = 1.0e-6;
int	maxNR = 500;

/* Allocate the Matrix */
//内存中分配 m * n 个大小为sizeof(double)的存储空间，
//并把存储空间的首地址返回给mat->x
int MATalloc(MAT *mat, int m, int n)
{
	if ( (mat->x = (double *)calloc(m * n, sizeof(double))) != NULL) {
		MATwidth(*mat)  = n;
		MATheight(*mat) = m;
		return (TRUE);
	}
	MATwidth(*mat) = MATheight(*mat) = 0;
	return (FAULT);
}

/* Free memory for Matrix */
void MATfree(MAT *mat)
{
	free(mat->x);
}

/* Uniquilize Matrix */
//*mat=I  单位矩阵
void MATsquare(MAT *mat)
{
	register int i, j;

	if ( MATwidth(*mat) == MATheight(*mat) )
		for (i = 1; i <= MATheight(*mat); ++i) {
			for (j = 1; j <= MATwidth(*mat); ++j)
				MATelem(*mat, i, j) = 0.0;
			MATelem(*mat, i, i) = 1.0;
		}
}

/* Assign Matrix */
//该函数先判断*mat1,*mat2是否是等宽等高，
//如果是的话就把*mat2的对应元素赋值给*mat1的对应元素
//即就是*mat1=*mat2
void MATassign(MAT *mat1, MAT *mat2)
{
	//The register keyword specifies that the variable is to be stored in a machine register, if possible
	register int i, j;

	if ( MATheight(*mat1) == MATheight(*mat2) 
		&& MATwidth(*mat1) == MATwidth(*mat2) ) {
		for (i = 1; i <= MATheight(*mat1); ++i)
			for (j = 1; j <= MATwidth(*mat1); ++j)
				MATelem(*mat1, i, j) = MATelem(*mat2, i, j);
	}
}

/* Inverse of Matrix */
//矩阵求逆，*invx等于*x的逆矩阵
int MATinvgauss(MAT *invx, MAT *x)
{
	register int i, j, k, n, mx;
	double temp;
	MAT a;

	n = MATheight(*invx);
	if (MATalloc(&a, n, n) == FAULT)
		return FAULT;
	MATassign(&a, x);//&a=x
	MATsquare(invx);//ia=I单位阵
	for (k = 1; k <= n-1; ++k)
	{
		for (i = k+1, mx = k; i <= n; ++i)
		{	//2003.3.6
			if ( fabs(MATelem(a, i, k)) > fabs(MATelem(a, mx, k)) )
				mx = i;
		}
		//2003.3.6
		if (fabs(MATelem(a, mx, k)) < MATeps)        
			return FAULT;

		if (mx != k)
		{
			for (j = k; j <= n; ++j)   
				swap(MATelem(a, k, j), MATelem(a, mx, j));

			for (j = 1; j <= n; ++j)  	
				swap(MATelem(*invx, k, j), MATelem(*invx, mx, j));
		}

		for (i = k + 1; i <= n; i++)
		{
			temp = MATelem(a, i, k) / MATelem(a, k, k);
			for (j = 1; j <= n; j++)   
				MATelem(a,i,j) -= temp * MATelem(a, k, j);

			for (j = 1; j <= n; j++)   
				MATelem(*invx, i, j) -= temp * MATelem(*invx, k, j);
		}
	}

	for (k = n; k >= 1; --k)
	{
		for (j = 1; j <= n; ++j)	   
			MATelem(*invx, k, j) /= MATelem(a, k, k);

		for (i = k - 1; i >= 1; --i) 
		{
			for (j = 1; j <= n; ++j)
				MATelem(*invx, i, j) -= MATelem(*invx, k, j)*MATelem(a, i, k);
		}
	}
	MATfree(&a);

	return TRUE;
}

/* End Of Libery */
