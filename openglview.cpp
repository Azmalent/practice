#include "openglview.h"
#include "mainwindow.h"

#include <algorithm>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWheelEvent>

#define UNITS_PER_DEGREE 8
#define DEGREES_PER_STEP 15

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

    scale = 1.0;
}

void OpenGLView::initializeGL()
{
    initializeOpenGLFunctions();

    createShaders();

    if(!importer.Load("C:/Users/Dmitry/Desktop/ink.obj"))
    {
        qDebug() << "Error loading model: " << shaders.log();
        exit(1);
    }

    QVector<float>* vertices;
    QVector<float>* normals;
    QVector<uint>*  indices;
    importer.getBufferData(&vertices, &normals, &indices);
    createGeometry(vertices, normals, indices);
    root = importer.getRoot().data();

    glClearColor(r, g, b, 1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);                  // глубина проверки пикселей
    glEnable(GL_CULL_FACE);                   // будем строить только внешние поверхности
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

void OpenGLView::createGeometry(QVector<float>* vertices, QVector<float>* normals, QVector<uint>* indices)
{
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
    view.setToIdentity();
    view.lookAt(QVector3D(0.0f, 0.0f, 1.2f),  // позиция камеры
                QVector3D(0.0f, 0.0f, 0.0f),  // центр
                QVector3D(0.0f, 1.0f, 0.0f)); // направление вверх

    view.rotate(rotation);
    view.scale(scale);

    QMatrix4x4 model;
    drawNode(model, root, QMatrix4x4());
}

void OpenGLView::drawNode(const QMatrix4x4& model, const Node* node, QMatrix4x4 parent)
{
    QMatrix4x4 local = parent * node->transformMatrix;
    QMatrix4x4 modelview = view * model * local;

    shaders.setUniformValue("u_MV",  modelview);
    shaders.setUniformValue("u_N",   modelview.normalMatrix());
    shaders.setUniformValue("u_MVP", projection * modelview);

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

void OpenGLView::mousePressEvent(QMouseEvent* e)
{
    if(e->buttons() == Qt::LeftButton) {
        newMousePos = QVector2D(e->localPos());
        oldMousePos = newMousePos;
    }

    e->accept();
}

void OpenGLView::mouseMoveEvent(QMouseEvent* e)
{
    if(e->buttons() == Qt::LeftButton) {
        newMousePos = QVector2D(e->localPos());

        QVector2D delta = newMousePos - oldMousePos;

        float angle = delta.length() / 2.0;
        QVector3D axis = QVector3D(delta.y(), delta.x(), 0.0);

        rotation = QQuaternion::fromAxisAndAngle(axis, angle) * rotation;

        oldMousePos = newMousePos;
    }

    e->accept();
}

void OpenGLView::wheelEvent(QWheelEvent* e)
{
    QPoint units = e->angleDelta();
    if(!units.isNull()) {
        QPoint steps = units / UNITS_PER_DEGREE / DEGREES_PER_STEP;
        int n = steps.y();
        zoom(n);
    }
}

void OpenGLView::zoom(int n)
{
    scale += n * 0.25f;
    scale = std::min(std::max(scale, 0.25f), 10.0f);
    repaint();
}

void OpenGLView::getModelFromVector(QVector<QPointF> points)
{
    QVector<float>* vertices;
    QVector<float>* normals;
    QVector<uint>*  indices;

    if(vertexArray.isCreated()) {
        vertexArray.destroy();
        vertexBuffer.destroy();
        normalBuffer.destroy();
        indexBuffer.destroy();
    }

    int n = points.size();

    //Преобразуем точки в вершины
    for(int i = 0; i < n; i++)
    {
        float x = (points[i].x() - 250) / 50;
        float y = (points[i].y() - 250) / 50;

        vertices->push_back(x);
        vertices->push_back(y);
        vertices->push_back(0);
    }

    //Потолок
    //Каждый третий элемент (координата Z) устанавливаем в высоту помещения, остальные координаты дублируем
    for(int i = 0; i < n; i++)
    {
        vertices->push_back(i%3 != 0 ? ROOM_HEIGHT : (*vertices)[i]);
    }

    //TODO...
}
