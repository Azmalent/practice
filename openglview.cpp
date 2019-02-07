#include "openglview.h"

OpenGLView::OpenGLView(QWidget *parent) : QOpenGLWidget(parent) { }

void OpenGLView::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);    // задаем глубину проверки пикселей
    glShadeModel(GL_FLAT);      // убираем режим сглаживания цветов
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
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очистка экрана
   glMatrixMode(GL_MODELVIEW); // задаем модельно-видовую матрицу
   glLoadIdentity();           // загрузка единичную матрицу

   glColor3i(0, 128, 0); // задаем цвет
   glBegin(GL_QUADS); // говорим, что рисовать будем прямоугольник

   // задаем вершины многоугольника
   glVertex3f(0.5, 0.5, 0.5);
   glVertex3f(-0.5, 0.5, 0.5);
   glVertex3f(-0.5, -0.5, 0.5);
   glVertex3f(0.5, -0.5, 0.5);
   glEnd();
}
