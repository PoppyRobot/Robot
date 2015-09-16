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
/*������Ҫ�Ĳ���*/
void inputData(int *num, double *endP, double *tSample,
			   double *lastTime, double *kP, double *kV)
{
	/* degree of freedom of arm*/
	/*���ɶ���*/
	*num = 3;

	/* target position of the front arm*/
	/*�ֱ�ǰ��Ŀ��λ��*/
	endP[0] = -1.5; 
	endP[1] = 1.0; 
	endP[2] = 0.5;

	/*set the time period of sampling*/
	/*����ʱ��� �趨*/
	*tSample = 0.025;

	/*time of motion*//*�˶�ʱ��*/
	*lastTime = 5.0;

	/*�ֽ���ٶȿ��Ƶ���Ҫϵ���趨*/
	*kP = 200.0; 
	*kV = 30.0;
}

/* ��ȡ�ֱ۲��� */
void readArmParameters(int num, struct PARA *arm)
{
	register int i;   //����Ĵ�������
	char dummy[80];   //������Ԫ����
	FILE *fpPara;     //�����ļ�ָ�����

	/* Open parameter File *//*�򿪲����ļ�*/
	// �ӵ�ǰ�ļ����У��򿪲����ļ�"armP.txt"; �����ʧ�ܣ���ʾ���˳�
	if ((fpPara = fopen("armP.txt", "r")) == NULL) {
		fprintf(stderr, "Can not open <Arm Parameter File>!\n");
		AfxMessageBox("�ڵ�ǰĿ¼�£��Ҳ���armP.txt�����ļ�\n���򽫽�����");
		
		exit(FAULT);
	}  

	/* Read data from File */
	fscanf(fpPara, "%s", dummy);   //�����һ��
	fscanf(fpPara, "%s", dummy);   //����ڶ���

	// ��е�۳���
	for (i = 0; i < num + 1; ++i)
		fscanf(fpPara, "%lf", (arm->Len + i));  

	// ��е����������λ��
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num+1; ++i)
		fscanf(fpPara, "%lf", (arm->LenG + i));
	
	// ��е��������ǧ�ˣ�
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num; ++i)
		fscanf(fpPara, "%lf", (arm->Marm + i));
	
	// ��е��ת������ ǧ��ƽ����
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num; ++i)
		fscanf(fpPara, "%lf", (arm->Iarm + i));
	
	// ��ʼ�ؽڽǶȣ��ȣ�
	fscanf(fpPara, "%s", dummy);
	for (i = 0; i < num; ++i) {
		fscanf(fpPara, "%lf", (arm->the0 + i));
		arm->the0[i] *= PI / 180.0;
	}

	//�رմ򿪵Ĳ����ļ�
	fclose(fpPara);
}


/* Position of the End-effector */
//���ݽṹ����ȷ��ĩ��λ��
/*
* @param: len  ���˳���
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
//����*len��*the������ſɱȾ���ĸ���Ԫ��ֵ�������*Jaco
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
//�����ſ˱Ⱦ���� ʱ��΢��
void dJacobianMatrix(
     double *len,   // ���˳���
     double *the,   // �ؽڽǶ�
     double *dThe,  // �ؽڽ��ٶ�
     MAT *dJaco)    // �ſ˱Ⱦ����ʱ��΢��
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
//����ĩ��ִ�������ٶ�
void endVelocity(
     MAT *Jaco,     // �ſ˱Ⱦ���
     double *dThe,  // ���ؽڽ��ٶ�
     double *dPos)  // ĩ��ִ�������ٶ�
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
//������ĩ���ٶ�����
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
//����ת����������
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
//��������Coriolis�������Ĺؽ�ת������*torC
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
//��������������*torG
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
//����ؽ�����*torq
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
//�ؽ���ת���ٶ�*ddThe
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
//�����˷����ùؽ���ת����
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
//�ڴ��з��� m * n ����СΪsizeof(double)�Ĵ洢�ռ䣬
//���Ѵ洢�ռ���׵�ַ���ظ�mat->x
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
//*mat=I  ��λ����
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
//�ú������ж�*mat1,*mat2�Ƿ��ǵȿ�ȸߣ�
//����ǵĻ��Ͱ�*mat2�Ķ�ӦԪ�ظ�ֵ��*mat1�Ķ�ӦԪ��
//������*mat1=*mat2
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
//�������棬*invx����*x�������
int MATinvgauss(MAT *invx, MAT *x)
{
	register int i, j, k, n, mx;
	double temp;
	MAT a;

	n = MATheight(*invx);
	if (MATalloc(&a, n, n) == FAULT)
		return FAULT;
	MATassign(&a, x);//&a=x
	MATsquare(invx);//ia=I��λ��
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
