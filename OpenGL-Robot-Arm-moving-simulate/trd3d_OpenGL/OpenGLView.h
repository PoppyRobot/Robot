// OpenGLView.h : COpenGLView クラスの宣言およびインターフェイスの定義をします。
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
protected: // シリアライズ機能のみから作成します。
	COpenGLView();
	DECLARE_DYNCREATE(COpenGLView)

// アトリビュート
public:
	COpenGLDoc* GetDocument();


// オペレーションoperation
public:

// オーバーライドoverriding
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//overriding of the virtual class function of classwizard
	//{{AFX_VIRTUAL(COpenGLView)
	public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~COpenGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
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
	virtual BOOL SetupViewingFrustum( GLdouble );//ﾉ靹ﾃﾊﾓﾍｼｿﾕｼ莊ﾄﾆｽｽﾘﾍｷﾌ�
	virtual BOOL SetupViewingTransform( void );	
	virtual BOOL SetupLighting( void );
	virtual BOOL PreRenderScene( void ){ return TRUE; }
	virtual BOOL RenderScene( void );
	virtual BOOL RenderStockScene();

public:
	float rotZ;
	float rotX;
	void drawCoordinate(double size);//ｻｭﾗ�ｱ�ﾖ�
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

#ifndef _DEBUG  // OpenGLView.cpp ファイルがデバッグ環境の時使用されます。
inline COpenGLDoc* COpenGLView::GetDocument()
   { return (COpenGLDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_OPENGLVIEW_H__BEF63AEE_123D_11D4_A3B2_00E0293527C9__INCLUDED_)
