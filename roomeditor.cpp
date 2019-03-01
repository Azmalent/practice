#include "roomeditor.h"

#include <cmath>
#include <algorithm>
#include <QVector2D>
#include <QMouseEvent>
#include <QDebug>

RoomEditor::RoomEditor(QWidget* parent) : QWidget(parent)
{
    backgroundBrush = QBrush(QColor(255, 255, 255));
    dotFillBrush = QBrush(QColor(200, 75, 50));
    polyFillBrush = QBrush(QColor(255, 200, 200));
    gridPen = QPen(QColor(200, 200, 200), 1);
    polyPen = QPen(QColor(200, 75,  50), 1);
}

void RoomEditor::mousePressEvent(QMouseEvent* e)
{
    QPointF pos = getDotPosition( e->localPos() );

    if(e->buttons() == Qt::MouseButton::LeftButton)
    {
        points.push_front(pos);
    }
    else if (e->buttons() == Qt::MouseButton::RightButton)
    {
        auto p = points.begin();
        while(p != points.end())
        {
            if(*p != pos) p++;
            else p = points.erase(p);
        }
    }

    update();
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
