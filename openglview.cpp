#include "openglview.h"
#include "mainwindow.h"

#include <QMessageBox>

OpenGLView::OpenGLView(QWidget* parent) : QOpenGLWidget(parent)
{
    QObject* window = MainWindow::instance();
    QObject::connect(window, SIGNAL( updateColorSignal(GLint, GLint, GLint) ),
                     this, SLOT( updateColor(GLint, GLint, GLint) ));

    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
}

void OpenGLView::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(r, g, b, 1.0f);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);    // задаем глубину проверки пикселей
    glEnable(GL_CULL_FACE);     // говорим, что будем строить только внешние поверхности
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); // фигуры будут закрашены с обеих сторон
}

void OpenGLView::resizeGL(int width, int height)
{
    glViewport(0, 0, height, height); // установка точки обзора
    glMatrixMode(GL_PROJECTION);      // установка режима матрицы
    glLoadIdentity();                 // загрузка матрицы
}

void OpenGLView::paintGL()
{  
   glClearColor(r, g, b, 1.0f);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очистка экрана
   glMatrixMode(GL_MODELVIEW); // задаем модельно-видовую матрицу
   glLoadIdentity();           // загружаем единичную матрицу

   glColor3f(1.0f, 0, 0);
   glBegin(GL_QUADS);    // говорим, что рисовать будем прямоугольник

   // задаем вершины многоугольника
   glVertex3f(0.5, 0.5, 0.5);
   glVertex3f(-0.5, 0.5, 0.5);
   glVertex3f(-0.5, -0.5, 0.5);
   glVertex3f(0.5, -0.5, 0.5);
   glEnd();
}

void OpenGLView::updateColor(GLint ir, GLint ig, GLint ib)
{
    r = static_cast<GLclampf>(ir) / 255;
    g = static_cast<GLclampf>(ig) / 255;
    b = static_cast<GLclampf>(ib) / 255;

    repaint();
}
