#include "roomeditor.h"

#include <cmath>
#include <algorithm>
#include <QVector2D>
#include <QMouseEvent>
#include <QDebug>

#define DIR_CLOCKWISE         1
#define DIR_COUNTERCLOCKWISE -1
#define DIR_NULL              0

RoomEditor::RoomEditor(QWidget* parent) : QWidget(parent)
{
    backgroundBrush = QBrush(QColor(255, 255, 255));
    dotFillBrush = QBrush(QColor(200, 75, 50));
    polyFillBrush = QBrush(QColor(255, 200, 200));
    gridPen = QPen(QColor(200, 200, 200), 1);
    polyPen = QPen(QColor(200, 75,  50), 1);
}

QVector<QPointF> RoomEditor::getPoints()
{
    return points;
}

void RoomEditor::mousePressEvent(QMouseEvent* e)
{
    QPointF pos = getDotPosition( e->localPos() );

    if(e->buttons() == Qt::MouseButton::LeftButton)
    {
        points.push_back(pos);
    }
    else if (e->buttons() == Qt::MouseButton::RightButton)
    {
        auto p = points.begin();
        while(p != points.end())
        {
            if(*p != pos) p++;
            else {
                p = points.erase(p);
                if(points.length() < 3) {
                    points.clear();
                    break;
                }
            }
        }
    }

    update();

    qDebug() << triangulate();
}

QPointF RoomEditor::getDotPosition(QPointF clickPos)
{
    QPointF pos = clickPos / CELL_SIZE + QPointF(0.5, 0.5);
    pos.setX( floor(pos.x()) );
    pos.setY( floor(pos.y()) );
    pos *= CELL_SIZE;

    pos.setX(std::max(pos.x(), static_cast<qreal>(CELL_SIZE)));
    pos.setX(std::min(pos.x(), static_cast<qreal>(WIDGET_SIZE - CELL_SIZE)));
    pos.setY(std::max(pos.y(), static_cast<qreal>(CELL_SIZE)));
    pos.setY(std::min(pos.y(), static_cast<qreal>(WIDGET_SIZE - CELL_SIZE)));

    return pos;
}

void RoomEditor::paintEvent(QPaintEvent* e)
{
    painter.begin(this);

    painter.setRenderHints(QPainter::RenderHint::Antialiasing);

    painter.setBackground(backgroundBrush);
    painter.setBackgroundMode(Qt::BGMode::OpaqueMode);
    painter.setPen(gridPen);

    //Рисование клеток
    int numCells = WIDGET_SIZE / CELL_SIZE;
    for(int i = 1; i < numCells; i++)
    {
        //Вертикальная линия
        QPoint a(CELL_SIZE*i, 0);
        QPoint b(CELL_SIZE*i, WIDGET_SIZE);
        painter.drawLine(a, b);

        //Горизонтальная линия
        QPoint c(0, CELL_SIZE*i);
        QPoint d(WIDGET_SIZE, CELL_SIZE*i);
        painter.drawLine(c, d);
    }

    painter.setPen(polyPen);

    painter.setBrush(dotFillBrush);
    painter.setBackgroundMode(Qt::BGMode::OpaqueMode);
    int n = points.size();
    for(int i = 0; i < n; i++)
    {
        float x = floor(points[i].x() - POINT_SIZE/2);
        float y = floor(points[i].y() - POINT_SIZE/2);
        painter.drawEllipse(x, y, POINT_SIZE, POINT_SIZE);
    }
    painter.setBrush(polyFillBrush);
    painter.drawPolygon(points.data(), points.size());

    painter.end();
}

//векторное произведение AB и AC
double RoomEditor::crossProduct(QPointF a, QPointF b, QPointF c)
{
    double x1 = b.x() - a.x(), x2 = c.x() - a.x();
    double y1 = b.y() - a.y(), y2 = c.y() - a.y();
    return x1 * y2 - x2 * y1;
}

int RoomEditor::walkDirection()
{
    int minIndex = 0;
    float minX = points[0].x(), minY = points[0].y();

    if(points.isEmpty()) return DIR_NULL;

    //поиск самой левой точки, если таких несколько - берём самую нижнюю
    int n = points.length();
    for (int i = 1; i < n; i++)
    {
        QPointF p = points[i];
        if(p.x() < minX)
        {
            minX = p.x();
            minIndex = i;
        }
        else if(p.x() == minX && p.y() < minY)
        {
            minY = p.y();
            minIndex = i;
        }
    }

    int last = n-1;
    int prev = (minIndex == 0) ? last : minIndex-1;
    int next = (minIndex == last) ? 0 : minIndex+1;

    //направление обхода зависит от знака векторного произведения между векторами AB и АС,
    //где А - минимальная вершина, а В и С - её соседи
    QPointF a = points[minIndex];
    QPointF b = points[next];
    QPointF c = points[prev];
    return (crossProduct(a, b, c) < 0) ? DIR_CLOCKWISE : DIR_COUNTERCLOCKWISE;
}

double RoomEditor::distance(QPointF a, QPointF b)
{
    QPointF ab = b - a;
    return std::sqrt( ab.x()*ab.x() + ab.y()*ab.y() );
}

double RoomEditor::triangleArea(QPointF a, QPointF b, QPointF c)
{
    double ab = distance(a, b), bc = distance(b, c), ca = distance(c, a);
    double p = (ab + bc + ca) / 2;
    return std::sqrt(p * (p-ab) * (p-bc) * (p-ca));
}

bool RoomEditor::hasPointsInTriangle(int ai, int bi, int ci)
{
    QPointF a = points[ai], b = points[bi], c = points[ci];
    double abc = triangleArea(a, b, c);

    for(int i = 0; i < points.length(); i++)
    {
        QPointF p = points[i];
        if (i == ai || i == bi || i == ci) continue;

        double pab = triangleArea(p,a,b), pbc = triangleArea(p,b,c), pca = triangleArea(p,c,a);
        double areaSum = pab+pbc+pca;

        if (std::abs(abc - areaSum) < 1.0e-5) return true;
    }
    return false;
}

//Возвращает массив индексов
QVector<int> RoomEditor::triangulate()
{
    int n = points.length();

    if(n < 3) return QVector<int>(0);

    bool* removed = new bool[n];
    for(int i = 0; i < n; i++) removed[i] = false;
    QVector<int> indices;

    int cur = 0, last = n-1;
    while(n > 3)
    {
        if(removed[cur]) continue;

        int prev = (cur == 0) ? last : cur-1;
        int next = (cur == last) ? 0 : cur+1;
        while(removed[prev]) prev = (prev == 0) ? last : prev-1;
        while(removed[next]) next = (next == last) ? 0 : next+1;

        int direction = walkDirection();
        double prod = crossProduct(points[prev], points[cur], points[next]);

        bool isEar;
        if(direction == DIR_CLOCKWISE && prod < 0 || direction == DIR_COUNTERCLOCKWISE && prod > 0)
        {
            isEar = !hasPointsInTriangle(prev, cur, next);
        }

        if(isEar)
        {
            indices.push_back(prev);
            indices.push_back(cur);
            indices.push_front(next);

            removed[cur] = true;
            n--;
        }

        cur = next;
    }

    //Добавляем оставшиеся 3 вершины
    for(int i = 0; i < points.length(); i++)
    {
        if(!removed[i]) indices.push_back(i);
    }
    delete[] removed;

    return indices;
}
