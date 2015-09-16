/////////////////////////////////////////////////////////
// �����OpenGL ���f����`��
//������OpenGLģ������
// MyView.cpp


#include "stdafx.h"
#include "OpenGLDoc.h"
#include "OpenGLView.h"
#include "MyView.h"

#include "trd3dLib.h"  

IMPLEMENT_DYNCREATE(CMyView, COpenGLView)

BEGIN_MESSAGE_MAP(CMyView, COpenGLView)
	//{{AFX_MSG_MAP(CMyView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// �O���ϐ�  
struct PARA Arm;
MAT		Jaco;		//�ſ˱Ⱦ���
MAT		dJaco;		//�ſ˱Ⱦ����ʱ��΢��
MAT		Inertia;	//���Ծ���

double	*the;		//�ؽڽǶ�
double	*dThe;		//�ؽڽ��ٶ�
double  *ddThe;		//�ؽڽǼ��ٶ�
double	*theRecord;	//�ؽڽǶȴ洢����

double posInt[N];    //��ʼλ��
double posEnd[N];    //����λ��

double  *torq;       //����q
double  *torq0;      //����q0
double  *torC;       //����C
double  *torG;       //����G

int num;
double tSample;       //����ʱ��
double t;
double lastTime;      //����ʱ��

/////////////////////////////////////////////////////////

//
CMyView::CMyView()
{	
  register int i;
  int count;
	
  double ddPos[N];
  double kP, kV;

  //�����Ҫ������
  inputData(&num, posEnd, &tSample, &lastTime, &kP, &kV);
  if(num != 3){
	exit(FAULT);
  }

  //ȷ����������Ĵ洢�ռ����
  if((the = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((dThe = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((ddThe = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((torG = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((torC = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((torq = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((torq0 = (double *)calloc(num, sizeof(double))) == NULL)
	exit(FAULT);
  if((theRecord = (double *)calloc(num*(int)(lastTime/tSample+1), sizeof(double))) == NULL)
	exit(FAULT);
  if(MATalloc(&Jaco, num, num) == FAULT)
	exit(FAULT);
  if(MATalloc(&dJaco, num, num) == FAULT)
	exit(FAULT);
  if(MATalloc(&Inertia, num, num) == FAULT)
	exit(FAULT);


  //�ؽڲ����Ķ��룬�ؽ����Ƶĳ�ʼ��
  readArmParameters(num, &Arm);

  endPosition(Arm.Len, Arm.the0, posInt);
  for(i = 0; i < num; ++i){
	the[i] = Arm.the0[i];
	dThe[i] = 0.0;//��ʼ�ٶ�Ϊ0
  }


  //ѭ������Ŀ�ʼ
  t = 0.0;
  count = 0;
  for(i=0; i<num; i++)
		torq0[i] = 0.0;

  do{

	  //ǰ�˵��������ٶ�
	demandInput(num, kP, kV, posInt, posEnd, Arm.Len, t, lastTime,
				the, dThe, ddPos);


	//�ſ˱Ⱦ�����ʱ��΢�ֵļ���
	jacobianMatrix(Arm.Len, the, &Jaco);
	dJacobianMatrix(Arm.Len, the, dThe, &dJaco);

	//arm��ת������
	inertiaMatrix(Arm.LenG, Arm.Len, Arm.Marm, Arm.Iarm, the, &Inertia);


	//�ؽڽǼ��ٶȵļ���
	if(jointRotateAcceleration(&Jaco, ddPos, ddThe) == FAULT)
	  exit(FAULT);

    //�ؽ�Ť�صļ���
	jointTorqueC(Arm.LenG, Arm.Len, Arm.Marm, Arm.Iarm, the, dThe, torC);
	jointTorqueG(Arm.LenG, Arm.Len, Arm.Marm, the, torG);
	jointTorque(&Inertia, ddThe, torC, torG, torq);

	//��һ����ʱ���趨
	t += tSample;
	for(i = 0; i < num; ++i)
		torq0[i] = torq[i];

	//�������Ĺؽڱ���
	jointRotateVariables(tSample, &Inertia, torq, torC, torG, the, dThe, ddThe);

	//�����ͼ�ùؽڽǶ�����
	for(i=0; i<num; i++)
		theRecord[ count*num + i ] = the[i] * 180.0/PI;

	count++;

  } while(t < lastTime);

  //������Ƶĳ�ʼ��
  for(i=0; i<num; i++)
	Arm.the0[i] = theRecord[i];

}

/////////////////////////////////////////////////////////

CMyView::~CMyView()
{
	
  free(the);
  free(dThe);
  free(ddThe);
  free(theRecord);
  free(torG);
  free(torC);
  free(torq);
  free(torq0);
  MATfree(&Jaco);
  MATfree(&dJaco);
  MATfree(&Inertia);

}

/////////////////////////////////////////////////////////

BOOL CMyView::RenderStockScene()
{
	drawCoordinate( 1.0 );  //�����������

	drawFloor(2); 	        //����������
	//drawTrajectory(posInt, posEnd);     //ǰ�˹켣������

	return TRUE;
}

/////////////////////////////////////////////////////////
//�����������ƶ��ĳ���
BOOL CMyView::RenderScene()
{
	// �����ֱ�
	drawArm();		

	return TRUE;
}

// �����ֱ�
void CMyView::drawArm()
{
	////////////////////////////////////////////////////////////////////
	// �����˸��ؽڵ�����任�� ͨ��"D-H"�����õ��Ĳ���                                                               //
	//	joint   ai    ��i    di    ��i
	//	==================================
	//	  1     0      0     l0    ��1
	//	----------------------------------
	//	  1.5   0      0      l1    0
	//	----------------------------------
	//	  2     0     ��/2    0    ��2
	//	----------------------------------
	//	  3     l2     0      0    ��3
	//	----------------------------------
	//	  H     l3     0      0     0
	//	----------------------------------


	GLdouble height, radius;
	int n_div = 16; 
	//�趨�ָ������������30

	//������ʵĶ���

    //��ɫ��
	GLfloat red_ambient[]  = { 0.2f, 0.0f, 0.0f }; //������
	GLfloat red_diffuse[]  = { 0.5f, 0.0f, 0.0f }; //��ɢ��
	GLfloat red_specular[] = { 0.7f, 0.6f, 0.6f }; //�����
	GLfloat red_shininess[]= { 32.0f };			//  ����ǿ��

    //��ɫ
	GLfloat green_ambient[]  = { 0.0f, 0.2f, 0.0f }; 
	GLfloat green_diffuse[]  = { 0.0f, 0.5f, 0.0f }; 
	GLfloat green_specular[] = { 0.6f, 0.7f, 0.6f }; 
	GLfloat green_shininess[]= { 32.0f };			

    //��ɫ
	GLfloat blue_ambient[]  = { 0.0f, 0.0f, 0.2f }; 
	GLfloat blue_diffuse[]  = { 0.0f, 0.0f, 0.5f }; 
	GLfloat blue_specular[] = { 0.6f, 0.6f, 0.7f }; 
	GLfloat blue_shininess[]= { 32.0f };			

	//��ɫ
	GLfloat yellow_ambient[]  = { 0.2f, 0.2f, 0.0f }; 
	GLfloat yellow_diffuse[]  = { 0.2f, 0.2f, 0.0f }; 
	GLfloat yellow_specular[] = { 0.7f, 0.7f, 0.5f }; 
	GLfloat yellow_shininess[]= { 32.0f };			

	glPushMatrix();
 
		
	//link 0  ��̨��
	/////////////////////////////////////////////////////////////
	// link 0 

	//������ʵĶ��壺��
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	radius = 0.3;
	mySolidCylinder( radius, Arm.Len[0], n_div );

	/*
	����void mySolidCylinder( double radius, double  height, int n_div);��
	z�᷽���ϵ�Բ�������������������������������£�
		 radius : �뾶
		 height : �߶�
		 n_div:   ԲͲ�ķָ����
	*/
	/////////////////////////////////////////////////////////////
	// joint 1

	// a1 = 0;
	// ��1 = 0;

	glTranslated( 0.0, 0.0, Arm.Len[0] );	// D-H������d1
	glRotated( Arm.the0[0], 0, 0, 1 );		// D-H�����Ħ�1

	//������ʵĶ��壺��ɫ 
	glMaterialfv( GL_FRONT, GL_AMBIENT, yellow_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, yellow_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,yellow_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	radius = 0.25;
	height = 0.01;
	mySolidCylinder( radius, height, n_div );

	//�������ϵ������������
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// link 1   

	//������ʵĶ��壺��ɫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, green_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, green_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,green_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, green_shininess );	

	radius = 0.2;
	mySolidCylinder( radius, Arm.Len[1], n_div );

	//�����ԲͲ
	glPushMatrix();    // ��ת���ؽ�1
		height = 2.0 * radius;
		glTranslated( 0.0, 0.0, Arm.Len[1] );
		glRotated( 90.0, 1.0, 0.0, 0.0 );
		glTranslated( 0.0, 0.0, -height/2.0 );
		mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	/////////////////////////////////////////////////////////////
	// joint 1.5
	glTranslated( 0.0, 0.0, Arm.Len[1] );	
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// joint 2

	// a2 = 0;
	glRotated( 90.0, 1.0, 0.0, 0.0);		//D-H�����Ħ�2

	// d2 = 0;
	glRotated( Arm.the0[1], 0, 0, 1 );		//D-H�����Ħ�2

	glPushMatrix();
	radius = 0.21;
	height = radius;

	//������ʵĶ��壺��ɫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );	

	glTranslated( 0.0, 0.0, -height/2.0 );
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	//�������ϵ������������
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// link 2

	//������ʵĶ���:��ɫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );	

	// link 2 �{��      //link2����
	glPushMatrix();
	radius = 0.15;
	glRotated( 90.0, 0.0, 1.0, 0.0 );
	mySolidCylinder( radius, Arm.Len[2], n_div );
	glPopMatrix();

	// link 2 ��[���̎���   //link2�϶˲���֧����
	glPushMatrix();
	height = radius * 2.0;
	glTranslated( Arm.Len[2], 0.0, 0.0 );
	glTranslated( 0.0, 0.0, -height/2.0 );
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	/////////////////////////////////////////////////////////////
	// joint 3

	glTranslated( Arm.Len[2], 0.0, 0.0 ); // a3
	// ��3 = 0
	// d3 = 0

	glRotated( Arm.the0[2], 0, 0, 1 );	// ��3

	glPushMatrix();
	radius = 0.16;
	height = radius;

	// ���̂̍ގ��̒�` : ��   //������ʵĶ��壺��
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	glTranslated( 0.0, 0.0, -height/2.0 );  
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	// ���΍��W�n�̍��W����`��  // �������ϵ������������
	drawCoordinate(0.5);

	/////////////////////////////////////////////////////////////
	// link 3

	// ���̂̍ގ��̒�` : ��    // ������ʵĶ���
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	glPushMatrix();
	radius = 0.1;
	glRotated( 90.0, 0.0, 1.0, 0.0 );
	mySolidCylinder( radius, Arm.Len[3], n_div );
	glPopMatrix();

	/////////////////////////////////////////////////////////////
	//  hand    //��
	glTranslated( Arm.Len[3], 0.0, 0.0 );

	//�������ϵ������������
	COpenGLView::drawCoordinate( 0.5 ); 		// ���΍��W�n�̍��W����`��


	glPopMatrix();

}

// �w�ߋO����`��     //ָ����������
void CMyView::drawTrajectory(double *pInitial, double *pTarget)
{
	// �����v���X�e�b�B�N    //��ɫ
	GLfloat white_ambient[]  = { 0.5f, 0.5f, 0.5f }; // ���F//������
	GLfloat white_diffuse[]  = { 0.5f, 0.5f, 0.5f }; // �g�U�F//��ɢ��
	GLfloat white_specular[] = { 0.5f, 0.6f, 0.6f }; // ���ʐF//�����
	GLfloat white_shininess[]= { 32.0f };			// ���˂̋���//����ǿ��

	glPushMatrix();

	// ���̂̍ގ��̒�` : ��    //������ʵĶ���
	glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, white_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );	

	// �w�ߋO����j���ŕ`��     //ָ��켣����������
	glEnable( GL_LINE_STIPPLE );
	glLineStipple( 4, 0xAAAA );

	glBegin( GL_LINES );
	glVertex3dv( pInitial );
	glVertex3dv( pTarget );
	glEnd();

	glDisable( GL_LINE_STIPPLE );

	glPopMatrix();

}


// �A�j���[�V�����p�^�C�}  //animation time  �����õ�ʱ��
int count;
void CMyView::OnTimer(UINT nIDEvent)
{
	int i;

	// �֐ߊp�x�͔z��ɕۑ����Ă���̂ŁA������Ăяo���Ďg�p����B
	//�ؽڽǶȱ��棬�����ٴε���
	for( i=0; i< num; i++)
	{
		Arm.the0[i] = theRecord[count*num +i];
	}

	count++;

	if ( count > (int)(lastTime / tSample) )
	{
		//KillTimer(1);
		count = 0;
	}

	// ��ʂ̍ĕ`��   //����������
	InvalidateRect(FALSE);
	
	COpenGLView::OnTimer(nIDEvent);
}

// ���̕`��    //�������
void CMyView::drawFloor(double floorSize)
{
	int i;

	// �����v���X�e�b�B�N    //��ɫ
	GLfloat white_ambient[]  = { 0.3f, 0.3f, 0.1f }; // ���F//����ɫ
	GLfloat white_diffuse[]  = { 0.3f, 0.3f, 0.1f }; // �g�U�F//��ɢɫ
	GLfloat white_specular[] = { 0.1f, 0.1f, 0.1f }; // ���ʐF//����ɫ
	GLfloat white_shininess[]= { 2.0f };			// ���˂̋���//����ǿ��

	// ��//��ɫ
	GLfloat blue_ambient[]  = { 0.2f, 0.2f, 0.1f }; // ���F //����ɫ
	GLfloat blue_diffuse[]  = { 0.0f, 0.0f, 0.0f }; // �g�U�F  //��ɢɫ
	GLfloat blue_specular[] = { 0.1f, 0.1f, 0.1f }; // ���ʐF   //����ɫ
	GLfloat blue_shininess[]= { 1.0f };			// ���˂̋���   //����ǿ��

	glPushMatrix();

	// ���̂̍ގ��̒�` : ��//����Ĳ��ʶ��壺��
	glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,white_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );

	// ���̃��C��  //���ϻ���
	glBegin(GL_LINES);

	for (i = -4; i <= 4; i++)
	{
		glVertex3d(-floorSize, floorSize / 4.0 * (double)i, 0.02 );
		glVertex3d( floorSize, floorSize / 4.0 * (double)i, 0.02 );
	}

	for (i = -4; i <= 4; i++)
	{
		glVertex3d( floorSize / 4.0 * (double)i, -floorSize, 0.02 );
		glVertex3d( floorSize / 4.0 * (double)i,  floorSize, 0.02 );
	}
	glEnd();

	// ���̂̍ގ��̒�` : ��    //������ʵĶ��壺��ɫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );

	// ����     //����
	glBegin(GL_QUADS);
	glVertex3d( floorSize, floorSize, 0.0 );
	glVertex3d(-floorSize, floorSize, 0.0 );
	glVertex3d(-floorSize,-floorSize, 0.0 );
	glVertex3d( floorSize,-floorSize, 0.0 );
	glEnd();

	glPopMatrix();
}

//////////////////////////////////
