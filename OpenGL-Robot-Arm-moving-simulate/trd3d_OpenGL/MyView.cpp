/////////////////////////////////////////////////////////
// 帺嶌偺OpenGL 儌僨儖傪昤偔
//自作的OpenGL模型描述
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



// 奜晹曄悢  
struct PARA Arm;
MAT		Jaco;		//雅克比矩阵
MAT		dJaco;		//雅克比矩阵的时间微分
MAT		Inertia;	//惯性矩阵

double	*the;		//关节角度
double	*dThe;		//关节角速度
double  *ddThe;		//关节角加速度
double	*theRecord;	//关节角度存储变量

double posInt[N];    //起始位置
double posEnd[N];    //终了位置

double  *torq;       //力矩q
double  *torq0;      //力矩q0
double  *torC;       //力矩C
double  *torG;       //力矩G

int num;
double tSample;       //采样时间
double t;
double lastTime;      //持续时间

/////////////////////////////////////////////////////////

//
CMyView::CMyView()
{	
  register int i;
  int count;
	
  double ddPos[N];
  double kP, kV;

  //读入必要的数据
  inputData(&num, posEnd, &tSample, &lastTime, &kP, &kV);
  if(num != 3){
	exit(FAULT);
  }

  //确保所需变量的存储空间分配
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


  //关节参数的读入，关节姿势的初始化
  readArmParameters(num, &Arm);

  endPosition(Arm.Len, Arm.the0, posInt);
  for(i = 0; i < num; ++i){
	the[i] = Arm.the0[i];
	dThe[i] = 0.0;//初始速度为0
  }


  //循环计算的开始
  t = 0.0;
  count = 0;
  for(i=0; i<num; i++)
		torq0[i] = 0.0;

  do{

	  //前端的期望加速度
	demandInput(num, kP, kV, posInt, posEnd, Arm.Len, t, lastTime,
				the, dThe, ddPos);


	//雅克比矩阵及其时间微分的计算
	jacobianMatrix(Arm.Len, the, &Jaco);
	dJacobianMatrix(Arm.Len, the, dThe, &dJaco);

	//arm的转动惯量
	inertiaMatrix(Arm.LenG, Arm.Len, Arm.Marm, Arm.Iarm, the, &Inertia);


	//关节角加速度的计算
	if(jointRotateAcceleration(&Jaco, ddPos, ddThe) == FAULT)
	  exit(FAULT);

    //关节扭矩的计算
	jointTorqueC(Arm.LenG, Arm.Len, Arm.Marm, Arm.Iarm, the, dThe, torC);
	jointTorqueG(Arm.LenG, Arm.Len, Arm.Marm, the, torG);
	jointTorque(&Inertia, ddThe, torC, torG, torq);

	//下一步的时间设定
	t += tSample;
	for(i = 0; i < num; ++i)
		torq0[i] = torq[i];

	//仿真器的关节变量
	jointRotateVariables(tSample, &Inertia, torq, torC, torG, the, dThe, ddThe);

	//保存绘图用关节角度数据
	for(i=0; i<num; i++)
		theRecord[ count*num + i ] = the[i] * 180.0/PI;

	count++;

  } while(t < lastTime);

  //描绘姿势的初始化
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
	drawCoordinate( 1.0 );  //坐标轴的描述

	drawFloor(2); 	        //背景的描述
	//drawTrajectory(posInt, posEnd);     //前端轨迹的描述

	return TRUE;
}

/////////////////////////////////////////////////////////
//在这里描述移动的场景
BOOL CMyView::RenderScene()
{
	// 绘制手臂
	drawArm();		

	return TRUE;
}

// 绘制手臂
void CMyView::drawArm()
{
	////////////////////////////////////////////////////////////////////
	// 机器人各关节的坐标变换， 通过"D-H"方法得到的参数                                                               //
	//	joint   ai    αi    di    θi
	//	==================================
	//	  1     0      0     l0    θ1
	//	----------------------------------
	//	  1.5   0      0      l1    0
	//	----------------------------------
	//	  2     0     π/2    0    θ2
	//	----------------------------------
	//	  3     l2     0      0    θ3
	//	----------------------------------
	//	  H     l3     0      0     0
	//	----------------------------------


	GLdouble height, radius;
	int n_div = 16; 
	//设定分割的栏数，上限30

	//立体材质的定义

    //红色的
	GLfloat red_ambient[]  = { 0.2f, 0.0f, 0.0f }; //环境光
	GLfloat red_diffuse[]  = { 0.5f, 0.0f, 0.0f }; //扩散光
	GLfloat red_specular[] = { 0.7f, 0.6f, 0.6f }; //境面光
	GLfloat red_shininess[]= { 32.0f };			//  反射强度

    //绿色
	GLfloat green_ambient[]  = { 0.0f, 0.2f, 0.0f }; 
	GLfloat green_diffuse[]  = { 0.0f, 0.5f, 0.0f }; 
	GLfloat green_specular[] = { 0.6f, 0.7f, 0.6f }; 
	GLfloat green_shininess[]= { 32.0f };			

    //青色
	GLfloat blue_ambient[]  = { 0.0f, 0.0f, 0.2f }; 
	GLfloat blue_diffuse[]  = { 0.0f, 0.0f, 0.5f }; 
	GLfloat blue_specular[] = { 0.6f, 0.6f, 0.7f }; 
	GLfloat blue_shininess[]= { 32.0f };			

	//黄色
	GLfloat yellow_ambient[]  = { 0.2f, 0.2f, 0.0f }; 
	GLfloat yellow_diffuse[]  = { 0.2f, 0.2f, 0.0f }; 
	GLfloat yellow_specular[] = { 0.7f, 0.7f, 0.5f }; 
	GLfloat yellow_shininess[]= { 32.0f };			

	glPushMatrix();
 
		
	//link 0  基台部
	/////////////////////////////////////////////////////////////
	// link 0 

	//立体材质的定义：红
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	radius = 0.3;
	mySolidCylinder( radius, Arm.Len[0], n_div );

	/*
	函数void mySolidCylinder( double radius, double  height, int n_div);是
	z轴方向上的圆柱体描述函数，各个参数的意义如下：
		 radius : 半径
		 height : 高度
		 n_div:   圆筒的分割份数
	*/
	/////////////////////////////////////////////////////////////
	// joint 1

	// a1 = 0;
	// α1 = 0;

	glTranslated( 0.0, 0.0, Arm.Len[0] );	// D-H参数的d1
	glRotated( Arm.the0[0], 0, 0, 1 );		// D-H参数的θ1

	//立体材质的定义：黄色 
	glMaterialfv( GL_FRONT, GL_AMBIENT, yellow_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, yellow_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,yellow_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	radius = 0.25;
	height = 0.01;
	mySolidCylinder( radius, height, n_div );

	//相对坐标系的坐标轴描述
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// link 1   

	//立体材质的定义：绿色
	glMaterialfv( GL_FRONT, GL_AMBIENT, green_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, green_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,green_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, green_shininess );	

	radius = 0.2;
	mySolidCylinder( radius, Arm.Len[1], n_div );

	//横向的圆筒
	glPushMatrix();    // 画转动关节1
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
	glRotated( 90.0, 1.0, 0.0, 0.0);		//D-H参数的α2

	// d2 = 0;
	glRotated( Arm.the0[1], 0, 0, 1 );		//D-H参数的θ2

	glPushMatrix();
	radius = 0.21;
	height = radius;

	//立体材质的定义：青色
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );	

	glTranslated( 0.0, 0.0, -height/2.0 );
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	//相对坐标系的坐标轴描述
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// link 2

	//立体材质的定义:青色
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );	

	// link 2 杮懱      //link2本体
	glPushMatrix();
	radius = 0.15;
	glRotated( 90.0, 0.0, 1.0, 0.0 );
	mySolidCylinder( radius, Arm.Len[2], n_div );
	glPopMatrix();

	// link 2 忋抂晹偺幉庴偗   //link2上端部的支撑轴
	glPushMatrix();
	height = radius * 2.0;
	glTranslated( Arm.Len[2], 0.0, 0.0 );
	glTranslated( 0.0, 0.0, -height/2.0 );
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	/////////////////////////////////////////////////////////////
	// joint 3

	glTranslated( Arm.Len[2], 0.0, 0.0 ); // a3
	// α3 = 0
	// d3 = 0

	glRotated( Arm.the0[2], 0, 0, 1 );	// 兤3

	glPushMatrix();
	radius = 0.16;
	height = radius;

	// 棫懱偺嵽幙偺掕媊 : 愒   //立体材质的定义：红
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	glTranslated( 0.0, 0.0, -height/2.0 );  
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	// 憡懳嵗昗宯偺嵗昗幉傪昤偔  // 相对坐标系的坐标轴描述
	drawCoordinate(0.5);

	/////////////////////////////////////////////////////////////
	// link 3

	// 棫懱偺嵽幙偺掕媊 : 愒    // 立体材质的定义
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
	//  hand    //手
	glTranslated( Arm.Len[3], 0.0, 0.0 );

	//相对坐标系的坐标轴描述
	COpenGLView::drawCoordinate( 0.5 ); 		// 憡懳嵗昗宯偺嵗昗幉傪昤偔


	glPopMatrix();

}

// 巜椷婳摴傪昤偔     //指令轨道的描述
void CMyView::drawTrajectory(double *pInitial, double *pTarget)
{
	// 敀偄僾儔僗僥僢傿僋    //白色
	GLfloat white_ambient[]  = { 0.5f, 0.5f, 0.5f }; // 娐嫬怓//环境光
	GLfloat white_diffuse[]  = { 0.5f, 0.5f, 0.5f }; // 奼嶶怓//扩散光
	GLfloat white_specular[] = { 0.5f, 0.6f, 0.6f }; // 嬀柺怓//境面光
	GLfloat white_shininess[]= { 32.0f };			// 斀幩偺嫮偝//反射强度

	glPushMatrix();

	// 棫懱偺嵽幙偺掕媊 : 敀    //立体材质的定义
	glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, white_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );	

	// 巜椷婳摴傪攋慄偱昤偔     //指令轨迹的虚线描述
	glEnable( GL_LINE_STIPPLE );
	glLineStipple( 4, 0xAAAA );

	glBegin( GL_LINES );
	glVertex3dv( pInitial );
	glVertex3dv( pTarget );
	glEnd();

	glDisable( GL_LINE_STIPPLE );

	glPopMatrix();

}


// 傾僯儊乕僔儑儞梡僞僀儅  //animation time  动画用的时间
int count;
void CMyView::OnTimer(UINT nIDEvent)
{
	int i;

	// 娭愡妏搙偼攝楍偵曐懚偟偰偁傞偺偱丄偙傟傪屇傃弌偟偰巊梡偡傞丅
	//关节角度保存，这里再次调用
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

	// 夋柺偺嵞昤夋   //画面的再描绘
	InvalidateRect(FALSE);
	
	COpenGLView::OnTimer(nIDEvent);
}

// 彴偺昤夋    //床的描绘
void CMyView::drawFloor(double floorSize)
{
	int i;

	// 敀偄僾儔僗僥僢傿僋    //白色
	GLfloat white_ambient[]  = { 0.3f, 0.3f, 0.1f }; // 娐嫬怓//环境色
	GLfloat white_diffuse[]  = { 0.3f, 0.3f, 0.1f }; // 奼嶶怓//扩散色
	GLfloat white_specular[] = { 0.1f, 0.1f, 0.1f }; // 嬀柺怓//境面色
	GLfloat white_shininess[]= { 2.0f };			// 斀幩偺嫮偝//反射强度

	// 惵//青色
	GLfloat blue_ambient[]  = { 0.2f, 0.2f, 0.1f }; // 娐嫬怓 //环境色
	GLfloat blue_diffuse[]  = { 0.0f, 0.0f, 0.0f }; // 奼嶶怓  //扩散色
	GLfloat blue_specular[] = { 0.1f, 0.1f, 0.1f }; // 嬀柺怓   //境面色
	GLfloat blue_shininess[]= { 1.0f };			// 斀幩偺嫮偝   //反射强度

	glPushMatrix();

	// 棫懱偺嵽幙偺掕媊 : 敀//立体的材质定义：白
	glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,white_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );

	// 彴偺儔僀儞  //床上画线
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

	// 棫懱偺嵽幙偺掕媊 : 惵    //立体材质的定义：青色
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );

	// 彴斅     //床板
	glBegin(GL_QUADS);
	glVertex3d( floorSize, floorSize, 0.0 );
	glVertex3d(-floorSize, floorSize, 0.0 );
	glVertex3d(-floorSize,-floorSize, 0.0 );
	glVertex3d( floorSize,-floorSize, 0.0 );
	glEnd();

	glPopMatrix();
}

//////////////////////////////////
