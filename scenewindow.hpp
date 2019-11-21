#ifndef SCENEWINDOW_HPP
#define SCENEWINDOW_HPP

#include <QWidget>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <vector>
#include <QOpenGLShaderProgram>
#include <QWidget>
#include <QOpenGLWidget>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLFunctions>
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>

#include <QtOpenGL/QtOpenGL>

#include <QtOpenGL/QGLWidget>

#include <QWidget>

class scenewindow:  public QGLWidget
{
        Q_OBJECT
        public:
          explicit scenewindow( QWidget* parent = 0, const char* name = 0, bool fs = false );\

           ~scenewindow();

        private:

        protected:
    void initializeGL();

        void paintGL();

        void resizeGL( int width, int height );

        void keyPressEvent(QKeyEvent *);
        bool fullScreen;
  };

#endif // SCENEWINDOW_HPP
