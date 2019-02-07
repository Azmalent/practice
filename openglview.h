#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <GL/gl.h>

namespace Ui
{
    class OpenGLView;
}

class OpenGLView : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT

    public:
        explicit OpenGLView(QWidget *parent = nullptr);
        GLclampf r;
        GLclampf g;
        GLclampf b;

    protected:
        void initializeGL() override;
        void resizeGL(int width, int height) override;
        void paintGL() override;

    private slots:
        void updateColor(GLint r, GLint g, GLint b);
};

#endif
