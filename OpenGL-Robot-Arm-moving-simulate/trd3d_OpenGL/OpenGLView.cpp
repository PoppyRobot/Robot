// OpenGLView.cpp : COpenGLView 僋儔僗偺摦嶌偺掕媊傪峴偄傑偡丅
//

#include "stdafx.h"
#include "OpenGL.h"

#include <math.h>
#include "OpenGLDoc.h"
#include "OpenGLView.h"
#include "Protocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MY_PROTOCOL;


/////////////////////////////////////////////////////////////////////////////
// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, CView)

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	//{{AFX_MSG_MAP(COpenGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND(ID_ANIMATION_START, OnAnimationStart)
	ON_COMMAND(ID_ANIMATION_STOP, OnAnimationStop)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_COORDINATE_ON, OnMenuCoordinateOn)
	ON_COMMAND(ID_MENU_COORDINATE_OFF, OnMenuCoordinateOff)
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_MENU_COORDINATE_OFF, OnUpdateMenuCoordinateOff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLView 僋儔僗偺峔抸/徚柵
//COpenGLView类的构造和析构

COpenGLView::COpenGLView() :
  m_hRC(0), m_pDC(0), m_ErrorStrings(_ErrorStrings[0])
{
	// TODO: 偙偺応強偵峔抸梡偺僐乕僪傪捛壛偟偰偔偩偝偄丅
 //默认情况下，不显示坐标轴
	  showCoordinate = FALSE;  // 僨僼僅儖僩偱偼嵗昗宯傪昞帵偟側偄
	 
	rotX = 0.0f;
	rotZ = 0.0f;

}

COpenGLView::~COpenGLView()
{
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 偙偺埵抲偱 CREATESTRUCT cs 傪廋惓偟偰 Window 僋儔僗傑偨偼僗僞僀儖傪
	//  廋惓偟偰偔偩偝偄丅

	cs.style |=  WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView 僋儔僗偺昤夋

void COpenGLView::OnDraw(CDC* pDC)
{
	COpenGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: 偙偺応強偵僱僀僥傿僽 僨乕僞梡偺昤夋僐乕僪傪捛壛偟傑偡丅
    //Buffer to be Cleared  The buffers currently enabled for color writing   
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	//PreRenderScene();

	glPushMatrix();//将当前矩阵压入矩阵堆栈

		// 帇揰堏摦偺偮傕傝 (-_-;
	//视点移动
		glRotatef( rotX, 1.0f, 0.0f, 0.0f );
		glRotatef( rotZ, 0.0f, 0.0f, 1.0f );
		{
		  ::glPushMatrix();

		  //在这里绘制静止物体
		  RenderStockScene();	// 惷揑側晹昳傪偙偙偱昤偔

		  ::glPopMatrix();//将当前矩阵弹出矩阵堆栈
		}
		{
		  ::glPushMatrix();

		  //在这里绘制移动物体
		  RenderScene();		// 摦揑側晹昳傪偙偙偱昤偔

		  ::glPopMatrix();
		}
	
	glPopMatrix();

	::glFinish();		// 偡傋偰偺OpenGL僐乕儖偑姰椆偟偨偙偲傪抦傜偣傞

	if( FALSE == ::SwapBuffers( m_pDC->GetSafeHdc() ) )
		SetError(7);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView 僋儔僗偺恌抐

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenGLDoc* COpenGLView::GetDocument() // 旕僨僶僢僌 僶乕僕儑儞偼僀儞儔僀儞偱偡丅
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLDoc)));
	return (COpenGLDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLView 僋儔僗偺儊僢僙乕僕 僴儞僪儔

int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 偙偺埵抲偵屌桳偺嶌惉梡僐乕僪傪捛壛偟偰偔偩偝偄
	
	InitalizeOpenGL();

	return 0;
}

void COpenGLView::OnDestroy() 
{
	CView::OnDestroy();
	
	KillTimer(1);

	// TODO: 偙偺埵抲偵儊僢僙乕僕 僴儞僪儔梡偺僐乕僪傪捛壛偟偰偔偩偝偄
	if( FALSE == ::wglMakeCurrent(0, 0) )
		SetError(2);
	if( FALSE == ::wglDeleteContext( m_hRC ) )
		SetError(6);

	if( m_pDC )
		delete m_pDC;
	
}

BOOL COpenGLView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: 偙偺埵抲偵儊僢僙乕僕 僴儞僪儔梡偺僐乕僪傪捛壛偡傞偐傑偨偼僨僼僅儖僩偺張棟傪屇傃弌偟偰偔偩偝偄
	
	// comment out original call
	//return CView::OnEraseBkgnd(pDC);
	return TRUE; // tell Windows not to erase the background
}

void COpenGLView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: 偙偺埵抲偵儊僢僙乕僕 僴儞僪儔梡偺僐乕僪傪捛壛偟偰偔偩偝偄

	GLdouble aspect_ratio;

	if( 0 >= cx || 0 >= cy )
		return;

	SetupViewport( cx, cy );

	// compute the aspect ratio
	aspect_ratio = (GLdouble)cx / (GLdouble)cy;

	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	//The glLoadIdentity 
	// function replaces the current matrix with the identity matrix

	SetupViewingFrustum( aspect_ratio );
	SetupViewingTransform();

	::glMatrixMode(GL_MODELVIEW);
	//Applies subsequent matrix operations to the modelview matrix stack
	::glLoadIdentity();

	SetupLighting();
}

const char* const COpenGLView::_ErrorStrings[]= {
	{"No Error"}, // 0
	{"Unable to get a DC"}, // 1
	{"ChoosePixelFormat failed"}, // 2
	{"SelectPixelFormat failed"}, // 3
	{"wglCreateContext failed"},  // 4
	{"wglMakeCurrent failed"},    // 5
	{"wglDeleteContext failed"},  // 6
	{"SwapBuffers failed"}, // 7
};

void COpenGLView::SetError(int e)
{
	// if there was no privious error,
	// then save this one
	if( _ErrorStrings[0] == m_ErrorStrings )
		m_ErrorStrings = _ErrorStrings[e];

}

BOOL COpenGLView::InitalizeOpenGL()
{
	m_pDC = new CClientDC(this);

	if( NULL == m_pDC ){ // failed to get DC
		SetError(1);
		return FALSE;
	}
	if( !SetupPixelFormat() )
		return FALSE;
	if( 0 == (m_hRC = 
		::wglCreateContext( m_pDC->GetSafeHdc() ) ) ){
		SetError(4);
		return FALSE;
	}
	if( FALSE ==
		::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC )  ){
		SetError(5);
		return FALSE;
	}

	// specify black as clear color
	::glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);

	// specify the back of the buffer as clear depth
	::glClearDepth( 1.0f );

	// enable depth test
	::glEnable( GL_DEPTH_TEST );

	return TRUE;
}

BOOL COpenGLView::SetupPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | //The buffer can draw to a window or device surface
			PFD_SUPPORT_OPENGL |//The buffer supports OpenGL drawing.
			PFD_DOUBLEBUFFER,//The buffer is double-buffered. This flag and PFD_SUPPORT_GDI are mutually exclusive in the current generic implementation.
		PFD_TYPE_RGBA,//RGBA pixels. Each pixel has four components in this order: red, green, blue, and alpha.
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixelformat;

	if( 0 == (pixelformat =
		::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd))  )
	{
		SetError(2);
		return FALSE;
	}

	if( FALSE == ::SetPixelFormat( m_pDC->GetSafeHdc(),
		pixelformat, &pfd) )
	{
		SetError(3);
		return FALSE;
	}

	return TRUE;

}


BOOL COpenGLView::SetupViewport( int cx, int cy )
{
	// select the full client area
	//The width and height, respectively, of the viewport.
	//When an OpenGL context is first attached to a window,
	//width and height are set to the dimensions of that window. 

	glViewport(0, 0, cx, cy);

	return TRUE;
}
//设置视见空间的平截头体
BOOL COpenGLView::SetupViewingFrustum(GLdouble aspect_ratio)
{
	// select a default viewing volume
	gluPerspective( 40.0f, aspect_ratio, 0.1f, 20.0f );

	return TRUE;
}

BOOL COpenGLView::SetupViewingTransform()
{
	int i;

	GLfloat fovy = 30.0; // 夋妏//画角
	GLfloat eye[3];
	GLfloat center[3] = { 0.0f, 0.0f, 1.0f };
	GLfloat eye_dir[3];
	GLfloat up[3];
	GLfloat norm, dist;

	eye[0] = 0.0;
	eye[1] = 5.0;
	eye[2] = 2.0;

	for(i=0; i<3; i++)
		eye_dir[i] = center[i] - eye[i];
	dist = (GLfloat)sqrt( eye_dir[0]*eye_dir[0] + eye_dir[1]*eye_dir[1] + eye_dir[2]*eye_dir[2] );
	for(i=0; i<3; i++)
		eye_dir[i] /= (GLfloat)dist;

	// 忋岦偒儀僋僩儖偺愝掕
	//向上的方向向量的设定
	up[0] = -eye_dir[0] * eye_dir[2];
	up[1] = -eye_dir[1] * eye_dir[2];
	up[2] = eye_dir[0] * eye_dir[0] + eye_dir[1] * eye_dir[1];
	norm = up[1]*up[1] + up[1]*up[1] + up[2]*up[2];
	norm = (GLfloat)sqrt(norm);
	for(i=0; i<3; i++)
		up[i] /= norm;


	gluLookAt( eye[0], eye[1],    eye[2],
			center[0], center[1], center[2],
			up[0],     up[1],     up[2]);

	return TRUE;
}

BOOL COpenGLView::SetupLighting()
{
	// 徠柧偺愝掕
	//设置光照
	GLfloat model_ambient[] = { 2.0f, 2.0f, 2.0f, 1.0f };
	GLfloat light_position0[]= { 1.0f, 0.0f, 5.0f, 0.0f };
	GLfloat light_color0[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 娐嫬岝
	//环境光
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, model_ambient );

	// 儔僀僩偺埵抲
	//光源位置
	glLightfv( GL_LIGHT0, GL_POSITION, light_position0 );

	// 儔僀僩偺怓
	//光源颜色
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light_color0 );

	// 岝尮 ON
	//打开光源
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	return TRUE;
}

// 摦揑側応柺傪昤偔
//移动场景的描绘
BOOL COpenGLView::RenderScene()
{
	// 愒偄儃乕儖偲惵偄棫曽懱傪昤偔 //红色和青色立方体的描绘
    //绕向量(1,0,0)旋转90度
	::glRotatef( 90.0f, 1.0f, 0.0f, 0.0f);

	//设置当前颜色为红色
	::glColor3f( 1.0f, 0.0f, 0.0f);
	//
	::auxWireSphere(0.5);
	::glColor3f( 0.5f, 0.5f, 1.0f );
	::auxWireCube( 1.0 );

	return TRUE;
}

// 惷揑側応柺傪昤偔
//静止场景的描绘
BOOL COpenGLView::RenderStockScene()
{
	// 僠僃僢僇乕僼儔僢僌傪昤偔
	return TRUE;
}


void COpenGLView::OnAnimationStart() 
{
	// TODO: 偙偺埵抲偵僐儅儞僪 僴儞僪儔梡偺僐乕僪傪捛壛偟偰偔偩偝偄

	// 傾僯儊乕僔儑儞梡偺僞僀儅傪僙僢僩
	//设置动画时间
	SetTimer(1, //Specifies a nonzero timer identifier
		42, //Specifies the time-out value, in milliseconds
		NULL);//Specifies the address of the application-supplied TimerProc 
	     //callback function that processes the WM_TIMER messages.
	    //If this parameter is NULL, the WM_TIMER messages are 
	    //placed in the application's message queue and handled by 
	    //the CWnd object
}

void COpenGLView::OnAnimationStop() 
{
	// TODO: 偙偺埵抲偵僐儅儞僪 僴儞僪儔梡偺僐乕僪傪捛壛偟偰偔偩偝偄
	
	// 僞僀儅傪嶍彍
	KillTimer(1);//The KillTimer function destroys the specified timer. 
	//删除定时器1
}

void COpenGLView::OnTimer(UINT nIDEvent) 
{
	// TODO: 偙偺埵抲偵儊僢僙乕僕 僴儞僪儔梡偺僐乕僪傪捛壛偡傞偐傑偨偼僨僼僅儖僩偺張棟傪屇傃弌偟偰偔偩偝偄

	CView::OnTimer(nIDEvent);
}
//根据*p0,*p1,*p2计算法向量*normal
void COpenGLView::CalcNormal(GLfloat *p0, GLfloat *p1, GLfloat *p2, GLfloat *normal)
{

	int i;
	GLfloat norm;
	GLfloat v0[3], v1[3];

	// 俀杮偺儀僋僩儖傪嶌傞
	for(i=0; i<3; i++){
		v0[i] = p2[i] - p1[i];
		v1[i] = p0[i] - p1[i];
	}

	// 慖傜傟偨儀僋僩儖偺奜愊傪媮傔傞
	normal[0] = v0[1] * v1[2] - v0[2] * v1[1];
	normal[1] = v0[2] * v1[0] - v0[0] * v1[2];
	normal[2] = v0[0] * v1[1] - v0[1] * v1[0];
	// 朄慄儀僋僩儖偼昁偢惓婯壔偟偰偍偔
	//法线的正规化，也就是单位化
	norm = (GLfloat)sqrt( (double)(normal[0] * normal[0]
									+ normal[1] * normal[1]
									+ normal[2] * normal[2]) );
	for(i=0; i<3; i++)
		normal[i] /= norm;

}


#define NDIV_MAX	30
// 惓 n_div 妏拰傪昤偔娭悢   //正多边形描述函数
// 僌儘乕僔僃乕僨傿儞僌傪梡偄偰墌拰傪昤偔
void COpenGLView::mySolidCylinder(GLdouble radius, GLdouble height, int n_div)
{
	int i;
	double angle;

	GLfloat x, y;
	GLfloat top_poly[NDIV_MAX][3], bottom_poly[NDIV_MAX][3];
	GLfloat nv[3];

    #if !defined PI
     #define PI 3.14159
    #endif

	// 揤堜偲掙偺惓 n_div 妏宍傪掕媊
	//定义上下两面的正n_div多边形
	for(i=0; i<n_div; i++){
		angle = (2.0 * PI * i) / n_div;
		x = (GLfloat)cos(angle);
		y = (GLfloat)sin(angle);

		// 忋晹偺惓懡妏宍偺捀揰傪忋偐傜尒偰斀帪寁廃傝偵掕媊
		//上部正多边形定点的逆时针定义
		top_poly[i][0] = (GLfloat)radius * x;
		top_poly[i][1] = (GLfloat)radius * y;
		top_poly[i][2] = (GLfloat)height;

		// 壓晹偺惓懡妏宍偺捀揰傪壓偐傜尒偰斀帪寁廃傝偵掕媊
		//下部正多边形定点的逆时针定义
		bottom_poly[i][0] = (GLfloat)radius * x;
		bottom_poly[i][1] = (GLfloat)radius * y;
		bottom_poly[i][2] = 0.0;
	}

	// 忋晹偺惓懡妏宍偺昤夋
	//上部正多边形的描述
	glBegin(GL_POLYGON);

		// 昁偢柺偺朄慄傪梌偊傞
	    //给出该表面的法向向量
		CalcNormal(top_poly[0], top_poly[1], top_poly[2], nv );
		glNormal3fv(nv);

		// 捀揰偺嵗昗傪斀帪寁廃傝偵巜帵偡傞
		//以逆时针方向表示出各定点坐标
		for(i=0; i<n_div; i++)
			glVertex3fv(top_poly[i]);
	glEnd();

	// 懁柺偺昤夋丅 巐曈宍偺楢懕偲偟偰埖偆
	//侧面的描绘，四边形的连续处理
	glBegin(GL_QUAD_STRIP);
	    for(i=0; i<=n_div; i++){
			nv[0] = top_poly[i % n_div][0] / (GLfloat)radius;//注意这里的%的作用很巧妙
			nv[1] = top_poly[i % n_div][1] / (GLfloat)radius;
			nv[2] = 0.0;
			glNormal3fv(nv);
			glVertex3fv(bottom_poly[i % n_div]);
			glVertex3fv(top_poly[i % n_div]);
		}
	glEnd();

	// 壓晹偺惓懡妏宍偺昤夋
	//下部的正多边形描绘
	glBegin(GL_POLYGON);

		// 昁偢柺偺朄慄傪梌偊傞
	   //给出该表面的法线向量
		CalcNormal(bottom_poly[0], bottom_poly[1], bottom_poly[2], nv );
		glNormal3fv(nv);

		for(i=(n_div-1); i>=0; i--)
			glVertex3fv(bottom_poly[i]);
	glEnd();
}

// 尰嵼偺嵗昗宯傪昤偔偨傔偺僗僀僢僠
//打开坐标描述标志位
void COpenGLView::OnMenuCoordinateOn() 
{
	// TODO: 偙偺埵抲偵僐儅儞僪 僴儞僪儔梡偺僐乕僪傪捛壛偟偰偔偩偝偄
	showCoordinate = TRUE;
	Invalidate();
}
//关闭坐标描述位
void COpenGLView::OnMenuCoordinateOff() 
{
	// TODO: 偙偺埵抲偵僐儅儞僪 僴儞僪儔梡偺僐乕僪傪捛壛偟偰偔偩偝偄
	showCoordinate = FALSE;
	Invalidate();
}

// 嵗昗宯傪昤偔
//坐标系的描绘
void COpenGLView::drawCoordinate(double size)
{
	GLdouble ovec[] = { 0.0, 0.0, 0.0 };
	GLdouble xvec[] = { size, 0.0, 0.0 };
	GLdouble yvec[] = { 0.0, size, 0.0 };
	GLdouble zvec[] = { 0.0, 0.0, size };

	GLdouble radius = size / 30; //箭头半径
	GLdouble height = size / 15; //箭头长度

	// 敀偄僾儔僗僥僢傿僋
    //白色的。。。

	GLfloat white_ambient[]  = { 0.5f, 0.5f, 0.5f }; // 娐嫬怓//环境色
	GLfloat white_diffuse[]  = { 0.5f, 0.5f, 0.5f }; // 奼嶶怓//扩散色
	GLfloat white_specular[] = { 0.5f, 0.6f, 0.6f }; // 嬀柺怓//经面色
	GLfloat white_shininess[]= { 32.0f };			// 斀幩偺嫮偝//反射强度

	glEnable(GL_COLOR_MATERIAL);
	

	// 显示向量坐标
	if ( showCoordinate == TRUE )
	{
		// 棫懱偺嵽幙偺掕媊 : 敀
	   //立体的材质定义：白
		glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
		glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
		glMaterialfv( GL_FRONT, GL_SPECULAR, white_specular);
		glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );	
       
		/*************************************************
		RGB: 238 210 160
		计算方法：
		238 / 255 = 0.93
		210 / 255 = 0.82
		160 / 255 = 0.63
		设置 glColor3f(0.93f , 0.82f , 0.63f)就差不多了 
		**************************************************/
		//*
		// x 幉    //x轴
		glColor3f(1.0f, 0.0f, 0.0f);  // 红色RGB(255, 0, 0)
		glBegin( GL_LINES );
			glVertex3dv( ovec );
			glVertex3dv( xvec );
		glEnd();
		glPushMatrix();
			glTranslated( size, 0.0, 0.0 );
			glRotated( 90.0, 0.0, 1.0, 0.0 );
			glutSolidCone( radius, height,10,10 );
		glPopMatrix();

		// y 幉   /y轴
		glColor3f(0.0f, 1.0f, 0.0f);   // 绿色RGB(0, 255, 0)
		glBegin( GL_LINES );
			glVertex3dv( ovec );
			glVertex3dv( yvec );
		glEnd();
		glPushMatrix();
			glTranslated( 0.0, size, 0.0 );
			glRotated( -90.0, 1.0, 0.0, 0.0 );
			glutSolidCone( radius, height,10,10 );
		glPopMatrix();
		//*/

		// z 幉    //z轴
		glColor3f(0.0f, 0.0f, 1.0f);  // 蓝色RGB(0, 0, 255)
		glBegin( GL_LINES );
			glVertex3dv( ovec );
			glVertex3dv( zvec );
		glEnd();
		glPushMatrix();
			glTranslated( 0.0, 0.0, size );
			glutSolidCone( radius, height, 10, 10 );
		glPopMatrix();
	}
	glDisable(GL_COLOR_MATERIAL);

}

void COpenGLView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: 偙偺埵抲偵儊僢僙乕僕 僴儞僪儔梡偺僐乕僪傪捛壛偡傞偐傑偨偼僨僼僅儖僩偺張棟傪屇傃弌偟偰偔偩偝偄
	
	float drot = 5.0f;

	switch (nChar)
	{
	case VK_HOME:
		rotX = rotZ = 0.0f;
		break;

	case VK_RIGHT:
		rotZ -= drot;
		break;

	case VK_LEFT:
		rotZ += drot;
		break;

	case VK_UP:
		rotX -= drot;
		break;

	case VK_DOWN:
		rotX += drot;
		break;

	}

	Invalidate(TRUE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COpenGLView::OnUpdateMenuCoordinateOff(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}


void COpenGLView::Up()
{
	
	//OnKeyDown(VK_UP, 0, 0);
}

void COpenGLView::Left()
{
	OnKeyDown(VK_LEFT, 0, 0);
}

void COpenGLView::ProcessCmd(char *buffer)
{
	if (NULL == buffer)
	{
		return;
	}
	
	_st_Data *p_data = (_st_Data*)buffer;
	if (p_data == NULL)
	{
		return;
	}

	switch (p_data->operator_id)
	{
	case EOP_DOWN:
		break;
	case EOP_UP:
		break;
	}
}