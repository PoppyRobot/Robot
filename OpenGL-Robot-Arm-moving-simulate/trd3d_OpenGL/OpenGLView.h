// OpenGLView.h : COpenGLView �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVIEW_H__BEF63AEE_123D_11D4_A3B2_00E0293527C9__INCLUDED_)
#define AFX_OPENGLVIEW_H__BEF63AEE_123D_11D4_A3B2_00E0293527C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Include the OpenGL headers
#include "gl\gl.h"
#include "gl\glu.h"
#include "gl\glaux.h"

class COpenGLView : public CView
{
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	COpenGLView();
	DECLARE_DYNCREATE(COpenGLView)

// �A�g���r���[�g
public:
	COpenGLDoc* GetDocument();


// �I�y���[�V����operation
public:

// �I�[�o�[���C�hoverriding
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//overriding of the virtual class function of classwizard
	//{{AFX_VIRTUAL(COpenGLView)
	public:
	virtual void OnDraw(CDC* pDC);  // ���̃r���[��`�悷��ۂɃI�[�o�[���C�h����܂��B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~COpenGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(COpenGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAnimationStart();
	afx_msg void OnAnimationStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenuCoordinateOn();
	afx_msg void OnMenuCoordinateOff();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateMenuCoordinateOff(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL SetupPixelFormat( void );
	virtual BOOL SetupViewport( int cx, int cy );
	virtual BOOL SetupViewingFrustum( GLdouble );//������ͼ�ռ��ƽ��ͷ��
	virtual BOOL SetupViewingTransform( void );	
	virtual BOOL SetupLighting( void );
	virtual BOOL PreRenderScene( void ){ return TRUE; }
	virtual BOOL RenderScene( void );
	virtual BOOL RenderStockScene();

public:
	float rotZ;
	float rotX;
	void drawCoordinate(double size);//��������
	void CalcNormal(GLfloat *p0, GLfloat *p1, GLfloat *p2, GLfloat *normal);
	void mySolidCylinder(GLdouble radius, GLdouble height, int n_div);

	void Up();
	void Left();
	/*
	void Down();
	void Left();
	void Right();
	*/

	void ProcessCmd(char *buffer);

private:
	BOOL showCoordinate;
	void SetError( int e );
	BOOL InitalizeOpenGL();
	
	HGLRC m_hRC;
	CDC*  m_pDC;

	static const char* const _ErrorStrings[];
	const char* m_ErrorStrings;

	// socket
private:
	UINT m_nPort;
};

#ifndef _DEBUG  // OpenGLView.cpp �t�@�C�����f�o�b�O���̎��g�p����܂��B
inline COpenGLDoc* COpenGLView::GetDocument()
   { return (COpenGLDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPENGLVIEW_H__BEF63AEE_123D_11D4_A3B2_00E0293527C9__INCLUDED_)
