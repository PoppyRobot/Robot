/////////////////////////////////////////////////////////
// CMyView.h

#if !defined ___MYVIEW_H___
#define  ___MYVIEW_H___


#include "stdAfx.h"
#include "OpenGLView.h"

class CMyView : public COpenGLView
{

protected:
	CMyView();	
	~CMyView(); 

private:
	DECLARE_DYNCREATE(CMyView)

protected:
	virtual BOOL RenderScene();
	virtual BOOL RenderStockScene(void);

private:
	void drawArm();
	void drawTrajectory(double *pInitial, double *pTarget);
	void drawFloor(double floorSize);


	// ClassWizard 
	//{{AFX_VIRTUAL(CMyView)
	public:
	protected:
	//}}AFX_VIRTUAL


protected:
	//{{AFX_MSG(CMyView)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif
