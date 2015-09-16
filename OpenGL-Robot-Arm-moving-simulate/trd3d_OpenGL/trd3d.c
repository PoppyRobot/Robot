/*
   --------------------------------------------
   Path Tracking Control of 3-DOF Manipulator
		(Resolved Acceleration Control)
   CopyRight (c) Shugen MA
   June 5, 1998
   ---------------------------------------------

#include "trd3d.h"


/* データの入力関数 */
void inputData(int *num, double *endP, double *tSample, double *lastTime, double *kP, double *kV)
{
  /* アームの自由度数 */
  *num = 3;
  /* アーム先端の目的位置 */
  endP[0] = -1.5; endP[1] = 1.0; endP[2] = 0.5;
  /* サンプリング時間の設定 */
  *tSample = 0.005;
  /* 駆動（運動）時間 */
  *lastTime = 5.0;
  /* 分解加速度制御のための重み係数の設定 */
  *kP = 200.0; *kV = 30.0;
}

/* アームパラメータの読み込みと設定関数 */
void readArmParameters(int num, struct PARA *arm)
{
  register int i;
  char dummy[80];
  FILE *fpPara;

  /* Open parameter File */
  if((fpPara = fopen("armP.dat", "r")) == NULL){
	fprintf(stderr, "Can not open <Arm Parameter File>!\n");
	exit(FAULT);
  }

  /* Read data from File */
  fscanf(fpPara, "%s", dummy);
  fscanf(fpPara, "%s", dummy);
  for(i = 0; i < num+1; ++i)
	fscanf(fpPara, "%lf", (arm->Len+i));
  fscanf(fpPara, "%s", dummy);
  for(i = 0; i < num+1; ++i)
	fscanf(fpPara, "%lf", (arm->LenG+i));
  fscanf(fpPara, "%s", dummy);
  for(i = 0; i < num; ++i)
	fscanf(fpPara, "%lf", (arm->Marm+i));
  fscanf(fpPara, "%s", dummy);
  for(i = 0; i < num; ++i)
	fscanf(fpPara, "%lf", (arm->Iarm+i));
  fscanf(fpPara, "%s", dummy);
  for(i = 0; i < num; ++i){
	fscanf(fpPara, "%lf", (arm->the0+i));
	arm->the0[i] *= PI / 180.0;
  }
  fclose(fpPara);
}

/* End of Program */
