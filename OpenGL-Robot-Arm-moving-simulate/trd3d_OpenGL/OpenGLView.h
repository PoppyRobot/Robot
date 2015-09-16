// OpenGLView.h : COpenGLView 僋儔僗偺愰尵偍傛傃僀儞僞乕僼僃僀僗偺掕媊傪偟傑偡丅
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
protected: // 僔儕傾儔僀僘婡擻偺傒偐傜嶌惉偟傑偡丅
	COpenGLView();
	DECLARE_DYNCREATE(COpenGLView)

// 傾僩儕價儏乕僩
public:
	COpenGLDoc* GetDocument();


// 僆儁儗乕僔儑儞operation
public:

// 僆乕僶乕儔僀僪overriding
	// ClassWizard 偼壖憐娭悢偺僆乕僶乕儔僀僪傪惗惉偟傑偡丅
	//overriding of the virtual class function of classwizard
	//{{AFX_VIRTUAL(COpenGLView)
	public:
	virtual void OnDraw(CDC* pDC);  // 偙偺價儏乕傪昤夋偡傞嵺偵僆乕僶乕儔僀僪偝傟傑偡丅
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// 僀儞僾儕儊儞僥乕僔儑儞
public:
	virtual ~COpenGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 惗惉偝傟偨儊僢僙乕僕 儅僢僾娭悢
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
	virtual BOOL SetupViewingFrustum( GLdouble );//设置视图空间的平截头体
	virtual BOOL SetupViewingTransform( void );	
	virtual BOOL SetupLighting( void );
	virtual BOOL PreRenderScene( void ){ return TRUE; }
	virtual BOOL RenderScene( void );
	virtual BOOL RenderStockScene();

public:
	float rotZ;
	float rotX;
	void drawCoordinate(double size);//画坐标轴
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

#ifndef _DEBUG  // OpenGLView.cpp 僼傽僀儖偑僨僶僢僌娐嫬偺帪巊梡偝傟傑偡丅
inline COpenGLDoc* COpenGLView::GetDocument()
   { return (COpenGLDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ 偼慜峴偺捈慜偵捛壛偺愰尵傪憓擖偟傑偡丅

#endif // !defined(AFX_OPENGLVIEW_H__BEF63AEE_123D_11D4_A3B2_00E0293527C9__INCLUDED_)
