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
/* �������ھ������ */
typedef struct {
	int m;          // ��������
	int n;          // ��������
	double *x;      // �����׵�ַ
} MAT;//����m��n�еľ���ṹ

#define MATwidth(mat)		((mat).n)  // ��������n
#define MATheight(mat)		((mat).m)  // ��������m

// ��������(i,j)Ԫ�ر�﷨  �����ǰ��еĴ���洢��
#define MATelem(mat, i, j)	((mat).x[((i) - 1) * MATwidth(mat) + (j) - 1])

/* Allocate the Matrix */
//�ڴ��з���m*n����СΪsizeof(double)�Ĵ洢�ռ䣬
//���Ѵ洢�ռ���׵�ַ���ظ�mat->x
int MATalloc(MAT*, int, int);

//*mat1=*mat2
void MATassign(MAT *, MAT *);

//�ͷž�����ڴ�ռ�
void MATfree(MAT *);

//��ȡ�ڶ������������󣬴浽��һ����
int MATinvgauss(MAT *, MAT *);

void MATsquare(MAT *);////*mat=I  ��λ����

#define N  3  /* Degrees of freedom ���ɶ���*/

/* Structure of PARAMETER */
struct PARA {
	double Len[N+1], LenG[N+1];   //��е�۳��ȣ���е����������λ��
	double Marm[N], Iarm[N];      //��е������Marm;��е��ת������Iarm
	double the0[N];               //��е�۳�ʼ�Ƕ�
};  //����ṹ�������洢����ֵ
   
/* GLOBLE VARIER DEFINATION ����ȫ�ֱ���*/
//������Ҫ����
void inputData(int *, double *, double *, double *, double *, double *);

//���ļ�"armP.txt"��������˽ṹ����
void readArmParameters(int, struct PARA *);

//���� ��е��ĩ��λ��
void endPosition(double *, double *, double *);

//����*len��*the������ſɱȾ���ĸ���Ԫ��ֵ�������*Jaco
void jacobianMatrix(double *, double *, MAT *);

//�����ſ˱Ⱦ���� ʱ��΢��
void dJacobianMatrix(double *, double *, double *, MAT *);

//����ĩ��ִ�������ٶ�
void endVelocity(MAT *, double *, double *);

//������ĩ���ٶ�����
int demandInput(int, double, double, double *, double *, double *,
		double, double, double *, double *, double *);

//����ת����������
void inertiaMatrix(double *, double *, double *, double *, double *, MAT *);

//��������Coriolis�������Ĺؽ�ת������*torC
void jointTorqueC(double *, double *, double *, double *, double *, double *, double *);

//��������������*torG
void jointTorqueG(double *, double *, double *, double *, double *);

//����ؽ�����*torq
int jointTorque(MAT *, double *, double *, double *, double *);

//�ؽ���ת���ٶ�*ddThe
int jointRotateAcceleration(MAT *, double *, double *);

//�����˷����ùؽ���ת����
int jointRotateVariables(double, MAT *, double *, double *, double *,
			 double *, double *, double *);
