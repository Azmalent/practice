#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include <QWidget>

#define CELL_SIZE 10

class RoomEditor : public QWidget
{
    public:
        RoomEditor();

    protected:
        void mousePressEvent(QMouseEvent* event) override;

    private:
        QVector2D getDotPosition();
};

#endif // ROOMEDITOR_H
