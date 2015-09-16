; CLW ファイルは MFC ClassWizard の情報を含んでいます。

[General Info]
Version=1
LastClass=COpenGLView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "OpenGL.h"
LastPage=0

ClassCount=6
Class1=COpenGLApp
Class2=COpenGLDoc
Class3=COpenGLView
Class4=CMainFrame
Class5=CMyView

ResourceCount=4
Resource1=IDR_MAINFRAME
Class6=CAboutDlg
Resource2=IDD_ABOUTBOX
Resource3=IDR_MAINFRAME (Japanese)
Resource4=IDD_ABOUTBOX (Japanese)

[CLS:COpenGLApp]
Type=0
HeaderFile=OpenGL.h
ImplementationFile=OpenGL.cpp
Filter=N

[CLS:COpenGLDoc]
Type=0
HeaderFile=OpenGLDoc.h
ImplementationFile=OpenGLDoc.cpp
Filter=N

[CLS:COpenGLView]
Type=0
HeaderFile=OpenGLView.h
ImplementationFile=OpenGLView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=COpenGLView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=OpenGL.cpp
ImplementationFile=OpenGL.cpp
Filter=D
LastObject=CAboutDlg
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=10
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_ANIMATION_START
Command3=ID_ANIMATION_STOP
Command4=ID_MENU_COORDINATE_ON
Command5=ID_MENU_COORDINATE_OFF
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[CLS:CMyView]
Type=0
HeaderFile=myview.h
ImplementationFile=myview.cpp
BaseClass=COpenGLView
LastObject=ID_MENU_COORDINATE_ON
Filter=C
VirtualFilter=VWC

[DLG:IDD_ABOUTBOX (Japanese)]
Type=1
Class=?
ControlCount=10
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME (Japanese)]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_ANIMATION_START
Command3=ID_ANIMATION_STOP
Command4=ID_MENU_COORDINATE_ON
Command5=ID_MENU_COORDINATE_OFF
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME (Japanese)]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

