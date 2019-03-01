#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include <QPainter>
#include <QWidget>

#define WIDGET_SIZE 500
#define CELL_SIZE 20

#define POINT_SIZE 5

namespace Ui
{
    class RoomEditor;
}

class RoomEditor : public QWidget
{
    Q_OBJECT

    public:
        RoomEditor(QWidget* parent);

    protected:
        void mousePressEvent(QMouseEvent* e) override;
        void paintEvent(QPaintEvent *e) override;

    private:
        QPointF getDotPosition(QPointF clickPos);

        QPainter painter;
        QBrush backgroundBrush, dotFillBrush, polyFillBrush;
        QPen gridPen, polyPen;
        QVector<QPointF> points;
};

#endif // ROOMEDITOR_H
