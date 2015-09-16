// OpenGLView.cpp : COpenGLView �N���X�̓���̒�`���s���܂��B
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
// COpenGLView �N���X�̍\�z/����
//COpenGLView��Ĺ��������

COpenGLView::COpenGLView() :
  m_hRC(0), m_pDC(0), m_ErrorStrings(_ErrorStrings[0])
{
	// TODO: ���̏ꏊ�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
 //Ĭ������£�����ʾ������
	  showCoordinate = FALSE;  // �f�t�H���g�ł͍��W�n��\�����Ȃ�
	 
	rotX = 0.0f;
	rotZ = 0.0f;

}

COpenGLView::~COpenGLView()
{
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	cs.style |=  WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView �N���X�̕`��

void COpenGLView::OnDraw(CDC* pDC)
{
	COpenGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: ���̏ꏊ�Ƀl�C�e�B�u �f�[�^�p�̕`��R�[�h��ǉ����܂��B
    //Buffer to be Cleared  The buffers currently enabled for color writing   
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	//PreRenderScene();

	glPushMatrix();//����ǰ����ѹ������ջ

		// ���_�ړ��̂��� (-_-;
	//�ӵ��ƶ�
		glRotatef( rotX, 1.0f, 0.0f, 0.0f );
		glRotatef( rotZ, 0.0f, 0.0f, 1.0f );
		{
		  ::glPushMatrix();

		  //��������ƾ�ֹ����
		  RenderStockScene();	// �ÓI�ȕ��i�������ŕ`��

		  ::glPopMatrix();//����ǰ���󵯳������ջ
		}
		{
		  ::glPushMatrix();

		  //����������ƶ�����
		  RenderScene();		// ���I�ȕ��i�������ŕ`��

		  ::glPopMatrix();
		}
	
	glPopMatrix();

	::glFinish();		// ���ׂĂ�OpenGL�R�[���������������Ƃ�m�点��

	if( FALSE == ::SwapBuffers( m_pDC->GetSafeHdc() ) )
		SetError(7);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView �N���X�̐f�f

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenGLDoc* COpenGLView::GetDocument() // ��f�o�b�O �o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLDoc)));
	return (COpenGLDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLView �N���X�̃��b�Z�[�W �n���h��

int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: ���̈ʒu�ɌŗL�̍쐬�p�R�[�h��ǉ����Ă�������
	
	InitalizeOpenGL();

	return 0;
}

void COpenGLView::OnDestroy() 
{
	CView::OnDestroy();
	
	KillTimer(1);

	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	if( FALSE == ::wglMakeCurrent(0, 0) )
		SetError(2);
	if( FALSE == ::wglDeleteContext( m_hRC ) )
		SetError(6);

	if( m_pDC )
		delete m_pDC;
	
}

BOOL COpenGLView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	
	// comment out original call
	//return CView::OnEraseBkgnd(pDC);
	return TRUE; // tell Windows not to erase the background
}

void COpenGLView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������

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
//�����Ӽ��ռ��ƽ��ͷ��
BOOL COpenGLView::SetupViewingFrustum(GLdouble aspect_ratio)
{
	// select a default viewing volume
	gluPerspective( 40.0f, aspect_ratio, 0.1f, 20.0f );

	return TRUE;
}

BOOL COpenGLView::SetupViewingTransform()
{
	int i;

	GLfloat fovy = 30.0; // ��p//����
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

	// ������x�N�g���̐ݒ�
	//���ϵķ����������趨
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
	// �Ɩ��̐ݒ�
	//���ù���
	GLfloat model_ambient[] = { 2.0f, 2.0f, 2.0f, 1.0f };
	GLfloat light_position0[]= { 1.0f, 0.0f, 5.0f, 0.0f };
	GLfloat light_color0[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// ����
	//������
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, model_ambient );

	// ���C�g�̈ʒu
	//��Դλ��
	glLightfv( GL_LIGHT0, GL_POSITION, light_position0 );

	// ���C�g�̐F
	//��Դ��ɫ
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light_color0 );

	// ���� ON
	//�򿪹�Դ
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	return TRUE;
}

// ���I�ȏ�ʂ�`��
//�ƶ����������
BOOL COpenGLView::RenderScene()
{
	// �Ԃ��{�[���Ɛ������̂�`�� //��ɫ����ɫ����������
    //������(1,0,0)��ת90��
	::glRotatef( 90.0f, 1.0f, 0.0f, 0.0f);

	//���õ�ǰ��ɫΪ��ɫ
	::glColor3f( 1.0f, 0.0f, 0.0f);
	//
	::auxWireSphere(0.5);
	::glColor3f( 0.5f, 0.5f, 1.0f );
	::auxWireCube( 1.0 );

	return TRUE;
}

// �ÓI�ȏ�ʂ�`��
//��ֹ���������
BOOL COpenGLView::RenderStockScene()
{
	// �`�F�b�J�[�t���b�O��`��
	return TRUE;
}


void COpenGLView::OnAnimationStart() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������

	// �A�j���[�V�����p�̃^�C�}���Z�b�g
	//���ö���ʱ��
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	
	// �^�C�}���폜
	KillTimer(1);//The KillTimer function destroys the specified timer. 
	//ɾ����ʱ��1
}

void COpenGLView::OnTimer(UINT nIDEvent) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������

	CView::OnTimer(nIDEvent);
}
//����*p0,*p1,*p2���㷨����*normal
void COpenGLView::CalcNormal(GLfloat *p0, GLfloat *p1, GLfloat *p2, GLfloat *normal)
{

	int i;
	GLfloat norm;
	GLfloat v0[3], v1[3];

	// �Q�{�̃x�N�g�������
	for(i=0; i<3; i++){
		v0[i] = p2[i] - p1[i];
		v1[i] = p0[i] - p1[i];
	}

	// �I��ꂽ�x�N�g���̊O�ς����߂�
	normal[0] = v0[1] * v1[2] - v0[2] * v1[1];
	normal[1] = v0[2] * v1[0] - v0[0] * v1[2];
	normal[2] = v0[0] * v1[1] - v0[1] * v1[0];
	// �@���x�N�g���͕K�����K�����Ă���
	//���ߵ����滯��Ҳ���ǵ�λ��
	norm = (GLfloat)sqrt( (double)(normal[0] * normal[0]
									+ normal[1] * normal[1]
									+ normal[2] * normal[2]) );
	for(i=0; i<3; i++)
		normal[i] /= norm;

}


#define NDIV_MAX	30
// �� n_div �p����`���֐�   //���������������
// �O���[�V�F�[�f�B���O��p���ĉ~����`��
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

	// �V��ƒ�̐� n_div �p�`���`
	//���������������n_div�����
	for(i=0; i<n_div; i++){
		angle = (2.0 * PI * i) / n_div;
		x = (GLfloat)cos(angle);
		y = (GLfloat)sin(angle);

		// �㕔�̐����p�`�̒��_���ォ�猩�Ĕ����v����ɒ�`
		//�ϲ�������ζ������ʱ�붨��
		top_poly[i][0] = (GLfloat)radius * x;
		top_poly[i][1] = (GLfloat)radius * y;
		top_poly[i][2] = (GLfloat)height;

		// �����̐����p�`�̒��_�������猩�Ĕ����v����ɒ�`
		//�²�������ζ������ʱ�붨��
		bottom_poly[i][0] = (GLfloat)radius * x;
		bottom_poly[i][1] = (GLfloat)radius * y;
		bottom_poly[i][2] = 0.0;
	}

	// �㕔�̐����p�`�̕`��
	//�ϲ�������ε�����
	glBegin(GL_POLYGON);

		// �K���ʂ̖@����^����
	    //�����ñ���ķ�������
		CalcNormal(top_poly[0], top_poly[1], top_poly[2], nv );
		glNormal3fv(nv);

		// ���_�̍��W�𔽎��v����Ɏw������
		//����ʱ�뷽���ʾ������������
		for(i=0; i<n_div; i++)
			glVertex3fv(top_poly[i]);
	glEnd();

	// ���ʂ̕`��B �l�ӌ`�̘A���Ƃ��Ĉ���
	//�������棬�ı��ε���������
	glBegin(GL_QUAD_STRIP);
	    for(i=0; i<=n_div; i++){
			nv[0] = top_poly[i % n_div][0] / (GLfloat)radius;//ע�������%�����ú�����
			nv[1] = top_poly[i % n_div][1] / (GLfloat)radius;
			nv[2] = 0.0;
			glNormal3fv(nv);
			glVertex3fv(bottom_poly[i % n_div]);
			glVertex3fv(top_poly[i % n_div]);
		}
	glEnd();

	// �����̐����p�`�̕`��
	//�²�������������
	glBegin(GL_POLYGON);

		// �K���ʂ̖@����^����
	   //�����ñ���ķ�������
		CalcNormal(bottom_poly[0], bottom_poly[1], bottom_poly[2], nv );
		glNormal3fv(nv);

		for(i=(n_div-1); i>=0; i--)
			glVertex3fv(bottom_poly[i]);
	glEnd();
}

// ���݂̍��W�n��`�����߂̃X�C�b�`
//������������־λ
void COpenGLView::OnMenuCoordinateOn() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	showCoordinate = TRUE;
	Invalidate();
}
//�ر���������λ
void COpenGLView::OnMenuCoordinateOff() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	showCoordinate = FALSE;
	Invalidate();
}

// ���W�n��`��
//����ϵ�����
void COpenGLView::drawCoordinate(double size)
{
	GLdouble ovec[] = { 0.0, 0.0, 0.0 };
	GLdouble xvec[] = { size, 0.0, 0.0 };
	GLdouble yvec[] = { 0.0, size, 0.0 };
	GLdouble zvec[] = { 0.0, 0.0, size };

	GLdouble radius = size / 30; //��ͷ�뾶
	GLdouble height = size / 15; //��ͷ����

	// �����v���X�e�b�B�N
    //��ɫ�ġ�����

	GLfloat white_ambient[]  = { 0.5f, 0.5f, 0.5f }; // ���F//����ɫ
	GLfloat white_diffuse[]  = { 0.5f, 0.5f, 0.5f }; // �g�U�F//��ɢɫ
	GLfloat white_specular[] = { 0.5f, 0.6f, 0.6f }; // ���ʐF//����ɫ
	GLfloat white_shininess[]= { 32.0f };			// ���˂̋���//����ǿ��

	glEnable(GL_COLOR_MATERIAL);
	

	// ��ʾ��������
	if ( showCoordinate == TRUE )
	{
		// ���̂̍ގ��̒�` : ��
	   //����Ĳ��ʶ��壺��
		glMaterialfv( GL_FRONT, GL_AMBIENT, white_ambient);
		glMaterialfv( GL_FRONT, GL_DIFFUSE, white_diffuse );
		glMaterialfv( GL_FRONT, GL_SPECULAR, white_specular);
		glMaterialfv( GL_FRONT, GL_SHININESS, white_shininess );	
       
		/*************************************************
		RGB: 238 210 160
		���㷽����
		238 / 255 = 0.93
		210 / 255 = 0.82
		160 / 255 = 0.63
		���� glColor3f(0.93f , 0.82f , 0.63f)�Ͳ���� 
		**************************************************/
		//*
		// x ��    //x��
		glColor3f(1.0f, 0.0f, 0.0f);  // ��ɫRGB(255, 0, 0)
		glBegin( GL_LINES );
			glVertex3dv( ovec );
			glVertex3dv( xvec );
		glEnd();
		glPushMatrix();
			glTranslated( size, 0.0, 0.0 );
			glRotated( 90.0, 0.0, 1.0, 0.0 );
			glutSolidCone( radius, height,10,10 );
		glPopMatrix();

		// y ��   /y��
		glColor3f(0.0f, 1.0f, 0.0f);   // ��ɫRGB(0, 255, 0)
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

		// z ��    //z��
		glColor3f(0.0f, 0.0f, 1.0f);  // ��ɫRGB(0, 0, 255)
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
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	
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