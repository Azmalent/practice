#include "openglview.h"
#include "mainwindow.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>

OpenGLView::OpenGLView(QWidget* parent) : QOpenGLWidget(parent)
{
    QObject* window = MainWindow::instance();
    QObject::connect(window, SIGNAL( updateColorSignal(GLint, GLint, GLint) ),
                     this, SLOT( updateColor(GLint, GLint, GLint) ));

    r = 1.0f;
    g = 1.0f;
    b = 1.0f;

    vertexBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    normalBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    indexBuffer  = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
}

void OpenGLView::initializeGL()
{
    initializeOpenGLFunctions();

    createShaders();
    createGeometry();

    view.setToIdentity();
    view.lookAt(QVector3D(0.0f, 0.0f, 1.2f),    // позиция камеры
                QVector3D(0.0f, 0.0f, 0.0f),    // точка, куда направлена камера
                QVector3D(0.0f, 1.0f, 0.0f));   // направление вверх


    glClearColor(r, g, b, 1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);                  // глубина проверки пикселей
    glEnable(GL_CULL_FACE);                   // говорим, что будем строить только внешние поверхности
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); // фигуры будут закрашены с обеих сторон
}

void OpenGLView::resizeGL(int width, int height)
{
    glViewport(0, 0, height, height); // установка точки обзора
    glMatrixMode(GL_PROJECTION);      // установка режима матрицы
    glLoadIdentity();                 // загрузка единичной матрицы

    projection.setToIdentity();
    projection.perspective(60.0f, (float)width/height, 0.3f, 1000);
}

void OpenGLView::paintGL()
{
   glClearColor(r, g, b, 1.0f);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очистка экрана
   glMatrixMode(GL_MODELVIEW); // задаем модельно-видовую матрицу
   glLoadIdentity();           // загружаем единичную матрицу

   shaders.bind();
   vertexArray.bind();
   drawModel();
   vertexArray.release();
   update();
}

void OpenGLView::createShaders() {
    addShader(":/shaders/vertex.vert",   QOpenGLShader::Vertex);
    addShader(":/shaders/fragment.frag", QOpenGLShader::Fragment);
    if(!shaders.link() ) {
        qDebug() << "Error linking shader program: " << shaders.log();
        exit(1);
    }

    shaders.bind();
    shaders.setUniformValue("u_light.position",   QVector4D(-1.0f,  1.0f, 1.0f, 1.0f));
    shaders.setUniformValue("u_light.intensity",  QVector3D( 1.0f,  1.0f, 1.0f));
}

void OpenGLView::createGeometry()
{
    if(!importer.Load("C:/Users/Dmitry/Desktop/ink.stl"))
    {
        qDebug() << "Error loading model: " << shaders.log();
        exit(1);
    }

    QVector<float>* vertices;
    QVector<float>* normals;
    QVector<uint>*  indices;

    importer.getBufferData(&vertices, &normals, &indices);

    vertexArray.create();
    vertexArray.bind();

    vertexBuffer.create();
    vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    vertexBuffer.bind();
    vertexBuffer.allocate(&(*vertices)[0], vertices->size() * sizeof((*vertices)[0]));

    shaders.enableAttributeArray(0);
    shaders.setAttributeBuffer(0, GL_FLOAT, 0, 3);

    normalBuffer.create();
    normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    normalBuffer.bind();
    normalBuffer.allocate(&(*normals)[0], normals->size() * sizeof((*normals)[0]));

    shaders.enableAttributeArray(1);
    shaders.setAttributeBuffer(1, GL_FLOAT, 0, 3);

    indexBuffer.create();
    indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    indexBuffer.bind();
    indexBuffer.allocate(&(*indices)[0], indices->size() * sizeof((*indices)[0]));

    vertexArray.release();
}

void OpenGLView::addShader(QString filename, QOpenGLShader::ShaderTypeBit type)
{
    QFile file(filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error opening file " << filename;
        exit(1);
    }
    QTextStream in(&file);
    QString shader = in.readAll();
    if(!shaders.addShaderFromSourceCode(type, shader)) {
        qDebug() << "Error in shader '" << filename << "': " << shaders.log();
        exit(1);
    }
}

void OpenGLView::drawModel()
{
    QMatrix4x4 model;
    model.translate(-0.2f, 0.0f, 0.5f);
    model.rotate(55.0f, 0.0f, 1.0f, 0.0f);

    drawNode(model, importer.getRoot().data(), QMatrix4x4());
}

void OpenGLView::drawNode(const QMatrix4x4& model, const Node* node, QMatrix4x4 parent)
{
    QMatrix4x4 local = parent * node->transformMatrix;
    QMatrix4x4 mv = view * model * local;

    shaders.setUniformValue("u_MV",  mv);
    shaders.setUniformValue("u_N",   mv.normalMatrix());
    shaders.setUniformValue("u_MVP", projection * mv);

    // Отрисовка мешей
    for(int i = 0; i < node->meshes.size(); i++)
    {
        const Mesh& m = *node->meshes[i];

        if (m.material->name != QString("DefaultMaterial")) {
            shaders.setUniformValue("u_material.ambient",   m.material->ambient);
            shaders.setUniformValue("u_material.diffuse",   m.material->diffuse);
            shaders.setUniformValue("u_material.specular",  m.material->specular);
            shaders.setUniformValue("u_material.shine",     m.material->shine);
        } else {
            shaders.setUniformValue("u_material.ambient",   QVector3D(0.05f, 0.2f, 0.05f));
            shaders.setUniformValue("u_material.diffuse",   QVector3D(0.3f,  0.5f, 0.3f));
            shaders.setUniformValue("u_material.specular",  QVector3D(0.6f,  0.6f, 0.6f));
            shaders.setUniformValue("u_material.shine",     50.f);
        }

        const GLvoid* indices = reinterpret_cast<const GLvoid*>(m.indexOffset * sizeof(GLuint));
        glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, indices);
    }

    // Отрисовка дочерних узлов
    for(int i = 0; i < node->children.size(); i++)
    {
        drawNode(model, &node->children[i], local);
    }
}

void OpenGLView::updateColor(GLint ir, GLint ig, GLint ib)
{
    r = static_cast<GLclampf>(ir) / 255;
    g = static_cast<GLclampf>(ig) / 255;
    b = static_cast<GLclampf>(ib) / 255;

    repaint();
}

