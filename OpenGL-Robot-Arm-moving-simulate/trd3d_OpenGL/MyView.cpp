/////////////////////////////////////////////////////////
// 自作のOpenGL モデルを描く
//ﾗﾔﾗ�ｵﾄOpenGLﾄ｣ﾐﾍﾃ靆�
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



// 外部変数  
struct PARA Arm;
MAT		Jaco;		//ﾑﾅｿﾋｱﾈｾﾘﾕ�
MAT		dJaco;		//ﾑﾅｿﾋｱﾈｾﾘﾕ�ｵﾄﾊｱｼ萸｢ｷﾖ
MAT		Inertia;	//ｹﾟﾐﾔｾﾘﾕ�

double	*the;		//ｹﾘｽﾚｽﾇｶﾈ
double	*dThe;		//ｹﾘｽﾚｽﾇﾋﾙｶﾈ
double  *ddThe;		//ｹﾘｽﾚｽﾇｼﾓﾋﾙｶﾈ
double	*theRecord;	//ｹﾘｽﾚｽﾇｶﾈｴ豢｢ｱ菽ｿ

double posInt[N];    //ﾆ�ﾊｼﾎｻﾖﾃ
double posEnd[N];    //ﾖﾕﾁﾋﾎｻﾖﾃ

double  *torq;       //ﾁｦｾﾘq
double  *torq0;      //ﾁｦｾﾘq0
double  *torC;       //ﾁｦｾﾘC
double  *torG;       //ﾁｦｾﾘG

int num;
double tSample;       //ｲﾉﾑ�ﾊｱｼ�
double t;
double lastTime;      //ｳﾖﾐ�ﾊｱｼ�

/////////////////////////////////////////////////////////

//
CMyView::CMyView()
{	
  register int i;
  int count;
	
  double ddPos[N];
  double kP, kV;

  //ｶﾁﾈ�ｱﾘﾒｪｵﾄﾊ�ｾﾝ
  inputData(&num, posEnd, &tSample, &lastTime, &kP, &kV);
  if(num != 3){
	exit(FAULT);
  }

  //ﾈｷｱ｣ﾋ�ﾐ雎菽ｿｵﾄｴ豢｢ｿﾕｼ莵ﾖﾅ�
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


  //ｹﾘｽﾚｲﾎﾊ�ｵﾄｶﾁﾈ�｣ｬｹﾘｽﾚﾗﾋﾊﾆｵﾄｳ�ﾊｼｻｯ
  readArmParameters(num, &Arm);

  endPosition(Arm.Len, Arm.the0, posInt);
  for(i = 0; i < num; ++i){
	the[i] = Arm.the0[i];
	dThe[i] = 0.0;//ｳ�ﾊｼﾋﾙｶﾈﾎｪ0
  }


  //ﾑｭｻｷｼﾆﾋ羞ﾄｿｪﾊｼ
  t = 0.0;
  count = 0;
  for(i=0; i<num; i++)
		torq0[i] = 0.0;

  do{

	  //ﾇｰｶﾋｵﾄﾆﾚﾍ�ｼﾓﾋﾙｶﾈ
	demandInput(num, kP, kV, posInt, posEnd, Arm.Len, t, lastTime,
				the, dThe, ddPos);


	//ﾑﾅｿﾋｱﾈｾﾘﾕ�ｼｰﾆ萍ｱｼ萸｢ｷﾖｵﾄｼﾆﾋ�
	jacobianMatrix(Arm.Len, the, &Jaco);
	dJacobianMatrix(Arm.Len, the, dThe, &dJaco);

	//armｵﾄﾗｪｶｯｹﾟﾁｿ
	inertiaMatrix(Arm.LenG, Arm.Len, Arm.Marm, Arm.Iarm, the, &Inertia);


	//ｹﾘｽﾚｽﾇｼﾓﾋﾙｶﾈｵﾄｼﾆﾋ�
	if(jointRotateAcceleration(&Jaco, ddPos, ddThe) == FAULT)
	  exit(FAULT);

    //ｹﾘｽﾚﾅ､ｾﾘｵﾄｼﾆﾋ�
	jointTorqueC(Arm.LenG, Arm.Len, Arm.Marm, Arm.Iarm, the, dThe, torC);
	jointTorqueG(Arm.LenG, Arm.Len, Arm.Marm, the, torG);
	jointTorque(&Inertia, ddThe, torC, torG, torq);

	//ﾏﾂﾒｻｲｽｵﾄﾊｱｼ菲雜ｨ
	t += tSample;
	for(i = 0; i < num; ++i)
		torq0[i] = torq[i];

	//ｷﾂﾕ貳�ｵﾄｹﾘｽﾚｱ菽ｿ
	jointRotateVariables(tSample, &Inertia, torq, torC, torG, the, dThe, ddThe);

	//ｱ｣ｴ貊賚ｼﾓﾃｹﾘｽﾚｽﾇｶﾈﾊ�ｾﾝ
	for(i=0; i<num; i++)
		theRecord[ count*num + i ] = the[i] * 180.0/PI;

	count++;

  } while(t < lastTime);

  //ﾃ霆贐ﾋﾊﾆｵﾄｳ�ﾊｼｻｯ
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
	drawCoordinate( 1.0 );  //ﾗ�ｱ�ﾖ盞ﾄﾃ靆�

	drawFloor(2); 	        //ｱｳｾｰｵﾄﾃ靆�
	//drawTrajectory(posInt, posEnd);     //ﾇｰｶﾋｹ�ｼ｣ｵﾄﾃ靆�

	return TRUE;
}

/////////////////////////////////////////////////////////
//ﾔﾚﾕ簑�ﾃ靆�ﾒﾆｶｯｵﾄｳ｡ｾｰ
BOOL CMyView::RenderScene()
{
	// ｻ贍ﾆﾊﾖｱﾛ
	drawArm();		

	return TRUE;
}

// ｻ贍ﾆﾊﾖｱﾛ
void CMyView::drawArm()
{
	////////////////////////////////////////////////////////////////////
	// ｻ�ﾆ�ﾈﾋｸ�ｹﾘｽﾚｵﾄﾗ�ｱ�ｱ莉ｻ｣ｬ ﾍｨｹ�"D-H"ｷｽｷｨｵﾃｵｽｵﾄｲﾎﾊ�                                                               //
	//	joint   ai    ｦﾁi    di    ｦﾈi
	//	==================================
	//	  1     0      0     l0    ｦﾈ1
	//	----------------------------------
	//	  1.5   0      0      l1    0
	//	----------------------------------
	//	  2     0     ｦﾐ/2    0    ｦﾈ2
	//	----------------------------------
	//	  3     l2     0      0    ｦﾈ3
	//	----------------------------------
	//	  H     l3     0      0     0
	//	----------------------------------


	GLdouble height, radius;
	int n_div = 16; 
	//ﾉ雜ｨｷﾖｸ�ｵﾄﾀｸﾊ�｣ｬﾉﾏﾏﾞ30

	//ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ�

    //ｺ�ﾉｫｵﾄ
	GLfloat red_ambient[]  = { 0.2f, 0.0f, 0.0f }; //ｻｷｾｳｹ�
	GLfloat red_diffuse[]  = { 0.5f, 0.0f, 0.0f }; //ﾀｩﾉ｢ｹ�
	GLfloat red_specular[] = { 0.7f, 0.6f, 0.6f }; //ｾｳﾃ貉�
	GLfloat red_shininess[]= { 32.0f };			//  ｷｴﾉ萇ｿｶﾈ

    //ﾂﾌﾉｫ
	GLfloat green_ambient[]  = { 0.0f, 0.2f, 0.0f }; 
	GLfloat green_diffuse[]  = { 0.0f, 0.5f, 0.0f }; 
	GLfloat green_specular[] = { 0.6f, 0.7f, 0.6f }; 
	GLfloat green_shininess[]= { 32.0f };			

    //ﾇ猖ｫ
	GLfloat blue_ambient[]  = { 0.0f, 0.0f, 0.2f }; 
	GLfloat blue_diffuse[]  = { 0.0f, 0.0f, 0.5f }; 
	GLfloat blue_specular[] = { 0.6f, 0.6f, 0.7f }; 
	GLfloat blue_shininess[]= { 32.0f };			

	//ｻﾆﾉｫ
	GLfloat yellow_ambient[]  = { 0.2f, 0.2f, 0.0f }; 
	GLfloat yellow_diffuse[]  = { 0.2f, 0.2f, 0.0f }; 
	GLfloat yellow_specular[] = { 0.7f, 0.7f, 0.5f }; 
	GLfloat yellow_shininess[]= { 32.0f };			

	glPushMatrix();
 
		
	//link 0  ｻ�ﾌｨｲｿ
	/////////////////////////////////////////////////////////////
	// link 0 

	//ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ螢ｺｺ�
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	radius = 0.3;
	mySolidCylinder( radius, Arm.Len[0], n_div );

	/*
	ｺｯﾊ�void mySolidCylinder( double radius, double  height, int n_div);ﾊﾇ
	zﾖ盥ｽﾏ�ﾉﾏｵﾄﾔｲﾖ�ﾌ蠹靆�ｺｯﾊ�｣ｬｸ�ｸ�ｲﾎﾊ�ｵﾄﾒ簫衒醯ﾂ｣ｺ
		 radius : ｰ�ｾｶ
		 height : ｸﾟｶﾈ
		 n_div:   ﾔｲﾍｲｵﾄｷﾖｸ�ｷﾝﾊ�
	*/
	/////////////////////////////////////////////////////////////
	// joint 1

	// a1 = 0;
	// ｦﾁ1 = 0;

	glTranslated( 0.0, 0.0, Arm.Len[0] );	// D-Hｲﾎﾊ�ｵﾄd1
	glRotated( Arm.the0[0], 0, 0, 1 );		// D-Hｲﾎﾊ�ｵﾄｦﾈ1

	//ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ螢ｺｻﾆﾉｫ 
	glMaterialfv( GL_FRONT, GL_AMBIENT, yellow_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, yellow_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,yellow_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	radius = 0.25;
	height = 0.01;
	mySolidCylinder( radius, height, n_div );

	//ﾏ犖ﾔﾗ�ｱ�ﾏｵｵﾄﾗ�ｱ�ﾖ眦靆�
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// link 1   

	//ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ螢ｺﾂﾌﾉｫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, green_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, green_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,green_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, green_shininess );	

	radius = 0.2;
	mySolidCylinder( radius, Arm.Len[1], n_div );

	//ｺ睹�ｵﾄﾔｲﾍｲ
	glPushMatrix();    // ｻｭﾗｪｶｯｹﾘｽﾚ1
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
	glRotated( 90.0, 1.0, 0.0, 0.0);		//D-Hｲﾎﾊ�ｵﾄｦﾁ2

	// d2 = 0;
	glRotated( Arm.the0[1], 0, 0, 1 );		//D-Hｲﾎﾊ�ｵﾄｦﾈ2

	glPushMatrix();
	radius = 0.21;
	height = radius;

	//ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ螢ｺﾇ猖ｫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );	

	glTranslated( 0.0, 0.0, -height/2.0 );
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	//ﾏ犖ﾔﾗ�ｱ�ﾏｵｵﾄﾗ�ｱ�ﾖ眦靆�
	COpenGLView::drawCoordinate( 0.5 );

	/////////////////////////////////////////////////////////////
	// link 2

	//ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ�:ﾇ猖ｫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );	

	// link 2 本体      //link2ｱｾﾌ�
	glPushMatrix();
	radius = 0.15;
	glRotated( 90.0, 0.0, 1.0, 0.0 );
	mySolidCylinder( radius, Arm.Len[2], n_div );
	glPopMatrix();

	// link 2 上端部の軸受け   //link2ﾉﾏｶﾋｲｿｵﾄﾖｧｳﾅﾖ�
	glPushMatrix();
	height = radius * 2.0;
	glTranslated( Arm.Len[2], 0.0, 0.0 );
	glTranslated( 0.0, 0.0, -height/2.0 );
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	/////////////////////////////////////////////////////////////
	// joint 3

	glTranslated( Arm.Len[2], 0.0, 0.0 ); // a3
	// ｦﾁ3 = 0
	// d3 = 0

	glRotated( Arm.the0[2], 0, 0, 1 );	// θ3

	glPushMatrix();
	radius = 0.16;
	height = radius;

	// 立体の材質の定義 : 赤   //ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ螢ｺｺ�
	glMaterialfv( GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, red_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,red_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, red_shininess );	

	glTranslated( 0.0, 0.0, -height/2.0 );  
	mySolidCylinder( radius, height , n_div );
	glPopMatrix();

	// 相対座標系の座標軸を描く  // ﾏ犖ﾔﾗ�ｱ�ﾏｵｵﾄﾗ�ｱ�ﾖ眦靆�
	drawCoordinate(0.5);

	/////////////////////////////////////////////////////////////
	// link 3

	// 立体の材質の定義 : 赤    // ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ�
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
	//  hand    //ﾊﾖ
	glTranslated( Arm.Len[3], 0.0, 0.0 );

	//ﾏ犖ﾔﾗ�ｱ�ﾏｵｵﾄﾗ�ｱ�ﾖ眦靆�
	COpenGLView::drawCoordinate( 0.5 ); 		// 相対座標系の座標軸を描く


	glPopMatrix();

}

// 指令軌道を描く     //ﾖｸﾁ�ｹ�ｵﾀｵﾄﾃ靆�
void CMyView::drawTrajectory(double *pInitial, double *pTarget)
{
	// 白いプラステッィク    //ｰﾗﾉｫ
	GLfloat white_ambient[]  = { 0.5f, 0.5f, 0.5f }; // 環境色//ｻｷｾｳｹ�
	GLfloat white_diffuse[]  = { 0.5f, 0.5f, 0.5f }; // 拡散色//ﾀｩﾉ｢ｹ�
	GLfloat white_specular[] = { 0.5f, 0.6f, 0.6f }; // 鏡面色//ｾｳﾃ貉�
	GLfloat white_shininess[]= { 32.0f };			// 反射の強さ//ｷｴﾉ萇ｿｶﾈ

	glPushMatrix();

	// 立体の材質の定義 : 白    //ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ�
	glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, white_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );	

	// 指令軌道を破線で描く     //ﾖｸﾁ�ｹ�ｼ｣ｵﾄﾐ鰕ﾟﾃ靆�
	glEnable( GL_LINE_STIPPLE );
	glLineStipple( 4, 0xAAAA );

	glBegin( GL_LINES );
	glVertex3dv( pInitial );
	glVertex3dv( pTarget );
	glEnd();

	glDisable( GL_LINE_STIPPLE );

	glPopMatrix();

}


// アニメーション用タイマ  //animation time  ｶｯｻｭﾓﾃｵﾄﾊｱｼ�
int count;
void CMyView::OnTimer(UINT nIDEvent)
{
	int i;

	// 関節角度は配列に保存してあるので、これを呼び出して使用する。
	//ｹﾘｽﾚｽﾇｶﾈｱ｣ｴ譽ｬﾕ簑�ﾔﾙｴﾎｵ�ﾓﾃ
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

	// 画面の再描画   //ｻｭﾃ豬ﾄﾔﾙﾃ霆�
	InvalidateRect(FALSE);
	
	COpenGLView::OnTimer(nIDEvent);
}

// 床の描画    //ｴｲｵﾄﾃ霆�
void CMyView::drawFloor(double floorSize)
{
	int i;

	// 白いプラステッィク    //ｰﾗﾉｫ
	GLfloat white_ambient[]  = { 0.3f, 0.3f, 0.1f }; // 環境色//ｻｷｾｳﾉｫ
	GLfloat white_diffuse[]  = { 0.3f, 0.3f, 0.1f }; // 拡散色//ﾀｩﾉ｢ﾉｫ
	GLfloat white_specular[] = { 0.1f, 0.1f, 0.1f }; // 鏡面色//ｾｳﾃ賈ｫ
	GLfloat white_shininess[]= { 2.0f };			// 反射の強さ//ｷｴﾉ萇ｿｶﾈ

	// 青//ﾇ猖ｫ
	GLfloat blue_ambient[]  = { 0.2f, 0.2f, 0.1f }; // 環境色 //ｻｷｾｳﾉｫ
	GLfloat blue_diffuse[]  = { 0.0f, 0.0f, 0.0f }; // 拡散色  //ﾀｩﾉ｢ﾉｫ
	GLfloat blue_specular[] = { 0.1f, 0.1f, 0.1f }; // 鏡面色   //ｾｳﾃ賈ｫ
	GLfloat blue_shininess[]= { 1.0f };			// 反射の強さ   //ｷｴﾉ萇ｿｶﾈ

	glPushMatrix();

	// 立体の材質の定義 : 白//ﾁ｢ﾌ蠏ﾄｲﾄﾖﾊｶｨﾒ螢ｺｰﾗ
	glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR,white_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );

	// 床のライン  //ｴｲﾉﾏｻｭﾏﾟ
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

	// 立体の材質の定義 : 青    //ﾁ｢ﾌ蟯ﾄﾖﾊｵﾄｶｨﾒ螢ｺﾇ猖ｫ
	glMaterialfv( GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, blue_shininess );

	// 床板     //ｴｲｰ�
	glBegin(GL_QUADS);
	glVertex3d( floorSize, floorSize, 0.0 );
	glVertex3d(-floorSize, floorSize, 0.0 );
	glVertex3d(-floorSize,-floorSize, 0.0 );
	glVertex3d( floorSize,-floorSize, 0.0 );
	glEnd();

	glPopMatrix();
}

//////////////////////////////////
