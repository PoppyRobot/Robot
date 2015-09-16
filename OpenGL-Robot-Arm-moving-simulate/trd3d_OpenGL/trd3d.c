/*
   --------------------------------------------
   Path Tracking Control of 3-DOF Manipulator
		(Resolved Acceleration Control)
   CopyRight (c) Shugen MA
   June 5, 1998
   ---------------------------------------------

#include "trd3d.h"


/* �f�[�^�̓��͊֐� */
void inputData(int *num, double *endP, double *tSample, double *lastTime, double *kP, double *kV)
{
  /* �A�[���̎��R�x�� */
  *num = 3;
  /* �A�[����[�̖ړI�ʒu */
  endP[0] = -1.5; endP[1] = 1.0; endP[2] = 0.5;
  /* �T���v�����O���Ԃ̐ݒ� */
  *tSample = 0.005;
  /* �쓮�i�^���j���� */
  *lastTime = 5.0;
  /* ���������x����̂��߂̏d�݌W���̐ݒ� */
  *kP = 200.0; *kV = 30.0;
}

/* �A�[���p�����[�^�̓ǂݍ��݂Ɛݒ�֐� */
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
