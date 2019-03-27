#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include <GL/gl.h>

#include "modelimporter.h"

#define ROOM_HEIGHT 0.5

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

        void mousePressEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;
        void wheelEvent(QWheelEvent* e) override;

    private:
        void createShaders();
        void createGeometry(QVector<float>* vertices, QVector<float>* normals, QVector<uint>* indices);
        void addShader(QString filename, QOpenGLShader::ShaderTypeBit type);
        void drawModel();
        void drawNode(const QMatrix4x4& model, const Node* node, QMatrix4x4 parent);
        void zoom(int n);

        ModelImporter importer;
        Node* root;
        QMatrix4x4 projection;
        QMatrix4x4 view;
        QQuaternion rotation;
        QOpenGLVertexArrayObject vertexArray;
        QOpenGLBuffer vertexBuffer;
        QOpenGLBuffer normalBuffer;
        QOpenGLBuffer indexBuffer;
        QOpenGLShaderProgram shaders;

        QVector2D oldMousePos, newMousePos;
        float scale;

        int indexCount;

    private slots:
        void updateColor(GLint r, GLint g, GLint b);
        void updateGeometry(QVector<QPointF> basePoints, QVector<int> baseIndices);
        //void exportGeometry(); //TODO
};

#endif
