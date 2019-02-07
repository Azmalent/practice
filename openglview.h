#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

namespace Ui
{
    class OpenGLView;
}

class OpenGLView : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT

    public:
        explicit OpenGLView(QWidget *parent = nullptr);

    protected:
        void initializeGL() override;
        void resizeGL(int nWidth, int nHeight) override;
        void paintGL() override;
};

#endif
