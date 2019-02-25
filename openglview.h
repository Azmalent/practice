#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include <GL/gl.h>

#include "modelimporter.h"

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

    private:
        void createGeometry();
        void createShaders(QString vertexFilename, QString fragmentFilename);
        void addShader(QString filename, QOpenGLShader::ShaderTypeBit type);
        void drawModel();
        void drawNode(const QMatrix4x4& model, const Node* node, QMatrix4x4 parent);

        //TODO: shaders
        ModelImporter importer;
        QMatrix4x4 projection;
        QMatrix4x4 view;
        QOpenGLVertexArrayObject vertexArray;
        QOpenGLBuffer vertexBuffer;
        QOpenGLBuffer normalBuffer;
        QOpenGLBuffer indexBuffer;
        QOpenGLShaderProgram shaders;

    private slots:
        void updateColor(GLint r, GLint g, GLint b);
};

#endif
