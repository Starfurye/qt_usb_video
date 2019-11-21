#include "scenewindow.hpp"
#include <iostream>
#include <QtGui/QMouseEvent>
#include <GL/GLU.h>
#include "scenewindow.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLFunctions>
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>

#include <QtOpenGL/QtOpenGL>

#include <QtOpenGL/QGLWidget>

#include <QWidget>


scenewindow::scenewindow( QWidget* parent, const char* name, bool fs )

    :QGLWidget( parent )

{
    fullScreen = fs;     //设置窗口的位置，即左上角为(0,0)点，大小为640*480。
    setGeometry( 0, 0, 640, 480 );     // 设置窗口的标题为“My's OpenGL Framework”。
    //setCaption( "My's OpenGL Framework" );     // 如果fullscreen为真，那么就全屏显示这个窗口。
    if ( fullScreen )
    {
        showFullScreen();
    }

}



scenewindow::~scenewindow()
{

}



void scenewindow::initializeGL()

{
    glShadeModel(GL_SMOOTH);
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    return;

}



void scenewindow::paintGL()

{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glTranslatef(-1.5,0.0,-6.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0,1.0,0.0);
    glVertex3f(-1.0,-1.0,0.0);
    glVertex3f(1.0,-1.0,0.0);
    glEnd();
    return;


}



void scenewindow::resizeGL(int width, int height)

{

    if( height == 0 )
    {        height =1;    }    //重置当前的视口（Viewport）
    glViewport( 0, 0, (GLint)width, (GLint)height );     //选择投影矩阵。
    glMatrixMode( GL_PROJECTION );     //重置投影矩阵
    glLoadIdentity();     //建立透视投影矩阵
    gluPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0 );    //
    //gluPerspective(45.0,(GLfloat)width()/(GLfloat)height(),0.1,100.0);    // 选择模型观察矩阵
    glMatrixMode( GL_MODELVIEW );     // 重置模型观察矩阵。
    glLoadIdentity();
    return;

}

void scenewindow::keyPressEvent(QKeyEvent * e)
{    switch ( e->key() )
    {        //如果按下了F2键，那么屏幕是否全屏的状态就切换一次。然后再根据需要，显示所要的全屏窗口或者 普通窗口。
    case Qt::Key_F2:  fullScreen = !fullScreen;

         if ( fullScreen )
         {
             showFullScreen();
         }
         else
         {
             showNormal();
             setGeometry( 0, 0, 640, 480 );
         }
         updateGL();
        break;         //如果按下了Escape键，程序退出。
    case Qt::Key_Escape:
        close();
    }
     return;
}


